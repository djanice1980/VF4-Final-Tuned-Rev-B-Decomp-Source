// Return types from the call register. GCC 3.2 for SH loads a call's
// address into r0 when the callee is declared to return an integer or a
// pointer (char, short, int, long long, any pointer), or is not declared at
// all (so implicitly int), and into r1 when it returns nothing in r0: void,
// float, double or a struct (checked by compiling each). So each call in
// GCC-built code records what its file's declaration said. A call counts when the address is loaded
// (mov.l @(disp,pc),rN) at most seven instructions before jsr @r0 / @r1 in a
// caller with GCC's frame setup (assembly left out); an address kept in
// another register says nothing (about 5% of r0/r1 calls disagree with a
// known declaration, where r0 was busy).
//
// An r1 call where r0 is read or written between the address load and the
// call (delay slot included) is not counted: GCC could not have used r0.
// A caller that reads r0 after the call (before writing it) shows a value
// comes back whatever the register: r0 can be busy with an argument after
// the address is loaded, so GCC takes r1 (that made three "void" functions
// whose callers use the result). Such functions are never set void.
// A caller through r1 that reads fr0 after the call shows a float comes back
// (nothing in r0, so the call register cannot tell it from void): with no
// caller through r0 or reading r0, the function gets a float in fr0.
// A caller that reads r0 after the call counts as a value on its own, with
// any number of calls. A function without a committed signature is given a
// value only when the decompiler finds it no parameters ("(void)" loses no
// caller's arguments). A function that returns the result of a call passes
// the value on: that callee is given a value too.
// A function whose calls all agree (at least two) is compared with the
// return the decompiler infers for it: callers all r1 but an integer
// inferred in r0 -> void (nothing else in r1's set comes back in r0);
// callers all r0 but void inferred -> an integer in r0 ("undefined4": the
// callers say an integer or pointer comes back, not which). Anything else
// already agrees; when the signature is not committed, the decompiler's
// parameters and return are committed as it infers them now (left to it,
// it infers again at export and can disagree with the callers), except an
// 8-byte return:
// the decompiler can infer one in the FPU pair (fr0/fr1), which exports as
// undefined8, an unsigned long long that GCC returns in r0/r1 and calls
// through r0. With callers all through r1 such a function is given a float
// in fr0 (-m4-single-only: no wider FPU value), or void when the decompiler
// finds nothing in the FPU. A function without a committed signature (one
// AssignPrototypes left to the decompiler) gets the decompiler's parameters
// committed with its return, so no caller loses arguments; variadic ones
// are only reported. Our own signatures (ghidra/symbols/) are only
// reported. Every function with a vote is written to ghidra/returns.tsv.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.Undefined4DataType;
import ghidra.program.model.data.VoidDataType;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Parameter;
import ghidra.program.model.listing.ReturnParameterImpl;
import ghidra.program.model.listing.VariableStorage;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;

public class AssignReturns extends GhidraScript {

	private static String mnem(Instruction i) {
		return i.getMnemonicString().replaceFirst("^_", "");
	}

	/** Does f set up GCC's frame pointer (mov r15,r14) before its first call or return? */
	private boolean gccFrame(Function f) {
		Instruction i = getInstructionAt(f.getEntryPoint());
		for (int n = 0; i != null && n < 24; n++, i = i.getNext()) {
			String m = mnem(i);
			if (m.equals("mov") && i.getNumOperands() == 2 && i.getDefaultOperandRepresentation(0).equals("r15")
				&& i.getDefaultOperandRepresentation(1).equals("r14")) {
				return true;
			}
			if (m.equals("jsr") || m.equals("bsr") || m.equals("rts")) {
				// the delay slot can hold it: GCC fills the first call's slot with the
				// frame setup (FUN_0c133c60: jsr @r1; mov r15,r14)
				Instruction slot = i.getNext();
				return slot != null && slot.toString().replaceFirst("^_", "").equals("mov r15,r14");
			}
		}
		return false;
	}

	private Instruction load; // the pool load callee() found

