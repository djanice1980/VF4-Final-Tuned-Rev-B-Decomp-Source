"""Near-miss matching with a local model behind an OpenAI-compatible server
(vLLM, Ollama) in place of an agent: the model gets the brief's patterns,
the decompiler's C and the instruction diff, answers with one C text,
the text is compiled and diffed against the game, and the diff goes back
to the model until it matches or the attempts run out. match.py stays the
judge: a text is kept only on verdict "match".

    python3 -I tools/match/local_match.py --batch 207 [options]
    python3 -I tools/match/local_match.py --addrs 0c12b9a0,0c140c80
    python3 -I tools/match/local_match.py --from-json build/agents/merged-K-adopt.json \
        --include-matched --sample 20 --seed 1 --tag control     # the control experiment

Options: --server URL (default http://192.168.1.64:8080/v1), --model NAME,
--jobs N functions at a time (default 6), --attempts N per function (12),
--max-tokens N per reply (16000), --no-think (turn the model's reasoning
off), --diff-lines N shown to the model (80), --tag T (names the outputs),
--dry (print the first prompt and the baseline diff, call no model).
Sampling: --temperature (0.6 thinking, 0.7 not), --top-p (0.95), --top-k
and --min-p (the server's defaults when left out), --presence-penalty (0).
Qwen's settings for its non-thinking mode are --top-p 0.8 --top-k 20
--min-p 0 --presence-penalty 1.5.

A reply whose text is one the model already tried (comments and spacing
aside) is not compiled again: the model is told which attempt it repeats.
--prefill starts every reply with the opening of the ```c block, so the
model answers with code and no reasoning before it.

Replies stream through a guard (--no-guard turns it off; --prefill does
too) that cuts one when it loops: the same long line --guard-lines times
(4) among the last 80 lines of prose, the same --guard-ngram words (12)
four times, or past --reason-budget tokens (8000). Text inside ``` fences
is not checked, nor anything while a fence is open. After a cut the reply
is kept up to where the repetition began and the model continues it from
a fixed bridge that opens the ```c block, with --final-tokens (4096) to
write the function; when a budget cut finds the function already written,
that text is taken as it is. Leaving the request cancels the generation
on the server. With thinking on, a cut ends the reply without the bridge.

The harness, not the model, decides each attempt's state and when to
stop. Every feedback message opens with a STATE block: the status
(IMPROVED, NO_GAIN, WORSE, NO_COMPILE, REPEAT, NO_CODE, CUT_OFF, RULE),
this text's and the best text's diff lines, game instructions in place
and first difference, the change against both, and the required next
step. A function stops on a match, after --attempts, after --patience (3)
attempts in a row without a new best, or past --fn-tokens completion
tokens (0: no limit). <addr>.state.jsonl beside the log has one line per
attempt.

--hunk works one hunk at a time instead of asking for the whole function:
the model sees the current C with line numbers, the first hunk of the diff
and the C lines our instructions in it come from (the compile carries -g,
which leaves the code as it is, for the line table), and answers with
--edits (3) different SEARCH/REPLACE edits. The harness applies and
compiles each and keeps the best one that leaves no fewer game
instructions in place and the first difference no earlier, and improves
one of them or the diff. A hunk that resists --hunk-patience (2) calls
gives way to the next one; any kept edit brings the focus back to the
first. --attempts counts calls; --patience calls in a row with no edit
kept stop the function.

A reply whose text repeats an earlier one is sampled again at once, up to
--resamples times (3), each --resample-step (0.3) hotter, without spending
an attempt; only a repeat that survives them counts as REPEAT.

Besides the log, build/agents/local/<tag>/<addr>.chat.md keeps the whole
conversation: the first prompt, every reply in full (with the reasoning
when the server returns it) and every feedback message; system.md beside
it holds the system prompt.

Outputs: build/automatch/local-<tag>.json, the matching texts in the form
adopt.py --texts takes; build/agents/local/<tag>/<addr>.log, every attempt
with its diff; a summary on stdout.
"""
import bisect, concurrent.futures, difflib, json, os, random, re, subprocess, sys, tempfile, threading, time, urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match as am
import classify as cl
import match

ROOT = am.ROOT
BRIEF = os.path.join(ROOT, "build", "agents", "BRIEF.md")

RULES = """You match decompiled C against a game's machine code. The target is
Virtua Fighter 4 Final Tuned for the Sega Naomi 2 (SH-4, little-endian),
built with GCC 3.2.3 at -O1. You are given a function's C as the decompiler
wrote it, and a unified diff of the game's instructions ("game") against
what our compile of the current C gives ("ours"). Rewrite the C so that it
compiles to the game's instructions exactly.

Rules:
- Answer with ONE ```c code block holding the whole function (and, when
  needed, `#define NOPROTO_<callee>` / `#define KRPROTO_<callee>` lines at
  the very top of the block). Nothing else is used; keep any commentary to
  one or two lines outside the block.
- Keep the function's name and keep every parameter the callers pass.
  The header already declares every FUN_xxx, DAT_xxx, agg_xxx and named
  symbol; do not declare them yourself. Types available: undefined,
  undefined1/2/4, byte, uchar, ushort, uint, float, int, short, char, and
  the typedefs the C already uses.
- Never write a main-RAM address (0x0c......) as a number: use the symbol.
  Hardware addresses (0xa0000000 and up, 0xff......) as constants are fine,
  e.g. *(volatile uint *)0xff000028.
- Never change a callee's prototype by casting the callee to a function
  pointer type. If the game loads a callee's arguments in another order
  than the prototype says, put `#define KRPROTO_<callee>` at the top and
  write the call plainly with the arguments in the order the game loads
  them (floats fr4.., ints r4..). If the game calls a void callee through
  r0 (expects a return value), use `#define NOPROTO_<callee>`.
- Only the instruction stream matters: register choice, instruction
  order, which branch comes first, what sits in a delay slot. Equal
  semantics with different instructions is a miss.
- Work from the diff: find the first hunk, decide which source shape
  would make GCC 3.2 emit the game's instructions there, and change only
  that. Then the next hunk.
- The decompiler's C already does exactly what the game does. Never change
  what the code computes: never negate or invert a condition on its own,
  never change a constant, an index, an offset or an operator. When a
  branch or a computation looks different in the diff, the cause is the
  shape of the source (which block comes first, a goto, an early return, a
  temporary, the order of declarations, the form of a loop), not the logic.
  Swapping an if's two blocks together with negating its condition keeps
  the behavior and is allowed.
- Never write register names (r0..r15, fr0..fr15) or stack offsets in the C.

Reading the diff:
- Every line starts with GAME (only in the game's code), OURS (only in
  our compile of the current C) or BOTH (in both). The game's code is the
  BOTH and GAME lines in order; ours is the BOTH and OURS lines in order.
  Never read GAME and OURS lines as one sequence: they are two different
  versions of the same stretch of code.
- "@@" starts a hunk; the unchanged lines between hunks are left out.
- Branch targets are labels L1, L2, ... numbered in address order, and
  the line a label marks starts with it ("L3:"). The game and ours number
  their labels separately; when a BOTH line differs between the two,
  ours follows after "| ours:".
- "=" marks a load from the literal pool and shows what is loaded:
  `mov.l =agg_0c37d65c,r1` puts the address of agg_0c37d65c in r1,
  `mov.w =173,r4` puts 173 in r4. Two different values there are a real
  difference.

SH-4 conditions and branches:
- T is the condition bit. tst Rm,Rn (and tst #imm,r0): T = 1 when
  (Rm & Rn) == 0. cmp/eq Rm,Rn: T = (Rn == Rm). cmp/gt Rm,Rn: T = (Rn > Rm),
  signed; cmp/ge: Rn >= Rm, signed; cmp/hi and cmp/hs: the same unsigned.
  cmp/pz Rn: T = (Rn >= 0). cmp/pl Rn: T = (Rn > 0).
- bt L jumps when T = 1, bf L when T = 0 (bt/s and bf/s: the same, with a
  delay slot).
- GCC jumps around a block: `if (x == 0) { B }` becomes
  `tst x,x; bf skip; B; skip:`, and `if (x != 0) { B }` becomes
  `tst x,x; bt skip; B; skip:`. So bf after tst is the normal code for an
  `== 0` test, not a sign that the condition is inverted. Whether GCC
  emits bt or bf, and which block falls through, follows the layout of the
  source.
- Operands go source first, destination last: `mov r5,r2` copies r5
  into r2, `add #4,r1` adds 4 to r1.
- Shifts: shll/shlr Rn shift left/right by 1 (logical), shal/shar by 1
  (arithmetic), shll2/8/16 and shlr2/8/16 by 2, 8 or 16. shad Rm,Rn shifts
  Rn by Rm: left when Rm >= 0, right (arithmetic) by -Rm when Rm < 0;
  shld is the same with a logical right shift. GCC writes `x >> n` with a
  variable n as a negated count and shad/shld; that is where the
  decompiler's `x >> (~n & 0x1f) + 1` comes from: it is a plain variable
  shift.
- exts.b/exts.w sign-extend, extu.b/extu.w zero-extend; mov.b and mov.w
  loads from memory sign-extend.
- Arguments arrive in r4-r7 (ints and pointers) and fr4-fr11 (floats); r0
  (fr0) returns the value. r8-r14 are saved by the callee and hold values
  that live across calls; the prologue pushes the ones the function uses.

What has fixed near misses before (from the project's brief):
"""


