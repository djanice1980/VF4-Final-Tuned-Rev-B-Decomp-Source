"""Rewrite near-miss decompiler output and keep a rewrite that matches.

    python3 -I tools/match/variants.py [--from results.tsv] [--limit N] [--out FILE.json]

For functions auto_match.py found compiling but differing (DIFF), the
decompiler's C is often right in meaning but not in shape: it stores a
call's result in a temporary before using it, guesses stack array sizes,
names a value it returns once. GCC 3.2 compiles those shapes differently
(FUN_0c09e640 matches once `v = f(i); a[i] = v;` is `a[i] = f(i);`, the
array is int[2] and the later use reads a[i]). Each rewrite below is tried
at each place it applies, up to three together; a variant is kept only if it
compiles to the game's bytes (match.py's run_checks, relocations included),
so the rewrites need not be exact: what matches is the game's code.

  store    v = E; T = v;   ->  T = E;   (later uses of v read T)
  return   v = E; return v;   ->  return E;
  inline   v = E; ... v ...   ->  ... (E) ...   (v used once)
  array    T a[N];   ->  T a[k]  for k < N   (stack arrays)
  sign     a local's or parameter's signedness flipped (uint <-> int, ...)
  cmp      (a < b)   ->  (b > a)
  ifswap   if (C) {A} else {B}   ->  if (!(C)) {B} else {A}
  early    v = A; if (C) { S; v = B; } return v;   ->  early returns
  rettype  another integer type for the function's return
  loop     V = I; if (G) { do { S; V = V + K; } while (C); }  ->
           for (V = I; C; V = V + K) { S }   (Ghidra writes every loop as
           a guarded do-while; GCC 3.2 tests a for loop's counter, not the
           guard: cmp/ge r5,r0 against cmp/pl r5), or while (C) { S }
  castsign *(byte *) <-> *(char *), *(ushort *) <-> *(short *), *(uint *)
           <-> *(int *) at one place, and an unsigned byte or short global
           read as signed ((char)DAT_x): a load GCC extends with extu.b/w,
           or does not, decides it (sh4 loads sign-extend)
  addrswap *(T *)(p + C)  ->  *(T *)(C + p)   (which operand GCC puts in
           r0 for @(r0,Rn))
  swap     two adjacent simple statements at one level exchanged (the
           order the original wrote them: GCC 3.2 at -O1 keeps statement
           order for calls and stores, so a constant set before or after a
           call shows; a swap that matches is the original order by
           definition, so no independence check is needed)

  bitfield x = x & M | (v & W) << S;  ->  a store to a C bitfield (the
           bits M clears) through a struct with unnamed padding:
           ((struct { unsigned : S; unsigned f : w; } *)&x)->f = v; the
           game's structs use bitfields (GCC computes the value, then loads
           and masks the word; the mask-and-or Ghidra shows compiles the
           other way round). Masks Ghidra shows as addresses
           ((uint)&DAT_ffe0ffff) are read as numbers. Also x = (E) | x & M.

  bitread  *(T *)(E) >> S & M  ->  ((struct { unsigned int : S;
           unsigned int f : w; } *)(E))->f, M = 2^w - 1 (a bitfield read; S may
           be 0: *(T *)(E) & M)

  framefit a stack array resized by the difference between the game's
           frame and ours (build/automatch/frames.tsv, from frames.py): the
           decompiler guesses stack arrays too big or too small
           (uint local_c[2] where the game reserves 4 bytes; a 68-byte
           buffer for a 64-byte matrix)

  noproto  the function's file without a declaration of a callee the game
           calls both through r0 and r1 (#define NOPROTO_<name>, which
           vf4ft_functions.h honours): GCC then calls it as returning int,
           through r0, as the original files that lacked the prototype did
           (FUN_0c0b9d20 matches with no prototype of mtx_store); one callee
           at a time, and all of them together

  ptrbase  an int / uint / undefined4 parameter or local used as the base
           of *(T *)(x + C) declared undefined * (the original's was a
           pointer: GCC forms the address differently for int arithmetic;
           FUN_0c055880 matches so)
  baselocal a global's address used as a base (PTR_... + C, &agg[i])
           loaded into a local pointer first (undefined *p = PTR_x;): the
           original's local changes the register allocation (FUN_0c0a08e0,
           FUN_0c044ee0)
  narrowparam a 4-byte parameter used only narrowed ((undefined1)param_2)
           declared that narrow (the game extends it at entry: extu.b)

  boolflag a comparison used as a value (return a != b; f((uint)(x == 2)))
           built as a flag: int r = 0; if (E) r = 1; then return r / f(r)
           (GCC computes the value with negc / movt, the game with a branch)
  retlocal a void function returning one of its integer locals (a copy
           loop returning its count: FUN_0c0fa080; its callers read r0)
  cmp      also with operands that hold casts and parentheses

  krproto  the file declaring a callee with a char / short parameter
           old-style (#define KRPROTO_<name>: void f();), so arguments
           pass unconverted, as sound_request's callers did

  structarray *(T *)(&DAT_X + i * S) as a field of an array of S-byte
           structs starting k bytes before (the decompiler folds the field
           offset into the address; the game adds the index to the array's
           start and reads @(k,rN): FUN_0c0831c0), for each k
  voidcall a call through a function pointer whose value is not used,
           through a pointer to a void function (called through r1, not r0)

  hoist    X = X op (E);  ->  t = E; X = X op t;  (the original computed E
           first: FUN_0c101c20)

  condcall p = A; if (C) { p = B; } (*p)(args);  ->  if (C) B(args); else
           A(args);  (GCC merges two direct calls into one jsr; Ghidra shows
           a function pointer chosen by the condition: FUN_0c046c80)
  guardret if (C) { S } return v;  ->  if (!(C)) return v; S return v;
           (an early return of a value already in r0: FUN_0c07dde0)
  notflag  return a != b;  ->  int t = a == b; return t == 0;  (the game's
           movt / tst / movt; GCC makes negc of the plain comparison:
           FUN_0c09fea0, obj_node_typed)
  movstr   __movstr_i4_even/odd(d, s, n);  ->  a struct assignment of
           (n + 1) * 8 bytes (+ 4 for odd): GCC calls the helper for a
           word-aligned struct copy (0c128840, 0c07fa80)
  hwconst  every hardware register symbol (DAT_a05f6c14, declared volatile
           in vf4ft_data.h) as its address, *(volatile T *)0xa05f6c14, all
           at once: GCC then forms neighbouring registers from one pool
           word with add #4 / add #16, as the game does; through the
           symbols each access loads its own pool word (0c0f2a40,
           0c114440, found by agents)
  shiftmask x << (n & 0x1f)  ->  x << n  (Ghidra's mask: SH's shld takes
           the count as it is)
  indexptr (&SYM)[i]  ->  ((char *)(i))[(int)&SYM], and *(T *)(&SYM + e)
           likewise: GCC makes the pointer-typed operand the base of an
           @(r0,Rn) access, and the game has the symbol in r0 (the player
           records at _1p_level)

--rules a,b,... uses only those rules (by the names above).

Variants are compiled in batches under the names V<k>_<address>, which
match.py maps to the address. Writes FILE (default
build/automatch/variants.json): {address: matching text}; adopt.py --texts
FILE moves them into source/.
"""
import itertools, json, multiprocessing, os, re, sys, tempfile
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program
import auto_match as am
import match

