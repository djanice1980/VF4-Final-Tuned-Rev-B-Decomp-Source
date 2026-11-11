// Assign functions to original source files using the assert() path
// strings left in the binary, then fill gaps by link order.
//
// A function that references "/p71/prj71p/vf4e2/<path>.c" gets tag
// "src:<path>" (evidence: named). A run of untagged functions sitting
// between two functions of the same file gets "src?:<path>" (inferred:
// the linker keeps one object file's code together). Code matched to a
// library's source has no assert strings: ghidra/known_files.tsv (start,
// end, file, evidence) gives every function starting in a range the tag
// "srcp:<path>" (zlib 1.1.4, nlajamma).
//
// Then by callers: an untagged function right next to a file's run whose
// every reference is a call from that file gets "srcc:<path>" (a static
// helper can only be called from its own file, and the linker keeps a
// file's code together). Repeated, with the gap fill, until nothing
// changes; a function referenced from data (a pointer in a table) or from
// nowhere stays unplaced.
//
// And by strings: string literals are laid out in object order too (the
// strings only one placed file uses run in the same order as the files'
// code, without overlapping), so an untagged function right next to a
// file's run, every string literal of which lies inside that file's strings
// (the nearest placed strings below and above it are both that file's, or
// it is that file's own), gets "srcs:<path>". A string between two files'
// strings, or next to one several files share, places nothing.
//
// Writes ghidra/source_map.tsv: address, name, size, file, evidence.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.DataIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Reference;

public class MapSourceFiles extends GhidraScript {

	private static final String PREFIX = "/p71/prj71p/vf4e2/";

	// A pool word pointing at the string sits at the end of (or just past)
	// the function that loads it, so fall back to the function before it.
	private Function owner(Address ref) {
		Function f = getFunctionContaining(ref);
		return f != null ? f : getFunctionBefore(ref);
	}

	private List<Address> refsTo(Address target) {
		List<Address> out = new ArrayList<>();
		for (Reference r : getReferencesTo(target)) {
			Address from = r.getFromAddress();
			Data d = getDataAt(from);
			// pool pointer: follow one more hop to the loading instruction
			if (d != null && d.isPointer()) {
				for (Reference r2 : getReferencesTo(from)) {
					out.add(r2.getFromAddress());
				}
			}
			out.add(from);
		}
		return out;
	}

	/**
	 * The one file every reference to f comes from, all of them calls (a
	 * call through a pool word: the word sits in or just after its caller);
	 * null when one is data from outside a function, f has no references, or
	 * they come from more than one file or from untagged code.
	 */
	private String callersFile(Function f, String[] file, Map<Address, Integer> index) {
		String only = null;
		int n = 0;
		for (Reference r : getReferencesTo(f.getEntryPoint())) {
			Address from = r.getFromAddress();
			if (!from.isMemoryAddress()) {
				continue;
			}
			boolean call = r.getReferenceType().isCall();
			Function c = call ? getFunctionContaining(from) : poolOwner(from);
			if (!call && (getDataAt(from) == null || !getDataAt(from).isPointer() || c == null)) {
				return null; // a pointer in a table, a jump, ...
			}
			if (c == null || c.equals(f)) {
				continue; // recursion says nothing
			}
			Integer k = index.get(c.getEntryPoint());
			if (k == null || file[k] == null || (only != null && !only.equals(file[k]))) {
				return null;
			}
			only = file[k];
			n++;
		}
		return n > 0 ? only : null;
	}

	/** The string literals (in .rodata) f loads, directly or through a pool word. */
	private List<Long> stringsOf(Function f) {
		java.util.TreeSet<Long> out = new java.util.TreeSet<>();
		for (ghidra.program.model.listing.Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) {
			for (Reference r : i.getReferencesFrom()) {
				Address t = r.getToAddress();
				if (!t.isMemoryAddress()) {
					continue;
				}
				Data d = getDataAt(t);
				if (d != null && d.isPointer() && t.getOffset() < RODATA) {
					for (Reference r2 : d.getReferencesFrom()) {
						addString(r2.getToAddress(), out);
					}
				} else {
					addString(t, out);
				}
			}
		}
		return new ArrayList<>(out);
	}

