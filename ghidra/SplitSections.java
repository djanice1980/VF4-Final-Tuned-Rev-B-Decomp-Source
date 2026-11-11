// Split main_prog into .text / .rodata / .data so the decompiler folds
// SH-4 literal pools and string pointers (it only folds read-only memory).
//
// Boundaries were measured with ReportSections.java:
//   .text   0x0C020000-0x0C15D9FF  last function ends at 0x0C15D9E5
//   .rodata 0x0C15DA00-0x0C242FFF  nothing writes here
//   .data   0x0C243000-           lowest store target past .text
// Hand-written asm in .text stores into its own scratch words; every
// write target inside .text is carved out as a small writable block so
// those reads are not folded to their initial zero.
// Functions outside .text are prologue-scan false hits and are removed.
// Auto-analysis also leaves stray "instructions" in .rodata/.data (it
// followed pointers into data before the split); those are cleared on
// every run, so later passes can type that data.
// @category VF4FT
import java.util.ArrayList;
import java.util.List;
import java.util.TreeSet;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;

public class SplitSections extends GhidraScript {

	private static final long RODATA = 0x0C15DA00L;
	private static final long DATA = 0x0C243000L;

	private MemoryBlock splitAt(MemoryBlock b, long at) throws Exception {
		Memory mem = currentProgram.getMemory();
		mem.split(b, toAddr(at));
		return mem.getBlock(toAddr(at));
	}

	private void perms(MemoryBlock b, String name, boolean w, boolean x) throws Exception {
		b.setName(name);
		b.setRead(true);
		b.setWrite(w);
		b.setExecute(x);
	}

	@Override
	protected void run() throws Exception {
		Memory mem = currentProgram.getMemory();
		MemoryBlock text = mem.getBlock(toAddr(0x0C020000L));
		if (!text.getName().equals(".text")) {
			split(mem, text);
		}
		clearDataInstructions();
	}

	private void clearDataInstructions() throws Exception {
		int n = 0;
		for (String name : new String[] { ".rodata", ".data" }) {
			MemoryBlock b = currentProgram.getMemory().getBlock(name);
			ghidra.program.model.listing.InstructionIterator ii =
				currentProgram.getListing().getInstructions(b.getStart(), true);
			List<ghidra.program.model.listing.Instruction> junk = new ArrayList<>();
			while (ii.hasNext()) {
				ghidra.program.model.listing.Instruction i = ii.next();
				if (!b.contains(i.getAddress())) {
					break;
				}
				junk.add(i);
			}
			for (ghidra.program.model.listing.Instruction i : junk) {
				clearListing(i.getAddress());
			}
			n += junk.size();
		}
		println("cleared " + n + " stray instructions in .rodata/.data");
	}

	private void split(Memory mem, MemoryBlock text) throws Exception {

		// Write targets inside .text, merged into clusters (gap <= 0x10).
		TreeSet<Long> targets = new TreeSet<>();
		ReferenceIterator ri = currentProgram.getReferenceManager().getReferenceIterator(text.getStart());
		while (ri.hasNext()) {
			Reference r = ri.next();
			long to = r.getToAddress().getOffset();
			if (r.getReferenceType().isWrite() && to >= 0x0C020000L && to < RODATA) {
				targets.add(to & ~3L);
			}
		}
		List<long[]> clusters = new ArrayList<>();
		for (long t : targets) {
			long[] last = clusters.isEmpty() ? null : clusters.get(clusters.size() - 1);
			if (last != null && t - last[1] <= 0x10) {
				last[1] = t + 4;
			} else {
				clusters.add(new long[] { t, t + 4 });
			}
		}

		// Remove functions outside .text before blocks lose execute.
		List<Function> stray = new ArrayList<>();
		FunctionIterator fi = currentProgram.getFunctionManager().getFunctions(toAddr(RODATA), true);
		while (fi.hasNext()) {
			Function f = fi.next();
			if (f.getEntryPoint().getOffset() < 0x0C360000L) {
				stray.add(f);
			}
		}
		for (Function f : stray) {
			println("removing stray function " + f.getEntryPoint());
			clearListing(f.getBody());
			removeFunction(f);
		}

		MemoryBlock rodata = splitAt(text, RODATA);
		MemoryBlock data = splitAt(rodata, DATA);
		perms(text, ".text", false, true);
		perms(rodata, ".rodata", false, false);
		perms(data, ".data", true, false);

		for (long[] c : clusters) {
			MemoryBlock host = mem.getBlock(toAddr(c[0]));
			MemoryBlock s = host.getStart().getOffset() == c[0] ? host : splitAt(host, c[0]);
			splitAt(s, c[1]).setName(".text");
			perms(s, String.format(".text_scratch_%08x", c[0]), true, false);
			println(String.format("writable scratch 0x%08x-0x%08x", c[0], c[1] - 1));
		}
	}
}
