"""Which program the matching tools work on: main.bin (default) or test.bin.

    VF4FT_PROGRAM=test python3 -I tools/match/match.py ...

main: the game, work/main.bin, exported to src/, matched C in source/.
test: the test-mode program, work/test.bin (ghidra/run.sh test), exported
      to src_test/, matched C in source_test/. Both load at 0x0C020000.
"""
import os

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
NAME = os.environ.get("VF4FT_PROGRAM", "main")
if NAME not in ("main", "test"):
    raise SystemExit(f"VF4FT_PROGRAM={NAME}: main or test")

IMAGE = os.path.join(ROOT, "work", f"{NAME}.bin")
SRC_REL = "src" if NAME == "main" else "src_test"
SRC = os.path.join(ROOT, SRC_REL)
SOURCE = os.path.join(ROOT, "source" if NAME == "main" else "source_test")
SLOTS = os.path.join(ROOT, "work", "delay_slots.tsv" if NAME == "main" else "delay_slots_test.tsv")
