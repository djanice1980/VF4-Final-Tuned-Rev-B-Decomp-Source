"""Put the judged live observations into ghidra/symbols/.

    python3 -I tools/observe_apply.py VERDICTS.tsv [...] [--dry]

A verdicts file has one routine per line: address, name, verdict,
observation, correction (tab separated). The verdicts come from reading
the call log of a running game (tools/flycast_observe.py,
tools/observe_summary.py; docs/live_observation.md says how the log is
made and judged) against each routine's name and description:

  observed   the log confirms a specific claim of the description (a
             value, a pairing of arguments and results, a timing);
  reached    the routine ran, but the log shows nothing the code does not
             already compel;
  conflict   the log contradicts the name or the description.

A second kind of verdicts file (ghidra/exercise_verdicts.tsv) is for the
routines the game never runs: the harness called them itself, with inputs
a judge chose from the description (build/agents/exercised/BRIEF.md):

  exercised  the results bore out what the description says the code
             does;
  untested   no honest test could be made (the reason is the sentence);
  conflict   the results contradict the description.

An exercised routine gets its sentence ("exercised <date>: ...") and its
row goes from inferred to exercised: a level below observed, since the
inputs were the judge's and not the game's. A row that is observed or
named keeps its level and still gets the sentence.

For an observed routine this appends the observation sentence ("observed
live <date>: ...") to the evidence of its row in ghidra/symbols/*.tsv and
raises the row's confidence from inferred to observed. A row that is
already named (its name comes from the game itself) keeps that level and
still gets the sentence. Reached rows are left as they are. Conflicts are
listed and left for a person: the description has to be corrected by hand.

Running it again changes nothing: a row whose evidence already holds the
same dated sentence is skipped.
"""
import glob, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATED = {"observed": re.compile(r"^(observed live \d{4}-\d{2}-\d{2}): "),
         "exercised": re.compile(r"^(exercised \d{4}-\d{2}-\d{2}): ")}
KINDS = ("observed", "reached", "conflict", "exercised", "untested")


def main():
    dry = "--dry" in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith("-")]
    if not files:
        sys.exit(__doc__)
    verdicts = {}
    for f in files:
        for line in open(f, encoding="utf-8"):
            p = line.rstrip("\n").split("\t")
            if len(p) < 4 or p[2] not in KINDS:
                continue
            # a routine can have a verdict of each kind (reached in the game, exercised in the harness)
            verdicts.setdefault(p[0].lower(), []).append((p[1], p[2], " ".join(p[3].split()), p[4].strip() if len(p) > 4 else ""))
    raised = kept = appended = skipped = 0
    by = {}
    found = set()
    for path in sorted(glob.glob(os.path.join(ROOT, "ghidra", "symbols", "*.tsv"))):
        lines = open(path, encoding="utf-8").read().split("\n")
        changed = False
        for i, line in enumerate(lines):
            p = line.split("\t")
            if len(p) != 5 or p[1] != "function" or p[0] not in verdicts:
                continue
            found.add(p[0])
            for name, verdict, text, _ in sorted(verdicts[p[0]], key=lambda v: KINDS.index(v[1])):
                if verdict not in DATED:
                    continue
                m = DATED[verdict].match(text)
                if not m:
                    sys.exit(f"{p[0]} {name}: the sentence does not start '{verdict} ... <date>: '")
                if m.group(1) + ": " in p[4]:
                    skipped += 1
                else:
                    p[4] = p[4].rstrip().rstrip(";") + "; " + text
                    appended += 1
                if p[3] == "inferred" or (p[3] == "exercised" and verdict == "observed"):
                    p[3] = verdict
                    raised += 1
                    by[verdict] = by.get(verdict, 0) + 1
                else:
                    kept += 1
            new = "\t".join(p)
            if new != line:
                lines[i] = new
                changed = True
        if changed and not dry:
            with open(path, "w", encoding="utf-8") as f:
                f.write("\n".join(lines))
    count = {k: sum(1 for x in verdicts.values() for v in x if v[1] == k) for k in KINDS}
    print(f"{len(verdicts)} routines with a verdict: " + ", ".join(f"{count[k]} {k}" for k in KINDS if count[k]))
    print(f"{appended} sentences appended ({skipped} already there); {raised} rows raised "
          f"({', '.join(f'{n} to {k}' for k, n in sorted(by.items())) or 'none'}), {kept} rows keep their level")
    for a in sorted(set(verdicts) - found):
        print(f"no symbols row: {a} {verdicts[a][0][0]} ({verdicts[a][0][1]})")
    for a in sorted(a for a, x in verdicts.items() if any(v[1] == "conflict" for v in x)):
        print(f"conflict: {a} {verdicts[a][0][0]}")


if __name__ == "__main__":
    main()
