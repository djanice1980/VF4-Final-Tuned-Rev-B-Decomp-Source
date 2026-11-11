// Give every fmov the right width and every FPU arithmetic instruction the
// right precision: compute FPSCR.SZ and FPSCR.PR at each instruction by
// dataflow and set the `sz` / `pr` disassembly contexts (ghidra/lang
// variant; the PR field is named fpu_pr) wherever Ghidra's flow context got them wrong, re-disassembling
// those instructions. SZ is done first, then PR, with the same dataflow:
// only fschg changes SZ; PR changes only by loading FPSCR (libgcc's divide
// helpers load 0x80000, PR=1, and restore the caller's FPSCR).
//
// Why flow context is not enough: fschg flips sz for the fall-through
// (globalset(inst_next)), but 59 fschg sit in the delay slot of a bra,
// bsr or bt.s/bf.s, whose target Ghidra decodes with the pre-flip mode;
// and 22 of those are bsr calls into asm helpers that run, and sometimes
// return, with SZ=1.
//
// Dataflow, per function over basic blocks:
//   - fschg flips SZ, including in a delay slot (it runs before the jump)
//   - `sts.l fpscr,@-r15` ... `lds.l @r15+,fpscr` restores the saved SZ
//     (libgcc's idiom); a load of a known constant sets SZ from bit 20
//     (`mov #8,r2; swap.w r2,r2; lds r2,fpscr`, `mov #4,r0; shll16 r0;
//     lds r0,fpscr` in the exception handlers' context save, or `mova L,r0;
//     lds.l @r0+,fpscr` from a literal pool); any other load makes SZ
//     unknown, which is only reported if an fmov then runs with it
//   - a call passes the current SZ to the callee; afterwards SZ is the
//     callee's exit summary (computed the same way), or unchanged if the
//     callee is unknown
// A function's entry SZ is what all its call sites agree on (0 if none);
// iterated until stable.
//
// Writes ghidra/fpu_context.tsv (re-decoded instructions and problems) and
// a "WARNING (fpu model)" line on functions where SZ stays ambiguous.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.block.BasicBlockModel;
import ghidra.program.model.block.CodeBlock;
import ghidra.program.model.block.CodeBlockReference;
import ghidra.program.model.block.CodeBlockReferenceIterator;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.listing.ProgramContext;

public class AssignFpuContext extends GhidraScript {

	static final int UNK = 2;
	static final String MARK = "WARNING (fpu model)";

	private BasicBlockModel model;
	private int bit;          // FPSCR bit of the field being solved: 20 = SZ, 19 = PR
	private boolean isSz;
	private final Map<Address, List<Instruction>> blockInsns = new HashMap<>();

	// per (function, entry sz): exit sz, or UNK
	private final Map<String, Integer> summary = new HashMap<>();
	private final Set<String> inProgress = new HashSet<>();

	// results of the final pass
	private final Map<Address, Integer> before = new TreeMap<>();
	private final Map<Function, String> problems = new HashMap<>();
	private final Map<Function, Set<Integer>> callerEntry = new HashMap<>();

	private static String mn(Instruction i) {
		return i.getMnemonicString().replaceFirst("^_", "");
	}

	private static String text(Instruction i) {
		return i.toString().toLowerCase().replaceFirst("^_", "");
	}

	private List<Instruction> insns(CodeBlock b) {
		return blockInsns.computeIfAbsent(b.getFirstStartAddress(), k -> {
			List<Instruction> out = new ArrayList<>();
			InstructionIterator ii = currentProgram.getListing().getInstructions(b, true);
			while (ii.hasNext()) {
				Instruction i = ii.next();
				out.add(i);
				// Ghidra may end a block at the branch and leave its delay slot out
				if (i.getDelaySlotDepth() > 0 && !ii.hasNext()) {
					Instruction ds = i.getNext();
					if (ds != null && !b.contains(ds.getAddress())) {
						out.add(ds);
					}
				}
			}
			return out;
		});
	}

	// value loaded by `lds rN,fpscr` or `lds.l @rN+,fpscr`, if known
	private Long fpscrValue(Instruction i, String t, Map<String, Long> konst) {
		java.util.regex.Matcher m = java.util.regex.Pattern.compile("^lds(\\.l)? @?(r\\d+)(\\+)?,fpscr").matcher(t);
		if (!m.find()) {
			return null;
		}
		Long r = konst.get(m.group(2));
		if (m.group(1) == null) {
			return r; // lds rN,fpscr
		}
		if (r == null) {
			return null;
		}
		try {
			long v = getInt(toAddr(r)) & 0xFFFFFFFFL; // lds.l @rN+,fpscr
			if (m.group(3) != null) {
				konst.put(m.group(2), r + 4);
			}
			return v;
		} catch (Exception e) {
			return null;
		}
	}

