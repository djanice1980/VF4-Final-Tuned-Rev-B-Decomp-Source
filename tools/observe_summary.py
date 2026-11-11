#!/usr/bin/env python3
"""Turn the logs of tools/flycast_observe.py into a digest per function, for
a reader who judges each function's description against what the running
game did.

    python3 tools/observe_summary.py [LOG.jsonl ...] [--out DIR] [--top N]

Without logs it reads every work/observe/*.jsonl. It writes, under --out
(default work/observe):
  summary.tsv          one row per function of ghidra/source_map.tsv: hits,
                       callers, modes, the values of each argument register,
                       the results, notes
  never_hit.tsv        the functions no log has a hit for, with the reason
                       when the observer could not set a breakpoint
  digest/<library>.md  the same per library, grouped by source file, with
                       the prototype and the description beside the
                       observations
  digest/index.md      the coverage counts per library
and prints the coverage counts.

How values are shown. A register value that points into memory is named:
  name / name+0x10     a named global of src/include/vf4ft_globals.h (from
                       its address up to the next global)
  &name / name+0x10    code: a function entry, or an address inside one
  stack+0x8            the stack, relative to r15 at the entry (the caller's
                       frame is above it); stack(0x...) elsewhere in the
                       stack area
  heap                 main_heap_area .. 0x0DFF0000 (malloc.c); heap
                       addresses differ from run to run, so they count as
                       one value
The bytes the observer read at a pointer are shown as text when they are a
string, else as words. Arguments are labeled from the prototype in
src/include/vf4ft_functions.h: the integer and pointer parameters are in
r4-r7 in order, the float parameters in fr4-fr7 in order (docs/progress.md:
float arguments are in order), further ones on the stack; the registers the
prototype does not use are listed after them, because a value that varies
there can mean an undeclared parameter."""
import argparse
import bisect
import collections
import csv
import glob
import json
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RAM_LO, RAM_HI = 0x0C000000, 0x0E000000
BSS_END = 0x0C618000      # _start clears 0x0C360000-0x0C618000 (pool words at 0x0C020090)
HEAP_END = 0x0DFF0000     # MAIN_HEAP_END in vf4lib/libcore/src/malloc.c
STACK_TOP = 0x0DFFC000    # r15 at _start (pool word at 0x0C0200A4)


def norm(a):
    return a & 0x1FFFFFFF if (a >> 29) in (4, 5) else a


def library(path):
    p = path.split("/")
    if p[0] in ("vf4lib", "sdk", "naomi2lib") and len(p) > 1:
        return p[1]
    return p[0]


