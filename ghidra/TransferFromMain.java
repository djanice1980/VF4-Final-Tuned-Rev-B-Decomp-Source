// Names, signatures and notes from main.bin onto the same code in test.bin.
// The test program is built from many of the same objects; an identical
// function has identical instruction bytes (only its constant pool holds
// different addresses, everything being laid out elsewhere). Each main.bin
// function's bytes up to its first rts and delay slot (at least 12 bytes;
// no pool words in there) that occur exactly once in main.bin and exactly
// once in test.bin mark the same function: it is created in test.bin if
// need be and given main.bin's name (when that is not a placeholder), its
// signature (when one was committed) and comment, with a line saying where
// it came from. Then the pool words of each such pair map main.bin's data
// to test.bin's: a word at the same offset in both functions points at the
// same object, so a named global gets its name (and type, where test.bin has
// none) at the address test.bin's word holds. Run on test.bin, with main.bin
// in the same project. Writes ghidra/test_transfer.tsv.
// @category VF4FT
import java.io.File;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import ghidra.app.cmd.function.ApplyFunctionSignatureCmd;
import ghidra.app.script.GhidraScript;
import ghidra.framework.model.DomainFile;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.AbstractStringDataType;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeConflictHandler;
import ghidra.program.model.data.DataUtilities;
import ghidra.program.model.data.FunctionDefinitionDataType;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.Program;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;

public class TransferFromMain extends GhidraScript {

	static final long BASE = 0x0C020000L;

	private static String hex(byte[] b) {
		StringBuilder s = new StringBuilder(b.length * 2);
		for (byte x : b) {
			s.append(String.format("%02x", x & 0xFF));
		}
		return s.toString();
	}

	/** Bytes from the entry to the first rts (0x000B) and its delay slot. */
	private static byte[] head(Program p, Function f) throws Exception {
		int size = (int) f.getBody().getNumAddresses();
		byte[] b = new byte[Math.min(size, 0x2000)];
		p.getMemory().getBytes(f.getEntryPoint(), b);
		for (int j = 0; j + 1 < b.length; j += 2) {
			if (b[j] == 0x0B && b[j + 1] == 0x00) {
				int n = Math.min(j + 4, b.length);
				byte[] h = new byte[n];
				System.arraycopy(b, 0, h, 0, n);
				return h;
			}
		}
		return null;
	}