	// just enough constant tracking for FPSCR loads: mov #imm, shll/2/8/16,
	// swap.w, mova, mov.l from a literal pool; anything else writing rN forgets it
	private void track(Instruction i, String m, String t, Map<String, Long> konst) {
		java.util.regex.Matcher mm;
		if ((mm = java.util.regex.Pattern.compile("^mov #(-?(?:0x)?[0-9a-f]+),(r\\d+)$").matcher(t)).find()) {
			konst.put(mm.group(2), (long) (byte) Long.decode(mm.group(1)).longValue());
			return;
		}
		if ((mm = java.util.regex.Pattern.compile("^shll(2|8|16)? (r\\d+)$").matcher(t)).find()) {
			// the exception handlers' context save: mov #4,r0; shll16 r0; lds r0,fpscr
			Long v = konst.get(mm.group(2));
			if (v != null) {
				int n = mm.group(1) == null ? 1 : Integer.parseInt(mm.group(1));
				konst.put(mm.group(2), (v << n) & 0xFFFFFFFFL);
			} else {
				konst.remove(mm.group(2));
			}
			return;
		}
		if ((mm = java.util.regex.Pattern.compile("^swap\\.w (r\\d+),(r\\d+)$").matcher(t)).find()) {
			Long v = konst.get(mm.group(1));
			if (v != null) {
				long x = v & 0xFFFFFFFFL;
				konst.put(mm.group(2), ((x << 16) | (x >>> 16)) & 0xFFFFFFFFL);
			} else {
				konst.remove(mm.group(2));
			}
			return;
		}
		if (m.equals("mova") || (m.equals("mov.l") && t.contains("@(") && t.contains("pc"))) {
			for (ghidra.program.model.symbol.Reference r : i.getReferencesFrom()) {
				String dst = t.substring(t.lastIndexOf(',') + 1).strip();
				try {
					long v = m.equals("mova") ? r.getToAddress().getOffset()
						: getInt(r.getToAddress()) & 0xFFFFFFFFL;
					konst.put(dst, v);
				} catch (Exception e) {
					konst.remove(dst);
				}
				return;
			}
		}
		for (Object o : i.getResultObjects()) {
			if (o instanceof Register) {
				konst.remove(((Register) o).getName().toLowerCase());
			}
		}
	}

	private Function callee(Instruction i) {
		for (Address t : i.getFlows()) {
			Function f = getFunctionAt(t);
			if (f != null) {
				return f;
			}
		}
		return null;
	}

