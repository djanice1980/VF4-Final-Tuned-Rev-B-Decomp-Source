# Live observation: watching every function in the running game

A function's name is **observed** when a run of the game in the Flycast
debug build shows the function doing the job its description says. This
runbook produces the evidence: for each function, who called it, in which
game mode, with which arguments, and what it returned.

- `tools/flycast_observe.py` sets a breakpoint on the entry of every
  function and logs each call.
- `tools/observe_summary.py` turns the logs into a digest per function.

**Status of the tools (2026-11-10):** everything was tested against a
stand-in for Flycast's GDB server (`build/agents/observe/`), not against
Flycast itself. The first live run is step 5 below, a one-minute run on five
functions; do that before the full run.

Every command below is run from the repository unless it says otherwise:

```bash
cd "$HOME/Claude Data/vf4ft-decomp"
```

## What you need

All of this is already on this machine; the checks are here in case
something moved.

1. The Flycast debug build. This prints one line with the file:

   ```bash
   ls -la "$HOME/Claude Data/flycast-gdb/build/flycast"
   ```

   If it is missing, it has to be built again from `~/Claude Data/flycast-gdb`
   (tag v2.7, `cmake -DENABLE_GDB_SERVER=ON -DUSE_LIBAO=OFF
   -DUSE_BREAKPAD=OFF`). The Flathub Flatpak cannot be used: it has no GDB
   server.

2. gdb with Python. This prints `GNU gdb (GDB) 18.1` or later:

   ```bash
   gdb --version
   ```

   If the command is not found:

   ```bash
   sudo pacman -S gdb
   ```

3. The game's code image, 3,407,872 bytes:

   ```bash
   ls -la work/main.bin
   ```

   If it is missing, the README's extraction step makes it again from the
   GD-ROM image.

4. The game itself, where Flycast looks for it:

   ```bash
   ls -la "$HOME/Games/naomi2/vf4tuned.zip"
   ```

## 1. Close every Flycast

Flycast writes its settings file when it exits, so the file can only be
edited while no Flycast runs. This prints nothing when none is running:

```bash
pgrep -a flycast
```

If it prints a line, close that Flycast from its own menu (Tab, then Exit)
and run the command again.

## 2. Switch the GDB server on

The settings file is shared by the Flatpak and the debug build:
`~/.var/app/org.flycast.Flycast/config/flycast/emu.cfg`. Keep a copy first:

```bash
cp "$HOME/.var/app/org.flycast.Flycast/config/flycast/emu.cfg" "$HOME/.var/app/org.flycast.Flycast/config/flycast/emu.cfg.bak-before-observe"
```

Change the one line `Debug.GDBEnabled = no` to `yes`:

```bash
sed -i 's/^Debug.GDBEnabled = .*/Debug.GDBEnabled = yes/' "$HOME/.var/app/org.flycast.Flycast/config/flycast/emu.cfg"
```

Check it. This must print `Debug.GDBEnabled = yes`, `Debug.GDBPort = 3263`
and `Debug.GDBWaitForConnection = no`:

```bash
grep '^Debug.GDB' "$HOME/.var/app/org.flycast.Flycast/config/flycast/emu.cfg"
```

## 3. Start the game in the debug build

In a terminal of its own (it stays busy while Flycast runs):

```bash
XDG_CONFIG_HOME="$HOME/.var/app/org.flycast.Flycast/config" XDG_DATA_HOME="$HOME/.var/app/org.flycast.Flycast/data" "$HOME/Claude Data/flycast-gdb/build/flycast" "$HOME/Games/naomi2/vf4tuned.zip"
```

If Flycast shows its game list instead of starting the game, double-click
Virtua Fighter 4 Final Tuned there.

The game has booted when the attract loop runs (SEGA logo, title screen,
demo fights). While the game runs, the GDB server listens; this prints a
line with `LISTEN` and `:3263`:

```bash
ss -ltn | grep 3263
```

The server listens on every network interface, so the port is reachable
from the LAN for as long as this Flycast runs.

## 4. Rules while the observer is attached

