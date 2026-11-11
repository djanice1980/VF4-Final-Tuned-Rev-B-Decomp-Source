// Commit parameter lists for the functions that have none.
//
// Without a stored prototype the decompiler shows, at each call, only the
// argument registers the caller writes before it, so an argument passed
// straight through (the caller's own parameter, still in r4) is lost:
// mktime called FUN_0c1597c0(t) but showed FUN_0c1597c0(). Ghidra's own
// Decompiler Parameter ID infers a function's parameters from its body
// alone, and measured over src/ it made things worse here: stub functions
// that ignore their argument lost it at every call (FUN_0c0ed780, a debug
// color call compiled out), code that saves fr8-fr11 for its caller looked
// like it read them (and the error spread to every caller), and returns
// came out wrong (vec3_length's fr0 committed as r0). So, using what was
// measured for this compiler (integer arguments r4-r7 in order, floats
// fr4-fr11 in order, the fifth integer argument on the stack):
//
//   callee   the parameters the decompiler infers from the function's body,
//            minus registers saved for the caller (pushed at entry and
//            popped back into the same register; some assembly reads its
//            arguments by pushing them and popping them elsewhere), widened
//            to a prefix (a function that reads r6 takes r4 and r5 too); with
//            stack parameters, all four integer registers and the stack
//            slots, when they are a short (at most 6) contiguous list;
//   callers  at every direct call, the argument registers written on every
//            path into it (back through up to three blocks, stopping at
//            calls and the function entry, plus the delay slot) and not read
//            again before the call: such a value has no use but the call,
//            since the call destroys r4-r7 / fr4-fr11, while one read on the
//            way is often scratch (18 of FUN_0c14a640's 30 callers build a
//            store pointer in r7); as a prefix, integers and floats apart;
//            the value at least half the call sites reach (a caller that
//            forwards its own parameter writes nothing, a stub's callers
//            all write);
//   result   per class the larger of the two: what the body uses, or what
//            the callers pass.
//
// Then the body's inputs are checked against the callers: a body can read
// a register only on a path that cannot run, and no caller passes it
// (FUN_0c0fd000; see shrinkUnsupplied).
//
// Returns are not committed; the decompiler keeps inferring them per call.
// Left alone: functions whose code runs into bytes that are not
// instructions (none since FlowOverAddressTables; a guard); functions with our own
// signatures (ApplyTypes); functions not
// on a 32-byte boundary (GCC aligns every function so here, 5,808 of
// 6,049; the rest are hand-written assembly with its own conventions, the
// Elan library at 0x0C104000-0x0C10E000 passing values in fr12 / fr15 and
// T); variadic ones (GCC's SH variadic prologue stores the unnamed
// argument registers for va_arg: see variadic()); functions whose
// inputs are outside r4-r7 / fr4-fr11, or whose stack reads are no short
// contiguous list (a struct passed by value: FUN_0c1479c0 reads 30 slots);
// and functions that would get no parameters (their callers keep showing
// what they pass). Callees come before their callers, in address order,
// one at a time, so the result is the same on every run. Every decision is
// written to ghidra/prototypes.tsv.
// @category VF4FT
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.block.CodeBlock;
import ghidra.program.model.block.CodeBlockReference;
import ghidra.program.model.block.CodeBlockReferenceIterator;
import ghidra.program.model.block.SimpleBlockModel;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.FloatDataType;
import ghidra.program.model.data.Undefined;
import ghidra.program.model.data.Undefined4DataType;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.ParameterImpl;
import ghidra.program.model.listing.Variable;
import ghidra.program.model.listing.VariableStorage;
import ghidra.program.model.pcode.HighFunction;
import ghidra.program.model.pcode.HighSymbol;
import ghidra.program.model.pcode.LocalSymbolMap;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

public class AssignPrototypes extends GhidraScript {

	private static final String[] INT = { "r4", "r5", "r6", "r7" };
	private static final String[] FLT = { "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11" };

	private final Map<String, Integer> why = new TreeMap<>();

	private final List<String> table = new ArrayList<>();

	private void count(String k) {
		why.merge(k, 1, Integer::sum);
	}

