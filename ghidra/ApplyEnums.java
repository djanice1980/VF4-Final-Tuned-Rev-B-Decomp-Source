// Build enum types from the debug name tables and apply them, plus
// Sega's debug-menu labels, to global variables.
//
// Inputs (from tools/name_tables.py and tools/debug_menu.py):
//   ghidra/name_tables.tsv  string-pointer runs, classed enum/assets/text
//   ghidra/debug_menu.tsv   debug-menu item records
//
// Tables: a run is cut where a menu item points into it (a menu table's
// length is its max value + 1) and, in enum-class runs, where a .text
// literal pool points into it (code_ref): those mark table starts, e.g.
// MODE_* ends at entry 26 and STROKE_* starts at 49. Text-class runs
// only become enums where a menu uses them as an enum list.
//
// Enum names, first rule that applies: prefix shared by all members;
// common stem of the menu labels that use it; first word shared by 75%
// of members; joined members (3 or fewer); e_names_<address>.
//
// Globals: every menu item with an absolute target names that global
// after its label (when Ghidra has no better name). Enum-list items get
// the enum type at their size; float items get float. Bit-flag items
// only get a comment listing the bits.
//
// Writes ghidra/enums.tsv (enum, table, count, how named, users).
// @category VF4FT
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.data.ArrayDataType;
import ghidra.program.model.data.CategoryPath;
import ghidra.program.model.data.CharDataType;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeConflictHandler;
import ghidra.program.model.data.DataTypeManager;
import ghidra.program.model.data.DataUtilities;
import ghidra.program.model.data.EnumDataType;
import ghidra.program.model.data.FloatDataType;
import ghidra.program.model.data.PointerDataType;
import ghidra.program.model.listing.CodeUnit;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.symbol.Symbol;

public class ApplyEnums extends GhidraScript {

	static final CategoryPath CAT = new CategoryPath("/vf4ft/enums");
	// 1- and 2-byte copies for globals narrower than an int; kept apart so
	// headers can emit them as sized typedefs instead of duplicate members
	static final CategoryPath SIZED = new CategoryPath("/vf4ft/enums/sized");

	static class Run {
		long start;
		String cls;
		List<String> names = new ArrayList<>();
		Set<Long> codeRefs = new HashSet<>();
	}

	static class Item {
		long record, target, table, max;
		String label;
		int kind, flags, size;
		boolean absolute;
	}

	static class Table {
		long start;
		List<String> names;
		List<String> members = new ArrayList<>();
		String enumName, how;
		ghidra.program.model.data.Enum base;
		Map<Integer, DataType> bySize = new HashMap<>();
		Set<String> users = new TreeSet<>();
	}

	private DataTypeManager dtm;
	private final Set<String> usedEnumNames = new HashSet<>();

	static String member(String s, int i) {
		String m = s.strip().replaceAll("[^A-Za-z0-9_]+", "_").replaceAll("_+", "_")
			.replaceAll("^_|_$", "").toUpperCase();
		if (m.isEmpty()) {
			return "ENTRY_" + i;
		}
		return Character.isDigit(m.charAt(0)) ? "_" + m : m;
	}

	static String commonPrefix(List<String> ms) {
		String p = ms.get(0);
		for (String m : ms) {
			int n = 0;
			while (n < p.length() && n < m.length() && p.charAt(n) == m.charAt(n)) {
				n++;
			}
			p = p.substring(0, n);
		}
		int cut = p.lastIndexOf('_');
		return cut >= 1 ? p.substring(0, cut) : "";
	}

	// "GAME_TIME_1P", "GAME_TIME_VS" -> "GAME_TIME"
	static String labelStem(Set<String> labels) {
		List<String[]> toks = new ArrayList<>();
		for (String l : labels) {
			String m = member(l.replace("*", ""), 0).replaceAll("(_(1P|2P|VS|L|R|\\d+))+$", "");
			toks.add(m.split("_"));
		}
		if (toks.isEmpty()) {
			return "";
		}
		List<String> common = new ArrayList<>();
		for (int i = 0;; i++) {
			String w = null;
			for (String[] t : toks) {
				if (i >= t.length || (w != null && !w.equals(t[i]))) {
					return String.join("_", common);
				}
				w = t[i];
			}
			common.add(w);
		}
	}

