"""One function: the game's instructions against our compile's.

    python3 -I tools/match/diff.py ADDRESS [file.c] [--flags F] [-n CONTEXT]

Compiles the function at ADDRESS (hex) from its C in src/, or from file.c
(the function alone, as it would sit in a source file; a "#define
NOPROTO_x" line in front is honoured), with its build group's flags as
auto_match.py picks them (-fno-delayed-branch where the game's C leaves the
delay slots empty; or --flags, "-" for none), and prints a unified
diff of both instruction listings (pools and alignment padding dropped,
addresses masked), then match.py's verdict for it. For trying an edit by
hand: change file.c, run again.
"""
import difflib, os, re, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match as am
import classify as cl
import delay_slots
import match

ROOT = am.ROOT


def main():
    args = sys.argv[1:]
    opt = lambda k, d: (str if d is None else type(d))(args[args.index(k) + 1]) if k in args else d
    pos = [a for i, a in enumerate(args) if not a.startswith("-") and (i == 0 or args[i - 1] not in ("--flags", "-n"))]
    a = int(pos[0], 16)
    c = {x[0]: x for x in am.candidates()}.get(a)
    if c is None:
        sys.exit(f"0x{a:08x}: not an unmatched function in {program.SRC_REL}/")
    text = open(pos[1]).read() if len(pos) > 1 else c[4]
    flags = opt("--flags", None)
    if flags is None:
        # from the build group itself, not a results file: auto_match writes
        # "-" for every function it skips, which would hide the group's flags
        flags = "-fno-delayed-branch" if am.kind_of(delay_slots.groups())(a) == "empty" else "-"
    img = open(program.IMAGE, "rb").read()
    name = f"V0_{a:08x}"
    t = re.sub(r"\b" + re.escape(c[3]) + r"\b(?=\s*\()", name, text, count=1)
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
        f = os.path.join(tmp, "one.c")
        am.write_batch(f, [(None, None, None, None, t)], [] if flags == "-" else [flags])
        secs, syms, _ = match.read_elf(match.compile_c(f, tmp))
        detail = []
        _, _, res = match.run_checks([f], img, match.load_symbols(), match.game_slots(), verbose=True,
                                     say=lambda *x, **k: detail.append(" ".join(str(y) for y in x)))
    s = next(x for x in syms if x["name"].lstrip("_") == name)
    code = secs[s["shn"]]["data"][s["value"]:s["value"] + s["size"]]
    g = [cl.norm(x, 0) for x in cl.insns(img[a - match.BASE:a - match.BASE + c[1]], a)]
    m = [cl.norm(x, 0) for x in cl.insns(code, a)]
    print(f"0x{a:08x} {c[3]}  flags {flags}  game {len(g)} instructions, ours {len(m)}")
    shown = 0
    for l in difflib.unified_diff(g, m, "game", "ours", n=opt("-n", 3), lineterm=""):
        print("  " + l)
        shown += 1
    if not shown and any(r[3] != "match" for r in res):
        # the listings mask addresses and constants: a pool word, a frame
        # size or a branch distance differs; match.py's own report shows which
        print("  (the listings agree; what differs, from match.py:)")
        for l in detail:
            print("  " + l)
    print("verdict:", ", ".join(r[3] for r in res) or "?")


if __name__ == "__main__":
    main()