- **One GDB client at a time.** Do not run `tools/flycast_live.py` (any
  mode) while the observer runs: Flycast sends its stops to the newest
  client, and the observer would wait forever. Use the `CPU` file of step 7
  instead of `flycast_live.py`'s cpu mode.
- **No save states.** A state saved while the observer is attached holds
  the breakpoints inside the game's code, and a state loaded then replaces
  the code under the observer. (One such state exists already:
  `vf4tuned_3.state`, slot 3, saved 2026-11-06 22:04, has a breakpoint word
  at `hit_lights_frame`. Loading it in the Flatpak makes the game run a trap
  instruction at the end of the first fight frame. Save over slot 3, or
  load it in the debug build and run the recovery command of step 9, then
  save it again.)
- **Leave Flycast's own menu (Tab) closed** while the stop rate is high.
- **Do not close Flycast first.** Stop the observer (step 8), then close
  Flycast. If Flycast was closed first, nothing is lost: the observer says
  that the connection is gone and ends.

## 5. First live run: one minute on five functions

This run is the test of the live path. It uses its own folder, so it does
not count toward the real coverage.

```bash
mkdir -p work/observe-smoke
```

```bash
printf 'mode_frame\nmode_update\nmode_exec\nmalloc\nrob_pose_eval\n' > work/observe-smoke/functions.txt
```

With the attract loop running:

```bash
VF4OBS_DIR=work/observe-smoke VF4OBS_RUN=smoke VF4OBS_FUNCS=work/observe-smoke/functions.txt VF4OBS_TIME=60 gdb -q -batch -nx -x tools/flycast_observe.py
```

What it should print, within a few seconds of starting (the game pauses for
a moment at each stop; the times and counts will differ):

```text
01:46:17 5 functions: 0 cannot take a breakpoint, 0 already finished, setting 72 breakpoints
01:46:17 set 72 breakpoints in 0.0 s; mode ADVERTISE.TITLE_DISP, frame 5120
01:46:17 observing; stop with Ctrl-C, or from another terminal: touch work/observe-smoke/STOP
01:46:22 stops 14 (3/s) hits 9 returns 4 | functions seen 3 of 5, finished 0, parked 3, set 67, ...
...
01:47:17 stopping: time limit
01:47:17 all breakpoints removed; detached, the emulator runs
```