	/** A committed function whose body asks for more than its callers pass. */
	private static class Unsupplied {
		Function f;
		String cc;
		int ni, nf, si, sf, row;

		Unsupplied(Function f, String cc, int ni, int nf, int si, int sf, int row) {
			this.f = f;
			this.cc = cc;
			this.ni = ni;
			this.nf = nf;
			this.si = si;
			this.sf = sf;
			this.row = row;
		}
	}

	private final List<Unsupplied> unsupplied = new ArrayList<>();

	/** Does f set up GCC's frame pointer (mov r15,r14) before its first call or return? The
	 *  assembly hands values to the next routine in argument registers, so only GCC callers
	 *  say anything about what a function takes. */
	private boolean gccFrame(Function f) {
		Instruction i = getInstructionAt(f.getEntryPoint());
		for (int n = 0; i != null && n < 40 && f.getBody().contains(i.getAddress()); n++, i = i.getNext()) {
			if (i.toString().equals("mov r15,r14")) {
				return true;
			}
			String m = mnem(i);
			if (m.equals("rts") || m.equals("jsr") || m.equals("bsr")) {
				// the delay slot can hold it: GCC fills the first call's slot with the
				// frame setup (FUN_0c133c60: jsr @r1; mov r15,r14)
				Instruction slot = i.getNext();
				return slot != null && slot.toString().replaceFirst("^_", "").equals("mov r15,r14");
			}
		}
		return false;
	}

	private static final int REAL = 0, PASS = 1, UNSET = 2, LEFT = 3;

	/** What a call argument is: a value the caller computes, its own parameter passed on, a
	 *  value it never set (an input it does not have as a parameter, a register it never
	 *  wrote), or what an earlier call left (GCC never passes that: the call destroyed it). */
	private static int kind(ghidra.program.model.pcode.Varnode v) {
		ghidra.program.model.pcode.HighVariable h = v == null ? null : v.getHigh();
		String n = h == null ? null : h.getName();
		if (h instanceof ghidra.program.model.pcode.HighParam) {
			return PASS;
		}
		if (n != null && n.startsWith("extraout_")) {
			return LEFT;
		}
		if (n != null && (n.startsWith("in_") || n.startsWith("unaff_"))) {
			return UNSET;
		}
		return REAL;
	}

	/** Is parameter k of hf used in the body? An INDIRECT is the decompiler noting that a call
	 *  might change the register, not a use. */
	private static boolean used(HighFunction hf, int k) {
		if (k >= hf.getLocalSymbolMap().getNumParams()) {
			return false;
		}
		ghidra.program.model.pcode.HighVariable h = hf.getLocalSymbolMap().getParamSymbol(k).getHighVariable();
		if (h == null) {
			return false;
		}
		for (ghidra.program.model.pcode.Varnode v : h.getInstances()) {
			java.util.Iterator<ghidra.program.model.pcode.PcodeOp> it = v.getDescendants();
			while (it.hasNext()) {
				if (it.next().getOpcode() != ghidra.program.model.pcode.PcodeOp.INDIRECT) {
					return true;
				}
			}
		}
		return false;
	}

	private void commit(Unsupplied u, int ni, int nf, String why) throws Exception {
		List<Variable> ps = new ArrayList<>();
		for (int i = 0; i < ni; i++) {
			ps.add(new ParameterImpl("param_" + (ps.size() + 1), Undefined4DataType.dataType,
				new VariableStorage(currentProgram, currentProgram.getRegister(INT[i])), currentProgram));
		}
		for (int j = 0; j < nf; j++) {
			ps.add(new ParameterImpl("param_" + (ps.size() + 1), FloatDataType.dataType,
				new VariableStorage(currentProgram, currentProgram.getRegister(FLT[j])), currentProgram));
		}
		u.f.updateFunction(u.cc, null, ps, Function.FunctionUpdateType.CUSTOM_STORAGE, true, SourceType.ANALYSIS);
		String[] row = table.get(u.row).split("\\t", -1);
		row[2] = ni + "/" + nf + " (cut from " + row[2].replaceAll(" .*", "") + ": " + why + ")";
		table.set(u.row, String.join("\t", row));
		u.ni = ni;
		u.nf = nf;
	}

