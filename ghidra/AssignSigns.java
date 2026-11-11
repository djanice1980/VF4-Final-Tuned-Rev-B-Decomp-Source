// Signed or unsigned bytes and halfwords, from how the code extends them.
// SH-4 loads (mov.b, mov.w) sign-extend, so GCC 3.2 follows the load of an
// unsigned char or short with extu.b / extu.w as soon as the full value is
// needed, and a signed one needs nothing. Ghidra types the unnamed globals
// it finds as undefined1 / undefined2, which the export writes as unsigned:
// our compile then adds an extu the game does not have (36 near misses
// matched once such a global was read as signed).
//
// For every pool load of a data address, the register is followed through
// straight-line code (copies and add #imm, as in AssignAggregates.java); at
// each mov.b / mov.w load through it the loaded value is followed until it
// is overwritten, and votes:
//   unsigned  extu.b / extu.w of it (of the load's size)
//   signed    a use at full width without extending it first: arithmetic,
//             a compare with a register or cmp/pl / cmp/pz, a shift, a
//             32-bit store, an int-to-float move (lds ... fpul), a call
//             argument (r4-r7 at a jsr) or the return value (r0 at rts)
// Uses where the upper bits do not matter (tst, and / or / xor / cmp/eq
// with an immediate, a store of the same size) do not vote.
//
// An address with signed votes and no unsigned one becomes char / short,
// when its data is undefined there (or an undefined1 / undefined2), it is
// not a function, not inside an array (a member of an object AssignAggregates
// made is retyped there, when untyped), and only loaded at that
// size. A 4-byte value compared signed (cmp/gt, cmp/ge, cmp/pl, cmp/pz) and
// never unsigned becomes int (undefined4 exports as unsigned, and a signed
// test x < 1 then compiles as x == 0); it runs after AssignPointers and
// AssignFloats, which take theirs first (ghidra/signs_words.tsv).
// It keeps its DAT_<address> name (an analysis label: typed data
// would otherwise be named from its type and value, CHAR_00h_...), which
// ExportSource treats as unnamed like a default one. Writes
// ghidra/signs.tsv: every address with a vote.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.CharDataType;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataUtilities;
import ghidra.program.model.data.ShortDataType;
import ghidra.program.model.data.Undefined;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;

public class AssignSigns extends GhidraScript {

	static final long LO = 0x0C15DA00L, HI = 0x0E000000L; // data after .text (as AssignAggregates)

	// address -> {signed votes, unsigned votes, size seen (bit 1 = byte, 2 = halfword, 4 = other)}
	private final Map<Long, int[]> votes = new TreeMap<>();

	private static String mnem(Instruction i) {
		return i.getMnemonicString().replaceFirst("^_", "");
	}

	private int[] at(long a) {
		return votes.computeIfAbsent(a, k -> new int[3]);
	}

	private static boolean isReg(String s) {
		return s.matches("r\\d+");
	}

	private boolean reads(Instruction i, Set<String> regs) {
		for (Object o : i.getInputObjects()) {
			if (o instanceof Register && regs.contains(((Register) o).getName())) {
				return true;
			}
		}
		return false;
	}

	/** How the value a load put in reg is used: 1 signed, -1 unsigned, 0 no vote. */
	private int vote(Instruction load, String reg, int size) {
		Set<String> val = new HashSet<>();
		val.add(reg);
		Instruction i = load.getNext();
		for (int n = 0; i != null && n < 16 && !val.isEmpty(); n++, i = i.getNext()) {
			String m = mnem(i);
			int ops = i.getNumOperands();
			String a = ops > 0 ? i.getDefaultOperandRepresentation(0) : "", b = ops > 1 ? i.getDefaultOperandRepresentation(1) : "";
			if (m.equals(size == 1 ? "extu.b" : "extu.w") && val.contains(a)) {
				return -1;
			}
			if (m.equals("rts")) {
				return val.contains("r0") ? 1 : 0;
			}
			if (i.getFlowType().isCall()) {
				for (String r : new String[] { "r4", "r5", "r6", "r7" }) {
					if (val.contains(r)) {
						return 1;
					}
				}
				return 0;
			}
			if (m.equals("mov") && ops == 2 && val.contains(a) && isReg(b)) {
				val.add(b); // a copy holds the value too
				continue;
			}
			if (reads(i, val)) {
				boolean imm = a.startsWith("#");
				boolean neutral = m.equals("tst") || m.startsWith("exts")
					|| (imm && (m.equals("and") || m.equals("or") || m.equals("xor") || m.equals("cmp/eq")))
					|| (m.equals(size == 1 ? "mov.b" : "mov.w") && b.startsWith("@") && val.contains(a));
				if (!neutral) {
					return 1;
				}
			}
			for (Object o : i.getResultObjects()) {
				if (o instanceof Register) {
					val.remove(((Register) o).getName());
				}
			}
			if (i.getFlowType().isJump() || i.getFlowType().isTerminal()) {
				return 0; // end of the straight line
			}
		}
		return 0;
	}