(72 = the five functions, the game's 66 mode handlers, which tell the
observer that the mode changed, and `code_jump_patch`, see "What can go
wrong".)

Three things to check on this run, in this order:

1. **The stops arrive and the game goes on.** `stops` must rise and the
   picture must keep moving between the status lines. If the picture
   freezes and `stops` stays the same for 30 seconds, press Ctrl-C; if that
   prints nothing within 10 seconds, use step 9.
2. **The calls make sense.** `mode_frame` runs once a frame and calls
   `mode_update` and then `mode_exec` (vf4lib/libbase/src/mode.c), so the
   digest must show exactly that:

   ```bash
   python3 tools/observe_summary.py work/observe-smoke/smoke.jsonl --out work/observe-smoke
   ```

   ```bash
   less work/observe-smoke/digest/libbase.md
   ```

   The blocks of `mode_update` and `mode_exec` must show the caller
   `mode_frame+0x...`, a mode by name (`ADVERTISE.TITLE_DISP` and so on),
   and `returns captured` above zero; the single calls must carry different
   frame numbers. (`rob_pose_eval` is only called
   while fighters are on screen, `malloc` only when something is loaded:
   either may be missing after one minute.)
3. **Nothing is left behind.** After the run the game must play as before,
   and the recovery command must find nothing:

   ```bash
   VF4OBS_MODE=clear gdb -q -batch -nx -x tools/flycast_observe.py
   ```

   It must print `clear: 0 trapa words found in the code, 0 breakpoints
   removed from Flycast's table, 0 words put back from the image`.

If one of the three fails, stop here and keep `work/observe-smoke/` for the
diagnosis.

## 6. The full run

In a second terminal, with the game in the attract loop:

```bash
VF4OBS_RUN=attract gdb -q -batch -nx -x tools/flycast_observe.py
```

`VF4OBS_RUN` is only the name of the log (`work/observe/attract.jsonl`).
Use a new name for each session (`attract`, `fight1`, `fight2`, ...); the
coverage carries over between sessions in `work/observe/state.json`, so a
later session only sets the breakpoints that are still open.

What you will see:

```text
01:24:42 6631 functions: 1 cannot take a breakpoint, 0 already finished, setting 6630 breakpoints
01:24:42   set 1000 of 6630 (0.0 s)
...
01:24:43 set 6630 breakpoints in 0.2 s; mode ADVERTISE.TITLE_DISP, frame 5120
01:24:43 observing; stop with Ctrl-C, or from another terminal: touch work/observe/STOP
01:24:48 stops 96 (19/s) hits 52 returns 40 | functions seen 49 of 6631, finished 0, parked 0, set 6629, waiting for a return 3 | ADVERTISE.TITLE_DISP
```

(The one that cannot take a breakpoint is `ta_input_finish`: the game
rewrites its entry while it boots.)

**The game freezes at first, and again at every new screen.** Every stop
halts the emulator for tens of milliseconds, and in the first frames almost
every function the game calls stops it. Expect the picture to stand still
for about 5 minutes after the start, then to move in jerks, then to run
almost freely. Each new screen (a new mode on the status line) brings a
shorter freeze of the same kind, about 1 to 3 minutes at first and less
later. This is normal. The status line tells you where it is:

- `stops N (R/s)`: R near 15 to 25 means a burst; R falling toward zero
  means the game is running freely again. A line comes every 5 seconds
  during a burst and every 15 seconds when the game runs freely.
- `functions seen`: the count that matters; it only goes up.
- `finished`: functions that have their hits from enough different callers
  or modes; their breakpoints are gone for good.
- `parked`: functions that have their hits in the current mode only; they
  are set again when the game enters a mode they were not seen in.
- the last field is the game's mode, by the game's own names.

Hold a button a little longer than usual during a burst: the game reads the
pad once per frame, and a frame can take many seconds.

### What to do in the game

The aim is to visit every mode and as many different fights as possible.
Wait at each step until the stop rate has dropped (R below about 2/s for a
minute), then go on. The times are estimates; nothing here has been timed
on Flycast yet.

| Step | What to do | Modes it covers | Time |
|---|---|---|---|
| A | Let the attract loop go around twice without touching anything: logos, title, rating screen, both demo fights, ranking. | ADVERTISE: SEGA_INIT to RANK_DISP | 20 to 30 min, of which 5 are the first freeze |
| B | Stop the observer (step 8), then start it again with `VF4OBS_RUN=fight1` and switch the AI on for both sides (step 7). | | 2 min |
| C | Insert a coin, press Start. Choose a character and, where the game offers it, a mode and a stage. | SELECTOR: SEL_INIT to SEL_EXIT | 5 to 10 min |
| D | Let the AI play the first fight to its end: stage intro, round calls, the rounds, the replay, the win pose. | GAME MODE: GAME_INIT to GAME_WINNER_DISP | 10 to 15 min for the first fight |
| E | Let it go on through four or five more fights. Each has another stage and opponent; most of the code is already finished by now, so these run close to full speed. | GAME MODE again, stage and character code | 3 to 5 min a fight |
| F | When the player side loses: let the continue countdown run to zero once (game over, and name entry if the score ranks). | OVER MODE: CONTINUE_*, GAMEOVER_*; ENDING MODE: NAME_ENTRY_* | 5 min |
| G | Start again with another character; this time, when the player side loses, insert a coin and press Start during the countdown. | OVER MODE continue path, SELECTOR again | 10 min |
| H | During a one-player fight, insert a coin and press Start on the second player's side. Let the versus fight end. | INTRUDE MODE, versus | 10 min |
| I | If a run reaches the last opponent and wins: let the ending and the staff roll play. | ENDING MODE: ENDING_INIT to ENDING_END | as it comes |
| J | Card play, only if the card reader is set up (see below). | card and network code | as it comes |

After each session, the summary tool lists the modes that were watched and
the ones no session has been in yet (step 11); use that list to decide
what to play next. The game's modes are: WARNING (INIT, DISP); ADVERTISE
(SEGA_INIT, SEGA_FREE, SEGA_DISP, CRI_DISP, TITLE_INIT, TITLE_DISP,
RATING_DISP, DEMO_INIT, DEMO_DISP, RANK_INIT, RANK_DISP); SELECTOR
(SEL_INIT, SEL_LOAD, SEL_CHARA_INIT, SEL_CHARA_SELECT, SEL_CHARA_DECIDE,
SEL_STAGE_INIT, SEL_STAGE_SELECT, SEL_STAGE_DECIDE, SUB_SEL_1PMODE_INIT,
SUB_SEL_1PMODE, SUB_SEL_1PMODE_DECIDE, SEL_EXIT); GAME MODE (GAME_INIT,
GAME_MASK_INIT, GAME_MASK, GAME_LOAD, GAME_STAGE_INIT, GAME_STAGE_DISP,
GAME_APPOSE_INIT, GAME_APPOSE_DISP, GAME_ROUND_INIT, GAME_ROUND_DISP,
GAME_DISP, GAME_SREP_DISP, GAME_JUDGE_INIT, GAME_JUDGE_DISP,
GAME_REPLAY_DISP, GAME_WINNER_DISP); ENDING MODE (ENDING_INIT, ENDING_FREE,
ENDING_DISP, ENDING_END, NAME_ENTRY_INIT, NAME_ENTRY_DISP); OVER MODE
(CONTINUE_INIT, CONTINUE_DISP, GAMEOVER_INIT, GAMEOVER_DISP,
GAMEOVER_RATING_DISP, FUKKATU_INIT, FUKKATU_DISP); INTRUDE MODE
(INTRUDE_INIT, INTRUDE_DISP); TEST (TEST_INIT, TEST_DISP, POLY_TEST_INIT,
POLY_TEST_CTRL: the game's own debug screens, not reachable from the
cabinet controls as far as is known).

**Coin and Start.** With Flycast's default keyboard layout (read from the
debug build's source, `core/input/keyboard_device.h`): coin is `Q`, Start
is Return, the stick is the arrow keys, the buttons are `X`, `C`, `S`, `D`.
If you mapped a controller, use its coin and start controls. To see or
change a mapping: Tab, Settings, Controls, then Map beside the device; the
arcade controls are listed there as Coin, Service, Test and Insert Card.

**The Naomi test menu.** The test menu is a second program
(`work/test.bin`) that the BIOS loads over the game, at the same address.
None of its code is in `ghidra/source_map.tsv`, so there is nothing to
observe in it. Entering it is safe: within 15 seconds the observer prints
`the game's code is no longer in RAM (test menu or reset): waiting for the
game to come back`, touches nothing while the test menu runs, and sets its
breakpoints again when you leave the menu and the game restarts (`the game
is back`). Leaving the test menu is therefore a way to see the game's boot
code: the observer is back about half a second after the game starts, so
it sees the boot from shortly after its first instructions. The keyboard
has no Test key by default; map one as described above.

**The boot from power-on.** Optional, and the least tested path. Close
Flycast, start it again (step 3), and start the observer at once with
`VF4OBS_BOOT=1`, while the BIOS is still loading the game:

```bash
VF4OBS_RUN=boot VF4OBS_BOOT=1 gdb -q -batch -nx -x tools/flycast_observe.py
```

It halts the machine a few times a second until the game's image is in RAM
and the program counter is inside it, then sets the breakpoints. The first
half second of the boot is missed.

**Cards.** The card code (422 functions of libcard, most of libnetwork)
runs only with a card reader. Whether this Flycast build emulates this
game's reader has not been checked here. If it does: the test menu's game
settings must have the card reader switched on, and Flycast's Insert Card
control (Settings, Controls, Map) must be mapped; press it when the game
asks for a card. If the game never asks, skip this step; the functions stay
on the never-hit list.

## 7. Letting the AI play both sides

The game has a debug switch, `cpu_on_off` (0x0C2926F0): 0 leaves the choice
to the game, 2 makes the AI play player 1, 3 player 2, 4 both. With 4 a
fight needs no hands; you still choose the character and insert the coins.

While the observer runs, write the switch through it, from another
terminal:

```bash
echo 4 > work/observe/CPU
```

The observer prints `cpu_on_off = 4` within a second or two. To give the
controls back:

```bash
echo 0 > work/observe/CPU
```

The same switch can be set with the existing tool, but **only while the
observer is not running** (one GDB client at a time). It takes about 10
seconds to connect:

```bash
VF4LIVE_MODE=cpu VF4LIVE_CPU=4 gdb -q -batch -nx -x tools/flycast_live.py
```

and back:

```bash
VF4LIVE_MODE=cpu VF4LIVE_CPU=0 gdb -q -batch -nx -x tools/flycast_live.py
```

The switch stays as written until it is written again or the game restarts.
Set it back to 0 before you close Flycast if you want to play yourself next
time in the same session.

## 8. Stopping cleanly

Either press Ctrl-C in the observer's terminal, or from another terminal:

```bash
touch work/observe/STOP
```

(`kill -TERM` on the gdb process does the same.) Within a few seconds it
prints:

```text
01:28:07 stopping: work/observe/STOP
01:28:07 all breakpoints removed; detached, the emulator runs
01:28:07 stops 1081 (43/s) hits 675 returns 390 | functions seen 213 of 6631, ...
01:28:07 log: work/observe/attract.jsonl
```

The game runs on at its normal debug-build speed. You can start another
session at once.

## 9. Recovery, if something was killed

If the observer was killed hard (`kill -9`, a closed terminal that took gdb
with it, a crash) its breakpoints are still in the game: the picture
freezes at the next one and stays frozen. Flycast itself is fine. Run:

```bash
VF4OBS_MODE=clear gdb -q -batch -nx -x tools/flycast_observe.py
```

It prints how many breakpoints it removed and `clear: detached, the
emulator runs`; the game moves again. It is safe to run at any time, also
when nothing is wrong (it then reports zeros). It also removes the
breakpoints of `tools/flycast_live.py`, so `VF4LIVE_MODE=clear` is not
needed after it.

If it prints `no GDB server at 127.0.0.1:3263`, Flycast is not running or
the game is not started: there is nothing to recover, because the
breakpoints only ever exist in the running emulator's memory.

## 10. Putting the settings back

Close Flycast from its own menu (Tab, then Exit). Check that it is gone
(prints nothing):

```bash
pgrep -a flycast
```

Switch the GDB server off again:

```bash
sed -i 's/^Debug.GDBEnabled = .*/Debug.GDBEnabled = no/' "$HOME/.var/app/org.flycast.Flycast/config/flycast/emu.cfg"
```

A GDB client switches the dynamic recompiler off and Flycast saves that
when it exits, which would slow the Flatpak down too. Switch it back on:

```bash
sed -i 's/^Dynarec.Enabled = .*/Dynarec.Enabled = yes/' "$HOME/.var/app/org.flycast.Flycast/config/flycast/emu.cfg"
```

Check both. This must print `Debug.GDBEnabled = no` and `Dynarec.Enabled =
yes`:

```bash
grep -E '^(Debug.GDBEnabled|Dynarec.Enabled)' "$HOME/.var/app/org.flycast.Flycast/config/flycast/emu.cfg"
```

## 11. The logs and the digest

In the tables, `<run>` stands for the name given with `VF4OBS_RUN`
(`attract`, `fight1`, ...) and `<library>` for a library's name (`libbase`,
`libaction`, `kamui`, ...). No command below needs either filled in.

