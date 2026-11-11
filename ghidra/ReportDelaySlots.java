// Every branch with a delay slot, and what is in the slot, as Ghidra sees
// the code (constant pools and jump tables inside a function are data, not
// branches). Written to work/delay_slots.tsv for tools/match/delay_slots.py,
// which tells from it how each file was built: GCC fills the slots unless
// given -fno-delayed-branch.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;

public class ReportDelaySlots extends GhidraScript {

	@Override
	protected void run() throws Exception {
		File out = new File(getSourceFile().getParentFile().getParentFile().getAbsolutePath(), currentProgram.getName().equals("main.bin") ? "work/delay_slots.tsv" : "work/delay_slots_test.tsv");
		int n = 0, filled = 0;
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("branch\tfunction\tmnemonic\tslot");
			for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
				for (Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) {
					if (i.getDelaySlotDepth() == 0) {
						continue;
					}
					Instruction s = getInstructionAt(i.getMaxAddress().add(1));
					String slot = s == null ? "?" : s.toString().replaceFirst("^_", "");
					w.printf("%s\t%s\t%s\t%s%n", i.getAddress(), f.getEntryPoint(),
						i.getMnemonicString().replaceFirst("^_", ""), slot);
					n++;
					if (!slot.equals("nop")) {
						filled++;
					}
				}
			}
		}
		println(String.format("delay slots: %d branches, %d filled -> %s", n, filled, out));
	}
}