	// address -> {signed uses, unsigned uses} of a 4-byte value loaded from it
	private final Map<Long, int[]> words = new TreeMap<>();

	/** How a 4-byte value is compared: 1 signed (cmp/gt, cmp/ge, cmp/pl,
	 *  cmp/pz, an arithmetic shift, a conversion to float without the
	 *  unsigned fix-up), -1 unsigned (cmp/hi, cmp/hs, a logical shift, a
	 *  conversion to float with it: its cmp/pz, before or after the lds, is
	 *  not a signed compare),
	 *  0 neither (undefined4 exports as unsigned: a signed compare
	 *  turns x < 1 into x == 0). */
	private int voteWord(Instruction load, String reg) {
		Set<String> val = new HashSet<>();
		val.add(reg);
		Instruction i = load.getNext();
		for (int n = 0; i != null && n < 16 && !val.isEmpty(); n++, i = i.getNext()) {
			String m = mnem(i);
			int ops = i.getNumOperands();
			String a = ops > 0 ? i.getDefaultOperandRepresentation(0) : "", b = ops > 1 ? i.getDefaultOperandRepresentation(1) : "";
			if (m.equals("mov") && ops == 2 && val.contains(a) && isReg(b)) {
				val.add(b);
				continue;
			}
			if (reads(i, val)) {
				if (m.equals("lds") && b.equalsIgnoreCase("fpul")) {
					// to float: an unsigned value needs a fix-up, which tests its
					// sign (cmp/pz) right after; a signed one converts as it is
					Instruction j = i.getNext();
					for (int k = 0; j != null && k < 8; k++, j = j.getNext()) {
						if (mnem(j).equals("cmp/pz") && reads(j, val)) {
							return -1;
						}
					}
					return 1;
				}
				if (m.equals("cmp/pz")) {
					// GCC's unsigned-to-float tests the sign first: cmp/pz, then
					// lds of the same value to fpul (FUN_0c130a40)
					Instruction j = i.getNext();
					for (int k = 0; j != null && k < 4; k++, j = j.getNext()) {
						if (mnem(j).equals("lds") && reads(j, val)) {
							return -1;
						}
					}
				}
				if (m.equals("cmp/gt") || m.equals("cmp/ge") || m.equals("cmp/pl") || m.equals("cmp/pz")
					|| m.equals("shar") || m.equals("shad")) {
					return 1;
				}
				if (m.equals("cmp/hi") || m.equals("cmp/hs") || m.startsWith("shlr") || m.equals("shld")) {
					return -1;
				}
			}
			for (Object o : i.getResultObjects()) {
				if (o instanceof Register) {
					val.remove(((Register) o).getName());
				}
			}
			if (i.getFlowType().isCall() || i.getFlowType().isJump() || i.getFlowType().isTerminal()) {
				return 0;
			}
		}
		return 0;
	}

	private void follow(Instruction load, String reg, long base) {
		Map<String, Integer> alias = new HashMap<>();
		alias.put(reg, 0);
		Instruction i = load.getNext();
		for (int n = 0; i != null && n < 32 && !alias.isEmpty(); n++, i = i.getNext()) {
			String m = mnem(i);
			int ops = i.getNumOperands();
			String a = ops > 0 ? i.getDefaultOperandRepresentation(0) : "", b = ops > 1 ? i.getDefaultOperandRepresentation(1) : "";
			// accesses through an alias at a known offset
			Integer off = null;
			java.util.regex.Matcher mm = java.util.regex.Pattern.compile("^@\\(0x([0-9a-f]+),(r\\d+)\\)$").matcher(a);
			if (mm.matches() && alias.containsKey(mm.group(2))) {
				off = alias.get(mm.group(2)) + Integer.parseInt(mm.group(1), 16);
			} else if ((mm = java.util.regex.Pattern.compile("^@(r\\d+)$").matcher(a)).matches() && alias.containsKey(mm.group(1))) {
				off = alias.get(mm.group(1));
			}
			for (String op : new String[] { a, b }) { // any other access marks the size
				java.util.regex.Matcher am = java.util.regex.Pattern.compile("^@(?:\\(0x([0-9a-f]+),)?-?(r\\d+)\\+?\\)?$").matcher(op);
				if (am.matches() && alias.containsKey(am.group(2))) {
					int o = alias.get(am.group(2)) + (am.group(1) == null ? 0 : Integer.parseInt(am.group(1), 16));
					at(base + o)[2] |= m.endsWith(".b") ? 1 : m.endsWith(".w") ? 2 : 4;
				}
			}
			if (off != null && m.equals("mov.l") && isReg(b) && !a.startsWith("@-")) {
				int v = voteWord(i, b);
				if (v != 0) {
					words.computeIfAbsent(base + off, k -> new int[2])[v > 0 ? 0 : 1]++;
				}
			}
			if (off != null && (m.equals("mov.b") || m.equals("mov.w")) && isReg(b)) {
				int v = vote(i, b, m.equals("mov.b") ? 1 : 2);
				if (v != 0) {
					at(base + off)[v > 0 ? 0 : 1]++;
				}
			}
			if (m.equals("mov") && ops == 2 && alias.containsKey(a) && isReg(b)) {
				alias.put(b, alias.get(a));
			} else if (m.equals("add") && ops == 2 && a.startsWith("#") && alias.containsKey(b)) {
				alias.put(b, alias.get(b) + (int) Long.decode(a.substring(1).replace("0x-", "-0x")).longValue());
			} else if (ops == 2 && isReg(b)) {
				alias.remove(b);
			}
			if (m.contains("@r") && (a.endsWith("+") || b.startsWith("@-"))) {
				alias.remove(a.replaceAll("[@+]", ""));
				alias.remove(b.replaceAll("[@-]", ""));
			}
			if (i.getFlowType().isCall() || i.getFlowType().isJump() || i.getFlowType().isTerminal()) {
				break;
			}
		}
	}

