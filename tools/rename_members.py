"""Rename an object, its type and its members in the C that spells them by hand
(source/, source_test/), after ghidra/types/*.h gave the object a named
struct in place of the aggregate AssignAggregates made.

    python3 -I tools/rename_members.py SPEC.json [--dry-run]

SPEC.json:
  {"type":   ["agg_0c370ce0", "camera_state"],
   "object": ["agg_0c370ce0_0c370ce0", "camera"],
   "members": {"field_0xc": "eye.x", "field_0x30": "flags", ...}}

Rewritten:
  object.member            -> new_object.new_member
  p->member                where p is declared `type *p` in the same function
  the type and the object  everywhere else (externs, &object, comments)
Members not in the map keep their names (the new struct keeps those too).
A use the rewrite cannot see (a cast pointer, a parameter typed int) keeps
the old member name; match.py's compile reports it, so nothing passes
silently. src/ is not touched: the next types + export regenerate it.
"""
import json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def files():
    for top in ("source", "source_test"):
        for d, _, fs in os.walk(os.path.join(ROOT, top)):
            for f in sorted(fs):
                if f.endswith((".c", ".h")):
                    yield os.path.join(d, f)


def main():
    spec = json.load(open(sys.argv[1]))
    dry = "--dry-run" in sys.argv
    old_t, new_t = spec["type"]
    old_o, new_o = spec["object"]
    members = spec["members"]
    mem = "|".join(sorted(map(re.escape, members), key=len, reverse=True))
    on_obj = re.compile(r"\b" + re.escape(old_o) + r"\.(" + mem + r")\b")
    decl = re.compile(r"\b" + re.escape(old_t) + r"\s*\*\s*(\w+)")
    total = 0
    for path in files():
        text = open(path).read()
        if old_t not in text and old_o not in text:
            continue
        n = 0

        def obj_sub(m):
            nonlocal n
            n += 1
            return new_o + "." + members[m.group(1)]
        # a spec that renames only the type and object has no member pass
        if members:
            text = on_obj.sub(obj_sub, text)
        # one function at a time: Ghidra's text closes each body with "}" at column 0
        out = []
        for chunk in re.split(r"(?m)(?<=^\}\n)", text):
            names = set(decl.findall(chunk))
            if names and members:
                arrow = re.compile(r"\b(" + "|".join(map(re.escape, sorted(names))) + r")->(" + mem + r")\b")

                def ptr_sub(m):
                    nonlocal n
                    n += 1
                    return m.group(1) + "->" + members[m.group(2)]
                chunk = arrow.sub(ptr_sub, chunk)
            out.append(chunk)
        text = "".join(out)
        text = re.sub(r"\b" + re.escape(old_o) + r"\b", new_o, text)
        text = re.sub(r"\b" + re.escape(old_t) + r"\b", new_t, text)
        print(f"{n:5d} member uses  {os.path.relpath(path, ROOT)}")
        total += n
        if not dry:
            open(path, "w").write(text)
    print(f"{total} member uses rewritten" + (" (dry run)" if dry else ""))


main()
