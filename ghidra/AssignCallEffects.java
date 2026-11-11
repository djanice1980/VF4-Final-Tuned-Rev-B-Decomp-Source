// Calling conventions for the assembly routines, from what they write.
//
// The compiler spec says a call destroys r0-r7 and fr0-fr11, as GCC's
// convention does (GCC code never saves them), so the decompiler does not
// carry a value across a call. The hand-written code keeps conventions of its
// own, and its callers rely on them: libgcc's division helpers (GCC knows what
// each one touches: after __sdivsi3_i4 a caller goes on using r6 and r7) and
// the assembly libraries (the Elan library at 0x0C104000-0x0C10E000, the
// vector helpers near 0x0C0DC500), which hand values from one routine to the
// next in those registers (bsr FUN_0c10af00, bsr FUN_0c10afa0, and the second
// reads what the first left). Under the default convention such a value shows
// as extraout_r6, unknown. So:
//
//   assembly  a function that does not set up GCC's frame pointer (mov
//             r15,r14 before its first call or return; 5,565 functions do,
//             the 484 that do not are the libraries above, libgcc's helpers,
//             face_skin and one-instruction stubs);
//   writes    the scratch registers (r1-r7, fr1-fr11; r0 and fr0 return
//             values) it, or anything it calls, writes (minus
//             ones pushed before their first write and popped back into
//             the same register: a push that comes after the routine has
//             already changed the register saves the routine's own value,
//             not the caller's, as coli_sphere_sets_test does with r2-r4
//             around its mask step);
//             an indirect call counts as writing all of them;
//   overrides ghidra/call_effects_overrides.tsv (address, name, writes,
//             why): the complete list of scratch registers a routine
//             writes, for routines whose body in the program does not show
//             it: a body cut short where a delay slot is also a branch
//             target (intensity_strip_send's specular loop), an entry that
//             falls into the next routine (intensity_send), pair moves
//             under fschg / frchg read as single registers
//             (vec_to_local_into). The list replaces the measured one
//             ("-" = writes none, "all" = every one) and is what callers
//             inherit;
//
// and an assembly function that leaves some of them alone gets a convention,
// added to the program as a compiler spec extension, that destroys exactly
// what it writes: __keeps_r6_r7 keeps r6 and r7. GCC-compiled functions keep
// the default even when they happen not to write a register: no GCC caller
// relies on that, and carrying values across them is what invented
// parameters. Runs before AssignPrototypes, which infers parameters through
// these calls. Every decision is written to ghidra/call_effects.tsv.
// @category VF4FT
import ghidra.app.script.GhidraScript;
import ghidra.program.database.SpecExtension;
import ghidra.program.model.lang.Register;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.symbol.Reference;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

public class AssignCallEffects extends GhidraScript {

	// the registers a call may or may not destroy: the scratch ones besides
	// the return registers r0 / fr0 (__sdivsi3_i4 keeps r1-r3, and its callers
	// keep values there: 58 functions showed extraout_r3) and the arguments
	private static final List<String> ARGS =
		List.of("r1", "r2", "r3", "r4", "r5", "r6", "r7", "fr1", "fr2", "fr3",
			"fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11");

	/** Registers in an operand list, dr pairs and fv quads split into fr. */
	private static Set<String> regs(Object[] objs) {
		Set<String> out = new HashSet<>();
		for (Object o : objs) {
			if (!(o instanceof Register)) {
				continue;
			}
			String n = ((Register) o).getName();
			if (n.startsWith("dr")) {
				int k = Integer.parseInt(n.substring(2));
				out.add("fr" + k);
				out.add("fr" + (k + 1));
			}
			else if (n.startsWith("fv")) {
				int k = Integer.parseInt(n.substring(2));
				for (int j = 0; j < 4; j++) {
					out.add("fr" + (k + j));
				}
			}
			else {
				out.add(n);
			}
		}
		return out;
	}

