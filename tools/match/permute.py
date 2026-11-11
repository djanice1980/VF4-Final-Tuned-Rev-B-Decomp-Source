"""Random search for a matching shape of near-miss functions.

    python3 -I tools/match/permute.py [--from results.tsv] [--top N] [--rounds R] [--width W]
                                      [--seed S] [--out FILE.json] [--shard K/N]

variants.py tries each rewrite systematically and gives up; this keeps
going. For each function auto_match.py found differing (DIFF), it measures
how far our code is from the game's: the edit distance between the two
sequences of 16-bit instruction words, relocated words wildcarded (so a
different pool order still counts as a near miss). The --top N closest are
searched: each round makes W mutants of the best text so far (one or two
random rewrites each), compiles them in one batch under V<k>_<address>
names, and keeps the closest; a tie is kept too, so the search can walk
across plateaus. It stops at distance 0 (then match.py's full check, bytes
and relocations, has the last word) or after R rounds.

Rewrites: variants.py's (store, return, inline, array, sign, cmp, ifswap)
and swapping the operands of a commutative operator, swapping two adjacent
statements, another integer type for a local, adding or dropping "!= 0".

--seeds FILE.json ({address: text}) starts each search from that text
instead of the decompiler's, and searches only those functions;
--skip M leaves out the M closest (another run has them);
--shard K/N searches only every N-th of the closest, starting at K (run N
copies at once, each with its own --out, to use more cores).
Writes FILE (default build/automatch/permuted.json): {address: text} for the
matches; adopt.py --texts FILE takes them. Progress goes to stdout.
"""
import difflib, json, os, random, re, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match as am
import delay_slots
import match
import variants as V

ROOT = am.ROOT
B = match.BASE


def m_commute(text):
    out = []
    for m in re.finditer(r"([\w.\[\]>-]+) (\+|\*|&|\||\^|==|!=) ([\w.\[\]>-]+)", text):
        if m.group(1) != m.group(3):
            out.append(text[:m.start()] + f"{m.group(3)} {m.group(2)} {m.group(1)}" + text[m.end():])
    return out


def m_swap(text):
    """Two adjacent simple statements (one line each, no braces) swapped."""
    out = []
    lines = text.split("\n")
    for i in range(len(lines) - 1):
        a, b = lines[i], lines[i + 1]
        simple = lambda l: l.strip().endswith(";") and not re.search(r"\b(return|if|while|for|goto|case|break)\b|[{}]", l) and "=" in l
        if simple(a) and simple(b) and a.strip() != b.strip():
            out.append("\n".join(lines[:i] + [b, a] + lines[i + 2:]))
    return out


TYPES = ["int", "uint", "short", "ushort", "char", "byte", "undefined4"]


def m_type(text):
    out = []
    for m in re.finditer(r"^([ \t]*)(int|uint|short|ushort|char|byte|undefined4|undefined2|undefined1) (\w+);", text, re.M):
        for t in TYPES:
            if t != m.group(2):
                out.append(text[:m.start()] + f"{m.group(1)}{t} {m.group(3)};" + text[m.end():])
    return out


def m_nonzero(text):
    out = []
    for m in re.finditer(r"if \(([\w.\[\]>-]+) != 0\)", text):
        out.append(text[:m.start()] + f"if ({m.group(1)})" + text[m.end():])
    for m in re.finditer(r"if \(([\w.\[\]>-]+)\)", text):
        out.append(text[:m.start()] + f"if ({m.group(1)} != 0)" + text[m.end():])
    return out


MUTATORS = V.RULES + [m_commute, m_swap, m_type, m_nonzero]


def mutate(text, rng):
    t = text
    for _ in range(rng.choice((1, 1, 2))):
        rules = MUTATORS[:]
        rng.shuffle(rules)
        for r in rules:
            opts = r(t)
            if opts:
                t = rng.choice(opts)
                break
    return t


def words(blob, masked):
    """Instruction words; a relocated word that holds the right address is a
    wildcard ("R"), a wrong one a word nothing matches ("X")."""
    return [(masked[j] if j in masked else blob[j:j + 2]) for j in range(0, len(blob) - 1, 2)]


def distance(code, game, masked):
    a, b = words(code, masked), words(game, {})
    # a relocated word matches whatever the game has there
    sm = difflib.SequenceMatcher(None, [w if w != "R" else None for w in a], b, autojunk=False)
    same = sum(bl.size for bl in sm.get_matching_blocks())
    wild = sum(1 for i, w in enumerate(a) if w == "R" and i < len(b))
    return max(len(a), len(b)) - same - wild