ROOT = am.ROOT
STMT = re.compile(r"^([ \t]*)(\w+) = ([^;\n]+);\n", re.M)


def decls(name, text):
    """How many local declarations of name (int iVar1; / int a[3];)."""
    return len(re.findall(r"^[ \t]*(?!return\b)\w[\w \t\*]*\b" + re.escape(name) + r"[ \t]*(?:\[[^\]]*\])?;", text, re.M))


def uses(name, text):
    return len(re.findall(r"\b" + re.escape(name) + r"\b", text))


def rw_store(text):
    """v = E; then the next statement T = v; -> T = E; and later v -> T."""
    out = []
    for m in STMT.finditer(text):
        ind, v, e = m.groups()
        nxt = re.compile(re.escape(ind) + r"([^;\n=]+?) = " + re.escape(v) + r";\n").match(text, m.end())
        if not nxt or re.search(r"\b" + re.escape(v) + r"\b", nxt.group(1)):
            continue
        t = nxt.group(1).strip()
        rest = re.sub(r"\b" + re.escape(v) + r"\b", lambda _: t, text[nxt.end():])
        out.append(text[:m.start()] + f"{ind}{t} = {e};\n" + rest)
    return out


def rw_return(text):
    out = []
    for m in STMT.finditer(text):
        ind, v, e = m.groups()
        nxt = re.compile(r"\s*return " + re.escape(v) + r";\n").match(text, m.end())
        if nxt and uses(v, text) == 2 + decls(v, text):
            out.append(text[:m.start()] + f"{ind}return {e};\n" + text[nxt.end():])
    return out


def rw_inline(text):
    out = []
    for m in STMT.finditer(text):
        ind, v, e = m.groups()
        later = text[m.end():]
        if uses(v, later) == 1 and uses(v, text) == 2 + decls(v, text):
            out.append(text[:m.start()] + re.sub(r"\b" + re.escape(v) + r"\b", lambda _: f"({e})", later, count=1))
    return out


def rw_array(text):
    out = []
    for m in re.finditer(r"^(\s*\w[\w\s\*]*?\b(a\w*Stack_\w+|local_\w+)\s*\[)(\d+)(\];)", text, re.M):
        n = int(m.group(3))
        for k in range(1, n):
            out.append(text[:m.start()] + m.group(1) + str(k) + m.group(4) + text[m.end():])
    return out


SIGN = {"uint": "int", "int": "uint", "ushort": "short", "short": "ushort", "byte": "char", "char": "byte",
        "uchar": "char", "undefined4": "int", "undefined2": "short", "undefined1": "char"}


def rw_sign(text):
    """One local's or parameter's signedness flipped (comparisons, extensions)."""
    out = []
    head, _, body = text.partition("\n{\n")
    for m in re.finditer(r"\b(uint|int|ushort|short|byte|char|uchar|undefined4|undefined2|undefined1) (\w+)(?=[,)])", head):
        out.append(head[:m.start()] + SIGN[m.group(1)] + " " + m.group(2) + head[m.end():] + "\n{\n" + body)
    for m in re.finditer(r"^([ \t]*)(uint|int|ushort|short|byte|char|uchar|undefined4|undefined2|undefined1) (\w+);", body, re.M):
        nb = body[:m.start()] + m.group(1) + SIGN[m.group(2)] + " " + m.group(3) + ";" + body[m.end():]
        out.append(head + "\n{\n" + nb)
    return out


FLIP = {"<": ">", ">": "<", "<=": ">=", ">=": "<="}


def rw_cmp(text):
    """a < b written b > a (which operand GCC loads first)."""
    out = []
    for m in re.finditer(r"\(([\w.\[\]]+) (<=|>=|<|>) ([\w.\[\]]+)\)", text):
        out.append(text[:m.start()] + f"({m.group(3)} {FLIP[m.group(2)]} {m.group(1)})" + text[m.end():])
    return out


def block_end(text, i):
    """Index just past the } closing the { at text[i]."""
    depth = 0
    for j in range(i, len(text)):
        if text[j] == "{":
            depth += 1
        elif text[j] == "}":
            depth -= 1
            if depth == 0:
                return j + 1
    return None