	/** The body can read a register only on a path that cannot run (FUN_0c0fd000 sets r7 when
	 *  r5 != -2 and reads it after a second, identical test) and then asks for a parameter no
	 *  caller passes. With every prototype committed, decompile the GCC callers of each
	 *  function whose body asks for more than its callers write, and look at the arguments past
	 *  what they write (integers and floats apart): cut them when no caller computes a value
	 *  there and either none passes anything at all, or one passes what an earlier call left
	 *  (proof, since GCC never does; then a caller handing on its own parameter is a wrapper that
	 *  only took it because of this function: model_size calls FUN_0c0fd000 with the r6 and r7
	 *  its previous call left, while FUN_0c0fc9a0 -> FUN_0c0fca60 -> FUN_0c0fcca0 pass theirs
	 *  through). Then a function left with trailing parameters its body no longer uses and
	 *  no caller writes is cut too (those wrappers), and it all repeats until nothing changes.
	 *  Returns how many cuts were made. */
	private int shrinkUnsupplied(DecompInterface d) throws Exception {
		int cut = 0;
		for (int round = 0; round < 8; round++) {
			Map<Function, HighFunction> cache = new HashMap<>();
			java.util.function.Function<Function, HighFunction> dec = c -> cache.computeIfAbsent(c, x -> {
				DecompileResults r = d.decompileFunction(x, 60, monitor);
				return r.getHighFunction();
			});
			boolean changed = false;
			for (Unsupplied u : unsupplied) {
				monitor.checkCancelled();
				if (u.ni <= u.si && u.nf <= u.sf) {
					continue;
				}
				// 1. the callers' arguments
				int[] ints = new int[4], flts = new int[4]; // counts of REAL, PASS, UNSET, LEFT
				boolean any = false;
				for (Reference ref : getReferencesTo(u.f.getEntryPoint())) {
					if (!ref.getReferenceType().isCall()) {
						continue;
					}
					Function caller = getFunctionContaining(ref.getFromAddress());
					if (caller == null || !gccFrame(caller)) {
						continue;
					}
					HighFunction hf = dec.apply(caller);
					boolean found = false;
					if (hf != null) {
						java.util.Iterator<ghidra.program.model.pcode.PcodeOpAST> it = hf.getPcodeOps(ref.getFromAddress());
						while (it.hasNext()) {
							ghidra.program.model.pcode.PcodeOpAST op = it.next();
							if (op.getOpcode() != ghidra.program.model.pcode.PcodeOp.CALL
								|| !op.getInput(0).getAddress().equals(u.f.getEntryPoint())) {
								continue;
							}
							found = true;
							for (int k = u.si; k < u.ni; k++) {
								ints[op.getNumInputs() > 1 + k ? kind(op.getInput(1 + k)) : REAL]++;
							}
							for (int j = u.sf; j < u.nf; j++) {
								flts[op.getNumInputs() > 1 + u.ni + j ? kind(op.getInput(1 + u.ni + j)) : REAL]++;
							}
						}
					}
					if (!found) {
						ints[REAL]++; // a call the decompiler does not show: no evidence
						flts[REAL]++;
					}
					any = true;
				}
				boolean cutInts = u.ni > u.si && any && ints[REAL] == 0 && (ints[PASS] == 0 || ints[LEFT] > 0);
				boolean cutFlts = u.nf > u.sf && any && flts[REAL] == 0 && (flts[PASS] == 0 || flts[LEFT] > 0);
				// 2. or: its own trailing parameters are no longer used (a wrapper of a cut function)
				String why = "no caller passes the rest";
				if (!cutInts && !cutFlts) {
					HighFunction self = dec.apply(u.f);
					if (self != null) {
						// back to the last parameter the body uses, never below what callers write
						int ki = u.ni, kf = u.nf;
						while (ki > u.si && !used(self, ki - 1)) {
							ki--;
						}
						while (kf > u.sf && !used(self, u.ni + kf - 1)) {
							kf--;
						}
						if (ki < u.ni || kf < u.nf) {
							commit(u, ki, kf, "unused, and no caller writes them");
							cut++;
							changed = true;
							cache.clear();
						}
					}
					continue;
				}
				commit(u, cutInts ? u.si : u.ni, cutFlts ? u.sf : u.nf, why);
				cut++;
				changed = true;
				cache.clear(); // the cut changes what its callers and itself decompile to
			}
			if (!changed) {
				break;
			}
		}
		return cut;
	}

