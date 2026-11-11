// Code wins over guessed address tables.
//
// Ghidra's Create Address Tables analyzer marks runs of words that look like
// addresses as pointer tables, also in .text bytes that nothing has
// disassembled yet. When a function found later flows into one, disassembly
// stops there ("conflicting data") and the function is cut short:
// FUN_0c04f720 ended after six pushes, at a "table" made of the next ones
// (mov.l r14,@-r15; fmov fr8,@-r15, ...). Of the 4,718 tables the analyzer
// makes here, 9 are such conflicts; the rest are real (.data, .rodata and
// the literal pools in .text) and stay.
//
// For each Bad Instruction bookmark "conflicting data" whose data is one of
// the analyzer's address tables in .text and whose flow comes from an
// instruction: clear the table, disassemble from there, fix the function
// body, and let analysis follow the new code; repeat until none is left.
// Each cleared table is printed.
// @category VF4FT
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Bookmark;
import ghidra.program.model.listing.BookmarkManager;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FlowOverAddressTables extends GhidraScript {

	private static final Pattern CONFLICT =
		Pattern.compile("conflicting data at ([0-9a-f]+) \\(flow from ([0-9a-f]+)\\)");

	private static final Pattern TABLE = Pattern.compile("Address table\\[(\\d+)\\] created");

	/** The analyzer's address table starting at or just before a (it bookmarks the start). */
	private Bookmark table(Address a) {
		BookmarkManager bm = currentProgram.getBookmarkManager();
		for (int k = 0; k <= 32; k += 2) {
			Address s = a.subtract(k);
			for (Bookmark b : bm.getBookmarks(s)) {
				Matcher m = TABLE.matcher(b.getComment());
				if (b.getTypeString().equals("Analysis") && m.matches()
					&& s.add(Integer.parseInt(m.group(1)) * 4L).compareTo(a) > 0) {
					return b;
				}
			}
		}
		return null;
	}

	@Override
	protected void run() throws Exception {
		BookmarkManager bm = currentProgram.getBookmarkManager();
		// .text as SplitSections draws it (this runs before the split)
		// (test.bin: the whole loaded program)
		Address textEnd = currentProgram.getName().equals("main.bin") ? toAddr(0x0C15DA00L)
			: currentProgram.getMemory().getBlock(toAddr(0x0C020000L)).getEnd().add(1);
		int cleared = 0;
		for (boolean again = true; again;) {
			again = false;
			List<Bookmark> errors = new ArrayList<>();
			bm.getBookmarksIterator("Error").forEachRemaining(errors::add);
			for (Bookmark e : errors) {
				Matcher m = CONFLICT.matcher(e.getComment());
				if (!e.getCategory().equals("Bad Instruction") || !m.find()) {
					continue;
				}
				Address at = e.getAddress();
				Address data = toAddr(Long.parseLong(m.group(1), 16));
				Address from = toAddr(Long.parseLong(m.group(2), 16));
				Bookmark t = table(data);
				if (t == null || getInstructionAt(from) == null || data.compareTo(textEnd) >= 0) {
					continue;
				}
				int n = Integer.parseInt(TABLE.matcher(t.getComment()).replaceAll("$1"));
				Address start = t.getAddress(), end = start.add(n * 4L - 1);
				Data d = getDataAt(start);
				println(String.format("flow over address table: %s [%d] (%s) at %s, flow from %s", start, n,
					d == null ? "?" : d.getDataType().getName(), at, from));
				clearListing(start, end);
				bm.removeBookmark(t);
				bm.removeBookmark(e);
				new DisassembleCommand(at, null, true).applyTo(currentProgram, monitor);
				Function f = getFunctionContaining(from);
				if (f != null) {
					CreateFunctionCmd.fixupFunctionBody(currentProgram, f, monitor);
				}
				cleared++;
				again = true;
			}
			if (again) {
				analyzeChanges(currentProgram);
			}
		}
		println("flow over address tables: " + cleared + " cleared");
	}
}
