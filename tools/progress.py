#!/usr/bin/env python3
"""Build progress/index.html: how much of VF4 Final Tuned is reconstructed.

Everything is measured from src/, the deliverable itself: each function
header there records its address, size, how its source file was assigned
and how sure the name is; src/include/ holds the enums and named globals.

  update [label]    measure the working tree, append a history point if
                    anything changed, regenerate the page (run.sh calls
                    this after every step)
  backfill          rebuild progress/history.tsv from every git commit
                    that has src/, then regenerate the page

The page is one self-contained file (inline SVG + a little JS), so it can
be opened locally or sent to someone as is.
"""
import datetime
import html
import json
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "progress")
HIST = os.path.join(OUT, "history.tsv")
TEXT_BYTES = 0x0C15DA00 - 0x0C020000  # .text, measured by ghidra/ReportSections.java


def code_bytes():
    """.text minus alignment padding and data (ghidra/text_layout.tsv, from
    TextLayout.java). Returns (code bytes, layout dict); falls back to all
    of .text if the layout has not been measured."""
    p = os.path.join(ROOT, "ghidra", "text_layout.tsv")
    if not os.path.exists(p):
        return TEXT_BYTES, None
    head, vals = [l.rstrip("\n").split("\t") for l in open(p)][:2]
    lay = dict(zip(head, map(int, vals)))
    return TEXT_BYTES - lay["padding"] - lay["data_referenced"] - lay["data_other"], lay


CODE_BYTES, LAYOUT = code_bytes()

HEADER = re.compile(r"^/\* 0x([0-9a-f]{8})  size 0x([0-9a-f]+)  (file by assert string|file by link order|file by match with source|file by callers|file by strings|file unknown)\n"
                    r" \* confidence: (\w+)\n(.*?)^ \*/", re.M | re.S)
# Decompiler placeholders, except in_xf*/in_xd*: those are the FPU back
# bank (XMTRX), a matrix that one routine loads and later ftrv code reads,
# so they are genuine inputs, not missing information.
DEF_NAME = re.compile(r"\n[^\n(]*?\b(\w+)\s*\(")  # the definition after a header
PLACEHOLDER_VAR = re.compile(r"(?<![.>])\b(?:in|unaff|extraout)_(?!x[fd]\d)[A-Za-z0-9_]+")
# what keeps matched C from reading like source: the decompiler's names for
# parameters, locals, stack slots and globals, and its undefined types
LEFTOVER = re.compile(r"\b(?:param_\d+|[a-z]{1,4}Var\d+|local_[0-9a-f]+|undefined\d?|DAT_[0-9a-f]{8})\b")
BODY_END = re.compile(r"^\}", re.M)

FIELDS = ["time", "label", "commit", "functions", "fn_bytes", "named_bytes", "inferred_bytes",
          "placeholder_bytes", "assert_bytes", "link_bytes", "named_fn", "inferred_fn",
          "globals", "enums", "asserts", "failed", "placeholder_vars", "fpu_warnings", "c_lines",
          "matched_bytes", "matched_fn", "caller_bytes", "readable_fn", "observed_bytes", "observed_fn",
          "exercised_bytes", "exercised_fn"]


def extents(src):
    """address -> bytes the source reproduces there (the function with its
    literal pool and tables), from tools/match/match.py's whole-tree run."""
    p = os.path.join(ROOT, "ghidra", "matched_extents_test.tsv" if os.path.basename(src) == "src_test" else "matched_extents.tsv")
    if not os.path.exists(p):
        return {}
    return {f[0]: int(f[1]) for f in (l.split("\t") for l in open(p).read().split("\n")[1:]) if len(f) >= 2}


def asm_readable():
    """Hand-written assembly routines whose source/asm/ file reads like
    source (tools/match/asm_readable.py says what that takes)."""
    sys.path.insert(0, os.path.join(ROOT, "tools", "match"))
    try:
        import asm_readable as a
        return a.readable_names()
    except Exception:
        return set()
    finally:
        sys.path.pop(0)