	private void record(Function f, String decision, String body, String callers, int sites) {
		table.add(String.format("%s\t%s\t%s\t%s\t%s\t%d", f.getEntryPoint(), f.getName(), decision, body, callers, sites));
	}

	private static String mnem(Instruction i) {
		return i.getMnemonicString().replaceFirst("^_", "");
	}

	/**
	 * For a variadic function, how many arguments are named: { int, float };
	 * null when it is not variadic. GCC 3.2's prologue sets up a frame
	 * (mov r15,r14), stores the unnamed int argument registers into it (r7
	 * down), then the unnamed float ones from fr11 down, one by one through a
	 * plain @rN: fatal_at (file, line, format, ...) stores r7 and fr11-fr4;
	 * (char *, float, float, ...) stores r5-r7 and fr11-fr6 (checked by
	 * compiling both). Float stores in any other order or through @-rN are
	 * something else (assembly writing a matrix out through r4, a function
	 * copying its float arguments into a global).
	 */
	private int[] variadic(Function f) {
		Set<Integer> savedInt = new HashSet<>();
		boolean frame = false;
		int next = 11, run = 0;
		Instruction ins = getInstructionAt(f.getEntryPoint());
		for (int n = 0; ins != null && n < 60; n++, ins = ins.getNext()) {
			String m = mnem(ins);
			if (m.equals("rts")) {
				break;
			}
			if (ins.getNumOperands() != 2) {
				continue;
			}
			String src = ins.getDefaultOperandRepresentation(0), dst = ins.getDefaultOperandRepresentation(1);
			if (m.equals("mov") && src.equals("r15") && dst.equals("r14")) {
				frame = true;
			} else if (m.startsWith("fmov") && src.matches("fr([0-9]|1[0-5])") && dst.startsWith("@")) {
				if (!frame || dst.startsWith("@-") || Integer.parseInt(src.substring(2)) != next) {
					break; // not the save area (or past its end)
				}
				run++;
				if (next-- == 4) {
					break;
				}
			} else if (run == 0 && frame && m.equals("mov.l") && src.matches("r[4-7]") && dst.matches("@\\(.*,r14\\)")) {
				savedInt.add(Integer.parseInt(src.substring(1)));
			}
		}
		if (run < 4) {
			return null;
		}
		return new int[] { 4 - savedInt.size(), 8 - run };
	}

	/** Does some instruction fall through into bytes that are not an instruction? */
	private boolean cutShort(Function f) {
		for (Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) {
			if (i.getDelaySlotDepth() > 0) {
				continue; // judged at its slot
			}
			if (i.isInDelaySlot()) {
				Instruction owner = i.getPrevious();
				if (owner == null || !owner.getFlowType().hasFallthrough()) {
					continue; // rts, bra, jmp
				}
			}
			else if (!i.getFlowType().hasFallthrough()) {
				continue;
			}
			if (getInstructionAt(i.getMaxAddress().add(1)) == null) {
				return true;
			}
		}
		return false;
	}