	/**
	 * Run the dataflow over f entered with SZ = entry. Returns the exit SZ
	 * (UNK if returns disagree). If record, fills before/problems/callerEntry.
	 */
	private int analyze(Function f, int entry, boolean record) throws Exception {
		CodeBlock first = model.getFirstCodeBlockContaining(f.getEntryPoint(), monitor);
		if (first == null) {
			return entry;
		}
		Map<Address, Integer> in = new HashMap<>();
		Map<Address, ArrayDeque<Integer>> savedIn = new HashMap<>();
		ArrayDeque<CodeBlock> work = new ArrayDeque<>();
		in.put(first.getFirstStartAddress(), entry);
		savedIn.put(first.getFirstStartAddress(), new ArrayDeque<>());
		work.add(first);
		Integer exit = null;
		while (!work.isEmpty()) {
			CodeBlock b = work.poll();
			int sz = in.get(b.getFirstStartAddress());
			ArrayDeque<Integer> saved = new ArrayDeque<>(savedIn.get(b.getFirstStartAddress()));
			Function pendingCall = null;
			boolean returns = false;
			Map<String, Long> konst = new HashMap<>(); // registers with a known value
			for (Instruction i : insns(b)) {
				if (record) {
					Integer was = before.putIfAbsent(i.getAddress(), sz);
					if (was != null && was != sz && !problems.containsKey(f)) {
						problems.put(f, "paths meet with different SZ at " + i.getAddress());
					}
				}
				String m = mn(i), t = text(i);
				if (record && sz == UNK && dependsOnField(m) && !problems.containsKey(f)) {
					problems.put(f, m + " at " + i.getAddress() + " runs after an FPSCR load of unknown value");
				}
				if (m.equals("fschg") && isSz) {
					sz = sz == UNK ? UNK : sz ^ 1;
				} else if (t.startsWith("sts.l fpscr,@-r15")) {
					saved.push(sz);
				} else if (t.startsWith("lds.l @r15+,fpscr") && !saved.isEmpty()) {
					sz = saved.pop();
				} else if (m.startsWith("lds") && t.contains("fpscr")) {
					Long v = fpscrValue(i, t, konst);
					sz = v == null ? UNK : (int) ((v >> bit) & 1);
				}
				track(i, m, t, konst);
				if (pendingCall != null && !i.isInDelaySlot()) {
					pendingCall = null; // (call without delay slot handled below)
				}
				if (i.getFlowType().isCall()) {
					pendingCall = callee(i);
					if (i.getDelaySlotDepth() == 0) {
						sz = afterCall(pendingCall, sz, record);
						pendingCall = null;
					}
					continue;
				}
				if (pendingCall != null && i.isInDelaySlot()) {
					sz = afterCall(pendingCall, sz, record);
					pendingCall = null;
				}
				if (i.getFlowType().isTerminal()) {
					returns = true;
				}
			}
			if (returns) {
				exit = exit == null ? sz : (exit == sz ? exit : UNK);
			}
			CodeBlockReferenceIterator di = b.getDestinations(monitor);
			while (di.hasNext()) {
				CodeBlockReference r = di.next();
				if (r.getFlowType().isCall()) {
					continue;
				}
				CodeBlock d = r.getDestinationBlock();
				if (d == null || getFunctionContaining(d.getFirstStartAddress()) != f) {
					continue;
				}
				Address da = d.getFirstStartAddress();
				Integer seen = in.get(da);
				if (seen == null) {
					in.put(da, sz);
					savedIn.put(da, saved);
					work.add(d);
				} else if (seen != sz && record && !problems.containsKey(f)) {
					problems.put(f, "paths meet with different SZ at " + da);
				}
			}
		}
		return exit == null ? entry : exit;
	}

	private int afterCall(Function g, int sz, boolean record) throws Exception {
		if (g == null || sz == UNK) {
			return sz;
		}
		if (record) {
			callerEntry.computeIfAbsent(g, k -> new HashSet<>()).add(sz);
		}
		return exitOf(g, sz);
	}

	private int exitOf(Function g, int entry) throws Exception {
		String key = g.getEntryPoint() + "/" + bit + "/" + entry;
		Integer s = summary.get(key);
		if (s != null) {
			return s;
		}
		if (!inProgress.add(key)) {
			return entry; // recursion: assume preserved
		}
		int r = analyze(g, entry, false);
		inProgress.remove(key);
		summary.put(key, r);
		return r;
	}

	static final Set<String> PR_DEPENDENT = new HashSet<>(java.util.Arrays.asList("fabs", "fadd", "fcmp/eq",
		"fcmp/gt", "fdiv", "fipr", "float", "fmac", "fmul", "fneg", "frchg", "fschg", "fsqrt", "fsrra", "fsca",
		"fsub", "ftrc", "ftrv"));

	private boolean dependsOnField(String m) {
		return isSz ? m.startsWith("fmov") : PR_DEPENDENT.contains(m);
	}

