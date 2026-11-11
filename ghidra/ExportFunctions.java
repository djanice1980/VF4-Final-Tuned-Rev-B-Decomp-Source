// Write ghidra/functions.tsv: entry, name, and every address range of
// each function body (for tools that analyse code outside Ghidra).
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

public class ExportFunctions extends GhidraScript {
	@Override
	protected void run() throws Exception {
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "functions.tsv");
		int n = 0;
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("entry\tname\tranges");
			FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
			while (it.hasNext()) {
				Function f = it.next();
				StringBuilder sb = new StringBuilder();
				for (AddressRange r : f.getBody()) {
					sb.append(sb.length() == 0 ? "" : ",").append(r.getMinAddress()).append('-')
						.append(r.getMaxAddress());
				}
				w.printf("%s\t%s\t%s%n", f.getEntryPoint(), f.getName(), sb);
				n++;
			}
		}
		println(n + " functions -> " + out);
	}
}