class Names:
    """the repo's names: functions, globals, prototypes, descriptions"""

    def __init__(self, root=ROOT):
        rows = list(csv.DictReader(open(os.path.join(root, "ghidra", "source_map.tsv")), delimiter="\t"))
        self.funcs = sorted((int(r["address"], 16), r["name"], int(r["size"]), r["file"]) for r in rows)
        self.fstart = [f[0] for f in self.funcs]
        self.fname = {f[0]: f[1] for f in self.funcs}
        self.code_lo = self.fstart[0]
        self.code_hi = (self.funcs[-1][0] + self.funcs[-1][2] + 0x1F) & ~0x1F
        self.globals = []
        pat = re.compile(r"^extern\s+(.*?)\s*;\s*/\*\s*0x([0-9a-fA-F]{8})\s*\*/")
        path = os.path.join(root, "src", "include", "vf4ft_globals.h")
        if os.path.exists(path):
            for line in open(path):
                m = pat.match(line)
                if m:
                    decl = m.group(1)
                    name = re.sub(r"(\[[^\]]*\])+$", "", decl).split()[-1].lstrip("*")
                    name = re.sub(r"[()]", "", name)
                    # "undefined name" is a bare label, often inside a larger object (rob0_fl_rate
                    # lies inside robs): an address past it is named from the object before it
                    label = re.match(r"^undefined1?\s+\w+$", decl) is not None
                    # a plain scalar's size, so that an address well past it is not
                    # taken to lie inside it (None: an array, a record, unknown)
                    sm = re.match(r"^(?:volatile\s+|const\s+)*(char|uchar|byte|bool|short|ushort|int|uint|long|ulong|float|undefined[1248]?)\s+\w+$", decl)
                    size = None
                    if sm and not label:
                        size = {"char": 1, "uchar": 1, "byte": 1, "undefined1": 1, "short": 2, "ushort": 2,
                                "undefined2": 2, "undefined8": 8}.get(sm.group(1), 4)
                    elif re.match(r"^.*\*\s*\w+$", decl):
                        size = 4
                    self.globals.append((int(m.group(2), 16), name, label, size))
        self.globals.sort()
        self.gaddr = [g[0] for g in self.globals]
        self.heap_lo = next((g[0] for g in self.globals if g[1] == "main_heap_area"), 0x0C700000)
        self.proto = {}
        pat = re.compile(r"^(.*\));\s*/\*\s*0x([0-9a-fA-F]{8})\s*\*/")
        path = os.path.join(root, "src", "include", "vf4ft_functions.h")
        if os.path.exists(path):
            for line in open(path):
                m = pat.match(line)
                if m:
                    self.proto[int(m.group(2), 16)] = m.group(1).strip()
        self.desc, self.conf = {}, {}
        csv.field_size_limit(1 << 30)
        for path in sorted(glob.glob(os.path.join(root, "ghidra", "symbols", "*.tsv"))):
            for r in csv.reader(open(path), delimiter="\t", quoting=csv.QUOTE_NONE):
                if len(r) < 5 or r[1] != "function":
                    continue
                a = int(r[0], 16)
                if a not in self.desc:
                    self.desc[a] = r[4]
                elif r[4] not in self.desc[a]:
                    self.desc[a] += " | " + r[4]
                if self.conf.get(a) != "named":
                    self.conf[a] = r[3]

    def global_at(self, a):
        """the named global an address lies in, as text"""
        k = bisect.bisect_right(self.gaddr, a) - 1
        if k < 0:
            return None
        g, name, label, size = self.globals[k]
        if a == g:
            return name
        g0, name0 = g, name
        while label and k > 0:           # past a bare label: the object it lies in
            k -= 1
            g, name, label, size = self.globals[k]
        if size is not None and a - g >= size:
            # the object before the label is a scalar that ends before this
            # address: the label starts an untyped buffer of its own
            # (rob_bone_buf after the int mot_bind_values_cursor)
            g, name = g0, name0
        return "%s+%#x" % (name, a - g)

    def function_at(self, a):
        k = bisect.bisect_right(self.fstart, a) - 1
        if k < 0 or not self.code_lo <= a < self.code_hi:
            return None
        s = self.fstart[k]
        return self.fname[s], a - s

    def resolve(self, v, sp=None, exact=False):
        """(text, kind) for a register value; kind is one of int, code,
        global, stack, heap, ram, io. exact: with the address of a heap
        block (for one call; the counts take every heap address as one)"""
        a = norm(v)
        if sp is not None and RAM_LO <= a < RAM_HI:
            d = a - norm(sp)
            if 0 <= d < 0x4000:
                return "stack+%#x" % d, "stack"
            if -0x400 <= d < 0:
                return "stack-%#x" % -d, "stack"
        if self.code_lo <= a < self.code_hi and (v >> 29) in (0, 4, 5):
            k = bisect.bisect_right(self.gaddr, a) - 1
            if k >= 0 and self.gaddr[k] == a:
                return self.globals[k][1], "global"      # a data object inside the code
            f = self.function_at(a)
            return ("&" + f[0] if f[1] == 0 else "%s+%#x" % f), "code"
        if RAM_LO <= a < RAM_HI and (v >> 29) in (0, 4, 5):
            if HEAP_END <= a <= STACK_TOP:
                return "stack(%#010x)" % a, "stack"
            if self.heap_lo <= a < HEAP_END:
                return ("heap(%#010x)" % a if exact else "heap"), "heap"
            if a < BSS_END:
                text = self.global_at(a)
                if text:
                    return text, "global"
            return "ram(%#010x)" % a, "ram"
        if v >= 0xA0000000:
            k = bisect.bisect_right(self.gaddr, v) - 1
            if k >= 0 and 0 <= v - self.gaddr[k] < 0x100 and self.gaddr[k] >= 0xA0000000:
                g, name = self.globals[k][:2]
                return (name if v == g else "%s+%#x" % (name, v - g)), "io"
        if v >= 0xFFFF0000:
            return str(v - (1 << 32)), "int"
        return (str(v) if v < 10 else "%#x" % v if v > 0xFFFF else "%d (%#x)" % (v, v)), "int"