def brief_patterns():
    """The brief's pattern section, between its heading and ## Output."""
    try:
        t = open(BRIEF).read()
    except OSError:
        return ""
    m = re.search(r"^## What fixed near misses.*?$(.*?)^## Output", t, re.M | re.S)
    return m.group(1).strip() if m else ""


def src_files(rev):
    """(path under src/, text) for every .c of src/: the working tree, or a
    git revision. The control experiment reads the decompiler's text of
    functions matched since from the commit before they were adopted; the
    working tree's src/ carries the matched text for those."""
    import subprocess
    if not rev:
        for d, _, fs in os.walk(program.SRC):
            for f in sorted(fs):
                if f.endswith(".c"):
                    p = os.path.join(d, f)
                    yield os.path.relpath(p, program.SRC), open(p, errors="replace").read()
        return
    rel = os.path.relpath(program.SRC, ROOT)
    names = subprocess.run(["git", "ls-tree", "-r", "--name-only", rev, "--", rel], cwd=ROOT,
                           capture_output=True, text=True, check=True).stdout.split()
    for n in sorted(names):
        if n.endswith(".c"):
            t = subprocess.run(["git", "show", f"{rev}:{n}"], cwd=ROOT, capture_output=True, text=True, check=True).stdout
            yield os.path.relpath(n, rel), t


def functions(include_matched, rev=None):
    """address -> (size, path, name, text) from src/, matched ones too when
    asked (am.candidates skips them)."""
    out = {}
    for path, content in src_files(rev):
        if True:
            for m in am.HEAD.finditer(content):
                if not include_matched and re.search(r"(?m)^ \* matched: ", m.group(3)):
                    continue
                text = re.sub(r"^(?:/\*.*?\*/\s*)+", "", m.group(4).strip(), flags=re.S)
                name = re.search(r"\b([A-Za-z_]\w*)\s*\(", text)
                ext = re.search(r"(?m)^ \* extent: 0x([0-9a-f]+)$", m.group(3))
                size = int(ext.group(1), 16) if ext else int(m.group(2), 16)
                out[int(m.group(1), 16)] = (size, path, name.group(1) if name else "?", text)
    return out


def flags_for(a):
    """The build flags of a's build group, as auto_match picks them ("-" =
    none). Not from a results*.tsv row: auto_match writes "-" for every
    function it skips, whatever its group needs."""
    import delay_slots
    global _KIND
    if _KIND is None:
        _KIND = am.kind_of(delay_slots.groups())
    return "-fno-delayed-branch" if _KIND(a) == "empty" else "-"


_KIND = None


BRANCHES = ("bt", "bf", "bt/s", "bf/s", "bra", "bsr")


def listing(xs, a, word, reloc, name_of):
    """[(key, text, label)] for one side's instructions: key is what the
    two sides are aligned on, text what the model reads. Branch targets
    become labels L1, L2, ... in address order (label marks the target's
    line); a literal-pool load shows what it loads, =symbol or =value, and
    is aligned on the value. word(addr, n) reads n bytes at addr;
    reloc(addr) is (symbol, address or None) for a relocated pool word (our
    side), else None; name_of(value) names an address, or None."""
    targets = sorted({int(x[2], 16) for x in xs if x[1] in BRANCHES and re.match(r"0x[0-9a-f]+$", x[2])})
    lab = {t: f"L{i + 1}" for i, t in enumerate(targets)}
    out = []
    for at, mn, ops in xs:
        key = text = f"{mn} {ops}".strip()
        m = re.match(r"0x([0-9a-f]+)(,.*)?$", ops)
        if m and mn in BRANCHES:
            t = int(m.group(1), 16)
            text = f"{mn} {lab.get(t, f'{t - a:+#x}')}"
            key = f"{mn} L"
        elif m and mn in ("mov.l", "mov.w") and m.group(2):
            pw = int(m.group(1), 16)
            n = 4 if mn == "mov.l" else 2
            r = reloc(pw)
            if r:
                what, v = r
            else:
                v = int.from_bytes(word(pw, n), "little", signed=(n == 2))
                what = name_of(v) if n == 4 else None
                if what is None:
                    what = str(v) if -4096 < v < 65536 else f"{v & 0xffffffff:#x}"
            text = f"{mn} ={what}{m.group(2)}"
            key = f"{mn} ={v & 0xffffffff:#x}{m.group(2)}" if v is not None else text
        elif m:
            key = cl.norm((at, mn, ops), 0)
        out.append((key, text, lab.get(at)))
    return out


def side_by_side(g, m, context=3):
    """The diff in words: GAME / OURS / BOTH lines, hunks split by @@."""
    sm = difflib.SequenceMatcher(None, [x[0] for x in g], [x[0] for x in m], autojunk=False)
    show = lambda x: (f"{x[2]}:" if x[2] else "").ljust(5) + x[1]
    lines = []
    for grp in sm.get_grouped_opcodes(context):
        lines.append("@@")
        for op, i1, i2, j1, j2 in grp:
            if op == "equal":
                for x, y in zip(g[i1:i2], m[j1:j2]):
                    t = "BOTH " + show(x)
                    if (x[1], x[2]) != (y[1], y[2]):
                        t += "   | ours: " + show(y).strip()
                    lines.append(t)
                continue
            lines += ["GAME " + show(x) for x in g[i1:i2]]
            lines += ["OURS " + show(y) for y in m[j1:j2]]
    return lines