def rw_ifswap(text):
    """if (C) { A } else { B }  ->  if (!(C)) { B } else { A }, at any nesting
    level (blocks found by brace matching; an else-if chain is left alone)."""
    out = []
    for m in re.finditer(r"\bif \(", text):
        # the condition: up to the parenthesis closing this one
        depth, j = 0, m.end() - 1
        for j in range(m.end() - 1, len(text)):
            depth += {"(": 1, ")": -1}.get(text[j], 0)
            if depth == 0:
                break
        cond = text[m.end():j]
        k = j + 1
        if not text.startswith(" {", k):
            continue
        a_end = block_end(text, k + 1)
        if a_end is None:
            continue
        e = re.compile(r"\s*else \{").match(text, a_end)
        if not e:
            continue
        b_open = e.end() - 1
        b_end = block_end(text, b_open)
        if b_end is None:
            continue
        a_body, b_body = text[k + 2:a_end - 1], text[b_open + 1:b_end - 1]
        ws = text[a_end:b_open - len("else ")]
        out.append(text[:m.start()] + f"if (!({cond})) {{{b_body}}}{ws}else {{{a_body}}}" + text[b_end:])
    return out


def rw_early(text):
    """v = A; if (C) { S; v = B; } return v;  ->  early returns, both ways round."""
    out = []
    for m in re.finditer(r"^([ \t]*)(\w+) = ([^;\n]+);\n\1if \(", text, re.M):
        ind, v, a = m.groups()
        depth, j = 1, m.end()
        for j in range(m.end(), len(text)):
            depth += {"(": 1, ")": -1}.get(text[j], 0)
            if depth == 0:
                break
        cond = text[m.end():j]
        if not text.startswith(" {", j + 1):
            continue
        end = block_end(text, j + 2)
        if end is None:
            continue
        body = text[j + 3:end - 1]
        last = re.search(r"\n([ \t]*)" + re.escape(v) + r" = ([^;\n]+);\n[ \t]*$", body)
        tail = re.compile(r"\n[ \t]*return " + re.escape(v) + r";\n").match(text, end)
        if not last or not tail or uses(v, body[:last.start()] + cond) > 0:
            continue
        s_body, b = body[:last.start()], last.group(2)
        inner = last.group(1)
        out.append(text[:m.start()] + f"{ind}if ({cond}) {{{s_body}\n{inner}return {b};\n{ind}}}\n{ind}return {a};\n" + text[tail.end():])
        out.append(text[:m.start()] + f"{ind}if (!({cond})) {{\n{inner}return {a};\n{ind}}}" + s_body.replace("\n" + inner, "\n" + ind) + f"\n{ind}return {b};\n" + text[tail.end():])
    return out


RET_TYPES = ["int", "uint", "short", "ushort", "char", "uchar", "byte", "undefined4", "undefined2", "undefined1", "bool"]


def rw_rettype(text):
    """Another integer type for the function's own return value."""
    m = re.match(r"(int|uint|short|ushort|char|uchar|byte|undefined4|undefined2|undefined1|bool)(\s+\w+\s*\()", text)
    if not m:
        return []
    return [t + text[m.end(1):] for t in RET_TYPES if t != m.group(1)]


def paren_end(text, i):
    """Index of the ) closing the ( at text[i]."""
    depth = 0
    for j in range(i, len(text)):
        depth += {"(": 1, ")": -1}.get(text[j], 0)
        if depth == 0:
            return j
    return None


STEP = re.compile(r"\n[ \t]*(\w+) = \1 \+ ([^;\n]+);[ \t]*$")


def rw_loop(text):
    """A do-while loop as the for or while loop it was probably written as."""
    out = []
    for m in re.finditer(r"([ \t]*)do \{\n", text):
        ind = m.group(1)
        end = block_end(text, m.end() - 2)
        if end is None:
            continue
        w = re.compile(r" while \(").match(text, end)
        if not w:
            continue
        ce = paren_end(text, w.end() - 1)
        if ce is None or not text.startswith(";", ce + 1):
            continue
        cond, body = text[w.end():ce], text[m.end():end - 1]
        tail = ce + 2  # past ");"
        # the steps: trailing "X = X + K;" statements of the body
        steps, b = [], body.rstrip()
        while True:
            sm = STEP.search("\n" + b)
            if not sm:
                break
            steps.insert(0, f"{sm.group(1)} = {sm.group(1)} + {sm.group(2)}")
            b = ("\n" + b)[:sm.start()].lstrip("\n")
        start, guard_end = m.start(), tail
        # a guard: the do-while is all an if's block holds
        g = re.search(r"([ \t]*)if \(([^\n]*)\) \{\n$", text[:m.start()])
        if g:
            close = re.compile(r"\s*\}\n").match(text, tail)
            if close and block_end(text, g.end() - 2) == close.end() - 1:
                start, guard_end, ind = g.start(), close.end() - 1, g.group(1)
        init = ""
        if steps:
            v = steps[0].split(" = ")[0]
            im = re.search(r"([ \t]*)" + re.escape(v) + r" = ([^;\n]+);\n$", text[:start])
            if im:
                init, start = f"{v} = {im.group(2)}", im.start()
                ind = im.group(1)
            body_s = b + "\n"
            out.append(text[:start] + f"{ind}for ({init}; {cond}; {', '.join(steps)}) {{\n{body_s}{ind}}}" + text[guard_end:])
        out.append(text[:start if not init else start] + (f"{ind}{init};\n" if init else "") +
                   f"{ind}while ({cond}) {{\n{body}\n{ind}}}" + text[guard_end:])
    return out


CAST_FLIP = {"byte": "char", "char": "byte", "uchar": "char", "undefined1": "char",
             "ushort": "short", "short": "ushort", "undefined2": "short",
             "uint": "int", "int": "uint", "undefined4": "int"}
NARROW = {}  # DAT_ name -> "char"/"short" for unsigned byte/short globals (vf4ft_data.h)