| File | What it is |
|---|---|
| `work/observe/<run>.jsonl` | the log of one session, one JSON object a line (`hit`, `ret`, `noret`, `mode`, `skip`, `note`, `start`, `end`) |
| `work/observe/state.json` | hits, callers and modes per function so far: what the next session reads to know what is still open |
| `work/observe/<run>.bps` | exists only while a session runs or after one was killed: the addresses that hold a breakpoint (what the recovery reads) |

`work/` is not in git. Make the digest from all the logs:

```bash
python3 tools/observe_summary.py
```

It prints the coverage per library, the modes that were watched and the
ones no session has been in, and writes:

| File | What it is |
|---|---|
| `work/observe/summary.tsv` | one row per function: hits, callers, modes, the values seen in r4-r7 and fr4-fr7, the results |
| `work/observe/never_hit.tsv` | the functions no session saw called |
| `work/observe/digest/index.md` | the coverage per library and the mode lists |
| `work/observe/digest/<library>.md` | per source file, per function: the prototype and description the repo has, then the callers, modes, arguments, results and four single calls |

To start the coverage over (the logs stay; the next session sets every
breakpoint again):

```bash
rm work/observe/state.json
```

## Settings

All by environment variable, on the same line before `gdb`:

| Variable | Default | Meaning |
|---|---|---|
| `VF4OBS_MODE` | `observe` | `clear`: the recovery of step 9. (`python3 tools/flycast_observe.py check` prints, without connecting, which functions cannot take a breakpoint.) |
| `VF4OBS_RUN` | `obs-<date>-<time>` | name of the session's log |
| `VF4OBS_DIR` | `work/observe` | folder of the logs, the state file and the `STOP` / `CPU` files |
| `VF4OBS_FUNCS` | every function | a file of function names or addresses, one a line, to watch only those |
| `VF4OBS_SKIP` | none | a file of functions to leave out, same format |
| `VF4OBS_HITS` | 3 | hits a function needs |
| `VF4OBS_VARIETY` | 2 | different contexts (call site and mode) the hits must come from before the function is finished |
| `VF4OBS_MODE_QUOTA` | as `VF4OBS_HITS` | hits taken in one mode before the function is parked until the mode changes |
| `VF4OBS_MAX_HITS` | 8 | hits after which a function is finished whatever the variety |
| `VF4OBS_RET` | 2 | how many hits of a function also capture the return; `all` for every hit |
| `VF4OBS_RET_FRAMES` | 4 | game frames to wait for a return before giving up on it |
| `VF4OBS_PEEK` | 16 | bytes read at each pointer argument and pointer result |
| `VF4OBS_IDLE` | 15 | seconds without a stop before the observer halts the game for a moment to check its breakpoints |
| `VF4OBS_TIME` | 0 | stop by itself after this many seconds |
| `VF4OBS_CPU` | not set | write this to `cpu_on_off` when attaching |
| `VF4OBS_BOOT` | not set | `1`: wait for the game image before setting anything |
| `VF4OBS_FRESH` | not set | `1`: ignore the counts in the state file |

