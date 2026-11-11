// Tell the decompiler the FPU mode at function entry: FPSCR_SZ = 0 and
// FPSCR_PR = 0 over all of .text.
//
// Why: the SH-4 module models SZ/PR as pseudo-registers that fmov and the
// FPU arithmetic semantics test. Left unknown, every function decompiles
// with `if (in_FPSCR_SZ == '\0')` branches (14,894 + 9,148 references with
// stock Ghidra). Evidence for the values:
//   - startup loads FPSCR = 0x40000 (0x0C020036): PR = 0, SZ = 0
//   - no fcnvsd/fcnvds anywhere: no double-precision code
//   - every GCC-compiled path returns with the SZ it entered with
//     (AssignFpuContext.java reports the asm exceptions)
// Since the SZ and PR context rewrites of the ghidra/lang variant, fmov
// widths and FPU precision are fixed at disassembly time (`sz` / `fpu_pr`,
// see AssignFpuContext.java), so no instruction tests these pseudo-registers
// any more. They are still read when `sts fpscr` packs FPSCR (libgcc's
// save of the caller's FPSCR); knowing them keeps in_FPSCR_SZ/PR out of
// those functions.
//
// The variant's pspec tracked_set did not reach the program context in
// Ghidra 12.1.2, so the values are set here instead.
// @category VF4FT
import java.math.BigInteger;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.ProgramContext;

public class SetFpuEntryState extends GhidraScript {
	@Override
	protected void run() throws Exception {
		ProgramContext pc = currentProgram.getProgramContext();
		for (String n : new String[] { "FPSCR_SZ", "FPSCR_PR" }) {
			pc.setValue(currentProgram.getRegister(n), toAddr(0x0C020000L), toAddr(0x0C15D9FFL),
				BigInteger.ZERO);
		}
		println("FPSCR_SZ = 0, FPSCR_PR = 0 over .text 0x0C020000-0x0C15D9FF");
	}
}