def measure(src):
    m = {k: 0 for k in FIELDS[3:]}
    libs = {}
    ext = extents(src)
    asm_ok = asm_readable()
    for dirpath, _, files in os.walk(src):
        for name in files:
            if not name.endswith(".c"):
                continue
            path = os.path.join(dirpath, name)
            rel = os.path.relpath(path, src)
            text = open(path, encoding="utf-8", errors="replace").read()
            # Sega's paths have a src/ level; public libraries (zlib/) are their own top directory
            lib = rel.split("/src/")[0] if "/src/" in rel else (
                rel.split("/")[0] if "/" in rel and not rel.startswith("_unmapped/") else "(source file unknown)")
            L = libs.setdefault(lib, {"files": set(), "functions": 0, "bytes": 0, "named": 0, "inferred": 0, "observed": 0, "exercised": 0,
                                      "matched": 0, "tree": {}})
            cells = L["tree"].setdefault(rel, [])
            if lib != "(source file unknown)":
                L["files"].add(rel)
            m["c_lines"] += text.count("\n")
            m["failed"] += text.count("/* decompile failed")
            m["asserts"] += len(re.findall(r"(?m)^ \* assert ", text))
            m["fpu_warnings"] += text.count("WARNING (fpu model)")
            m["placeholder_vars"] += len(PLACEHOLDER_VAR.findall(text))
            for h in HEADER.finditer(text):
                addr, size, how, conf, rest = h.groups()
                # a matched function counts with its literal pool and tables
                b = ext.get(addr, int(size, 16))
                matched = bool(re.search(r"(?m)^ \* matched: ", rest))
                if matched:
                    # hand-written C from source/ that compiles to the game's bytes
                    m["matched_bytes"] += b
                    m["matched_fn"] += 1
                    L["matched"] += 1
                    end = BODY_END.search(text, h.end())
                    if "hand-written assembly" in rest:
                        # no C to read: its source is the assembly in source/asm/
                        dn = DEF_NAME.match(text, h.end())
                        if dn and dn.group(1) in asm_ok:
                            m["readable_fn"] += 1
                    elif end and not LEFTOVER.search(text, h.end(), end.start()):
                        m["readable_fn"] += 1
                d = DEF_NAME.match(text, h.end())
                # one treemap cell: name (None for FUN_<address>), address, size, state
                state = 2 if matched else 1 if conf in ("named", "observed", "exercised", "inferred") else 0
                fn = d.group(1) if d else None
                cells.append([None if fn in (None, "FUN_" + addr) else fn, int(addr, 16), b, state])
                m["functions"] += 1
                m["fn_bytes"] += b
                L["functions"] += 1
                L["bytes"] += b
                if how == "file by assert string":
                    m["assert_bytes"] += b
                elif how == "file by link order":
                    m["link_bytes"] += b
                elif how in ("file by callers", "file by strings", "file by match with source"):
                    # placed by what the code uses (callers, string literals), or by
                    # the source/ file whose C compiles to it (that file's place
                    # came from the same evidence before the function matched)
                    m["caller_bytes"] += b
                if conf == "named":
                    m["named_bytes"] += b
                    m["named_fn"] += 1
                    L["named"] += 1
                elif conf == "observed":
                    # named from analysis, and a logged run of the game confirmed
                    # what its description says (tools/observe_apply.py)
                    m["observed_bytes"] += b
                    m["observed_fn"] += 1
                    L["observed"] += 1
                elif conf == "exercised":
                    # named from analysis; the game never runs it, and calling it in
                    # the harness with chosen inputs bore out its description
                    m["exercised_bytes"] += b
                    m["exercised_fn"] += 1
                    L["exercised"] += 1
                elif conf == "inferred":
                    m["inferred_bytes"] += b
                    m["inferred_fn"] += 1
                    L["inferred"] += 1
                else:
                    m["placeholder_bytes"] += b
    inc = os.path.join(src, "include")
    g = os.path.join(inc, "vf4ft_globals.h")
    e = os.path.join(inc, "vf4ft_enums.h")
    if os.path.exists(g):
        m["globals"] = len(re.findall(r"(?m)^extern ", open(g).read()))
    if os.path.exists(e):
        m["enums"] = len(re.findall(r"(?m)^typedef enum ", open(e).read()))
    for L in libs.values():
        L["files"] = len(L["files"])
        L["matched_bytes"] = sum(c[2] for cells in L["tree"].values() for c in cells if c[3] == 2)
    return m, libs


def git(*args):
    return subprocess.run(["git", "-C", ROOT, *args], capture_output=True, text=True).stdout.strip()


def read_history():
    if not os.path.exists(HIST):
        return []
    rows = []
    with open(HIST) as f:
        head = f.readline().rstrip("\n").split("\t")
        for line in f:
            v = line.rstrip("\n").split("\t")
            row = {k: 0 for k in FIELDS[3:]}  # fields added later start at 0
            row.update({k: (v[i] if k in ("time", "label", "commit") else int(v[i])) for i, k in enumerate(head)})
            rows.append(row)
    return rows


def write_history(rows):
    os.makedirs(OUT, exist_ok=True)
    with open(HIST, "w") as f:
        f.write("\t".join(FIELDS) + "\n")
        for r in rows:
            f.write("\t".join(str(r[k]) for k in FIELDS) + "\n")


def next_steps():
    p = os.path.join(ROOT, "docs", "progress.md")
    if not os.path.exists(p):
        return []
    text = open(p).read()
    part = text.split("## Next", 1)[-1] if "## Next" in text else ""
    steps = []
    for item in re.findall(r"(?ms)^\d+\.\s+(.*?)(?=^\d+\.\s|\Z)", part):
        one = " ".join(item.split())
        title = re.match(r"\*\*(.+?)\*\*\s*(.*)", one)
        steps.append((title.group(1), title.group(2)) if title else (one, ""))
    return steps


def pct(a, b):
    return 100.0 * a / b if b else 0.0


# ---------------------------------------------------------------- rendering

def svg_naming_bar(m):
    """Stacked bar of .text bytes by naming status (ordinal blue ramp)."""
    parts = [
        ("named", "Named from the binary or matched source", m["named_bytes"], "var(--ord-4)"),
        ("observed", "Named from analysis, confirmed in the running game", m["observed_bytes"], "var(--ord-3)"),
        ("exercised", "Named from analysis, borne out by calling the routine in a harness",
         m["exercised_bytes"], "var(--ord-25)"),
        ("inferred", "Named from analysis", m["inferred_bytes"], "var(--ord-2)"),
        ("placeholder", "Placeholder name (FUN_…)", m["placeholder_bytes"], "var(--ord-1)"),
        ("none", "Not yet in any function", max(0, CODE_BYTES - m["fn_bytes"]), "var(--rest)"),
    ]
    W, H, gap = 1000, 28, 2
    x, rects = 0.0, []
    for key, label, b, color in parts:
        w = W * b / CODE_BYTES
        if w <= 0:
            continue
        draw = max(w - gap, 1.0)
        tip = f"{label}: {b:,} bytes ({pct(b, CODE_BYTES):.2f}% of code)"
        rects.append(f'<rect class="seg" x="{x:.2f}" y="0" width="{draw:.2f}" height="{H}" rx="4" '
                     f'fill="{color}" data-tip="{html.escape(tip)}"><title>{html.escape(tip)}</title></rect>')
        x += w
    legend = "".join(
        f'<li><span class="sw" style="background:{c}"></span>{html.escape(l)} '
        f'<span class="num">{pct(b, CODE_BYTES):.2f}%</span></li>'
        for _, l, b, c in parts)
    return (f'<svg class="bar" viewBox="0 0 {W} {H}" preserveAspectRatio="none" role="img" '
            f'aria-label="Code by naming status">{"".join(rects)}</svg><ul class="legend">{legend}</ul>')


def nice_max(v):
    if v <= 0:
        return 1
    for mag in [10 ** k for k in range(-2, 9)]:  # smallest 1/2/2.5/5 x 10^k that holds v
        for step in (1, 2, 2.5, 5):
            if step * mag >= v:
                return step * mag
    return v