## What can go wrong

| What you see | What it is | What to do |
|---|---|---|
| `no GDB server at 127.0.0.1:3263` | Flycast is not running, the game is not started, or `Debug.GDBEnabled` is `no`, or it is the Flatpak | steps 1 to 3 |
| `only N% of the function entries in RAM match work/main.bin: the game is not loaded yet` | the observer was started while the BIOS or the test menu was running | wait for the attract loop and start it again |
| the picture is frozen and no status line comes for a minute | a burst prints a line every 5 seconds, so this is not a burst: the observer is gone or stuck | Ctrl-C; if nothing happens in 10 seconds, close that terminal and run step 9 |
| `leftover breakpoints of an earlier session: ...` at the start | an earlier session was killed; they were removed before anything else | nothing |
| `a breakpoint that is not ours at ...: removed` | a breakpoint word that Flycast did not know, as a loaded save state brings | nothing; do not load states while observing |
| `N breakpoints were gone from RAM ...: set again` | the game reloaded its code (a reset) | nothing |
| `the game ran its own trapa ...` and the session ends | the game hit one of its own fatal-error traps; with a debugger attached Flycast halts there instead of letting the game handle it | note the function it names; restart the game |
| `the emulated CPU stopped with SIGILL / SIGSEGV / SIGBUS ...` and the session ends | the game crashed in the emulator | keep the log; restart Flycast |
| `removing the breakpoints through a second connection` | the observer's own connection failed, so it started the recovery by itself | nothing if it ends with `clear: detached, the emulator runs`; otherwise step 9 |
| the Naomi test menu's RAM or ROM check reports an error while the observer is attached | not expected (the test menu is another program), but a check that reads the game's code would see the breakpoint words | stop the observer and run the check again |