	private static final long RODATA = 0x0C15DA00L, DATA = 0x0C243000L;

	private void addString(Address a, java.util.Set<Long> out) {
		long o = a.getOffset();
		if (o >= RODATA && o < DATA) {
			Data d = getDataAt(a);
			if (d != null && d.hasStringValue()) {
				out.add(o);
			}
		}
	}

	/** Each string placed functions use: its file, or "" when several files use it. */
	private TreeMap<Long, String> stringOwners(List<List<Long>> strings, String[] file) {
		TreeMap<Long, String> owner = new TreeMap<>();
		for (int i = 0; i < file.length; i++) {
			if (file[i] == null) {
				continue;
			}
			for (long a : strings.get(i)) {
				String o = owner.get(a);
				owner.put(a, o == null || o.equals(file[i]) ? file[i] : "");
			}
		}
		return owner;
	}

	/** The one file all these strings lie inside the strings of; null when not one. */
	private static String stringsFile(List<Long> mine, TreeMap<Long, String> owner) {
		String only = null;
		for (long a : mine) {
			String o = owner.get(a);
			if (o == null) {
				Map.Entry<Long, String> lo = owner.lowerEntry(a), hi = owner.higherEntry(a);
				o = lo != null && hi != null && lo.getValue().equals(hi.getValue()) ? lo.getValue() : "";
			}
			if (o.isEmpty() || (only != null && !only.equals(o))) {
				return null;
			}
			only = o;
		}
		return only;
	}

	/** The function a pool word belongs to: every instruction loading it is in that one function. */
	private Function poolOwner(Address pool) {
		Function owner = null;
		for (Reference r : getReferencesTo(pool)) {
			Function c = getFunctionContaining(r.getFromAddress());
			if (c == null || (owner != null && !owner.equals(c))) {
				return null;
			}
			owner = c;
		}
		return owner;
	}