	static String majorityWord(List<String> ms) {
		Map<String, Integer> c = new HashMap<>();
		for (String m : ms) {
			int u = m.indexOf('_');
			if (u > 0) {
				c.merge(m.substring(0, u), 1, Integer::sum);
			}
		}
		for (Map.Entry<String, Integer> e : c.entrySet()) {
			if (e.getValue() * 4 >= ms.size() * 3) {
				return e.getKey();
			}
		}
		return "";
	}

	private String enumName(Table t, List<String> members) {
		String base, how;
		String prefix = members.size() > 1 ? commonPrefix(members) : "";
		String stem = labelStem(t.users);
		String word = majorityWord(members);
		if (!prefix.isEmpty()) {
			base = "e_" + prefix.toLowerCase();
			how = "common prefix " + prefix + "_";
		} else if (!stem.isEmpty()) {
			base = "e_" + stem.toLowerCase();
			how = "menu labels " + String.join(", ", t.users);
		} else if (!word.isEmpty()) {
			base = "e_" + word.toLowerCase();
			how = "majority prefix " + word + "_";
		} else if (members.size() <= 3) {
			base = "e_" + String.join("_", members).toLowerCase();
			how = "members";
		} else {
			base = String.format("e_names_%08x", t.start);
			how = "placeholder";
		}
		String name = base;
		if (usedEnumNames.contains(name)) {
			name = String.format("%s_%08x", base, t.start);
		}
		usedEnumNames.add(name);
		t.how = how;
		return name;
	}

	private DataType sized(Table t, int bytes) {
		return t.bySize.computeIfAbsent(bytes, b -> {
			if (b == 4) {
				return t.base;
			}
			EnumDataType e = new EnumDataType(SIZED, t.enumName + "_" + (b * 8), b, dtm);
			for (long v : t.base.getValues()) {
				e.add(t.base.getName(v), v, t.base.getComment(t.base.getName(v)));
			}
			e.setDescription(t.base.getDescription());
			return dtm.addDataType(e, DataTypeConflictHandler.REPLACE_HANDLER);
		});
	}

	private List<String[]> readTsv(String file) throws Exception {
		List<String[]> rows = new ArrayList<>();
		File f = new File(getSourceFile().getParentFile().getAbsolutePath(), file);
		try (BufferedReader r = new BufferedReader(new FileReader(f))) {
			r.readLine();
			for (String l; (l = r.readLine()) != null;) {
				rows.add(l.split("\t", -1));
			}
		}
		return rows;
	}

	private boolean hasRealName(Address a) {
		Symbol s = getSymbolAt(a);
		return s != null && s.getSource() != SourceType.DEFAULT;
	}

