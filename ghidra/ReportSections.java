// Report section boundaries in main_prog: last function end (.text),
// lowest address any instruction writes to (start of .data), and the
// functions that sit past .text (likely false prologue hits in data).
// @category VF4FT
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceIterator;

public class ReportSections extends GhidraScript {
	@Override
	protected void run() throws Exception {
		MemoryBlock b = currentProgram.getMemory().getBlock("main_prog");
		Address lowWrite = null, lowWriteFrom = null;
		ReferenceIterator it = currentProgram.getReferenceManager().getReferenceIterator(b.getStart());
		while (it.hasNext()) {
			Reference r = it.next();
			if (!b.contains(r.getFromAddress())) {
				break;
			}
			Address to = r.getToAddress();
			if (r.getReferenceType().isWrite() && b.contains(to)) {
				if (lowWrite == null || to.compareTo(lowWrite) < 0) {
					lowWrite = to;
					lowWriteFrom = r.getFromAddress();
				}
			}
		}
		println("lowest written address: " + lowWrite + " (from " + lowWriteFrom + ")");

		// .text end = end of the last function before the first 64K bucket with none
		Address textEnd = null;
		FunctionIterator fe = currentProgram.getFunctionManager().getFunctions(b.getStart(), true);
		while (fe.hasNext()) {
			Function f = fe.next();
			if (textEnd != null && f.getEntryPoint().subtract(textEnd) > 0x10000) {
				println("outlier function past .text: " + f.getEntryPoint() + " " + f.getName());
				continue;
			}
			if (b.contains(f.getEntryPoint())) {
				textEnd = f.getBody().getMaxAddress();
			}
		}
		println(".text ends at " + textEnd);

		Address dataStart = null;
		java.util.TreeSet<Address> inText = new java.util.TreeSet<>();
		ReferenceIterator ri = currentProgram.getReferenceManager().getReferenceIterator(b.getStart());
		while (ri.hasNext()) {
			Reference r = ri.next();
			Address to = r.getToAddress();
			if (!r.getReferenceType().isWrite() || !b.contains(to)) {
				continue;
			}
			if (to.compareTo(textEnd) <= 0) {
				inText.add(to);
			} else if (dataStart == null || to.compareTo(dataStart) < 0) {
				dataStart = to;
			}
		}
		println("lowest write past .text (.data start): " + dataStart);
		println("write targets inside .text: " + inText);

		long[] hist = new long[0x40];
		FunctionIterator fi = currentProgram.getFunctionManager().getFunctions(b.getStart(), true);
		while (fi.hasNext()) {
			Function f = fi.next();
			if (!b.contains(f.getEntryPoint())) {
				break;
			}
			hist[(int) ((f.getEntryPoint().getOffset() - 0x0C000000L) >> 16)]++;
		}
		StringBuilder sb = new StringBuilder("functions per 64K: ");
		for (int i = 2; i < 0x36; i++) {
			sb.append(String.format("%02x:%d ", i, hist[i]));
		}
		println(sb.toString());
	}
}