## 12. Judging the log, and the "observed" level

A routine that appears in the log has only been *reached*. It counts as
*observed* when the log confirms something its description claims that the
code alone does not compel. Each routine the run entered is read against
its name and description (the digest section, the raw log where the digest
is too thin, and the C), and gets one of three verdicts:

| Verdict | Meaning |
|---|---|
| observed | the log confirms a specific claim: a value, a pairing of arguments and results, memory read before and after, a timing |
| reached | the routine ran, but the log shows nothing beyond what the code compels (literal arguments, a constant return, only the idle path, an effect outside the logged bytes) |
| conflict | the log contradicts the name or the description; the description is then corrected by hand |

The rulings that keep the verdicts even:

- an argument that is a literal at the call site, or a return the code
  compels, is not evidence;
- a status routine that only ever gave its normal answer, or a getter that
  only ever returned 0, is reached;
- memory seen through a register that is not a parameter may be used if
  the row says so;
- "was not called" is only safe while the routine's breakpoint was still
  armed (three hits in two contexts retire it), so a missing callee proves
  nothing by itself;
- values that come from hardware registers show the emulator's model, not
  the game, and are not evidence either way.

The verdicts of a run are kept in `ghidra/live_verdicts.tsv` (address, name,
verdict, observation, correction). `tools/observe_apply.py` puts them into
the symbol tables: an observed routine's row gets the sentence
`observed live <date>: ...` appended to its evidence and its confidence
raised from `inferred` to `observed` (a `named` row keeps its level). The
next `ghidra/run.sh types` and `export` carry that into `src/`, and
`tools/progress.py` counts it on the page ("Code whose name is confirmed").

