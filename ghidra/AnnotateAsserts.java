// Name the assert handler and record every assert on its caller.
//
// FUN_0c0f23a0 is called as (file, __LINE__, "assertion `%s' failed.\n",
// #expr) from every assert site, so it is named fatal_at (inferred; also used printf-style).
// Each caller gets "assert <file>:<line>: <expr>" lines in its plate
// comment, and ghidra/asserts.tsv lists them all. The line numbers give
// source order within a file; LinkOrderCheck in this script reports any
// file whose functions are not in increasing line order by address.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.pcode.HighFunction;
import ghidra.program.model.pcode.PcodeOp;
import ghidra.program.model.pcode.PcodeOpAST;
import ghidra.program.model.pcode.Varnode;
import ghidra.program.model.symbol.SourceType;

public class AnnotateAsserts extends GhidraScript {

	private static final long HANDLER = 0x0C0F23A0L;
	private static final String PREFIX = "/p71/prj71p/vf4e2/";

	// String arguments arrive as CAST/COPY/PTRSUB of a constant pointer.
	private Long constValue(Varnode v) {
		for (int depth = 0; v != null && depth < 8; depth++) {
			if (v.isConstant()) {
				return v.getOffset();
			}
			PcodeOp def = v.getDef();
			if (def == null) {
				return null;
			}
			switch (def.getOpcode()) {
			case PcodeOp.CAST:
			case PcodeOp.COPY:
			case PcodeOp.INT_ZEXT:
				v = def.getInput(0);
				break;
			case PcodeOp.PTRSUB: // (base 0, offset) for a global
				if (def.getInput(0).isConstant() && def.getInput(0).getOffset() == 0) {
					v = def.getInput(1);
					break;
				}
				return null;
			default:
				return null;
			}
		}
		return null;
	}

	// Fallback: walk back from the call (jsr) through its basic block and
	// take the last constant written to rN (pool load or immediate).
	private Long regBefore(Address call, String reg) {
		ghidra.program.model.listing.Instruction ins = getInstructionAt(call);
		ghidra.program.model.listing.Instruction delay = ins == null ? null : ins.getNext();
		// delay slot executes before the target: check it first
		for (ghidra.program.model.listing.Instruction i : new ghidra.program.model.listing.Instruction[] { delay }) {
			Long v = constWrite(i, reg);
			if (v != null) return v;
		}
		for (int n = 0; ins != null && n < 16; n++) {
			ins = ins.getPrevious();
			if (ins == null) break;
			Long v = constWrite(ins, reg);
			if (v != null) return v;
			if (ins.getFlowType().isJump() || ins.getFlowType().isCall()) break;
			if (getReferencesTo(ins.getAddress()).length > 0 && isJumpTarget(ins.getAddress())) break;
		}
		return null;
	}

	private boolean isJumpTarget(Address a) {
		for (ghidra.program.model.symbol.Reference r : getReferencesTo(a)) {
			if (r.getReferenceType().isFlow()) return true;
		}
		return false;
	}

	private Long constWrite(ghidra.program.model.listing.Instruction i, String reg) {
		if (i == null) return null;
		Object[] res = i.getResultObjects();
		boolean writes = false;
		for (Object o : res) {
			if (o instanceof ghidra.program.model.lang.Register
				&& ((ghidra.program.model.lang.Register) o).getName().equals(reg)) writes = true;
		}
		if (!writes) return null;
		String m = i.getMnemonicString();
		if (m.equals("mov") && i.getScalar(0) != null) {
			return i.getScalar(0).getSignedValue();
		}
		if (m.equals("mov.l") || m.equals("mov.w")) {
			for (ghidra.program.model.symbol.Reference r : i.getReferencesFrom()) {
				if (r.getReferenceType().isRead()) {
					try {
						Address a = r.getToAddress();
						return m.equals("mov.l") ? (getInt(a) & 0xFFFFFFFFL) : (long) getShort(a);
					} catch (Exception e) {
						return null;
					}
				}
			}
		}
		return null;
	}