	/** Registers pushed (mov.l Rn,@-r15 / fmov FRn,@-r15 / DRn pairs) at entry and popped
	 *  back into the same register later: saved for the caller, not inputs. A register
	 *  pushed and popped into another one is how some assembly reads an argument
	 *  (FUN_0c12d720 pushes fr4-fr6 and pops them into fr15); when no pop goes into a
	 *  register that was not pushed, every entry push is a save. */
	private Set<String> saved(Function f) {
		Set<String> pushed = new HashSet<>();
		Set<String> out = pushed;
		Instruction ins = getInstructionAt(f.getEntryPoint());
		for (int n = 0; ins != null && n < 24; n++, ins = ins.getNext()) {
			String m = mnem(ins);
			if (m.equals("fschg")) {
				continue;
			}
			if (ins.getNumOperands() != 2 || !ins.getDefaultOperandRepresentation(1).equals("@-r15")) {
				break;
			}
			String r = ins.getDefaultOperandRepresentation(0);
			if (r.startsWith("dr") || r.startsWith("xd")) {
				int k = Integer.parseInt(r.substring(2));
				out.add("fr" + k);
				out.add("fr" + (k + 1));
			}
			else {
				out.add(r);
			}
		}
		Set<String> popped = new HashSet<>();
		for (Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) {
			if (i.getNumOperands() == 2 && i.getDefaultOperandRepresentation(0).equals("@r15+")) {
				String r = i.getDefaultOperandRepresentation(1);
				if (r.startsWith("dr") || r.startsWith("xd")) {
					int k = Integer.parseInt(r.substring(2));
					popped.add("fr" + k);
					popped.add("fr" + (k + 1));
				}
				else {
					popped.add(r);
				}
			}
		}
		Set<String> elsewhere = new HashSet<>(popped);
		elsewhere.removeAll(pushed);
		if (!elsewhere.isEmpty()) {
			pushed.retainAll(popped); // a pushed value lands in another register
		}
		// otherwise every entry push is a save, also one this body never pops: FUN_0c04f720
		// saves r8-r14 and fr8-fr15 and leaves by jmp @r0 to an epilogue outside its body
		return pushed;
	}

	private static int index(String[] names, String reg) {
		for (int i = 0; i < names.length; i++) {
			if (names[i].equals(reg)) {
				return i;
			}
		}
		return -1;
	}

	/** Registers written by an instruction, fr pairs split. */
	private static Set<String> written(Instruction ins) {
		return regs(ins.getResultObjects());
	}

	/** Registers read by an instruction, fr pairs split. */
	private static Set<String> read(Instruction ins) {
		return regs(ins.getInputObjects());
	}

	private static Set<String> regs(Object[] objs) {
		Set<String> out = new HashSet<>();
		for (Object o : objs) {
			if (o instanceof Register) {
				String n = ((Register) o).getName();
				if (n.startsWith("dr") || n.startsWith("xd")) {
					int k = Integer.parseInt(n.substring(2));
					out.add("fr" + k);
					out.add("fr" + (k + 1));
				}
				else {
					out.add(n);
				}
			}
		}
		return out;
	}

	private SimpleBlockModel blocks;

	/** Apply one instruction: a register it reads stops counting (its value had another use),
	 *  then the ones it writes count. */
	private static void step(Instruction ins, Set<String> w, Set<String> readFirst, Set<String> seen) {
		for (String r : read(ins)) {
			w.remove(r);
			if (readFirst != null && !seen.contains(r)) {
				readFirst.add(r);
			}
		}
		Set<String> out = written(ins);
		w.addAll(out);
		seen.addAll(out);
	}

	/** Registers written in [start, limit) of a block after its last call (a call's delay slot
	 *  feeds that call) whose value nothing has read since; null limit = the whole block.
	 *  calls[0] is set if a call was seen; readFirst gets the registers read here before
	 *  being written here (a value from an earlier block that this one uses). */
	private Set<String> scan(CodeBlock b, Address limit, boolean[] calls, Set<String> readFirst) {
		Set<String> w = new HashSet<>(), seen = new HashSet<>();
		Instruction ins = getInstructionAt(b.getMinAddress());
		while (ins != null && b.contains(ins.getAddress()) && (limit == null || ins.getAddress().compareTo(limit) < 0)) {
			if (ins.getFlowType().isCall()) {
				w.clear();
				calls[0] = true;
				readFirst = null; // before this call: another call's arguments
				if (ins.getDelaySlotDepth() > 0) {
					ins = ins.getNext(); // the slot runs before this call: its writes are this call's
				}
			}
			else {
				step(ins, w, readFirst, seen);
			}
			ins = ins == null ? null : ins.getNext();
		}
		return w;
	}