def rw_castsign(text):
    out = []
    for m in re.finditer(r"\*\((byte|char|uchar|undefined1|ushort|short|undefined2|uint|int|undefined4) \*\)", text):
        out.append(text[:m.start()] + f"*({CAST_FLIP[m.group(1)]} *)" + text[m.end():])
    for m in re.finditer(r"(?<![&\w])(DAT_[0-9a-f]{8})\b(?!\s*=[^=])", text):
        t = NARROW.get(m.group(1))
        if t:
            out.append(text[:m.start()] + f"(({t}){m.group(1)})" + text[m.end():])
    return out


def rw_addrswap(text):
    out = []
    for m in re.finditer(r"\*\((\w+ \*)\)\((\w+) \+ (0x[0-9a-f]+|\d+)\)", text):
        out.append(text[:m.start()] + f"*({m.group(1)})({m.group(3)} + {m.group(2)})" + text[m.end():])
    return out


SIMPLE = re.compile(r"^([ \t]+)(?!return\b|break\b|continue\b|goto\b|if\b|else\b|do\b|while\b|for\b|switch\b|case\b|default\b)"
                    r"([^\s{}][^\n{}]*;)\n", re.M)
DECL_LINE = re.compile(r"^\s*(?:const\s+)?[A-Za-z_]\w*(?:\s+\w+)?\s*\**\s*\w+(?:\s*\[[^\]]*\])*;$")


def rw_swap(text):
    out = []
    body_at = text.find("\n{\n")
    lines = list(SIMPLE.finditer(text, body_at if body_at >= 0 else 0))
    for m, n in zip(lines, lines[1:]):
        if m.end() != n.start() or m.group(1) != n.group(1):
            continue  # not adjacent, or not at one level
        a, b = m.group(2), n.group(2)
        if DECL_LINE.match(a) or DECL_LINE.match(b) or a == b:
            continue  # declarations stay where they are
        out.append(text[:m.start()] + n.group(0) + m.group(0) + text[n.end():])
    return out


BITFIELD_VALUE_FIRST = re.compile(r"^([ \t]*)([^\n=;]+?) =\s+(.+?) \| \2 & ((?:0x[0-9a-f]+|\(\w+\)&DAT_[0-9a-f]+|~[^|;\n]+));$", re.M | re.S)
BITFIELD = re.compile(r"^([ \t]*)([^\n=;]+?) =\s+\2 & ((?:0x[0-9a-f]+|\(\w+\)&DAT_[0-9a-f]+|~[^|;\n]+)) \| ([^;]+);$", re.M)


UNIT = {"uint": ("unsigned int", 32), "int": ("unsigned int", 32), "undefined4": ("unsigned int", 32),
        "ushort": ("unsigned short", 16), "short": ("unsigned short", 16), "undefined2": ("unsigned short", 16),
        "byte": ("unsigned char", 8), "uchar": ("unsigned char", 8), "char": ("unsigned char", 8), "undefined1": ("unsigned char", 8)}


def mask_value(t):
    t = t.strip()
    m = re.fullmatch(r"0x([0-9a-f]+)", t) or re.fullmatch(r"\(\w+\)&DAT_([0-9a-f]+)", t)
    return int(m.group(1), 16) if m else None


def field_value(e, lo, w):
    """The value stored, without the mask and shift GCC adds for a bitfield."""
    e = e.strip()
    full = (1 << w) - 1
    for pat in (r"\((.+) & (0x[0-9a-f]+|\d+)\) << (0x[0-9a-f]+|\d+)", r"(.+) << (0x[0-9a-f]+|\d+)", r"(.+) & (0x[0-9a-f]+|\d+)"):
        m = re.fullmatch(pat, e)
        if not m:
            continue
        g = m.groups()
        if len(g) == 3 and int(g[1], 0) == full and int(g[2], 0) == lo:
            return g[0]
        if pat.startswith("(.+) <<") and int(g[1], 0) == lo:
            return g[0]
        if pat.startswith("(.+) &") and lo == 0 and int(g[1], 0) == full:
            return g[0]
    c = re.fullmatch(r"0x[0-9a-f]+|\d+", e)
    if c:
        return hex((int(e, 0) >> lo) & full)
    return None


def rw_bitfield(text):
    out = []
    forms = [(BITFIELD, lambda m: (m.group(1), m.group(2), m.group(3), m.group(4)))]
    for pat, parts in forms + [(BITFIELD_VALUE_FIRST, lambda m: (m.group(1), m.group(2), m.group(4), m.group(3)))]:
        for m in pat.finditer(text):
            ind, lv, mask, e = parts(m)
            e = " ".join(e.split())  # a statement Ghidra wrapped over lines
            if pat is BITFIELD:
                out.append(text[:m.start()] + f"{ind}{lv} = ({e}) | {lv} & {mask};" + text[m.end():])
            mv = mask_value(mask)
            um = re.fullmatch(r"\*\((\w+) \*\)(.+)", lv)
            if um:
                unit, ptr = UNIT.get(um.group(1)), um.group(2)
            elif re.fullmatch(r"agg_[0-9a-f]+_[0-9a-f]+\.\w+|[A-Za-z_]\w*", lv):
                unit, ptr = ("unsigned int", 32), "&" + lv  # a member or a global: a word
            else:
                continue
            if mv is None or unit is None:
                continue
            typ, bits = unit
            clear = ~mv & ((1 << bits) - 1)
            if not clear:
                continue
            lo = (clear & -clear).bit_length() - 1
            w = bin(clear).count("1")
            if clear != ((1 << w) - 1) << lo:
                continue  # not one run of bits
            v = field_value(e, lo, w)
            if v is None:
                continue
            pad = f"{typ} : {lo}; " if lo else ""
            st = f"struct {{ {pad}{typ} f : {w}; }}"
            out.append(text[:m.start()] + f"{ind}(({st} *)({ptr}))->f = {v};" + text[m.end():])
    return out