class Judge:
    """Compile one text and diff it against the game (diff.py's core)."""

    def __init__(self):
        # the line table comes from -g, which match.FLAGS carries for every
        # compile since 2026-11-10 (before that it was added here)
        self.img = open(program.IMAGE, "rb").read()
        self.symtab = match.load_symbols()
        self.slots = match.game_slots()
        self.by_addr = {}
        for k, v in self.symtab.items():
            self.by_addr.setdefault(v, k)
        # the aggregate globals the data header declares, named by address
        hdr = os.path.join(program.SRC, "include", "vf4ft_data.h")
        if os.path.exists(hdr):
            for full in set(re.findall(r"\bagg_[0-9a-f]{8}_\w+", open(hdr).read())):
                self.by_addr.setdefault(int(full[4:12], 16), full)

    def run(self, a, size, name, text, flags):
        """-> (verdict, diff lines, error, score) ; verdict 'match' / 'DIFF' / ...;
        error is the compiler's complaint when the text did not build; score
        is {same: game instructions in place, game: their count, first:
        instructions before the first difference, g / m: the two listings,
        ops: the aligner's non-equal opcodes, mline: the C line (1-based, in
        text) of each of our instructions or None}, None without a build."""
        v = f"V0_{a:08x}"
        t = re.sub(r"\b" + re.escape(name) + r"\b(?=\s*\()", v, text, count=1)
        if v not in t:
            return "no-def", [], f"the text has no definition of {name}", None
        with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
            f = os.path.join(tmp, "one.c")
            am.write_batch(f, [(None, None, None, None, t)], [] if flags == "-" else [flags])
            try:
                o = match.compile_c(f, tmp)
                secs, syms, relocs = match.read_elf(o)
            except (Exception, SystemExit) as e:  # match.compile_c exits with the compiler's output
                msg = str(e)
                msg = "\n".join(l for l in msg.splitlines() if "warning:" not in l and "one.c" in l)[:1500]
                return "no-compile", [], msg or str(e)[:800], None
            detail = []
            try:
                _, _, res = match.run_checks([f], self.img, self.symtab, self.slots, verbose=True,
                                             say=lambda *x, **k: detail.append(" ".join(str(y) for y in x)))
            except SystemExit as e:
                return "no-compile", [], str(e)[:800], None
            dl = subprocess.run(["sh4-elf-objdump", "--dwarf=decodedline", o], capture_output=True, text=True).stdout
            built = open(f).read().split("\n")
        s = next((x for x in syms if x["name"].lstrip("_") == v), None)
        if s is None:
            return "no-def", [], f"the compile has no symbol {name}", None
        data = secs[s["shn"]]["data"]
        code = data[s["value"]:s["value"] + s["size"]]
        here = {}  # section offset of a relocated pool word -> (symbol(+addend), address or None)
        named = {}  # address -> the name our text uses for it, to name the game's values alike
        for off, _, sym, add in relocs.get(s["shn"], []):
            nm = sym["name"].lstrip("_") or secs[sym["shn"]]["name"]
            at = match.address_of(nm, self.symtab) if sym["name"] else None
            here[off] = (nm + (f"+{add:#x}" if add else ""), at + add if at is not None else None)
            if at is not None:
                named.setdefault(at + add, here[off][0])

        def name_of(v):
            if v in named:
                return named[v]
            if v in self.by_addr:
                return self.by_addr[v]
            return f"DAT_{v:08x}" if 0x0c000000 <= v < 0x0d000000 else None
        g = listing(cl.insns(self.img[a - match.BASE:a - match.BASE + size], a), a,
                    lambda p, n: self.img[p - match.BASE:p - match.BASE + n], lambda p: None, name_of)
        m = listing(cl.insns(code, a), a,
                    lambda p, n: data[s["value"] + p - a:s["value"] + p - a + n],
                    lambda p: here.get(s["value"] + p - a), name_of)
        verdict = ", ".join(r[3] for r in res) or "?"
        lines = side_by_side(g, m) if [x[1] for x in g] != [x[1] for x in m] else []
        if not lines and verdict != "match":
            lines = ["(the instruction listings agree; what differs:)"] + detail[:20]
        sm = difflib.SequenceMatcher(None, [x[0] for x in g], [x[0] for x in m], autojunk=False)
        blocks = sm.get_matching_blocks()
        score = {"same": sum(b.size for b in blocks), "game": len(g),
                 "first": blocks[0].size if blocks[0].a == 0 and blocks[0].b == 0 else 0,
                 "g": g, "m": m, "ops": [op for op in sm.get_opcodes() if op[0] != "equal"],
                 "mline": self.line_map(dl, built, t, v, cl.insns(code, a), s["value"] - a)}
        return verdict, lines, None, score

    @staticmethod
    def line_map(dl, built, t, v, xs, base):
        """The C line in t (1-based) of each of our instructions, from the
        line table; the build file adds lines above the text and takes
        define lines out, so the two are aligned on the definition line."""
        rows = sorted((int(at, 0), int(ln)) for ln, at in
                      re.findall(r"^one\.c\s+(\d+)\s+(0x[0-9a-f]+|\d+)\b", dl, re.M))
        defn = re.compile(re.escape(v) + r"\s*\(")
        dc = next((i for i, l in enumerate(built) if defn.search(l)), None)
        dt = next((i for i, l in enumerate(t.split("\n")) if defn.search(l)), None)
        if not rows or dc is None or dt is None:
            return [None] * len(xs)
        n, starts, out = len(t.split("\n")), [r[0] for r in rows], []
        for at, _, _ in xs:
            i = bisect.bisect_right(starts, base + at) - 1
            ln = rows[i][1] - (dc - dt) if i >= 0 else None
            out.append(ln if ln and 1 <= ln <= n else None)
        return out


class Guard:
    """Says when a streaming reply should be cut: the same long line `lines`
    times among the last `window` lines of prose, the same run of `ngram`
    words `ngrams` times, or past `budget` tokens. Inside ``` fences (a
    fence still open included) only // comment lines are checked: code
    repeats lines legitimately, but a model that reasons in comments loops
    there too."""

    def __init__(self, budget, lines=4, ngram=12, ngrams=4, window=80):
        self.budget, self.lines, self.ngram, self.ngrams, self.window = budget, lines, ngram, ngrams, window

    @staticmethod
    def norm(s):
        s = re.sub(r"^[\s*>#\-\d.)/]+", "", s)
        return re.sub(r"\s+", " ", s).strip().lower()

    def check(self, text, tokens):
        """-> None, or (why, offset in text where the repetition began)."""
        # fenced text blanked out but for its // comments, offsets kept
        keep = lambda l: l.group(0) if l.group(0).lstrip().startswith("//") else " " * len(l.group(0))
        prose = re.sub(r"```.*?(```|$)", lambda m: re.sub(r"[^\n]+", keep, m.group(0)), text, flags=re.S)
        rows = [(m.start(), self.norm(m.group(0))) for m in re.finditer(r"[^\n]+", prose)]
        where = {}
        for at, s in [r for r in rows if len(r[1]) >= 25][-self.window:]:
            where.setdefault(s, []).append(at)
            if len(where[s]) >= self.lines:
                return f"the line {s[:70]!r} {self.lines} times", where[s][1]
        words = [(m.start(), m.group(0).strip("`*.,:;()[]").lower()) for m in re.finditer(r"\S+", prose)][-2000:]
        seen = {}
        for i in range(len(words) - self.ngram + 1):
            key = tuple(w for _, w in words[i:i + self.ngram])
            seen.setdefault(key, []).append(words[i][0])
            if len(seen[key]) >= self.ngrams:
                return f"the words {' '.join(key)[:70]!r} {self.ngrams} times", seen[key][1]
        if self.budget and tokens > self.budget:
            return "budget", len(text)
        return None