	/** Registers written on every path into the block before limit, and not read after that,
	 *  looking back up to depth blocks and stopping at calls and at the function entry. */
	private Set<String> mustWritten(CodeBlock b, Address limit, int depth, Set<Address> path) throws Exception {
		boolean[] calls = { false };
		Set<String> readFirst = new HashSet<>();
		Set<String> w = scan(b, limit, calls, readFirst);
		if (calls[0] || depth == 0 || !path.add(b.getMinAddress())) {
			return w;
		}
		Set<String> all = null;
		CodeBlockReferenceIterator it = b.getSources(monitor);
		boolean any = false;
		while (it.hasNext()) {
			CodeBlockReference ref = it.next();
			if (ref.getFlowType().isCall()) {
				continue;
			}
			any = true;
			Set<String> p = mustWritten(ref.getSourceBlock(), null, depth - 1, new HashSet<>(path));
			if (all == null) {
				all = p;
			}
			else {
				all.retainAll(p);
			}
		}
		if (any && all != null) {
			all.removeAll(readFirst);
			w.addAll(all);
		}
		return w;
	}

	/** How many (int, float) argument registers a call passes: up to the last one written on every path to it. */
	private int[] callSite(Address at) throws Exception {
		Instruction call = getInstructionAt(at);
		if (call == null) {
			return null;
		}
		CodeBlock b = blocks.getFirstCodeBlockContaining(at, monitor);
		if (b == null) {
			return null;
		}
		Set<String> w = mustWritten(b, at, 3, new HashSet<>());
		if (call.getDelaySlotDepth() > 0 && call.getNext() != null) {
			step(call.getNext(), w, null, new HashSet<>()); // the delay slot runs before the callee
		}
		// up to the last argument register written: one written after a run of
		// unwritten ones still counts them (a caller forwarding its own r4-r6
		// and setting r7 passes four: FUN_0c12ff00's mov #1,r7 to FUN_0c12fcc0)
		int ni = 0, nf = 0;
		for (int k = 0; k < INT.length; k++) {
			if (w.contains(INT[k])) {
				ni = k + 1;
			}
		}
		for (int k = 0; k < FLT.length; k++) {
			if (w.contains(FLT[k])) {
				nf = k + 1;
			}
		}
		return new int[] { ni, nf };
	}

	/** The largest k that at least half of the values reach. */
	private static int majority(List<Integer> v) {
		if (v.isEmpty()) {
			return 0;
		}
		List<Integer> d = new ArrayList<>(v);
		d.sort(java.util.Collections.reverseOrder());
		return d.get((d.size() + 1) / 2 - 1);
	}

	/** The argument register (r4-r7, fr4-fr11) a storage lies in, or null. */
	private String argRegister(VariableStorage st) {
		Address a = st.getMinAddress();
		for (String[] set : new String[][] { INT, FLT }) {
			for (String n : set) {
				Register r = currentProgram.getRegister(n);
				Address b = r.getAddress();
				if (a.getAddressSpace().equals(b.getAddressSpace()) && a.getOffset() >= b.getOffset()
					&& a.getOffset() < b.getOffset() + r.getMinimumByteSize()) {
					return n;
				}
			}
		}
		return null;
	}

	private boolean hasFlowIn(Address a) {
		for (Reference r : getReferencesTo(a)) {
			if (r.getReferenceType().isFlow() && !r.getReferenceType().isCall()) {
				return true;
			}
		}
		return false;
	}

	private void order(Function f, Set<Function> seen, List<Function> out) throws Exception {
		if (!seen.add(f)) {
			return;
		}
		TreeMap<Address, Function> callees = new TreeMap<>();
		for (Function c : f.getCalledFunctions(monitor)) {
			callees.put(c.getEntryPoint(), c);
		}
		for (Function c : callees.values()) {
			order(c, seen, out);
		}
		out.add(f);
	}