```bash
cd "$HOME/Claude Data/vf4ft-decomp"
```

```bash
python3 -I tools/observe_apply.py ghidra/live_verdicts.tsv
```

Running it again changes nothing. The first judged run (2026-11-10, 30.8
minutes of attract mode, a coin, character select and fights) entered 2,494
of the 6,632 routines: 1,716 observed, 737 reached, 41 conflicts.

### Judging a harness log

The second judged run came from the VF4debug harness, not the emulator:
62 unattended scenarios, 4,026 routines entered, a digest section per
routine (`tools/digest.py` there). Its record changes two of the rulings
above: counts are exact (per routine, per scenario, per call site), so
"it did not call X" and "once per frame" can be read off; and each kept
call shows the bytes behind its pointers at entry and at return and every
store it made, so a routine with no arguments and no result can be
judged by what it wrote. What it adds to watch for:

- the hardware is minimal models: a value read from a modelled register,
  a planted BIOS word or the sound model is the harness's answer, while a
  value the game itself wrote to a register is the game's;
- scenarios that poke globals: what a routine does with its inputs is
  evidence, when it is called is evidence only from real inputs;
- coverage of a routine's own instructions decides how much of its
  description was tested (only the early-out path ran: reached);
- a routine built on a computed jump can show every instruction run while
  only some cases ran.

The judging brief with all its rulings is
`build/agents/observed2/BRIEF.md` (not in git; the rulings that matter
are the ones above). The second run judged 2,185 routines: 1,626
observed, 498 reached, 61 conflicts.

Design notes, for whoever reads the logs:

- A hit's `caller` is the function the return address (PR) lies in. A
  function reached by a jump from the end of another one (a tail call, or
  assembly that falls through) shows the caller of that other function.
- The return is captured at the return address when r15 is back at its
  entry value; a deeper call through the same call site does not count.
  A function that does not come back within `VF4OBS_RET_FRAMES` frames gets
  a `noret` line and no further return captures.
- The eight routines of `sdk/sy/exception_vectors.c` are entered by an
  exception; their hits are logged without a caller and without a return.
- While a function waits for its return, and between a hit without return
  capture and the next stop, its entry breakpoint is out, so calls in that
  window are not seen. The counts are a sample, not a call count.
- `code_jump_patch` is watched in every session, also with `VF4OBS_FUNCS`:
  it is the routine the game rewrites a function entry with
  (`ta_input_finish`, at boot). When it is called, the observer first takes
  its own breakpoint out of the entry that is about to be rewritten.