	/** Is r0 read or written between the address load and the call, delay slot included? */
	private boolean r0Busy(Instruction jsr) {
		ghidra.program.model.lang.Register r0 = currentProgram.getRegister("r0");
		Instruction slot = jsr.getNext();
		for (Instruction i = load.getNext(); i != null; i = i.getNext()) {
			if (!i.equals(jsr)) {
				for (Object o : i.getInputObjects()) {
					if (r0.equals(o)) {
						return true;
					}
				}
				for (Object o : i.getResultObjects()) {
					if (r0.equals(o)) {
						return true;
					}
				}
			}
			if (i.equals(slot) || i.getAddress().compareTo(slot.getAddress()) > 0) {
				return false;
			}
		}
		return false;
	}

	/** The function a jsr @rN calls, when rN was loaded from the constant pool just before. */
	private Function callee(Instruction jsr, String reg) {
		Instruction i = jsr.getPrevious();
		for (int n = 0; i != null && n < 7; n++, i = i.getPrevious()) {
			if (i.getNumOperands() == 2 && i.getDefaultOperandRepresentation(1).equals(reg)) {
				// Ghidra shows mov.l @(disp,pc),rN as the pool address it reads
				if (!mnem(i).equals("mov.l") || !i.getDefaultOperandRepresentation(0).matches("0x[0-9a-fA-F]+")) {
					return null; // rN came from something else
				}
				load = i;
				for (Reference r : i.getReferencesFrom()) {
					if (r.getToAddress().isMemoryAddress()) {
						try {
							Address t = toAddr(getInt(r.getToAddress()) & 0xFFFFFFFFL);
							return getFunctionAt(t);
						} catch (Exception e) {
							return null;
						}
					}
				}
				return null;
			}
		}
		return null;
	}

	private ghidra.app.decompiler.DecompInterface dec;

	/** Does a value of this type come back in r0 (an integer, enum or pointer)? */
	private static boolean inR0(DataType t) {
		if (t instanceof ghidra.program.model.data.TypeDef) {
			t = ((ghidra.program.model.data.TypeDef) t).getBaseDataType();
		}
		return !(t instanceof VoidDataType || t instanceof ghidra.program.model.data.AbstractFloatDataType
			|| t instanceof ghidra.program.model.data.Composite);
	}

	private int inferredLength; // the size of the return inferred() last found

	/** "r0", "fr0", "void" or another storage, as the decompiler infers f's return. */
	private String inferred(Function f) {
		inferredLength = 0;
		ghidra.app.decompiler.DecompileResults r = dec.decompileFunction(f, 60, monitor);
		if (r.getHighFunction() == null) {
			return "?";
		}
		ghidra.program.model.pcode.FunctionPrototype p = r.getHighFunction().getFunctionPrototype();
		if (p.getReturnType() == null || p.getReturnType() instanceof VoidDataType) {
			return "void";
		}
		VariableStorage st = p.getReturnStorage();
		inferredLength = p.getReturnType().getLength();
		return st == null || st.isUnassignedStorage() ? "void" : st.toString().replaceFirst(":.*", "");
	}

	/**
	 * Does the caller read r0 after this call, before writing it? Then a value
	 * comes back whatever register the address went through (r0 can be busy
	 * with an argument after the address is loaded, so GCC takes r1).
	 */
	private boolean readsR0After(Instruction jsr) {
		return readsAfter(jsr, "r0");
	}

	/** Does the caller read reg after this call, before writing it? */
	private boolean readsAfter(Instruction jsr, String reg) {
		ghidra.program.model.lang.Register r0 = currentProgram.getRegister(reg);
		Instruction i = jsr.getNext(); // the delay slot runs before the call
		if (i == null) {
			return false;
		}
		i = i.getNext();
		for (int n = 0; i != null && n < 16; n++, i = i.getNext()) {
			for (Object o : i.getInputObjects()) {
				if (r0.equals(o)) {
					return true;
				}
			}
			for (Object o : i.getResultObjects()) {
				if (r0.equals(o)) {
					return false;
				}
			}
			if (!i.getFlowType().isFallthrough() || i.getFlowType().isCall()) {
				return false; // end of the straight line after the call
			}
		}
		return false;
	}

	private final java.util.Set<Function> valued = new java.util.LinkedHashSet<>();

	/** Does the decompiler find no parameters for f (so committing "(void)" loses nothing)? */
	private boolean noParameters(Function f) {
		ghidra.app.decompiler.DecompileResults r = dec.decompileFunction(f, 60, monitor);
		return r.getHighFunction() != null && r.getHighFunction().getFunctionPrototype().getNumParams() == 0;
	}

