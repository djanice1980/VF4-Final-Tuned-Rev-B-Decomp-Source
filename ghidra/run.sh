#!/usr/bin/env bash
# Ghidra passes for work/main.bin. Project lives in work/ghidra/vf4ft.
#
#   ghidra/run.sh import    install SH-4 variant, fresh import + Naomi 2 map + auto-analysis
#   ghidra/run.sh discover  prologue scan, then auto-analysis of what it found, then code
#                           flowing into a guessed address table wins (FlowOverAddressTables.java)
#   ghidra/run.sh sections  split .text/.rodata/.data, FPU entry state, fmov widths (SZ context)
#   ghidra/run.sh annotate  known functions (libgcc...), fatal_at, every assert on its caller
#   ghidra/run.sh enums     enums from name tables, debug-menu names/types on globals
#   ghidra/run.sh types     structs from ghidra/types/, names/signatures from ghidra/symbols/,
#                           then object layouts from base + offset addressing (AssignAggregates.java),
#                           then pointers from globals whose value is used as an address (AssignPointers.java),
#                           then floats from globals the FPU moves (AssignFloats.java),
#                           then signed bytes/halfwords/words from how loads are extended and compared (AssignSigns.java)
#   ghidra/run.sh prototypes  convention names no function uses freed first (PruneCallingConventions.java: the program
#                           holds at most 126), then
#                           conventions for callees that keep argument registers (AssignCallEffects.java;
#                           what a routine's body does not show is in ghidra/call_effects_overrides.tsv),
#                           then parameter lists for functions without one (AssignPrototypes.java),
#                           then returns from the register each call uses (AssignReturns.java)
#   ghidra/run.sh export    source-file map + regenerate src/ (+ work/delay_slots.tsv)
#   ghidra/run.sh all       import, discover, sections, annotate, enums, types, prototypes, export
#   ghidra/run.sh test      import work/test.bin beside it, carry names over from main.bin (TransferFromMain.java),
#                           export its source to src_test/
#
# Close the Ghidra GUI first: headless runs cannot open a project it has open.
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
hl=/opt/ghidra/support/analyzeHeadless
proj=("$root/work/ghidra" vf4ft)
common=(-scriptPath "$root/ghidra" -max-cpu 16)
mkdir -p "$root/work/ghidra"

# analyzeHeadless exits 0 even when a script fails to compile or throws (and a
# compile error in one script breaks loading all of ghidra/), so look for its
# script error reports and stop the run there
hl_run() {
    local out status
    out=$(mktemp)
    set +e
    "$hl" "$@" 2>&1 | tee "$out"
    status=${PIPESTATUS[0]}
    set -e
    if grep -q "SCRIPT ERROR" "$out"; then
        grep -A3 "SCRIPT ERROR" "$out" >&2
        rm -f "$out"
        echo "run.sh: a Ghidra script failed (above); stopping" >&2
        exit 1
    fi
    rm -f "$out"
    return "$status"
}

# progress/index.html is refreshed after every step (tools/progress.py)
trap 'status=$?; [[ $status -eq 0 && -n ${step:-} ]] && python3 -I "$root/tools/progress.py" update "$step"' EXIT
step=${1:-}
[[ $step == all ]] && step=

case ${1:-} in
import)
    # SH-4 variant with a working FPU-mode model (ghidra/lang/)
    "$root/ghidra/lang/install.sh"
    hl_run "${proj[@]}" -import "$root/work/main.bin" -overwrite \
        -loader BinaryLoader -loader-baseAddr 0x0C020000 \
        -processor SuperH4:LE:32:vf4ft \
        "${common[@]}" -preScript SetupNaomi2.java ;;
discover)
    hl_run "${proj[@]}" -process main.bin "${common[@]}" -preScript FindPrologues.java \
        -postScript FlowOverAddressTables.java ;;
sections)
    hl_run "${proj[@]}" -process main.bin -noanalysis "${common[@]}" -postScript SplitSections.java \
        -postScript SetFpuEntryState.java -postScript AssignFpuContext.java ;;
annotate)
    hl_run "${proj[@]}" -process main.bin -noanalysis "${common[@]}" -postScript ApplyKnownNames.java \
        -postScript AnnotateAsserts.java ;;
enums)
    python3 -I "$root/tools/name_tables.py" "$root/work/main.bin" "$root/ghidra/name_tables.tsv"
    python3 -I "$root/tools/debug_menu.py" "$root/work/main.bin" "$root/ghidra/debug_menu.tsv"
    hl_run "${proj[@]}" -process main.bin -noanalysis "${common[@]}" -postScript ApplyEnums.java ;;
types)
    python3 -I "$root/tools/mode_table.py" "$root/work/main.bin" "$root"
    # mode_table.py writes ghidra/symbols/gen_mode.tsv afresh, without the
    # live observations of its rows: put them back from the verdicts
    python3 -I "$root/tools/observe_apply.py" "$root/ghidra/live_verdicts.tsv" "$root/ghidra/exercise_verdicts.tsv" > /dev/null
    hl_run "${proj[@]}" -process main.bin -noanalysis "${common[@]}" -postScript ApplyTypes.java \
        -postScript AssignAggregates.java -postScript AssignPointers.java -postScript AssignFloats.java -postScript AssignSigns.java ;;
prototypes)
    hl_run "${proj[@]}" -process main.bin -noanalysis "${common[@]}" -postScript PruneCallingConventions.java \
        -postScript AssignCallEffects.java \
        -postScript AssignPrototypes.java -postScript AssignReturns.java ;;
export)
    rm -rf "$root/src"
    hl_run "${proj[@]}" -process main.bin -noanalysis "${common[@]}" \
        -postScript MapSourceFiles.java -postScript ExportSource.java -postScript TextLayout.java \
        -postScript ReportDelaySlots.java ;;
test)
    # the test-mode program, a second program in the same project; names,
    # signatures and globals come over from main.bin where the code is the same
    hl_run "${proj[@]}" -import "$root/work/test.bin" -overwrite \
        -loader BinaryLoader -loader-baseAddr 0x0C020000 \
        -processor SuperH4:LE:32:vf4ft \
        "${common[@]}" -preScript SetupNaomi2.java
    rm -rf "$root/src_test"
    hl_run "${proj[@]}" -process test.bin "${common[@]}" -preScript FindPrologues.java \
        -postScript FlowOverAddressTables.java -postScript TransferFromMain.java -postScript ApplyTypes.java \
        -postScript MapSourceFiles.java -postScript ExportSource.java -postScript ReportDelaySlots.java ;;
all)
    for step in import discover sections annotate enums types prototypes export; do "$0" "$step"; done ;;
*)
    sed -n '2,18p' "$0"; exit 2 ;;
esac