BITREAD = re.compile(r"\*\((uint|int|undefined4|ushort|short|undefined2|byte|uchar|char|undefined1) \*\)"
                     r"(\((?:[^()]|\([^()]*\))*\)|\w+)(?: >> (0x[0-9a-f]+|\d+))? & (0x[0-9a-f]+|\d+)\b")


def rw_bitread(text):
    out = []
    for m in BITREAD.finditer(text):
        typ, bits = UNIT[m.group(1)]
        lo = int(m.group(3), 0) if m.group(3) else 0
        mask = int(m.group(4), 0)
        w = mask.bit_length()
        if mask != (1 << w) - 1 or lo + w > bits or (lo == 0 and w == bits):
            continue  # not one run of low bits after the shift
        pad = f"{typ} : {lo}; " if lo else ""
        st = "struct { " + pad + typ + " f : " + str(w) + "; }"
        out.append(text[:m.start()] + "((" + st + " *)" + m.group(2) + ")->f" + text[m.end():])
    return out


FRAME_DELTA = {}  # function name -> bytes the game reserves minus ours (frames.tsv)
ELEM = {"undefined1": 1, "char": 1, "byte": 1, "uchar": 1, "undefined2": 2, "short": 2, "ushort": 2,
        "undefined4": 4, "int": 4, "uint": 4, "float": 4, "undefined8": 8}
STACK_ARRAY = re.compile(r"^([ \t]*)(\w+)(\s*\**\s*)((?:a\w*Stack|local)_\w+)\s*\[(\d+)\];", re.M)


def rw_framefit(text):
    name = re.search(r"\b([A-Za-z_]\w*)\s*\(", text)
    d = FRAME_DELTA.get(name.group(1)) if name else None
    if not d:
        return []
    out = []
    for m in STACK_ARRAY.finditer(text):
        size = 4 if "*" in m.group(3) else ELEM.get(m.group(2))
        if not size or d % size:
            continue
        n = int(m.group(5)) + d // size
        if n < 1:
            continue
        decl = f"{m.group(1)}{m.group(2)}{m.group(3)}{m.group(4)} [{n}];"
        out.append(text[:m.start()] + decl + text[m.end():])
        if n == 1:  # one element: a plain local (x[0] -> x, x -> &x)
            v = re.escape(m.group(4))
            rest = re.sub(r"\b" + v + r"\s*\[0\]", "\0", text[m.end():])
            rest = re.sub(r"\b" + v + r"\b(?!\s*\[)", "&" + m.group(4), rest).replace("\0", m.group(4))
            out.append(text[:m.start()] + f"{m.group(1)}{m.group(2)}{m.group(3)}{m.group(4)};" + rest)
    return out


def load_frames():
    p = os.path.join(ROOT, "build", "automatch", "frames.tsv")
    if os.path.exists(p):
        for l in open(p).read().split("\n")[1:]:
            f = l.split("\t")
            if len(f) == 6 and f[2] == f[4] and f[3] != f[5]:  # same registers saved, other space
                FRAME_DELTA[f[1]] = int(f[3]) - int(f[5])


MIXED = set()  # callees vf4ft_functions.h guards with NOPROTO_<name>


def rw_noproto(text):
    if text.startswith("#define NOPROTO_"):
        return []  # one set per variant
    called = [n for n in dict.fromkeys(re.findall(r"\b([A-Za-z_]\w*)\s*\(", text.split("{", 1)[-1])) if n in MIXED]
    out = [f"#define NOPROTO_{n}\n" + text for n in called]
    if len(called) > 1:
        out.append("".join(f"#define NOPROTO_{n}\n" for n in called) + text)
    return out


KR = set()  # callees vf4ft_functions.h can declare old-style (KRPROTO_<name>)


def rw_krproto(text):
    if text.startswith("#define "):
        return []
    called = [n for n in dict.fromkeys(re.findall(r"\b([A-Za-z_]\w*)\s*\(", text.split("{", 1)[-1])) if n in KR]
    out = [f"#define KRPROTO_{n}\n" + text for n in called]
    if len(called) > 1:
        out.append("".join(f"#define KRPROTO_{n}\n" for n in called) + text)
    return out


def load_mixed():
    p = os.path.join(os.environ.get("VF4FT_INC") or os.path.join(program.SRC, "include"), "vf4ft_functions.h")
    h = open(p).read()
    MIXED.update(re.findall(r"^#if(?:ndef|\s+defined\()\s*NOPROTO_(\w+)\)?$", h, re.M))
    KR.update(re.findall(r"^#elif defined\(KRPROTO_(\w+)\)$", h, re.M))


WIDE = r"(?:int|uint|undefined4)"


def rw_ptrbase(text):
    head, sep, body = text.partition("\n{\n")
    if not sep:
        return []
    out = []
    names = set(re.findall(r"\*\(\w+ \*\)\((\w+) \+ (?:0x[0-9a-f]+|\d+)\)", body))
    for n in names:
        hm = re.search(r"\b" + WIDE + r" " + re.escape(n) + r"(?=[,)])", head)
        if hm:
            out.append(head[:hm.start()] + "undefined *" + n + head[hm.end():] + sep + body)
            continue
        bm = re.search(r"^([ \t]*)" + WIDE + r" " + re.escape(n) + r";", body, re.M)
        if bm:
            out.append(head + sep + body[:bm.start()] + f"{bm.group(1)}undefined *{n};" + body[bm.end():])
    return out