def svg_lines(rows, series, ymax=None, unit="%", aria=""):
    """Line chart over history points. series: [(label, key_fn, css_var)]."""
    W, H, L, R, T, B = 1000, 260, 64, 24, 16, 52
    n = len(rows)
    vals = [[fn(r) for r in rows] for _, fn, _ in series]
    top = ymax or nice_max(max([max(v) for v in vals] + [0]) * 1.1)

    def X(i):
        return L + (W - L - R) * (i / (n - 1) if n > 1 else 0.5)

    def Y(v):
        return T + (H - T - B) * (1 - v / top)

    out = []
    for k in range(5):
        v = top * k / 4
        y = Y(v)
        lab = f"{v:,.0f}{unit}" if top >= 10 else f"{v:.2f}{unit}"
        out.append(f'<line class="grid" x1="{L}" x2="{W - R}" y1="{y:.1f}" y2="{y:.1f}"/>'
                   f'<text class="tick" x="{L - 8}" y="{y + 4:.1f}" text-anchor="end">{lab}</text>')
    # x labels: the first, the last, and evenly spaced ones between, none
    # closer than 90 units to the next (two labels at the right end collided)
    want = [i for i in range(n) if n <= 12 or i in (0, n - 1) or i % max(1, n // 8) == 0]
    ticks = []
    for i in reversed(want):
        if not ticks or X(ticks[-1]) - X(i) >= 90 or i == 0:
            if i == 0 and ticks and X(ticks[-1]) - X(0) < 90 and ticks[-1] != n - 1:
                ticks.pop()
            ticks.append(i)
    for i in sorted(set(ticks)):
        r = rows[i]
        anchor = "end" if i == n - 1 and n > 1 else "start" if i == 0 and n > 1 else "middle"
        out.append(f'<text class="tick" x="{X(i):.1f}" y="{H - B + 18}" text-anchor="{anchor}">'
                   f'{html.escape(r["label"][:18])}</text>'
                   f'<text class="tick sub" x="{X(i):.1f}" y="{H - B + 34}" text-anchor="{anchor}">'
                   f'{html.escape(r["time"][5:10])}</text>')
    for (label, _, color), v in zip(series, vals):
        pts = " ".join(f"{X(i):.1f},{Y(x):.1f}" for i, x in enumerate(v))
        out.append(f'<polyline class="line" points="{pts}" style="stroke:{color}"/>')
        for i, x in enumerate(v):
            out.append(f'<circle class="dot" cx="{X(i):.1f}" cy="{Y(x):.1f}" r="4" style="fill:{color}"/>')
    # direct labels above each line's last point, pushed apart so they never overlap
    ends = sorted(((Y(v[-1]) - 10, label) for (label, _, _), v in zip(series, vals)), reverse=True)
    placed = []
    for y, label in ends:
        if placed and y > placed[-1][0] - 15:
            y = placed[-1][0] - 15
        placed.append((y, label))
    for y, label in placed:
        out.append(f'<text class="direct" x="{X(n - 1) - 6:.1f}" y="{y:.1f}" text-anchor="end">'
                   f'{html.escape(label)}</text>')
    data = json.dumps({"x": [X(i) for i in range(n)],
                       "rows": [f'{r["label"]} · {r["time"][:16].replace("T", " ")}' for r in rows],
                       "series": [{"label": l, "color": c, "values": v, "unit": unit}
                                  for (l, _, c), v in zip(series, vals)]})
    hit = f'<rect class="hit" x="{L}" y="{T}" width="{W - L - R}" height="{H - T - B}" fill="transparent"/>'
    return (f'<svg class="chart" viewBox="0 0 {W} {H}" role="img" aria-label="{html.escape(aria)}" '
            f"data-lines='{html.escape(data)}'>{''.join(out)}"
            f'<line class="xhair" x1="0" x2="0" y1="{T}" y2="{H - B}" visibility="hidden"/>{hit}</svg>')


def badges(m):
    """Shield-style summary row: what each headline figure is, at a glance."""
    items = [
        ("functions", f"{m['functions']:,}"),
        ("placed", f"{pct(m['assert_bytes'] + m['link_bytes'] + m['caller_bytes'], CODE_BYTES):.1f}%"),
        ("named", f"{pct(m['named_bytes'] + m['observed_bytes'] + m['exercised_bytes'] + m['inferred_bytes'], CODE_BYTES):.2f}%"),
        ("matching C", f"{pct(m['matched_bytes'], CODE_BYTES):.2f}%"),
        ("globals", f"{m['globals']:,}"),
        ("enums", f"{m['enums']:,}"),
    ]
    return "".join(f'<span class="badge"><span class="bk">{html.escape(k)}</span>'
                   f'<span class="bv">{v}</span></span>' for k, v in items)


STATES = [("Placeholder name", "var(--tm-0)"), ("Named, not yet matched", "var(--s2)"), ("Rebuilt from matching C", "var(--s3)")]


def treemap_data(libs):
    """Libraries -> files -> functions for the treemap, biggest first (the
    page lays it out at whatever width it is shown)."""
    out = []
    for lib, L in sorted(libs.items(), key=lambda kv: -kv[1]["bytes"]):
        files = [{"n": rel, "f": sorted(cells, key=lambda c: -c[2])}
                 for rel, cells in L["tree"].items() if cells]
        files.sort(key=lambda f: -sum(c[2] for c in f["f"]))
        if files:
            out.append({"n": lib, "files": files})
    return json.dumps(out, separators=(",", ":")).replace("</", "<\\/")


def treemap_legend(libs):
    n, b = [0, 0, 0], [0, 0, 0]
    for L in libs.values():
        for cells in L["tree"].values():
            for c in cells:
                n[c[3]] += 1
                b[c[3]] += c[2]
    total = sum(b) or 1
    return "".join(
        f'<li><span class="sw" style="background:{color}"></span>{html.escape(label)} '
        f'<span class="num">{n[i]:,} functions · {pct(b[i], total):.1f}% of function bytes</span></li>'
        for i, (label, color) in reversed(list(enumerate(STATES))))


def md(text):
    """Inline Markdown from docs/progress.md: `code` and **bold**."""
    out = html.escape(text)
    out = re.sub(r"`([^`]+)`", r"<code>\1</code>", out)
    return re.sub(r"\*\*(.+?)\*\*", r"<b>\1</b>", out)


def tile(label, value, sub="", meter=None):
    bar = ""
    if meter is not None:
        bar = (f'<div class="meter" role="meter" aria-valuenow="{meter:.1f}" aria-valuemin="0" aria-valuemax="100">'
               f'<span style="width:{min(100, meter):.2f}%"></span></div>')
    return (f'<div class="tile"><div class="tl">{html.escape(label)}</div><div class="tv">{value}</div>'
            f'{bar}<div class="ts">{sub}</div></div>')


def big(n):
    """Token counts as 10.2 billion / 195 million."""
    return f"{n / 1e9:.1f} billion" if n >= 1e9 else f"{n / 1e6:.0f} million"


def effort_tiles(m):
    """The Effort section's tiles from progress/effort.json (tools/effort.py)."""
    p = os.path.join(OUT, "effort.json")
    if not os.path.exists(p):
        return "<p class='note'>Not measured yet.</p>", ""
    e = json.load(open(p))
    t = e["tokens"]
    days = e["elapsed_days"]
    tiles = "".join([
        tile("Calendar time", f"{days:g} days", f"from {e['first'][:10]} to {e['last'][:10]}"),
        tile("Active session time", f"{e['session_hours']:,.0f} h",
             "the main session, pauses over 15 minutes left out"),
        tile("Agent hours", f"{e['agent_hours']:,.0f} h",
             f"{e['agents']:,} subagents and workflow agents, often several at once"),
        tile("All Claude hours", f"{e['session_hours'] + e['agent_hours']:,.0f} h",
             "the main session and every agent added up"),
        tile("Tokens processed", big(e["tokens_total"]),
             f"{big(t['cache_read_input_tokens'])} of them cached context re-read on each step"),
        tile("New tokens", big(e["tokens_new"]),
             f"excluding cache reads; {big(t['output_tokens'])} written as output"),
        tile("Cost at API prices", f"${e['api_cost_usd']:,}",
             "estimate: the same tokens at Anthropic's API list prices, per model and cache type"),
        tile("Same work by hand", f"{m['matched_fn'] / 2000:.0f}–{2 * m['matched_fn'] / 2000:.0f} years",
             f"of full-time work (2,000 h a year): {m['matched_fn'] / 1000:.1f}k–{2 * m['matched_fn'] / 1000:.1f}k "
             "hours at 1 to 2 expert hours per matched function, tooling and naming included"),
    ])
    return tiles, html.escape(e["last"][:16].replace("T", " "))


def render(rows, libs):
    m = rows[-1]
    effort, effort_when = effort_tiles(m)
    in_fn = pct(m["fn_bytes"], CODE_BYTES)
    placed = pct(m["assert_bytes"] + m["link_bytes"] + m["caller_bytes"], CODE_BYTES)
    named = pct(m["named_bytes"] + m["observed_bytes"] + m["inferred_bytes"], CODE_BYTES)
    confirmed = pct(m["named_bytes"] + m["observed_bytes"], CODE_BYTES)
    matched = pct(m["matched_bytes"], CODE_BYTES)
    tiles_main = "".join([
        tile("Code found as functions", f"{in_fn:.1f}%", f"{m['functions']:,} functions in .text", in_fn),
        tile("Code placed in an original source file", f"{placed:.1f}%",
             f"from assert paths, link order, callers, strings and known library sources", placed),
        tile("Code with a real function name", f"{named:.2f}%",
             f"{m['named_fn'] + m['observed_fn'] + m['exercised_fn'] + m['inferred_fn']:,} of {m['functions']:,} functions", named),
        tile("Code rebuilt from matching C", f"{matched:.3f}%",
             f"{m['matched_fn']:,} functions in source/ compile to the game's bytes", matched),
        tile("Matching source that reads like source", f"{pct(m['readable_fn'], m['matched_fn']):.1f}%",
             f"{m['readable_fn']:,} of {m['matched_fn']:,} matched functions: C with no decompiler "
             f"placeholder names or types left, hand-written assembly with every routine "
             f"described and its code commented", pct(m['readable_fn'], m['matched_fn'])),
        tile("Code whose name is confirmed", f"{confirmed:.1f}%",
             f"{m['named_fn']:,} functions named by the game's own text or a public library's source, "
             f"{m['observed_fn']:,} whose description a logged run of the game bore out. Apart from these, "
             f"{m['exercised_fn']:,} routines the game never runs were called in a harness with chosen inputs "
             f"and did what their descriptions say; the other "
             f"{m['inferred_fn']:,} are named from reading the code alone", confirmed),
    ])
    tiles_more = "".join([
        tile("Named globals", f"{m['globals']:,}", "from Sega's debug-menu labels"),
        tile("Enums recovered", f"{m['enums']:,}", "from debug name tables"),
        tile("Asserts annotated", f"{m['asserts']:,}", "file, line and expression"),
        tile("Decompile failures", f"{m['failed']:,}", "lower is better"),
        tile("Placeholder variables", f"{m['placeholder_vars']:,}", "in_/unaff_/extraout_ (not the XMTRX bank); lower is better"),
        tile("Lines of C", f"{m['c_lines']:,}", "in src/"),
    ])
    timeline = svg_lines(rows, [
        ("Placed in a source file", lambda r: pct(r["assert_bytes"] + r["link_bytes"] + r["caller_bytes"], CODE_BYTES), "var(--s1)"),
        ("Real function name", lambda r: pct(r["named_bytes"] + r["observed_bytes"] + r["exercised_bytes"] + r["inferred_bytes"], CODE_BYTES), "var(--s2)"),
        ("Matching C", lambda r: pct(r["matched_bytes"], CODE_BYTES), "var(--s3)"),
    ], aria="Percent of code placed in a source file, named, and rebuilt from matching C, over time")
    noise = svg_lines(rows, [("Placeholder variables", lambda r: r["placeholder_vars"], "var(--s1)")],
                      unit="", aria="Placeholder variables in the decompiled code, over time")
    steps = "".join(f"<li><b>{md(t)}</b> {md(d)}</li>" for t, d in next_steps())

    lib_rows = []
    for lib, L in sorted(libs.items(), key=lambda kv: (kv[0].startswith("("), kv[0])):
        lib_rows.append(
            f"<tr><td>{html.escape(lib)}</td><td class='n'>{L['files'] or '–'}</td>"
            f"<td class='n'>{L['functions']:,}</td><td class='n'>{L['named'] + L['observed'] + L['exercised'] + L['inferred']:,}</td>"
            f"<td class='n'>{pct(L['named'] + L['observed'] + L['exercised'] + L['inferred'], L['functions']):.1f}%</td>"
            f"<td class='n'>{L['named'] + L['observed']:,}</td><td class='n'>{L['exercised']:,}</td>"
            f"<td class='n'>{L['matched']:,}</td><td class='n'>{pct(L['matched_bytes'], L['bytes']):.1f}%</td>"
            f"<td class='n'>{L['bytes']:,}</td></tr>")
    hist_rows = "".join(
        f"<tr><td>{html.escape(r['time'][:16].replace('T', ' '))}</td><td>{html.escape(r['label'])}</td>"
        f"<td><code>{html.escape(r['commit'])}</code></td><td class='n'>{r['functions']:,}</td>"
        f"<td class='n'>{pct(r['assert_bytes'] + r['link_bytes'] + r['caller_bytes'], CODE_BYTES):.1f}%</td>"
        f"<td class='n'>{pct(r['named_bytes'] + r['observed_bytes'] + r['exercised_bytes'] + r['inferred_bytes'], CODE_BYTES):.2f}%</td>"
        f"<td class='n'>{pct(r['matched_bytes'], CODE_BYTES):.2f}%</td>"
        f"<td class='n'>{r['globals']:,}</td><td class='n'>{r['enums']:,}</td>"
        f"<td class='n'>{r['placeholder_vars']:,}</td><td class='n'>{r['failed']:,}</td></tr>"
        for r in reversed(rows))
    updated = m["time"][:16].replace("T", " ")
    return PAGE.format(updated=html.escape(updated), commit=html.escape(m["commit"]), label=html.escape(m["label"]),
                       tiles_main=tiles_main, tiles_more=tiles_more, naming=svg_naming_bar(m),
                       timeline=timeline, noise=noise, steps=steps or "<li>See docs/progress.md</li>",
                       lib_rows="".join(lib_rows), hist_rows=hist_rows, text_bytes=f"{CODE_BYTES:,}",
                       basis=basis(), badges=badges(m), tm_data=treemap_data(libs),
                       tm_legend=treemap_legend(libs), effort=effort, effort_when=effort_when)


PAGE = """<!doctype html>
<html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>VF4FT Decomp Progress</title>
<style>
:root {{
  color-scheme: light;
  --page: #f9f9f7; --surface: #fcfcfb; --ink: #0b0b0b; --ink2: #52514e; --muted: #898781;
  --grid: #e1e0d9; --axis: #c3c2b7; --ring: rgba(11,11,11,0.10);
  --s1: #2a78d6; --s2: #eb6834; --s3: #1baf7a; --tm-0: #d6d5cd;
  --ord-4: #0a2a55; --ord-3: #104281; --ord-25: #1c5cab; --ord-2: #2a78d6; --ord-1: #86b6ef; --rest: #e1e0d9;
}}
@media (prefers-color-scheme: dark) {{
  :root:where(:not([data-theme="light"])) {{
    color-scheme: dark;
    --page: #0d0d0d; --surface: #1a1a19; --ink: #f0efec; --ink2: #c3c2b7; --muted: #898781;
    --grid: #2c2c2a; --axis: #383835; --ring: rgba(255,255,255,0.10);
    --s1: #3987e5; --s2: #d95926; --s3: #199e70; --tm-0: #3a3a37;
    --ord-4: #c6ddf8; --ord-3: #86b6ef; --ord-25: #609fea; --ord-2: #3987e5; --ord-1: #184f95; --rest: #2c2c2a;
  }}
}}
:root[data-theme="dark"] {{
  color-scheme: dark;
  --page: #0d0d0d; --surface: #1a1a19; --ink: #f0efec; --ink2: #c3c2b7; --muted: #898781;
  --grid: #2c2c2a; --axis: #383835; --ring: rgba(255,255,255,0.10);
  --s1: #3987e5; --s2: #d95926; --s3: #199e70; --tm-0: #3a3a37;
  --ord-4: #c6ddf8; --ord-3: #86b6ef; --ord-25: #609fea; --ord-2: #3987e5; --ord-1: #184f95; --rest: #2c2c2a;
}}
* {{ box-sizing: border-box; }}
body {{ margin: 0; background: var(--page); color: var(--ink);
  font: 15px/1.5 system-ui, -apple-system, "Segoe UI", sans-serif; }}
main {{ max-width: 1080px; margin: 0 auto; padding: 28px 16px 56px; }}
h1 {{ font-size: 26px; margin: 0 0 4px; letter-spacing: -0.01em; }}
h2 {{ font-size: 17px; margin: 0 0 4px; }}
.lede, .note {{ color: var(--ink2); margin: 0; }}
.note {{ font-size: 13px; margin-top: 8px; }}
.meta {{ color: var(--muted); font-size: 13px; margin: 6px 0 24px; }}
section {{ background: var(--surface); border-radius: 12px; box-shadow: 0 0 0 1px var(--ring);
  padding: 18px 20px; margin: 0 0 16px; }}
.tiles {{ display: grid; gap: 12px; grid-template-columns: repeat(auto-fit, minmax(210px, 1fr)); margin: 0 0 16px; }}
.tile {{ background: var(--surface); border-radius: 12px; box-shadow: 0 0 0 1px var(--ring); padding: 14px 16px; }}
.tl {{ color: var(--ink2); font-size: 13px; }}
.tv {{ font-size: 30px; font-weight: 650; margin: 2px 0 6px; }}
.ts {{ color: var(--muted); font-size: 12.5px; }}
.meter {{ height: 8px; border-radius: 4px; background: var(--grid); overflow: hidden; margin-bottom: 6px; }}
.meter span {{ display: block; height: 100%; background: var(--s1); border-radius: 4px; }}
svg.bar {{ width: 100%; height: 28px; display: block; margin: 12px 0 10px; }}
svg.chart {{ width: 100%; height: auto; display: block; margin-top: 8px; overflow: visible; }}
.legend {{ list-style: none; display: flex; flex-wrap: wrap; gap: 6px 18px; padding: 0; margin: 0;
  font-size: 13px; color: var(--ink2); }}
.legend .sw {{ display: inline-block; width: 12px; height: 12px; border-radius: 3px; margin-right: 6px;
  vertical-align: -1px; }}
.legend .num {{ color: var(--ink); font-variant-numeric: tabular-nums; margin-left: 4px; }}
.grid {{ stroke: var(--grid); stroke-width: 1; }}
.tick {{ fill: var(--muted); font-size: 12px; font-variant-numeric: tabular-nums; }}
.tick.sub {{ font-size: 11px; }}
.line {{ fill: none; stroke-width: 2; stroke-linejoin: round; }}
.dot {{ stroke: var(--surface); stroke-width: 2; }}
.direct {{ fill: var(--ink2); font-size: 12.5px; }}
.xhair {{ stroke: var(--axis); stroke-width: 1; }}
#tip {{ position: fixed; pointer-events: none; background: var(--surface); color: var(--ink);
  box-shadow: 0 0 0 1px var(--ring), 0 4px 14px rgba(0,0,0,.12); border-radius: 8px; padding: 8px 10px;
  font-size: 12.5px; line-height: 1.45; display: none; z-index: 10; max-width: 320px; }}
#tip .k {{ display: inline-block; width: 10px; height: 10px; border-radius: 2px; margin-right: 6px; }}
ol.steps {{ margin: 8px 0 0; padding-left: 22px; }} ol.steps li {{ margin: 6px 0; }}
.tablewrap {{ overflow-x: auto; }}
table {{ border-collapse: collapse; width: 100%; font-size: 13.5px; margin-top: 8px; }}
th, td {{ text-align: left; padding: 6px 10px; border-bottom: 1px solid var(--grid); white-space: nowrap; }}
th {{ color: var(--ink2); font-weight: 600; }}
td.n, th.n {{ text-align: right; font-variant-numeric: tabular-nums; }}
details summary {{ cursor: pointer; color: var(--ink2); }}
code {{ font-size: 12.5px; }}
.badges {{ display: flex; flex-wrap: wrap; gap: 8px; margin: 0 0 20px; }}
.badge {{ display: inline-flex; border-radius: 6px; overflow: hidden; font-size: 12.5px;
  box-shadow: 0 0 0 1px var(--ring); font-variant-numeric: tabular-nums; }}
.badge .bk {{ background: var(--grid); color: var(--ink2); padding: 3px 8px; }}
.badge .bv {{ background: var(--surface); color: var(--ink); padding: 3px 8px; font-weight: 600; }}
#treemap {{ width: 100%; margin: 12px 0 10px; }}
#treemap svg {{ display: block; width: 100%; }}
#treemap .lib {{ fill: var(--grid); }}
#treemap .libname {{ fill: var(--ink2); font-size: 12px; font-weight: 600; pointer-events: none; }}
#treemap .fn {{ shape-rendering: crispEdges; }}
#treemap .fn.s0 {{ fill: var(--tm-0); }} #treemap .fn.s1 {{ fill: var(--s2); }} #treemap .fn.s2 {{ fill: var(--s3); }}
#treemap .hl {{ fill: none; stroke: var(--ink); stroke-width: 2; pointer-events: none; }}
</style></head>
<body><main>
<h1>Virtua Fighter 4: Final Tuned — source reconstruction</h1>
<p class="lede">An annotated C reconstruction of the Naomi 2 game program (Ver. B), for preservation.
All figures are measured from the reconstructed source itself.</p>
<p class="meta">Updated {updated} · step “{label}” · commit <code>{commit}</code></p>
<p class="note" style="margin:-16px 0 16px">{basis}</p>
<div class="badges">{badges}</div>

<div class="tiles">{tiles_main}</div>

<section>
<h2>Game code by naming status</h2>
<p class="lede">Share of the {text_bytes} bytes of executable code. Names are the slow, hand-checked part;
everything starts as a placeholder.</p>
{naming}
</section>

<section>
<h2>Every function, by library and file</h2>
<p class="lede">Each cell is one function, sized by its bytes, grouped into its original source file and library.
Functions not yet placed in a source file are grouped by address. Hover a cell for its name, address and file;
the table further down has the same figures per library.</p>
<ul class="legend" style="margin-top:10px">{tm_legend}</ul>
<div id="treemap" role="img" aria-label="Treemap of every function by library and file, colored by status"></div>
<script type="application/json" id="tm-data">{tm_data}</script>
</section>

<section>
<h2>Progress over time</h2>
<p class="lede">Percent of executable code, one point per pipeline step that changed the output.</p>
{timeline}
<ul class="legend"><li><span class="sw" style="background:var(--s1)"></span>Placed in an original source file</li>
<li><span class="sw" style="background:var(--s2)"></span>Has a real function name</li>
<li><span class="sw" style="background:var(--s3)"></span>Rebuilt from C that compiles to the game's bytes</li></ul>
</section>

<div class="tiles">{tiles_more}</div>

<section>
<h2>Decompiler noise</h2>
<p class="lede">Placeholder variables (<code>in_</code>, <code>unaff_</code>, <code>extraout_</code>) the decompiler
invents when it lacks information. Each fix to the processor model or calling convention brings this down.
Inputs from the FPU's back register bank (<code>in_xf</code>, <code>in_xd</code>) are not counted: that bank is the
hardware matrix (XMTRX) one routine loads and later code reads, so those are real inputs.</p>
{noise}
</section>

<section>
<h2>Effort so far</h2>
<p class="lede">The reconstruction is done by Claude Code (Anthropic's coding agent) working with one person who
directs and reviews it. Measured from the session transcripts, last at {effort_when} UTC; agent time counts each
agent separately. The cost and by-hand figures are estimates, not measurements.</p>
<div class="tiles" style="margin:12px 0 0">{effort}</div>
</section>

<section>
<h2>What's left</h2>
<p class="lede">Next steps, in order, from <code>docs/progress.md</code>.</p>
<ol class="steps">{steps}</ol>
</section>

<section>
<h2>By original library</h2>
<p class="lede">Libraries from the original build tree (<code>/p71/prj71p/vf4e2/</code>).</p>
<div class="tablewrap"><table>
<thead><tr><th>Library</th><th class="n">Files</th><th class="n">Functions</th><th class="n">Named</th>
<th class="n">Named %</th><th class="n">Name confirmed</th><th class="n">Exercised</th><th class="n">Matched</th><th class="n">Matched % of bytes</th><th class="n">Bytes</th></tr></thead>
<tbody>{lib_rows}</tbody></table></div>
</section>

<section>
<details><summary><b>History table</b> (every point on the charts)</summary>
<div class="tablewrap"><table>
<thead><tr><th>Time</th><th>Step</th><th>Commit</th><th class="n">Functions</th><th class="n">Placed</th>
<th class="n">Named</th><th class="n">Matching C</th><th class="n">Globals</th><th class="n">Enums</th><th class="n">Placeholder vars</th>
<th class="n">Failures</th></tr></thead>
<tbody>{hist_rows}</tbody></table></div>
</details>
</section>
</main>
<div id="tip" role="tooltip"></div>
<script>
(() => {{
  const tip = document.getElementById('tip');
  const show = (html, e) => {{
    tip.innerHTML = html; tip.style.display = 'block';
    const w = tip.offsetWidth, h = tip.offsetHeight;
    let x = e.clientX + 14, y = e.clientY + 14;
    if (x + w > innerWidth - 8) x = e.clientX - w - 14;
    if (y + h > innerHeight - 8) y = e.clientY - h - 14;
    tip.style.left = x + 'px'; tip.style.top = y + 'px';
  }};
  const hide = () => {{ tip.style.display = 'none'; }};
  document.querySelectorAll('rect.seg').forEach(r => {{
    r.addEventListener('mousemove', e => show(r.dataset.tip, e));
    r.addEventListener('mouseleave', hide);
  }});
  document.querySelectorAll('svg[data-lines]').forEach(svg => {{
    const d = JSON.parse(svg.dataset.lines), hair = svg.querySelector('.xhair'), hit = svg.querySelector('.hit');
    const fmt = (v, u) => u === '%' ? v.toFixed(v < 10 ? 2 : 1) + '%' : Math.round(v).toLocaleString();
    hit.addEventListener('mousemove', e => {{
      const pt = svg.createSVGPoint(); pt.x = e.clientX; pt.y = e.clientY;
      const p = pt.matrixTransform(svg.getScreenCTM().inverse());
      let i = 0; d.x.forEach((x, k) => {{ if (Math.abs(x - p.x) < Math.abs(d.x[i] - p.x)) i = k; }});
      hair.setAttribute('x1', d.x[i]); hair.setAttribute('x2', d.x[i]); hair.setAttribute('visibility', 'visible');
      show('<b>' + d.rows[i] + '</b><br>' + d.series.map(s =>
        '<span class="k" style="background:' + s.color + '"></span>' + s.label + ': ' + fmt(s.values[i], s.unit)
      ).join('<br>'), e);
    }});
    hit.addEventListener('mouseleave', () => {{ hide(); hair.setAttribute('visibility', 'hidden'); }});
  }});

  // treemap: squarified layout, computed at the width it is shown
  const box = document.getElementById('treemap');
  const libs = JSON.parse(document.getElementById('tm-data').textContent);
  const STATE = ['Placeholder name', 'Named, not yet matched', 'Rebuilt from matching C'];
  const sum = a => a.reduce((s, v) => s + v, 0);
  const squarify = (vals, x, y, w, h) => {{
    const out = [], scale = w * h / (sum(vals) || 1);
    let i = 0;
    while (i < vals.length) {{
      const side = Math.min(w, h);
      let j = i, s = 0, hi = 0, lo = Infinity, best = Infinity;
      while (j < vals.length) {{
        const a = vals[j] * scale, s2 = s + a, h2 = Math.max(hi, a), l2 = Math.min(lo, a);
        const worst = Math.max(side * side * h2 / (s2 * s2), s2 * s2 / (side * side * l2));
        if (j > i && worst > best) break;
        best = worst; s = s2; hi = h2; lo = l2; j++;
      }}
      const t = s / side;
      let off = 0;
      for (let k = i; k < j; k++) {{
        const len = vals[k] * scale / t;
        out.push(w >= h ? [x, y + off, t, len] : [x + off, y, len, t]);
        off += len;
      }}
      if (w >= h) {{ x += t; w -= t; }} else {{ y += t; h -= t; }}
      i = j;
    }}
    return out;
  }};
  const inset = (r, d) => [r[0] + d, r[1] + d, Math.max(0, r[2] - 2 * d), Math.max(0, r[3] - 2 * d)];
  let cells = [];
  const draw = () => {{
    const W = box.clientWidth, H = Math.round(Math.max(380, Math.min(680, W * 0.62)));
    const parts = [], fileBoxes = [];
    cells = [];
    const libSizes = libs.map(l => sum(l.files.map(f => sum(f.f.map(c => c[2])))));
    squarify(libSizes, 0, 0, W, H).forEach((lr, li) => {{
      const lib = libs[li], r = inset(lr, 1.5);
      parts.push(`<rect class="lib" x="${{r[0]}}" y="${{r[1]}}" width="${{r[2]}}" height="${{r[3]}}" rx="4"/>`);
      let inner = inset(r, 2);
      if (r[2] > 60 && r[3] > 44) {{
        const chars = Math.floor((r[2] - 10) / 7);
        const name = lib.n.length > chars ? lib.n.slice(0, Math.max(1, chars - 1)) + '…' : lib.n;
        parts.push(`<text class="libname" x="${{r[0] + 5}}" y="${{r[1] + 13}}">${{name.replace(/&/g, '&amp;').replace(/</g, '&lt;')}}</text>`);
        inner = [inner[0], inner[1] + 16, inner[2], Math.max(0, inner[3] - 16)];
      }}
      squarify(lib.files.map(f => sum(f.f.map(c => c[2]))), ...inner).forEach((fr, fi) => {{
        const file = lib.files[fi], q = inset(fr, 1);
        fileBoxes.push(q);
        squarify(file.f.map(c => c[2]), ...q).forEach((cr, ci) => {{
          const c = file.f[ci], k = cells.length, g = cr[2] > 3 && cr[3] > 3 ? 0.5 : 0;
          cells.push({{ c, file: file.n, lib: lib.n, box: fileBoxes.length - 1 }});
          parts.push(`<rect class="fn s${{c[3]}}" data-i="${{k}}" x="${{(cr[0] + g).toFixed(1)}}" y="${{(cr[1] + g).toFixed(1)}}" ` +
            `width="${{Math.max(0.5, cr[2] - 2 * g).toFixed(1)}}" height="${{Math.max(0.5, cr[3] - 2 * g).toFixed(1)}}"/>`);
        }});
      }});
    }});
    box.innerHTML = `<svg viewBox="0 0 ${{W}} ${{H}}" height="${{H}}">${{parts.join('')}}` +
      `<rect class="hl" visibility="hidden"/><rect class="hl file" visibility="hidden" rx="2"/></svg>`;
    box.fileBoxes = fileBoxes;
  }};
  const hex = v => '0x' + v.toString(16).padStart(8, '0');
  box.addEventListener('mousemove', e => {{
    const i = e.target.dataset && e.target.dataset.i;
    const [hl, hf] = box.querySelectorAll('.hl');
    if (i === undefined) {{ hide(); hl && hl.setAttribute('visibility', 'hidden'); hf && hf.setAttribute('visibility', 'hidden'); return; }}
    const {{ c, file, lib, box: b }} = cells[i], t = e.target, fb = box.fileBoxes[b];
    ['x', 'y', 'width', 'height'].forEach(a => hl.setAttribute(a, t.getAttribute(a)));
    hl.setAttribute('visibility', 'visible');
    hf.setAttribute('x', fb[0]); hf.setAttribute('y', fb[1]); hf.setAttribute('width', fb[2]); hf.setAttribute('height', fb[3]);
    hf.setAttribute('visibility', 'visible');
    const sw = ['var(--tm-0)', 'var(--s2)', 'var(--s3)'][c[3]];
    show(`<b>${{c[0] || 'FUN_' + hex(c[1]).slice(2)}}</b><br>${{hex(c[1])}} · ${{c[2].toLocaleString()}} bytes<br>` +
      `<span class="k" style="background:${{sw}}"></span>${{STATE[c[3]]}}<br>` +
      `<span style="color:var(--muted)">${{file}}</span>`, e);
  }});
  box.addEventListener('mouseleave', () => {{ hide(); box.querySelectorAll('.hl').forEach(h => h.setAttribute('visibility', 'hidden')); }});
  let rt;
  draw();
  new ResizeObserver(() => {{ clearTimeout(rt); rt = setTimeout(draw, 120); }}).observe(box);
}})();
</script>
</body></html>
"""


def basis():
    if not LAYOUT:
        return "Percentages are of all of .text (layout not measured yet)."
    return (f"Percentages are of the {TEXT_BYTES:,} bytes of .text minus "
            f"{LAYOUT['padding']:,} bytes of alignment padding and "
            f"{LAYOUT['data_referenced'] + LAYOUT['data_other']:,} bytes of data that belong to no function. "
            f"A matched function counts with its literal pool and tables: the byte range its source "
            f"reproduces (ghidra/matched_extents.tsv, written by tools/match/match.py; measured by "
            f"ghidra/TextLayout.java). Points before 2026-11-09 counted function bodies without their "
            f"pools, so the lines step there.")


def write_page(rows, libs):
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "index.html"), "w") as f:
        f.write(render(rows, libs))