def stream(server, body, guard=None):
    """One streamed completion -> {text, reasoning, tokens, finish, cut,
    wait}: cut is (why, offset) when the guard stopped it, the offset into
    reasoning + "\\n" + text (just text without reasoning) where the
    repetition began; wait is the seconds to the first token. Leaving the
    request cancels the generation on the server."""
    body = dict(body, stream=True, stream_options={"include_usage": True})
    req = urllib.request.Request(server.rstrip("/") + "/chat/completions", data=json.dumps(body).encode(),
                                 headers={"Content-Type": "application/json"})
    text, reasoning, n, why, usage, cut, checked, wait = [], [], 0, None, None, None, 0, None
    t0 = time.time()
    with urllib.request.urlopen(req, timeout=1800) as r:
        for raw in r:
            line = raw.decode("utf-8", "replace").strip()
            if not line.startswith("data:"):
                continue
            data = line[5:].strip()
            if data == "[DONE]":
                break
            ev = json.loads(data)
            usage = ev.get("usage") or usage
            for ch in ev.get("choices") or []:
                d = ch.get("delta") or {}
                for k, out in (("reasoning_content", reasoning), ("content", text)):
                    if d.get(k):
                        out.append(d[k])
                        n += 1
                        wait = wait if wait is not None else time.time() - t0
                why = ch.get("finish_reason") or why
            if guard and n - checked >= 32:
                checked = n
                rs = "".join(reasoning)
                cut = guard.check(rs + "\n" + "".join(text) if rs else "".join(text), n)
                if cut:
                    why = "guard"
                    break
    return {"text": "".join(text), "reasoning": "".join(reasoning), "finish": why, "cut": cut, "wait": wait,
            "tokens": n if cut else (usage or {}).get("completion_tokens") or n}


BRIDGE = {"loop": "\n\nI am repeating myself, so I stop the analysis here and commit to the most promising "
                  "change so far. The whole function:\n\n```c\n",
          "budget": "\n\nThat is enough analysis; I commit to the most promising change so far. "
                    "The whole function:\n\n```c\n"}


HUNK_BRIDGE = {"loop": "\n\nI am repeating myself, so I stop the analysis here and commit to my edits.\n\n"
                       "```edit\n<<<<<<< SEARCH\n",
               "budget": "\n\nThat is enough analysis; I commit to my edits.\n\n```edit\n<<<<<<< SEARCH\n"}


def ask(server, model, messages, max_tokens, think, opt, name, temp=None, mode="function"):
    """-> (reply, completion tokens, finish reason, reasoning, guard note or
    None). A reply the guard cuts is continued from a bridge that opens the
    ```c block (not with thinking on), unless a budget cut finds the
    function already written. temp overrides the temperature; mode "hunk"
    bridges into ```edit blocks instead."""
    if temp is None:
        temp = opt.temperature if opt.temperature is not None else (0.6 if think else 0.7)
    body = {"model": model, "messages": messages, "max_tokens": max_tokens, "temperature": temp,
            "top_p": opt.top_p, "presence_penalty": opt.presence_penalty,
            "chat_template_kwargs": {"enable_thinking": bool(think)}}
    if opt.top_k is not None:
        body["top_k"] = opt.top_k
    if opt.min_p is not None:
        body["min_p"] = opt.min_p
    guard = None if opt.no_guard or opt.prefill else Guard(opt.reason_budget, opt.guard_lines, opt.guard_ngram)
    r = stream(server, body, guard)
    text, reasoning, used, why, note = r["text"], r["reasoning"], r["tokens"], r["finish"], None
    if r["cut"]:
        what, at = r["cut"]
        if reasoning:
            reasoning, text = (reasoning[:at], "") if at <= len(reasoning) else (reasoning, text[:at - len(reasoning) - 1])
        else:
            text = text[:at]
        if text.count("```") % 2:  # cut inside a code block: drop the open block
            text = text[:text.rindex("```")]
        text = text.rstrip()
        note = f"guard cut at {used} tokens: {what}"
        if mode == "hunk":
            done = bool(parse_edits(text))
        else:
            done = code_block(text)
            done = bool(done and re.search(r"\b" + re.escape(name) + r"\s*\(", done))
        if what == "budget" and done:
            note += "; the answer was already written, taken as it is"
            why = "guard"
        elif not think:
            bridge = (HUNK_BRIDGE if mode == "hunk" else BRIDGE)["budget" if what == "budget" else "loop"]
            prefix = text + bridge
            more_body = dict(body, messages=messages + [{"role": "assistant", "content": prefix}],
                             max_tokens=opt.final_tokens)
            if mode != "hunk":  # one ```c block; edits come several to a reply
                more_body["stop"] = ["\n```"]
            more = stream(server, more_body)
            cont = more["text"]
            if cont.startswith(prefix[:200]):  # the server may echo the prefill
                cont = cont[len(prefix):]
            text = prefix + cont.rstrip()
            if mode != "hunk" or text.count("```") % 2:
                text += "\n```"
            used += more["tokens"]
            why = "guard+" + str(more["finish"])
            note += (f"; continued from the bridge: {more['tokens']} tokens, finish {more['finish']}, "
                     f"first token after {more['wait'] or 0:.1f} s")
    # the reasoning: a reasoning_content field when the server parses it,
    # else inline, closed by </think> (the template supplies the opener, so
    # the output often carries only the closing tag)
    if "</think>" in text:
        inline, text = text.rsplit("</think>", 1)
        reasoning = reasoning or inline.replace("<think>", "").strip()
    return text.strip(), used, why, reasoning, note


PREFILL = "```c\n"


def code_block(reply, first=False):
    """The last ```c block of a reply (or the last fenced block); the first
    one when the reply was prefilled, so code quoted in any commentary after
    it is not taken."""
    blocks = re.findall(r"```(?:c|C)?\s*\n(.*?)```", reply, re.S)
    return (blocks[0] if first else blocks[-1]).strip() + "\n" if blocks else None


def same_text(c):
    """A text with comments and spacing taken out, to spot a repeat."""
    c = re.sub(r"/\*.*?\*/|//[^\n]*", "", c, flags=re.S)
    return re.sub(r"\s+", " ", c).strip()


STATUS = {
    "IMPROVED": "compiled, fewer diff lines than any earlier text",
    "NO_GAIN": "compiled, as many diff lines as your best text",
    "WORSE": "compiled, more diff lines than your best text",
    "NO_COMPILE": "the compiler rejected the text",
    "REPEAT": "the same text as an earlier one; not compiled",
    "NO_CODE": "the reply had no ```c block",
    "CUT_OFF": "the reply hit the token limit before its ```c block closed",
    "RULE": "the text breaks a rule; not compiled",
}


def describe(t):
    """One text's place in the STATE block."""
    s = t["score"]
    return (f"{t['lines']} diff lines; {s['same']} of {s['game']} game instructions in place; "
            f"first difference at instruction {s['first'] + 1}" if s else f"{t['lines']} diff lines")


def state_block(status, k, opt, stale, this, best, last, next_step):
    """The STATE header every feedback message opens with: decided by the
    harness, so the model never has to judge its own progress."""
    left = opt.attempts - k
    if opt.patience:
        left = min(left, opt.patience - stale)
    who = lambda t: f"attempt {t['attempt']}" if t["attempt"] else "the decompiler's text"
    out = ["STATE (from the harness)",
           f"status: {status}: {STATUS[status]}",
           f"attempt: {k} of {opt.attempts}; the run stops after {left} more without a new best"
           if opt.patience and opt.patience - stale < opt.attempts - k else f"attempt: {k} of {opt.attempts}",
           "this text: " + (describe(this) if this else "not compiled"),
           "best text: this one" if this is best else f"best text: {describe(best)} ({who(best)})"]
    if this:
        out.append(f"change: {this['lines'] - best['lines']:+d} diff lines against the best, "
                   f"{this['lines'] - last['lines']:+d} against {who(last)}" if this is not best else
                   f"change: {this['lines'] - last['lines']:+d} diff lines against {who(last)}")
    out.append("required next: " + next_step)
    return "\n".join(out)