def rw_baselocal(text):
    head, sep, body = text.partition("\n{\n")
    if not sep or "baselocal_p" in body:
        return []
    out = []
    bases = set(re.findall(r"\*\(\w+ \*\)\(((?:PTR|DAT)_\w+) \+ (?:0x[0-9a-f]+|\d+)\)", body))
    bases |= {"&" + m for m in re.findall(r"\(&(agg_[0-9a-f]+_[0-9a-f]+\.field_0x0|[A-Za-z_]\w*)\)\[", body)}
    for b in bases:
        expr = f"({b})" if b.startswith("&") else b
        new = body.replace(expr if b.startswith("&") else b, "baselocal_p")
        # declarations first, then the local's assignment
        lines = new.split("\n")
        k = 0
        while k < len(lines) and (re.match(r"^\s*[A-Za-z_][\w \*]*\b\w+(\s*\[[^\]]*\])?;\s*$", lines[k]) or not lines[k].strip()):
            k += 1
        decl = "  undefined *baselocal_p;"
        init = f"  baselocal_p = (undefined *){b};"
        out.append(head + sep + "\n".join([decl] + lines[:k] + [init] + lines[k:]))
    return out


def rw_narrowparam(text):
    head, sep, body = text.partition("\n{\n")
    if not sep:
        return []
    out = []
    for m in re.finditer(r"\b" + WIDE + r" (param_\d+)(?=[,)])", head):
        n = m.group(1)
        casts = re.findall(r"\((undefined1|undefined2|byte|char|short|ushort)\)" + re.escape(n) + r"\b", body)
        if casts and len(casts) == len(re.findall(r"\b" + re.escape(n) + r"\b", body)):
            t = {"undefined1": "byte", "undefined2": "ushort"}.get(casts[0], casts[0])
            out.append(head[:m.start()] + f"{t} {n}" + head[m.end():] + sep + body)
    return out


CMPOP = re.compile(r"\s(==|!=|<=|>=|<|>)\s")


def top_compare(e):
    """(left, op, right) when e is one comparison at its top level."""
    depth, at = 0, None
    for i, ch in enumerate(e):
        depth += {"(": 1, ")": -1}.get(ch, 0)
        if depth == 0:
            m = CMPOP.match(e, i)
            if m:
                if at is not None:
                    return None
                at = m
    if at is None or "&&" in e or "||" in e:
        return None
    return e[:at.start()].strip(), at.group(1), e[at.end():].strip()


def rw_boolflag(text):
    out = []
    for m in re.finditer(r"^([ \t]*)return (.+);$", text, re.M):
        e = m.group(2).strip()
        if e.startswith("(") and paren_end(e, 0) == len(e) - 1:
            e = e[1:-1]  # outer parentheses around all of it
        if top_compare(e):
            ind = m.group(1)
            out.append(text[:m.start()] + f"{ind}int boolflag_r = 0;\n{ind}if ({e}) boolflag_r = 1;\n{ind}return boolflag_r;"
                       + text[m.end():])
    for m in re.finditer(r"^([ \t]*)([^\n]*)\((?:uint|int|bool|byte|char)\)\(([^()\n]*(?:\([^()\n]*\)[^()\n]*)*)\)([^\n]*;)$", text, re.M):
        if top_compare(m.group(3)):
            ind = m.group(1)
            out.append(text[:m.start()] + f"{ind}int boolflag_r = 0;\n{ind}if ({m.group(3)}) boolflag_r = 1;\n{ind}{m.group(2)}boolflag_r{m.group(4)}"
                       + text[m.end():])
    return out


def rw_retlocal(text):
    if not text.startswith("void "):
        return []
    head, sep, body = text.partition("\n{\n")
    out = []
    for m in re.finditer(r"^[ \t]*(int|uint|undefined4|short|ushort) (\w+);$", body, re.M):
        t, v = m.groups()
        nb = re.sub(r"\n([ \t]*)return;\n\}\s*$", f"\n\\1return {v};\n}}\n", body)
        if nb != body:
            out.append(t + text[4:len(head)] + sep + nb)
    return out


def rw_cmp_wide(text):
    """(A op B) -> (B op' A) for one top-level comparison inside parentheses."""
    out = []
    for m in re.finditer(r"\(", text):
        end = paren_end(text, m.start())
        if end is None or end - m.start() > 200:
            continue
        inner = text[m.start() + 1:end]
        c = top_compare(inner)
        if c and c[1] in FLIP:
            l, op, r = c
            out.append(text[:m.start() + 1] + f"{r} {FLIP[op]} {l}" + text[end:])
    return out


ELT = {"undefined4": 4, "int": 4, "uint": 4, "float": 4, "undefined2": 2, "short": 2, "ushort": 2,
       "undefined1": 1, "byte": 1, "char": 1, "uchar": 1}


def rw_structarray(text):
    out = []
    for m in re.finditer(r"\*\((\w+) \*\)\(&(DAT_[0-9a-f]{8}) \+ ([^()\n]+?) \* (0x[0-9a-f]+|\d+)\)", text):
        t, base, idx, stride = m.group(1), m.group(2), m.group(3), int(m.group(4), 0)
        size = ELT.get(t)
        if not size or stride > 0x400 or stride < 2 * size:
            continue
        for k in range(size, stride - size + 1, size):
            st = f"struct {{ char pad[{k}]; {t} f; char rest[{stride - k - size}]; }}" if stride - k - size else \
                 f"struct {{ char pad[{k}]; {t} f; }}"
            out.append(text[:m.start()] + f"(({st} *)((char *)&{base} - {k}))[{idx}].f" + text[m.end():])
    return out


def rw_voidcall(text):
    out = []
    for m in re.finditer(r"^([ \t]*)\(\*\(code \*\)", text, re.M):
        out.append(text[:m.start()] + m.group(1) + "(*(void (*)())" + text[m.end():])
    return out


def rw_hoist(text):
    out = []
    for m in re.finditer(r"^([ \t]*)([A-Za-z_][\w.]*) = \2 ([-+|&^]) \((.+)\);$", text, re.M):
        ind, lv, op, e = m.groups()
        if e.count("(") != e.count(")") or "hoist_t" in text:
            continue
        for t in ("int", "uint"):
            out.append(text[:m.start()] + f"{ind}{t} hoist_t = {e};\n{ind}{lv} = {lv} {op} hoist_t;" + text[m.end():])
    return out