def score_batch(items, flags, img, symtab):
    """items: [(address, name, k, text)] -> {(address, k): distance} for those that compile."""
    funcs = [(None, None, None, f"V{k}_{a:08x}", re.sub(r"\b" + re.escape(n) + r"\b(?=\s*\()", f"V{k}_{a:08x}", t, count=1))
             for a, n, k, t in items]
    out = {}
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
        ok = am.split_compilable(funcs, [] if flags == "-" else [flags], tmp, [])
        if not ok:
            return out
        f = os.path.join(tmp, "batch.c")
        am.write_batch(f, ok, [] if flags == "-" else [flags])
        secs, syms, relocs = match.read_elf(match.compile_c(f, tmp))
    for si, s in enumerate(secs):
        if s["typ"] != 1 or not (s["flags"] & 4):
            continue
        for fs in (x for x in syms if x["shn"] == si and x["type"] == 2):
            m = re.match(r"_?V(\d+)_(0c[0-9a-f]{6})$", fs["name"])
            if not m:
                continue
            a, k = int(m.group(2), 16), int(m.group(1))
            lo, size = fs["value"], fs["size"]
            code = s["data"][lo:lo + size]
            game = img[a - B:a - B + max(size, game_size.get(a, size))]
            masked = {}
            for off, typ, sym, add in relocs.get(si, []):
                if not lo <= off < lo + size:
                    continue
                o = off - lo
                ok = True
                if typ == 1 and o + 4 <= len(game):
                    word = int.from_bytes(game[o:o + 4], "little")
                    addend = add + int.from_bytes(code[o:o + 4], "little")
                    if sym["name"] and sym["type"] != 3:
                        dest = match.address_of(sym["name"], symtab)
                        ok = dest is None or word == (dest + addend) & 0xFFFFFFFF
                for j in range(o & ~1, o + 4, 2):
                    masked[j] = "R" if ok else "X"
            out[(a, k)] = distance(code, game, masked)
    return out


game_size = {}


def main():
    args = sys.argv[1:]
    opt = lambda k, d: type(d)(args[args.index(k) + 1]) if k in args else d
    src = opt("--from", os.path.join(ROOT, "build", "automatch", "results.tsv"))
    top, rounds, width = opt("--top", 300), opt("--rounds", 30), opt("--width", 12)
    rng = random.Random(opt("--seed", 1))
    out = opt("--out", os.path.join(ROOT, "build", "automatch", "permuted.json"))
    rows = [l.split("\t") for l in open(src).read().split("\n")[1:] if l.count("\t") == 5]
    # flags from the build group, as auto_match picks them: the results row
    # says "-" for every function auto_match skipped, whatever its group is
    kind = am.kind_of(delay_slots.groups())
    flags_of = lambda a: "-fno-delayed-branch" if kind(a) == "empty" else "-"
    if "--seeds" in args:
        # start from these texts (an agent's best try) and search only them,
        # whatever auto_match made of the decompiler's text (often skipped)
        seeds = json.load(open(args[args.index("--seeds") + 1]))
        cands = {c[0]: c[:4] + (seeds[f"{c[0]:08x}"],) + tuple(c[5:])
                 for c in am.candidates() if f"{c[0]:08x}" in seeds}
    else:
        diff = {int(r[0], 16) for r in rows if r[5] == "DIFF"}
        cands = {c[0]: c for c in am.candidates() if c[0] in diff}
    want = {a: flags_of(a) for a in cands}
    for a, c in cands.items():
        game_size[a] = c[1]
    img = open(program.IMAGE, "rb").read()
    symtab = match.load_symbols()
    # 1. how far each one is now
    base = {}
    for flags in ("-fno-delayed-branch", "-"):
        group = [c for a, c in cands.items() if want[a] == flags]
        for i in range(0, len(group), 50):
            res = score_batch([(c[0], c[3], 0, c[4]) for c in group[i:i + 50]], flags, img, symtab)
            for (a, _), d in res.items():
                base[a] = d
    order = sorted(base, key=lambda a: (base[a], cands[a][1]))[opt("--skip", 0):top]
    if "--shard" in args:
        k, n = (int(x) for x in args[args.index("--shard") + 1].split("/"))
        order = order[k::n]
    print(f"{len(base)} near misses measured; searching the {len(order)} closest "
          f"(distance {base[order[0]] if order else '-'}..{base[order[-1]] if order else '-'})", flush=True)
    found = json.load(open(out)) if os.path.exists(out) else {}
    # 2. search, a few functions at a time so each batch has many mutants
    for i in range(0, len(order), 4):
        group = order[i:i + 4]
        best = {a: (base[a], cands[a][4]) for a in group if f"{a:08x}" not in found}
        for r in range(rounds):
            live = [a for a in best if best[a][0] > 0]
            if not live:
                break
            for flags in ("-fno-delayed-branch", "-"):
                items = []
                for a in live:
                    if want[a] != flags:
                        continue
                    for k in range(width):
                        items.append((a, cands[a][3], k, mutate(best[a][1], rng)))
                if not items:
                    continue
                res = score_batch(items, flags, img, symtab)
                for a, n, k, t in items:
                    d = res.get((a, k))
                    if d is not None and d <= best[a][0] and t != best[a][1]:
                        best[a] = (d, t)
        for a, (d, t) in best.items():
            if d == 0:
                # the last word: match.py's full check
                with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
                    f = os.path.join(tmp, "one.c")
                    am.write_batch(f, [(None, None, None, cands[a][3], t)], [] if want[a] == "-" else [want[a]])
                    try:
                        _, _, res = match.run_checks([f], img, symtab, match.game_slots(), say=lambda *x, **y: None)
                    except SystemExit:
                        res = []
                if any(x[3] == "match" for x in res):
                    found[f"{a:08x}"] = t
                    json.dump(found, open(out, "w"), indent=0)
            print(f"  0x{a:08x} {cands[a][3]}: distance {base[a]} -> {d}" + (" MATCH" if f"{a:08x}" in found else ""), flush=True)
    print(f"{len(found)} matching -> {out}")


if __name__ == "__main__":
    main()