def work(a, info, opt, judge, log_dir, lock, results):
    size, path, name, text = info
    flags = flags_for(a)
    log = open(os.path.join(log_dir, f"{a:08x}.log"), "w")
    say = lambda *x: (print(*x, file=log), log.flush())
    chat = open(os.path.join(log_dir, f"{a:08x}.chat.md"), "w")
    note = lambda x: (print(x, file=chat), chat.flush())
    states = open(os.path.join(log_dir, f"{a:08x}.state.jsonl"), "w")
    verdict, diff, err, score = judge.run(a, size, name, text, flags)
    say(f"== {a:08x} {name} ({size} bytes, {path}, flags {flags}) baseline: {verdict}, {len(diff)} diff lines")
    if verdict == "match":
        say("already matches as decompiled")
        return a, "match", 0, 0
    if err:
        # the decompiler's text does not build against today's headers (a
        # control text from an old commit after symbols moved): not a fair
        # test of the model
        say(f"excluded: the starting text does not compile: {err[:300]}")
        return a, "excluded (baseline no-compile)", 0, 0
    head = (f"Function {name} at 0x{a:08x} ({size} bytes in the game, file {path}, "
            f"build flags: {flags if flags != '-' else 'none'}).\n\nThe decompiler's C:\n```c\n{text}\n```\n\n"
            f"The diff, game against ours (first {opt.diff_lines} lines):\n```diff\n"
            + "\n".join(diff[:opt.diff_lines]) + "\n```\n\nWrite the C that compiles to the game's instructions.")
    system = RULES + brief_patterns()
    history = [{"role": "user", "content": head}]
    note(f"# {a:08x} {name}\n\n## prompt\n\n{head}\n")
    best = last = {"text": text, "lines": len(diff), "diff": diff, "score": score, "attempt": 0}
    tried = {same_text(text): (0, f"{len(diff)} diff lines")}
    tokens, stale, stop, k = 0, 0, "attempts", 0
    whole = "answer with the whole function in one ```c block"
    base_temp = opt.temperature if opt.temperature is not None else (0.7 if opt.no_think else 0.6)
    for k in range(1, opt.attempts + 1):
        msgs = [{"role": "system", "content": system}] + history[:1] + history[1:][-6:]
        if len(history) > 1:
            note(f"## feedback before attempt {k}\n\n{history[-1]['content']}\n")
        if opt.prefill:
            msgs.append({"role": "assistant", "content": PREFILL})
        for r in range(opt.resamples + 1):
            # a repeat is sampled again at once, hotter, without spending an
            # attempt: telling the model about it does not change what it sends
            temp = None if r == 0 else base_temp + opt.resample_step * r
            try:
                reply, used, why, reasoning, cut = ask(opt.server, opt.model, msgs, opt.max_tokens,
                                                       not opt.no_think, opt, name, temp)
            except Exception as e:
                say(f"-- attempt {k}: server error {e}")
                time.sleep(10)
                reply = None
                break
            tokens += used
            if opt.prefill and not reply.startswith("```"):
                # the server may return the continuation alone
                reply = PREFILL + reply
            label = f"attempt {k}" + (f" (resample {r}, temperature {temp:.2f})" if r else "")
            note(f"## {label}: {used} tokens, finish {why}\n" + (f"\n{cut}\n" if cut else ""))
            if reasoning:
                note(f"### reasoning\n\n{reasoning}\n")
            note(f"### reply\n\n{reply}\n")
            cand = code_block(reply, first=opt.prefill)
            say(f"-- {label}: {used} tokens, finish {why}")
            if cut:
                say(f"   {cut}")
            if cand is None or why in ("length", "guard+length") or same_text(cand) not in tried or r == opt.resamples:
                break
            say(f"   repeat of attempt {tried[same_text(cand)][0]}, sampled again")
        if reply is None:
            continue
        cut_off = why in ("length", "guard+length")
        this, keep = None, 4000
        if cut_off or cand is None:
            say("   reply tail: " + reply[-600:].replace("\n", "\n   | "))
            say("   cut off at the token limit" if cut_off else "   no code block in the reply")
            status, keep = ("CUT_OFF" if cut_off else "NO_CODE"), 2000
            body, step = "", "reason briefly, then " + whole + "."
        elif re.search(r"0x0c[0-9a-fA-F]{6}", cand):
            status, body = "RULE", "The text hard-codes a main-RAM address (0x0c......)."
            step = "use the symbol declared for it instead, and " + whole + "."
            say("   " + body)
        elif re.search(r"\(\*\)\s*\([^()]*\)\s*\)\s*(FUN_|mtx_|script_)", cand):
            status, body = "RULE", "The text casts a callee to a function-pointer type."
            step = ("do not: use #define KRPROTO_<callee> (arguments in the game's load order) or "
                    "#define NOPROTO_<callee> at the top instead, and " + whole + ".")
            say("   " + body)
        elif same_text(cand) in tried:
            k0, what = tried[same_text(cand)]
            status = "REPEAT"
            body = ((f"That text is the same as your attempt {k0} ({what}), " if k0 else
                     f"That text is the same as the decompiler's ({what}), ") + "apart from comments and spacing.")
            step = "do not send it again: change the source shape that explains the first hunk of the diff, and " + whole + "."
            say(f"   repeat of attempt {k0}, not compiled")
        else:
            verdict, diff, err, score = judge.run(a, size, name, cand, flags)
            tried[same_text(cand)] = (k, "did not compile" if err else f"{len(diff)} diff lines")
            if verdict == "match":
                say(f"   MATCH after {k} attempts")
                states.write(json.dumps({"attempt": k, "tokens": used, "finish": why, "guard": cut, "status": "MATCH"}) + "\n")
                with lock:
                    results[f"{a:08x}"] = cand
                    json.dump(results, open(opt.out, "w"), indent=1)
                return a, "match", k, tokens
            if err:
                say(f"   {verdict}: {err}")
                status, body = "NO_COMPILE", f"The compiler's complaint:\n```\n{err}\n```"
                step = "fix it and " + whole + "."
            else:
                this = {"text": cand, "lines": len(diff), "diff": diff, "score": score, "attempt": k}
                say(f"   {verdict}, {len(diff)} diff lines")
                say("\n".join(diff[:opt.diff_lines]))
                status = ("IMPROVED" if this["lines"] < best["lines"] else
                          "NO_GAIN" if this["lines"] == best["lines"] else "WORSE")
                keep = 6000
        if status == "IMPROVED":
            stale, prev, best = 0, last, this
            body = "The diff, game against ours:\n```diff\n" + "\n".join(this["diff"][:opt.diff_lines]) + "\n```"
            step = "change the source shape that explains the first hunk, and " + whole + "."
        else:
            stale, prev = stale + 1, last
            if this:
                # worse than, or no better than, the best text: send that one
                # back with its diff, so the model works from it and not from
                # the regression
                body = (f"Your best text:\n```c\n{best['text']}```\nIts diff, game against ours:\n```diff\n"
                        + "\n".join(best["diff"][:opt.diff_lines]) + "\n```")
                step = "start again from your best text, change one thing that explains its first hunk, and " + whole + "."
        if this:
            last = this
        st = {"attempt": k, "resamples": r, "tokens": used, "finish": why, "guard": cut, "status": status,
              "lines": this and this["lines"], "same": this and this["score"]["same"],
              "first": this and this["score"]["first"], "best": best["lines"],
              "best_attempt": best["attempt"], "stale": stale}
        states.write(json.dumps(st) + "\n")
        states.flush()
        say(f"   state: {status}; best {best['lines']} lines ({best['attempt'] or 'baseline'}); "
            f"{stale} in a row without a new best; {tokens} tokens so far")
        # stopping is the harness's call, never the model's
        if opt.patience and stale >= opt.patience:
            stop = f"stalled: {stale} without a new best"
            break
        if opt.fn_tokens and tokens >= opt.fn_tokens:
            stop = f"token budget: {tokens}"
            break
        if k == opt.attempts:
            break
        fb = state_block(status, k, opt, stale, this, best, prev, step) + ("\n\n" + body if body else "")
        history += [{"role": "assistant", "content": reply[-keep:]}, {"role": "user", "content": fb}]
    say(f"   no match in {k} attempts ({stop}); best {best['lines']} lines")
    open(os.path.join(log_dir, f"{a:08x}.best.c"), "w").write(best["text"])
    return a, f"miss ({best['lines']} lines; {stop})", k, tokens