	private String cString(long c) {
		Data d = getDataAt(toAddr(c));
		if (d != null && d.hasStringValue()) {
			return String.valueOf(d.getValue());
		}
		// short strings such as "0" are often left undefined: read the bytes
		try {
			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < 256; i++) {
				int b = getByte(toAddr(c + i)) & 0xFF;
				if (b == 0) {
					return sb.toString();
				}
				if (b < 0x20 || b > 0x7E) {
					return null;
				}
				sb.append((char) b);
			}
		} catch (Exception e) {
			// not readable memory
		}
		return null;
	}

	private String stringAt(Varnode v) {
		Long c = constValue(v);
		return c == null ? null : cString(c);
	}

	@Override
	protected void run() throws Exception {
		Function handler = getFunctionAt(toAddr(HANDLER));
		handler.setName("fatal_at", SourceType.USER_DEFINED);
		handler.setComment("name: inferred\n"
			+ "printf-style fatal error: (__FILE__, __LINE__, fmt, ...).\n"
			+ "assert() calls it with fmt \"assertion `%s' failed.\\n\" and #expr;\n"
			+ "malloc.c uses it directly (\"mixed heap or broken %p\", \"over write %p\").");

		List<Function> callers = new ArrayList<>(handler.getCallingFunctions(monitor));
		callers.sort((a, b) -> a.getEntryPoint().compareTo(b.getEntryPoint()));

		DecompInterface ifc = new DecompInterface();
		ifc.setOptions(new DecompileOptions()); // without this, read-only pools are not folded
		ifc.openProgram(currentProgram);

		// file -> (function address -> first assert line)
		Map<String, TreeMap<Address, Long>> order = new TreeMap<>();
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "asserts.tsv");
		int sites = 0;
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("function\tfile\tline\texpr");
			for (Function f : callers) {
				DecompileResults r = ifc.decompileFunction(f, 60, monitor);
				HighFunction hf = r.getHighFunction();
				if (hf == null) {
					println("no decompile for " + f.getEntryPoint());
					continue;
				}
				StringBuilder note = new StringBuilder();
				Iterator<PcodeOpAST> ops = hf.getPcodeOps();
				while (ops.hasNext()) {
					PcodeOp op = ops.next();
					// SH-4 calls are jsr @rN: CALLIND with the target folded to a constant
					int opc = op.getOpcode();
					// (file, line, fmt[, expr...]): a plain fatal has three arguments
					if ((opc != PcodeOp.CALL && opc != PcodeOp.CALLIND)
						|| op.getInput(0).getOffset() != HANDLER || op.getNumInputs() < 4) {
						continue;
					}
					boolean hasExpr = op.getNumInputs() >= 5;
					Address site = op.getSeqnum().getTarget();
					String file = stringAt(op.getInput(1));
					Long lineVal = constValue(op.getInput(2));
					String fmt = stringAt(op.getInput(3));
					String expr = hasExpr ? stringAt(op.getInput(4)) : null;
					// decompiler merges shared pool entries across branches; read registers
					if (file == null) { Long c = regBefore(site, "r4"); file = c == null ? null : cString(c); }
					if (lineVal == null) lineVal = regBefore(site, "r5");
					if (fmt == null) { Long c = regBefore(site, "r6"); fmt = c == null ? null : cString(c); }
					if (expr == null && hasExpr) { Long c = regBefore(site, "r7"); expr = c == null ? null : cString(c); }
					// non-assert uses (printf-style): record the format instead
					if (fmt != null && !fmt.startsWith("assertion")) {
						expr = "fatal \"" + fmt.strip() + "\"";
					}
					if (file == null || lineVal == null) {
						println("unresolved assert args in " + f.getEntryPoint());
						continue;
					}
					if (file.startsWith(PREFIX)) {
						file = file.substring(PREFIX.length());
					}
					long line = lineVal;
					note.append(String.format("assert %s:%d: %s\n",
						file.substring(file.lastIndexOf('/') + 1), line, expr));
					w.printf("%s\t%s\t%d\t%s%n", f.getEntryPoint(), file, line, expr);
					order.computeIfAbsent(file, k -> new TreeMap<>())
						.merge(f.getEntryPoint(), line, Math::min);
					sites++;
				}
				if (note.length() > 0) {
					String old = f.getComment();
					String keep = old == null ? "" : old.replaceAll("(?m)^assert .*\n?", "");
					f.setComment((keep + note).strip());
				}
			}
		}
		ifc.dispose();
		println(String.format("%d assert sites in %d functions -> %s", sites, callers.size(), out));

		// Link-order check: by address, first-assert lines should rise.
		int ok = 0, bad = 0;
		for (Map.Entry<String, TreeMap<Address, Long>> e : order.entrySet()) {
			long prev = -1;
			boolean mono = true;
			for (long line : e.getValue().values()) {
				if (line < prev) {
					mono = false;
				}
				prev = line;
			}
			if (e.getValue().size() < 2) {
				continue;
			}
			if (mono) {
				ok++;
			} else {
				bad++;
				println("out of line order: " + e.getKey() + " " + e.getValue());
			}
		}
		println(String.format("line order by address: %d files rising, %d not", ok, bad));
	}
}