def same(a, b):
    return all(a[k] == b[k] for k in FIELDS[3:])


def update(label):
    src = os.path.join(ROOT, "src")
    if not os.path.isdir(src):
        print("progress: no src/ yet")
        return
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import effort
    effort.refresh()  # progress/effort.json, for the Effort section
    m, libs = measure(src)
    rows = read_history()
    row = {"time": datetime.datetime.now().isoformat(timespec="seconds"), "label": label,
           "commit": (git("rev-parse", "--short", "HEAD") or "none") + ("+" if git("status", "--porcelain", "src") else ""),
           **m}
    if not rows or not same(rows[-1], row):
        rows.append(row)
        write_history(rows)
        print(f"progress: new point ({label})")
    else:
        print(f"progress: no change since last point ({label})")
    write_page(rows, libs)


def backfill():
    rows = []
    commits = git("log", "--reverse", "--format=%H%x09%cI%x09%s", "--", "src").splitlines()
    for c in commits:
        sha, when, subject = c.split("\t", 2)
        with tempfile.TemporaryDirectory() as tmp:
            arch = subprocess.run(["git", "-C", ROOT, "archive", sha, "src"], capture_output=True).stdout
            subprocess.run(["tar", "-x", "-C", tmp], input=arch, check=True)
            m, _ = measure(os.path.join(tmp, "src"))
        label = subject.split(":")[0][:40]
        rows.append({"time": when[:19], "label": label, "commit": sha[:7], **m})
    write_history(rows)
    print(f"progress: {len(rows)} points from git history")
    update("working tree")


if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "update"
    if cmd == "backfill":
        backfill()
    else:
        update(sys.argv[2] if len(sys.argv) > 2 else "manual")