HUNK_ANSWER = """- Answer with edits, not the whole function. Put each edit in its own
  ```edit block holding one or more SEARCH/REPLACE pairs:
  ```edit
  <<<<<<< SEARCH
  lines copied exactly from the current C (without the line numbers)
  =======
  the lines that take their place
  >>>>>>> REPLACE
  ```
  The SEARCH lines must occur in the current C exactly once; copy enough
  of them to make that so. One edit may hold several pairs, e.g. one that
  adds a local to the declarations and one that changes a statement. A
  `#define NOPROTO_<callee>` / `#define KRPROTO_<callee>` line goes in a
  pair whose SEARCH is the function's first line, with the define above
  it in the REPLACE. Keep commentary to a few lines.
"""
HUNK_RULES = RULES[:RULES.index("- Answer with ONE")] + HUNK_ANSWER + RULES[RULES.index("- Keep the function's name"):]
NUM = re.compile(r"^\s*\d+\|\s?")


def parse_edits(reply):
    """[[(search, replace), ...] for each fenced block that holds a pair]."""
    out = []
    for block in re.findall(r"```[A-Za-z]*[ \t]*\n(.*?)```", reply, re.S):
        pairs = re.findall(r"<<<<<<<\s*SEARCH[ \t]*\n(.*?)\n?=======[ \t]*\n(.*?)\n?>>>>>>>\s*REPLACE", block, re.S)
        if pairs:
            out.append(pairs)
    return out


def apply_edit(text, pairs):
    """-> (text with the pairs applied, None) or (None, why not). SEARCH lines
    match whatever their indentation; line numbers copied from the prompt
    are dropped."""
    lines = text.split("\n")
    for search, replace in pairs:
        want = [NUM.sub("", l).strip() for l in search.split("\n")]
        while want and not want[0]:
            want.pop(0)
        while want and not want[-1]:
            want.pop()
        if not want:
            return None, "a SEARCH part is empty"
        have = [l.strip() for l in lines]
        at = [i for i in range(len(lines) - len(want) + 1) if have[i:i + len(want)] == want]
        if len(at) != 1:
            return None, ("its SEARCH lines are not in the current C" if not at else
                          f"its SEARCH lines occur {len(at)} times; copy more lines to make them unique")
        lines[at[0]:at[0] + len(want)] = [NUM.sub("", l) for l in replace.split("\n")]
    return "\n".join(lines), None


def hunk_view(score, h, text, context=3):
    """-> (the diff lines of non-equal opcode h with some context, (lo, hi)
    the C lines our instructions in it come from, or None)."""
    g, m, ml = score["g"], score["m"], score["mline"]
    _, i1, i2, j1, j2 = score["ops"][h]
    show = lambda x: (f"{x[2]}:" if x[2] else "").ljust(5) + x[1]
    out = (["BOTH " + show(x) for x in g[max(0, i1 - context):i1]] + ["GAME " + show(x) for x in g[i1:i2]]
           + ["OURS " + show(y) for y in m[j1:j2]] + ["BOTH " + show(x) for x in g[i2:i2 + context]])
    ks = [k for k in (ml[j1:j2] if j2 > j1 else ml[max(0, j1 - 1):j1 + 1]) if k]
    if not ks:
        return out, None
    lo, hi = max(1, min(ks) - 1), min(len(text.split("\n")), max(ks) + 1)
    return out, (lo, min(hi, lo + 30))


def frame_hunks(score, text):
    """The hunks whose instructions come only from the function's opening
    and closing braces (prologue, epilogue): register saves and frame size
    follow from the body, so they are noted, never the focus."""
    lines = text.split("\n")
    open_ = next((i for i, l in enumerate(lines, 1) if "{" in l), 1)
    close = max((i for i, l in enumerate(lines, 1) if "}" in l), default=len(lines))
    out = set()
    for h, (_, i1, i2, j1, j2) in enumerate(score["ops"]):
        ml = score["mline"]
        ks = [k for k in (ml[j1:j2] if j2 > j1 else ml[max(0, j1 - 1):j1 + 1]) if k]
        if ks and all(k <= open_ or k >= close for k in ks):
            out.add(h)
    return out


def compile_error(err):
    """The compiler's first real complaint, without the file name and
    GCC's footnotes."""
    for l in err.split("\n"):
        m = re.search(r"one\.c:\d+:\s*(\S.*)", l)
        if m and not re.search(r"Each undeclared|for each function", l):
            return m.group(1).strip()
    return err.strip().split("\n")[-1]


# the decompiler's local names carry their type in the prefix
PREFIX_TYPE = [("pp", "undefined4 **"), ("pu", "undefined4 *"), ("pi", "int *"), ("pf", "float *"),
               ("ps", "short *"), ("pc", "char *"), ("pb", "byte *"), ("p", "undefined *"),
               ("i", "int"), ("u", "uint"), ("s", "short"), ("c", "char"), ("b", "byte"), ("f", "float"),
               ("d", "double")]
DECL = re.compile(r"^\s*(?:const\s+|volatile\s+|unsigned\s+|signed\s+)*[A-Za-z_]\w*[\s*]+\**\w+(\[\w*\])?;\s*$")


def declare(text, name):
    """-> (text with a declaration of the local `name` added after the
    function's declarations, its type), or (None, None) for a name that is
    not a local's (a global, a register): the type is the one the text
    casts to when it assigns the name, else the one its prefix stands for."""
    m = re.match(r"([a-z]*)Var\d+$|local_\w+$", name)
    if not m:
        return None, None
    cast = re.search(r"\b" + re.escape(name) + r"\s*=\s*\(\s*([A-Za-z_][\w\s]*\**)\s*\)", text)
    if cast:
        ty = cast.group(1).strip()
    else:
        pre = m.group(1) or ""
        ty = next((t for p, t in PREFIX_TYPE if pre == p), "int")
    lines = text.split("\n")
    at = next((i for i, l in enumerate(lines) if "{" in l), None)
    if at is None:
        return None, None
    j = at + 1
    while j < len(lines) and (DECL.match(lines[j]) or not lines[j].strip()):
        j += 1
    while j > at + 1 and not lines[j - 1].strip():
        j -= 1
    lines.insert(j, f"  {ty}{'' if ty.endswith('*') else ' '}{name};")
    return "\n".join(lines), ty


def judge_repaired(judge, a, size, name, text, flags, tries=3):
    """judge.run, plus a harness repair: a local used without a declaration
    gets one (declare), up to `tries` names. -> (verdict, diff, error,
    score, the text compiled, [notes])."""
    notes = []
    for _ in range(tries + 1):
        v, d, e, sc = judge.run(a, size, name, text, flags)
        m = re.search(r"`(\w+)' undeclared", e or "")
        if not m:
            break
        fixed, ty = declare(text, m.group(1))
        if fixed is None:
            g = re.match(r"(?:DAT|PTR|agg)_([0-9a-f]{8})", m.group(1))
            if g and int(g.group(1), 16) in judge.by_addr:
                notes.append(f"{m.group(1)} is not declared; the symbol at 0x{g.group(1)} is "
                             f"{judge.by_addr[int(g.group(1), 16)]}")
            break
        text = fixed
        notes.append(f"the harness declared {ty} {m.group(1)}")
    return v, d, e, sc, text, notes