	/** Does f set up GCC's frame pointer (mov r15,r14) before its first call or return? */
	private boolean gccFrame(Function f) {
		Instruction i = getInstructionAt(f.getEntryPoint());
		for (int n = 0; i != null && n < 40 && f.getBody().contains(i.getAddress()); n++, i = i.getNext()) {
			// (in the delay slot of a conditional branch too, where Ghidra
			// writes the instruction with a leading underscore:
			// mv_hdr_buffer_set's bf.s / mov r15,r14)
			if (i.toString().replaceFirst("^_", "").equals("mov r15,r14")) {
				return true;
			}
			String m = i.getMnemonicString().replaceFirst("^_", "");
			if (m.equals("rts") || m.equals("jsr") || m.equals("bsr")) {
				// the delay slot can hold it: GCC fills the first call's slot with the
				// frame setup (FUN_0c133c60: jsr @r1; mov r15,r14)
				Instruction slot = i.getNext();
				return slot != null && slot.toString().replaceFirst("^_", "").equals("mov r15,r14");
			}
		}
		return false;
	}

	private static String prototype(String name, Set<String> killed) {
		StringBuilder k = new StringBuilder();
		for (String r : List.of("r0", "fr0")) { // the return registers
			k.append("<register name=\"" + r + "\"/>");
		}
		for (String r : ARGS) {
			if (killed.contains(r)) {
				k.append("<register name=\"" + r + "\"/>");
			}
		}
		for (String r : List.of("MACH", "MACL", "PR", "FPUL", "S", "M", "Q", "T")) {
			k.append("<register name=\"" + r + "\"/>");
		}
		StringBuilder in = new StringBuilder();
		for (int i = 4; i <= 11; i++) {
			in.append("<pentry minsize=\"1\" maxsize=\"4\" metatype=\"float\"><register name=\"fr" + i + "\"/></pentry>");
		}
		for (int i = 4; i <= 10; i += 2) {
			in.append("<pentry minsize=\"5\" maxsize=\"8\" metatype=\"float\"><register name=\"dr" + i + "\"/></pentry>");
		}
		for (int i = 4; i <= 7; i++) {
			in.append("<pentry minsize=\"1\" maxsize=\"4\"><register name=\"r" + i + "\"/></pentry>");
		}
		in.append("<pentry minsize=\"1\" maxsize=\"500\" align=\"4\"><addr offset=\"0\" space=\"stack\"/></pentry>");
		StringBuilder un = new StringBuilder();
		for (String r : List.of("r15", "r14", "r13", "r12", "r11", "r10", "r9", "r8", "fr12", "fr13", "fr14", "fr15",
			"FPSCR", "FPSCR_SZ", "FPSCR_PR")) {
			un.append("<register name=\"" + r + "\"/>");
		}
		return "<prototype name=\"" + name + "\" extrapop=\"0\" stackshift=\"0\" strategy=\"register\">"
			+ "<input>" + in + "</input>"
			+ "<output><pentry minsize=\"1\" maxsize=\"4\" metatype=\"float\"><register name=\"fr0\"/></pentry>"
			+ "<pentry minsize=\"5\" maxsize=\"8\" metatype=\"float\"><register name=\"dr0\"/></pentry>"
			+ "<pentry minsize=\"1\" maxsize=\"4\"><register name=\"r0\"/></pentry>"
			+ "<pentry minsize=\"5\" maxsize=\"8\"><addr space=\"join\" piece1=\"r1\" piece2=\"r0\"/></pentry></output>"
			+ "<unaffected>" + un + "</unaffected>"
			+ "<killedbycall>" + k + "</killedbycall>"
			+ "</prototype>";
	}

