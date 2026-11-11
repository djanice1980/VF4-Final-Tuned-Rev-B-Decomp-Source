// Name functions identified by an exact match: with public source code, or
// byte for byte (relocations masked) with a symbol-carrying library object
// (ghidra/known_functions.tsv: address, name, signature, evidence). These
// count as "named" in src/: the name is the original symbol of the
// matched code.
//
// signature (optional) gives exact storage, for routines outside the C
// calling convention: "int@FPUL __sdivsi3_i4(int dividend@r4, int divisor@r5)" means
// it returns in FPUL. Without it the decompiler expects r0, and callers
// show the real result as extraout_FPUL. Types: int, uint, and "void *" for
// an address ("void *dst@r4").
// @category VF4FT
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

import java.util.ArrayList;
import java.util.List;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.IntegerDataType;
import ghidra.program.model.data.UnsignedIntegerDataType;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Parameter;
import ghidra.program.model.listing.ParameterImpl;
import ghidra.program.model.listing.ReturnParameterImpl;
import ghidra.program.model.listing.VariableStorage;
import ghidra.program.model.symbol.SourceType;

public class ApplyKnownNames extends GhidraScript {
	private DataType type(String t) {
		switch (t) {
		case "int":
			return IntegerDataType.dataType;
		case "uint":
			return UnsignedIntegerDataType.dataType;
		case "void *":
			// an address of no particular type (the block addresses of __movstr_i4_odd)
			return currentProgram.getDataTypeManager().getPointer(ghidra.program.model.data.VoidDataType.dataType);
		default:
			throw new IllegalArgumentException("unknown type " + t);
		}
	}

	private VariableStorage reg(String r) throws Exception {
		return new VariableStorage(currentProgram, currentProgram.getRegister(r));
	}

	// "int@FPUL name(int a@r4, int b@r5)", or "void name(...)" (no return register)
	private void applySignature(Function fn, String sig) throws Exception {
		java.util.regex.Matcher m = java.util.regex.Pattern
			.compile("(\\w+)(?:@(\\w+))?\\s+\\w+\\((.*)\\)").matcher(sig.strip());
		if (!m.matches() || (m.group(2) == null) != m.group(1).equals("void")) {
			throw new IllegalArgumentException("bad signature " + sig);
		}
		ReturnParameterImpl ret = m.group(1).equals("void")
			? new ReturnParameterImpl(ghidra.program.model.data.VoidDataType.dataType, VariableStorage.VOID_STORAGE, currentProgram)
			: new ReturnParameterImpl(type(m.group(1)), reg(m.group(2)), currentProgram);
		List<Parameter> params = new ArrayList<>();
		for (String p : m.group(3).split(",")) {
			java.util.regex.Matcher pm = java.util.regex.Pattern
				.compile("(\\w+(?: \\*)?)\\s*\\b(\\w+)@(\\w+)").matcher(p.strip());
			if (!pm.matches()) {
				throw new IllegalArgumentException("bad parameter " + p);
			}
			params.add(new ParameterImpl(pm.group(2), type(pm.group(1)), reg(pm.group(3)), currentProgram));
		}
		fn.setCustomVariableStorage(true);
		fn.updateFunction(null, ret, params, Function.FunctionUpdateType.CUSTOM_STORAGE, true,
			SourceType.USER_DEFINED);
	}

	@Override
	protected void run() throws Exception {
		File in = new File(getSourceFile().getParentFile().getAbsolutePath(), "known_functions.tsv");
		int n = 0;
		try (BufferedReader r = new BufferedReader(new FileReader(in))) {
			r.readLine();
			for (String l; (l = r.readLine()) != null;) {
				String[] f = l.split("\t", -1);
				Function fn = getFunctionAt(toAddr(Long.parseLong(f[0], 16)));
				if (fn == null && getInstructionAt(toAddr(Long.parseLong(f[0], 16))) == null) {
					// code nothing calls (an exception vector's handler) was never disassembled
					disassemble(toAddr(Long.parseLong(f[0], 16)));
				}
				if (fn == null) {
					fn = createFunction(toAddr(Long.parseLong(f[0], 16)), f[1]);
				}
				fn.setName(f[1], SourceType.USER_DEFINED);
				if (!f[2].isEmpty()) {
					applySignature(fn, f[2]);
				}
				fn.setComment("name: named\n" + f[3]);
				n++;
			}
		}
		println(n + " known functions named from " + in.getName());
	}
}
