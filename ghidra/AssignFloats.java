// Floats among the unnamed globals, from how the code moves them. Ghidra
// types a 4-byte global it finds as undefined4, which the export declares
// as an integer; a float stored there then goes through GCC's
// __fixunssfsi (float to unsigned) where the game stores the bits
// (DAT_0c489444 = DAT_0c489444 + 0.5: 113 near misses called the helper).
//
// For every pool load of a data address, the register is followed through
// straight-line code (copies and add #imm, as in AssignAggregates.java);
// each access through it is counted: fmov to or from an FPU register
// (fmov.s @rA,frN / frN,@rA) is a float access, mov.l a word access (copies
// move floats that way too), mov.b / mov.w another size, and a loaded word
// used as an address a pointer. A global with float accesses, no other size
// and no pointer use, undefined there (or undefined4), not a function and
// not inside an array (a member of an object AssignAggregates made is
// retyped there, when untyped), becomes a float. It keeps its
// DAT_<address> name (an analysis label, as AssignSigns.java does). Writes
// ghidra/floats.tsv.
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
import ghidra.program.model.data.DataUtilities;
import ghidra.program.model.data.Undefined;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;

public class AssignFloats extends GhidraScript {

	static final long LO = 0x0C15DA00L, HI = 0x0E000000L;

	// address -> {pointer uses, 4-byte (mov.l) accesses, other-size accesses, float (fmov) accesses}
	private final Map<Long, int[]> votes = new TreeMap<>();

	private static String mnem(Instruction i) {
		return i.getMnemonicString().replaceFirst("^_", "");
	}

	private static boolean isReg(String s) {
		return s.matches("r\\d+");
	}

	private int[] at(long a) {
		return votes.computeIfAbsent(a, k -> new int[4]);
	}

	/** Is the value a load put in reg used as an address before it is overwritten? */
	private boolean usedAsBase(Instruction load, String reg) {
		Set<String> val = new HashSet<>();
		val.add(reg);
		Instruction i = load.getNext();
		for (int n = 0; i != null && n < 16 && !val.isEmpty(); n++, i = i.getNext()) {
			String m = mnem(i);
			int ops = i.getNumOperands();
			for (int k = 0; k < ops; k++) {
				String op = i.getDefaultOperandRepresentation(k);
				java.util.regex.Matcher mm = java.util.regex.Pattern.compile("^@(?:\\(0x[0-9a-f]+,|\\(r0,)?-?(r\\d+)\\+?\\)?$").matcher(op);
				if (mm.matches() && val.contains(mm.group(1))) {
					return true;
				}
			}
			String a = ops > 0 ? i.getDefaultOperandRepresentation(0) : "", b = ops > 1 ? i.getDefaultOperandRepresentation(1) : "";
			if (m.equals("mov") && ops == 2 && val.contains(a) && isReg(b)) {
				val.add(b);
				continue;
			}
			for (Object o : i.getResultObjects()) {
				if (o instanceof Register) {
					val.remove(((Register) o).getName());
				}
			}
			if (i.getFlowType().isCall() || i.getFlowType().isJump() || i.getFlowType().isTerminal()) {
				return false;
			}
		}
		return false;
	}