def peek_text(hexbytes):
    """a string when the bytes are one, else the first words"""
    b = bytes.fromhex(hexbytes)
    end = b.find(b"\0")
    s = b if end < 0 else b[:end]
    if len(s) >= 3 and all(0x20 <= c < 0x7F for c in s):
        return '"%s%s"' % (s.decode("ascii"), "" if end >= 0 else "...")
    words = struct.unpack_from("<%dI" % (len(b) // 4), b)
    return "[" + " ".join("%08x" % w for w in words[:4]) + "]"


def fnum(v):
    return "%.6g" % v if v is not None else "nan/inf"


def params(proto):
    """[(name, 'r4' | 'fr4' | 'stack+0' ...)] and the return kind from a C
    prototype, as far as the plain cases go"""
    if not proto or "(" not in proto:
        return [], None
    head, _, rest = proto.partition("(")
    args = rest.rsplit(")", 1)[0].strip()
    ret = head.rsplit(None, 1)[0].strip() if " " in head.strip() else ""
    star = "*" in head
    rkind = "void" if ret == "void" and not star else "float" if ret in ("float", "double") and not star else "int"
    out = []
    if args in ("", "void"):
        return out, rkind
    ni = nf = ns = 0
    depth, cur, parts = 0, "", []
    for ch in args:
        depth += ch == "("
        depth -= ch == ")"
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    parts.append(cur)
    for p in parts:
        p = p.strip()
        if p == "...":
            out.append(("...", "variadic"))
            continue
        m = re.search(r"\(\s*\*\s*(\w+)\s*\)", p)
        name = m.group(1) if m else re.sub(r"(\[[^\]]*\])+$", "", p).split()[-1].lstrip("*") if p.split() else "?"
        is_float = re.match(r"^(const\s+)?(float|double)\s+\w+$", p) is not None
        if is_float and nf < 4:
            out.append((name, "fr%d" % (4 + nf)))
            nf += 1
        elif not is_float and ni < 4:
            out.append((name, "r%d" % (4 + ni)))
            ni += 1
        else:
            out.append((name, "stack+%d" % (4 * ns)))
            ns += 1
    return out, rkind


class Stats(collections.Counter):
    entered = None


class Obs:
    __slots__ = ("hits", "runs", "callers", "modes", "regs", "fregs", "stk", "peeks", "rets", "r0", "fr0", "r0peek",
                 "notes", "noret", "skip", "sps", "calls")

    def __init__(self):
        self.hits = 0
        self.runs = set()
        self.callers = collections.Counter()
        self.modes = collections.Counter()
        self.regs = [collections.Counter() for _ in range(4)]
        self.fregs = [collections.Counter() for _ in range(4)]
        self.stk = [collections.Counter() for _ in range(4)]
        self.peeks = {}
        self.rets = 0
        self.r0 = collections.Counter()
        self.fr0 = collections.Counter()
        self.r0peek = {}
        self.notes = collections.Counter()
        self.noret = collections.Counter()
        self.skip = None
        self.sps = set()
        self.calls = []            # single calls, for the sample lines


def load(paths, names):
    obs = collections.defaultdict(Obs)
    mode_names = {}
    hit_fn = {}
    stats = Stats()
    entered = collections.Counter()      # modes the game was in while a session watched
    for path in paths:
        for n, line in enumerate(open(path), 1):
            try:
                e = json.loads(line)
            except ValueError:
                stats["unreadable lines"] += 1     # a line cut off by a kill
                continue
            k = e.get("k")
            if k == "start":
                mode_names.update(e.get("mode_names", {}))
                stats["sessions"] += 1
                entered[e.get("mode")] += 0
            elif k == "mode":
                entered[e["to"]] += 0
            elif k == "skip":
                obs[int(e["fn"], 16)].skip = e["why"]
            elif k == "hit":
                a = int(e["fn"], 16)
                o = obs[a]
                o.hits += 1
                o.runs.add(e["id"].rsplit(":", 1)[0])
                if e.get("exc"):
                    who = "an exception (PR, the interrupted code's: %s)" % (
                        "%s+%#x" % (e["caller"], e["coff"]) if e.get("caller") else e["pr"])
                elif e.get("caller"):
                    who = "%s+%#x" % (e["caller"], e["coff"])
                else:
                    who = "outside the image (%s)" % e["pr"]
                o.callers[who] += 1
                call = {"who": who, "mode": e["mode"], "fc": e["fc"], "ret": None,
                        "r": [names.resolve(v, e["sp"], True)[0] for v in e["r"]],
                        "fr": [fnum(v) for v in e["fr"]],
                        "stk": [names.resolve(v, e["sp"], True)[0] for v in e.get("stk", [])]}
                if len(o.calls) < 64:
                    o.calls.append(call)
                hit_fn[e["id"]] = (a, e["sp"], call)
                o.modes[e["mode"]] += 1
                entered[e["mode"]] += 1
                sp = e["sp"]
                o.sps.add(sp)
                pk = e.get("pk", {})
                for i in range(4):
                    text, kind = names.resolve(e["r"][i], sp)
                    o.regs[i][text] += 1
                    b = pk.get(str(4 + i))
                    if b and (i, text) not in o.peeks:
                        o.peeks[(i, text)] = peek_text(b)
                    o.fregs[i][fnum(e["fr"][i])] += 1
                for i, w in enumerate(e.get("stk", [])[:4]):
                    o.stk[i][names.resolve(w, sp)[0]] += 1
                if "pc" in e:
                    o.notes["ran through the mirror address %s" % e["pc"]] += 1
                if e["ret"] != 1:
                    stats["hits without return capture"] += 1
                stats["hits"] += 1
            elif k == "ret":
                a = int(e["fn"], 16)
                o = obs[a]
                o.rets += 1
                h = hit_fn.get(e["id"], (a, e["sp"], None))
                sp = h[1]
                if h[2] is not None:
                    h[2]["ret"] = (names.resolve(e["r0"], sp, True)[0], fnum(e["fr0"]), e.get("dfc", 0))
                text, kind = names.resolve(e["r0"], sp)
                o.r0[text] += 1
                if e.get("pk") and text not in o.r0peek:
                    o.r0peek[text] = peek_text(e["pk"])
                o.fr0[fnum(e["fr0"])] += 1
                if e.get("dsp"):
                    o.notes["came back with r15 %d above its entry value" % e["dsp"]] += 1
                stats["returns"] += 1
            elif k == "noret":
                obs[int(e["fn"], 16)].noret[e["why"]] += 1
            elif k == "note":
                stats["note: " + e.get("what", "?")] += 1
    stats.entered = entered
    return obs, mode_names, stats


def modes_report(mode_names, entered):
    """the game's modes with the hits logged in each, and the ones no session was in"""
    subs = sorted((k for k in mode_names if "." in k), key=lambda k: [int(x) for x in k.split(".")])
    seen = ["%s (%d hits)" % (mode_names[k], entered[k]) for k in subs if k in entered]
    missing = [mode_names[k] for k in subs if k not in entered]
    return seen, missing


def top(counter, n, peeks=None, idx=None):
    items = counter.most_common()
    out = []
    for text, c in items[:n]:
        s = "%s x%d" % (text, c)
        if peeks is not None and (idx, text) in peeks:
            s += " -> " + peeks[(idx, text)]
        out.append(s)
    if len(items) > n:
        out.append("... %d more values" % (len(items) - n))
    return out


def varies(counter):
    return len(counter) > 1


def write(names, obs, mode_names, stats, out, topn, samples=4):
    os.makedirs(os.path.join(out, "digest"), exist_ok=True)
    mname = lambda m: "%s (%s)" % (mode_names[m], m) if m in mode_names else m
    libs = collections.defaultdict(lambda: collections.defaultdict(list))
    for a, name, size, path in names.funcs:
        libs[library(path)][path].append(a)
    cover = {}
    with open(os.path.join(out, "summary.tsv"), "w") as tsv, open(os.path.join(out, "never_hit.tsv"), "w") as nh:
        cols = ["address", "name", "library", "file", "confidence", "hits", "sessions", "callers", "modes",
                "r4", "r5", "r6", "r7", "fr4", "fr5", "fr6", "fr7", "returns_seen", "r0", "fr0", "notes",
                "prototype"]
        tsv.write("\t".join(cols) + "\n")
        nh.write("address\tname\tlibrary\tfile\tconfidence\twhy\n")
        # the other direction: which routines each routine was seen calling
        # (from the callees' own records; a sample like everything else)
        made = collections.defaultdict(collections.Counter)
        for a2, o2 in obs.items():
            for who, c in o2.callers.items():
                made[who.split("+")[0]]["%s (at +%s)" % (names.fname.get(a2, "%08x" % a2), who.split("+")[1] if "+" in who else "?")] += c
        for lib in sorted(libs):
            n_fn = n_hit = n_ret = 0
            md = ["# %s: what the running game showed" % lib, "",
                  "From tools/observe_summary.py. Per function: the prototype and description the repo has, then "
                  "the calls seen. `x3` is how often a value was seen. Registers the prototype does not use are "
                  "listed as *other registers*.", ""]
            never = []
            for path in sorted(libs[lib], key=lambda p: libs[lib][p][0]):
                md += ["## %s" % path, ""]
                for a in libs[lib][path]:
                    n_fn += 1
                    name = names.fname[a]
                    o = obs.get(a)
                    conf = names.conf.get(a, "")
                    proto = names.proto.get(a, "")
                    if o is None or not o.hits:
                        why = (o.skip if o and o.skip else "")
                        never.append((a, name, why))
                        nh.write("%08x\t%s\t%s\t%s\t%s\t%s\n" % (a, name, lib, path, conf, why))
                        tsv.write("\t".join(["%08x" % a, name, lib, path, conf, "0", "0"] + [""] * 13 +
                                            [why, proto]) + "\n")
                        continue
                    n_hit += 1
                    n_ret += bool(o.rets)
                    ps, rkind = params(proto)
                    notes = ["%s x%d" % kv for kv in o.notes.items() if not kv[0].startswith("entered by")] + \
                            ["no return seen: %s x%d" % kv for kv in o.noret.items()]
                    if o.skip:
                        notes.append("later left alone: " + o.skip)
                    row = ["%08x" % a, name, lib, path, conf, str(o.hits), str(len(o.runs)),
                           "; ".join(top(o.callers, 99)), "; ".join("%s x%d" % (mname(m), c) for m, c in
                                                                    o.modes.most_common())]
                    row += ["; ".join(top(o.regs[i], topn, o.peeks, i)) for i in range(4)]
                    row += ["; ".join(top(o.fregs[i], topn)) for i in range(4)]
                    row += [str(o.rets), "; ".join(top(o.r0, topn)), "; ".join(top(o.fr0, topn)),
                            "; ".join(notes), proto]
                    tsv.write("\t".join(x.replace("\t", " ") for x in row) + "\n")
                    # ---- the readable block ----
                    md += ["### %s `%08x` (%s)" % (name, a, conf or "no symbol row"), ""]
                    if proto:
                        md += ["`%s`" % proto, ""]
                    if names.desc.get(a):
                        md += ["> " + names.desc[a], ""]
                    md.append("- **Seen**: %d hits in %d session%s; %d returns captured" % (
                        o.hits, len(o.runs), "" if len(o.runs) == 1 else "s", o.rets))
                    md.append("- **Callers** (%d): %s" % (len(o.callers), ", ".join(top(o.callers, 12))))
                    if made.get(name):
                        md.append("- **Seen calling** (%d): %s" % (len(made[name]), ", ".join(top(made[name], 16))))
                    md.append("- **Modes** (%d): %s" % (len(o.modes), ", ".join(
                        "%s x%d" % (mname(m), c) for m, c in o.modes.most_common())))
                    used = set()
                    lines = []
                    for pname, where in ps:
                        used.add(where)
                        if where.startswith("fr"):
                            vals = top(o.fregs[int(where[2:]) - 4], topn)
                        elif where.startswith("r"):
                            i = int(where[1:]) - 4
                            vals = top(o.regs[i], topn, o.peeks, i)
                        elif where.startswith("stack+") and int(where[6:]) < 16:
                            vals = top(o.stk[int(where[6:]) // 4], topn)
                        else:
                            vals = ["not logged"]
                        lines.append("  - `%s` (%s): %s" % (pname, where, ", ".join(vals)))
                    if lines:
                        md.append("- **Arguments**:")
                        md += lines
                    else:
                        md.append("- **Arguments**: the prototype has none")
                    other = []
                    for i in range(4):
                        if "r%d" % (4 + i) not in used:
                            other.append("r%d: %s" % (4 + i, ", ".join(top(o.regs[i], 4, o.peeks, i)))
                                         + (" (varies)" if varies(o.regs[i]) else ""))
                    for i in range(4):
                        if "fr%d" % (4 + i) not in used and varies(o.fregs[i]):
                            other.append("fr%d: %s (varies)" % (4 + i, ", ".join(top(o.fregs[i], 4))))
                    if other:
                        md.append("- *Other registers*: " + "; ".join(other))
                    if o.rets:
                        r = "r0: " + ", ".join("%s%s" % (t, " -> " + o.r0peek[t.rsplit(" x", 1)[0]]
                                                          if t.rsplit(" x", 1)[0] in o.r0peek else "")
                                                for t in top(o.r0, topn))
                        f = "fr0: " + ", ".join(top(o.fr0, topn))
                        if rkind == "void":
                            md.append("- **Returns**: declared void; left in %s%s" % (
                                r, " (varies)" if varies(o.r0) else ""))
                        elif rkind == "float":
                            md.append("- **Returns** (float): %s; %s" % (f, r))
                        else:
                            md.append("- **Returns**: %s%s" % (r, "; " + f if varies(o.fr0) else ""))
                    else:
                        md.append("- **Returns**: none captured")
                    picked, seen_who = [], set()
                    for c in sorted(o.calls, key=lambda c: c["ret"] is None):   # with a result first
                        if c["who"] not in seen_who or len(o.calls) <= samples:
                            seen_who.add(c["who"])
                            picked.append(c)
                    for c in o.calls:
                        if len(picked) < samples and c not in picked:
                            picked.append(c)
                    if picked:
                        md.append("- **Single calls**:")
                    for c in picked[:samples]:
                        args = []
                        for pname, where in ps:
                            if where.startswith("fr"):
                                args.append("%s=%s" % (pname, c["fr"][int(where[2:]) - 4]))
                            elif where.startswith("r"):
                                args.append("%s=%s" % (pname, c["r"][int(where[1:]) - 4]))
                            elif where.startswith("stack+") and int(where[6:]) // 4 < len(c["stk"]):
                                args.append("%s=%s" % (pname, c["stk"][int(where[6:]) // 4]))
                        if not ps:
                            args.append("r4-r7 = " + ", ".join(c["r"]))
                        if c["ret"] is None:
                            res = "return not captured"
                        elif rkind == "float":
                            res = "fr0 = %s" % c["ret"][1]
                        else:
                            res = "r0 = %s%s" % (c["ret"][0], " (declared void)" if rkind == "void" else "")
                            if c["ret"][2]:
                                res += ", %d frames later" % c["ret"][2]
                        md.append("  - from %s in %s, frame %d: (%s) -> %s" % (
                            c["who"], mname(c["mode"]), c["fc"], ", ".join(args), res))
                    if notes:
                        md.append("- **Notes**: " + "; ".join(notes))
                    md.append("")
            if never:
                md += ["## Never hit (%d)" % len(never), ""]
                md += ["- %s `%08x`%s" % (n, a, " (%s)" % w if w else "") for a, n, w in never]
                md.append("")
            cover[lib] = (n_fn, n_hit, n_ret)
            open(os.path.join(out, "digest", lib + ".md"), "w").write("\n".join(md))
    tot = [sum(c[i] for c in cover.values()) for i in range(3)]
    lines = ["# Live observation coverage", "",
             "%d of %d functions were seen called (%.1f%%); %d with at least one captured return; %d never hit."
             % (tot[1], tot[0], 100.0 * tot[1] / tot[0], tot[2], tot[0] - tot[1]), "",
             "| library | functions | seen | with a return | never hit |", "|---|---:|---:|---:|---:|"]
    for lib in sorted(cover, key=lambda l: -cover[l][0]):
        n, h, r = cover[lib]
        lines.append("| [%s](%s.md) | %d | %d | %d | %d |" % (lib, lib, n, h, r, n - h))
    seen, missing = modes_report(mode_names, stats.entered)
    lines += ["", "## Game modes", "", "Watched in: " + (", ".join(seen) or "none") + ".", "",
              "No session was in: " + (", ".join(missing) or "none") + "."]
    lines += ["", "Log totals: " + ", ".join("%s %d" % kv for kv in sorted(stats.items())), ""]
    open(os.path.join(out, "digest", "index.md"), "w").write("\n".join(lines))
    return cover, tot


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("logs", nargs="*")
    ap.add_argument("--out", default=os.path.join(ROOT, "work", "observe"))
    ap.add_argument("--top", type=int, default=6, help="values listed per register (default 6)")
    ap.add_argument("--samples", type=int, default=4, help="single calls listed per function (default 4)")
    a = ap.parse_args()
    logs = a.logs or sorted(glob.glob(os.path.join(ROOT, "work", "observe", "*.jsonl")))
    if not logs:
        sys.exit("no logs: nothing under work/observe/*.jsonl (run tools/flycast_observe.py first)")
    names = Names()
    obs, mode_names, stats = load(logs, names)
    cover, tot = write(names, obs, mode_names, stats, a.out, a.top, a.samples)
    print("%d logs, %d sessions, %d hits, %d returns" % (len(logs), stats["sessions"], stats["hits"],
                                                         stats["returns"]))
    print("%-16s %9s %6s %8s %9s" % ("library", "functions", "seen", "returns", "never hit"))
    for lib in sorted(cover, key=lambda l: -cover[l][0]):
        n, h, r = cover[lib]
        print("%-16s %9d %6d %8d %9d" % (lib, n, h, r, n - h))
    print("%-16s %9d %6d %8d %9d" % ("total", tot[0], tot[1], tot[2], tot[0] - tot[1]))
    seen, missing = modes_report(mode_names, stats.entered)
    print("game modes watched (%d): %s" % (len(seen), ", ".join(seen)))
    print("game modes no session was in (%d): %s" % (len(missing), ", ".join(missing)))
    rel = os.path.relpath(a.out, os.getcwd())
    print("wrote %s/summary.tsv, %s/never_hit.tsv, %s/digest/*.md" % (rel, rel, rel))


if __name__ == "__main__":
    main()