	private void setReturn(Function f, boolean toVoid) throws Exception {
		setReturn(f, toVoid ? "void" : "value");
	}

	/** Nothing in r0 per the callers: does the decompiler's return agree (void, or a 4-byte float)? */
	private boolean agreesNothingInR0(Function f) {
		String st = inferred(f);
		return st.equals("void") || (st.startsWith("fr") && inferredLength == 4);
	}

	/** Variadic: Ghidra's own flag, a "variadic:" note from AssignPrototypes, or "..." in a symbol row's name. */
	private boolean variadic(Function f) {
		ghidra.app.decompiler.DecompileResults r = dec.decompileFunction(f, 60, monitor);
		return f.hasVarArgs() || (f.getComment() != null && f.getComment().contains("variadic"))
			|| (r.getHighFunction() != null && r.getHighFunction().getFunctionPrototype().isVarArg())
			|| java.util.Set.of("printf", "sprintf", "snprintf", "fprintf", "vsprintf", "sscanf").contains(f.getName());
	}

	/** Commit the parameters and the return the decompiler finds; a return
	 *  narrower than a register (bool, char) only as parameters: committed,
	 *  it makes every caller that uses r0 whole decompile to CONCAT31(...),
	 *  which took 110 functions out of matching. */
	private void commitAll(Function f) throws Exception {
		ghidra.app.decompiler.DecompileResults r = dec.decompileFunction(f, 60, monitor);
		if (r.getHighFunction() != null) {
			DataType rt = r.getHighFunction().getFunctionPrototype().getReturnType();
			boolean narrow = rt != null && !(rt instanceof VoidDataType) && rt.getLength() > 0 && rt.getLength() < 4;
			ghidra.program.model.pcode.HighFunctionDBUtil.commitParamsToDatabase(r.getHighFunction(), true,
				narrow ? ghidra.program.model.pcode.HighFunctionDBUtil.ReturnCommitOption.NO_COMMIT
					: ghidra.program.model.pcode.HighFunctionDBUtil.ReturnCommitOption.COMMIT, SourceType.ANALYSIS);
		}
	}

	/** Commit the parameters the decompiler finds, so a return can be set without losing them. */
	private void commitParameters(Function f) throws Exception {
		ghidra.app.decompiler.DecompileResults r = dec.decompileFunction(f, 60, monitor);
		if (r.getHighFunction() != null) {
			ghidra.program.model.pcode.HighFunctionDBUtil.commitParamsToDatabase(r.getHighFunction(), true,
				ghidra.program.model.pcode.HighFunctionDBUtil.ReturnCommitOption.NO_COMMIT, SourceType.ANALYSIS);
		}
	}

	/** kind: "void", "value" (an integer in r0) or "float" (in fr0). */
	private void setReturn(Function f, String kind) throws Exception {
		List<Parameter> ps = new ArrayList<>(java.util.Arrays.asList(f.getParameters()));
		ReturnParameterImpl ret = kind.equals("void")
			? new ReturnParameterImpl(VoidDataType.dataType, VariableStorage.VOID_STORAGE, currentProgram)
			: kind.equals("float")
			? new ReturnParameterImpl(ghidra.program.model.data.FloatDataType.dataType,
				new VariableStorage(currentProgram, currentProgram.getRegister("fr0")), currentProgram)
			: new ReturnParameterImpl(Undefined4DataType.dataType,
				new VariableStorage(currentProgram, currentProgram.getRegister("r0")), currentProgram);
		f.updateFunction(f.getCallingConventionName(), ret, ps,
			Function.FunctionUpdateType.CUSTOM_STORAGE, true, SourceType.ANALYSIS);
	}

	/** The function whose call produces the value f returns, when f just passes it on. */
	private Function tailCallee(Function f) {
		ghidra.app.decompiler.DecompileResults r = dec.decompileFunction(f, 60, monitor);
		if (r.getHighFunction() == null) {
			return null;
		}
		java.util.Iterator<ghidra.program.model.pcode.PcodeOpAST> ops = r.getHighFunction().getPcodeOps();
		Function only = null;
		while (ops.hasNext()) {
			ghidra.program.model.pcode.PcodeOpAST op = ops.next();
			if (op.getOpcode() != ghidra.program.model.pcode.PcodeOp.RETURN || op.getNumInputs() < 2) {
				continue;
			}
			ghidra.program.model.pcode.PcodeOp def = op.getInput(1).getDef();
			if (def != null && def.getOpcode() == ghidra.program.model.pcode.PcodeOp.INDIRECT) {
				def = r.getHighFunction().getOpRef((int) def.getInput(1).getOffset()); // the call it comes from
			}
			if (def == null || def.getOpcode() != ghidra.program.model.pcode.PcodeOp.CALL) {
				return null; // some return is not a call's result
			}
			Function g = getFunctionAt(def.getInput(0).getAddress());
			if (g == null || (only != null && !only.equals(g))) {
				return null;
			}
			only = g;
		}
		return only;
	}