def better(new, cur):
    """No fewer game instructions in place, the first difference no
    earlier, and one of them, or the diff, better."""
    a, b = new["score"], cur["score"]
    return (a["same"] >= b["same"] and a["first"] >= b["first"]
            and (a["same"] > b["same"] or a["first"] > b["first"] or new["lines"] < cur["lines"]))


def short(s, n=110):
    s = re.sub(r"\s+", " ", "\n".join(NUM.sub("", l) for l in s.split("\n"))).strip()
    return s if len(s) <= n else s[:n] + "..."


def work_hunk(a, info, opt, judge, log_dir, lock, results):
    size, path, name, text = info
    flags = flags_for(a)
    log = open(os.path.join(log_dir, f"{a:08x}.log"), "w")
    say = lambda *x: (print(*x, file=log), log.flush())
    chat = open(os.path.join(log_dir, f"{a:08x}.chat.md"), "w")
    note = lambda x: (print(x, file=chat), chat.flush())
    states = open(os.path.join(log_dir, f"{a:08x}.state.jsonl"), "w")
    verdict, diff, err, score = judge.run(a, size, name, text, flags)
    say(f"== {a:08x} {name} ({size} bytes, {path}, flags {flags}) baseline: {verdict}, {len(diff)} diff lines")
    if verdict == "match":
        say("already matches as decompiled")
        return a, "match", 0, 0
    if err:
        say(f"excluded: the starting text does not compile: {err[:300]}")
        return a, "excluded (baseline no-compile)", 0, 0
    system = HUNK_RULES + brief_patterns()
    note(f"# {a:08x} {name} (hunk mode; the system prompt is in system.md)\n")
    cur = {"text": text, "lines": len(diff), "score": score, "attempt": 0}
    tried = {same_text(text): "the starting text"}
    focus, focus_fails, fails, stale, tokens, stop, k = 0, 0, [], 0, 0, "attempts", 0
    last = "none yet"
    for k in range(1, opt.attempts + 1):
        s = cur["score"]
        if not s["ops"]:
            stop = "the listings agree; what differs is not an instruction"
            break
        frame = frame_hunks(s, cur["text"])
        body = [i for i in range(len(s["ops"])) if i not in frame] or list(range(len(s["ops"])))
        h = body[min(focus, len(body) - 1)]
        view, win = hunk_view(s, h, cur["text"])
        lines_t = cur["text"].split("\n")
        parts = [f"Function {name} at 0x{a:08x} ({size} bytes in the game, file {path}, "
                 f"build flags: {flags if flags != '-' else 'none'}).",
                 "The current C, with line numbers:\n```c\n"
                 + "\n".join(f"{i:3d}| {l}" for i, l in enumerate(lines_t, 1)) + "\n```",
                 "STATE (from the harness)\n"
                 f"last call: {last}\n"
                 f"now: {s['same']} of {s['game']} game instructions in place; first difference at instruction "
                 f"{s['first'] + 1}; {cur['lines']} diff lines\n"
                 f"call: {k} of {opt.attempts}\n"
                 f"focus: hunk {h + 1} of {len(s['ops'])}"
                 + (" (the first place in the body where our code departs from the game's)" if focus == 0 else
                    f" (the {focus} in the body before it resisted {opt.hunk_patience} calls each)"),
                 "The hunk, game against ours:\n```diff\n" + "\n".join(view) + "\n```"]
        if frame:
            fl = [l for f in sorted(frame) for l in hunk_view(s, f, cur["text"], 0)[0]]
            parts.insert(-1, "The prologue/epilogue differs too; it follows from the body (which registers it "
                         "uses, how much stack), so it is not the focus, but it tells you what the game's body "
                         "uses:\n```diff\n" + "\n".join(fl[:16]) + "\n```")
        if win:
            lo, hi = win
            parts.append(f"Our instructions in this hunk come from lines {lo}-{hi} of the C:\n```c\n"
                         + "\n".join(f"{i:3d}| {lines_t[i - 1]}" for i in range(lo, hi + 1)) + "\n```")
        if fails:
            parts.append("Already tried for this hunk and rejected; do not send these again:\n" + "\n".join(fails[-6:]))
        parts.append(f"Propose {opt.edits} different edits, each a different idea, that make our instructions in "
                     "this hunk match the game's without breaking what already matches. Each edit is applied on its "
                     "own to the current C, so it must be complete: when it needs a new local, the declaration goes "
                     "in the same ```edit block as a second SEARCH/REPLACE pair (declarations stay at the top of "
                     "the function). Answer with the edits, each in its own ```edit block.")
        prompt = "\n\n".join(parts)
        note(f"## call {k}\n\n### prompt\n\n{prompt}\n")
        msgs = [{"role": "system", "content": system}, {"role": "user", "content": prompt}]
        try:
            reply, used, why, reasoning, cut = ask(opt.server, opt.model, msgs, opt.max_tokens,
                                                   not opt.no_think, opt, name, mode="hunk")
        except Exception as e:
            say(f"-- call {k}: server error {e}")
            time.sleep(10)
            continue
        tokens += used
        note(f"### reply: {used} tokens, finish {why}\n" + (f"\n{cut}\n" if cut else "")
             + (f"\n#### reasoning\n\n{reasoning}\n" if reasoning else "") + f"\n{reply}\n")
        say(f"-- call {k} (hunk {h + 1} of {len(s['ops'])}; {s['same']}/{s['game']} in place, first difference at "
            f"{s['first'] + 1}, {cur['lines']} lines): {used} tokens, finish {why}")
        if cut:
            say(f"   {cut}")
        edits = parse_edits(reply)[:opt.edits]
        outs, kept, kept_n = [], None, 0
        if not edits:
            outs.append("- no ```edit block with a SEARCH/REPLACE pair in the reply")
            say("   no edits in the reply")
        for n, pairs in enumerate(edits, 1):
            what = "; ".join(f"{short(sr, 60)} -> {short(rp)}" for sr, rp in pairs)
            new, why_not = apply_edit(cur["text"], pairs)
            if new is None:
                res = f"NO_APPLY: {why_not}"
            elif same_text(new) in tried:
                res = f"REPEAT of {tried[same_text(new)]}"
            else:
                tried[same_text(new)] = f"call {k} edit {n}"
                v2, d2, e2, sc2, new, fixes = judge_repaired(judge, a, size, name, new, flags)
                tried[same_text(new)] = f"call {k} edit {n}"
                if v2 == "match":
                    say(f"   edit {n}: MATCH after {k} calls")
                    states.write(json.dumps({"call": k, "hunk": h + 1, "status": "MATCH", "tokens": used}) + "\n")
                    with lock:
                        results[f"{a:08x}"] = new
                        json.dump(results, open(opt.out, "w"), indent=1)
                    return a, "match", k, tokens
                if e2:
                    res = "NO_COMPILE: " + short(compile_error(e2), 160)
                    if fixes and "is not declared" in fixes[-1]:
                        res += " (" + fixes[-1] + ")"
                else:
                    cand = {"text": new, "lines": len(d2), "score": sc2, "attempt": k}
                    up = better(cand, cur)
                    same_score = (sc2["same"], sc2["first"]) == (s["same"], s["first"])
                    res = (("IMPROVED" if up else "NO_GAIN" if same_score else "WORSE")
                           + f": {sc2['same']} in place ({sc2['same'] - s['same']:+d}), first difference at "
                           f"{sc2['first'] + 1} ({sc2['first'] - s['first']:+d}), {len(d2)} diff lines "
                           f"({len(d2) - cur['lines']:+d})")
                    if fixes:
                        res += " (" + "; ".join(fixes) + ")"
                    key = lambda c: (c["score"]["same"], c["score"]["first"], -c["lines"])
                    if up and (kept is None or key(cand) > key(kept)):
                        kept, kept_n = cand, n
            say(f"   edit {n}: {res}\n      {what}")
            outs.append(f"- edit ({what}): {res}")
        if kept:
            last = (f"edit {kept_n} kept: {kept['score']['same']} in place ({kept['score']['same'] - s['same']:+d}), "
                    f"first difference at {kept['score']['first'] + 1} ({kept['score']['first'] - s['first']:+d})")
            cur, stale, focus, focus_fails, fails = kept, 0, 0, 0, []
        else:
            last = "no edit was kept (see the rejected list below)" if edits else "the reply had no edits"
            stale += 1
            focus_fails += 1
            fails += outs
            if focus_fails >= opt.hunk_patience and focus + 1 < len(body):
                focus, focus_fails, fails = focus + 1, 0, []
                last += f"; hunk {h + 1} resisted {opt.hunk_patience} calls, so the focus moves to the next one"
        st = {"call": k, "hunk": h + 1, "hunks": len(s["ops"]), "tokens": used, "finish": why, "guard": cut,
              "edits": len(edits), "results": outs, "kept": kept_n, "same": cur["score"]["same"],
              "first": cur["score"]["first"], "lines": cur["lines"], "stale": stale}
        states.write(json.dumps(st) + "\n")
        states.flush()
        say(f"   state: {'kept edit %d' % kept_n if kept else 'nothing kept'}; now {cur['score']['same']}/"
            f"{cur['score']['game']} in place, first difference at {cur['score']['first'] + 1}, {cur['lines']} lines; "
            f"{stale} in a row with nothing kept; {tokens} tokens so far")
        if opt.patience and stale >= opt.patience:
            stop = f"stalled: {stale} calls with nothing kept"
            break
        if opt.fn_tokens and tokens >= opt.fn_tokens:
            stop = f"token budget: {tokens}"
            break
    say(f"   no match in {k} calls ({stop}); best {cur['lines']} lines")
    open(os.path.join(log_dir, f"{a:08x}.best.c"), "w").write(cur["text"])
    return a, f"miss ({cur['lines']} lines; {stop})", k, tokens


