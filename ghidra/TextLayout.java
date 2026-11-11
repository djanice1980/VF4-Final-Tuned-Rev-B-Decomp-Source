// Classify every .text byte, for the progress page's denominator:
// in a function (the byte range its source reproduces, code with literal
// pool and tables, from ghidra/matched_extents.tsv, which tools/match/match.py
// writes; else the function body), alignment padding (nop / zero), data the code references
// (literal pools, jump tables), other defined data, instructions outside
// any function, and undefined bytes. Executable code = everything but
// padding and data. Writes ghidra/text_layout.tsv.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Instruction;

public class TextLayout extends GhidraScript {
	@Override
	protected void run() throws Exception {
		long inFn = 0, pad = 0, dataRef = 0, dataOther = 0, insn = 0, undef = 0;
		Address a = toAddr(0x0C020000L), end = toAddr(0x0C15DA00L);
		java.util.TreeMap<Long, Long> extents = new java.util.TreeMap<>();
		java.util.Set<Long> dataExtents = new java.util.HashSet<>(); // data a source file keeps in .text
		File ext = new File(getSourceFile().getParentFile().getAbsolutePath(), "matched_extents.tsv");
		if (ext.exists()) {
			for (String line : java.nio.file.Files.readAllLines(ext.toPath())) {
				String[] f = line.split("\t");
				if (f.length >= 2 && f[0].matches("[0-9a-f]{8}")) {
					extents.put(Long.parseLong(f[0], 16), Long.parseLong(f[0], 16) + Long.parseLong(f[1]));
					if (f.length >= 4 && f[3].equals("data")) {
						dataExtents.add(Long.parseLong(f[0], 16));
					}
				}
			}
		}
		while (a.compareTo(end) < 0) {
			java.util.Map.Entry<Long, Long> e = extents.floorEntry(a.getOffset());
			if (e != null && a.getOffset() < e.getValue()) {
				if (dataExtents.contains(e.getKey())) {
					dataOther += e.getValue() - a.getOffset(); // reproduced by source, but data: not code
				} else {
					inFn += e.getValue() - a.getOffset();
				}
				a = toAddr(e.getValue());
				continue;
			}
			CodeUnit cu = currentProgram.getListing().getCodeUnitContaining(a);
			int len = cu == null ? 2 : cu.getLength();
			if (getFunctionContaining(a) != null) {
				inFn += len;
			} else if (cu instanceof Instruction) {
				if (((Instruction) cu).getMnemonicString().replaceFirst("^_", "").equals("nop")) {
					pad += len;
				} else {
					insn += len;
				}
			} else if (cu instanceof Data && ((Data) cu).isDefined()) {
				if (cu.getReferenceIteratorTo().hasNext()) {
					dataRef += len;
				} else {
					dataOther += len;
				}
			} else if ((a.getOffset() & 1) != 0) {
				// an odd address: the second byte of a word whose first byte
				// is a one-byte data unit; judge it by the aligned word
				int w = getShort(a.subtract(1)) & 0xFFFF;
				if (w == 0x0009 || w == 0x0000) {
					pad += 1;
				} else {
					undef += 1;
				}
				len = 1;
			} else {
				int w = getShort(a) & 0xFFFF;
				if (w == 0x0009 || w == 0x0000) {
					pad += 2;
				} else {
					undef += 2;
				}
				len = 2;
			}
			a = a.add(len);
		}
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "text_layout.tsv");
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("in_functions\tpadding\tdata_referenced\tdata_other\tinstructions_outside\tundefined");
			w.printf("%d\t%d\t%d\t%d\t%d\t%d%n", inFn, pad, dataRef, dataOther, insn, undef);
		}
		println(String.format(".text: %d in functions, %d padding, %d referenced data, %d other data, "
			+ "%d instructions outside functions, %d undefined -> %s", inFn, pad, dataRef, dataOther, insn, undef, out));
	}
}
