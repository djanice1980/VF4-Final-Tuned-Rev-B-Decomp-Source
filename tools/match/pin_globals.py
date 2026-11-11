"""Give source/ files their own declarations of the unnamed globals they use.

    python3 -I tools/match/pin_globals.py [--from-git REV] [file.c ...]

Ghidra's placeholder names are not stable: PTR_ names follow the label of
what they point to, and a global that becomes part of an object loses its
DAT_ label. So matching C must not depend on the generated vf4ft_data.h.
Each file gets an "unnamed globals" block declaring every DAT_ / PTR_ name it
uses, PTR_ ones renamed to DAT_<address> (stable: match.py reads the address
from the name; an object's agg_<address>_<address> label stays), each with
the type it had when the file matched; the include
of vf4ft_data.h is dropped. Declarations come from src/include/vf4ft_data.h
(or that file at git REV, which is what files adopted from that export
used). With no files, every file in source/ that includes vf4ft_data.h.
"""
import os, re, subprocess, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import program

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DECL = re.compile(r"^extern (.*?)((?<![\w])_?(?:DAT|PTR|LAB|SUB)_[A-Za-z0-9_]+|agg_[0-9a-f]{8}_[0-9a-f]{8})((?:\[[^\]]*\])*); /\* 0x([0-9a-f]{8})", re.M)
NAME = re.compile(r"(?<![\w])(?:_?(?:DAT|PTR)_[A-Za-z0-9_]+|agg_[0-9a-f]{8}_[0-9a-f]{8})\b"
                  r"|(?<!goto )\b(?:LAB|SUB)_[0-9a-f]{8}\b(?!:)")  # a goto's LAB_ is a label
BLOCK = re.compile(r"/\* unnamed globals[^\n]*\*/\n(?:extern [^\n]*\n)*\n?")


def declarations(rev):
    text = (subprocess.run(["git", "-C", ROOT, "show", f"{rev}:{program.SRC_REL}/include/vf4ft_data.h"], capture_output=True, text=True, check=True).stdout
            if rev else open(os.path.join(program.SRC, "include", "vf4ft_data.h")).read())
    return {m.group(2): (m.group(1).strip(), m.group(3), m.group(4)) for m in DECL.finditer(text)}


def pin(path, decls, refresh=()):
    """refresh: DAT_<address> names to declare with their type in decls now,
    not the one the file had (a newly matched function wants the new type)."""
    s = open(path).read()
    old = {}
    for m in DECL.finditer("".join(BLOCK.findall(s))):  # an existing block keeps its types
        old[m.group(2)] = (m.group(1).strip(), m.group(3), m.group(4))
    body = BLOCK.sub("", s)
    names = sorted(set(NAME.findall(body.split("*/", 1)[1] if "*/" in body else body)))
    lines, missing = [], []
    for n in names:
        d = (decls.get(n) or old.get(n)) if n in refresh else (old.get(n) or decls.get(n))
        if d is None:
            m = re.fullmatch(r"_?DAT_([0-9a-f]{8})", n)
            if m:  # not in the header: an untyped byte there (a wrong type fails the match check)
                d = ("undefined", "", m.group(1))
            else:
                missing.append(n)
                continue
        typ, dims, addr = d
        # an object's label is its address already; a _DAT_ name too, and it is
        # another type than the DAT_ at that address, so it keeps its own name
        stable = n if n.startswith(("agg_", "_DAT_")) else f"DAT_{addr}"
        if n != stable:
            body = re.sub(r"\b" + re.escape(n) + r"\b", stable, body)
        lines.append((stable, f"extern {typ} {stable}{dims}; /* 0x{addr} */".replace(" * ", " *").replace("*  ", "* ")))
    if missing:
        return missing
    seen, block = set(), []
    for stable, l in sorted(lines):
        if stable not in seen:
            seen.add(stable)
            block.append(l)
    body = body.replace('#include "vf4ft_data.h"\n', "")
    if block:
        head, inc, rest = body.partition('#include "vf4ft.h"\n')
        body = head + inc + "\n/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */\n" + "\n".join(block) + "\n" + rest
    open(path, "w").write(re.sub(r"\n{3,}", "\n\n", body))
    return []


def main():
    args = sys.argv[1:]
    rev = None
    if "--from-git" in args:
        rev = args[args.index("--from-git") + 1]
        args = [a for a in args if a not in ("--from-git", rev)]
    decls = declarations(rev)
    files = args or sorted(os.path.join(d, f) for d, _, fs in os.walk(program.SOURCE) for f in fs
                           if f.endswith(".c") and '#include "vf4ft_data.h"' in open(os.path.join(d, f)).read())
    bad = 0
    for f in files:
        miss = pin(f, decls)
        if miss:
            bad += 1
            print(f"{os.path.relpath(f, ROOT)}: no declaration for {', '.join(miss[:5])}")
    print(f"pinned {len(files) - bad} of {len(files)} files")


if __name__ == "__main__":
    main()
