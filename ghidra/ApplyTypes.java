// Apply recovered types and symbols.
//
//   ghidra/types/*.h       C type definitions (plain C, no preprocessor),
//                          parsed in name order, filed under /vf4ft/types
//   ghidra/bogus_entries.tsv  function starts analysis made inside a routine,
//                          removed first (address, evidence); test.bin's
//                          in ghidra/bogus_entries_test.tsv
//   ghidra/symbols/*.tsv   address, kind (data | function), C declaration,
//                          confidence (named | observed | inferred), evidence
//
// data rows: the declaration's type is created at the address and the name
// becomes the primary label; a bare name only labels the address (the data
// there keeps its type, an object AssignAggregates lays out keeps its layout). function rows: the prototype is applied as the
// function's signature and name; a bare name (no parentheses) only names it
// (creating the function where none exists yet; one that analysis started
// in the nop padding before the row's address is removed first).
// A variadic prototype ("...") only names the function: with this calling
// convention Ghidra drops varargs passed in registers once a "..." prototype
// is set (snprintf's r7 argument vanished at every call), while an
// unprototyped call keeps the arguments the decompiler sees. The full
// declaration still reaches src/include/vf4ft_functions.h: ExportSource
// declares every function with a row as its row says (Ghidra also drops
// const). Each gets "name: <confidence>" and
// "evidence: ..." lines in its comment (other comment lines are kept), so
// src/ shows why the name is what it is.
//
// Everything is parsed into a scratch type manager by one CParser (a fresh
// parser only finds types by name in the root category), then copied into
// the program with a replace policy, so re-runs update types in place.
// @category VF4FT
import java.io.File;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import ghidra.app.cmd.function.ApplyFunctionSignatureCmd;
import ghidra.app.script.GhidraScript;
import ghidra.app.util.cparser.C.CParser;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.Array;
import ghidra.program.model.data.ArrayDataType;
import ghidra.program.model.data.Category;
import ghidra.program.model.data.CategoryPath;
import ghidra.program.model.data.Composite;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeComponent;
import ghidra.program.model.data.DataTypeConflictHandler;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.DataUtilities;
import ghidra.program.model.data.FunctionDefinition;
import ghidra.program.model.data.Pointer;
import ghidra.program.model.data.StandAloneDataTypeManager;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;

public class ApplyTypes extends GhidraScript {

	static final CategoryPath CAT = new CategoryPath("/vf4ft/types");
	static final Pattern DATA_DECL = Pattern.compile("^(.*?)\\b([A-Za-z_][A-Za-z0-9_]*)\\s*((?:\\[\\s*\\d+\\s*\\])*)\\s*$");

	private CParser cp;

	private DataType parse(String what, String text) throws Exception {
		DataType dt = cp.parse(text);
		if (!cp.didParseSucceed()) {
			throw new IllegalStateException(what + ": " + cp.getParseMessages());
		}
		return dt;
	}

	private static String note(String conf, String ev, String old) {
		String rest = old == null ? "" : old.replaceAll("(?m)^(name|evidence): .*\\n?", "").strip();
		return "name: " + conf + "\nevidence: " + ev + (rest.isEmpty() ? "" : "\n" + rest);
	}

	/** The address after an instruction of the function that falls through
	 *  into bytes that are not an instruction (AssignPrototypes.cutShort's
	 *  test), or null. */
	private Address fallsIntoData(Function f) {
		for (ghidra.program.model.listing.Instruction i : currentProgram.getListing().getInstructions(f.getBody(), true)) {
			if (i.getDelaySlotDepth() > 0) {
				continue; // judged at its slot
			}
			if (i.isInDelaySlot()) {
				ghidra.program.model.listing.Instruction owner = i.getPrevious();
				if (owner == null || !owner.getFlowType().hasFallthrough()) {
					continue; // rts, bra, jmp
				}
			} else if (!i.getFlowType().hasFallthrough()) {
				continue;
			}
			Address next = i.getMaxAddress().add(1);
			if (getInstructionAt(next) == null) {
				return next;
			}
		}
		return null;
	}