	private void follow(Instruction load, String reg, long base) {
		Map<String, Integer> alias = new HashMap<>();
		alias.put(reg, 0);
		Instruction i = load.getNext();
		for (int n = 0; i != null && n < 32 && !alias.isEmpty(); n++, i = i.getNext()) {
			String m = mnem(i);
			int ops = i.getNumOperands();
			String a = ops > 0 ? i.getDefaultOperandRepresentation(0) : "", b = ops > 1 ? i.getDefaultOperandRepresentation(1) : "";
			for (String op : new String[] { a, b }) {
				// an indexed access (@(r0,rX), rX holding the address) reads an
				// element of an array starting there: counts for its first
				java.util.regex.Matcher xm = java.util.regex.Pattern.compile("^@\\(r0,(r\\d+)\\)$").matcher(op);
				if (xm.matches() && alias.containsKey(xm.group(1))) {
					int off = alias.get(xm.group(1));
					if (m.startsWith("fmov") && (b.startsWith("fr") || a.startsWith("fr"))) {
						at(base + off)[3]++;
					} else if (m.equals("mov.l")) {
						at(base + off)[1]++;
					} else {
						at(base + off)[2]++;
					}
					continue;
				}
				java.util.regex.Matcher am = java.util.regex.Pattern.compile("^@(?:\\(0x([0-9a-f]+),)?-?(r\\d+)\\+?\\)?$").matcher(op);
				if (am.matches() && alias.containsKey(am.group(2))) {
					int off = alias.get(am.group(2)) + (am.group(1) == null ? 0 : Integer.parseInt(am.group(1), 16));
					if (m.startsWith("fmov") && (b.startsWith("fr") || a.startsWith("fr"))) {
						at(base + off)[3]++;
					} else if (m.equals("mov.l") && op.equals(a) && isReg(b)) {
						at(base + off)[1]++;
						if (usedAsBase(i, b)) {
							at(base + off)[0]++;
						}
					} else if (m.equals("mov.l")) {
						at(base + off)[1]++;
					} else {
						at(base + off)[2]++;
					}
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
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "floats.tsv");
		int set = 0, left = 0;
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("address\tfloat accesses\tword accesses\tother accesses\tpointer uses\tdecision");
			for (Map.Entry<Long, int[]> e : votes.entrySet()) {
				int p = e.getValue()[0], loads = e.getValue()[1], other = e.getValue()[2], fl = e.getValue()[3];
				if (fl == 0) {
					continue;
				}
				Address a = toAddr(e.getKey());
				String decision = other > 0 ? "accessed at other sizes too: left alone"
					: p > 0 ? "used as an address too: left alone" : retype(a);
				if (decision.startsWith("set")) {
					set++;
				} else {
					left++;
				}
				w.printf("%s\t%d\t%d\t%d\t%d\t%s%n", a, fl, loads, other, p, decision);
			}
		}
		println(String.format("floats: %d set float, %d left alone -> %s", set, left, out));
	}

	private String retype(Address a) throws Exception {
		if (a.getOffset() % 4 != 0) {
			return "not word aligned: left alone";
		}
		if (getFunctionContaining(a) != null) {
			return "in code: left alone";
		}
		Data d = currentProgram.getListing().getDataContaining(a);
		if (d != null && d.getDataType() instanceof ghidra.program.model.data.Composite) {
			return retypeMember(d, a, ghidra.program.model.data.FloatDataType.dataType);
		}
		if (d != null && d.getDataType() instanceof ghidra.program.model.data.Array) {
			return "inside " + d.getDataType().getName() + ": left alone";
		}
		if (d != null && d.isPointer()) {
			return "a pointer: left alone";
		}
		if (d != null && d.isDefined() && !(d.getDataType() instanceof Undefined)) {
			return "typed " + d.getDataType().getName() + ": left alone";
		}
		if (d != null && d.isDefined() && !d.getAddress().equals(a)) {
			return "inside other data: left alone";
		}
		try {
			DataUtilities.createData(currentProgram, a, ghidra.program.model.data.FloatDataType.dataType, -1,
				DataUtilities.ClearDataMode.CLEAR_ALL_UNDEFINED_CONFLICT_DATA);
			ghidra.program.model.symbol.Symbol s = currentProgram.getSymbolTable().getPrimarySymbol(a);
			if (s == null || s.getSource() == SourceType.DEFAULT) {
				createLabel(a, "DAT_" + a.toString(), true, SourceType.ANALYSIS);
			}
		} catch (Exception ex) {
			return "does not fit: left alone (" + ex.getMessage() + ")";
		}
		return "set float";
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