CONDCALL = re.compile(r"^([ \t]*)(\w+) = (\w+);\n\1if \((.+)\) \{\n[ \t]*\2 = (\w+);\n\1\}\n\1\(\*\2\)\(([^;\n]*)\);\n", re.M)


def rw_condcall(text):
    out = []
    for m in CONDCALL.finditer(text):
        ind, p, a, c, b, args = m.groups()
        out.append(text[:m.start()] + f"{ind}if ({c}) {b}({args}); else {a}({args});\n" + text[m.end():])
        out.append(text[:m.start()] + f"{ind}if (!({c})) {a}({args}); else {b}({args});\n" + text[m.end():])
    return out


NEG = {"<": ">=", ">": "<=", "<=": ">", ">=": "<", "==": "!=", "!=": "=="}


def rw_guardret(text):
    m = re.search(r"\n([ \t]*)if \((.+)\) \{\n((?:(?!\breturn\b).)*?)\n\1\}\n\1return ([^;\n]+);\n\}\s*$", text, re.S)
    if not m:
        return []
    ind, c, body, v = m.groups()
    tc = top_compare(c)
    neg = f"{tc[0]} {NEG[tc[1]]} {tc[2]}" if tc else f"!({c})"
    body = "\n".join(l[len(ind):] if l.startswith(ind + ind) else l for l in body.split("\n"))
    return [text[:m.start()] + f"\n{ind}if ({neg}) return {v};\n{body}\n{ind}return {v};\n}}\n"]


def rw_notflag(text):
    out = []
    pat = re.compile(r"\n([ \t]*)return (?:\((?:bool|uint|int|undefined[14]?|char|uchar)\))?\(?([^;\n]+?)\)?;")
    open_at = text.index("{\n") + 2 if "{\n" in text else None
    if open_at is None:
        return out
    found = []
    for m in pat.finditer(text):
        tc = top_compare(m.group(2))
        if tc and tc[1] in NEG and "t_nf" not in text:
            found.append((m, f"\n{m.group(1)}t_nf = {tc[0]} {NEG[tc[1]]} {tc[2]};\n{m.group(1)}return t_nf == 0;"))
    for m, new in found:
        out.append(text[:open_at] + "  int t_nf;\n" + text[open_at:m.start()] + new + text[m.end():])
    if len(found) > 1:
        t = text
        for m, new in reversed(found):
            t = t[:m.start()] + new + t[m.end():]
        out.append(t[:open_at] + "  int t_nf;\n" + t[open_at:])
    return out


MOVSTR = re.compile(r"^([ \t]*)__movstr_i4_(odd|even)\(((?:[^(),]|\((?:[^()]|\([^()]*\))*\))+),"
                    r"((?:[^(),]|\((?:[^()]|\([^()]*\))*\))+),(0x[0-9a-f]+|\d+)\);$", re.M)


def rw_movstr(text):
    out = []
    open_at = text.index("{\n") + 2 if "{\n" in text else None
    for m in MOVSTR.finditer(text):
        ind, kind, d, s, n = m.groups()
        if open_at is None or re.search(r"\b0x0?c[0-9a-f]{6}\b", d + s):
            continue  # a raw main-RAM address: leave it for a symbol
        words = (int(n, 0) + 1) * 2 + (kind == "odd")
        t = f"movstr_blk{words}"
        new = f"{ind}*({t} *)({d.strip()}) = *({t} *)({s.strip()});"
        decl = f"  typedef struct {{ int w[{words}]; }} {t};\n"
        body = text[:m.start()] + new + text[m.end():]
        out.append(body[:open_at] + (decl if decl not in body else "") + body[open_at:])
    return out


HWREG = {}  # hardware register symbol (DAT_a05f6c18, _DAT_e0000000) -> its declared type (vf4ft_data.h)
# a symbol's address cast to a number ((uint)&DAT_ffe0ffff) is a mask Ghidra
# shows as an address, not a register: left alone
HWUSE = re.compile(r"(?<!\(uint\))(?<!\(int\))(&)?\b(_?DAT_(?:a[0-9a-f]|e[0-9a-f]|ff)[0-9a-f]{6})\b")


def load_hwreg():
    p = os.path.join(os.environ.get("VF4FT_INC") or os.path.join(program.SRC, "include"), "vf4ft_data.h")
    for m in re.finditer(r"^extern (?:volatile )?([^;]+?)\s*\b(_?DAT_(?:a[0-9a-f]|e[0-9a-f]|ff)[0-9a-f]{6});", open(p).read(), re.M):
        HWREG[m.group(2)] = m.group(1)


def rw_hwconst(text):
    """Every hardware register symbol as its address, in one step (the gain
    is GCC seeing the neighbours together): volatile, as the original's
    register headers were, and plain (the cache arrays and store queues
    match plain)."""
    out = []
    for q in ("volatile ", ""):
        def sub(m):
            t = HWREG.get(m.group(2))
            if t is None:
                return m.group(0)
            addr = "0x" + m.group(2)[-8:]
            return f"(({q}{t} *){addr})" if m.group(1) else f"(*({q}{t} *){addr})"
        new = HWUSE.sub(sub, text)
        if new != text:
            out.append(new)
    return out


def rw_shiftmask(text):
    out = []
    for m in re.finditer(r"(<<|>>) \(([^()\n]*(?:\([^()\n]*\)[^()\n]*)*) & 0x1f\)", text):
        out.append(text[:m.start()] + f"{m.group(1)} ({m.group(2)})" + text[m.end():])
    if len(out) > 1:  # all of them at once (a handler masks each shift)
        out.append(re.sub(r"(<<|>>) \(([^()\n]*(?:\([^()\n]*\)[^()\n]*)*) & 0x1f\)", r"\1 (\2)", text))
    return out


