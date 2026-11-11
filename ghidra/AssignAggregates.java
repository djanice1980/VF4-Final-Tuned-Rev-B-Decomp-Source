// Aggregates (structs, arrays) from how the code addresses them. GCC 3.2 at
// -O1 loads one pool word per object and reaches its members as base +
// offset (mov.b r4,@(disp,rN), add #k,rN ...), while two separate globals
// each get their own pool word; so a data address the code accesses at
// positive offsets is the start of one object reaching at least that far.
// The decompiler cannot know that from the data alone and shows the members
// as separate DAT_ globals, and its output then compiles to the wrong code
// (FUN_0c02d320 matches once 0x0C370CE0 is one object).
//
// For every pool load of an address in .rodata / .data / .bss, the register
// is followed through straight-line code (copies and add #imm included);
// each memory access through it at offset > 0 is recorded with its size
// (mov.b 1, mov.w 2, mov.l / fmov 4). Objects whose ranges overlap are
// merged. Each gets a structure type agg_<address> in /vf4ft/aggregates
// with fields at the observed offsets (field_0x..: our names; a layout, not
// a reconstruction of the original type), applied at its address, unless
// the range starts at a typed named global or holds a named array / struct or
// other typed data (left alone, reported; a name alone at the start, from a
// bare-name symbol row, stays the object's name); named scalars inside (debug-menu labels
// of single members) become members with that name and type.
// GCC reaches a member past @(disp,rN)'s 60 bytes through a second pool
// word, base = ((off + 4) & ~60) - 4 from the object's start, so one large
// object shows as a run of 64-byte pieces with gaps between them, which the
// overlap merge cannot join. ghidra/aggregate_extents.tsv (address, size,
// evidence) gives such an object the size it reaches at least; the pieces
// inside then merge into it.
// Writes ghidra/aggregates.tsv.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.CategoryPath;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeConflictHandler;
import ghidra.program.model.data.DataUtilities;
import ghidra.program.model.data.FloatDataType;
import ghidra.program.model.data.StructureDataType;
import ghidra.program.model.data.Undefined1DataType;
import ghidra.program.model.data.Undefined2DataType;
import ghidra.program.model.data.Undefined4DataType;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;

public class AssignAggregates extends GhidraScript {

	static final long LO = 0x0C15DA00L, HI = 0x0E000000L;
	static final CategoryPath CAT = new CategoryPath("/vf4ft/aggregates");

	// base address -> (offset -> access size; 0 = float)
	private final Map<Long, TreeMap<Integer, Integer>> seen = new TreeMap<>();

	private static String mnem(Instruction i) {
		return i.getMnemonicString().replaceFirst("^_", "");
	}

	private static int size(String m) {
		return m.startsWith("fmov") ? 0 : m.endsWith(".b") ? 1 : m.endsWith(".w") ? 2 : m.endsWith(".l") ? 4 : -1;
	}

	private void note(long base, int off, int sz) {
		if (off >= 0 && off < 0x10000 && sz >= 0) {
			// a float (fmov, size 0) wins over a word move of the same member:
			// floats are copied with mov.l too
			seen.computeIfAbsent(base, k -> new TreeMap<>()).merge(off, sz,
				(old, now) -> (old == 4 && now == 0) || (old == 0 && now == 4) ? 0 : old);
		}
	}

