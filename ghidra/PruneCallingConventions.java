// Free the calling-convention names nothing uses any more.
//
// A program keeps its calling-convention names in a table of at most 126
// entries (ids 2-127), and Ghidra has no call that removes one. Every
// __keeps_... convention AssignCallEffects.java ever gave a function stays
// there, so as the measured conventions change from run to run the table
// fills, and then a new one cannot be assigned: "Unable to assign calling
// convention ... allocation capacity exceeded", and the function is left
// with an unknown convention (2026-11-10: 119 names in use, the table full
// after two runs with a few new names each).
//
// Runs before AssignCallEffects.java, which works every assignment out
// again: each function that has a __keeps_... convention is put back to the
// default one, then every name in the table that no function and no function
// type uses is deleted from it (through the program's database handle, the
// only way there is), and the data type manager's cache is dropped so that
// the freed ids can be given out in the same run.
// @category VF4FT
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import db.DBHandle;
import db.DBRecord;
import db.RecordIterator;
import db.Table;
import ghidra.app.script.GhidraScript;
import ghidra.program.database.ProgramDB;
import ghidra.program.database.data.DataTypeManagerDB;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.FunctionDefinition;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;

public class PruneCallingConventions extends GhidraScript {

	@Override
	protected void run() throws Exception {
		int reset = 0;
		Set<String> used = new HashSet<>();
		for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
			String cc = f.getCallingConventionName();
			if (cc != null && cc.startsWith("__keeps")) {
				SourceType src = f.getSignatureSource();
				f.setCallingConvention(Function.DEFAULT_CALLING_CONVENTION_STRING);
				if (f.getSignatureSource() != src) {
					f.setSignatureSource(src);
				}
				reset++;
			}
			else if (cc != null) {
				used.add(cc);
			}
		}
		java.util.Iterator<DataType> it = currentProgram.getDataTypeManager().getAllDataTypes();
		while (it.hasNext()) {
			DataType dt = it.next();
			if (dt instanceof FunctionDefinition) {
				String cc = ((FunctionDefinition) dt).getCallingConventionName();
				if (cc != null) {
					used.add(cc);
				}
			}
		}
		DBHandle dbh = ((ProgramDB) currentProgram).getDBHandle();
		int tables = 0, kept = 0;
		List<String> dropped = new ArrayList<>();
		for (Table t : dbh.getTables()) {
			if (!t.getName().endsWith("Calling Conventions")) {
				continue;
			}
			tables++;
			List<db.Field> keys = new ArrayList<>();
			RecordIterator ri = t.iterator();
			while (ri.hasNext()) {
				DBRecord rec = ri.next();
				String name = rec.getString(0);
				if (used.contains(name)) {
					kept++;
				}
				else {
					keys.add(rec.getKeyField());
					dropped.add(name);
				}
			}
			for (db.Field k : keys) {
				t.deleteRecord(k);
			}
		}
		if (tables != 1) {
			throw new IllegalStateException("expected one calling-convention table, found " + tables);
		}
		((DataTypeManagerDB) currentProgram.getDataTypeManager()).invalidateCache();
		println(String.format("calling conventions: %d functions put back to the default for AssignCallEffects; "
			+ "%d names freed from the program's table, %d kept (%s)", reset, dropped.size(), kept, used));
	}
}