	@Override
	protected void run() throws Exception {
		dtm = currentProgram.getDataTypeManager();

		Map<Long, Run> runs = new LinkedHashMap<>();
		for (String[] f : readTsv("name_tables.tsv")) {
			long a = Long.parseLong(f[0], 16);
			Run r = runs.computeIfAbsent(a, k -> new Run());
			r.start = a;
			r.cls = f[1];
			r.names.add(f[3]);
			if (f[4].equals("1")) {
				r.codeRefs.add(a + 4L * Integer.parseInt(f[2]));
			}
		}
		List<Item> items = new ArrayList<>();
		for (String[] f : readTsv("debug_menu.tsv")) {
			Item it = new Item();
			it.record = Long.parseLong(f[0], 16);
			it.label = f[1];
			it.kind = Integer.parseInt(f[2], 16);
			it.flags = Integer.parseInt(f[3], 16);
			it.size = Integer.parseInt(f[4]);
			it.target = Long.parseLong(f[5], 16);
			it.absolute = f[6].equals("1");
			it.table = Long.parseLong(f[7], 16);
			it.max = Long.parseLong(f[8], 16);
			items.add(it);
		}

		// menu-defined table lengths
		Map<Long, Integer> menuLen = new HashMap<>();
		for (Item it : items) {
			if (it.kind == 0x0F && it.table != 0) {
				menuLen.merge(it.table, (int) it.max + 1, Math::max);
			}
		}

		// cut runs into tables
		Map<Long, Table> tables = new TreeMap<>();
		for (Run r : runs.values()) {
			if (r.cls.equals("assets")) {
				continue;
			}
			long end = r.start + 4L * r.names.size();
			TreeSet<Long> cuts = new TreeSet<>();
			for (long m : menuLen.keySet()) {
				if (m > r.start && m < end) {
					cuts.add(m);
				}
			}
			if (r.cls.equals("enum")) {
				for (long c : r.codeRefs) {
					if (c > r.start) {
						cuts.add(c);
					}
				}
			}
			long cur = r.start;
			while (cur < end) {
				Long next = cuts.higher(cur);
				long stop = next == null ? end : next;
				Integer ml = menuLen.get(cur);
				if (ml != null) {
					stop = Math.min(end, cur + 4L * ml);
				}
				boolean keep = ml != null || r.cls.equals("enum");
				int n = (int) ((stop - cur) / 4);
				if (keep && n >= 2) {
					Table t = new Table();
					t.start = cur;
					int from = (int) ((cur - r.start) / 4);
					t.names = new ArrayList<>(r.names.subList(from, from + n));
					tables.put(cur, t);
				}
				cur = stop;
			}
		}

		// menu labels that use each table (for naming)
		for (Item it : items) {
			if (it.kind == 0x0F && tables.containsKey(it.table)) {
				tables.get(it.table).users.add(it.label.replace("*", "").strip());
			}
		}

		// members and enum names
		Map<String, Integer> uses = new HashMap<>();
		for (Table t : tables.values()) {
			Set<String> seen = new HashSet<>();
			for (int i = 0; i < t.names.size(); i++) {
				String m = member(t.names.get(i), i);
				if (!seen.add(m)) {
					m = m + "_" + i;
					seen.add(m);
				}
				t.members.add(m);
				uses.merge(m, 1, Integer::sum);
			}
			t.enumName = enumName(t, t.members);
		}
		// C enumerators share one namespace: prefix names used by several
		// enums (or reserved, below) with their enum's stem (e_cpu_mot_type: TA_GUARD ->
		// CPU_MOT_TYPE_TA_GUARD); the original string stays in the comment.
		// The stem drops a disambiguating _<address> unless the result
		// would still collide.
		// names the headers of the exported source define (<stddef.h>,
		// <stdint.h>, vf4ft_base.h) count as taken too: a light type table
		// has "NULL"
		for (String r : new String[] { "NULL", "offsetof", "bool", "true", "false" }) {
			if (uses.containsKey(r)) {
				uses.merge(r, 1, Integer::sum);
			}
		}
		int renamed = 0;
		// insertion order = table address order, so the rename is deterministic
		Map<Table, List<Integer>> clash = new LinkedHashMap<>();
		for (Table t : tables.values()) {
			for (int i = 0; i < t.members.size(); i++) {
				if (uses.get(t.members.get(i)) > 1) {
					clash.computeIfAbsent(t, k -> new ArrayList<>()).add(i);
				}
			}
		}
		Set<String> taken = new HashSet<>(uses.keySet());
		for (Map.Entry<Table, List<Integer>> e : clash.entrySet()) {
			Table t = e.getKey();
			String full = t.enumName.substring(2).toUpperCase();
			String shortStem = full.replaceAll("_[0-9A-F]{8}$", "");
			for (int i : e.getValue()) {
				String m = t.members.get(i);
				String bare = m.startsWith("_") ? m.substring(1) : m;
				String cand = m.startsWith(shortStem + "_") ? m : shortStem + "_" + bare;
				if (taken.contains(cand)) {
					cand = full + "_" + bare;
				}
				taken.add(cand);
				t.members.set(i, cand);
				renamed++;
			}
		}
		println(renamed + " enum members prefixed to keep C names unique");

		// create enum types and label/type the tables
		for (Table t : tables.values()) {
			List<String> members = t.members;
			EnumDataType e = new EnumDataType(CAT, t.enumName, 4, dtm);
			for (int i = 0; i < members.size(); i++) {
				String orig = t.names.get(i);
				e.add(members.get(i), i, orig.equals(members.get(i)) ? null : "\"" + orig + "\"");
			}
			e.setDescription(String.format("Values index the name table at 0x%08x (%d entries).",
				t.start, members.size()));
			t.base = (ghidra.program.model.data.Enum) dtm.addDataType(e, DataTypeConflictHandler.REPLACE_HANDLER);

			Address a = toAddr(t.start);
			DataType arr = new ArrayDataType(new PointerDataType(CharDataType.dataType),
				members.size(), 4);
			try {
				DataUtilities.createData(currentProgram, a, arr, -1,
					DataUtilities.ClearDataMode.CLEAR_ALL_CONFLICT_DATA);
			} catch (Exception ex) {
				println("could not type table " + a + ": " + ex.getMessage());
			}
			if (!hasRealName(a)) {
				createLabel(a, t.enumName + "_names", true, SourceType.USER_DEFINED);
			}
		}

		// apply menu labels and types to globals
		Map<Long, List<Item>> byTarget = new TreeMap<>();
		for (Item it : items) {
			if (it.absolute) {
				byTarget.computeIfAbsent(it.target, k -> new ArrayList<>()).add(it);
			}
		}
		Set<String> usedNames = new HashSet<>();
		int named = 0, typed = 0, failed = 0;
		for (Map.Entry<Long, List<Item>> e : byTarget.entrySet()) {
			Address a = toAddr(e.getKey());
			List<Item> its = e.getValue();
			Item first = its.get(0);
			StringBuilder note = new StringBuilder();
			for (Item it : its) {
				String bit = it.kind == 0x0E ? String.format(" bit %d", it.flags) : "";
				note.append(String.format("debug menu \"%s\" kind 0x%02x%s (record 0x%08x)\n",
					it.label, it.kind, bit, it.record));
			}

			DataType dt = null;
			if (first.kind == 0x0F && first.table != 0 && tables.containsKey(first.table)) {
				Table t = tables.get(first.table);
				dt = sized(t, 1 << first.size);
			} else if (first.kind == 0x00) {
				dt = FloatDataType.dataType;
			}
			if (dt != null) {
				// re-runs: our own earlier type is fine to replace; anything
				// else that is not default data is a real conflict
				ghidra.program.model.listing.Data have = getDataAt(a);
				boolean ours = have != null && (have.getDataType().isEquivalent(dt)
					|| have.getDataType().getCategoryPath().getPath().startsWith("/vf4ft"));
				try {
					DataUtilities.createData(currentProgram, a, dt, -1, ours
						? DataUtilities.ClearDataMode.CLEAR_ALL_CONFLICT_DATA
						: DataUtilities.ClearDataMode.CLEAR_ALL_DEFAULT_CONFLICT_DATA);
					typed++;
				} catch (Exception ex) {
					failed++;
					note.append("type not applied: ").append(ex.getMessage()).append('\n');
				}
			}

			if (!hasRealName(a)) {
				String name = first.kind == 0x0E
					? String.format("bitflags_%08x", e.getKey())
					: member(first.label.replace("*", ""), 0).toLowerCase();
				// a label of one or two letters ("r", "g") only made sense
				// inside its submenu, so keep the address with it
				if (name.length() <= 2 && !name.startsWith("bitflags")) {
					name = String.format("%s_%08x", name, e.getKey());
				}
				if (!usedNames.add(name)) {
					name = String.format("%s_%08x", name, e.getKey());
					usedNames.add(name);
				}
				createLabel(a, name, true, SourceType.USER_DEFINED);
				named++;
			}
			setPlateComment(a, note.toString().strip());
		}

		File out = new File(getSourceFile().getParentFile().getAbsolutePath(), "enums.tsv");
		try (PrintWriter w = new PrintWriter(out)) {
			w.println("enum\ttable\tcount\tnamed_by\tmenu_users");
			for (Table t : tables.values()) {
				w.printf("%s\t%08x\t%d\t%s\t%s%n", t.enumName, t.start, t.names.size(), t.how,
					String.join(", ", t.users));
			}
		}
		println(String.format("%d enums; %d globals named, %d typed (%d type conflicts) -> %s",
			tables.size(), named, typed, failed, out));
	}
}