def rw_indexptr(text):
    out = []
    for m in re.finditer(r"\(&([A-Za-z_]\w*)\)\[([^\[\]\n]+)\]", text):
        out.append(text[:m.start()] + f"((char *)({m.group(2)}))[(int)&{m.group(1)}]" + text[m.end():])
    for m in re.finditer(r"\*\((\w+ \*)\)\(&([A-Za-z_]\w*) \+ ([^()\n]+(?:\([^()\n]*\)[^()\n]*)*)\)", text):
        out.append(text[:m.start()] + f"*({m.group(1)})((char *)({m.group(3)}) + (int)&{m.group(2)})" + text[m.end():])
    if len(out) > 1:
        t = re.sub(r"\(&([A-Za-z_]\w*)\)\[([^\[\]\n]+)\]", r"((char *)(\2))[(int)&\1]", text)
        out.append(t)
    return out


def load_narrow():
    p = os.path.join(program.SRC, "include", "vf4ft_data.h")
    for m in re.finditer(r"^extern (undefined1|byte|uchar|undefined2|ushort) (DAT_[0-9a-f]{8});", open(p).read(), re.M):
        NARROW[m.group(2)] = "char" if m.group(1) in ("undefined1", "byte", "uchar") else "short"


# targeted rules first: variants() keeps the first cap it builds
RULES = [rw_condcall, rw_guardret, rw_notflag, rw_movstr, rw_hwconst, rw_shiftmask, rw_indexptr, rw_hoist, rw_structarray, rw_voidcall, rw_krproto, rw_boolflag, rw_retlocal, rw_ptrbase, rw_baselocal, rw_narrowparam, rw_noproto, rw_framefit, rw_loop, rw_castsign, rw_bitfield, rw_bitread, rw_addrswap, rw_swap, rw_store, rw_return, rw_inline, rw_early, rw_ifswap,
         rw_rettype, rw_sign, rw_cmp, rw_cmp_wide, rw_array]


def variants(text, cap=256, depth=3):
    """Every combination of up to depth rewrites (breadth first), at most cap."""
    seen, out, level = {text}, [], [text]
    for _ in range(depth):
        nxt = []
        for t in level:
            for r in RULES:
                for u in r(t):
                    if u not in seen:
                        seen.add(u)
                        nxt.append(u)
        out += nxt
        level = nxt
        if len(out) >= cap:
            break
    return out[:cap]


def main():
    args = sys.argv[1:]
    opt = lambda k, d: type(d)(args[args.index(k) + 1]) if k in args else d
    src = opt("--from", os.path.join(ROOT, "build", "automatch", "results.tsv"))
    limit = opt("--limit", 0)
    out = opt("--out", os.path.join(ROOT, "build", "automatch", "variants.json"))
    rows = [l.split("\t") for l in open(src).read().split("\n")[1:] if l.count("\t") == 5]
    want = {int(r[0], 16): r[4] for r in rows if r[5] == "DIFF"}
    cands = sorted((c for c in am.candidates() if c[0] in want), key=lambda c: c[1])
    if limit:
        cands = cands[:limit]
    load_narrow()
    load_frames()
    load_mixed()
    load_hwreg()
    if "--rules" in args:
        names = set(opt("--rules", "").split(","))
        RULES[:] = [r for r in RULES if r.__name__[3:] in names]
        print("rules: " + ", ".join(r.__name__[3:] for r in RULES), flush=True)
    jobs = opt("--jobs", max(1, (os.cpu_count() or 2) - 4))
    work, tally = [], Counter()
    for flags in ("-fno-delayed-branch", "-"):
        # variants with NOPROTO_ defines go in batches of their own (a define
        # is for the whole file): keyed by the defines they carry
        batches = {}
        for c in (c for c in cands if want[c[0]] == flags):
            vs = variants(c[4])
            tally["variants"] += len(vs)
            for k, t in enumerate(vs):
                key = "".join(am.NOPROTO.findall(t))
                b = batches.setdefault(key, [])
                b.append((c[0], c[3], k, t))
                if len(b) >= 60:
                    work.append((flags, b))
                    batches[key] = []
        work += [(flags, b) for b in batches.values() if b]
    print(f"{len(cands)} functions, {tally['variants']} variants in {len(work)} batches on {jobs} processes", flush=True)
    found = {}
    with multiprocessing.Pool(jobs) as p:
        for i, res in enumerate(p.imap_unordered(run_batch, work)):
            for a, t in res:
                found.setdefault(a, t)
            if i % 50 == 0:
                print(f"  {i + 1}/{len(work)} batches: {len(found)} match", flush=True)
    json.dump({f"{a:08x}": t for a, t in found.items()}, open(out, "w"), indent=0)
    print(f"{len(cands)} functions, {tally['variants']} variants tried: {len(found)} match -> {out}")


_ctx = {}


def run_batch(job):
    """Compile one batch of variants (each a function of its own:
    V<k>_<address>); [(address, text)] for the ones that match."""
    flags, batch = job
    if not _ctx:
        _ctx.update(img=open(program.IMAGE, "rb").read(), symtab=match.load_symbols(), slots=match.game_slots())
    funcs = [(None, None, None, f"V{k}_{a:08x}", re.sub(r"\b" + re.escape(name) + r"\b(?=\s*\()", f"V{k}_{a:08x}", t, count=1))
             for a, name, k, t in batch]
    with tempfile.TemporaryDirectory(dir=os.path.join(ROOT, "build")) as tmp:
        ok = am.split_compilable(funcs, [] if flags == "-" else [flags], tmp, [])
        if not ok:
            return []
        f = os.path.join(tmp, "batch.c")
        am.write_batch(f, ok, [] if flags == "-" else [flags])
        try:
            _, _, res = match.run_checks([f], _ctx["img"], _ctx["symtab"], _ctx["slots"], say=lambda *a, **kw: None)
        except SystemExit:
            return []
    good = {r[0] for r in res if r[3] == "match"}
    return [(a, t) for a, name, k, t in batch if f"V{k}_{a:08x}" in good]


if __name__ == "__main__":
    main()