	@Override
	protected void run() throws Exception {
		Map<Address, String> named = new TreeMap<>();
		DataIterator it = currentProgram.getListing().getDefinedData(true);
		while (it.hasNext() && !monitor.isCancelled()) {
			Data d = it.next();
			if (!d.hasStringValue()) {
				continue;
			}
			String s = String.valueOf(d.getValue());
			if (!s.startsWith(PREFIX)) {
				continue;
			}
			String path = s.substring(PREFIX.length());
			for (Address from : refsTo(d.getAddress())) {
				Function f = owner(from);
				if (f != null) {
					named.put(f.getEntryPoint(), path);
				}
			}
		}

		List<Function> all = new ArrayList<>();
		FunctionIterator fi = currentProgram.getFunctionManager().getFunctions(true);
		while (fi.hasNext()) {
			all.add(fi.next());
		}

		// address ranges matched to a library's source (no assert strings there)
		List<long[]> ranges = new ArrayList<>();
		List<String> rangeFile = new ArrayList<>();
		// the library ranges: main.bin's, or test.bin's own (its addresses differ)
		for (String l : java.nio.file.Files.readAllLines(new File(getSourceFile().getParentFile().getAbsolutePath(),
			currentProgram.getName().equals("main.bin") ? "known_files.tsv" : "known_files_test.tsv").toPath())) {
			String[] c = l.split("\t", -1);
			if (c[0].equals("start") || l.isBlank()) {
				continue;
			}
			ranges.add(new long[] { Long.parseLong(c[0], 16), Long.parseLong(c[1], 16) });
			rangeFile.add(c[2]);
		}

		String[] file = new String[all.size()];
		boolean[] inferred = new boolean[all.size()];
		boolean[] matched = new boolean[all.size()];
		int last = -1;
		for (int i = 0; i < all.size(); i++) {
			String p = named.get(all.get(i).getEntryPoint());
			if (p == null) {
				continue;
			}
			file[i] = p;
			if (last >= 0 && p.equals(file[last])) {
				for (int j = last + 1; j < i; j++) {
					file[j] = p;
					inferred[j] = true;
				}
			}
			last = i;
		}

		// by callers, to a fixed point
		Map<Address, Integer> index = new java.util.HashMap<>();
		for (int i = 0; i < all.size(); i++) {
			index.put(all.get(i).getEntryPoint(), i);
		}
		boolean[] byCallers = new boolean[all.size()];
		boolean[] byStrings = new boolean[all.size()];
		List<List<Long>> strings = new ArrayList<>();
		for (Function f : all) {
			strings.add(stringsOf(f));
		}
		for (boolean changed = true; changed;) {
			changed = false;
			for (int i = 0; i < all.size(); i++) {
				if (file[i] != null) {
					continue;
				}
				String left = i > 0 ? file[i - 1] : null, right = i + 1 < all.size() ? file[i + 1] : null;
				if (left == null && right == null) {
					continue;
				}
				String only = callersFile(all.get(i), file, index);
				if (only != null && (only.equals(left) || only.equals(right))) {
					file[i] = only;
					byCallers[i] = true;
					changed = true;
				}
			}
			for (int i = 0; i < all.size(); i++) {
				if (file[i] != null || strings.get(i).isEmpty()) {
					continue;
				}
				String left = i > 0 ? file[i - 1] : null, right = i + 1 < all.size() ? file[i + 1] : null;
				if (left == null && right == null) {
					continue;
				}
				String only = stringsFile(strings.get(i), stringOwners(strings, file));
				if (only != null && (only.equals(left) || only.equals(right))) {
					file[i] = only;
					byStrings[i] = true;
					changed = true;
				}
			}
			// a placed function can close a gap between two of a file's functions
			int prev = -1;
			for (int i = 0; i < all.size(); i++) {
				if (file[i] == null) {
					continue;
				}
				if (prev >= 0 && prev < i - 1 && file[i].equals(file[prev])) {
					for (int j = prev + 1; j < i; j++) {
						file[j] = file[i];
						inferred[j] = true;
						changed = true;
					}
				}
				prev = i;
			}
		}

		for (int i = 0; i < all.size(); i++) {
			long a = all.get(i).getEntryPoint().getOffset();
			for (int k = 0; k < ranges.size(); k++) {
				if (a >= ranges.get(k)[0] && a <= ranges.get(k)[1]) {
					file[i] = rangeFile.get(k);
					inferred[i] = false;
					byCallers[i] = false;
					byStrings[i] = false;
					matched[i] = true;
				}
			}
		}

		// Re-runs must not keep tags from an earlier map (function bounds
		// change when new functions are found), so clear them first.
		for (Function f : all) {
			for (ghidra.program.model.listing.FunctionTag t : new ArrayList<>(f.getTags())) {
				if (t.getName().startsWith("src:") || t.getName().startsWith("src?:") || t.getName().startsWith("srcp:")
					|| t.getName().startsWith("srcc:") || t.getName().startsWith("srcs:")) {
					f.removeTag(t.getName());
				}
			}
		}

		File out = new File(getSourceFile().getParentFile().getAbsolutePath(),
			currentProgram.getName().equals("main.bin") ? "source_map.tsv" : "source_map_test.tsv");
		int n = 0, m = 0, c = 0, st = 0;
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("address\tname\tsize\tfile\tevidence");
			for (int i = 0; i < all.size(); i++) {
				if (file[i] == null) {
					continue;
				}
				Function f = all.get(i);
				f.addTag((matched[i] ? "srcp:" : byCallers[i] ? "srcc:" : byStrings[i] ? "srcs:" : inferred[i] ? "src?:" : "src:") + file[i]);
				w.printf("%s\t%s\t%d\t%s\t%s%n", f.getEntryPoint(), f.getName(),
					f.getBody().getNumAddresses(), file[i],
					matched[i] ? "matched-source" : byCallers[i] ? "callers" : byStrings[i] ? "strings" : inferred[i] ? "link-order" : "assert-string");
				if (byCallers[i]) c++; else if (byStrings[i]) st++; else if (inferred[i]) m++; else n++;
			}
		}
		println(String.format("source map: %d named, %d inferred by link order, %d by callers, %d by strings, %d functions total -> %s",
			n, m, c, st, all.size(), out));
	}
}