	@Override
	protected void run() throws Exception {
		model = new BasicBlockModel(currentProgram);
		List<Function> funcs = new ArrayList<>();
		FunctionIterator fi = currentProgram.getFunctionManager().getFunctions(true);
		while (fi.hasNext()) {
			funcs.add(fi.next());
		}
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "fpu_context.tsv");
		Map<Function, String> allProblems = new HashMap<>();
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("address\tfield\tkind\tdetail");
			for (String field : new String[] { "sz", "pr" }) {
				isSz = field.equals("sz");
				bit = isSz ? 20 : 19;
				blockInsns.clear(); // instructions change between fields
				solve(funcs, field, w);
				for (Map.Entry<Function, String> e : problems.entrySet()) {
					allProblems.merge(e.getKey(), field.toUpperCase() + ": " + e.getValue(), (a, b) -> a + "; " + b);
				}
			}
		}
		problems.clear();
		problems.putAll(allProblems);
		warn(funcs);
		println("FPU context done -> " + out);
	}

	private void solve(List<Function> funcs, String field, PrintWriter w) throws Exception {

		Map<Function, Integer> entry = new HashMap<>();
		Set<Function> entryConflict = new HashSet<>();
		for (int round = 0; round < 8; round++) {
			before.clear();
			problems.clear();
			callerEntry.clear();
			summary.clear();
			for (Function f : funcs) {
				analyze(f, entry.getOrDefault(f, 0), true);
			}
			boolean changed = false;
			entryConflict.clear();
			for (Map.Entry<Function, Set<Integer>> e : callerEntry.entrySet()) {
				Set<Integer> v = e.getValue();
				int want = v.size() == 1 ? v.iterator().next() : 0;
				if (v.size() > 1) {
					entryConflict.add(e.getKey());
				}
				if (entry.getOrDefault(e.getKey(), 0) != want) {
					entry.put(e.getKey(), want);
					changed = true;
				}
			}
			println(String.format("%s round %d: %d functions entered with %s=1, %d call-site conflicts", field,
				round, entry.values().stream().filter(x -> x == 1).count(), field.toUpperCase(), entryConflict.size()));
			if (!changed) {
				break;
			}
		}
		for (Function f : entryConflict) {
			problems.putIfAbsent(f, "call sites enter it with both SZ=0 and SZ=1");
		}

		// apply: instructions whose decoded context disagrees with the dataflow
		ProgramContext pc = currentProgram.getProgramContext();
		// context fields: sz, and fpu_pr (plain "pr" would clash with the PR register)
		Register szReg = currentProgram.getRegister(field.equals("pr") ? "fpu_pr" : field);
		int fixed = 0;
		{
			// repeat until every fmov matches: re-decoding a branch can change
			// how its delay slot decodes
			for (int pass = 0, n = -1; pass < 3 && n != 0; pass++) {
			n = 0;
			for (Map.Entry<Address, Integer> e : before.entrySet()) {
				int want = e.getValue();
				if (want == UNK) {
					continue;
				}
				Address a = e.getKey();
				Instruction i = getInstructionAt(a);
				if (i == null || !dependsOnField(mn(i))) {
					continue;
				}
				// the context this instruction was actually decoded with
				// (null: decoded with the default context, which is sz = 0)
				BigInteger have = i.getValue(szReg, false);
				if ((have == null ? 0 : have.intValue()) == want) {
					continue;
				}
				// a delay-slot instruction is decoded together with its branch
				Address from = i.isInDelaySlot() ? i.getPrevious().getAddress() : a;
				Address to = i.getMaxAddress();
				String old = i.toString();
				// context can only change where no instruction sits; a delay
				// slot is decoded with the context flowing from its branch, so
				// the branch gets its own computed value too
				clearListing(from, to);
				if (!from.equals(a)) {
					Integer bw = before.get(from);
					pc.setValue(szReg, from, a.previous(), BigInteger.valueOf(bw == null || bw == UNK ? want : bw));
				}
				pc.setValue(szReg, a, to, BigInteger.valueOf(want));
				new DisassembleCommand(from, new AddressSet(from, to), false).applyTo(currentProgram, monitor);
				Instruction ni = getInstructionAt(a);
				w.printf("%s\t%s\tredecoded\t%s -> %s (%s=%d)%n", a, field, old, ni == null ? "?" : ni.toString(),
					field.toUpperCase(), want);
				fixed++;
				n++;
			}
			println(field + " apply pass " + pass + ": " + n + " instructions re-decoded");
			}
			for (Map.Entry<Function, String> e : problems.entrySet()) {
				w.printf("%s\t%s\tproblem\t%s%n", e.getKey().getEntryPoint(), field, e.getValue());
			}
		}
		long on = entry.values().stream().filter(x -> x == 1).count();
		println(String.format("%s: %d instructions re-decoded, %d functions entered with %s=1, %d with problems",
			field, fixed, on, field.toUpperCase(), problems.size()));
	}

	// warnings on functions, replacing earlier ones
	private void warn(List<Function> funcs) {
		for (Function f : funcs) {
			String old = f.getComment() == null ? "" : f.getComment();
			String keep = old.replaceAll("(?m)^" + java.util.regex.Pattern.quote(MARK) + ".*\\n?", "").strip();
			String p = problems.get(f);
			if (p != null) {
				keep = (keep.isEmpty() ? "" : keep + "\n") + MARK + ": " + p
					+ "; instructions after that point decode as on the first path seen";
			}
			if (!keep.equals(old.strip())) {
				f.setComment(keep.isEmpty() ? null : keep);
			}
		}
	}
}