	@Override
	protected void run() throws Exception {
		for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
			for (Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) {
				if (!mnem(i).equals("mov.l") || i.getNumOperands() != 2) {
					continue;
				}
				String reg = i.getDefaultOperandRepresentation(1);
				if (!isReg(reg) || !i.getDefaultOperandRepresentation(0).matches("0x[0-9a-fA-F]+")) {
					continue;
				}
				for (Reference r : i.getReferencesFrom()) {
					Address pa = r.getToAddress();
					if (!pa.isMemoryAddress() || pa.getOffset() >= LO || !currentProgram.getMemory().getLoadedAndInitializedAddressSet().contains(pa)) {
						continue;
					}
					long v;
					try {
						v = getInt(pa) & 0xFFFFFFFFL;
					} catch (ghidra.program.model.mem.MemoryAccessException e) {
						continue;
					}
					if (v >= LO && v < HI) {
						follow(i, reg, v);
					}
				}
			}
		}
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "signs.tsv");
		int setChar = 0, setShort = 0, unsignedSeen = 0, mixed = 0, skipped = 0;
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("address\tsigned uses\tunsigned uses\tsizes\tdecision");
			for (Map.Entry<Long, int[]> e : votes.entrySet()) {
				int s = e.getValue()[0], u = e.getValue()[1], sizes = e.getValue()[2];
				if (s == 0 && u == 0) {
					continue;
				}
				Address a = toAddr(e.getKey());
				String decision;
				int size = sizes == 1 ? 1 : sizes == 2 ? 2 : 0;
				if (u > 0) {
					decision = s > 0 ? "mixed: left alone" : "unsigned: left alone";
					if (s > 0) {
						mixed++;
					} else {
						unsignedSeen++;
					}
				} else if (size == 0) {
					decision = "loaded at more than one size: left alone";
					skipped++;
				} else {
					decision = retype(a, size);
					if (decision.startsWith("set")) {
						if (size == 1) {
							setChar++;
						} else {
							setShort++;
						}
					} else {
						skipped++;
					}
				}
				w.printf("%s\t%d\t%d\t%s\t%s%n", a, s, u, sizes == 1 ? "byte" : sizes == 2 ? "halfword" : "mixed", decision);
			}
		}
		// words: compared signed, never unsigned, only ever accessed whole
		int setInt = 0;
		File wout = new File(getSourceFile().getParentFile().getAbsolutePath(), "signs_words.tsv");
		try (PrintWriter w = new PrintWriter(wout)) {
			w.println("address\tsigned compares\tunsigned compares\tdecision");
			for (Map.Entry<Long, int[]> e : words.entrySet()) {
				int sv = e.getValue()[0], uv = e.getValue()[1];
				int[] all = votes.get(e.getKey());
				Address a = toAddr(e.getKey());
				String decision;
				if (uv > 0) {
					decision = sv > 0 ? "mixed: left alone" : "unsigned: left alone";
				} else if (all != null && (all[2] & 3) != 0 && sv < 4) {
					// a narrow read of a word (its low byte) does not argue
					// against it being an int; with few compares, stay out
					decision = "accessed as bytes or halfwords too: left alone";
				} else {
					decision = retypeWord(a);
					if (decision.startsWith("set")) {
						setInt++;
					}
				}
				w.printf("%s\t%d\t%d\t%s%n", a, sv, uv, decision);
			}
		}
		println(String.format("signs: %d set char, %d set short, %d set int; %d unsigned, %d mixed, %d left alone -> %s",
			setChar, setShort, setInt, unsignedSeen, mixed, skipped, out));
	}

	/** A 4-byte global (or member) as int, when it is still undefined there. */
	private String retypeWord(Address a) throws Exception {
		if (getFunctionContaining(a) != null || a.getOffset() % 4 != 0) {
			return "in code or not aligned: left alone";
		}
		Data d = currentProgram.getListing().getDataContaining(a);
		if (d != null && d.getDataType() instanceof ghidra.program.model.data.Composite) {
			return retypeMember(d, a, ghidra.program.model.data.IntegerDataType.dataType);
		}
		if (d != null && d.getDataType() instanceof ghidra.program.model.data.Array) {
			return "inside " + d.getDataType().getName() + ": left alone";
		}
		if (d != null && d.isDefined() && !(d.getDataType() instanceof Undefined)) {
			return "typed " + d.getDataType().getName() + ": left alone";
		}
		if (d != null && d.isDefined() && (!d.getAddress().equals(a) || d.getLength() != 4)) {
			return "other data there: left alone";
		}
		try {
			DataUtilities.createData(currentProgram, a, ghidra.program.model.data.IntegerDataType.dataType, -1,
				DataUtilities.ClearDataMode.CLEAR_ALL_UNDEFINED_CONFLICT_DATA);
			ghidra.program.model.symbol.Symbol sy = currentProgram.getSymbolTable().getPrimarySymbol(a);
			if (sy == null || sy.getSource() == ghidra.program.model.symbol.SourceType.DEFAULT) {
				createLabel(a, "DAT_" + a.toString(), true, ghidra.program.model.symbol.SourceType.ANALYSIS);
			}
		} catch (Exception ex) {
			return "does not fit: left alone (" + ex.getMessage() + ")";
		}
		return "set int";
	}

	/** Type a as char / short when nothing better is there. */
	private String retype(Address a, int size) throws Exception {
		if (getFunctionContaining(a) != null) {
			return "in code: left alone";
		}
		Data d = currentProgram.getListing().getDataContaining(a);
		if (d != null && d.getDataType() instanceof ghidra.program.model.data.Composite) {
			return retypeMember(d, a, size == 1 ? CharDataType.dataType : ShortDataType.dataType);
		}
		if (d != null && d.getDataType() instanceof ghidra.program.model.data.Array) {
			return "inside " + d.getDataType().getName() + ": left alone";
		}
		if (d != null && d.isDefined() && !(d.getDataType() instanceof Undefined)) {
			return "typed " + d.getDataType().getName() + ": left alone";
		}
		if (d != null && d.isDefined() && !d.getAddress().equals(a)) {
			return "inside other data: left alone";
		}
		DataType t = size == 1 ? CharDataType.dataType : ShortDataType.dataType;
		// typed data gets a default label from its type and value (CHAR_00h_...,
		// SHORT_...); keep the address name the rest of the tree uses
		String keep = "DAT_" + a.toString();
		try {
			DataUtilities.createData(currentProgram, a, t, -1, DataUtilities.ClearDataMode.CLEAR_ALL_UNDEFINED_CONFLICT_DATA);
			ghidra.program.model.symbol.Symbol s = currentProgram.getSymbolTable().getPrimarySymbol(a);
			if (s == null || s.getSource() == ghidra.program.model.symbol.SourceType.DEFAULT) {
				createLabel(a, keep, true, ghidra.program.model.symbol.SourceType.ANALYSIS);
			}
		} catch (Exception ex) {
			return "does not fit: left alone (" + ex.getMessage() + ")";
		}
		return "set " + t.getName();
	}

	/** A member of an object made by AssignAggregates: given type t when it
	 *  is still untyped there (undefined1/2/4, not part of a bigger member)
	 *  and t fits it exactly; its name stays. */
	private String retypeMember(Data obj, Address a, ghidra.program.model.data.DataType t) {
		if (!obj.getDataType().getName().startsWith("agg_")
			|| !(obj.getDataType() instanceof ghidra.program.model.data.Structure)) {
			return "inside " + obj.getDataType().getName() + ": left alone";
		}
		ghidra.program.model.data.Structure st = (ghidra.program.model.data.Structure) obj.getDataType();
		int off = (int) a.subtract(obj.getAddress());
		ghidra.program.model.data.DataTypeComponent c = st.getComponentContaining(off);
		if (c == null || c.getOffset() != off) {
			return "inside a member of " + st.getName() + ": left alone";
		}
		if (!(c.getDataType() instanceof ghidra.program.model.data.Undefined) || c.getLength() != t.getLength()) {
			return "member of " + st.getName() + " typed " + c.getDataType().getName() + ": left alone";
		}
		st.replaceAtOffset(off, t, t.getLength(), c.getFieldName(), c.getComment());
		return "set " + t.getName() + " (member of " + st.getName() + ")";
	}

}