	/** A function row inside a function that starts in the alignment padding
	 *  before it (only nops between that entry and the row's address: an
	 *  entry analysis made in the padding, 0x0C02C716 before 0x0C02C720): the
	 *  padded function is removed, so the row's function can be made at its
	 *  real start. Anything else at a row's address inside a function is an
	 *  error. */
	private void dropPaddedStart(Row r) throws Exception {
		if (getFunctionAt(r.addr) != null) {
			return;
		}
		Function host = getFunctionContaining(r.addr);
		if (host == null) {
			return;
		}
		Address a = host.getEntryPoint();
		while (a.compareTo(r.addr) < 0) {
			ghidra.program.model.listing.Instruction in = getInstructionAt(a);
			if (in == null || !in.getMnemonicString().replaceFirst("^_", "").equals("nop")) {
				throw new IllegalStateException(r.table + ": " + r.addr + " is inside " + host.getName()
					+ ", which does not start in nop padding before it");
			}
			a = in.getMaxAddress().next();
		}
		println("removing " + host.getName() + ": it starts in the padding before " + r.addr);
		removeFunction(host);
	}

	static class Row {
		String table, kind, decl, conf, ev, name;
		Address addr;
		DataType type; // in the scratch manager until resolved
	}

	@Override
	protected void run() throws Exception {
		DataTypeManager dtm = currentProgram.getDataTypeManager();
		File root = new File(getSourceFile().getParentFile().getAbsolutePath());

		// 0. function starts analysis made in the middle of a routine
		// (ghidra/bogus_entries.tsv: address, evidence): removed, so the
		// routine's real start can be made from its symbol row
		// (test.bin's, at its own addresses: ghidra/bogus_entries_test.tsv)
		File bogus = new File(root, currentProgram.getName().equals("main.bin") ? "bogus_entries.tsv" : "bogus_entries_test.tsv");
		if (bogus.exists()) {
			List<String> lines = Files.readAllLines(bogus.toPath());
			for (String line : lines.subList(1, lines.size())) {
				if (line.isBlank()) {
					continue;
				}
				Address ba = toAddr(Long.parseLong(line.split("\t", -1)[0], 16));
				Function f = getFunctionAt(ba);
				if (f != null) {
					println("removing " + f.getName() + ": bogus_entries.tsv");
					removeFunction(f);
				}
				// the routine it was cut from: its flow stopped at the bogus
				// entry, so its body is followed again (the fall-through body,
				// a shared exit it branches to)
				Function host = getFunctionBefore(ba);
				if (host != null && !host.getBody().contains(ba)) {
					ghidra.app.cmd.function.CreateFunctionCmd.fixupFunctionBody(currentProgram, host, monitor);
					if (host.getBody().contains(ba)) {
						println("bogus_entries: " + host.getName() + " now covers " + ba);
					}
				}
			}
		}

		StandAloneDataTypeManager scratch = new StandAloneDataTypeManager("vf4ft-parse");
		int tx = scratch.startTransaction("parse");
		try {
			cp = new CParser(scratch, true, new DataTypeManager[] { dtm });

			// 1. headers
			File[] headers = new File(root, "types").listFiles((d, n) -> n.endsWith(".h"));
			Arrays.sort(headers);
			for (File h : headers) {
				cp.setParseFileName(h.getName());
				parse(h.getName(), new String(Files.readAllBytes(h.toPath())));
			}

			// 2. symbol rows, parsed with the same parser
			List<Row> rows = new ArrayList<>();
			// main.bin's rows in ghidra/symbols/, test.bin's (its own addresses) in ghidra/symbols_test/
			File[] tables = new File(root, currentProgram.getName().equals("main.bin") ? "symbols" : "symbols_test")
				.listFiles((d, n) -> n.endsWith(".tsv"));
			Arrays.sort(tables);
			for (File t : tables) {
				List<String> lines = Files.readAllLines(t.toPath());
				for (String line : lines.subList(1, lines.size())) {
					if (line.isBlank()) {
						continue;
					}
					String[] f = line.split("\t", -1);
					Row r = new Row();
					r.table = t.getName();
					r.addr = toAddr(Long.parseLong(f[0], 16));
					r.kind = f[1];
					r.decl = f[2];
					r.conf = f[3];
					r.ev = f[4];
					if (r.kind.equals("data") && r.decl.strip().matches("[A-Za-z_][A-Za-z0-9_]*")) {
						r.name = r.decl.strip(); // name only: the data there keeps its type
					} else if (r.kind.equals("data")) {
						// "T name[a][b]": parse the element declaration, build the arrays
						Matcher m = DATA_DECL.matcher(r.decl);
						if (!m.matches()) {
							throw new IllegalStateException(r.table + ": bad declaration " + r.decl);
						}
						r.name = m.group(2);
						parse(r.table + " " + r.decl, m.group(1) + " " + r.name + ";");
						DataType dt = cp.getLastDataType();
						Matcher dims = Pattern.compile("\\[\\s*(\\d+)\\s*\\]").matcher(m.group(3));
						List<Integer> ns = new ArrayList<>();
						while (dims.find()) {
							ns.add(Integer.parseInt(dims.group(1)));
						}
						for (int i = ns.size() - 1; i >= 0; i--) {
							dt = new ArrayDataType(dt, ns.get(i), dt.getLength(), scratch);
						}
						r.type = dt;
					} else if (r.kind.equals("function") && !r.decl.contains("(")) {
						r.name = r.decl.strip(); // name only, no signature
					} else if (r.kind.equals("function") && r.decl.contains("...")) {
						Matcher m = Pattern.compile("([A-Za-z_][A-Za-z0-9_]*)\\s*\\(").matcher(r.decl);
						if (!m.find()) {
							throw new IllegalStateException(r.table + ": bad declaration " + r.decl);
						}
						r.name = m.group(1); // variadic: named only, see above
					} else if (r.kind.equals("function")) {
						DataType dt = parse(r.table + " " + r.decl, r.decl + ";");
						if (!(dt instanceof FunctionDefinition)) {
							throw new IllegalStateException(r.table + ": not a prototype: " + r.decl);
						}
						r.type = dt;
						r.name = dt.getName();
					} else {
						throw new IllegalStateException(r.table + ": unknown kind " + r.kind);
					}
					rows.add(r);
				}
			}

			// 3. file the header types under /vf4ft/types and copy them in
			List<DataType> made = new ArrayList<>();
			made.addAll(cp.getComposites().values());
			made.addAll(cp.getEnums().values());
			made.addAll(cp.getTypes().values());
			// an anonymous struct or union is named from the parser's running count
			// (_union_41), which moves whenever a header gains a struct, so the copy
			// step below could meet last run's copy of a different one under that
			// name (ct_data_s.dl took the name of inflate_huft_s.word's union, and
			// zlib stopped compiling); name each after its place, <outer>_<field>,
			// and file it with the header types (the parser lists only named ones)
			Pattern anon = Pattern.compile("_(struct|union)_\\d+");
			for (List<DataType> pass = new ArrayList<>(made); !pass.isEmpty();) {
				List<DataType> renamed = new ArrayList<>();
				for (DataType t : pass) {
					if (!(t instanceof Composite) || t.getDataTypeManager() != scratch || t.isDeleted()
							|| anon.matcher(t.getName()).matches()) {
						continue;
					}
					for (DataTypeComponent c : ((Composite) t).getDefinedComponents()) {
						DataType ct = c.getDataType();
						while (ct instanceof Array || ct instanceof Pointer) {
							ct = ct instanceof Array ? ((Array) ct).getDataType() : ((Pointer) ct).getDataType();
						}
						if (ct != null && ct.getDataTypeManager() == scratch && anon.matcher(ct.getName()).matches()) {
							String field = c.getFieldName() != null ? c.getFieldName() : "field" + c.getOrdinal();
							ct.setName(t.getName() + "_" + field);
							renamed.add(ct);
						}
					}
				}
				made.addAll(renamed);
				pass = renamed;
			}
			// earlier runs' copies under counted names, left at the root or in
			// /vf4ft/types; gone before the copy, or it resolves a renamed type to
			// an equivalent old one and keeps the counted name
			List<String> stale = new ArrayList<>();
			for (CategoryPath cp2 : new CategoryPath[] { CategoryPath.ROOT, CAT }) {
				Category c = dtm.getCategory(cp2);
				for (DataType t : c == null ? new DataType[0] : c.getDataTypes()) {
					if (anon.matcher(t.getName()).matches()) {
						stale.add(t.getName());
						dtm.remove(t, monitor);
					}
				}
			}
			if (!stale.isEmpty()) {
				println("types: removed the counted anonymous types " + stale);
			}
			// on a re-run the parser also finds last run's copies in the program
			// (a "struct tree_desc_s" use resolves there), so scratch can hold one
			// under /vf4ft/types beside the header's new definition at the root:
			// the header wins, taking the copy's place and its references
			int nTypes = 0;
			List<String> pulled = new ArrayList<>();
			for (DataType t : made) {
				if (t.getDataTypeManager() == scratch && !t.isDeleted() && t.getCategoryPath().isRoot()) {
					DataType old = scratch.getDataType(CAT, t.getName());
					if (old != null && old != t) {
						scratch.replaceDataType(old, t, true);
						pulled.add(t.getName());
					} else {
						t.setCategoryPath(CAT);
					}
				}
			}
			if (!pulled.isEmpty()) {
				println("types: replaced last run's copies of " + pulled);
			}
			for (DataType t : made) {
				if (t.getDataTypeManager() == scratch && !t.isDeleted()) {
					dtm.addDataType(t, DataTypeConflictHandler.REPLACE_HANDLER);
					nTypes++;
				}
			}

			// 4. function rows with no function yet, from the highest address
			// down: a routine with an entry part way in (a hand-written renderer
			// whose hook entry is 10 bytes after its start) then ends at that
			// entry, as the existing 2-byte functions before their float entries do
			// (one that an earlier run made across a row's entry, at another
			// row's address, is made again the same way)
			java.util.Map<Address, Row> byAddr = new java.util.HashMap<>();
			for (Row r : rows) {
				if (!r.kind.equals("data")) {
					byAddr.put(r.addr, r);
				}
			}
			for (Row r : byAddr.values()) {
				Function host = getFunctionAt(r.addr) == null ? getFunctionContaining(r.addr) : null;
				if (host != null && byAddr.containsKey(host.getEntryPoint())) {
					println("making " + host.getName() + " again: it runs over the row at " + r.addr);
					removeFunction(host);
				}
			}
			List<Row> fresh = new ArrayList<>();
			for (Row r : byAddr.values()) {
				if (getFunctionAt(r.addr) == null && getFunctionContaining(r.addr) == null) {
					fresh.add(r);
				}
			}
			fresh.sort((a, b) -> b.addr.compareTo(a.addr));
			for (Row r : fresh) {
				if (getInstructionAt(r.addr) == null) {
					// a curated row is a deliberate decision: data analysis made at the
					// start (a pointer it took the first words for) gives way to code
					Data d = getDataContaining(r.addr);
					if (d != null) {
						println("clearing " + d.getDataType().getName() + " at " + d.getAddress() + " for the row at " + r.addr);
						removeData(d);
					}
					disassemble(r.addr);
				}
				if (createFunction(r.addr, r.name) == null) {
					throw new IllegalStateException(r.table + ": no function at " + r.addr);
				}
			}

			// 4b. a row's routine whose code falls through into a pointer nothing
			// reads: analysis took the words after its start for pointers too
			// before the row made the start code, so the disassembly stopped at
			// them (coli_poly_contains_hit was two instructions, a 4-byte
			// function, and ghidra/source_map.tsv gave it size 4). The pointer
			// gives way, the code is followed on and the body made again.
			for (Row r : byAddr.values()) {
				Function fn = getFunctionAt(r.addr);
				for (int guard = 0; fn != null && guard < 64; guard++) {
					Address stop = fallsIntoData(fn);
					Data d = stop == null ? null : getDataAt(stop);
					if (d == null || !d.isDefined() || !(d.getDataType() instanceof Pointer)
						|| getReferencesTo(stop).length != 0) {
						break;
					}
					println("clearing " + d.getDataType().getName() + " at " + stop + ": " + r.name + " runs on into it");
					removeData(d);
					disassemble(stop);
					ghidra.app.cmd.function.CreateFunctionCmd.fixupFunctionBody(currentProgram, fn, monitor);
				}
			}

			// 5. apply rows
			int nData = 0, nFunc = 0;
			// the names ApplyEnums gave debug-menu targets (absolute ones), and
			// every row's name: a menu label inside a curated object that no row
			// keeps is a field of that object, not a global of its own
			Set<Long> menuTargets = new HashSet<>();
			for (String l : currentProgram.getName().equals("main.bin")
				? Files.readAllLines(new File(root, "debug_menu.tsv").toPath()) : new ArrayList<String>()) {
				String[] f = l.split("\t", -1);
				if (f.length > 6 && f[6].equals("1") && f[5].matches("[0-9a-f]{8}")) {
					menuTargets.add(Long.parseLong(f[5], 16));
				}
			}
			Set<String> rowNames = new HashSet<>();
			for (Row r : rows) {
				rowNames.add(r.name);
			}
			for (Row r : rows) {
				if (r.kind.equals("data") && r.type == null) {
					// a name alone names the object and leaves its data as it is (an
					// object AssignAggregates lays out from the code keeps that layout)
					Symbol s = createLabel(r.addr, r.name, true, SourceType.USER_DEFINED);
					s.setPrimary();
					String old = currentProgram.getListing().getComment(CodeUnit.PLATE_COMMENT, r.addr);
					setPlateComment(r.addr, note(r.conf, r.ev, old));
					nData++;
				} else if (r.kind.equals("data")) {
					DataType dt = dtm.resolve(r.type, DataTypeConflictHandler.DEFAULT_HANDLER);
					// a curated row is a deliberate decision: it replaces whatever data
					// analysis put there (pointers inside a table), never instructions
					if (getInstructionContaining(r.addr) != null || (dt.getLength() > 1 && currentProgram.getListing()
						.getInstructions(new ghidra.program.model.address.AddressSet(r.addr, r.addr.add(dt.getLength() - 1)), true)
						.hasNext())) {
						throw new IllegalStateException(r.table + ": code inside " + r.decl + " at " + r.addr);
					}
					DataUtilities.createData(currentProgram, r.addr, dt, -1,
						DataUtilities.ClearDataMode.CLEAR_ALL_CONFLICT_DATA);
					// ApplyEnums labels every debug name table it finds, also one that
					// is a member of a curated object (the layer names in each
					// sel_chara_info record), and every debug-menu target, also a field
					// of one (j_ex_00's parameters); a placeholder table name inside the
					// object is not a global of its own, so it goes, and so is a menu
					// label no row keeps inside an object the menu edits from its first
					// byte (the menu's entries walk that object's fields); inside other
					// objects (camera, robs, user_back_data ...) a menu label names a
					// member and stays as its alias
					if (dt.getLength() > 1) {
						List<Symbol> inside = new ArrayList<>();
						for (Symbol in : currentProgram.getSymbolTable().getSymbols(
							new ghidra.program.model.address.AddressSet(r.addr.add(1), r.addr.add(dt.getLength() - 1)),
							ghidra.program.model.symbol.SymbolType.LABEL, true)) {
							if (in.getSource() != SourceType.USER_DEFINED || rowNames.contains(in.getName())) {
								continue;
							}
							if (in.getName().matches("e_names_[0-9a-f]{8}_names")
								|| (menuTargets.contains(r.addr.getOffset())
									&& menuTargets.contains(in.getAddress().getOffset()))) {
								inside.add(in);
							}
						}
						for (Symbol in : inside) {
							boolean menu = !in.getName().matches("e_names_[0-9a-f]{8}_names");
							println("dropped " + (menu ? "debug-menu" : "name-table") + " label " + in.getName() + " inside " + r.name);
							if (menu) {
								setPlateComment(in.getAddress(), null);
							}
							in.delete();
						}
					}
					Symbol s = createLabel(r.addr, r.name, true, SourceType.USER_DEFINED);
					s.setPrimary();
					String old = currentProgram.getListing().getComment(CodeUnit.PLATE_COMMENT, r.addr);
					setPlateComment(r.addr, note(r.conf, r.ev, old));
					nData++;
				} else if (r.type == null) {
					dropPaddedStart(r);
					Function fn = getFunctionAt(r.addr);
					if (fn == null && getInstructionAt(r.addr) == null) {
						// code nothing calls was never disassembled (an exception entry)
						disassemble(r.addr);
					}
					if (fn == null) {
						fn = createFunction(r.addr, r.name);
					}
					if (fn == null) {
						throw new IllegalStateException(r.table + ": no function at " + r.addr);
					}
					fn.setName(r.name, SourceType.USER_DEFINED);
					fn.setComment(note(r.conf, r.ev, fn.getComment()));
					nFunc++;
				} else {
					// the C parser leaves the convention "unknown", which applies
					// as locked custom storage ("Unknown calling convention"
					// in every decompile); use the cspec's default prototype
					FunctionDefinition fd = (FunctionDefinition) r.type.copy(r.type.getDataTypeManager());
					fd.setCallingConvention("__stdcall");
					dropPaddedStart(r);
					Function fn = getFunctionAt(r.addr);
					if (fn == null && getInstructionAt(r.addr) == null) {
						// code nothing calls was never disassembled (zlib's get_crc_table,
						// zlibVersion, zError, _tr_tally)
						disassemble(r.addr);
					}
					if (fn == null) {
						fn = createFunction(r.addr, r.name);
					}
					if (fn == null) {
						throw new IllegalStateException(r.table + ": no function at " + r.addr);
					}
					// a convention AssignCallEffects measured (__keeps_...) stays on
					// the routine: without this a types run after a prototypes run
					// took it off every hand-written routine that has a declaration
					String measured = fn.getCallingConventionName();
					if (!new ApplyFunctionSignatureCmd(r.addr, fd, SourceType.USER_DEFINED).applyTo(currentProgram)) {
						throw new IllegalStateException(r.table + ": signature failed at " + r.addr + ": " + r.decl);
					}
					if (measured != null && measured.startsWith("__keeps")) {
						fn.setCallingConvention(measured);
						fn.setSignatureSource(SourceType.USER_DEFINED);
					}
					fn.setName(r.name, SourceType.USER_DEFINED);
					fn.setComment(note(r.conf, r.ev, fn.getComment()));
					nFunc++;
				}
			}
			Category cat = dtm.getCategory(CAT);
			println(String.format("types: %d from %d headers (/vf4ft/types holds %d); symbols: %d data, %d functions",
				nTypes, headers.length, cat == null ? 0 : cat.getDataTypes().length, nData, nFunc));
		} finally {
			scratch.endTransaction(tx, false);
			scratch.close();
		}
	}
}
