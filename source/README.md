# source/: reconstructed C that matches

C for functions of the game, laid out like `src/` (one file per original
source file; where the original file is not known, a file of our own
naming). Some is written by hand; most is the decompiler's own output that
`tools/match/auto_match.py` found compiling to the game's bytes and
`tools/match/adopt.py` moved here (files in `_unmapped/` are named after
the start of their build group, `tools/match/delay_slots.py --groups`). Every function here compiles, with GCC 3.2.3 and the flags its
file names, to the game's exact bytes:

```bash
python3 -I tools/match/match.py
```

checks the whole tree; it must report every function matching before a
commit. `ghidra/run.sh export` puts each function defined here into `src/`
in place of the decompiler's version (its header says `matched:`), and
declares it in `src/include/vf4ft_functions.h` exactly as defined here.

Rules:

- A function goes in only once it matches. Work in progress stays outside
  the tree (any path given to `match.py` explicitly).
- The function's name is its name in the Ghidra project (a placeholder
  `FUN_0c...` until it gets a real one); the export fails on a name the
  project does not have.
- Signature on one line, `{` and `}` at column 0 (how the exporter finds
  definitions).
- Each file includes `vf4ft.h` and declares the unnamed globals it uses
  itself, under `DAT_<address>` with the type they had when the code
  matched (`tools/match/pin_globals.py`; Ghidra's placeholder names and
  types change as its analysis improves, and matching code must not). A
  global that needs a real name or better type gets a data row in
  `ghidra/symbols/`. Extra compiler flags go in a
  `match-flags:` line in the file's opening comment.
- Our own reconstruction only: no code from SDKs, other projects or
  references (see the top-level README).
