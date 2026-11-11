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
DATED = re.compile(r"^observed live (\d{4}-\d{2}-\d{2}): ")


def main():
    dry = "--dry" in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith("-")]
    if not files:
        sys.exit(__doc__)
    verdicts = {}
    for f in files:
        for line in open(f, encoding="utf-8"):
            p = line.rstrip("\n").split("\t")
            if len(p) < 4 or p[2] not in ("observed", "reached", "conflict"):
                continue
            verdicts[p[0].lower()] = (p[1], p[2], " ".join(p[3].split()), p[4].strip() if len(p) > 4 else "")
    raised = kept = appended = skipped = 0
    found = set()
    for path in sorted(glob.glob(os.path.join(ROOT, "ghidra", "symbols", "*.tsv"))):
        lines = open(path, encoding="utf-8").read().split("\n")
        changed = False
        for i, line in enumerate(lines):
            p = line.split("\t")
            if len(p) != 5 or p[1] != "function" or p[0] not in verdicts:
                continue
            name, verdict, text, _ = verdicts[p[0]]
            found.add(p[0])
            if verdict != "observed":
                continue
            m = DATED.match(text)
            if not m:
                sys.exit(f"{p[0]} {name}: the observation does not start 'observed live <date>: '")
            if f"observed live {m.group(1)}: " in p[4]:
                skipped += 1
            else:
                p[4] = p[4].rstrip().rstrip(";") + "; " + text
                appended += 1
            if p[3] == "inferred":
                p[3] = "observed"
                raised += 1
            elif p[3] == "named":
                kept += 1
            new = "\t".join(p)
            if new != line:
                lines[i] = new
                changed = True
        if changed and not dry:
            with open(path, "w", encoding="utf-8") as f:
                f.write("\n".join(lines))
    count = {v: sum(1 for x in verdicts.values() if x[1] == v) for v in ("observed", "reached", "conflict")}
    print(f"{len(verdicts)} verdicts: {count['observed']} observed, {count['reached']} reached, {count['conflict']} conflicts")
    print(f"{appended} observations appended ({skipped} already there); {raised} rows raised to observed, "
          f"{kept} named rows keep their level")
    for a in sorted(set(verdicts) - found):
        print(f"no symbols row: {a} {verdicts[a][0]} ({verdicts[a][1]})")
    for a in sorted(a for a, v in verdicts.items() if v[1] == "conflict"):
        print(f"conflict: {a} {verdicts[a][0]}")


if __name__ == "__main__":
    main()