	@Override
	protected void run() throws Exception {
		if (currentProgram.getName().equals("main.bin")) {
			throw new IllegalStateException("run on test.bin");
		}
		DomainFile df = state.getProject().getProjectData().getRootFolder().getFile("main.bin");
		Program main = (Program) df.getReadOnlyDomainObject(this, DomainFile.DEFAULT_VERSION, monitor);
		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "test_transfer.tsv");
		int named = 0, signed = 0, created = 0, globals = 0, pairs = 0;
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("test address\tmain address\tname\twhat");
			// main.bin functions by their heads, kept only when the head is unique there
			Map<String, List<Function>> byHead = new HashMap<>();
			for (Function f : main.getFunctionManager().getFunctions(true)) {
				byte[] h = head(main, f);
				if (h != null && h.length >= 12) {
					byHead.computeIfAbsent(hex(h), k -> new ArrayList<>()).add(f);
				}
			}
			byte[] test = new byte[(int) currentProgram.getMemory().getBlock(toAddr(BASE)).getSize()];
			currentProgram.getMemory().getBytes(toAddr(BASE), test);
			String testHex = hex(test);
			for (Map.Entry<String, List<Function>> e : byHead.entrySet()) {
				if (e.getValue().size() != 1 || monitor.isCancelled()) {
					continue;
				}
				String key = e.getKey();
				int at = testHex.indexOf(key);
				while (at >= 0 && at % 4 != 0) { // a match at a byte boundary inside a word: keep looking
					at = testHex.indexOf(key, at + 1);
				}
				if (at < 0) {
					continue;
				}
				int again = testHex.indexOf(key, at + 2);
				while (again >= 0 && again % 4 != 0) {
					again = testHex.indexOf(key, again + 1);
				}
				if (again >= 0) {
					continue; // more than once in test.bin
				}
				Function fm = e.getValue().get(0);
				Address ta = toAddr(BASE + at / 2);
				Function ft = getFunctionAt(ta);
				if (ft == null) {
					if (getInstructionAt(ta) == null) {
						disassemble(ta);
					}
					ft = createFunction(ta, null);
					if (ft == null) {
						continue;
					}
					created++;
				}
				pairs++;
				String what = "same code";
				if (fm.getSymbol().getSource() != SourceType.DEFAULT) {
					ft.setName(fm.getName(), SourceType.USER_DEFINED);
					named++;
					what += ", name";
				}
				if (fm.getSignatureSource() == SourceType.USER_DEFINED) {
					FunctionDefinitionDataType fd = new FunctionDefinitionDataType(fm.getSignature());
					DataType r = currentProgram.getDataTypeManager().resolve(fd, DataTypeConflictHandler.DEFAULT_HANDLER);
					if (r instanceof ghidra.program.model.data.FunctionDefinition && new ApplyFunctionSignatureCmd(ta,
						(ghidra.program.model.data.FunctionDefinition) r, SourceType.USER_DEFINED).applyTo(currentProgram)) {
						signed++;
						what += ", signature";
					}
				}
				String note = "same code as main.bin " + fm.getEntryPoint() + " (ghidra/TransferFromMain.java)";
				ft.setComment(fm.getComment() == null ? note : fm.getComment() + "\n" + note);
				w.printf("%s\t%s\t%s\t%s%n", ta, fm.getEntryPoint(), fm.getName(), what);
				globals += pools(main, fm, ft, w);
			}
		} finally {
			main.release(this);
		}
		println(String.format("from main.bin: %d functions paired (%d created), %d named, %d signatures, %d globals named -> %s",
			pairs, created, named, signed, globals, out));
	}

	/** Pool words at the same offset in both: main.bin's named data -> test.bin's address. */
	private int pools(Program main, Function fm, Function ft, PrintWriter w) throws Exception {
		int n = 0;
		long delta = ft.getEntryPoint().getOffset() - fm.getEntryPoint().getOffset();
		for (Instruction i : main.getListing().getInstructions(fm.getBody(), true)) {
			for (Reference r : i.getReferencesFrom()) {
				Address pm = r.getToAddress();
				if (!pm.isMemoryAddress() || pm.getOffset() >= 0x0C15DA00L) {
					continue;
				}
				Data pd = main.getListing().getDataAt(pm);
				if (pd == null || !pd.isPointer() || pd.getLength() != 4) {
					continue;
				}
				long vm = main.getMemory().getInt(pm) & 0xFFFFFFFFL;
				Address pt = toAddr(pm.getOffset() + delta);
				if (!currentProgram.getMemory().contains(pt)) {
					continue;
				}
				long vt = currentProgram.getMemory().getInt(pt) & 0xFFFFFFFFL;
				Symbol s = main.getSymbolTable().getPrimarySymbol(main.getAddressFactory().getDefaultAddressSpace().getAddress(vm));
				if (s == null || s.getSource() != SourceType.USER_DEFINED || main.getFunctionManager().getFunctionAt(s.getAddress()) != null) {
					continue;
				}
				// a row still named by its address (DAT_0c5cb000) names main.bin's
				// address, not this one, and can be test.bin's own default name
				// for other data there: two declarations of one name
				if (s.getName().matches("(?:DAT|PTR|BYTE|WORD|DWORD|LAB|UNK)_[0-9a-f]{8}")) {
					continue;
				}
				Address at = toAddr(vt);
				if (!currentProgram.getMemory().contains(at) || getSymbolAt(at) != null && getSymbolAt(at).getSource() == SourceType.USER_DEFINED) {
					continue;
				}
				createLabel(at, s.getName(), true, SourceType.USER_DEFINED);
				Data dm = main.getListing().getDataAt(s.getAddress());
				Data dt = getDataAt(at);
				if (dm != null && dt != null && dt.getDataType() instanceof AbstractStringDataType
						&& !(dm.getDataType() instanceof AbstractStringDataType)) {
					// analysis made a string here; main.bin's declared type (char[n]) replaces it,
					// since the exporter cannot write Ghidra's string type as C
					removeData(dt);
					dt = null;
				}
				if (dm != null && dt == null) {
					try {
						DataType t = currentProgram.getDataTypeManager().resolve(dm.getDataType(), DataTypeConflictHandler.DEFAULT_HANDLER);
						DataUtilities.createData(currentProgram, at, t, -1, DataUtilities.ClearDataMode.CLEAR_ALL_UNDEFINED_CONFLICT_DATA);
					} catch (Exception e) {
						// the type does not fit there: the name alone
					}
				}
				w.printf("%s\t%s\t%s\tglobal%n", at, s.getAddress(), s.getName());
				n++;
			}
		}
		return n;
	}
}