def main():
    import argparse
    p = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    p.add_argument("--batch", type=int)
    p.add_argument("--addrs")
    p.add_argument("--from-json")
    p.add_argument("--include-matched", action="store_true")
    p.add_argument("--src-rev", help="read src/ from this git revision (the control experiment)")
    p.add_argument("--sample", type=int)
    p.add_argument("--seed", type=int, default=0)
    p.add_argument("--server", default="http://192.168.1.64:8080/v1")
    p.add_argument("--model", default="qwen3.6-35b-a3b")
    p.add_argument("--jobs", type=int, default=6)
    p.add_argument("--attempts", type=int, default=12)
    p.add_argument("--max-tokens", type=int, default=16000)
    p.add_argument("--no-think", action="store_true")
    p.add_argument("--diff-lines", type=int, default=400)
    p.add_argument("--presence-penalty", type=float, default=0.0, help="against repetition loops in long replies")
    p.add_argument("--temperature", type=float, help="default 0.6 thinking, 0.7 not")
    p.add_argument("--top-p", type=float, default=0.95)
    p.add_argument("--top-k", type=int, help="default: the server's")
    p.add_argument("--min-p", type=float, help="default: the server's")
    p.add_argument("--prefill", action="store_true", help="start every reply with the ```c block's opening")
    p.add_argument("--no-guard", action="store_true", help="no loop guard on the replies")
    p.add_argument("--reason-budget", type=int, default=8000, help="guard: tokens before the reply is cut (0: none)")
    p.add_argument("--guard-lines", type=int, default=4, help="guard: repeats of one long line that cut the reply")
    p.add_argument("--guard-ngram", type=int, default=12, help="guard: words in a run that, four times over, cut the reply")
    p.add_argument("--final-tokens", type=int, default=4096, help="guard: tokens for the function after a cut")
    p.add_argument("--hunk", action="store_true", help="one hunk at a time, answered with SEARCH/REPLACE edits")
    p.add_argument("--edits", type=int, default=3, help="--hunk: edits asked for per call")
    p.add_argument("--hunk-patience", type=int, default=2, help="--hunk: calls before the focus moves to the next hunk")
    p.add_argument("--resamples", type=int, default=3, help="a repeated text is sampled again up to N times (0: never)")
    p.add_argument("--resample-step", type=float, default=0.3, help="temperature added per resample")
    p.add_argument("--patience", type=int, default=3, help="stop after N attempts in a row without a new best (0: never)")
    p.add_argument("--fn-tokens", type=int, default=0, help="stop a function past N completion tokens (0: never)")
    p.add_argument("--tag", default=time.strftime("%m%d-%H%M"))
    p.add_argument("--dry", action="store_true")
    opt = p.parse_args()
    addrs = []
    if opt.batch:
        addrs = [int(x["address"], 16) for x in json.load(open(os.path.join(ROOT, "build", "agents", f"batch{opt.batch}.json")))]
    if opt.addrs:
        addrs += [int(x, 16) for x in opt.addrs.split(",")]
    if opt.from_json:
        addrs += [int(x, 16) for x in json.load(open(opt.from_json))]
    if opt.sample:
        random.Random(opt.seed).shuffle(addrs)
        addrs = addrs[:opt.sample]
    fns = functions(opt.include_matched or bool(opt.src_rev), opt.src_rev)
    todo = [(a, fns[a]) for a in addrs if a in fns]
    missing = [a for a in addrs if a not in fns]
    if missing:
        print("not in src/ (or matched; use --include-matched):", " ".join(f"{a:08x}" for a in missing))
    if not todo:
        sys.exit("nothing to do")
    judge = Judge()
    if opt.dry:
        a, (size, path, name, text) = todo[0]
        verdict, diff, err, _ = judge.run(a, size, name, text, flags_for(a))
        print(f"{a:08x} {name}: baseline {verdict}, {len(diff)} diff lines, system prompt {len(RULES + brief_patterns())} chars")
        print("\n".join(diff[:40]))
        return
    opt.out = os.path.join(ROOT, "build", "automatch", f"local-{opt.tag}.json")
    log_dir = os.path.join(ROOT, "build", "agents", "local", opt.tag)
    os.makedirs(log_dir, exist_ok=True)
    open(os.path.join(log_dir, "system.md"), "w").write((HUNK_RULES if opt.hunk else RULES) + brief_patterns() + "\n")
    results = json.load(open(opt.out)) if os.path.exists(opt.out) else {}
    done = [a for a, _ in todo if f"{a:08x}" in results]
    if done:
        print(f"{len(done)} already matched in {opt.out}, skipped")
        todo = [(a, i) for a, i in todo if f"{a:08x}" not in results]
    lock = threading.Lock()
    t0 = time.time()
    print(f"{len(todo)} functions, {opt.jobs} at a time, {opt.attempts} attempts each, model {opt.model} -> {opt.out}")
    rows = []
    with concurrent.futures.ThreadPoolExecutor(opt.jobs) as ex:
        futs = {ex.submit(work_hunk if opt.hunk else work, a, info, opt, judge, log_dir, lock, results): a
                for a, info in todo}
        for f in concurrent.futures.as_completed(futs):
            try:
                a, outcome, k, tokens = f.result()
            except Exception as e:
                a, outcome, k, tokens = futs[f], f"error {e}", 0, 0
            rows.append((a, outcome, k, tokens))
            print(f"  {a:08x} {outcome:18s} attempts {k:2d} tokens {tokens:7d}  [{time.time() - t0:6.0f}s]", flush=True)
    got = sum(1 for r in rows if r[1] == "match")
    print(f"{got} of {len(rows)} matched in {(time.time() - t0) / 60:.1f} min; "
          f"{sum(r[3] for r in rows)} completion tokens -> {opt.out}")


if __name__ == "__main__":
    main()