	@Override
	protected void run() throws Exception {
		Map<Function, Set<String>> writes = new LinkedHashMap<>();
		Map<Function, Set<Function>> callees = new HashMap<>();
		for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
			Set<String> w = new HashSet<>(), pushed = new HashSet<>(), popped = new HashSet<>();
			Set<Function> cs = new HashSet<>();
			boolean indirect = false;
			for (Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) {
				if (i.getNumOperands() == 2 && i.getDefaultOperandRepresentation(1).equals("@-r15")) {
					// a save only when nothing has written the register yet
					for (String r : regs(i.getOpObjects(0))) {
						if (!w.contains(r)) {
							pushed.add(r);
						}
					}
				}
				if (i.getNumOperands() == 2 && i.getDefaultOperandRepresentation(0).equals("@r15+")) {
					popped.addAll(regs(i.getResultObjects()));
				}
				w.addAll(regs(i.getResultObjects()));
				if (i.getFlowType().isCall()) {
					boolean known = false;
					for (Reference r : i.getReferencesFrom()) {
						if (r.getReferenceType().isCall()) {
							Function c = getFunctionAt(r.getToAddress());
							if (c != null) {
								cs.add(c);
								known = true;
							}
						}
					}
					indirect |= !known;
				}
			}
			pushed.retainAll(popped);
			w.removeAll(pushed);
			w.retainAll(ARGS);
			if (indirect) {
				w.addAll(ARGS);
			}
			writes.put(f, w);
			callees.put(f, cs);
		}
		// the overrides: a routine's whole list, from its source
		Set<Function> fixed = new HashSet<>();
		java.io.File ov = new java.io.File(getSourceFile().getParentFile().getAbsolutePath(), "call_effects_overrides.tsv");
		if (ov.exists() && currentProgram.getName().equals("main.bin")) {
			List<String> lines = java.nio.file.Files.readAllLines(ov.toPath());
			for (String line : lines.subList(1, lines.size())) {
				String[] c = line.split("\t", -1);
				if (c.length < 3 || c[0].isBlank()) {
					continue;
				}
				Function f = getFunctionAt(toAddr(Long.parseLong(c[0], 16)));
				if (f == null || !f.getName().equals(c[1])) {
					throw new IllegalStateException("call_effects_overrides.tsv: no function " + c[1] + " at " + c[0]);
				}
				Set<String> w = new HashSet<>();
				if (c[2].equals("all")) {
					w.addAll(ARGS);
				}
				else if (!c[2].equals("-")) {
					for (String r : c[2].split(",")) {
						if (!ARGS.contains(r.strip()) && !r.strip().equals("r0") && !r.strip().equals("fr0")) {
							throw new IllegalStateException("call_effects_overrides.tsv: " + c[1] + ": not a scratch register: " + r);
						}
						w.add(r.strip());
					}
					w.retainAll(ARGS);
				}
				writes.put(f, w);
				fixed.add(f);
			}
		}
		for (boolean changed = true; changed;) {
			changed = false;
			for (Function f : writes.keySet()) {
				if (fixed.contains(f)) {
					continue; // its list is complete as given
				}
				for (Function c : callees.get(f)) {
					changed |= writes.get(f).addAll(writes.get(c));
				}
			}
		}
		SpecExtension ext = new SpecExtension(currentProgram);
		Set<String> added = new HashSet<>();
		List<String> table = new ArrayList<>();
		Map<String, Integer> byModel = new TreeMap<>();
		int assigned = 0;
		for (Map.Entry<Function, Set<String>> e : writes.entrySet()) {
			monitor.checkCancelled();
			Function f = e.getKey();
			Set<String> kept = new TreeSet<>((a, b) -> ARGS.indexOf(a) - ARGS.indexOf(b));
			kept.addAll(ARGS);
			kept.removeAll(e.getValue());
			if (kept.isEmpty() || gccFrame(f)) {
				// an earlier run may have given it a convention
				// (before an override said it writes everything)
				String cc = f.getCallingConventionName();
				if (cc != null && cc.startsWith("__keeps")) {
					ghidra.program.model.symbol.SourceType src = f.getSignatureSource();
					f.setCallingConvention(Function.DEFAULT_CALLING_CONVENTION_STRING);
					if (f.getSignatureSource() != src) {
						f.setSignatureSource(src);
					}
				}
				continue;
			}
			String name = "__keeps_" + String.join("_", kept);
			if (added.add(name)) {
				ext.addReplaceCompilerSpecExtension(prototype(name, e.getValue()), monitor);
			}
			ghidra.program.model.symbol.SourceType src = f.getSignatureSource();
			f.setCallingConvention(name);
			if (f.getSignatureSource() != src) {
				f.setSignatureSource(src); // still no parameters of ours: AssignPrototypes may give some
			}
			assigned++;
			byModel.merge(name, 1, Integer::sum);
			List<String> w = new ArrayList<>(e.getValue());
			w.sort((a, b) -> ARGS.indexOf(a) - ARGS.indexOf(b));
			table.add(String.format("%s\t%s\t%s\t%s", f.getEntryPoint(), f.getName(), name,
				w.isEmpty() ? "-" : String.join(",", w)));
		}
		java.io.File out = new java.io.File(getSourceFile().getParentFile().getAbsolutePath(), "call_effects.tsv");
		try (java.io.PrintWriter pw = new java.io.PrintWriter(out, "UTF-8")) {
			pw.println("function\tname\tconvention\twrites (scratch registers)");
			for (String l : table) {
				pw.println(l);
			}
		}
		println(String.format("call effects: %d assembly functions keep some scratch registers (%d conventions): %s",
			assigned, added.size(), byModel));
	}
}