	@Override
	protected void run() throws Exception {
		List<Function> todo = new ArrayList<>();
		Set<Function> seen = new HashSet<>();
		for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
			order(f, seen, todo);
		}
		DecompInterface d = new DecompInterface();
		DecompileOptions opts = new DecompileOptions();
		opts.grabFromProgram(currentProgram);
		d.setOptions(opts);
		d.openProgram(currentProgram);
		int committed = 0, ints = 0, flts = 0, fromCallers = 0, over = 0;
		blocks = new SimpleBlockModel(currentProgram);
		Map<String, String> sample = new LinkedHashMap<>();
		for (Function f : todo) {
			monitor.checkCancelled();
			if (f.isThunk() || f.getSignatureSource() != SourceType.DEFAULT) {
				count("own signature");
				continue;
			}
			if ((f.getEntryPoint().getOffset() & 31) != 0) {
				// GCC starts every function on a 32-byte boundary here (5,808 of 6,049);
				// the rest are hand-written assembly or entry points inside it, with their
				// own register conventions (fr12 / fr15 and T as arguments in the Elan
				// library at 0x0C104000-0x0C10E000)
				count("not 32-byte aligned (assembly)");
				record(f, "skipped: not 32-byte aligned (assembly)", "-", "-", 0);
				continue;
			}
			if (cutShort(f)) {
				// its code runs on into bytes that are not instructions (data defined on top of
				// code: FUN_0c04f720 stops after six pushes, before the ones of fr8-fr15 and
				// every pop), so its inputs cannot be read from its body
				count("body runs into non-code");
				record(f, "skipped: body runs into non-code", "-", "-", 0);
				continue;
			}
			int[] named = variadic(f);
			if (named != null) {
				// no prototype for Ghidra (a "..." one drops the register varargs at
				// every call); the header writer (ExportSource) declares it from this
				count("variadic");
				record(f, "variadic: " + named[0] + " named int, " + named[1] + " named float", "-", "-", 0);
				String c = f.getComment() == null ? "" : f.getComment().replaceAll("(?m)^variadic: .*\\n?", "").strip();
				f.setComment((c.isEmpty() ? "" : c + "\n") + "variadic: " + named[0] + " named int, "
					+ named[1] + " named float arguments (from the registers its prologue stores)");
				continue;
			}
			DecompileResults r = d.decompileFunction(f, 60, monitor);
			HighFunction hf = r.getHighFunction();
			if (hf == null) {
				count("did not decompile");
				continue;
			}
			Set<String> pushed = saved(f);
			LocalSymbolMap map = hf.getLocalSymbolMap();
			int ci = 0, cf = 0;
			boolean odd = false;
			TreeMap<Long, VariableStorage> stack = new TreeMap<>();
			for (int k = 0; k < map.getNumParams(); k++) {
				HighSymbol p = map.getParamSymbol(k);
				VariableStorage st = p.getStorage();
				if (st.isStackStorage()) {
					stack.put((long) st.getStackOffset(), st);
					continue;
				}
				if (!st.isRegisterStorage()) {
					odd = true;
					continue;
				}
				String n = argRegister(st);
				if (n == null) {
					Register reg = st.getRegister();
					if (reg != null && pushed.contains(reg.getName())) {
						continue; // a saved register read as an input (non-GCC prologue)
					}
					odd = true;
					continue;
				}
				if (pushed.contains(n)) {
					continue;
				}
				int i = index(INT, n), j = index(FLT, n);
				if (i >= 0) {
					ci = Math.max(ci, i + 1);
				}
				else if (j >= 0) {
					cf = Math.max(cf, j + 1);
				}
				else {
					odd = true;
				}
			}
			if (!stack.isEmpty()) {
				// GCC passes the fifth integer argument on: a few 4-byte slots, contiguous; a
				// longer or gapped list is the body reading its caller's frame (a struct passed
				// by value, FUN_0c1479c0 reads 30 slots), not arguments: leave such functions
				long first = stack.firstKey(), next = first;
				boolean ok = stack.size() <= 6;
				for (Map.Entry<Long, VariableStorage> e : stack.entrySet()) {
					ok &= e.getKey() == next && e.getValue().size() <= 4;
					next = e.getKey() + 4;
				}
				if (!ok) {
					count("stack parameters not a short contiguous list");
					record(f, "skipped: stack reads not a short contiguous list", stack.size() + " stack", "-", 0);
					continue;
				}
				ci = INT.length; // stack arguments come after all four integer registers
				count("stack parameters");
			}
			if (odd) {
				count("other registers");
				record(f, "skipped: inputs outside r4-r7 / fr4-fr11", "-", "-", 0);
				continue;
			}
			List<Integer> vi = new ArrayList<>(), vf = new ArrayList<>();
			for (Reference ref : getReferencesTo(f.getEntryPoint())) {
				if (!ref.getReferenceType().isCall()) {
					continue;
				}
				int[] st = callSite(ref.getFromAddress());
				if (st != null) {
					vi.add(st[0]);
					vf.add(st[1]);
				}
			}
			int sites = vi.size(), si = majority(vi), sf = majority(vf);
			int ni = Math.max(ci, si), nf = Math.max(cf, sf);
			if (ni == 0 && nf == 0) {
				count("no parameters");
				record(f, "none (callers keep what they pass)", ci + "/" + cf, si + "/" + sf, sites);
				continue;
			}
			if (ni > ci || nf > cf) {
				fromCallers++;
			}
			List<Variable> ps = new ArrayList<>();
			for (int i = 0; i < ni; i++) {
				ps.add(new ParameterImpl("param_" + (ps.size() + 1), Undefined4DataType.dataType,
					new VariableStorage(currentProgram, currentProgram.getRegister(INT[i])), currentProgram));
			}
			for (VariableStorage st : stack.values()) {
				ps.add(new ParameterImpl("param_" + (ps.size() + 1), Undefined.getUndefinedDataType(st.size()), st, currentProgram));
			}
			for (int j = 0; j < nf; j++) {
				DataType t = FloatDataType.dataType;
				ps.add(new ParameterImpl("param_" + (ps.size() + 1), t,
					new VariableStorage(currentProgram, currentProgram.getRegister(FLT[j])), currentProgram));
			}
			for (int k = 0; k < sites; k++) {
				if (vi.get(k) > ni || vf.get(k) > nf) {
					over++;
				}
			}
			// keep a convention AssignCallEffects chose (what the function's calls destroy);
			// a function without one gets the default
			String cc = f.getCallingConventionName();
			if (cc.equals(Function.UNKNOWN_CALLING_CONVENTION_STRING) || cc.equals(Function.DEFAULT_CALLING_CONVENTION_STRING)) {
				cc = "__stdcall";
			}
			f.updateFunction(cc, null, ps, Function.FunctionUpdateType.CUSTOM_STORAGE, true,
				SourceType.ANALYSIS);
			committed++;
			record(f, ni + "/" + nf + (stack.isEmpty() ? "" : "+" + stack.size() + " stack"), ci + "/" + cf, si + "/" + sf, sites);
			if (stack.isEmpty() && (ni > si || nf > sf)) {
				unsupplied.add(new Unsupplied(f, cc, ni, nf, si, sf, table.size() - 1));
			}
			ints += ni;
			flts += nf;
			if (sample.size() < 8 && ni + nf > 0) {
				sample.put(f.getEntryPoint().toString(), ni + " int, " + nf + " float (body " + ci + "/" + cf
					+ ", callers " + (sites == 0 ? "-" : si + "/" + sf) + ")");
			}
		}
		int shrunk = shrinkUnsupplied(d);
		d.dispose();
		java.io.File out = new java.io.File(new java.io.File(getSourceFile().getParentFile().getAbsolutePath()), "prototypes.tsv");
		try (java.io.PrintWriter w = new java.io.PrintWriter(out, "UTF-8")) {
			w.println("function\tname\tprototype (int/float)\tbody (int/float)\tcallers (int/float, majority)\tcall sites");
			table.sort(null);
			for (String line : table) {
				w.println(line);
			}
		}
		println(String.format("prototypes: %d committed (%d integer, %d float parameters; %d widened by the callers; %d call sites write more than the result; %d cut back to what every caller passes); skipped %s",
			committed, ints, flts, fromCallers, over, shrunk, why));
		println("prototypes: e.g. " + sample);
	}
}
