// Create functions that analysis missed (reached only through pointer
// tables) by finding GCC SH prologues in .text:
//
//   pushes          (zero or more, any order)
//   sts.l pr,@-r15  (LE bytes 22 4F)
//
// Pushes are mov.l Rm,@-r15 (0x2Fm6), fmov FRm,@-r15 (0xFFmB; GCC saves
// fr12-fr15 this way), fschg (pair pushes), sts.l mach/macl,@-r15 and
// stc.l gbr,@-r15. The function starts at the first push of the run;
// the walk stops at a delay slot (the previous function's `rts; fschg`),
// except on a 32-byte boundary: there the "branch" before it is a pool
// word (0x0c23 reads as braf r12, 0xa000 as bra), and stopping put six
// entries 2 bytes into their function (0c0a4600, 0c0af220, ...).
// Walking back over mov.l only put 173 entries inside their prologue. Hits inside existing
// functions or defined data are skipped. Leaf functions (no pr save) are
// not found here; they turn up as call targets of the functions that are,
// except handlers reached only through a table of code pointers: the
// tables listed in ghidra/code_tables.tsv (address, count, stride, name,
// evidence)
// seed a function at every entry.
// @category VF4FT
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

import java.io.File;
import java.nio.file.Files;

public class FindPrologues extends GhidraScript {

	private static int half(byte[] d, int i) {
		return (d[i] & 0xFF) | (d[i + 1] & 0xFF) << 8;
	}

	private static boolean isPush(int w) {
		return (w & 0xFF0F) == 0x2F06      // mov.l Rm,@-r15
			|| (w & 0xFF0F) == 0xFF0B      // fmov FRm,@-r15
			|| w == 0xF3FD                 // fschg
			|| w == 0x4F02 || w == 0x4F12  // sts.l mach/macl,@-r15
			|| w == 0x4F13;                // stc.l gbr,@-r15
	}

	// branches with a delay slot: the next instruction belongs to them
	private static boolean delayed(int w) {
		return w == 0x000B || w == 0x002B                        // rts, rte
			|| (w & 0xF000) == 0xA000 || (w & 0xF000) == 0xB000  // bra, bsr
			|| (w & 0xF0FF) == 0x402B || (w & 0xF0FF) == 0x400B  // jmp, jsr
			|| (w & 0xF0FF) == 0x0023 || (w & 0xF0FF) == 0x0003  // braf, bsrf
			|| (w & 0xFF00) == 0x8D00 || (w & 0xFF00) == 0x8F00; // bt/s, bf/s
	}

	@Override
	protected void run() throws Exception {
		// .text range as measured by ReportSections.java (see SplitSections.java)
		// (test.bin: the whole loaded program; its sections are not measured)
		long base = 0x0C020000L;
		long end = currentProgram.getName().equals("main.bin") ? 0x0C15DA00L
			: currentProgram.getMemory().getBlock(toAddr(base)).getEnd().getOffset() + 1;
		byte[] d = new byte[(int) (end - base)];
		currentProgram.getMemory().getBytes(toAddr(base), d);

		int made = 0, seen = 0;
		for (int i = 0; i + 1 < d.length && !monitor.isCancelled(); i += 2) {
			if ((d[i] & 0xFF) != 0x22 || (d[i + 1] & 0xFF) != 0x4F) {
				continue;
			}
			int s = i;
			while (s >= 2 && isPush(half(d, s - 2))
					&& !(s >= 4 && delayed(half(d, s - 4)) && ((base + s - 2) & 31) != 0)) {
				s -= 2;
			}
			seen++;
			Address a = toAddr(base + s);
			if (getFunctionContaining(a) != null || getDataContaining(a) != null) {
				continue;
			}
			if (getInstructionAt(a) == null && !disassemble(a)) {
				continue;
			}
			if (createFunction(a, null) != null) {
				made++;
			}
		}
		println(String.format("prologue scan: %d candidates, %d new functions", seen, made));

		File tables = new File(new File(getSourceFile().getParentFile().getAbsolutePath()), "code_tables.tsv");
		int entries = 0, seeded = 0;
		// main.bin's code-pointer tables (addresses in main.bin only)
		java.util.List<String> rows = currentProgram.getName().equals("main.bin")
			? Files.readAllLines(tables.toPath()) : java.util.Collections.emptyList();
		for (String line : rows) {
			String[] f = line.split("\t");
			if (f[0].equals("address")) {
				continue;
			}
			long t = Long.parseLong(f[0], 16);
			int n = Integer.parseInt(f[1]);
			int stride = Integer.parseInt(f[2]);
			for (int k = 0; k < n; k++) {
				long target = getInt(toAddr(t + (long) stride * k)) & 0xFFFFFFFFL;
				if (target < base || target >= base + d.length || (target & 1) != 0) {
					throw new IllegalStateException(f[3] + "[" + k + "] = " + Long.toHexString(target) + " is not in .text");
				}
				entries++;
				Address a = toAddr(target);
				if (getFunctionAt(a) != null) {
					continue;
				}
				if (getFunctionContaining(a) != null) {
					throw new IllegalStateException(f[3] + "[" + k + "] at " + a + " is inside " + getFunctionContaining(a).getName());
				}
				if (getInstructionAt(a) == null && !disassemble(a)) {
					throw new IllegalStateException(f[3] + "[" + k + "] at " + a + " does not disassemble");
				}
				if (createFunction(a, null) == null) {
					throw new IllegalStateException(f[3] + "[" + k + "] at " + a + ": createFunction failed");
				}
				seeded++;
			}
		}
		println(String.format("code tables: %d entries, %d new functions", entries, seeded));
	}
}