	/** Follow the register a pool load of base filled, recording accesses through it. */
	private void follow(Instruction load, String reg, long base) {
		Map<String, Integer> alias = new HashMap<>(); // register -> offset from base
		alias.put(reg, 0);
		Instruction i = load.getNext();
		for (int n = 0; i != null && n < 32 && !alias.isEmpty(); n++, i = i.getNext()) {
			String m = mnem(i);
			int ops = i.getNumOperands();
			String a = ops > 0 ? i.getDefaultOperandRepresentation(0) : "", b = ops > 1 ? i.getDefaultOperandRepresentation(1) : "";
			// memory operands through an alias: @(disp,rX), @rX, @rX+, @-rX
			for (String op : new String[] { a, b }) {
				java.util.regex.Matcher mm = java.util.regex.Pattern.compile("^@\\(0x([0-9a-f]+),(r\\d+)\\)$").matcher(op);
				if (mm.matches() && alias.containsKey(mm.group(2))) {
					note(base, alias.get(mm.group(2)) + Integer.parseInt(mm.group(1), 16), size(m));
				}
				mm = java.util.regex.Pattern.compile("^@-?(r\\d+)\\+?$").matcher(op);
				if (mm.matches() && alias.containsKey(mm.group(1))) {
					note(base, alias.get(mm.group(1)), size(m));
				}
			}
			// how the aliases change
			if (m.equals("mov") && ops == 2 && alias.containsKey(a) && b.matches("r\\d+")) {
				alias.put(b, alias.get(a));
			} else if (m.equals("add") && ops == 2 && a.startsWith("#") && alias.containsKey(b)) {
				alias.put(b, alias.get(b) + (int) Long.decode(a.substring(1).replace("0x-", "-0x")).longValue());
			} else if (ops == 2 && b.matches("r\\d+")) {
				alias.remove(b); // overwritten (or changed some other way)
			}
			if (m.contains("@r") && (a.endsWith("+") || b.startsWith("@-"))) {
				// post-increment / pre-decrement change the register; stop tracking it
				alias.remove(a.replaceAll("[@+]", ""));
				alias.remove(b.replaceAll("[@-]", ""));
			}
			if (i.getFlowType().isCall() || i.getFlowType().isJump() || i.getFlowType().isTerminal()) {
				break; // end of the straight line (the delay slot ran before)
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
				if (!reg.matches("r\\d+") || !i.getDefaultOperandRepresentation(0).matches("0x[0-9a-fA-F]+")) {
					continue;
				}
				for (Reference r : i.getReferencesFrom()) {
					// the pool word itself (in .text); the load also references what it points to
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
		// objects known to reach further than their own accesses show
		Map<Long, Integer> least = new HashMap<>();
		File known = new File(getSourceFile().getParentFile().getAbsolutePath(), "aggregate_extents.tsv");
		for (String line : java.nio.file.Files.readAllLines(known.toPath())) {
			String[] f = line.split("\t");
			if (!f[0].equals("address")) {
				least.put(Long.parseLong(f[0], 16), Integer.decode(f[1]));
			}
		}
		// merge overlapping objects (an access range reaching into the next base)
		TreeMap<Long, Integer> extent = new TreeMap<>();
		Map<Long, TreeMap<Integer, Integer>> fields = new TreeMap<>();
		for (Map.Entry<Long, TreeMap<Integer, Integer>> e : seen.entrySet()) {
			long base = e.getKey();
			if (e.getValue().lastKey() == 0) {
				continue; // only ever accessed directly: a field at most (below)
			}
			Map.Entry<Integer, Integer> last = e.getValue().lastEntry();
			int ext = last.getKey() + Math.max(1, last.getValue() == 0 ? 4 : last.getValue());
			Map.Entry<Long, Integer> prev = extent.floorEntry(base);
			if (prev != null && base < prev.getKey() + prev.getValue()) {
				long pb = prev.getKey();
				int shift = (int) (base - pb);
				extent.put(pb, Math.max(prev.getValue(), shift + ext));
				for (Map.Entry<Integer, Integer> fe : e.getValue().entrySet()) {
					fields.get(pb).putIfAbsent(shift + fe.getKey(), fe.getValue());
				}
				fields.get(pb).putIfAbsent(shift, -1); // the inner base itself is a member
			} else {
				extent.put(base, Math.max(ext, least.getOrDefault(base, 0)));
				fields.put(base, new TreeMap<>(e.getValue()));
			}
		}
		// addresses accessed directly that fall inside an object are fields of it
		for (Map.Entry<Long, TreeMap<Integer, Integer>> e : seen.entrySet()) {
			Integer sz = e.getValue().get(0);
			Map.Entry<Long, Integer> obj = extent.floorEntry(e.getKey());
			if (sz != null && obj != null && e.getKey() > obj.getKey() && e.getKey() < obj.getKey() + obj.getValue()) {
				fields.get(obj.getKey()).putIfAbsent((int) (e.getKey() - obj.getKey()), sz);
			}
		}
		int made = 0, skipped = 0;
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "aggregates.tsv");
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("address\tsize\tfields\tresult");
			for (Map.Entry<Long, Integer> e : extent.entrySet()) {
				long base = e.getKey();
				int ext = e.getValue();
				Address a = toAddr(base);
				String why = conflict(a, ext);
				TreeMap<Integer, Integer> fs = fields.get(base);
				if (why != null) {
					w.printf("%08x\t%d\t%d\tleft alone: %s%n", base, ext, fs.size(), why);
					skipped++;
					continue;
				}
				StructureDataType st = new StructureDataType(CAT, String.format("agg_%08x", base), ext);
				// pointers already defined inside are members too: keep them as pointer fields
				Map<Integer, DataType> ptrs = new TreeMap<>();
				ghidra.program.model.listing.DataIterator di = currentProgram.getListing()
					.getDefinedData(new ghidra.program.model.address.AddressSet(a, a.add(ext - 1)), true);
				while (di.hasNext()) {
					Data d = di.next();
					if (d.isPointer()) {
						ptrs.put((int) d.getAddress().subtract(a), d.getDataType());
					} else if (d.getDataType().getName().startsWith("agg_")) {
						// made by an earlier run of this step: its pointer members
						for (int k = 0; k < d.getNumComponents(); k++) {
							Data m = d.getComponent(k);
							if (m.isPointer()) {
								ptrs.put((int) m.getAddress().subtract(a), m.getDataType());
							}
						}
					}
				}
				for (Map.Entry<Integer, Object[]> ne : named.entrySet()) {
					DataType t = ne.getValue()[1] == null ? Undefined1DataType.dataType : (DataType) ne.getValue()[1];
					if (ne.getKey() + t.getLength() <= ext) {
						st.replaceAtOffset(ne.getKey(), t, t.getLength(), (String) ne.getValue()[0], "debug-menu name");
					}
				}
				int end = 0;
				for (Map.Entry<Integer, Integer> fe : fs.entrySet()) {
					int off = fe.getKey(), sz = fe.getValue();
					DataType t = ptrs.containsKey(off) ? ptrs.remove(off)
						: sz == 0 ? FloatDataType.dataType : sz == 4 ? Undefined4DataType.dataType
						: sz == 2 ? Undefined2DataType.dataType : sz == 1 ? Undefined1DataType.dataType : null;
					if (t == null || off < end || off + t.getLength() > ext
						|| st.getComponentContaining(off) != null && st.getComponentContaining(off).getFieldName() != null
						&& !st.getComponentContaining(off).getFieldName().startsWith("field_")
						|| overlapsNamed(st, off, t.getLength())) {
						continue; // an overlapping access (or the merged inner base): leave undefined
					}
					st.replaceAtOffset(off, t, t.getLength(), String.format("field_0x%x", off), null);
					end = off + t.getLength();
				}
				for (Map.Entry<Integer, DataType> pe : ptrs.entrySet()) { // pointers no access reached
					int off = pe.getKey();
					if (st.getComponentAt(off) != null && st.getComponentAt(off).getDataType() == ghidra.program.model.data.DataType.DEFAULT
						&& off + 4 <= ext) {
						st.replaceAtOffset(off, pe.getValue(), 4, String.format("field_0x%x", off), null);
					}
				}
				DataType dt = currentProgram.getDataTypeManager().addDataType(st, DataTypeConflictHandler.REPLACE_HANDLER);
				// clear the range first: when the object grew (aggregate_extents.tsv),
				// the type is replaced in place but the data made last time keeps
				// its old length, createData leaves it as it is, and the old pieces
				// inside stay (AssignSigns then types their members, not ours).
				// conflict() allowed only agg_ objects and named scalars here; the
				// named ones and the pointers are members of st already
				currentProgram.getListing().clearCodeUnits(a, a.add(ext - 1), false);
				DataUtilities.createData(currentProgram, a, dt, -1, DataUtilities.ClearDataMode.CLEAR_ALL_CONFLICT_DATA);
				w.printf("%08x\t%d\t%d\tmade agg_%08x%s%n", base, ext, fs.size(), base,
					named.isEmpty() ? "" : " (members named from the debug menu: " + named.size() + ")");
				made++;
			}
		}
		println(String.format("aggregates: %d made, %d left alone (named or typed globals inside) -> %s", made, skipped, out));
	}

	private boolean overlapsNamed(StructureDataType st, int off, int len) {
		for (Map.Entry<Integer, Object[]> ne : named.entrySet()) {
			DataType t = ne.getValue()[1] == null ? Undefined1DataType.dataType : (DataType) ne.getValue()[1];
			if (off < ne.getKey() + t.getLength() && ne.getKey() < off + len) {
				return true;
			}
		}
		return false;
	}

	/** The data at a: on its own, or (when this step runs again) the member of
	 *  an agg_ object made last time, so a named member keeps its type. */
	private Data scalarAt(Address at) {
		Data d = getDataAt(at);
		if (d != null) {
			return d;
		}
		Data obj = currentProgram.getListing().getDataContaining(at);
		if (obj != null && obj.getDataType().getName().startsWith("agg_")) {
			Data m = obj.getComponentContaining((int) at.subtract(obj.getAddress()));
			if (m != null && m.getAddress().equals(at)) {
				return m;
			}
		}
		return null;
	}

	private java.util.Set<Long> bare;

	/** Addresses of data rows in the symbol tables that give a name and no type. */
	private java.util.Set<Long> bareNames() throws Exception {
		if (bare != null) {
			return bare;
		}
		bare = new java.util.HashSet<>();
		File[] tables = new File(getSourceFile().getParentFile().getAbsolutePath(),
			currentProgram.getName().equals("main.bin") ? "symbols" : "symbols_test").listFiles((d, n) -> n.endsWith(".tsv"));
		for (File t : tables == null ? new File[0] : tables) {
			for (String l : java.nio.file.Files.readAllLines(t.toPath())) {
				String[] c = l.split("\t", -1);
				if (c.length > 2 && c[1].equals("data") && c[0].matches("[0-9a-f]{8}") && c[2].strip().matches("[A-Za-z_]\\w*")) {
					bare.add(Long.parseLong(c[0], 16));
				}
			}
		}
		return bare;
	}

	// named scalars inside the range conflict() last looked at: offset -> {name, type}
	private final Map<Integer, Object[]> named = new TreeMap<>();

	/** Why a structure cannot go at [a, a+len): a named start, a named array or
	 *  struct inside, other typed data, or code. Named scalars inside are left in
	 *  named, to become members. */
	private String conflict(Address a, int len) throws Exception {
		Address end = a.add(len - 1);
		if (getMemoryBlock(a) == null || !getMemoryBlock(a).equals(getMemoryBlock(end))) {
			return "crosses a memory block";
		}
		// an object that starts earlier and covers this range (an element of a
		// typed array: &jvs_reply_offset[k] folds to one address)
		Data around = currentProgram.getListing().getDataContaining(a);
		if (around != null && around.getAddress().compareTo(a) < 0 && around.isDefined()
			&& !(around.getDataType() instanceof ghidra.program.model.data.Undefined) && !around.isPointer()) {
			return "inside " + around.getDataType().getName() + " at " + around.getAddress();
		}
		named.clear();
		for (Symbol s : currentProgram.getSymbolTable().getSymbols(new ghidra.program.model.address.AddressSet(a, end), ghidra.program.model.symbol.SymbolType.LABEL, true)) {
			if (s.getSource() == SourceType.DEFAULT) {
				continue;
			}
			if (s.getAddress().equals(a)) {
				// a bare-name symbol row (no type of its own): the object keeps
				// the name and gets the layout the code gives it
				if (bareNames().contains(a.getOffset())) {
					continue;
				}
				return "named global " + s.getName() + " at its start";
			}
			// a debug-menu name for one scalar inside: a member, keeping its name and type
			Data d = scalarAt(s.getAddress());
			if (d != null && (d.getLength() > 8 || d.getDataType() instanceof ghidra.program.model.data.Composite
				|| d.getDataType() instanceof ghidra.program.model.data.Array)) {
				return "named global " + s.getName() + " (" + d.getDataType().getName() + ") inside";
			}
			named.put((int) s.getAddress().subtract(a), new Object[] { s.getName(), d == null ? null : d.getDataType() });
		}
		ghidra.program.model.listing.DataIterator di = currentProgram.getListing().getDefinedData(new ghidra.program.model.address.AddressSet(a, end), true);
		while (di.hasNext()) {
			Data d = di.next();
			String n = d.getDataType().getName();
			if (!(d.getDataType() instanceof ghidra.program.model.data.Undefined) && !d.isPointer()
				&& !n.startsWith("undefined") && !n.startsWith("agg_") && !named.containsKey((int) d.getAddress().subtract(a))) {
				return "typed data " + n + " at " + d.getAddress();
			}
		}
		if (currentProgram.getListing().getInstructions(new ghidra.program.model.address.AddressSet(a, end), true).hasNext()) {
			return "code inside";
		}
		return null;
	}
}