	@Override
	protected void run() throws Exception {
		dec = new ghidra.app.decompiler.DecompInterface();
		dec.openProgram(currentProgram);
		Map<Address, int[]> votes = new TreeMap<>(); // r0, r1, calls that read r0 after, r1 with r0 busy
		for (Function caller : currentProgram.getFunctionManager().getFunctions(true)) {
			if (!gccFrame(caller)) {
				continue;
			}
			for (Instruction i : currentProgram.getListing().getInstructions(caller.getBody(), true)) {
				if (!mnem(i).equals("jsr")) {
					continue;
				}
				String op = i.getDefaultOperandRepresentation(0);
				int k = op.equals("@r0") ? 0 : op.equals("@r1") ? 1 : -1;
				if (k < 0) {
					continue;
				}
				Function f = callee(i, op.substring(1));
				if (f != null) {
					int[] v = votes.computeIfAbsent(f.getEntryPoint(), a -> new int[5]);
					if (k == 1 && r0Busy(i)) {
						v[3]++; // GCC could not have used r0: no vote
					} else {
						v[k]++;
					}
					if (readsAfter(i, "fr0")) {
						v[4]++; // a float comes back (through r1: nothing in r0)
					}
					if (readsR0After(i)) {
						v[2]++;
					}
				}
			}
		}
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "returns.tsv");
		int setVoid = 0, setValue = 0, setFloat = 0, agree = 0, rowsDisagree = 0, unsigned = 0;
		List<String> disagree = new ArrayList<>();
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("function\tname\tcalls through r0\tthrough r1\treturn before\tdecision\tcalls reading r0 after\tthrough r1 with r0 busy (not counted)\tcalls reading fr0 after");
			for (Map.Entry<Address, int[]> e : votes.entrySet()) {
				Function f = getFunctionAt(e.getKey());
				int r0 = e.getValue()[0], r1 = e.getValue()[1], used = e.getValue()[2], forced = e.getValue()[3];
				int frUsed = e.getValue()[4];
				// callers through r1 that read fr0 after the call: a float return
				boolean wantFloat = frUsed > 0 && used == 0 && r0 == 0 && r1 >= 1;
				DataType before = f.getReturnType();
				String decision;
				// a caller reading r0 after the call is evidence of a value on its own
				boolean wantValue = used > 0 || (r0 >= 2 && r1 == 0);
				boolean wantVoid = !wantValue && r1 >= 2 && r0 == 0;
				// an 8-byte return (undefined8, which GCC calls through r0) with
				// even one call through r1 and none through r0: the 8 bytes are
				// the decompiler's (FUN_0c12d2a0); one vote is enough there
				if (!wantValue && !wantVoid && r1 >= 1 && r0 == 0 && used == 0) {
					inferred(f);
					wantVoid = inferredLength == 8;
				}
				// the same with a few calls through r0 among many through r1
				// (FUN_0c12d1d0: 48 / 2): the r0 ones are from files without
				// the prototype (it is "mixed", so NOPROTO_ covers them)
				if (!wantValue && !wantVoid && r1 >= 4 && r1 >= 4 * r0 && used == 0) {
					inferred(f);
					wantVoid = inferredLength == 8;
				}
				SourceType src = f.getSignatureSource();
				if (wantFloat) {
					if (src == SourceType.USER_DEFINED || src == SourceType.IMPORTED) {
						boolean ok = !inR0(before) && !(before instanceof VoidDataType);
						decision = ok ? "own signature, agrees (float)" : "own signature, DISAGREES (callers read fr0)";
						if (ok) {
							agree++;
						} else {
							rowsDisagree++;
							disagree.add(f.getName());
						}
					} else if (inferred(f).startsWith("fr") && inferredLength == 4) {
						decision = "agrees (float)";
						agree++;
						if (src == SourceType.DEFAULT && !variadic(f)) {
							commitAll(f);
							decision = "agrees (float), committed";
						}
					} else if (variadic(f)) {
						decision = "no committed signature (variadic): callers read fr0";
						unsigned++;
					} else {
						if (src == SourceType.DEFAULT) {
							commitParameters(f);
						}
						setReturn(f, "float");
						decision = "set float (callers read fr0)";
						setFloat++;
					}
				} else if (!wantValue && !wantVoid) {
					decision = r0 > 0 && r1 > 0 ? "mixed: left alone" : "one call: left alone";
				} else if (src == SourceType.USER_DEFINED || src == SourceType.IMPORTED) {
					// r1: nothing in r0 (void, float, double, struct); r0: an integer or pointer
					boolean ok = wantVoid == !inR0(before);
					decision = ok ? "own signature, agrees" : "own signature, DISAGREES (callers say "
						+ (wantVoid ? "nothing in r0" : "an integer or pointer") + ")";
					if (!ok) {
						rowsDisagree++;
						disagree.add(f.getName());
					} else {
						agree++;
					}
				} else if (wantVoid ? agreesNothingInR0(f) : !inferred(f).equals("void")) {
					decision = "agrees";
					agree++;
					if (src == SourceType.DEFAULT && !variadic(f)) {
						// left uncommitted, the decompiler infers again at export and
						// can land elsewhere (FUN_0c114860: 14 callers read r0, exported
						// void); commit what it infers now, which the callers agree with
						commitAll(f);
						decision = "agrees, committed";
					}
				} else if (wantVoid && inferred(f).startsWith("fr")) {
					// an FPU pair (undefined8, called through r0): a float in fr0
					if (src == SourceType.DEFAULT) {
						commitParameters(f);
					}
					setReturn(f, "float");
					decision = "set float (was " + inferredLength + " bytes in the FPU)";
					setFloat++;
				} else if (src == SourceType.DEFAULT && !(wantValue && noParameters(f)) && variadic(f)) {
					// committing the decompiler's parameters would cut off the "..."
					decision = "no committed signature (variadic): callers say " + (wantVoid ? "nothing in r0" : "an integer or pointer");
					unsigned++;
				} else {
					if (src == SourceType.DEFAULT && !(wantValue && noParameters(f))) {
						commitParameters(f); // the decompiler's parameters, so no caller loses arguments
					}
					setReturn(f, wantVoid);
					valued.add(f);
					decision = wantVoid ? "set void" : src == SourceType.DEFAULT ? "set a value in r0 (no parameters)" : "set a value in r0";
					if (wantVoid) {
						setVoid++;
						valued.remove(f);
					} else {
						setValue++;
					}
				}
				w.printf("%s\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%d%n", e.getKey(), f.getName(), r0, r1, before.getDisplayName(), decision, used, forced, frUsed);
			}
		}
		// a function that returns a value by passing on a call's result: that
		// callee returns one too (FUN_0c130d40 ends in a call whose r0 it leaves)
		int passed = 0;
		java.util.Deque<Function> work = new java.util.ArrayDeque<>(valued);
		java.util.Set<Function> done = new java.util.HashSet<>(valued);
		while (!work.isEmpty()) {
			Function f = work.pop();
			Function g = tailCallee(f);
			if (g == null || done.contains(g) || g.getSignatureSource() == SourceType.USER_DEFINED
				|| g.getSignatureSource() == SourceType.IMPORTED || !inferred(g).equals("void")
				|| (g.getSignatureSource() == SourceType.DEFAULT && !noParameters(g))) {
				continue;
			}
			setReturn(g, false);
			done.add(g);
			work.push(g);
			passed++;
			println("returns: " + g.getName() + " returns what " + f.getName() + " passes on");
		}
		dec.dispose();
		println(String.format("returns: %d more set a value because a caller passes their result on", passed));
		println(String.format("returns: %d set float (callers read fr0, or 8 bytes in the FPU before), %d set void, %d set a value, %d already agree; our own signatures disagree with their callers: %d %s; no committed signature: %d -> %s",
			setFloat, setVoid, setValue, agree, rowsDisagree, disagree, unsigned, out));
	}
}
