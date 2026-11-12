# Progress

## 2026-10-21: foundation

Pipeline, from your own dump to annotated C:

```bash
tools/extract.sh "$HOME/Downloads/VF4 Final Tuned Rev B"
ghidra/run.sh all
```

| Step | Result |
|------|--------|
| GD-ROM -> DIMM image | CHD verified against MAME; `VF41_S.BIN` decrypted with the PIC key (MAME DES port) |
| Program split | main 0x340000 bytes @ 0x0C020000; test mode 0x80000 bytes |
| Sections | `.text` 0x0C020000-0x0C15D9FF, `.rodata` -0x0C242FFF, `.data` 0x0C243000-; 6 hand-asm scratch words kept writable |
| Functions | 5,995 (4,039 from auto-analysis, 1,754 from the GCC prologue scan, 1 false hit removed) |
| Source files | 107 functions placed by assert string, 445 by link order, in 45 original `.c` files |
| Asserts | 104 sites: file, line, expression, recorded in `ghidra/asserts.tsv` and on each function |
| Name tables | 116 string-pointer tables, 3,787 names (`ghidra/name_tables.tsv`) |
| Export | `src/`, 5,989 functions decompiled (6 failed), laid out like `/p71/prj71p/vf4e2/` |

### Findings

- **Source order is preserved.** In all 17 files with asserts in two or
  more functions, the assert line numbers rise with address. Functions
  are emitted in source order, so the link-order file assignment holds.
- **Compiler is GCC**, from the frame-pointer prologue
  (`mov.l r14,@-r15; sts.l pr,@-r15; mov r15,r14`) and in-order
  emission; confirmed later by the libgcc division helpers (FPU pass).
- **`fatal_at` (0x0C0F23A0)** is a printf-style `(file, line, fmt, ...)`
  fatal error. `assert()` wraps it; `malloc.c`, `load.c`, `sound_cmn.c`
  and others call it directly with their own messages.
- **Hand-written SH-4 assembly** around 0x0C10C000-0x0C13C000 stores
  into scratch words inside its own code (FPU-scheduled routines,
  probably matrix/vertex work for the Elan path).
- **Name tables** give whole enums: the fighter action state machine
  (`MODE_NO_ACT` ... at 0x0C16CB04; 26 entries, see the enums pass), per-character move lists
  (`AK_UKASHI` ... 240 entries), cameras, stages, character IDs, and a
  964-entry asset filename table (`obj_debug.bin.gz` ...).
- **`mode_data`** at 0x0C162B00: stride 0xCC per main mode, a
  `main_func` and an array of 12-byte `sub_func` entries, each starting
  with `mode_asg` (its own index, checked by assert).

## 2026-10-21: enums pass

| Step | Result |
|------|--------|
| Name tables | 93 string-pointer runs, classed enum / assets / text (`ghidra/name_tables.tsv`) |
| Debug menu | 1,456 item records decoded (`ghidra/debug_menu.tsv`, layout in `tools/debug_menu.py`) |
| Enums | 69 Ghidra enum types in `/vf4ft/enums`, 50 members prefixed to keep C names unique (`ghidra/enums.tsv`) |
| Globals | 908 named from Sega's menu labels, 446 typed (enum or float), plus 69 `<enum>_names` tables |
| Headers | `src/include/vf4ft.h`, `vf4ft_enums.h`, `vf4ft_globals.h`; compile with `gcc -std=c11 -fsyntax-only` |

### Findings

- **The debug menu describes its globals.** Each 0x58-byte record holds
  a label, a widget kind, a size code, step/min/max, an optional enum
  name table and the target address (or a struct offset, flag 0x01).
  Enum-list items give the exact table length (max + 1).
- **Literal-pool references mark table starts.** `MODE_*` is 26
  entries, followed by a 23-entry flag table and `STROKE_*` at entry 49;
  both cuts are pool-referenced. The same holds for `ALAB_STAT_*` /
  `DIMM_*`, `TOTAL_RANK_*` / `RESULT_RANK_*` and the challenge-map tables.
- **Linker tail-merges strings** (`"HIT"` points into `"NO HIT"`), so a
  table entry can point into the middle of another string.
- **Auto-analysis left 341 junk instructions in `.rodata`/`.data`**;
  `SplitSections.java` now clears them on every run.
- **FPSCR.SZ is unknown at function entry**, so the decompiler emits
  `if (in_FPSCR_SZ == '\0')` branches around paired FPU moves.

Most enum constants are not visible in code yet: the typed globals are
debug settings, and the enums' real users are struct fields and
parameters, which come with the struct pass.

## 2026-10-21: FPU mode pass

| Check | Stock SH-4 language | `SuperH4:LE:32:vf4ft` |
|-------|---------------------|-----------------------|
| `in_`/`unaff_FPSCR_*` placeholders in `src/` | 24,042 | 0 |
| Decompile failures | 6 | 3 |
| Lines of C (game + unmapped) | 377,117 | 263,865 |
| Functions breaking the FPU-mode model | n/a | 4 of 5,995, flagged in `src/` |

### Findings

- **Ghidra's SH-4 `fschg` never updates `FPSCR_SZ`**, the pseudo-register
  every `fmov` tests, so the decompiler could not follow mode switches.
  `ghidra/lang/` holds a language variant (derived from Ghidra's SuperH4
  module, Apache-2.0) where `fschg` also flips `FPSCR_SZ`, and the
  calling convention keeps FPSCR/SZ/PR across calls.
- **Entry state comes from the binary**: startup loads FPSCR = 0x40000
  (PR = 0, SZ = 0). `SetFpuEntryState.java` sets SZ/PR = 0 over `.text`;
  the variant pspec's tracked_set did not take effect in Ghidra 12.1.2.
  Inside a function, `fschg` still switches mode (the 64-byte matrix
  loads at 0x0C0D8480 stay eight 64-bit transfers).
- **`CheckFpuMode.java` verifies it on every path of every function**:
  118 functions use `fschg`; 4 break the model, all hand-written asm
  (0x0C107E02 and its helper 0x0C1088AC use a private SZ=1 convention;
  0x0C13B822/0x0C13B8A6 reload FPSCR from scratch words). Each carries a
  `WARNING (fpu model)` comment.
- **Compiler confirmed: GCC, `-m4-single`.** 0x0C159240 and 0x0C159258
  are libgcc's `__sdivsi3_i4` / `__udivsi3_i4` for single-precision
  default, instruction-for-instruction (gcc 3.0.4 through 4.0.4, the
  version before the PR 6526 rewrite). Named via `known_functions.tsv`.
- Delay slots: Ghidra names a delay-slot instruction `_fschg`, and 58
  `fschg` sit in `rts` delay slots, so any mnemonic match must strip `_`.

## 2026-10-21: calling-convention pass

Measured with `tools/cc_stats.py` (objdump + Ghidra function bounds):

| Register(s) | Saved by functions that write them | Verdict |
|-------------|------------------------------------|---------|
| r8-r14 | 99.3-99.5% | callee-saved (already in the cspec) |
| fr12-fr15 | 456/456, 254/254, 176/176, 100/100 in GCC code | callee-saved, added to the cspec |
| r0-r7, fr0-fr11, MACH, MACL, FPUL | ~0% | caller-saved (as before) |

- **Float arguments are in order** (fr4, fr5, ...): fr4 is live-in in 155
  single-float functions, fr5 in 10; pairs are (fr4, fr5) 54 times. No
  change needed. (A swapped-pair quirk I half-remembered does not apply.)
- **Prologue-scan bug fixed:** `FindPrologues.java` only walked back over
  `mov.l rN,@-r15`, so 173 functions started inside their own prologue,
  after `fmov fr12,@-r15` or `sts.l macl,@-r15`. 157 of the corrected
  entries are 32-byte aligned (GCC's function alignment); 14 stops are
  `rts; fschg` delay slots of the previous function. 39 fragments merged
  back: 5,995 -> 5,956 functions.
- **libgcc division helpers return in FPUL:** custom storage via
  `known_functions.tsv` (`int __sdivsi3_i4(int dividend, int divisor)`).
  `extraout_FPUL` 389 -> 25; the rest sit around the private-convention
  asm at 0x0C108840-0x0C108A18 or behind unresolved indirect calls.
- **Remaining noise has a different root:** each SH-4 `fmov` has both a
  32-bit and a 64-bit path in its semantics, chosen by FPSCR_SZ at
  decompile time. With SZ known the 64-bit path is removed, but only after
  the decompiler has merged fr12/fr13 into the dr12 pair, so a callee-save
  reads as `local = (ulonglong)in_dr12 >> 0x20` (575 `in_dr12`, 296
  `in_dr14`). Listing dr12/dr14 as unaffected did not help (tested).

## 2026-10-21: SZ as a disassembly context

| Measure | Before | After |
|---------|--------|-------|
| `in_dr12` / `in_dr14` | 575 / 296 | 248 / 229 |
| "Removing unreachable block" warnings | 36,802 | 11,645 |
| Lines of C | 259,843 | 230,867 |
| Placeholder variables (excluding the XMTRX bank, as on the progress page) | 3,914 | 3,552 |
| Functions | 5,956 | 5,961 (5 GCC functions found that bad decoding had hidden) |

- **The language variant now decodes each `fmov` at one width.** `sz` is
  a SLEIGH context variable; the 8 `fmov` forms are split into `sz=0`
  (frN, 4 bytes) and `sz=1` (drN/xdN, 8 bytes) constructors, and `fschg`
  sets the context for the following instruction. The listing shows real
  `dr`/`xd` registers (`fmov @r14+,dr0`, `fmov @r3+,xd2`).
- **Flow context alone is wrong in 195 places.** 59 `fschg` sit in the
  delay slot of a `bra`/`bsr`/`bt.s`/`bf.s` (all in hand-written asm), and
  3 asm helpers are entered with SZ=1. `AssignFpuContext.java` computes SZ
  per instruction (per-function dataflow, call-site entry modes and
  callee exit summaries, constant FPSCR loads such as libgcc's `0x80000`)
  and re-decodes the 195 `fmov`s that disagree (72 to 64-bit, 123 to
  32-bit). A second pass finds nothing left; it replaces CheckFpuMode.
- **A delay slot decodes with its branch's context**, so fixing one means
  setting the branch's computed SZ as well.
- **`in_xf*` inputs are real, not noise**: they read the back register
  bank (XMTRX) that one routine loads and `ftrv` code uses later, e.g. a
  dot product with a matrix column at 0x0C10D170. The progress page no
  longer counts them as noise.
- **What is left of the noise comes from PR**: `fadd`, `fmul`, `fneg` and
  the rest still carry a single and a double path, chosen by FPSCR_PR.

## 2026-10-21: PR as a disassembly context

| Measure | After SZ pass | After PR pass |
|---------|---------------|---------------|
| `in_dr12` / `in_dr14` | 248 / 229 | 51 / 105 |
| "Removing unreachable block" warnings | 11,645 | 2,259 |
| Lines of C | 230,867 | 218,965 |
| Placeholder variables (progress-page measure) | 3,552 | 3,324 |
| Decompile failures | 3 | 1 |

- **18 constructors split by a `fpu_pr` context** (single forms with
  `pr=0`, double forms with `pr=1`), keeping stock Ghidra's quirks: the
  double form of `fcmp/eq`, `float`, `fneg` needs even registers (odd ones
  fall back to single), and `fipr`, `fmac`, `frchg`, `fschg`, `ftrv`,
  `fsrra`, `fsca` do nothing with PR=1. The field is `fpu_pr`, not `pr`:
  Ghidra register names are case-insensitive and `pr` clashed with the PR
  (return address) register.
- **AssignFpuContext.java solves PR after SZ** with the same dataflow.
  Exactly 10 instructions need PR=1, all inside libgcc's `__sdivsi3_i4` /
  `__udivsi3_i4`, which now read like their source (`float fpul,dr0`,
  `fdiv dr2,dr0`, `ftrc dr0,fpul`).
- **Decompile failures moved**: the 3 earlier ones now decompile;
  0x0C10D740 (asm vertex routine, `ftrv` in a `bf.s` delay slot) now fails
  with the same "Conditional execution: Illegal op in iblock" decompiler
  limitation. The error depends on the shape of delay-slot p-code, not on
  the model being wrong.
- `in_FPSCR_CAUSE/FLAG/...` (6 each) remain where `sts fpscr` saves the
  caller's FPSCR; those are runtime exception state, correctly unknown.

## 2026-10-21: struct pass

New pipeline step `types`: `ghidra/types/*.h` (plain C, every field
commented with its evidence) and `ghidra/symbols/*.tsv` (address, kind,
C declaration, confidence, evidence), applied by `ApplyTypes.java`;
`tools/mode_table.py` generates the mode-table part from the binary.

| Area | Recovered |
|------|-----------|
| `mode.c` | `mode_func` / `mode_data_t`, the 8-entry `mode_data` state machine; `e_mode_main` + 8 sub-mode enums (60 sub-modes); 68 handlers named after their own mode (`sel_chara_select`, `game_stage_init`, ...); the driver (`mode_req_main`, `mode_update`, `mode_exec`, `mode_frame`, ...) and its globals (`main_mode_no`, `sub_mode_no` from the asserts) |
| `malloc.c` | `heap_block` (32-byte header, `bsize` and `user_request_size` named by asserts), `heap` (0x6C), `e_block_state`; `malloc_internal` / `check_heap_internal` (named by their own messages), the list, guard and poison helpers, `heap_malloc/free/calloc`, `malloc/free/calloc` on `main_heap`; `memset`, `memcpy` |
| `tm_sys.c` | `struct tm`, `rtc_time`; `get_now_tm`, `mktime` |
| file access | `dimm_file` (0x4A4, 32 handles), `dimm_fopen/fclose/fcheck`, PIO and DMA `fread`s |
| `load.c` | `load_slot` (0x58, 32 slots), `e_load_state`; `load_req`, `load_req_type`, `load_service`, `load_file`, `read_file`; `decode_gzip_data` (named by its error message); string helpers |

124 functions and 19 globals named by this pass (128 named or inferred
functions in all); headers still compile.

### Findings

- **`tm_sys.c` is time, not a task manager** (my earlier guess from the
  name): it fills a C `struct tm` from the RTC and calls `mktime`.
- **Every allocation is tagged with its caller's address**: the public
  heap wrappers pass their own `PR` (`sts pr`) as `malloc_internal`'s tag,
  store an allocation serial for the "break point" debug stop, and run
  with interrupts masked. Fresh memory is filled with 0xCD, freed with 0xDD,
  and a `"++++"` guard follows each allocation.
- **Do not give variadic functions a `...` prototype**: with this calling
  convention Ghidra then drops the arguments passed in registers
  (`snprintf`'s r7 vanished at all 41 calls). Name them only.
- The decompiler hides an unused register parameter: `free_internal`
  looked like `(heap, ptr)` but reads `ptr` from r6 (`heap, tag, ptr`).
- Header types can only reference each other if one C parser parses them
  all; a fresh parser finds types by name only in the root category.

## 2026-10-21: asset filesystem

- **SimpleFlashFS** (Sega's name, version string "1999.5.5") at 0x800000:
  908 files packed in 4 KiB blocks, 0x100-byte header + data each.
  `tools/sffs.py` lists and extracts them by the same walk as the game's
  loader; `docs/assets.md` documents the format, `docs/assets.tsv` lists
  every file with its SHA1.
- **VF41_S2.BIN is a byte-identical copy** of VF41_S.BIN once decrypted.
- 650 gzip files (3 empty placeholders) all unpack; modification times
  run 2003-10-17 .. 2004-10-26.
- **106 filenames in the program have no file on the disc** (test assets,
  unused character variants, a PS2 stage, older sprite names): listed in
  `docs/assets.md`.
- Code: `sffs_entry`, `sffs_dir[1200]`, `sffs_load_dir`, `sffs_find`,
  `path_resolve`, `dimm_read`, string helpers.

## 2026-10-21: asset formats

- **Textures decoded**: `tools/pvr_tex.py`, 10,434 textures from the
  216 `obj_`/`tex_` sets (PowerVR2 twiddled / VQ / mipmapped / rectangle).
  Descriptor table in `obj_`, data in `tex_`, format word as
  `tune_texture.c` reads it.
- **Sprites decoded**: `tools/spr_tex.py`, 905 pages and 2,942 cells from
  the 57 `spr_` files, through the sprite registry in the program.
- Two corrections found by checking, not assumed: the commonly quoted
  twiddled-mipmap offsets are 2 texels off for this data (measured by
  smoothness over all 20 such textures), and rows are stored bottom-up
  (text on the NYC helipad and in the sprite pages; confirmed by the cells).
- Identified, not yet decoded: DTPK sound banks (header verified on all 80),
  AET scenes, motion data, stage collision, per-character tables, the AICA
  ARM7 driver. Details in `docs/asset-formats.md`.

## 2026-10-21: sound banks

- **DTPK wave table decoded** (no public description found): offset, AICA
  format field (bits 23-24 above a 23-bit address), loop points, stereo flag,
  length; the samples tile all 80 banks. `tools/dtpk.py` writes 2,403 waves
  (ADPCM / PCM8 / PCM16, mono and stereo, loops in a `smpl` chunk) and 32
  continuous music streams.
- **Playback rate open**: written at 22,050 Hz. Voices fit about 22 kHz; the
  Final Tuned music is in tune at 22,050 Hz, the older music about 40 cents
  flat there, and a tone-record field (0x00F4 vs 0x1DFA) marks which.
  Settling it needs the driver's pitch code or an emulator log (Naomi 2 BIOS).
- Corrected readings on the way: bit 23 is part of the format field, not a
  loop flag; codes 0/2 are not the format on their own.

## 2026-10-21: playback rate from the driver

Read from `aicadrv.bin` (ARM7, hand-written; calls are
`stmdb sp!,{pc}; mov r0,r0; b target`), Ghidra project `work/ghidra_aica`.

| Driver code | What it does |
|-------------|--------------|
| 0x4314 | tone pitch `P = 256*(note-60) + 256*s8(tone[0xA]) + 4*s8(tone[0xB])` + track transpose/detune |
| 0x33DC | `P` to the AICA pitch register: 0xC00 per octave, FNS from the table at 0x8050 (`1024*(2^(v/3072)-1)`, checked) |
| 0x3A28-0x39D8 | tone lookup: a program is a 0x40-byte header then 64-byte tones |
| 0x4430 | wave lookup: tone +2 as u16; 0x100 + n is wave n of the bank, below 0x100 a driver built-in |
| 0x6BCC, 0x6EFC, 0x6F48 | section 5 voices: pitch is parameter 4, 0x180 per octave centred on 0xC00 |

- **Rates**: the VF1/VF2/VF4 music 32,010 Hz, the three Final Tuned tracks
  22,050 Hz, every voice 16,005 Hz, effects per tone (44,100, 41,625, 32,010,
  22,050, 16,005, 11,025 ... Hz). `tools/dtpk.py` writes each WAV at its rate.
- **Cross-checks**: the older music measured 40 cents flat at 22,050 Hz;
  32,010 Hz is 645 cents higher, so it lands within 5 cents of in tune. Voice F0
  at 16,005 Hz is 381 Hz for Aoi and Pai and 239-341 Hz for the men, normal for
  shouted speech (at 22,050 Hz the men would be 330-470 Hz). The earlier
  "voices fit 22 kHz" was a by-ear guess; the driver supersedes it.
- **Found a missed wave per bank**: the table's count is the last index, so
  every bank has one more wave (2,483, not 2,403); for music it is the final
  chunk of the stream. The samples tile all 80 files exactly from the table's
  true end.
- **A bit 25 is LPCTL**: A >> 16 is AICA channel register 0 (PCMS bits 7-8,
  LPCTL bit 9). 24 effects with loop points from 0 have it clear and are
  one-shots; the old heuristic had looped them.
- **Not settled**: 57 stage-ambience waves are used by no program in their own
  bank (`se_stage_sea` has none at all; `se_stage_harbor`'s first 13 waves
  match it). The driver takes waves through a bank pointer that can be another
  loaded bank, so their rate needs the stage's bank set; written at 22,050 Hz
  and marked `none` in `manifest.tsv`. Track transpose comes from the
  sequences at run time and is not applied.

## 2026-10-21: models

- **`mdl_` is Naomi 2 Elan display lists.** Each node of the `obj_` tree has
  a model block in `mdl_` (by node index): a 0x40-byte header (magic 0x100,
  bounding sphere, size) and an Elan command list of GMP materials and ICH
  vertex lists, in the layouts of Flycast's Elan emulation. All 14,315
  blocks walk to their exact end; `tools/mdl.py` writes 215 sets to OBJ +
  MTL + a node-tree JSON (3.84 M vertices, 2.68 M triangles).
- **Read from the loader**: `load_poly.c` 0x0C0E8540, `pol_uti.c` 0x0C0E7120
  (offsets to pointers), 0x0C0E6E60 (blocks copied unchanged to main RAM),
  0x0C0E87C0 (`mdl_` indexed by node number).
- **Checks**: GMP `tex0` indexes the set's texture descriptors (in range in
  all 215 sets) and the textures land right (skin, cloth, sky, building
  facades); the NYC helipad reads "AM2 R&D" from above, so nothing is
  mirrored; texture v needs no flip, which agrees with the bottom-up rows
  found for the textures.
- **Characters come out as parts.** The skinned `envelope` mesh is in body
  space, but each rigid piece is in its own bone's space and the node tree is
  flat; assembling them needs the skeleton (motion pass).
- 48 vertices store a NaN texture v in the data (written as 0).
- **In the decompile**: `obj_header`, `obj_node` and `mdl_block` types and 16
  loader functions named (`ghidra/symbols/poly.tsv`: `load_poly`,
  `obj_relocate`, `mdl_copy_step`, `model_size`, ...).
- **Fixed in ApplyTypes**: prototypes were applied with the C parser's
  "unknown" calling convention, which Ghidra treats as locked custom storage;
  every typed function (145) carried an "Unknown calling convention" warning.
  They now use the cspec default (`__stdcall`); the rest of `src/` is
  unchanged, and the two warnings left are the libgcc divide helpers, whose
  custom storage (result in FPUL) is real. A from-scratch `run.sh all`
  matches.

## 2026-10-21: skeletons and motion

- **Motion format decoded** (`tools/motion.py`): the directory in
  `motdat_EP.bin` gives each of 5,756 motions six streams in its bank
  (frames, bone/channel masks, key counts, times, 2-bit key codes, half-float
  values); keys are cubic Hermite with tangents, 30 frames a second. Decoded
  the way the loader does, every stream ends where the next begins.
- **Skeletons** (`tools/rig.py`): 13 body rigs of 47 bones in `main.bin`,
  chosen per character by the character record; an IK rig (aim bones, two-bone
  chains with law-of-cosines bends, targets in a global frame), emulated from
  `rob_eval_ik` and its helpers.
- **Skinning**: the envelope's skin groups and 162 bind matrices; the bind
  matrices' inverses confirm every slot-to-bone pairing.
- **Posed characters** (`tools/pose.py`): Akira's stance and Sarah's high kick
  render correctly from game data alone.
- **Approximate / not emulated**: ribbon, belt and cloth bones (`ex_*`) and
  the wrist slots (their vertices follow the nearest bone), motion blending,
  look-at, the face rig, `mothead_` / `smooth_` / `ctrl_`.
- **Decompile**: `mot_bank`, `bone_def`, `rig_entry`, `mot_channel` types and
  41 names (`ghidra/symbols/motion.tsv`): the motion loader and sampler, the
  rig evaluation, skinning and the matrix-stack helpers.
- **Found a decompiler bug**: `fipr` comes out wrong. `vec3_length`
  (0x0C12D9E0, `fipr fv0,fv0; fsqrt`) decompiles as a plain load, so the
  hand-written matrix code that uses `fipr` reads wrong. Fixed in the next pass (FPU vector and pair semantics); not fixed in this
  pass.

## 2026-10-21: FPU vector and pair semantics

Three modelling bugs in the SH-4 language, all from stock Ghidra and all in
`ghidra/lang/SuperH4_vf4ft/.../SuperH4.sinc` (marked `vf4ft`):

| Bug | Effect | Fix |
|-----|--------|-----|
| `fipr` / `ftrv` reached vector components through `*[register]` pointers (`&fv0 + 4k`) | the decompiler cannot follow register-space LOAD/STORE ("Restarted to delay deadcode elimination for space: register", 118 times); and as the little-endian layout swaps fr pairs (fr1 at 0x200, fr0 at 0x204), `&fv0 + 4k` walked fr1, fr0, fr3, fr2: `fipr` stored into fr2, not fr3, and `ftrv` computed P M P instead of M v | component tables (`FVN0..3`, `FVM0..3`) export the named registers |
| `frchg` only flipped FPSCR.FR | code that swaps banks and reads fr12 (the matrix translation column) showed `unaff_fr12` | swap the 16 fr / xf values |
| paired (SZ=1) `fmov` moved `dr` as one 8-byte value | every paired load / store swapped its two floats; `dr` <-> `xd` moves crossed them | word by word, lower address to the even register (as Flycast's `sh4_fpu.cpp`; tables `XDN_lo/hi`, `XDM_lo/hi`) |

Evidence: `vec3_length` (0x0C12D9E0) went from `return param[1]` to
`SQRT(x*x + y*y + z*z + 0.0)`. The vertex loop at 0x0C10BA4E loads a vertex as
two pairs and sets fr7 = 1.0 before `ftrv fv4`; before the fix it read
`(p[1], p[0], p[3], 1)`, now `(p[0], p[1], p[2], 1)`, and its constant store
writes 1.0 at +0 and 0 at +4 as `fldi1 fr2; fldi0 fr3; fmov dr2,@-r6` does.

| Metric | Before | After |
|--------|-------:|------:|
| decompile failures | 1 | 0 |
| "delay deadcode elimination for space: register" | 118 | 0 |
| `CONCAT44` (float pairs glued into 64-bit values) | 584 | 63 |
| functions whose decompile changed | | 195 (164 in the hand-written asm 0x0C100000-0x0C140000; the rest matrix helpers) |

- **Corrects an earlier note**: the PR pass said 0x0C10D740 failed from a
  delay-slot p-code shape "not the model being wrong". It was this bug: its
  `ftrv` sits in a `bf.s` delay slot, and with named registers it decompiles.
- **More FPU inputs show, honestly**: `unaff_fr12..15` 144 -> 498, `in_xf`
  2,908 -> 3,422. The asm routines take vectors in fv12 and the matrix in the
  xf bank; the old model hid those reads inside register-space loads.
- **Limit of C**: routines whose only effect is on the XMTRX bank (matrix
  normalise, 0x0C12CFD0) now decompile to an empty body, since register-only
  state has no C form.
- The project database keeps instruction prototypes from the language it
  was disassembled with, so a language change needs `run.sh all` (from
  import), not just `export`.

## 2026-10-21: glTF animation export

- **`tools/gltf.py`** writes each character set as one binary glTF 2.0 file:
  the 47-bone rig as joints, rigid pieces skinned 1:1, each envelope as its
  own skin with the game's bind matrices and weights, every motion of the
  character's bank baked per frame (glTF has no IK), PNG textures embedded.
  No libraries beyond numpy.
- **Checked three ways**: `tools/gltf_check.py` validates the structure and
  re-skins the file from its own data, matching `pose.py` to 1e-6 at every
  vertex (Akira, Sarah, mid-motion); three.js `GLTFLoader` loads and plays them
  with no warnings (`tools/viewer.html`); the shading check below.
- **Fixed in `mdl.py`: the strip triangulation took the third-vertex marker
  for a fan.** Strip 1 / fan 1 is a strip's third vertex (Flycast `isThird`);
  only strip 0 / fan 1 is a fan, and no character or stage list has one. The
  triangle set was unchanged (the fan triangle is the same three vertices) but
  every strip's winding parity flipped after its third vertex. With that
  fixed, winding follows the ISP cull mode exactly (opposite the normals in
  every cull 2 list, along them in every cull 3 list), which three.js
  needs: mixed winding on double-sided materials flipped the normals of half
  the triangles and lit the arms in patches.
- **All 65 character sets export** (19,229 animations, about 1 GB of .glb) and
  pass the structure check. Two fixes on the way: `pose.Character.envelope`
  inverted all 162 bind matrices, but past a 0x13 body's 123 slots the table
  is not bind matrices (chr_ao8 / ao9 hit a singular one), so only the slots
  in use are inverted; MOT, the test character, has no motion bank and gets
  the common one.
- **Fixed: vertex colors.** The envelopes are VUR lists whose colors are 0;
  writing them as `COLOR_0` turned the clothing black. The Elan (Flycast
  `setModelColors`) replaces the vertex color with the GMP diffuse when
  paramSelect bit 0 is set, as in every character list, so the exporter now
  follows that rule.

## 2026-10-21: cloth and swing bones

- **`tools/physics.py`** emulates the two simulations the game runs after the
  rig, read from the rob code (0x0C0D2120-0x0C0DCA00): the `ex_*` swing-bone
  chains (482 in the character sets: belt tails, sash ends, headband knots,
  hair) and the type 0x0F cloth grids (headband tails, scarves, robes). Their
  data (chains, parameter tables per character / costume / item, item
  records, cross-links, collision spheres and capsules, cloth anchors,
  particles and vertex maps) is decoded from the obj_ files and `main.bin`;
  both step at 60 Hz in `pose.py` and `gltf.py`, where the swing bones and
  cloth particles become extra joints. Stable on every set's longest motion.
- **Found and fixed: the translucent pass was missing from every model
  export.** A model block has four list slots; `mdl.py` read only the first
  (opaque). 5,224 of 14,315 blocks have translucent lists (hair, cloth,
  glass, foliage), 3,954 nothing else; Akira had looked bare-chested without
  his gi top. The exports now carry 4.83 M vertices, up from 3.84 M, and the
  "walk to their exact end" check now covers every slot. Two more vertex
  types appear there (0x42, 0x10A).
- **Corrected: slot 1 is the spine base `j_mune`, not the root.** Its bind
  matrix in `chr_je2` puts it at the hips with `t_mune`'s rotation; 14
  chains hang from it, and on the root they had trailed 1.8 m behind the body.
- **Fixed**: the item scan started at the wrong address, and the chain
  parameters are now one table per fighter, as in the game.
- **Fixed: slots come from a fixed table.** The fighter fills slots 1-22 from
  set bones whatever meshes a set has; the name-based mapping missed slot 4 in
  `chr_bo8` (no waki piece), so its slot-4 chains never ran. Every set's
  node names agree with the table. `rig.skin` also stops at the end of a
  short (123-slot) bind table.
- **All 65 sets re-exported** with the translucent lists and the physics:
  19,229 animations, 2.0 GB of .glb, every structure check passing.
- **Not emulated**: the stage wind, the debug offsets (0 as shipped), the
  assembly variant of the cloth step (taken to equal the C one), the exact
  cloth normal order, and the wrist slots (they follow the hand).

## 2026-10-21: faces and expressions

- **`tools/face.py`** decodes the faces from the rob code (names in
  `ghidra/symbols/face.tsv`). Each costume has a rigid face per expression
  (`t_kao_00..03`: neutral, after a voice cue, a held state, a blink) and a
  skinned face on a 33-bone face rig, from the character's costume table. The
  game draws the skinned face only while a face motion plays, and a move sets
  that with command 36 of its `mothead_` record: the `*_KAO` motions of the
  intros, win poses and Quest scenes, 259 in all. `motion.py` now records each
  move's face motion.
- **The face rig** has its own walk: two scale steps (10, then 0.01626),
  translations x 0.1, an offset per costume, and the eye bones copy their
  angles out. The skin is a third format (per-influence bind matrices, then
  static vertices copied over the result). At rest the skinned face matches
  the rigid neutral face to 0.005 mm for Akira; posed with `TOJYO_A` he closes
  his eyes and shouts.
- **Eyes**, which no export had before: two models per costume from a table
  at 0x0C2B0B1C, in the head frame, turned by the face motion. The game
  truncates the eye nodes' radian angles to 16-bit units, so their 90 and 180
  degree rotations do nothing; the tools do the same.
- **glTF**: the face rig joints, the skinned and rigid faces and the eyes hang
  under the head joint; each animation keys the face it does not show to scale
  0, as the game switches per move. `gltf_check` skips hidden vertices and
  matches `pose.py` within 1e-6 on frames with and without a face motion.
- **Fixed in the viewer**: three.js caches a skinned mesh's bounds from the
  first pose it draws and culled whole meshes (the head, hands, face) in close
  views; `viewer.html` no longer frustum-culls skinned meshes.
- **Found**: the move commands are two tables (52 run at the move start, 46
  timed by frame), and 77 of their 98 handlers were not functions yet; the
  face command is now one. Type 0x10 envelopes are not only Dural and XRAY:
  MUE's neck, Jeffry, Jacky, Kage and Lion costumes and some hats use them,
  and the tools leave them out (MUE shows a hole under the chin).
- **Not emulated**: the eye look-at (camera or opponent), the blink and the
  other rigid expressions at runtime (they are in the OBJ export).

## 2026-10-21: type 0x10 envelopes

- **Decoded**: `envelope_update` sends type 0x10 envelopes to their own skin,
  `envelope_skin_nouv` (0x0C0EA5E0): the type 0x12 format without u, v in the
  vertex records, the model keeping its own UVs. `rig.skin` reads both; all 24
  type 0x10 and 996 type 0x12 envelopes parse to their exact end and cover
  exactly their model's vertices.
- **Missing geometry restored** in 13 character sets: MUE's neck and arm
  wrappings, Jeffry's shoulders and chest, Jacky's jacket, Lion's trousers,
  Kage's top, Dural's and the XRAY bodies' skins. These are the main body
  envelopes of those costumes, not only Dural and XRAY as the node table had
  said.
- **Names**: `envelope_skin_nouv`, `envelope_skin_sq` (the type 0x12 skin
  through the SH-4 store queues, the path the game takes as shipped) and
  `mtx_mul_into`.
- **Found while checking**: five face skin groups (MUE, Jeffry, Wolf faces)
  blend 5 face bones; glTF viewers read 4, so those 1-4 vertices per face keep
  the largest 4 (up to 0.45 mm off), and `gltf.py` lists them.

## 2026-10-21: move records (mothead)

- **`tools/mothead.py`** decodes the 5,707 move records of the 21 banks to
  JSON (`work/moves/`): flags, the command list run at the move start (19,547
  commands), the timed list fired by frame (12,061) and the branch windows
  (925). The record is 20 bytes; its three lists, the timed entry format and
  the branch format are read from `move_start`, `move_timed_run` and
  `move_branch_try`.
- **98 handlers recovered as functions.** The two command tables (52 run at
  the start, 46 timed) were reached only through pointers, so 77 handlers were
  missing from the project; `ghidra/code_tables.tsv` now seeds every entry in
  the discover step. All 98 are named in `ghidra/symbols/move.tsv` with what
  they write: the attack (damage scaled by the attack rate), the frame marks
  and frame table, throw damage, damage by distance, sounds and voices,
  launches, hand shapes, the rigid face and eye look-at cues, the scripted
  camera, approach, limb targets, slot effects and the flag words.
- **Branches** are how strings and cancels work: a frame window, two input
  patterns (`input_patterns`, 54 entries) and a condition, then a script
  that picks the next move.
- **Not decoded yet**: that script, byte code for a 0x50-opcode interpreter
  (`move_script_exec`, handlers at 0x0C29F6CC); the meaning of many rob fields
  the commands set, which the JSON names by offset.

## 2026-10-21: the move script VM

- **The command input is a byte code**, and `tools/movescript.py`
  disassembles it: 80 opcodes (`move_script_ops`, 0x0C29F6CC: handler and
  instruction size), all named in `ghidra/symbols/script.tsv` (6 handlers were
  missing from the project and are now seeded by `code_tables.tsv`). It works
  like a backtracking regular expression over the 64-frame input history:
  input tests step back through the frames within a window, choice points
  (a 32-entry stack) give alternatives, and a move op ends the run with the
  move word.
- **`ctrl_<chr>.bin` decoded**: the per-character command lists, one control
  set per stance (29 sets in 17 files), each with six entry scripts (the
  neutral list among them) and a table. 17,056 instructions decode with no
  stray byte and no overlap.
- **Branch scripts decoded**: the branch windows' scripts are instruction
  pointer lists; all 925 end in a move or a null pointer and are written into
  the move JSON (`script_code`).
- **Still open**: which direction bit is which (the history's bits 4-7) and
  the condition table at 0x0C29F94C, so the listings show raw masks; the
  input object's remaining fields.

## 2026-10-21: input semantics

- **The input word decoded** (`input_sample`, 0x0C05C6C0): buttons G P K E in
  bits 0-3, raw directions in 4-7, and the forward / back and side-relative
  up / down bits the sampler adds from the facing and side flags. The button
  bits follow from the order of the hold counters (P, K, G, P+K, G+P, G+K,
  all); the directions from the moves (crouching moves after 2 / 3 / 1).
  The 0x28-byte history entry, `input_conditions` (16 held-for-N-frames
  conditions) and the 54 branch input patterns are decoded too.
- **The scripts print as commands**: `movescript.py` shows input masks in
  numpad notation and writes `commands_<chr>.txt` with the inputs on every
  path to every move: Akira's P, 2P and 4 6 6 P+K Tetsuzankō come out as the
  game's move list has them. The branch windows' inputs and conditions are
  in the move JSON as text.
- **Corrected**: three VM op names (`press` / `held` were the frame's input
  and the new presses, `dir_bit` the rob flag snapshot) and the branch
  condition, which needs at least, not at most, the value.
- **Still open**: the E button and the raw-horizontal bits in play; the
  move-word flags above the motion id; the paths list is a reconstruction.

## 2026-10-21: the rob fields of the move commands

- **Attack timing**: `frames` is the active window and the recovery end
  (`attack_active_flag` keys the attack flag on the window).
- **Attack data**: the level (high, mid, low by the game's own three tests;
  throws and down attacks), the attacking limb (the lean toward the opponent
  in `rob_attack_reach`), damage and stun flags.
- **The frame table is the hit reactions**: per guard / hit / counter hit and
  four more situations, a stun, a recovery end and a reaction move, with the
  game's defaults from the damage (`guard_stun`, `hit_stun`).
- **Frame data**: the game's training display computes frame advantage
  (`frame_advantage`, 0x0C0F0680); `mothead.py` applies the same formula to
  every attack and `movescript.py` writes `framedata_<bank>.tsv` with the
  commands. Akira's jab: 12 frames, +1 on guard, +5 on hit.
- **Still open**: the defender-state and 0x800 / 0x4000-0x10000 hit-result
  situations, the attack flags and bytes +3..+6, the floats; checking the
  numbers against the training display in an emulator.

## 2026-10-21: phantom arguments (r12 propagation)

- **Found**: a decompiled call argument changed between rebuilds. Ghidra's
  SuperH analyzer has an option, "Propagate constant R12" (on by default),
  meant for position-independent code that keeps a GOT pointer in r12: at
  every call where r12 holds a constant, it writes that constant as r12's
  value at the callee's entry. This game is not PIC; r12 is an ordinary
  callee-saved register, so 2,148 of 6,049 functions got a made-up entry
  value, the last caller analyzed winning (hence the run-to-run changes),
  and the decompiler treated it as known: calls to functions without a
  prototype showed invented constant arguments (`chara_data_get(id,
  &DAT_0c2926e4)`, `mtx_push(0, FUN_0c0cec80)`, `FUN_0c1597c0("%d")` in
  `mktime`).
- **Fixed**: `SetupNaomi2.java` turns the option off before analysis. 268
  phantom arguments disappear in 237 functions; checked call by call, every
  removed argument was the propagated r12 value, and no call gained one.
- **Fixed, a bug this exposed in our own script**: `AnnotateAsserts`
  required four arguments to `fatal_at`, so a plain fatal message (three)
  only counted when a phantom padded it. It now takes three: the assert
  table grows from 105 to 130 entries, none lost, no function changes file.
- **Verified**: two independent from-scratch builds with the fix give
  byte-identical `src/` and tables.
- **Still open**: calls that pass a caller's argument straight through to a
  function without a prototype show no argument (`mktime` calls
  `FUN_0c1597c0(t)`, shown as `()`); the fix is prototypes (Ghidra's
  Decompiler Parameter ID, or our signature tables). Four merged fatal calls
  in malloc.c and load.c (one call for two messages) stay unresolved, as
  before.

## 2026-10-21: prototypes for the unprototyped functions

- **New step `prototypes`** (`ghidra/AssignPrototypes.java`, between types
  and export): about 4,000 functions get a parameter list, so calls show
  the arguments a caller passes straight through (`mktime` now calls
  `FUN_0c1597c0(t)`, `FUN_0c08a180(param_1)`, `dimm_read(dst, off, 0x100)`).
- **Ghidra's Decompiler Parameter ID was tried first and rejected**: over
  `src/` it lost real arguments at 796 calls (a debug stub that ignores its
  color argument lost it at all 88 calls), invented fr8-fr11 parameters
  from code that saves them for its caller (spreading up the call graph),
  dropped 242 return values and multiplied the decompiler noise
  (`extraout_` 1,454 to 3,524, `CONCAT` 96 to 601).
- **The rules used instead**, from this compiler's measured convention: the
  body's inputs (saved registers excepted, widened to the r4-r7 / fr4-fr11
  prefix), or what at least half the call sites pass (written on every
  path to the call), whichever is more; stack parameters only as a short
  contiguous list; no returns committed. Left alone: hand-written assembly
  (functions not on GCC's 32-byte boundary, 146), variadic functions (14),
  frame-reading code, and functions with no parameters (1,755). Every
  decision is in `ghidra/prototypes.tsv`.
- **Checked**: against the committed source, 2,171 calls gain arguments and
  the decompiler noise is unchanged (`CONCAT`, `extraout_`, `in_stack`,
  `in_fr` equal; `unaff_` +14, `in_r` +22); 103 calls show fewer arguments, each where
  the callee's body does not read the dropped register, and some are
  corrections (`FUN_0c14a600(DAT_0c41e1b0)`, checked in the disassembly,
  had shown two wrong arguments); the assert table is unchanged; two
  independent builds give identical output.
- **Fixed**: `run.sh` now stops when a Ghidra script fails. Headless Ghidra
  exits 0 even then, and a compile error in one script stops all of
  `ghidra/` from loading, so the export step had deleted `src/` and
  written nothing.

## 2026-10-21: calls destroy the argument registers

An audit of the prototypes found two defects, both fixed.

- **The compiler spec let r4-r7 and fr4-fr11 survive calls.** Ghidra's
  stock SuperH4 spec leaves them out of the registers a call destroys; in
  this binary nothing preserves them (of the GCC functions writing r4 / r5
  / r6 / r7, 1 / 0 / 2 / 1 save it, against 3,222 of 3,226 for r8). So the
  decompiler carried a value across a call and read it as the caller's own
  input: `FUN_0c035140` calls `FUN_0c0913e0()` and then `FUN_0c03d9a0` with
  nothing in r4, and showed `FUN_0c03d9a0(param_1)`; it got a parameter it
  does not have, and its caller `intrude_disp` then passed `in_r4`. Now in
  `ghidra/lang/.../SuperH4_le.cspec`.
- **Assembly keeps its own conventions** (`ghidra/AssignCallEffects.java`,
  first in the prototypes step): libgcc's division helpers and the assembly
  libraries (Elan at 0x0C104000-0x0C10E000, the vector helpers near
  0x0C0DC500) hand values to their callers and to the next helper in those
  registers. A function without GCC's frame setup (`mov r15,r14`; 5,565 have
  it, the other 484 are those libraries, libgcc and one-instruction stubs)
  gets a convention that destroys exactly the argument registers it, or
  what it calls, writes: 432 functions, 55 conventions such as
  `__keeps_r6_r7_fr6_fr7_fr8_fr9_fr10_fr11` (`__udivsi3_i4`). Decisions in
  `ghidra/call_effects.tsv`.
- **The call-site rule counted scratch registers.** 18 of
  `FUN_0c14a640`'s 30 callers build a store pointer in r7 (`add r9,r7;
  mov.l r1,@r7`) and it got a fourth parameter. A register now counts at a
  call only if nothing reads its last write before the call (the call
  destroys it, so such a value has no other use).
- **Functions that run into non-code are skipped**: 10 functions fall
  through into a 4-byte pointer defined on top of code, already present
  after the types step (`FUN_0c04f720` ends after six pushes, before those
  of fr8-fr15 and every pop, so it seemed to read eight floats, and its
  callers inherited them). The data itself is not fixed yet.
- **Result**, against the previous prototypes: 3,945 functions committed
  (was 3,978; 6,952 integer and 795 float parameters, was 7,072 and 939),
  95 widened by their callers (was 136). Calls passing a register the
  caller never set (`in_r6` and the like): 8 to 1. Of the 139 registers
  callers add, every call site passes a value with no other use at 126, and
  most sites at the other 13 (79 were doubtful). `in_r` 236 to 218,
  `unaff_` 960 to 908. `extraout_` rose (1,454 to 4,648) because values the
  assembly returns in argument registers now show as what the call left
  instead of the value from before the call: traced to their calls, 1,423
  of 1,475 such values come from a callee that writes that register, 43
  from indirect calls, and 9 from GCC callees on paths where the value is
  unused.
- **Known limit**: the body's inputs can include a register read only on a
  path that cannot run (`FUN_0c0fd000` sets r7 when `r5 != -2` and reads it
  after a second, identical test, so it takes four parameters where its
  callers pass two); 12 calls in GCC code pass such unset values.

## 2026-10-21: code wins over guessed address tables

- **Cause of the ten cut-short functions**: Ghidra's Create Address Tables
  analyzer, during the import step's auto-analysis, marks runs of words
  that look like addresses as pointer tables, also in .text bytes nothing
  has disassembled yet. A function found later flows into one and
  disassembly stops ("conflicting data"): `FUN_0c04f720` ended after six
  pushes, at a 4-entry "table" made of the next ones. Of the 4,718 tables
  it makes, 9 (11 counting two reached only after the first was cleared)
  conflict with code; the rest are real (`.data`, `.rodata`, literal pools)
  and stay.
- **Fix**: `ghidra/FlowOverAddressTables.java`, after the discover step's
  analysis: where an instruction flows into one of the analyzer's tables in
  .text, the table is cleared, the code disassembled, the function body
  fixed and analysis continued, until none is left. All ten functions now
  disassemble to their end (no function falls through into non-code), and
  the prototypes step no longer skips any for it.
- **And a save rule**: `FUN_0c04f720` and `FUN_0c04f900` are hand-written
  (no frame setup, r14 a plain register) and save r8-r14 and fr8-fr15,
  leaving by `jmp @r0` to an epilogue outside their bodies, so the pushes
  of fr8-fr11 looked like eight float inputs. A register pushed at entry
  is now an input only when some pop goes into a register that was not
  pushed (`FUN_0c12d720` pushes fr4-fr6 and pops them into fr15);
  otherwise every entry push is a save. Only these two functions (and
  `FUN_0c06cac0`, which passed their floats on) changed: 4/0 and 4/1, what
  their callers pass.

## 2026-10-21: zlib 1.1.4

- **All of zlib 1.1.4 is named** (confidence: named), matched function by
  function against the public source (zlib.net/fossils/zlib-1.1.4.tar.gz,
  MD5 abc405d0bdd3ee22782d7aa20e440f08): 71 functions in
  0x0C151E80-0x0C157E9F, in this link order: compress, crc32, uncompr,
  deflate, trees, zutil, inflate, infblock, inftrees, infcodes, infutil,
  inffast, adler32, each file's functions in source order. The evidence
  per function is its call graph, strings and tables
  (`ghidra/symbols/zlib.tsv`). Four had never been disassembled because
  nothing calls them: `get_crc_table`, `zlibVersion`, `zError` and
  `_tr_tally` (deflate.c uses the `_tr_tally_lit` / `_tr_tally_dist`
  macros).
- **29 tables, found by their exact contents** parsed from the source:
  `crc_table`, `configuration_table`, the trees.c and trees.h tables, the
  inftrees.c / inffixed.h / infutil.c / infblock.c tables, the three tree
  descriptors, `z_errmsg`, `my_version` and both copyright strings.
- **Types** (`ghidra/types/zlib.h`): `z_stream`, `deflate_state`, the
  inflate states, `ct_data`, `inflate_huft`, `config`, with the original
  field names. The layouts are checked against the binary: each init
  function allocates its state's size (deflateInit2_ 0x16B8, inflateInit2_
  0x18, inflate_blocks_new 0x40, inflate_codes_new 0x1C), `_tr_tally`'s
  field offsets match, and the game passes 0x38, sizeof(z_stream), with
  "1.1.4". Both state structs are `struct internal_state` in the source
  (one per file); here they are `deflate_state` and `inflate_state`, and
  `z_stream.state` stays opaque, so code reaching the state through the
  stream (deflateInit2_) still shows offsets.
- **Source files for matched code**: `ghidra/known_files.tsv` maps address
  ranges to files (there are no assert strings in zlib); MapSourceFiles
  tags them `srcp:` and they export to `src/zlib/` as "file by match with
  source".
- **Fixed on the way**: functions created for a prototype row on bytes
  nothing had disassembled got a 1-byte body; ApplyTypes now disassembles
  first, which also gave six table-only mode handlers (`mode_warning` and
  others) their real 12-byte bodies.
- Named functions 74 to 145, named bytes 14,310 to 34,300.

## 2026-10-21: inputs no caller passes; the exception handlers' FPU mode

- **Parameters seen on paths that cannot run**: the decompiler can count a
  register as an input when the body reads it only on a path that cannot
  run (`FUN_0c0fd000` sets r7 when `r5 != -2` and reads it after a second,
  identical test), and the prototype then asks for a parameter no caller
  passes. A second phase of the prototypes step decompiles the GCC callers
  (the assembly hands real values over in argument registers, so its call
  sites are no evidence) and cuts the arguments past what the callers
  write when no caller computes a value there and either none passes
  anything, or one passes what an earlier call left (GCC never does that:
  `model_size` calls `FUN_0c0fd000` with the r6 and r7 its previous call
  destroyed). Wrappers then left with parameters they no longer use and no
  caller writes are cut too: `FUN_0c0fc9a0`, `FUN_0c0fca60`, `FUN_0c0fcca0`
  passed r6 and r7 through only because `FUN_0c0fd000` asked for them. 6
  functions cut (`ghidra/prototypes.tsv` says why); calls in GCC code
  passing a value the caller never set: 12 to 5, all of them to functions
  with stack parameters (left alone), except `FUN_0c145c20`'s, which reads
  r7 that its one caller does not pass and now shows it as `in_r7`.
- **The exception handlers' FPU mode**: `FUN_0c137316` and `FUN_0c13749e`
  (disassembled since the address-table fix) save the full context and set
  FPSCR with `mov #4,r0; shll16 r0; lds r0,fpscr`, so 0x40000 (SZ=0, PR=0)
  before pushing both banks of 16 single registers. AssignFpuContext now
  follows `shll`/`shll2`/`shll8`/`shll16` of a known constant; no FPU mode
  problems are left.

## 2026-10-21: SDK libraries: what was linked, and the first of them

- **What the SDK is, from the binary**: every Sega library in the game was
  built on 2004-04-05 between 17:08 and 17:14 (LIBNAOMI2 0.03, NAOMI
  LIBRARY 0.8, KamuiTL 1.10[R4], NLOBJPUT 0.99, libsnd 1.05, nlajamma 1.01
  and the sy* system modules), so one SDK build; the network kit (NLASOCK,
  NLADIMM, alAb) is from 2004-10-19. The sy* modules are a 1.x line, several
  marked `am_am2c` (syChain 1.50am_am2c, syTmr 1.04am_am2c ...).
- **How library code is identified here**: from the binary (its own
  strings, tables and behavior), checked against reference material that
  is not in this repo and never will be. Names are ours unless the binary
  itself carries them (version strings, assert paths, the self-test's
  `nlSin` / `nlAtan2` labels, libam's file names); the evidence notes say
  what was verified. Nothing from the references is copied here.
- **Hand-written assembly, byte for byte**: `cache_prefetch_range`
  (0x0C136700, syCache), `int_break_handler` (0x0C137160, installed in the
  VBR table) and `exception_entry` (0x0C137258, before the two exception
  handlers); the latter two had never been disassembled
  (`ghidra/symbols/sy_modules.tsv`).
- **nlajamma, the JAMMA / JVS I/O library, all of it** (36 functions,
  `ghidra/symbols/nlajamma.tsv`, `ghidra/types/nlajamma.h`, exported to
  `src/sdk/nlajamma/`): the request and read logic (`jvs_read_node`,
  `jvs_find_nodes`, `jvs_query_node`, `jvs_bus_setup` ...), the 14 reply
  handlers of `jvs_reply_handlers` (a 256-entry table indexed by JVS
  command code: `jvs_reply_switches` for 0x20, `jvs_reply_channels` for
  0x21-0x23 ...), credits and the per-frame I/O routine (`frame_io_update`), and
  the old (pre-JVS) I/O board. Four had never been disassembled because
  nothing calls them. The build shows a configuration without the hardware
  wait (`jvs_wait` is a 1,000,000-count loop) and without the printer
  commands; replies must carry the JVS sync byte 0xE0.
- **Data**: the node table (`jvs_nodes`, 0xAC bytes per node), the reply
  buffers, command buffers and error counters, 17 in all, their sizes
  confirmed by sitting back to back in .bss. Pairs whose order the
  compiler could have changed (two coin switch words, two per-chute
  arrays) stay unnamed.
- **libam, by call site**: the 14 transport and credit functions
  nlajamma calls (`jvs_send`, `jvs_get_reply`, `rom_dma_busy` ...) are named
  from where it calls them (`ghidra/symbols/nlam.tsv`); their bodies are
  not checked yet.
- The label for code mapped to a file this way is "file by match with
  source"; `ghidra/known_files.tsv` holds the ranges.

## 2026-10-22: the compiler is GCC 3.2.3, -O1 -ml -m4-single-only (zlib 71 of 71)

- **Method**: zlib 1.1.4 is public source the game's compiler also built,
  so it is an oracle. Each candidate GCC was built for sh-elf in a Debian
  3.1 container (`tools/toolchain/`: Dockerfile, `build-gcc.sh`), zlib
  compiled to assembly with it (`compile-zlib.sh`), assembled with the
  host's sh4-elf-as and compared function by function, relocations masked
  (`zlib_match.py`).
- **Note, 2026-11-10**: the tree is compiled with `-g` as well since
  that day (tools/match/match.py's FLAGS; the maintainer's ruling). It changes no
  instruction in any function that matched without it, and one function,
  vram_used_total_update, matches as plain C only with it. See the entry
  of 2026-11-10 on the ORIGINAL SHAPE audit.
- **Result**: stock GCC 3.2.3 with `-O1 -ml -m4-single-only` reproduces
  all 71 of zlib's functions byte for byte. The other releases fall away
  (3.2.2 70, 3.1.1 69, 3.3.3 23, 3.4.3 7; every
  `-O2` / `-Os` build 0-1), and about 80 extra flags were tried: those that
  change anything only lose matches (`-fomit-frame-pointer`,
  `-fno-delayed-branch` 0; `-fschedule-insns2`, `-fregmove`,
  `-fstrict-aliasing`, `-mdalign`, `-mrelax`, `-mhitachi` ...). `-O1` keeps
  the frame pointer on SH, which is where every function's `mov r15,r14`
  comes from. `-m4-single-only` rather than `-m4-single`: with it
  `deflateCopy`'s struct copies match too.
- **Declarations matter to the bytes**: for a while `zcfree` was the one
  miss (`free`'s address in r0 where the game has r1). GCC's build
  directory carries a placeholder `stdlib.h` that declares nothing, and it
  shadowed the stub headers, so `free` was undeclared (implicitly `int`);
  with the prototype `void free(void *)`, as the game's C library header
  had it, 3.2.3 picks r1 and the function matches. Reconstructed source
  has to declare what it calls the way the original did. (The stubs now
  come first, with `-I`; `-isystem` comes after GCC's own directory.)
- **For the reconstruction**: `-m4-single-only` makes `double` 32 bits,
  the same as `float`, so the game has no double precision at all; GCC
  3.0.4 (which cannot be built on a 64-bit host) was not needed.

## 2026-10-22: matching works; the game's own code is built with -fno-delayed-branch

- **`tools/match/match.py`** compiles a C file with GCC 3.2.3 at the
  game's flags (in the `vf4ft-gcc-build` container, from
  `build/toolchain/`), assembles it with sh4-elf-as and compares every
  function with the game: all code bytes, and every relocated word
  against what it should point at (a function or global by its current
  name from the exported `src/` and `vf4ft_globals.h`, or the address in a
  `FUN_` / `DAT_` name; a string literal must point at the same bytes in
  the game). A file can name extra flags in a `match-flags:` comment.
- **Two flag sets**: the game's own code (vf4lib, naomi2lib) leaves branch
  delay slots empty, 1.6 % filled over 20,000 branches, against 89-97 %
  in nlajamma and zlib, so it was built with `-fno-delayed-branch` on top
  of `-O1 -ml -m4-single-only`; the linked libraries without it. The few
  filled slots in game code turned out to be assembly (see "which code is
  built which way" below).
- **First matches** (`tools/match/tests/`, 10 of 10): from mode.c
  `mode_req_main` (its assert reproduces the path, line 451, format and
  expression text exactly; `#line` sets them), `mode_req_sub`,
  `mode_get_main`, `mode_get_sub`, `mode_exec`; from nlajamma
  `jvs_index_replies` (208 bytes, the indirect handler call),
  `jvs_set_main_node`, `jvs_wait`, `jvs_reply_reset`, `coin_state_clear`.

## 2026-10-22: the exported headers compile under GCC 3.2

- **One definition per type**: Ghidra's DataTypeWriter starts each file
  with its own copies of the basic types (`typedef unsigned char byte;` ...),
  which C before C11 rejects. They now live once, in a new
  `src/include/vf4ft_base.h`, and the writer's copies are dropped; every
  header has an include guard (GCC 3.2 calls `#pragma once` obsolete).
- **One enum member clash** among 1,682: zlib's private infcodes.c enum and
  a debug-table enum (`e_type_0c2b1010`: END, BALL, XY, YZ, ZX) both have
  `END`. In the original build they never met; our single header puts
  them together, so zlib's becomes `C_END`, as inflate.c's `BAD` became
  `I_BAD`.
- **Two-dimensional globals were declared backwards**:
  `jvs_reply_offset[31][256]` came out `[256][31]` (the exporter appended
  the outer count after the inner one). The Ghidra project was right; only
  the header writer was wrong. Found by compiling a matched function
  against the header.
- **Re-running the types step failed** (`tree_desc_s already exists`): on a
  re-run the C parser also finds last run's types in the program (a
  `struct tree_desc_s` use resolves there), so the scratch manager held a
  copy beside the header's new definition. The header's definition now
  replaces the copy (five zlib structs on a re-run); a fresh build never
  saw it.
- **The matching tests include `vf4ft.h`** instead of declaring types and
  named globals themselves, so a wrong declaration fails the compile; still
  10 of 10. The headers have no function prototypes and no unnamed `DAT_`
  globals yet, so those stay local.

## 2026-10-22: every function declared; variadic functions by their real prologue

- **`src/include/vf4ft_functions.h`** declares all 6,072 functions: a
  function with a row in `ghidra/symbols/` as the row says (it can hold
  what Ghidra drops: `const`, `...`), any other as the decompiler prints
  its signature. The decompiler no longer prints Ghidra's convention names
  (`__keeps_r4_r5...` is not C); a function with a measured convention
  says so in a `convention:` line of its comment instead.
- **Variadic functions, found by GCC's save area**: a frame (`mov
  r15,r14`), the unnamed int argument registers stored into it (r7 down),
  then the unnamed float ones from fr11 down, one by one through a plain
  `@rN`. Compiling `(fmt, ...)`, `(buf, n, fmt, ...)` and `(buf, float,
  float, ...)` with GCC 3.2.3 gives exactly that, so the stores also say
  how many arguments are named: fatal_at and snprintf 3, FUN_0c132640
  (printf-shaped) 1. The old test (six or more float stores) had 5 false
  positives among its 14: three assembly matrix stores writing through
  `@-r4` after `frchg` (0x0C12D060/80/A0), an assembly copy
  (0x0C1019E0) and a function copying its float arguments into a global in
  ascending order (0x0C1263A0). Those now get prototypes (one pointer;
  two pointers; 4 int, 8 float and 2 stack), and FUN_0c129900 is seen
  passing its argument on to 0x0C1019E0. 9 variadic functions remain;
  each is declared with its named arguments and `...`, and Ghidra keeps
  them unprototyped (a `...` prototype drops the register varargs at every
  call).
- **Standard C library prototypes** for the 11 library functions with rows
  (`size_t strlen(const char *s)`, `void *memcpy(void *dst, const void
  *src, size_t n)`, ...): GCC knows them as built-ins and warned at our
  looser versions; callers now show `size_t` results. `size_t` is
  `unsigned int` for sh-elf (`__SIZE_TYPE__`), defined for Ghidra's parser
  in `ghidra/types/libc.h`; the headers take it from the compiler's
  `<stddef.h>`. fatal_at has a row of its own (`ghidra/symbols/assert.tsv`).
- **A debug-table enum had a member `NULL`** (light types: OFF, NULL,
  PARALELL, POINT, SPOT), which `<stddef.h>` defines; ApplyEnums now treats
  the names our headers define as taken, so it is `TYPE_NULL` (the
  original string stays in its comment).
- **Checked**: a from-scratch build; `#include "vf4ft.h"` compiles under
  GCC 3.2.3 with `-Wall` and no warnings; the matching tests take every
  declaration from the headers (only two unnamed `DAT_` globals stay
  local) and still match 10 of 10. Of the exported functions, 72 changed
  beyond formatting, every one a caller of a function whose declaration
  changed.

## 2026-10-22: which code is built which way (delay slots)

- **`ghidra/ReportDelaySlots.java`** (export step) lists every branch with
  a delay slot and its slot instruction, as Ghidra sees the code, into
  `work/delay_slots.tsv`; `tools/match/delay_slots.py` sums it per file,
  lists the filled slots, and with `--groups` maps the address ranges built
  alike. The earlier 1.6 % came from a linear disassembly that read
  constant pools and jump tables as branches (mode.c's 15 "filled slots"
  were data).
- **The game's C never fills a delay slot.** Every named vf4lib and
  naomi2lib file has 0 filled slots. The 74 filled slots in game code are
  all in 38 hand-written assembly routines (no GCC frame setup; `fschg`,
  `ftrv xmtrx`, `pref`, the float bank). So `-fno-delayed-branch` holds for
  all of the game's C; no file was built differently.
- **The build map** (`delay_slots.py --groups`; C functions by their
  slots, assembly by the missing frame):

  | Range | Built | What |
  |---|---|---|
  | 0x0C0200C0-0x0C0FBDA0 | C, slots empty | the game (vf4lib), with small assembly routines inside |
  | 0x0C0FBEE0-0x0C1041A0 | C, slots filled | a library |
  | 0x0C104262-0x0C10DAF0 | assembly (136) | the Elan routines |
  | 0x0C10DBA0-0x0C114400 | C, slots filled | a library |
  | 0x0C114440-0x0C11B000 | C, slots empty | libam |
  | 0x0C11B180-0x0C11CEC0 | C, slots filled | nlajamma |
  | 0x0C11CF60-0x0C12C760 | C, slots empty | game-built (naomi2lib, the network code) |
  | 0x0C12C7CA-0x0C12DA90 | assembly (58) | matrix and vector routines |
  | 0x0C12DAC0-0x0C15D960 | C, slots filled, assembly runs | the system libraries, zlib, the C library |

- **libam is built like the game**, not like the libraries around it:
  `source/sdk/libam/libam.c` (then a test file) matches 6 of 6 (`rom_dma_busy`,
  `coin_meter_table`, `uptime_save`, `credit_backup_update`,
  `io_ready_check`, `frame_io_done`) with `-fno-delayed-branch`. Its two
  first misses were the source's shape: the clock read into a variable
  before the store, and `return 0; return 1;` rather than `== 0` (GCC
  makes a shift of one and `tst`/`movt` of the other). 16 of 16 in all.
- **The libraries with filled slots take the common flags**:
  12 functions (now in `source/sdk/naomi2_library/` and
  `source/sdk/lib_0c10dba0/`) match from
  0x0C0FBEE0-0x0C1041A0 (the binary's "NAOMI2 LIBRARY Ver 0.03 SOFT1 R&D")
  and 0x0C10DBA0-0x0C114400 with `-O1 -ml -m4-single-only` alone; 28 of 28
  in all tests. Of the 24 tried, the other 12 differ in the source's shape
  (block order, which load comes first), not in the flags: the slots are
  filled the same way. Some of those call through r0 where ours uses r1.
- **The call register says what the caller thought the callee returns**:
  GCC 3.2 loads a call's address into r0 when the callee is declared to
  return a value (or not declared at all, so implicitly `int`), and into r1
  when it is declared `void` (checked by compiling all three). Over the
  game, 5,932 calls through r0 go to functions the decompiler gives a
  return value and 930 to ones it calls `void`; 6,787 through r1 to `void`
  ones and 854 to value ones. The 930 point at 199 functions that really
  return a value nobody reads, and 275 callees are reached through r0 at
  some sites and another register at others: files that called them
  without a declaration.
- **Public hardware reference**: DragonMinded's libnaomi (a Naomi homebrew
  SDK, Unlicense) documents the clock `uptime_save` reads:
  0xA0710000 / 0xA0710004 hold the high and low 16 bits of a seconds count
  (0xA0710008 enables writes). Used for what registers mean, as evidence;
  none of its code is used here.
- **`match.py` points out a flag mismatch** on a DIFF: when the game's
  slots for that function disagree with the file's `-fno-delayed-branch`,
  it says how many the game fills.

## 2026-10-22: source/, the reconstructed C that matches

- **`source/`** holds hand-written C laid out like `src/` (one file per
  original source file; where it is not known, a file of our own naming).
  A function goes in only once it matches; `python3 -I
  tools/match/match.py` with no arguments checks the whole tree (28 of 28)
  and must pass before a commit. The matching tests moved there:
  `vf4lib/libbase/src/mode.c`, `sdk/nlajamma/{jvs,jvs_replies,credit}.c`,
  `sdk/libam/libam.c`, `sdk/naomi2_library/naomi2_library.c`,
  `sdk/lib_0c10dba0/lib_0c10dba0.c`. Rules in `source/README.md`.
- **The export uses it**: a function defined in `source/` is written into
  `src/` as that definition instead of the decompiler's, with a `matched:
  source/...` header line, and declared in `vf4ft_functions.h` exactly as
  defined; the export fails on a `source/` function the project has no
  name for. `src/` stays generated.
- **Progress counts it**: "Code rebuilt from matching C", from the
  `matched:` lines: 28 functions, 854 bytes (0.086 % of code).
- **A Docker mount recreated `src/` as root under a running export**:
  `-v` creates a missing host directory, so a compile run during the
  export step (which deletes `src/`) made `src/include` again, owned by
  root, and the export could not write. match.py now binds with
  `--mount`, which fails instead.

## 2026-10-22: the C library is newlib, about release 1.9.0

- **`tools/match/find_in_game.py`** compiles C or SH assembly with the
  game's compiler and searches the game for each function (relocations
  wildcarded); `tools/match/scan_newlib.sh <version>` runs it over a newlib
  release with newlib's own SH defines (`-DHAVE_GETTIMEOFDAY
  -DWANT_PRINTF_LONG_LONG`, from `configure.host`). newlib's sources stay
  in `build/` (README has the download); nothing from them is here.
- **Four releases compared** (functions found at exactly one address, of
  0x0C157EA0-0x0C15D960 and the stdio code before it):

  | newlib | functions | bytes |
  |---|---|---|
  | 1.9.0 (Dec 2000) | 52 | 9,730 |
  | 1.10.0 (Dec 2001) | 45 | 7,192 |
  | 1.11.0 (Jan 2003) | 38 (+ assembly) | 5,576 |
  | 1.12.0 (Dec 2003) | 39 (+ assembly) | 5,604 |

  1.9.0 finds everything 1.10.0 does and more (`__ieee754_sqrtf`,
  `floorf`, `scalbnf`, `isnanf`, `strftime`, `strrchr`). The SH assembly
  `memcpy`, `memset`, `strcmp` and `strcpy` match in every release; the
  game's `strlen` is newlib's C one, and the assembly `strlen.S` only
  arrived in June 2002. All of it compiled with `-O1 -ml
  -m4-single-only`, the common flags (newlib's default `-O2` does not
  match).
- **What is not newlib**: `malloc` and friends are the game's own debug
  allocator (vf4lib malloc.c), and the printf family differs from the
  start: newlib's `snprintf` builds a whole `FILE` on the stack (a 144-byte
  frame), the game's has a 56-byte one (the varargs save area). Its
  strings ("SNaN") are not newlib's either.
- **45 names** go in: 35 new rows in `ghidra/symbols/newlib.tsv` (string,
  ctype, float math kernels, `mktime`'s helpers, `strtok_r`, `strcmp`...;
  public functions get their standard prototypes, internal ones names
  only) and 10 existing rows upgraded to `named`, each with the releases
  it matches in. Every earlier guess at a library name held, except one:
  0x0C1595C0, called "strncmp", is newlib's `memcmp`, and the real
  `strncmp` is 0x0C15A780 (we had called it "strncmp_dir"). Three
  addresses stay unnamed because two or three newlib functions compile to
  the same code there: 0x0C1592E0 (`atoi` / `atol`), 0x0C15D580 (`atof` /
  `atoff`), 0x0C159E00 (`fprintf` / `fiprintf` / `fscanf`).

## 2026-10-22: return types from the call register

- **What the register says**: compiling each return type with GCC 3.2.3,
  a call loads the callee's address into r0 for char, short, int, long
  long and pointer returns (and for an undeclared callee, implicitly
  int), and into r1 for void, float, double and struct returns. So r0 is
  specific (an integer or pointer comes back) and r1 only says nothing
  comes back in r0. `tools/match/call_registers.py` counts it per callee;
  the known ones agree (fatal_at 112 of 113 through r1; memset, memcpy,
  strcpy, strlen through r0 even where the result is ignored).
- **`ghidra/AssignReturns.java`** (prototypes step, after
  AssignPrototypes) acts on it where every counted call agrees (at least
  two): an integer return the decompiler did not infer is set (37
  functions, e.g. the atoi/atol wrapper at 0x0C1592E0, which ends in a tail
  call, and FUN_0c101c20, whose result a caller uses); an integer return
  the callers rule out becomes void (1). Our own signatures and functions
  without a committed signature (379) are only reported, in
  `ghidra/returns.tsv`. **All 86 of our signatures that callers vote on
  agree with them.**
- **r1 is not proof of void**: the first run set 8 functions void, and
  their callers' code then showed `extraout_r0` 43 times: 7 of the 8 have
  callers that read r0 after the call. GCC takes r1 when r0 is busy with an
  argument after the address is loaded, so two r1 votes can agree by
  chance. A function is now set void only when no caller reads r0 after
  the call.
- **Effect**: placeholder variables 6,729 -> 6,735, all six `in_r0` in
  FUN_0c0fa1a0, which returns error codes (0xFFFFFF35...0xFFFFFF9C) on four
  paths and leaves r0 as it was on the fifth; before, it showed as void and
  the codes were lost. Headers compile; source/ still 28 of 28.
- The `## Next` heading below had been lost in the previous commit (the
  newlib entry replaced it), so the progress page showed no next steps.

## 2026-10-22: calls without a declaration? Not in the mapped files

- **Within a real source file the register is consistent**: of 961
  (callee, mapped file) pairs, 950 use one register throughout, as one
  declaration per file would. The callees reached through both registers
  (134) split between files, and an `_unmapped/` chunk is several files,
  so where the split falls inside one there is no telling yet which file
  lacked a declaration.
- **The other 1 % is the compiler, shown in code we have the source
  of**: zlib's deflate.h declares `_tr_flush_block` void, and
  `deflate_stored` calls it three times through one macro, yet the game's
  (matching) copy loads the address into r0 once and r1 twice: the macro
  computes a conditional argument at that point, and the allocator takes
  r0. So the register is a strong hint, not proof, in both directions.
- **r0 busy between the load and the call** (an r1 vote GCC was forced
  into) is now left out of AssignReturns' count; it explains only 36
  calls, so it is not where the noise comes from.
- No mapped file shows a call without a declaration; the 11 inconsistent
  pairs look like the zlib case.

## 2026-10-22: newlib 1.8.2; placing helpers by their callers

- **The C library is newlib 1.8.2** (November 1999). With the 1.8.x
  releases downloaded (sourceware's md5 list checked), 1.8.2 finds
  everything 1.9.0 does and `__ieee754_asinf` besides (536 bytes; the
  binary's "asinf" string is its error name): 53 functions, 10,266 bytes.
  1.8.0 and 1.8.1 have no SH assembly `strcmp` / `strcpy`, which the game
  has byte for byte, so they are out. The symbol rows now list every
  release each function matches in; the README's download is 1.8.2.
- **MapSourceFiles places a function by its callers**: an untagged
  function right next to a file's run, every reference to which is a call
  from that file, goes in that file ("file by callers"; a static helper can
  only be called from its own file, and the linker keeps a file together).
  Repeated, with the link-order gap fill, until nothing changes; a function
  referenced from data stays unplaced. 28 functions, 2,686 bytes: helpers
  at the edges of mode.c (9), network.c (11), user_back.c, sound.c,
  sound_cmn.c, sol.c, aetset.c, pol_uti.c and malloc.c.
- **Why most of malloc.c is still unplaced**: its last assert-tagged
  function is at 0x0C0F3F80; `realloc_internal`, `malloc`, `free` and
  `calloc` after it are called from all over, and the small helpers before
  it (`size_to_units`, `align32`, ...) are also called from
  `realloc_internal`, so not from placed code only. That needs another
  signal (where an object file ends).

## 2026-10-22: string literals are laid out in object order

- **The test**: for each placed file, the string literals only that file
  uses. In code order the files' string runs follow one another without
  overlapping: system_common.c 0x0C218634-0x0C218684, cns_nickname.c
  0x0C2186AC-..., replay.c, mode.c 0x0C218B04-0x0C218DD8, console.c, ...
  malloc.c 0x0C241758-0x0C241884, load.c, gz.c, the network library, zlib.
  (Strings several files use are stored once, so they are left out; other
  read-only data and `.data` do not keep the order as cleanly.)
- **MapSourceFiles uses it**, as conservatively as the callers rule: an
  unplaced function next to a file's run, whose every string lies inside
  that file's strings (the nearest placed strings below and above are both
  the file's), goes in it ("file by strings"). **It places one function**
  (154 bytes): a file's unplaced neighbours sit at the edges of its code,
  so their strings sit at the edges of its strings, in the gap before the
  next file's, which could equally be an unplaced file's. The rule stays:
  it is right, and it places more as other evidence fills files in.

## 2026-10-22: the decompiler's own output, compiled: 1,305 functions match

- **`src/include/vf4ft_data.h`** declares the 6,487 unnamed globals the
  exported code uses (`DAT_...`, `PTR_...`), as Ghidra types them, so
  decompiler output compiles. It is not part of `vf4ft.h`: `source/`
  files that type a global better (an array, a pointer, `int`) would
  conflict, and those types now go into Ghidra instead (7 data rows in
  `nlajamma.tsv` / `nlam.tsv`, from the matching C; applied at the next
  build).
- **`tools/match/auto_match.py`** compiles every unmatched decompiled
  function with the game's compiler (flags by its build group, assembly
  and decompiler-only constructs left out, batches split until the one
  that does not compile is found) and checks each with match.py's
  `run_checks`, now a function. **First run: 1,277 functions, 48,066
  bytes, compile from the decompiler's output to the game's exact bytes**;
  2,516 compile but differ, 672 do not compile, the rest were left out
  (705 for decompiler warnings, which a second pass now tries: a warning
  is no reason to skip, since a function whose decompile lost something
  just does not match).
- **`tools/match/adopt.py`** moves the matches into `source/` (signature on
  one line; a file per known source file, else
  `source/_unmapped/<build group start>.c`), compiling and checking each
  file it writes and leaving out what does not compile or match there (none
  did). `source/` now holds **1,305 functions in 64 files, every one
  matching**; the export takes all 1,305 from it, the headers still
  compile, and "code rebuilt from matching C" is 4.92 % (was 0.086 %).
- The seven typed rows are applied: `DAT_0c5cb000` is `int[4]` in
  `vf4ft_globals.h`, and the hand-written files dropped their own copies.

## 2026-10-22: object layouts from base + offset addressing

- **Why many near-misses differ**: GCC 3.2 at `-O1` loads one pool word
  per object and reaches its members as base + offset, while separate
  globals each get their own pool word. FUN_0c02d320 stores two bytes
  through one base, 0x0C370CE0, at +114 and +115; the decompiler shows
  two globals, and its C loads two addresses. Written against one object
  at 0x0C370CE0 it matches. Of the 2,516 near-misses, 461 load such a base
  (some of those are a different case, a call to another function).
- **`ghidra/AssignAggregates.java`** (types step) follows every pool load
  of a data address through straight-line code (copies, `add #imm`) and
  records each access through it with its size; an address accessed at
  positive offsets starts an object reaching that far (overlapping ones
  merge; addresses accessed directly inside become fields). Each gets a
  structure `agg_<address>` with fields `field_0x..` (our names: a layout,
  not the original type) unless the range holds named or typed globals or
  lies inside a typed object (`&jvs_reply_offset[k]` folds to one address
  inside that array: the first version overwrote the array, now refused).
  348 objects made, 21 left alone; `ghidra/aggregates.tsv`. Untyped pointer
  members are written `pointer`, now `void *` in `vf4ft_base.h`.
- **source/ no longer depends on Ghidra's placeholder names**: a `PTR_` name
  follows the label of what it points to, and a global that becomes part of
  an object loses its `DAT_` label, so type changes renamed what matching C
  used. `tools/match/pin_globals.py` gives every `source/` file its own
  declarations of the unnamed globals it uses, as `DAT_<address>` (match.py
  reads the address from the name) with the type they had when the file
  matched; adopt.py does the same for new files. After the rebuild all
  1,306 functions still match.
- **The retry with the layouts**: of the 3,188 functions that differed or
  did not compile, 89 now match (3,610 bytes); `pin_globals` keeps an
  object's `agg_<address>_<address>` label as it is. `source/` holds
  **1,395 functions, all matching**; code rebuilt from matching C 5.29 %.
  2,317 still differ and 781 do not compile (109 more than before).
- **Those 109**: Ghidra's header writer names an undefined filler byte of a
  structure `field9_0x9`, its decompiler `field_0x9`; the exported headers
  now use the decompiler's name (one member per offset, so no clash).
  Retrying the 781: 68 compile now, one more matches (1,396 in all).

## 2026-10-22: returns from callers that read r0; 1,448 functions match

- **AssignReturns** now counts a caller that reads r0 after the call as a
  value on its own (any number of calls), gives a value to a function
  without a committed signature when the decompiler finds it no
  parameters, and passes a value on through a function that returns a
  call's result. 93 functions get a return (was 37). FUN_0c130d40 had
  matched as `void` (GCC makes the same bytes for `void f(){ ...; g(x); }`
  and `int f(){ ...; return g(x); }`), but 65 callers read its result:
  it now returns `FUN_0c130ce0(...)`, and 107 uses of it compile.
- **`tools/match/unadopt.py`** takes functions whose return changed back out
  of `source/` (38) so their new decompile is matched again; two in files
  with hand-written code are left as they are (FUN_0c10df00,
  FUN_0c029460).
- **vf4ft_data.h** declares the names the code uses at any address, not
  only main RAM (109 functions used one outside it).
- **A regression of mine, fixed**: with warnings no longer a reason to skip,
  a decompile starting with "/* WARNING: Removing unreachable block ..." was
  named "block" by auto_match (583 functions); leading comments are now
  dropped, and adopt.py skips a definition it cannot read instead of
  stopping.
- **Result**: 86 more adopted; `source/` holds **1,448 functions, all
  matching**.

## 2026-10-22: rewriting near misses (tools/match/variants.py)

- **The decompiler's C is often right in meaning but not in shape.**
  FUN_0c09e640 stores a call's result into a two-player stack array and
  passes it on; the decompiler writes `v = f(i); a[i] = v; ... v + 0x1c`
  with `a[3]`. The game computes the element's address before the call and
  has an 8-byte frame: written `a[i] = f(i); return g(a[i] + 0x1c);` with
  `int a[2]` it matches.
- **`tools/match/variants.py`** tries rewrites of each near miss, up to
  three together (store into the target directly, return directly, inline
  a temporary used once, smaller stack arrays), compiles the variants in
  batches under address names, and keeps one that matches (bytes and
  relocations); `adopt.py --texts` moves those into source/. Over the
  3,213 near misses, 16,735 variants: **46 match**, 45 adopted.
- **Three more rewrites** (a local's or parameter's signedness flipped,
  a comparison mirrored, a one-level if/else swapped with the condition
  negated): over the 3,167 left, 90,581 variants, **30 match**, 29
  adopted. The if/else swap won 22 of them (GCC 3.2 lays out the `then`
  block first, so the order of the two blocks in the source decides the
  branch), signedness 4, comparisons 2, combinations 2.
- **The swap at any nesting level** (blocks found by brace matching):
  95,799 variants over the 3,137 left, **5 match**. The rewrites of this
  kind are running out; what is left needs other evidence.
- `source/` holds **1,527 functions, all matching** (6.12 % of the code).

## 2026-10-22: object members named by the debug menu

- **The debug menu names single members of objects** the code reaches as
  base + offset: 0x0C370CE0 holds `cam_rob_no` and three more, 0x0C3744E8
  thirteen colour components (`win_color`...), 0x0C3AD21C twelve (`wind_bias`...).
  AssignAggregates now makes those objects too: each named scalar inside
  becomes a member with its name and type (Sega's labels, from the binary).
  An object whose start is named, or holding a named array or struct, is
  still left alone. 354 objects (6 with named members), 15 left alone.
- **Headers**: a named global now inside an object is declared on its own,
  typed as its member, so code using the name compiles as it did; the types
  header no longer repeats an enum a member uses (vf4ft_enums.h has it,
  sized variants included) and names it by its typedef (`e_off_on_8`, a
  typedef of unsigned char, has no enum tag). All 1,527 still match; a
  retry against the named objects adds 4 (**1,531**).
- **Calls through a code pointer** (`(*(code *)p)(...)`, 285 functions):
  Ghidra's `code` was `void`, which C cannot call; it is now
  `typedef int code();` (a function with any arguments), and the 285 are
  tried: 17 match (**1,548**).

## 2026-10-22: test.bin, and a permuter

- **`ghidra/run.sh test`** imports `work/test.bin` (the test-mode program,
  512 KB at 0x0C020000) as a second program in the same project:
  SetupNaomi2 (RAM above the program now starts where the program ends),
  FindPrologues and FlowOverAddressTables (their `.text` range is the whole
  program for anything but main.bin; main.bin's code tables are skipped),
  then **`ghidra/TransferFromMain.java`**: a main.bin function whose bytes up
  to its first return occur once in each program is the same function, and
  gets main.bin's name, signature and comment; the pool words of each pair
  map main.bin's named globals to test.bin's addresses. **1,062 functions
  paired** (64 created), 102 named, 88 signatures, 31 globals: zlib, the
  JVS / credit I/O code, the DIMM file system (`sffs_dir`, `dimm_files`),
  math helpers, the C library (`ghidra/test_transfer.tsv`).
- **`tools/match/permute.py`**: random search on the closest near misses.
  Distance = edit distance over instruction words, a relocated word equal
  only when it holds the right address; each round mutates the best text
  so far (variants.py's rewrites plus commuted operands, swapped
  statements, other integer types, `!= 0` added or dropped) and keeps the
  closest. The trial matched FUN_0c069c20 (distance 1 -> 0). `match.py`
  takes `VF4FT_INC=<dir>`, so a long run compiles against a frozen copy of
  the headers and survives exports. A run over the 400 closest (1 to 5
  words off) is going.
- **`src_test/`**: the same step maps test.bin's functions to files (its
  own assert paths: system_common.c, user_back.c, card_common.c, tm_sys.c,
  the network library) and exports **1,962 functions** in 15 files; 30
  come from source/ (the same functions, already matched), and the C
  library rows are found by name there. Its headers compile clean. What is
  test.bin's own: the test menus ("INPUT TEST", "GAME ASSIGNMENTS", "GAME
  BOOKKEEPING", "CARD MENU", "VS DIAGRAM").
- **A mistake in the first src_test/ (commit bda2071)**: it took 80
  functions from source/ by name, 49 of them wrongly. Both programs load at
  0x0C020000, so a placeholder name like `FUN_0c021380` means a different
  function in each; main.bin's matched C went under unrelated test.bin
  functions. Placeholder names are no longer looked up for test.bin.
- **test.bin's own names**: `ghidra/symbols_test/` holds its rows
  (ApplyTypes reads it for test.bin); 21 screens of the test menu named
  from their on-screen titles (`test_menu` "VF4 FINAL TUNED GAME TEST
  MENU", `input_test`, `game_assignments`, `backup_data_clear`,
  `vs_diagram`, `network_test`, `card_menu`, ...).

## 2026-10-22: names audited for publishing

- **Library and module names**: every one the repo uses is in the
  binary's own strings (nlajamma, libam / nlam, KamuiTL, NAOMI2 LIBRARY,
  NLOBJPUT, libsnd, the sy* modules with their versions) except one, a name
  for the system library family that only reference material uses; the
  symbol table that carried it is now `ghidra/symbols/sy_modules.tsv`.
- **Every symbol row** (686): 523 our own snake_case, 146 from public code
  (zlib, newlib, libgcc), 10 strings in the binary, 7 placeholders; no
  SDK-style identifier.
- **Names too close to the reference names**: about 20 of the nlajamma /
  libam names followed the reference material's word choice (several were
  its names without the prefix). They are renamed after what each does in
  the binary: `coin_state_clear`, `coin_input_update`, `coin_meter_table`,
  `io_ready_check`, `jvs_bus_setup`, `jvs_frame_command`,
  `jvs_set_frame_command`, `jvs_main_node`, `jvs_set_main_node`,
  `jvs_reply_dac`, `jvs_node_desc`, `nlajamma_version_string`,
  `backup_flush`, `uptime_save`, `frame_io_update`, `frame_io_done`, and
  `legacy_io_*` (with the file `sdk/nlajamma/legacy_io.c`). All 1,548
  matching functions still match; src/ and src_test/ carry the new names.

## 2026-10-22: matched C for the test program

- **The matching tools take `VF4FT_PROGRAM=test`** (`tools/match/program.py`:
  image, src/ or src_test/, source/ or source_test/, the delay-slot file);
  main.bin stays the default. The test step reports test.bin's delay slots:
  its own C, from 0x0C0200C0, leaves them empty like the game's, the
  libraries after it fill them.
- **auto_match on test.bin**: **42 functions match** (the network library
  and the test program's own code), adopted into `source_test/`; src_test/
  takes 72 functions from matched C (30 shared with the game, 42 its own).
  1,271 differ, 249 do not compile: the same shape as main.bin's first run.
  variants.py on those 1,271 (46,414 variants): 4 more, **46** in source_test/.
- **Permuter results so far** (8 shards over the 800 closest near misses,
  8 over the next 1,200; variants.py's rewrites plus early returns and the
  function's return type, added after a look at the distance-1 cases):
  39 match, adopted (renamed to the audited names first, since these runs
  started before the rename): main.bin's `source/` holds **1,587**.
- **Tier 1 done** (800 closest): 49 permuter matches in all, 11 more
  adopted; **variants.py with early returns and return types** over all
  3,295 near misses (127,902 variants): 44 match, most already found by the
  permuter, 8 more adopted. main.bin's `source/` holds **1,606**.

## 2026-10-23: the compiler without a container; near misses by kind

- **GCC runs on the host** (`match.run_gcc`): the 3.2.3 build is a 32-bit
  Linux program and runs with the host's 32-bit glibc, byte for byte as in
  the image. A container per compile had cost ~0.7 s against ~0.1 s of
  compiling, and dockerd was the bottleneck; match.py over source/ went
  from 66 s to 3 s, the permuter ~65x. A deep permuter pass (all near
  misses, 100 rounds of 24 mutants) then found only 4: random edits are
  spent.
- **`tools/match/classify.py`** compiles every near miss as it stands
  and names how it differs from the game (pools and alignment nops
  dropped): of 3,232, 902 branches, 804 mixed, 606 a call more or fewer,
  561 the frame, 136 order only, 120 extensions, 59 registers only, 35
  the call register.
- **Loops**: Ghidra writes every loop as a guarded do-while; GCC 3.2
  tests a for loop's counter against the bound (`cmp/ge r5,r0` with the
  counter in r0) where the guard compiles to `cmp/pl r5`. variants.py's
  `loop` rule rewrites to for and while: 19 matches.
- **Byte and short signedness**: sh4 loads sign-extend, so GCC adds
  `extu.b`/`extu.w` only for unsigned types; Ghidra types unnamed globals
  unsigned. Reading one as signed (`castsign`) matched 36.
- **Call registers** (r0 when the callee returns a value or is
  undeclared, r1 when void): a callee declared `undefined8` is called
  through r0, not r1 as AssignReturns assumed; `mtx_store` is called
  through r0 45 times and r1 6 times, so some files lacked its prototype.
- variants.py now runs in parallel and puts the targeted rules first: 65
  match over all near misses, 62 adopted. main.bin's `source/` holds
  **1,673**.

## 2026-10-23: signed bytes and halfwords; 8-byte returns

- **`ghidra/AssignSigns.java`** (types step): at each mov.b / mov.w load
  of a global, how the value is used votes: extended with extu.b / extu.w
  is unsigned; used at full width without it (arithmetic, a register
  compare, a shift, a call argument, the return value, a 32-bit store) is
  signed; tst, masks and same-size stores do not vote. Signed with no
  unsigned vote: **83 globals typed char, 119 short** (99 unsigned, 20
  mixed, left alone; members of objects are not touched). They keep their
  `DAT_<address>` names (an analysis label: Ghidra names typed data from
  its value, `CHAR_00h_...`), which ExportSource treats as unnamed.
- **AssignReturns**: callers all through r1 while Ghidra infers an 8-byte
  return: `undefined8` is a long long, which GCC returns in r0/r1 and
  calls through r0, so these were counted as agreeing by mistake.
  Functions without a committed signature now get the decompiler's
  parameters committed with their return (variadic ones only reported):
  11 set void (FUN_0c12d220, FUN_0c12d260, mtx_rot_z_s16 ...).
- **Re-running a step on a built project was not safe**: AssignAggregates
  rebuilt its objects with the named members' and pointer members' types
  lost (`pers_req` became a byte, and a matched function broke). It now
  reads them from the object made last time. A full `ghidra/run.sh all`
  takes 3 minutes; src/ from it differs from the previous export only in
  the 192 retyped globals.
- **ExportSource** declares a `DAT_<address>` name source/ uses for data
  Ghidra labels otherwise (`PTR_LAB_...`), so src/ compiles where matched
  code spells a pointer table by its address.
- **adopt.py**: a function that fails only because its file pins a global
  with the old unsigned type is tried with the global declared as it is
  now, and kept if the whole file still matches.
- **Result**: 48 functions match straight from the new export (25 exact,
  23 with an unverifiable relocation, left), 25 adopted, plus 3 from
  variants.py: main.bin's `source/` holds **1,701**.

## 2026-10-23: bitfields, and why decompiled C did not compile

- **The game's structs use C bitfields.** Ghidra shows a bitfield store as
  `x = x & 0xfffffff8 | v & 7`; GCC compiles that loading the word first,
  and a real bitfield computes the value first, which is the game's order.
  variants.py's `bitfield` rule writes the store through a struct with
  unnamed padding (`((struct { unsigned int : 16; unsigned int f : 5; }
  *)(p + 0x38))->f = v`, little-endian: the first field is the low bits):
  12 matches. A statement-swap rule (`swap`) found 1 in 273,099 variants:
  the "order" near misses are not statement order.
- **`tools/match/triage.py`** groups the functions whose decompiled C does
  not compile by their first error. Of 525: 125 void values (79 untyped
  pointer globals exported as `void *`, 53 a void callee's result used),
  86 `X._56_4_` (the decompiler's notation for bytes at an offset of an
  object with no field there), 80 too many arguments (38 to one assembly
  routine the decompiler saw no parameters for), 79 a 4-byte global
  dereferenced, 69 undeclared names (`&UNK_ffffa880`, a constant taken for
  an address; `LAB_`/`SUB_` code addresses).
- **Fixes**: an untyped pointer exports as `undefined *` (the decompiler
  indexes it in bytes); `X._N_S_` and `&UNK_x` are written as C;
  `LAB_`/`SUB_` names a function takes as values are declared (a goto's
  label is not), and match.py reads P1/P2 mirror addresses (`ac136140`)
  from names; `mtx_translate(x, y, z)` (assembly: ftrv of (x, y, z, 1)
  into the matrix's translation row) has its signature.
  **`ghidra/AssignPointers.java`** (types step): a global whose loaded
  value is used as an address becomes `undefined *`: 68.
  **AssignReturns** commits the decompiler's signature where the callers
  agree with it (left uncommitted it can change by export: FUN_0c114860
  was exported void with 14 callers reading r0), but not a return
  narrower than a register: committed, a bool return made 110 callers
  decompile to CONCAT31.
- Not compiling: 525 -> 189; near misses 3,171 -> 3,430. 40 functions
  matched straight from the exports, 18 from rewrites: main.bin's
  `source/` holds **1,759**.
- Left: a void callee's result used (55: callees whose decompile ends in
  an endless loop, float returns the call votes do not see, callers built
  without a prototype), too many arguments to compiler helpers
  (FUN_0c1591e8, FUN_0c159212), NAN constants.

## 2026-10-23: division helpers and hardware registers

- **Register placeholders** (284 functions auto_match skips): the most
  common was `extraout_r3` (58 functions), after `__sdivsi3_i4` and
  `__udivsi3_i4`. GCC knows those libgcc helpers leave r1-r3 alone and
  keeps values there across a division; AssignCallEffects only weighed
  the argument registers, so a call destroyed r0-r3 as usual. It now
  weighs r1-r3 and fr1-fr3 too (r0 and fr0 carry results): 428 assembly
  functions keep some scratch registers, 132 conventions. extraout_r3
  uses in src/: 126 -> 37. The export writes the helpers' calls as the
  `/` they compile from (`((int)(a) / (int)(b))`).
- **`_DAT_<address>`** is the decompiler's name for an access of another
  type than the data there: 196 of them, mostly hardware registers
  (a05f... the graphics chip through the uncached mirror, e0... / e1...
  the CPU's control registers). auto_match skipped them (107 functions).
  The export now declares each with the access's type (from the p-code
  varnode at that address), `volatile` outside main RAM, as a polling
  loop needs; match.py reads any 8-digit address from a name, and
  pin_globals keeps a `_DAT_` name as it is (another type than the
  `DAT_` at the same address).
- 37 functions match straight from the export (the unverified-relocation
  ones among them: their hardware addresses now resolve), 8 more from
  variants.py: main.bin's
  `source/` holds **1,804**. Register placeholders left: 263 (`in_xf*`,
  the FPU back bank, in 57: assembly anyway; then unaff_r11, in_T,
  extraout_r4/r0/r6 around the Elan library, no one cause).

## 2026-10-23: float globals; what the extra calls were

- **`tools/match/callees.py`** lists, for the near misses of one kind, the
  functions the game calls and our compile does not, and the reverse. Of
  the 681 that differ by a call, 127 called `__fixunssfsi` (GCC's float to
  unsigned conversion) where the game never does.
- Two spellings caused it. `(uint)fVar1`: the decompiler casts to the
  destination's type, the game converts with ftrc (to int); the export now
  writes `(int)` for an unsigned cast of a float operand. And float
  globals typed `undefined4`, an integer to C, so storing a float into
  one converted it (`DAT_0c489444 = DAT_0c489444 + 0.5`).
  **`ghidra/AssignFloats.java`** (types step): a global the FPU moves
  (fmov to or from it) with no byte, halfword or pointer use becomes a
  float: **345**; AssignAggregates lets an fmov access make a member a
  float even when a word move (a copy) saw it first. 39 more functions
  match; 66 near misses still call the helper (float returns the call
  votes do not see, members of objects).
- **Bitfield reads** (`*(uint *)(p + 0x38) >> 0x10 & 0x1f` as a field)
  compile to the same code as the shift and mask in 39 of 40 cases: only
  stores tell a bitfield from a mask. variants.py's `bitread` rule stays
  (it reads better) but finds nothing.
- The rest of the call differences: TrapAlways (the decompiler's `trapa`)
  and fatal_at in 14 functions each, mtx_pop in 22, and calls whose
  address sits in a register across calls (not compared).
- main.bin's `source/` holds **1,844**.

## 2026-10-23: members typed in their objects; float returns; frames

- **Members of objects**: AssignSigns, AssignPointers and AssignFloats
  left a member of an AssignAggregates object alone; they now retype it
  in the object when it is still untyped there (its name stays): 66 more
  char/short, 15 more pointers, 3 more floats (the other float members
  were typed already).
- **Float returns**: a call through r1 says nothing comes back in r0, which
  void and float both fit; AssignReturns now also counts callers that read
  fr0 after the call, and with no r0 call or read gives the function a
  float: 54 functions have such callers, most already agreed, 4 set.
- 7 more functions match.
- **`tools/match/frames.py`** reads the prologue of the "frame" near misses
  (654) on both sides. One callee-saved register more or fewer is the
  commonest difference (128 / 114: one value more or less kept alive
  across a call); the space reserved differs most often by 4 bytes (98
  where the game reserves 4 less, 60 more): the decompiler sizes stack
  arrays wrong (`uint local_c[2]` for 4 bytes; a 68-byte buffer for a
  64-byte matrix). variants.py's `framefit` rule resizes a stack array by
  the game's frame minus ours (frames.tsv): of 156 functions with that
  difference alone, 82 get the game's frame exactly and 78 come closer,
  but they differ elsewhere too, so 2 match; 73 have no array (a scalar
  local more or fewer).
- main.bin's `source/` holds **1,851**.

## 2026-10-23: calls without a prototype

- **The original files did not all declare what they called.** GCC 3.2
  calls a function declared void through r1 and an undeclared one (an
  implicit int) through r0; `mtx_store` is called through r0 45 times and
  r1 6 times, and 83 callees are called both ways (returns.tsv "mixed").
- **vf4ft_functions.h** now wraps those 83 declarations in
  `#ifndef NOPROTO_<name>`; a file that defines `NOPROTO_mtx_store` before
  its includes calls mtx_store undeclared, as such an original file did.
  FUN_0c0b9d20, FUN_0c0b9da0 and FUN_0c0b9f80 match that way and not with
  the prototype.
- **variants.py's `noproto` rule** tries one missing declaration at a time
  (and all of a function's together), in batches of their own; auto_match's
  `write_batch` and adopt.py put the define before the file's includes.
  adopt.py leaves out a function whose define breaks one already in the
  file, and in an `_unmapped/` chunk puts it in a file of its own named by
  its address: the call register shows a file boundary the chunk did not
  have (`_unmapped/0c1033c0.c`, `0c103ec0.c`, `0c103f20.c`, `0c1345c0.c`).
- 19 functions match without a declaration (12 in their chunk, 7 in new
  files): main.bin's `source/` holds **1,870**.

## 2026-10-23: a pilot by hand, and what it turned up

- **`tools/match/diff.py ADDRESS [file.c]`**: one function's instructions
  against the game's, for trying edits by hand.
- **Twelve close near misses worked by hand** (distance 1-2, one or two
  of each kind): eleven match. Two were tool bugs, the rest showed
  patterns, one was its own:
  - `isalpha` already matched: match.py's symbol table had given it
    FUN_0c15d220's address. The header pattern let a header whose
    signature puts the return type on a line of its own run on to the next
    function's name; 24 names had the wrong address (and their callers'
    relocations failed), 29 were missing. 5 more matched once fixed.
  - `inflateEnd`: zlib's `struct internal_state` is known by name only
    (deflate.c and inflate.c each define their own); exported empty, GCC
    sized it 0, so `z->state + 0x14` (bytes, to the decompiler) came out as
    + 0. An empty struct now gets one placeholder byte.
  - a local pointer for a global's address (FUN_0c0a08e0, FUN_0c044ee0,
    FUN_0c07e580), a pointer parameter where Ghidra says int (FUN_0c055880,
    move_cmd_29_list_380), a byte parameter (FUN_0c0e3160), an unsigned
    short return (FUN_0c085ea0), a volatile unused local (FUN_0c03f5a0:
    the stack slot of an empty function). FUN_0c0fca20 (a delay slot GCC
    fills differently) is not solved.
  - variants.py gains `ptrbase`, `baselocal` and `narrowparam`: **54
    match** over all near misses. adopt.py now writes a matched function's
    new signature into vf4ft_functions.h (a rewrite that changed its own
    return or parameter types failed as conflicting there) and checks the
    whole tree after: `sound_request` matched with a short parameter, but a
    matched caller passes it unconverted, so the caller's file had an
    old-style declaration (`void sound_request();`) and the definition's
    file the prototype; left unmatched until declarations can differ per
    file in that way too.
- **Small clusters**: the decompiler's `TrapAlways(0x50)` is the `trapa
  #0x50` instruction, exported as inline assembly (40 uses, 2 match). The
  others (mtx_pop, fatal_at "called only by us") were not real: **1,855
  functions' bodies have gaps** (a pool or jump table mid-function), and
  the header's size counts only the body, so the tools compared with the
  game's function cut short. The header now carries the span
  (` * extent: 0x...`), which the tools use (match.py never used the
  size); the "a call more or fewer" near misses drop from 630 to 129
  (__fixunssfsi 64, __floatdisf 6, builtin_strncpy 4, CacheBlockPurge 2).
- main.bin's `source/` holds **1,930**.

## 2026-10-23: a second pilot (branches); declarations three ways

- **Eleven of the closest "branches" near misses by hand**: the causes,
  each then a fix for all near misses:
  - a 4-byte global compared signed but exported unsigned (`undefined4`):
    `DAT_0c292550 < 1` compiles to `== 0` for an unsigned value, the game
    has cmp/pl (gameover_rating_disp, move_tcmd_19_adjust_177). AssignSigns
    now types words too: compared signed (cmp/gt, cmp/ge, cmp/pl, cmp/pz)
    and never unsigned becomes int (92), run after the pointer and float
    steps. GCC's unsigned-to-float conversion tests the sign with cmp/pz
    before its lds, which counted as signed at first and broke two matched
    functions; it now counts as unsigned.
  - a comparison used as a value (`return a != b`, `f((uint)(x == 2))`)
    built as a flag by the original: `int r = 0; if (E) r = 1;`. GCC makes
    the value with negc / movt, the game with a branch (`boolflag`, 14).
  - a copy loop that returns its count (FUN_0c0fa080: callers read r0;
    `retlocal`), a comparison written the other way round with cast
    operands (`cmp` now takes those), and stg_light_num, a debug-menu
    global the game reads as a signed halfword (left: it is also read as a
    byte, so the vote is mixed). FUN_0c14f2c0 (a delay slot) is not solved.
- **Old-style declarations**: vf4ft_functions.h declares 112 functions
  three ways, `#if defined(NOPROTO_f)` (none) / `#elif defined(KRPROTO_f)`
  (`void f();`: the return type known, arguments passed unconverted) /
  the prototype: the mixed callees, and every function with a char or
  short parameter. adopt.py writes the three-way block when it gives a
  function a narrow parameter, and repairs a matched caller that breaks
  by declaring the function old-style in the caller's file:
  `sound_request(undefined4, short)` matches now, with
  `_unmapped/0c04fb20.c` declaring it old-style. variants.py's `krproto`
  rule tries that for near misses.
- 47 more functions match: main.bin's `source/` holds **1,977**.

## 2026-10-23: pilots on "other" and "order"

- **"other" (the largest pool of close near misses)**, by hand:
  - a hardware register at a computed address, `*(T *)(x + -0x5fa08000)`
    (0xa05f8000 + x): the game adds and loads at @(0,rN), as for a
    volatile access. The export writes such a dereference (a constant that
    large is no struct offset) `volatile`.
  - a float array read with an index (fmov @(r0,rN)): AssignFloats now
    counts indexed accesses too (397 floats, 49 more).
  - a lost fourth argument (`mov #1,r7` before the call): AssignPrototypes
    counted a call's arguments only as an unbroken run from r4, and a caller
    that forwards its own r4-r6 and sets r7 counted as passing none; it now
    counts up to the last register written (22 declarations gained
    arguments).
  - an array of 16-byte structs whose field offset the decompiler folded
    into the address (`structarray`), a call through a void function
    pointer (`voidcall`), a call through the uncached mirror built as
    `func | 0xa0000000` (not solved: 7 calls), and two routines in the
    0x0c14xxxx library that never put the frame setup in a delay slot (not
    solved: the compiler's own choice, no flag tried reproduces it).
- **"order"**, by hand:
  - **the 0x0c13xxxx library puts its frame setup in the first call's delay
    slot** (`jsr @r1; mov r15,r14`), and AssignCallEffects, AssignPrototypes
    and AssignReturns took any function without `mov r15,r14` before its
    first call for assembly, so their calls were never counted: now the slot
    counts. FUN_0c12d2a0 then shows its one call through r1: an 8-byte
    (undefined8) return with a call through r1 and none through r0 is now
    set void on one vote (7 more).
  - a subexpression computed into a temporary first (`hoist`), a constant
    stored into a named byte array (`baselocal` now takes named arrays), and
    a bitfield store into an object member written value-first (`bitfield`
    takes both orders and members).
- 21 more functions match: main.bin's `source/` holds **1,998**.
- The status page's timeline no longer prints two labels on top of each
  other at its right end.

## 2026-10-23: other compilers; near misses by agents

- **GCC 3.2.2 and 3.1.1** (gcc-core from ftp.gnu.org, built by
  tools/toolchain/build-gcc.sh; match.py takes `VF4FT_GCC=<version>`) give
  the same result as 3.2.3 over every candidate: not one function matches
  with them that does not with 3.2.3, and none is lost. The 44 near misses
  where the game never puts the frame setup in a delay slot (mostly
  0x0c13xxxx-0x0c14xxxx) are not a 3.1-3.2 difference.
- **Six agents, fifteen close near misses each** (build/agents/BRIEF.md:
  the tools, the fixes found so far, results written to their own files and
  adopted centrally): **85 of 90 matched**, in 4-10 minutes a batch. Their
  fixes, by how often:
  - statement shapes: early returns of a value already in r0
    (`if (r <= 0) return r;`), `r = a; r -= b;`, for loops with the pointer
    step in the increment, pre-increment in a test (`if (++g > n)`), an
    if/else of two direct calls GCC merges into one jsr (Ghidra shows a
    function pointer chosen by a condition);
  - **Ghidra's `& 0x1f` on a shift count is not in the source** (`1 << n`
    matches; SH's shld needs no mask) in the move_tcmd_* bit handlers;
  - call registers: callees still declared with the wrong return
    (FUN_0c141d20, _cleanup_r: void but called through r0; FUN_0c12d1d0:
    undefined8 but called through r1), fixed with a cast at the call;
  - **which operand is pointer-typed decides `@(r0,Rn)`**: GCC makes the
    pointer the base and the other the index in r0. The player records at
    `_1p_level` (0x0c37e8b8, 0x68 bytes each) match written as
    `p = (char *)(i * 0x68); p[(int)&_1p_level]` (the index the pointer, the
    symbol the integer), where every pointer-typed base gives the opposite;
    the agents' literal-address versions were rewritten that way, so no RAM
    address is hard-coded;
  - narrow types, stack buffers sized 4 bytes smaller than Ghidra says.
  - Not matched: a float register pair swapped, an explicit `@(0,rN)`
    store, the random-number step's pool order, FUN_0c087b00 (the player
    records again, with more around them).
- diff.py now prints match.py's report when the instruction listings agree
  but the bytes do not (a pool constant, a frame size, a branch distance).
- main.bin's `source/` holds **2,084**.

## 2026-10-24: agent rounds A-C; rules from them; hardware mirrors

- **Rules from the agents' fixes** (variants.py): condcall (two direct
  calls Ghidra shows as one function pointer), guardret (early return of
  the value in r0), shiftmask (Ghidra's `& 0x1f` on a shift count),
  indexptr (the symbol as the integer operand of `@(r0,Rn)`), notflag
  (`int t = a == b; return t == 0;` for the game's movt / tst / movt). The
  bitread rule's pattern had been lost from the file since e4d615d; it is
  back.
- **Three agent rounds** of ten batches each over the close near misses
  (distance <= 4): about 140 adopted per round, **2,110 -> 2,534**. Fixes
  that came up again and again are in build/agents/BRIEF.md for the next
  round: `muls.w` from two short variables, switch cases written in jump
  table order, K&R definitions for callees that narrow their own char
  parameter, fields at large offsets split by GCC as
  ((off + 4) & ~60) - 4, and things Ghidra loses (a varargs argument, a
  float copy shown as an int copy, a value shown as an address).
- **debug_out** (0x0c1e3924, ghidra/symbols/debug.tsv): the handle every
  debug print passes first; Ghidra showed it as `&PTR_DAT_0c322d60`.
- **Hardware mirrors mapped**: the game reaches its registers through the
  uncached P2 mirror (0xA05F6800, 0xA25F6800, 0xA85F6800, 0xA8800000, the
  AICA at 0xA0700000), which was in no memory block. As ordinary memory, a
  store nothing reads afterwards was dropped (the border-colour store before
  a fatal `for (;;)`, seven functions the agents fixed by hand). Now volatile
  blocks (SetupNaomi2.java); FUN_0c151a20 matched on the rebuild.
- Signed words: strong signed evidence now wins over a few unsigned uses
  (`DAT_0c292550`, 52 signed compares, is int). The export declares every
  typed global, not only the ones src/ uses, except names that are not C
  identifiers (pointers named after their string).
- adopt.py: K&R definitions; a signature sync that breaks the file's own
  callers now leaves out only that function (one had kept 36 others out);
  header syncs are undone for functions not kept; VF4FT_ADOPT_DEBUG=1 says
  why a file's functions were left out.
- match.py and permute.py wrap relocation addends to 32 bits (`&SYM - 8`
  was a false DIFF).
- main.bin's `source/` holds **2,535**. Held: FUN_0c048f80, which needs a
  typed global at 0x0c3777a4 (its extent is not known yet).

## 2026-10-24: agent round D

- The variants rules on the rebuilt tree: 15 more (**2,550**).
- **Thirteen agents over the 194 close near misses left: 186 matched**, all
  adopted (**2,736**). New, and in the brief for later rounds:
  `__movstr_i4_even/odd(d, s, n)` is a struct assignment of (n + 1) * 8
  bytes (+ 4 for odd), now also a variants rule (movstr); Ghidra's shared
  tails were the same statements in each branch, merged by GCC's
  cross-jumping; adjacent stack locals that are one array; a call result
  stored to a struct field (GCC takes the address before the call).
- Two declarations the agents had cast around, now in
  ghidra/symbols/signatures.tsv: FUN_0c03d420 takes (int, float, float,
  uint), and FUN_0c11fc00 returns int (92 callers read only r0).
- The 8 not matched are register allocation or an address GCC folds
  differently: several reach fields of one large struct at agg_0c41e010
  that Ghidra splits into separate aggregates (agg_0c41e34c is its offset
  0x33c).
- Ghidra also drops an ordinary RAM store before `for (;;)` (FUN_0c10ef20),
  not only hardware ones; not fixed.

## 2026-10-24: agent round E (farther near misses)

- Batches dealt by `build/agents/make_batches.py` from the classify rows
  not tried yet, at distance 5 to 8. **Three waves of agents (12, 12 and
  11), 518 functions: 470 matched**, 468 adopted (**3,206**, from 2,737).
  The third wave matched 144 of 158 (91%), so the hit rate has not fallen
  with distance yet.
- A variants rule `hwconst` writes every hardware register symbol as its
  address (`*(volatile T *)0xa05f6c14`), the form the agents needed, in
  one step. On its own it finished 1 of the 215 near misses that touch a
  register, alone or with the other rules (48,936 variants): the
  register form was needed in those functions, but never the only thing.
  It stays in the rule set for the full runs.
- Three declarations for the next export, in ghidra/symbols/signatures.tsv:
  FUN_0c039780 and FUN_0c03a100 take their float first (each has one
  caller, matched only in that order); FUN_0c136240 is a trampoline to
  the uncached FUN_0c136140 and takes its two arguments. Their callers
  already cast to those shapes, so nothing matched changes.
- adopt.py's repair now tries both NOPROTO and KRPROTO for each changed
  callee in a file that stops compiling and keeps the better; the export
  guards every function a source file names in such a define. FUN_0c12f460
  is now void, as its callers (through r1) say.
- **libgcc's `__fixunssfsi` (FUN_0c1592a0) matches only at -O2**, the
  flags libgcc is built with; its file carries `match-flags: -O2`. The
  function after it (FUN_0c1592e0) is -O1 code from another unit (at -O2
  its call becomes a tail jump) and has its own file.
- New, and in the brief for wave 3: hardware registers written as
  integer-constant addresses let GCC derive neighbours with `add #4`
  (the declared `DAT_a05f...` symbols do not; four functions); if/goto
  chains that were a `switch`; small local arrays that keep values apart;
  interrupt masking (`stc sr` / `ldc sr`) and `pref` that Ghidra drops.
- Header notes from the third wave, not yet applied: FUN_0c0f5380 takes
  nine parameters (the game copies five stack words to its snprintf-like
  callee; the matched text reads the ninth as `(&param_8)[1]` so its
  callers still compile); FUN_0c0df100's parameter order is in doubt (its
  matched callers cast it three different ways); FUN_0c0698e0 and
  FUN_0c083c60 return int; FUN_0c0ef3a0 returns int; FUN_0c0cc660 is
  void; FUN_0c0f53e0 is called through r0; FUN_0c113dc0 takes three
  shorts; and `_unmapped/0c0e6260.c` may be an -O2 unit (its miss matches
  exactly at -O2).
- Agents keep finding fields of one struct at agg_0c41e010 (+0x230 a
  {fn, arg} table, +0x43c, +0xbc/+0xcc/+0xdc), agg_0c41fc50 (+0x130),
  and agg_0c450f24 (+0xfc, +0x104, +0x130, +0x13c) that Ghidra splits
  into neighbouring aggregates; defining those three structs would make
  a cluster of near misses plain.
- The misses left are register allocation, loop-invariant hoisting, and
  the order GCC's jump pass places a constant load or a return block.

## 2026-10-25: agent round F (distance 9-12)

- A quick refresh first (export, auto_match, adopt, classify) applied the
  round E declarations; nothing broke. Then **two waves of thirteen
  agents over 379 near misses at distance 9-12: 332 matched**, all
  adopted, plus 9 held-back matches (below): **3,547** (from 3,206). The
  hit rate (88%) has still not fallen with distance.
- **Wrong parameter order in callee prototypes was the most common reason
  an agent held a match back** (the brief asks them to report rather than
  cast). SH-4 passes ints and floats in separate registers, so any order
  of the two kinds is the same call to the callee, but the caller loads
  its arguments in declaration order. Four are fixed in
  ghidra/symbols/signatures.tsv, each from callers that match only that
  way: FUN_0c0dfbc0 (eight floats, then three ints, then two floats and a
  pointer on the stack; five agents), FUN_0c0dec00 (two agents),
  FUN_0c0deb40, FUN_0c0e9ca0. With them, nine held-back texts matched
  once their casts were removed, and the second wave's agents matched
  more of the FUN_0c0de7xx-0c0deaxx family against the fixed header.
- Still to check before changing: FUN_0c0df100 (two agents found
  (int, float, float, float, uint, byte *); round E callers had cast it
  other ways); FUN_0c0e1340 (float, int *).
- **Delay slots in `_unmapped/0c0f5fc0.c`** (0x0c0fe5a0-0x0c0fece0, already
  built without -fno-delayed-branch): six siblings differ only in which
  argument move fills the call's delay slot. Batch 113 matched two of
  them by writing the counter post-incremented inside the call argument
  (`f(buf, n++, ...)`) with its first value set before the `if (param)`
  guard; the others probably need the same.
- **va_list**: FUN_0c0f5500 passes a va_list (a 20-byte struct on SH-4,
  copied on the stack: the "five stack words"). FUN_0c0f5380 (round E)
  and FUN_0c0f2420 (this round) match through a `(&param_8)[1]` stand-in
  that reads past their last parameter; they are probably the same
  pattern and want va_list / variadic declarations.
- Other leads: 0c0af222 is 2 bytes into FUN_0c0af220 (a function boundary
  error; the whole function matches as one); FUN_0c11e280 matches only
  with -fstrict-aliasing (its file's flags?); the game reloads a callee's
  address before each call where GCC keeps it in a register (0c0b6280,
  0c1421e0, 0c0b5e60), which -fno-function-cse does not reproduce;
  FUN_0c11a460 and FUN_0c08e400 are RSA's reference MD5Update.

## 2026-10-26: agent round G (distance 13-20); a prototype pass

- **Four waves of agents (36 batches) over 523 near misses at distance
  13-20: 451 matched**, 443 adopted at once (3,990, from 3,547). The hit
  rate held at 87% for distance 13-16 and fell to about 82% at 17-20. Most
  of the misses left are register allocation and loop-invariant hoisting
  with the code's shape already right.
- **A prototype pass on what the agents reported** (they report a wrong
  callee prototype rather than cast to it):
  - Float-first parameter orders in ghidra/symbols/signatures.tsv:
    FUN_0c0cd740 (two callers), FUN_0c0debc0, FUN_0c0df100 (a round F
    lead, confirmed by a second agent), FUN_0c0df600 (its two matched
    callers still match with their arguments put in the new order),
    FUN_0c0399a0. FUN_0c096ae0, a matched pass-through, takes its float
    second.
  - Returns: FUN_0c159d80 (sprintf-like) returns int, FUN_0c14eee0 int,
    FUN_0c138e40 void.
  - FUN_0c059480's four checks take one argument. The r5 loads the
    decompiler gave them (18, 21, 16, 13) belong to the shared call after
    each check.
  - The script handlers take the script pointer and pass it on in r4:
    script_press_held and script_op_08_new_press had called script_step_back
    / script_backtrack through no-argument casts, which compile the same at
    -O1 because nothing touches r4. They now take the parameter and pass it
    explicitly.
  - A float at 0x0c2184cc (+infinity, after the powers-of-ten table) for
    FUN_0c15d680.
  - With these, 15 held-back texts matched (14 with their casts removed, one
    through the new float), six adopted casts became redundant and were removed, and the re-export let
    auto_match find 8 more: **4,014**.
- **Narrowing a parameter in the header can break a caller in the same
  file.** FUN_0c0a9760 matched as `(uchar)`, and its caller FUN_0c0a0e00
  began zero-extending the argument, which the game does not. The original
  called it with no narrow prototype in view: it is now a one-line K&R
  definition behind `#define KRPROTO_FUN_0c0a9760`. match.py catches this,
  so check it after every adoption that narrows a parameter.
- A file's own `extern` of a global can differ from the header: FUN_0c0b37c0
  matched alone but not in `_unmapped/0c0b06a0.c`, which declares
  DAT_0c292808 `undefined1`. It needs the `(char)` read the rest of that
  file uses.
- Leads, all in build/agents/G-header-leads.md:
  - Narrow parameter types reported by agents and matched through locals or
    casts. Each needs its callers checked as above.
  - Split structs: agg_0c41fd4c (agg_0c41fc50+0xfc), agg_0c375ae4 inside
    agg_0c3744e8, agg_0c4edce4 (the AET stack), agg_0c4ee14c (starts at
    DAT_0c4ee0d0), agg_0c417488 (agg_0c416b0c+0x97c), agg_0c37d9c4
    (DAT_0c37d848+0x17c), agg_0c45583c (0x24c-byte entries).
  - FUN_0c0c63a0 takes a 12-byte struct by value (it needs a named vec3
    type).
  - 0c11e500 / 0c11e480 hoist a global's load across a call, as if the
    callee were declared const.
  - 0c08b580 / 0c08b7e0 recompute `param_1 ^ 1` for a second call, where
    GCC reuses the first.
  - FUN_0c0b1680/17e0/1940 and FUN_0c0de9a0 may also take a float before
    an int.
  - diff.py decodes the pool word 0x9540 as an instruction (a phantom
    line; the verdict is right).

## 2026-10-27: agent round H (distance 21-30)

- **Two waves of agents (22 batches) over 322 near misses at distance
  21-30: 257 matched**, 250 adopted (**4,264**, from 4,014). The hit rate
  was about 79% for 21-25 and 81% for 26-30; these are larger functions,
  so most batches stopped at ~10 minutes per function. The misses left are
  again register allocation, loop-invariant hoisting, and a constant base
  folded into one pool word where the game adds the offset.
- Seven texts that call through a function-pointer cast are held
  (build/agents/held-H1-casts.json, held-H2-casts.json) for the header
  pass, along with about five that agents proved only against a reordered
  callee prototype.
- **A file's `NOPROTO_` define can change which register a call goes
  through.** FUN_0c0d3a00 matched alone but not in `_unmapped/0c0b06a0.c`,
  which defined both NOPROTO_ and KRPROTO_mtx_rot_x_s16. NOPROTO (no
  declaration at all, so an implicit int return) wins, and it moved the
  call address from r1 to r0. The file still matches with KRPROTO alone,
  so the NOPROTO line was dropped. The file also lacked six DAT_0c2edaXX
  externs, and the agent's text wrote the mask 0xfff7ffff as the address
  of a symbol at that value.
- New source shapes the agents found:
  - `case 0: default:` (or `case 0x14: default:` for a table running
    1..20) to get the game's jump table or compare tree.
  - A constant held in a local (`int m = 0xff`, `short z = 0`,
    `float y = -4.0; y + 64.0`): GCC 3.2 keeps it in a register instead of
    folding it.
  - A `memcpy` the game really calls stays a call when the source's
    alignment is dropped: `(void *)(int)&SYM`.
  - Two more zlib 1.1.4 functions (gen_bitlen, scan_tree) matched on the
    original text.
- Leads, all in build/agents/H-header-leads.md:
  - mtx_rot_x_s16 / mtx_rot_y_s16: at least six batches needed KRPROTO_,
    because callers pass a signed short with no extu.w; the ushort
    prototype is wrong.
  - Returns: legacy_io_poll, jvs_read_node, jvs_read_two_nodes,
    FUN_0c098640, FUN_0c137ae0, FUN_0c130d40 return int; FUN_0c1318a0 and
    FUN_0c047d80 return float; script_op_06_input_bit takes and passes the
    script pointer like op_07.
  - Parameter order: FUN_0c0d4d20, FUN_0c02d080, FUN_0c0dea20,
    FUN_0c041160, FUN_0c1355a0, FUN_0c0ac7e0, FUN_0c0de760, FUN_0c12d8a0.
    FUN_0c0debc0 and FUN_0c12d990 have callers that load their arguments
    in different orders; they stay as they are.
  - Two misses (jvs_read_two_nodes, 0c110e60) differ mainly in the call
    register (r0 against r1), the same symptom as FUN_0c0d3a00's: check
    their callees' declarations first.
  - 0c125380 and 0c0ea8f4 are hand-written assembly (no frame); skip them.

## 2026-10-27: round H header pass (4,277)

- **A call through r0 means the caller saw a value-returning declaration.**
  FUN_0c0d3a00 showed it directly (an implicit-int declaration moved the
  call address from r1 to r0), and ghidra/returns.tsv counts it per callee.
  legacy_io_poll, jvs_read_node and jvs_read_two_nodes (all called through
  r0 by frame_io_update) and FUN_0c133420 now return int, though their
  bodies return nothing: an old-style int function, as the original had.
- mtx_rot_x_s16 / mtx_rot_y_s16 take a plain int. They are hand-written
  (`lds r4,fpul; fsca`) and use the low 16 bits; callers pass sign-extended
  shorts or zero-extended ushorts, which an int parameter reproduces. All
  eighteen KRPROTO_ defines for them are gone; three files keep
  NOPROTO_mtx_rot_y_s16 (an implicit int, so a call through r0).
- Returns: FUN_0c137ae0 int, FUN_0c1318a0 and FUN_0c047d80 float,
  FUN_0c0b0460 void; script_op_06_input_bit takes the script pointer.
- Parameter orders (floats before ints, as the callers load them):
  FUN_0c0d4d20, FUN_0c041160, FUN_0c0ac7e0, FUN_0c12d8a0, FUN_0c0b1560
  (float, char), FUN_0c099860, FUN_0c02d080, FUN_0c0dea20; FUN_0c107de0's
  third parameter is an int and FUN_0c0a7ae0 takes one. Matched callers
  were put in the new order with a script, and still match.
- **FUN_0c0de760 keeps its order.** One caller (0c0aa220) loads its int
  third, but 22 of its 66 matched callers break when they are reordered:
  the callee cannot tell the two apart, so the callers must come from
  different declarations or macros. 0c0aa220 stays held, like
  FUN_0c0debc0's and FUN_0c12d990's conflicting callers.
- With these, 13 held texts matched with their casts removed: **4,277**.

## 2026-10-27: function boundaries; agent round I (distance 31-40)

- **Six functions started 2 bytes late.** FindPrologues walks back from
  `sts.l pr,@-r15` over the pushes and stops at a delay slot; the halfword
  before these functions' first push is a pool word that reads as a
  delayed branch (0x0c23 = braf r12, 0xa000 = bra). A push on a 32-byte
  boundary now always starts the function. After a full `run.sh all` the
  entries are 0c0a4600, 0c0af220, 0c0dce20, 0c136260, 0c136380 and
  0c1364a0 (all six were skipped by AssignPrototypes as "not 32-byte
  aligned"). 0c1364a0 matched at once; two matched callers that reached
  the real entry as `FUN_0c136262 - 2` now name it; four more matched in
  round I. test.bin was re-imported too (its 46 still match).
- **Round I: 15 batches over 214 near misses at distance 31-40, plus a
  retry batch of 13 earlier misses with known residuals.** 180 of the 214
  matched (84%); 178 are adopted and two that need a header change are
  held (build/agents/held-I-casts.json, with 0c07a620 and 0c0a4600). The
  retry batch got 1 of 13 (0c0f4460): the residuals left after a round
  are register allocation that no source shape reaches, so retrying them
  is not worth a batch. **4,457** functions match.
- New source shapes:
  - Whole-struct copies of 12-byte vectors (`typedef struct { float x, y,
    z; } V;`) and chained copies `*(V *)a = *(V *)b = src` where Ghidra
    shows word copies.
  - A GNU statement expression for what was a macro:
    `({ float r = FUN_0c131ae0(); FUN_0c0bf7c0(lo, hi, r); })` computes
    the store address before the call, as the game does.
  - Declaration order sets the stack-slot order of spilled locals.
  - A precedence bug in the original: `if ((r = f() < 0))`.
- Header leads, in build/agents/I-header-leads.md: FUN_0c0f53e0 returns
  int (two matched or held callers cast it); FUN_0c0a47a0 takes five
  arguments; FUN_0c0e5360 is (float, float, undefined4);
  mtx_rot_zyx_s16 has the same signed-short problem mtx_rot_x/y_s16 had;
  0c0c6460 takes a 12-byte vector by value (needs a named vec3 type);
  FUN_0c12d990 has a third caller order (move_cmd_40_velocity_scale).
- **Header pass (4,459):** mtx_rot_zyx_s16 takes ints like mtx_rot_x/y_s16
  (three files drop their KRPROTO_ define); FUN_0c0e5360 is (float, float,
  undefined4); FUN_0c0a47a0 takes five parameters and FUN_0c1345c0 two
  floats. None of these had a matched caller to reorder. 0c0a4600 and
  0c0a93a0 then matched without their casts. 0c07a620 stays held:
  network.c calls FUN_0c0f53e0 (a variadic string append) through r1
  twice and through r0 once, so neither declaration fits the whole file.

## 2026-10-29: agent round J (distance 41+); K&R calls; agg_0c41e010 (4,583)

- **Round J: 10 batches over the near misses past distance 40.** 112
  texts came back matched; 110 are adopted and two are held
  (build/agents/held-J-casts.json): 0c097640 cast mtx_scale_v (fixed in
  the header pass below), and 0c0c7b40 passes a 24-byte struct by value
  to FUN_0c0c78e0, which needs a named struct type first. Twelve more
  agents' texts matched only with a header change (below).
- **The permuter, seeded with the agents' best texts** (`permute.py
  --seeds`, 131 seeds, 4 shards, `--top 1000 --rounds 30 --width 12`, run
  alongside round J): two new matches (0c046520, 0c0c4ee0) and many
  distance cuts that stop short of 0. The other four "matches" were
  held cast texts that started at 0. Not worth repeating at this scale.
- **Callers that disagree on argument order are K&R calls.** Every held
  text whose callee needed one argument order in one caller and another
  order elsewhere matches as an unprototyped call. An old-style call loads
  the arguments in the written order (the registers stay the same: floats
  in fr4.., ints in r4..), so both orders are right, each in its own
  caller. Two round J agents found this on their own (0c03e340, 0c03d740).
  Callees: FUN_0c12d990, FUN_0c0de760, FUN_0c0debc0, FUN_0c0de9a0 and
  FUN_0c0df600 (whose signatures.tsv row already said FUN_0c038440 needs
  one order). Their callers' files now `#define KRPROTO_<callee>`, and
  every matched function in those files still matches, including the
  callers written in the other order.
- **Header pass:** signatures.tsv rows for FUN_0c02bca0 (the int after
  the two floats; both callers agree), FUN_0c095180 (returns float),
  FUN_0c0bfd00 (float third) and mtx_scale_v (takes a float[3]). With
  these and the K&R guards, the 12 held proofs match with no casts
  (build/agents/merged-J-uncast.json): 0c02bfa0, 0c02c120, 0c0d16e0,
  0c0bfae0, 0c097640, 0c0443c0, 0c066dc0, 0c0d8e80, 0c0aa220, 0c03d420,
  0c03e340, 0c03d740.
- **agg_0c41e010 is one 0x580-byte object.** GCC splits an address past
  `@(disp,rN)`'s 60-byte reach into its own pool word at `((off + 4) &
  ~60) - 4`, so one big object shows up as 64-byte pieces at offsets
  ≡ 60 mod 64. ghidra/aggregate_extents.tsv gives AssignAggregates a
  minimum size per object, from the pool scan (split bases unbroken from
  +0x7c to +0x57c). tools/merge_aggregate.py then rewrote 9 matched files
  from the pieces to fields of the one object. Two pieces stay separate.
  0c41e524 (+0x514) and 0c41e53c (+0x52c) are not at split bases, so they
  were their own bases in the game's code, and folding them broke five
  functions. Three functions that use the base and a piece in one body
  (0c13d420, 0c147300, 0c148de0) changed register allocation once both
  were one symbol, so they keep their piece. **4,583** functions match.

## 2026-10-30: stale pieces; two more merged objects; agent rounds K and L (4,711)

- **AssignAggregates left the old pieces in the program.** When an object
  grows, the type is replaced in place, but the data made last time keeps
  its old length (agg_0c41e010 was still 188 bytes in the listing, its
  type 0x580). createData then leaves that data as it is, so the old pieces
  inside stayed. The export writes the headers from the type, so they
  looked right, but AssignSigns typed the old pieces' members and never
  the merged ones. Merged fields that should be `int` came out
  `undefined4`, and signed compares changed. AssignAggregates now clears the
  object's range before it makes the object. With that fix FUN_0c148de0
  merges. FUN_0c13d420 and FUN_0c147300 still keep their pieces, and
  0c41e524/0c41e53c still break five functions when folded.
- **agg_0c41fc50 (0x130) and agg_0c450f24 (0x160)** merged the same way:
  word split bases at +0x7c/+0xbc/+0xfc and +0xbc/+0xfc/+0x13c, each up to
  the end of its last piece (aggregate_extents.tsv has the evidence). Three
  matched files were rewritten; all still match.
- **0c0c7b40:** FUN_0c0c78e0 takes a 24-byte struct by value. New types in
  ghidra/types/vf4ft_types.h: `vec3` and `ray3` (a direction and a point).
  With a signatures.tsv row for FUN_0c0c78e0 using `ray3`, the text matches
  with no cast.
- **0c07a620:** FUN_0c0f53e0 (a variadic string append) returns int. With
  `int FUN_0c0f53e0(int *, char *, ...)` declared, all 41 functions of
  network.c match, FUN_0c07af80 included, though it calls through r1 twice.
  The r0/r1 call-register rule is not absolute. FUN_0c07b060 drops its
  cast, and 0c07a620 matches.
- A refresh with the new headers matched two more by rule. **4,587**
  functions match before round K.
- **Round K: 8 batches over the near misses at distance 51-60 (109,
  some newly close at 47-50 after the header changes), plus a retry batch
  of 3 struct misses.** 85 matched (78%); 84 adopted at once. The K&R rule
  from round J, now in the brief, fixed three more callers
  (FUN_0c0dec00, FUN_0c0df600, FUN_0c077fa0). The retry got 0c14c3a0 with
  the merged agg_0c41e010; 0c12b9a0 and 0c140c80 are down to one register
  swap each.
- **Round K header pass:** FUN_0c03a2e0 is (float, int, int, int) and
  returns int; FUN_0c05c320 takes the int its caller passes (its matched
  definition had `void`); `maple_dma_end` names the end of the 0x1ff0-byte
  buffer at 0c325ac8 (FUN_0c115300 subtracts the two symbols; Ghidra folds
  that to 0x1ff0, so a DAT_ label would never be exported). Lesson:
  FUN_0c039420's callers read its result as a char but sign-extend it
  themselves, so it must return a word. A `char` return drops the exts.b,
  and once the types step has applied a signature it stays until a row
  says otherwise. 0c034240, 0c0551c0 and 0c115300 then matched. **4,674**
  functions match.
- **Second header pass:** FUN_0c04aec0 returns char (no matched caller
  yet), FUN_0c0cffe0 takes three chars, FUN_0c0a1d20 returns int (`bool`
  is `int` here, so its matched definition is unchanged), agg_0c37d804
  grows to 0x44 to take DAT_0c37d844, and the two camera tables FUN_0c02d8e0
  picks between are `cam_pers_dist_table` and `cam_pers_dist_table_ending`
  (`pers_range[6]`: a tag, a distance range and the perspective value it
  eases toward; the ending table runs 40-60 where the other runs 25-45).
  0c04c780 and 0c02d8e0 then compile to the game's instructions up to one
  register swap each (r9/r10, fr12/fr13), which no source shape has moved
  so far. FUN_0c069b80's `uchar` parameter was not applied: both callers
  already match and 176 others would be at risk.
- **The agg_0c41e000 lead does not hold.** A round K agent read the pool
  words 0c41e53c and 0c41e57c as split bases of a 64-aligned object (they
  are ≡ 60 mod 64 from 0c41e000, not from 0c41e010). The scan shows they
  sit in dense runs of direct addresses (0c41e538, 53c, 540 are
  consecutive words; 0c41e39c..41c come every 16 bytes), and 0c41e53c is
  used as a base with displacements +0xc..+0x28, which a base at 0c41e010
  would have split at 0c41e50c and 0c41e54c instead. So 0c41e53c is a
  separate global after the big object, which is how the matched code
  already treats it. Nothing was re-based.
- **Round L: 5 batches over the 69 near misses at distance 61-70.** 35
  matched (51%, against 78% in round K: the band is harder, and the
  residue is register allocation), plus 3 held for header fixes. The
  agents were cut off twice by the account's spend limit and resumed from
  their saved results each time. **Header pass:** FUN_0c14f2c0 returns int
  (its one caller reads r0; the matched definition changed, and
  0c151240 then matched); K&R guards for FUN_0c0decc0 (the uint last),
  FUN_0c0e0f80/1120/1240 (the float first) and FUN_0c1034e0 (called
  through r0); rows for FUN_0c0365a0 (float, int, float), FUN_0c145e80
  (returns int) and FUN_0c15d740 (a strtod-like routine: returns float,
  takes char **). 0c032ba0 and 0c0e14e0 then matched as K&R calls.
- **A static inline helper is a valid text.** 0c02d500 matches only with
  a `static inline int padhit(int m)` above it: the game's `mov #1,r0;
  and r0,r1` comes from an inlined parameter, which no plain-C shape
  gives (cse never folds a cost-1 constant into a register operand).
  adopt.py now takes the last definition in a text as the one being
  adopted (it took the first, the helper), and ExportSource skips
  `static` definitions when it checks source/ against the program's
  functions. **4,711** functions match.
- **A local model as the matcher: tested and not viable.** tools/match/
  local_match.py drives a model behind an OpenAI-compatible server through
  the same loop an agent runs (the brief's patterns, the decompiler's C,
  the instruction diff; one C text per reply; compile, diff, repeat), with
  match.py as the judge. Against Qwen3.6-35B-A3B (NVFP4, vLLM on a 32 GB
  RTX 5090, ~1,000 tokens/s aggregate) on 15 functions round K's agents
  had matched, given the decompiler's text from before that round: **0 of
  15** in non-thinking mode (most replies do not compile: the decompiler's
  temporaries left undeclared, register names written into C, code outside
  the function; the compiler's message and the diff change nothing from
  one attempt to the next), and in thinking mode the reasoning runs to
  the token limit (16k, then 32k with a bf16 KV cache, so it is the model,
  not the fp8 cache) without producing a text. Five more functions were
  excluded because their old text no longer compiles after the
  agg_0c41e010 merge; the harness now reports those instead of counting
  them. A dense model of the newest generation, Qwen3.8-27B (NVFP4, ~50
  tokens/s a stream), did better in kind and no better in result: it acts
  on the diff (141 -> 95, 179 -> 139, 281 -> 186, 131 -> 71 lines over the
  first attempts), then plateaus or regresses, and thinking (which does
  terminate on it) changes nothing: 0 of 15 without thinking, 0 of 5 with,
  the same functions the agents matched at 78%. Both models make the
  first-order fixes the brief lists and never do the second-order work
  (register allocation, cross-jumped tails, declaration order) that the
  agents' notes are full of. The agents stay the path for the matching;
  the harness is kept for a model that can reason about register
  allocation. Two practical findings along the way: unsloth's and
  NVIDIA's NVFP4 builds of the Qwen3.6/3.8 models quantize the output head
  in a form vLLM 0.21 cannot load (pin a revision, or pick a build whose
  quantization config ignores `lm_head`), and a dense 27B's bf16 KV cache
  fills a 32 GB card at 5-6 requests (fp8 doubles it).
- **Round L header pass, second part:** agg_0c3d7828 is a 0x70-byte object
  (a 0x18 header and two 44-byte entries), so an aggregate_extents.tsv row
  grows it from 11 bytes and the DAT_0c3d7840..788c scalars become its
  fields; no matched code named them. Not applied: the two 268-byte
  entries at 0c41f0c8 (no object there to grow, and matched code uses
  DAT_0c41f0c8/0c9/1d5 as scalars in thirteen places); FUN_0c0b7cc0's
  int third parameter and FUN_0c0deb80's uint-last order (their callers
  match through KRPROTO_ already). The leads file is
  build/agents/L-header-leads.md.

## 2026-10-31: agent rounds M, N and O (4,789)

- **Rounds M, N and O** took the untried near misses at distance 71-80,
  81-90 and 91-100 (batches 212-220, nine agents, 118 functions):

  | Round | Distance | Batches | Matched |
  |---|---|---|---|
  | M | 71-80 | 212-215 | 38 of 53 (72%) |
  | N | 81-90 | 216-218 | 23 of 35 (66%) |
  | O | 91-100 | 219-220 | 17 of 30 (57%) |

  K (51-60) had 78% and L (61-70) 51%, so distance alone does not set the
  yield; the misses are mostly register near-ties with the structure
  matched (most within 1-20 instructions), plus a few large functions the
  agents did not reach. Leads per batch: build/agents/MNO-header-leads.md.
- **Two prototypes fixed for held texts** (signatures.tsv):
  FUN_0c0dc61c is (float *, float *, float *, float, float), the decompiler
  dropped the second float in fr5 (0c0d9440); FUN_0c15d5a0 is the C
  library's integer division with remainder and returns a new `div_t`
  {quot, rem} in r0:r1 (validate_structure, 0c1597c0). AssignReturns
  reports FUN_0c15d5a0 as disagreeing with its callers because it only
  knows scalar returns; the matched caller settles it. Both texts matched
  without their casts after the export and were adopted.
- **zlib texts**: build_tree and longest_match matched from zlib 1.1.4's
  own logic, like the rest of src/zlib; they were brought into the repo's
  zlib style (no zlib comments, its NIL/MAX_MATCH/MIN_LOOKAHEAD/MAX_DIST
  macros replaced by their values; build/agents/zlib_style.py) and still
  match.
- **Not applied yet** (no held text needs them): the bytes at 0c29283e /
  0c29283f as their own globals (0c0eeaa0, one store pair); FUN_0c1479c0's
  29-word struct passed by value; FUN_0c0877a0 / FUN_0c086e60 K&R guards
  for 0c086840 (a register miss besides); the 13-float array at
  0c454998; the unsolved `(sym + idx) + K` index form shared by
  sel_chara_select and 0c049fe0.

## 2026-10-31: agent rounds P, Q and R (4,843)

- **Rounds P, Q and R** took the untried near misses at distance 101-110,
  111-120 and 121-130 (batches 221-226, six agents, 71 functions):

  | Round | Distance | Batches | Matched |
  |---|---|---|---|
  | P | 101-110 | 221-222 | 17 of 22 (77%) |
  | Q | 111-120 | 223-224 | 16 of 21 + 1 held (81%) |
  | R | 121-130 | 225-226 | 19 of 28 + 1 held (71%) |

  The yield did not fall with distance: the batches were smaller and the
  brief now leads with copying a matched sibling (the 0c14abc0 family
  gave four) and the loop.c hoisting rule. Misses are again register
  ties and spills with the structure matched, plus a few large functions
  (0c13d920, 0c0a9cc0, 0c1001e0). Leads: build/agents/PQR-header-leads.md.
- **FUN_0c0deb00 is (int, float x4, int)**: two callers (0c0918e0,
  0c034ba0) load its second int after the four floats. Its definition
  in _unmapped/0c0dca60.c had the decompiler's (u4, u4, f, f, f, f); the
  parameters were reordered (names kept, so the body is unchanged; the
  file still matches 83 of 83) and both callers then matched without
  casts. Integer and float arguments use separate registers on the SH-4,
  so only the callers' evaluation order depends on this.
- **A text adopt dropped silently**: 0c089b40 matched alone but adopt
  left it out ("did not compile or match in its file"). Its group file,
  _unmapped/0c072320.c, has a file-level `#define G ((T *)DAT_0c2a0964)`
  and the text declared a local typedef `G`; renamed, it adopts and
  matches. adopt hides the compiler's message: to see it, compose and
  compile the group file by hand. The brief now asks for descriptive
  local type names.
- **FUN_0c15d5a0** (div) itself matched, as newlib writes it, returning
  the div_t added in round N.

## 2026-10-31: decomp-permuter on SH-4; the local harness's guard and hunk mode (4,854)

- **decomp-permuter, ported to SH-4.** Simon Lindholm's permuter (MIT)
  supports MIPS, PowerPC, ARM32 and x86. The port lives outside this repo,
  in a fork at `Claude Data/decomp-permuter`, branch `sh4`; its
  `sh4/README.md` has the details. Its randomizer works on the C syntax
  tree with no architecture-specific code. Two pieces needed replacing:
  - **Scoring.** objdump text against a `target.o` doesn't work on SH-4:
    constants and addresses sit in literal pools, so our object holds
    relocations where the game holds addresses, and the pool layout moves
    with the code. The SH-4 scorer reads the game's bytes from the image
    and lists both sides as local_match.py's judge does (pool loads by the
    value they load, branch targets as labels). Then it applies the stock
    penalties: 5 per operand, 60 per reordered instruction, 100 per
    inserted or deleted one.
  - **Setup.** The importer runs a function and its headers through GCC
    3.2.3's own preprocessor. It keeps only the declarations the function
    reaches (about 43,000 down to a handful), which raised throughput from
    about 44 to about 200 candidates/s on 16 threads. Pruned and full
    builds compile identically for 590 of 590 parseable near misses.
- **The first batch matched 8 of 44 near misses** in 5 minutes each:
  0c03f240, 0c048c60, 0c088320, 0c088480, 0c0b49c0, 0c0ded20, 0c0ee700,
  0c128160. The 44 were picked from classify.tsv: the order / registers /
  values / extend kinds, plus functions within 2 instructions of the game
  by mnemonic count. For comparison, permute.py's last deep pass over all
  near misses found 4. The 8 texts are build/agents/permuter-sh4-1.json.
  0c0b49c0 is the call-register rule again: the permuter got its `jsr @r0`
  by giving `mtx_store` a value-returning declaration, and the text
  matches with `#define NOPROTO_mtx_store`.
- **A second pass on the 9 closest** that were left (scores 10-35, where 5
  is one operand) gave each function 30 minutes on 3 threads. It matched
  3 more: 0c0c1bc0, 0c0ed980 and 0c14f3e0 (build/agents/permuter-sh4-2.json).
  The other six stopped at the scores they had reached in 5 minutes:
  0c0c28e0 and 0c0e7820 at 10; 0c050ec0, 0c087b00 and 0c0cec80 at 20;
  0c025300 at 35. **4,854** functions match.
- **Line breaks change GCC 3.2.3's code.** FUN_0c0b8620, reprinted by the
  permuter's C printer, picks r2 instead of r1 for one value. The tokens
  are identical. Put on a single line, both versions compile the same, so
  the layout alone moved the register. The likely carrier is GCC 3.x's
  line notes, which are emitted without `-g` (not yet confirmed in
  GCC's source). A text is therefore checked exactly as written, never
  reformatted after it matches. The permuter's statement-joining pass
  (`perm_sameline`) is weighted up for this compiler.
- **The local harness (local_match.py), continued**, with Qwen3.5-35B-A3B
  (Q5_K_M, llama.cpp's llama-server):
  - A reasoning guard cuts a streaming reply that repeats lines or
    12-word runs, or that passes a token budget, and has the model finish
    from what it had.
  - A STATE block gives the model the attempt's status and the best text
    so far (IMPROVED / NO_GAIN / WORSE / NO_COMPILE / REPEAT / ...). The
    harness, not the model, decides when to stop.
  - A repeated text is resampled at a higher temperature instead of
    costing an attempt.
  - A `--hunk` mode asks for SEARCH/REPLACE edits to one diff hunk at a
    time and declares undeclared locals itself.

  Results, re-scored with one judge over the 15 fair control functions
  (2,115 game instructions):

  | Run | Diff lines | Instructions in place |
  |---|---|---|
  | decompiler's text | 2,882 | 1,080 |
  | whole function + guard + resampling | **2,310** | **1,316** |
  | hunk mode + declaration repair | 2,642 | 1,192 |

  Still 0 matched. Hunk mode can't make the whole-shape rewrites (a loop's
  form, the control flow) where whole-function mode gains most, so it is
  kept behind `--hunk` only. The verdict on models that fit a 32 GB card
  stands.

## 2026-10-31: agent rounds S-V; the permuter seeded from agent texts; pool detection (4,923)

- **Rounds S, T, U and V** took the last untried near misses, distance
  131 and up (batches 227-236, ten agents, 129 functions):

  | Round | Distance | Batches | Matched |
  |---|---|---|---|
  | S | 131-149 | 227-228 | 10 of 30 |
  | T | 150-169 | 229-230 | 11 of 20 |
  | U | 170-197 | 231-232 | 8 of 19 |
  | V | 200-1225 | 233-236 | 31 of 60 |

  Every near miss has now been tried at least once. The library code
  was among the large ones: zlib's inflate_blocks, inflate_codes,
  inflate_fast, deflate_fast, deflate_slow, send_tree, send_all_trees
  and compress_block (written from zlib 1.1.4 and brought into src/zlib's
  style by build/agents/zlib_style.py: no comments, macros replaced by
  their values, the mode names from vf4ft_types.h); newlib's mktime and
  strftime; and MD5Transform twice (0c08e580, whose callee
  FUN_0c08ef00 is Decode, and 0c11a5e0), each matching once the 64 steps were
  written out in reference order with the constants as values. What
  matched the rest: typed records instead of pointer casts, stores in
  the game's order, a float variable that holds a constant across calls,
  two calls that differ only in a constant (`tst; mov #0; bf; mov #1`),
  and the NOPROTO/KRPROTO guards. 0c126960 is the first text with an
  `fsca` asm statement. Misses are again register ties and loop
  hoisting with the structure matched. Leads:
  build/agents/STUV-header-leads.md (mtx_store is called through r0 by
  45 callers against 6 through r1; FUN_0c0dec00 takes its flag last;
  five functions take char parameters; agg_0c41e010's arrays).
- **The permuter, seeded with agents' best texts** from earlier rounds
  (34 functions, 10 minutes each), matched 9: 0c0903c0, 0c0a2ce0,
  0c0bc840, 0c0d66a0, 0c0dc080, 0c0e55e0, 0c0ed580, 0c0f5ea0 and
  0c1318a0. A tenth (0c0ef860) reached 0 only by changing
  FUN_0c10fa40's return type and is a header lead. The agents' brief now
  asks for a best text for every function they do not match.
- **Pool detection** (classify.insns): a pool word that happened to
  decode as `mov.w`/`mov.l` marked its "target" as pool, and a real
  instruction vanished from the listing (0c0873c0). 195 of 6,082
  functions lost 877 halfwords that way. pc-relative loads only reach
  forward, so a word already known to be pool no longer marks anything.

## 2026-11-01: header pass; the permuter's wider pass; flags of skipped functions (4,929)

- **Header pass** on round S-V's leads (ghidra/symbols/signatures.tsv
  and nlam.tsv rows, applied by the types step):
  - `mtx_store` returns `undefined4 *` (39 callers load it through r0),
    and the six `NOPROTO_mtx_store` defines are gone.
  - FUN_0c0dec00 takes its flag last.
  - 4.3BSD vfprintf's `cvt` (FUN_0c15ade0) and `round` (FUN_0c15b3e0)
    take their double first; cvt then matched without its casts.
  - FUN_0c0c63a0 takes a `vec3` by value, so 0c0c6f60 matched without
    its cast.
  - FUN_0c0f5720 is (float, int, float *).
  - FUN_0c062720 and FUN_0c07e6c0 return int.
  - FUN_0c03f860 takes two chars.
  - DAT_0c28f9c0/c4 are int.
  - `font_char_width[95][6]` is the per-character width table.
  - `maple_dma2_end` replaces a hand-written address in FUN_0c1154e0.

  FUN_0c0ff100 looked int-returning from its one caller (0c100b80 calls
  it through r0), but declaring it int changes its own epilogue. It
  stays void, and the export now guards it so that caller can use
  `NOPROTO_`. Every matched function still matched after each step.
- **The permuter's wider pass** (61 functions within 6 mnemonic diffs,
  5 minutes each) matched 3: 0c03e280, 0c0e65a0 and 0c0feae0. With the
  held 0c0c26e0 and the two uncast texts, 6 more functions match.
- **No helper functions in source/**: a permuter text adopted in round V
  (0c0a2ce0) defined `inline static int inline_fn`. The export refuses
  a function in source/ the program does not have, which stopped this
  export. The same result written as `iVar2 = uVar3 + 1; uVar3 = iVar2
  & 0xff;` matches. 0c0feae0's text, which had the same kind of helper,
  matches with the expression written out.
- **Flags of skipped functions**: diff.py and adopt.py --texts took a
  function's build flags from its newest results row. auto_match writes
  `-` for every function it skips, so a function in an empty-delay-slot
  group got compiled with filled slots once its row said "skipped".
  That happened to 0c0c6f60 after the vec3 change. It would have hit
  every function of a pilot batch, since those are all skipped or fail
  to compile. Both tools now ask the build group, as auto_match does.
  On export49's 600 compiled rows the build group gives the same flags
  as the results file every time.

## 2026-11-01: retry round wave 1, and a pilot of never-tried functions (5,019)

- **Retry batches 237-250** gave each near miss to a second agent. Where
  the first agent had saved its closest text, the batch entry included it
  with that agent's note. Agents matched 91 of 210 (43%). Per batch:
  8, 8, 7, 6, 6, 9, 6, 10, 10, 5, 3, 3, 4 and 6 of about 15. 85 were
  adopted. One (0c0e65a0) the permuter had already matched, and five
  are held (below). Five more matched only through a cast or a header
  change and are logged as leads.
  - **What worked:**
    - copying a matched sibling from the same file;
    - padded-struct access over the big global objects;
    - base pointers declared inside a loop body, which decide what the
      loop pass hoists;
    - early returns in place of Ghidra's value flags;
    - one call per branch so GCC merges the tails itself;
    - parameters copied to locals in the order the prologue copies them;
    - K&R calls with the float arguments first.
  - **New findings:**
    - The store-queue registers (0xff000038/3c) are written without
      `volatile`: a filled delay slot moves past them in the game.
    - 0c136240 calls its callee through the uncached mirror
      (`| 0xa0000000`).
  - **What was left:** almost all register ties with the structure
    already right. These are the next permuter candidates, listed per
    batch in build/agents/retry-header-leads.md.
- **Pilot batch 268**: 15 functions no agent had seen, spread over the
  reasons auto_match skips or fails them. 11 matched:
  - no compile 5/5;
  - float conversion 3/3;
  - register placeholder 2/5;
  - CONCAT 1/2.

  Three of the four misses are hand-written assembly: 0c10c120, 0c108870
  and 0c106494, which use XD/XF and fipr/ftrv and leave callee-saved
  registers unsaved. Most register placeholders and CONCATs come from
  Ghidra getting a callee's arguments or return type wrong. The C
  functions in this pool match far more often than the retries do.
- **Held** (build/agents/held-R1P1.json, 11):
  - cast-only texts waiting for their callee's header fix;
  - `__builtin_expect` in 0c0e5940 and an empty asm barrier in
    0c1452e0, both probably not the original source;
  - 0c10e1c0: group file _unmapped/0c10dba0.c calls FUN_0c12f500
    through r0 in 0c10f0c0, but through r1 in 0c10e1c0 and 0c10e940, so
    two original files share that group;
  - 0c1479c0, which takes a 29-word struct by value and needs that
    typedef in the header.
- **Hand assembly, not C** (do not deal again): 0c10c120, 0c108870,
  0c106494 and 0c0ea8f4. **0c11b240 is not a function**: it is the tail
  of another, a boundary to fix.

## 2026-11-01: never-tried functions, wave 2 (5,095)

- **Batches 269-283**: 15 batches of functions no agent had held, dealt
  from the reasons auto_match skips or fails them (no compile, register
  placeholder, float conversion, CONCAT, stack address, SUBn, ZEXT/SEXT).
  Agents matched 122. Per batch: 7, 8, 6, 9, 8, 10, 10, 11, 7, 6, 9, 7,
  9, 11 and 5. About 74 of the rest are hand-written assembly; agents
  listed them per batch and build/agents/asm_detect.py now screens the
  pool before dealing (FPU instructions GCC never emits, callee-saved
  registers written without a save, starts that are not 4-aligned).
- **76 adopted.** 46 are held for the header pass
  (build/agents/held-U1.json): 32 call a callee through a cast to the
  prototype the game uses, and 14 define a function with a
  signature the header does not have yet (return types Ghidra read as
  longlong, char parameters, const in zlib).
- **Square roots are inline asm.** Ghidra's `SQRT` is `fsqrt`, and 15
  texts wrote it as a `static inline` helper around `__asm__`. Building
  with `-fno-math-errno` keeps all 5,019 functions matching and makes
  `__builtin_sqrtf` a bare fsqrt too, but only 9 of the 15 then match.
  In the other 6 the game takes the root in place and copies it after
  (`fsqrt fr1; fmov fr1,fr0`), which only the tied asm operand gives.
  So the base flags stay as they were and the helpers stay.
- **Fixed: names with two leading underscores.** match.py stripped every
  leading `_` from object symbols, where the toolchain adds only one, so
  `__ieee754_asinf` was reported as `ieee754_asinf`. Adopt then left the
  two newlib functions out although they matched. The same bug gave the
  eight `?` rows in every auto_match run (`_tr_init`, `_cleanup_r`,
  `__kernel_rem_pio2f` and others): their results were never read.

## 2026-11-01: header pass over the held texts (5,145)

- **Texts that change their own signature** (14, held in wave 2) went
  through adopt, which writes the new signature into the header; match.py
  over the tree then checks every caller. Two callers needed care:
  - FUN_0c113e20 matched only while FUN_0c1377a0 was declared to return
    a longlong. With the true int return, it matches when the call sits in
    the return expression rather than in a temporary.
  - FUN_0c12f5e0 takes a second parameter it passes straight through, and
    its one-line wrapper FUN_0c12f620 forwards both. That wrapper compiles
    to the same jump.
- **Fixed in adopt.py**: the signature it writes into the header came from
  the text's first line. In a text that opens with a static inline helper
  (an fsqrt or interrupt-mask wrapper) that line is the helper's, so the
  header kept the old signature and the definition did not compile. It now
  takes the last definition, as `defname` does.
- **Callees whose declarations the cast texts proved wrong**: 14 matched
  definitions edited (return types: FUN_0c1315c0, FUN_0c158100 and
  FUN_0c0a2fe0 return float; FUN_0c0ed5c0 returns uint, its callee's value
  unextended; the http trio and FUN_0c029460 return their callees' values;
  unused parameters dropped from six) and 7 signatures.tsv rows
  (FUN_0c0f55e0 float, FUN_0c137d40 int, FUN_0c0f25c0, FUN_0c11dde0,
  FUN_0c1397ae, FUN_0c12d180, FUN_0c12d1b0). A row for FUN_0c0c78e0 was
  taken back: a matched caller passes it two words and a ray3, the held
  text two vec3, so the two callers disagree.
- **Result**: auto_match then matched 9 more straight from the decompiler
  (6 that had been held with casts, and `_tr_init`, `_cleanup_r`,
  `__ten_mul`, whose results the underscore bug used to hide). 27 held
  texts matched with their casts removed and were adopted.
- **Still held** (9): two calls through va_list (0c0f5500, 0c132640) and two
  structs by value (0c1482e0, 0c1479c0) that need hand types; FUN_0c12c7ca,
  cast two ways by its two callers (0c10ff20, 0c110460); 0c0c7a80
  (FUN_0c0c78e0 above); 0c1452e0, 0c0e5940 and 0c10e1c0 as before.

## 2026-11-01: never-tried functions, wave 3 (5,291)

- **Batches 284-298** (15 agents at once): 173 of 224 matched (77%). Per
  batch: 11, 12, 9, 11, 15, 12, 12, 11, 13, 13, 10, 12, 9, 12 and 11.
  146 adopted; 27 held (build/agents/held-W3.json), almost all for one
  of a few callee families: the va_list printf/scanf family (vsnprintf,
  vsscanf, __svfscanf, vfatal_at), FUN_0c148080's 252-byte struct by value,
  the interrupt-level helpers FUN_0c0fbb20/60 (int, not ulonglong), and
  FUN_0c0c78e0, whose own text wants two vec3 where a matched caller passes
  a ray3.
- **What the skip reasons turned out to be:** register placeholders and
  CONCATs of SR bits are the stc sr / ldc sr interrupt mask (and `stc vbr`)
  that Ghidra drops; `extraout_r0` after a division is the dividend
  __sdivsi3_i4 leaves in r0, so plain `%` works; "stack address" is a
  va_list, written `__builtin_stdarg_start` (GCC 3.2's `__builtin_va_start`
  is a real call) and passed by value. Library code (zlib's inflate and
  deflate entry points, newlib sinf/cosf/copysignf/scalbnf/memmove/memchr,
  the old fdlibm sqrtf, BSD strtol, MD5 Decode) matched from its source.
- **Fixed in adopt.py:** two texts carrying the same static inline helper
  (fsqrt_asm) in one file were a redefinition; compose now leaves out a
  helper the file or an earlier text already defines identically. A helper
  written on one line is not read as a definition: the brief now asks for
  the multi-line form.
- **Not C** (do not deal again): 0c149da0, 0c10d1c0, 0c108968, 0c108a18,
  0c1088ec, 0c1088cc, 0c0dc800. The rest of the misses are register ties
  with the structure right, listed per batch for the permuter in
  build/agents/retry-header-leads.md.

## 2026-11-01: hand assembly; retry wave 4; header pass 2 (5,753)

- **Hand-written assembly as .s files** (tools/match/to_asm.py): the 295
  functions with no C behind them (no r14 frame, `ldc`/`stc` sequences,
  shared literal pools, jump tables in the middle of code) are written out
  from the game's own bytes as 150 files in source/asm, assembled with
  `sh4-elf-as` and compared like the C. A region grows over the pools and
  branch targets past its last routine and stops at a matched function;
  a bsr/bt to another file has no relocation in gas, so it is written as
  `.word 0xNNNN ! bsr _FUN_x`; a routine at a 2-mod-4 address keeps the
  halfword before it so its pc-relative loads stay aligned. match.py
  assembles a .s and the exporter marks those functions matched with
  their declaration only.
- **Retry wave 4** (batches 251-267 and 299, every entry a near miss one
  agent had already tried, its closest text and note given): 126 of 255
  matched. Per batch: 5, 5, 8, 9, 6, 9, 5, 7, 9, 3, 8, 5, 10, 10, 5, 5, 5
  and 12. What fixed them: a matched sibling's text copied first; a base
  pointer held in an int local so the symbol and offset are not folded;
  struct views over raw offsets (`(p + 8)->x` keeps `mov.w; add` where
  `p[8].x` folds); statement or comma-expression temporaries to order
  loads and stop loop.c hoisting; the shared tail written out in both arms
  so cross-jumping merges it; KRPROTO calls for argument load order. New:
  a value the game computes between `tst` and `bt` that only the other
  arm uses is an if-converted `?:`. The misses are almost all register
  priority with the structure right, listed for the permuter in
  build/agents/retry-header-leads.md (closest: 0c08a2a0 one insn,
  0c117be0 4 lines, 0c087360 4, 0c138c40 4, 0c0fc0e0 6, 0c05c6c0 6).
- **Header pass 2** (signatures.tsv rows, ghidra/types): va_list is GCC's
  20-byte record on SH-4, passed by value, so the printf/scanf family
  (vsnprintf, __svfscanf, vsscanf, vfatal_at and their wrappers) takes it
  as one parameter; two functions that had matched as eight ints
  forwarding `(&param_8)[1]` are va_list functions. Structs by value:
  FUN_0c148080 takes a 252-byte block, FUN_0c1479c0 a 116-byte one, and
  FUN_0c0f4820 returns newlib's `struct mallinfo`; a struct that does not
  fit the argument registers goes wholly on the stack and leaves r6/r7
  untouched. FUN_0c0c78e0 takes (int, int, vec3, vec3, float): the ray3
  a caller passed is the two vec3. Return and parameter fixes for
  FUN_0c0fbb20, 0c10df00, 0c1177a0, 0c14d020, 0c1402a0, 0c108a30/32 and
  `__kernel_sinf`'s newlib order (x, y, iy). The NEC copyright text at
  0x0C32A918 is named. With these, 28 held texts matched without casts.
- **File and boundary fixes**: source/_unmapped/0c10dba0.c is two files
  (callers of five helpers read r1 before 0x0C10EE60 and r0 after); adopt
  routes by a `split-from:` line. A symbols row had put
  jvs_save_command_after at 0x0C11B240, which is jvs_index_replies' rts;
  it is at 0x0C11B260 (and matched, with the second loop reusing the
  first's pointer as jvs_save_command_before does). FUN_0c021de0 and
  FUN_0c021a20 are K&R definitions: 0c04a280 passes their char arguments
  unconverted.
- **Saved near misses re-tested** against the new headers
  (retest_best.py): 9 of 335 match now; 6 adopted, 3 held for a callee
  cast (0c059f20, 0c0d3e60, 0c11d600).
- **Result**: 166 more functions match (5,587 to 5,753: 160 from this
  round's texts, 6 from the re-test).
- **Still held**: 0c0e73e0 (FUN_0c100160 called as implicit int: wants a
  NOPROTO define and guard), 0c10fbe0, 0c131000, 0c144cc0, 0c1452e0,
  0c0e5940.

## 2026-11-01: big functions, one agent each; the permuter on wave 4's near misses (5,781)

- **Big functions** (batches 400-417: 18 functions of 900-4,842 bytes, one
  agent each with about 90 minutes, working top-down and saving the
  closest text at every gain): 10 matched outright, 1 matched only with
  `__builtin_expect` (held), 7 are near misses. Matched: vsnprintf
  (0c15b5c0, 2,928 bytes), __svfscanf (0c15c240, 3,676), move_start,
  rob_attack_reach, FUN_0c0f71c0 (2,954), FUN_0c0d24c0 (3,040),
  FUN_0c06cac0, FUN_0c13f700, FUN_0c046100 and FUN_0c03e660.
  - The C library's formatted I/O is two different lineages: vsnprintf
    is the 4.3BSD-Tahoe `_doprnt` (the vfprintf.c with `cvt()`) turned
    into a vsnprintf, with extra `%` and `b` (binary) cases; __svfscanf
    is newlib's, its front end from 1.9.0 and its CT_FLOAT block from
    1.8.2, without the L/D/O cases.
  - Several were rewritten from the game's listing rather than from
    Ghidra's text, which drops dead struct copies that size the frame and
    reuses one variable for unrelated values.
  - GCC 3.2's `va_arg(ap, short)` (or char) compiles to an abort call: the
    game's narrowed loads are `(short)va_arg(ap, int)`.
  - What moved the last lines, beyond the earlier patterns: a pointer
    local to a global that is spilled across calls, so reload puts
    `&sym + offset` back at every use (the "game reloads the address"
    signature); one float local reused for two values; a chained
    assignment `A = B = 0` where the game stores through its own address
    register; splitting an `||` into if / else-if with identical arms,
    whose label ends CSE's block; a goto loop to keep loop.c from hoisting;
    locals declared in spill-slot order (slots follow declaration order).
    Agents read GCC's `-dl -dg -dL` dumps (global.c's allocation order,
    loop.c's hoisting threshold) for the register ties.
  - Header fixes: atof (FUN_0c15d580) returns float (strtod's result);
    FUN_0c0dea20's callers disagree on argument order, so 0c03e660 calls
    it K&R.
  - Near misses: FUN_0c14db00 (3 instruction lines, one float register),
    rob_eval_output (33),
    FUN_0c14f4a0 (58), FUN_0c14c680 (124), FUN_0c0fd280 (230),
    FUN_0c150ba0 (272). FUN_0c147b20 matches only with
    `__builtin_expect(param_6 != 0, 0)` on one branch (GCC predicts the
    plain test 50/50 and fills a delay slot from the target); the source
    shape that gives the game's prediction is not known, so it is held.
- **The SH-4 permuter seeded with wave 4's closest texts** (82 within 40
  diff lines, 5 minutes each): 17 matched, the best yield yet (the first
  batch found 8 of 44 from the decompiler's text). A 15-minute pass on
  the 7 big near misses matched FUN_0c06d740 (from 17 lines); its call to
  FUN_0c047d80 loads the float before the int, so like FUN_0c0dea20 it is
  a K&R call in its file. Two permuter helpers spelled `inline static`
  stopped the export (ExportSource takes them for game functions): they
  are `static inline` with real names now, and adopt.py respells them.
- **Result**: 28 more functions match (5,753 to 5,781).
- **Fixed in adopt.py**: a text carrying `#define KRPROTO_x` (or
  NOPROTO_) did nothing when the header had no guard for x yet; compose
  now adds the guard.

## 2026-11-01: near misses read from GCC's RTL dumps (5,872)

- **One agent pass over every near miss within 40 changed lines** (163
  functions in batches 500-520, 7-8 each, plus the six big near misses
  one per agent, 521-526), each starting from the closest text any agent
  or the permuter had written. The new tool is
  `build/agents/rtl_dump.py ADDRESS TEXT.c OUTDIR`: it compiles a text
  exactly as diff.py does and prints GCC's local-alloc, global-alloc and
  loop dumps with a summary (allocation order and the hard register each
  pseudo got; each loop and how many insns loop.c moved out). Agents
  stopped guessing at register ties and worked from priorities
  (refs x frequency / live length), loop.c's hoisting threshold and
  reload's rotating choice of reload registers.
- **Result**: 100 texts matched; 91 adopted (5,781 to 5,872). Big ones:
  FUN_0c0fd280 (4,842 bytes, the store-queue display-list builder),
  FUN_0c14db00 (1,952) and rob_eval_output (2,818).
  - rob_eval_output calls ik_cos_law with the arguments evaluated in
    the order (float, float, float, int, float *): a K&R call
    (`KRPROTO_ik_cos_law`), like FUN_0c0dea20 and FUN_0c047d80 before it.
    Its 0.0/1.0 registers came out right only with the matrix written
    as an initialized local `vec3 m[3]`.
  - FUN_0c02bca0 takes a char sixth parameter, but its caller in the
    same file passes the int unnarrowed: a K&R definition and call.
  - What moved the last lines, beyond the earlier patterns: branch
    prediction (a value tested against 0 that is a pointer, a guarded
    call, an early return) changes block frequencies and so register
    priority without changing the code; an argument the game loads
    after an earlier argument's `mov` must be a one-insn memory operand
    (GCC precomputes costlier ones into a pseudo); pointer-typed locals
    are always the base of `@(r0,Rn)`; a variable set in both arms of an
    if/else is no longer block-local; one variable per zero register the
    game stores from; Ghidra's `unaff_rNN` return is a variable left
    unset on that path.
- **Held, 9 texts**: 5 match through a cast of a callee whose prototype
  is wrong (FUN_0c0cc660 and FUN_0c043860 return void, FUN_0c10fa40,
  envelope_skin_sq/_nouv return a value, FUN_0c0c7340's parameter order,
  FUN_0c0578c0 takes one argument); 4 match only through compiler
  steering and are held: `__builtin_expect` (0c06e600, with 0c147b20
  from the last round), an invented asm input (0c101000), and dead code
  that pads a loop to the size the game's loop had when loop.c ran
  (0c14f4a0, 0c14c680). The likely real cause of the last two is
  `static inline` helpers, whose parameter copies count at loop time and
  are deleted afterwards. Header leads for a third pass are in
  build/agents/rtl-leads.md.

## 2026-11-01: header pass 3; the held workarounds marked (5,883)

- **Header facts from the RTL round** (ghidra/symbols/signatures.tsv):
  FUN_0c0cc660 and FUN_0c043860 return void (a value-returning callee
  sets r0, which takes r0 out of reload's rotation and moves every later
  reload register); FUN_0c10fa40 returns FUN_0c10fbe0's value;
  envelope_skin_sq and envelope_skin_nouv return `float *` like
  envelope_skin (FUN_0c0e8080 calls all three through one jsr); callers
  of FUN_0c0c7340 evaluate its two floats before its two pointers;
  FUN_0c0578c0 takes one argument. Three globals: a char at 0x0c41f060
  (a member of agg_0c41f05c, now spelled by that name), a char at
  0x0c2a4a8d and a float at 0x0c2a4a90 in place of the 4-byte pointer
  Ghidra had overlapping both.
- **Released**: the five texts that cast those callees, FUN_0c02d7c0 and
  FUN_0c0b5600 (the two globals), and FUN_0c043ec0, which was a loop-size
  miss: the game computes its angle step inside the loop, and loop.c's
  hoisting of it uses up the threshold that otherwise hoists the call
  addresses.
- **Known workarounds, adopted and marked** (`WORKAROUND` comment at the
  line): FUN_0c147b20 and FUN_0c06e600 use `__builtin_expect` to get
  the game's branch prediction; FUN_0c101000's `trapa #0x41` carries an
  input operand the game's sibling does not have, to raise one
  variable's allocation priority. Each matches; the source shape that
  gives the same result without the trick is not known.
- **Result**: 11 more functions match (5,872 to 5,883).

## 2026-11-01: agents on every open function, permuter P2/P3 (5,951)

- **Coverage**: every function still open got an agent this round:
  batches by distance band, one agent per big function, redo pairs for
  the ones a batch never reached, and the 40 nearest misses in fours.
  Permuter runs P2 (69 seeds) and P3 (53) took the agents' best texts as
  seeds.
- **Big functions rewritten from the game's assembly** had the best hit
  rate (FUN_0c0a8b60, FUN_0c03f860 and eleven more): Ghidra's text is a
  poor seed when it missed jump tables or swapped arms; struct locals
  for reload spill slots, nested `switch (param)` for `tst/cmp/eq`
  dispatch, and shared tails written in each arm for cross-jumping.
- **New patterns** (build/agents/BRIEF.md): a variable assigned in two
  arms becomes a multi-block pseudo, which breaks a local-alloc tie and
  avoids block_alloc's 3-quantity sort (it compares by quantity number);
  a call written at the end of both outer arms keeps CSE from reusing
  the callee address; the 8.0-style constant as the else of an inverted
  test gets ifcvt's speculated load between `tst` and `bt`; global.c's
  priority (log2 refs x frequency / live length) reproduced by hand
  settles r12/r13-style ties.
- **Held, matching but not adoptable without a decision** (13, listed
  in build/agents/rtl-leads.md): dead stores, `volatile` reads or flags
  that only steer branch prediction or if-conversion, a constant held in
  a `char *`, a `long long` copy, and one empty `if` the permuter added.
- **Header pass 4 leads** collected in rtl-leads.md, among them
  FUN_0c06b820 `(void)` (releases FUN_0c06ba80), `DAT_0c32a918` as a
  char array (releases FUN_0c144200), FUN_0c0deb80's flag-last order,
  `__kernel_rem_pio2f`'s prototype, and agg_0c450f24 growing to 0x164.
- **Result**: 68 more functions match (5,883 to 5,951).

## 2026-11-02: header pass 4 (5,964)

- **Prototypes from rtl-leads.md** (ghidra/symbols/signatures.tsv, each
  tried first on a header copy over the whole tree, 5951/5951):
  FUN_0c06b820 and FUN_0c13f320 take no arguments, FUN_0c092460 one;
  FUN_0c1102e0 is `int (float *, float *)`; `__kernel_rem_pio2f` has
  newlib's prototype, so __ieee754_rem_pio2f passes `tx` and
  two_over_pi uncast; FUN_0c084a00's fourth parameter is an int;
  FUN_0c10e940 returns int; FUN_0c0e3360 returns a pointer; FUN_0c0deb80
  takes its flag word last, which lets 0c0200c0.c and 0c072320.c drop
  their KRPROTO_ for it. Char and short parameters for FUN_0c0ccc40,
  FUN_0c039de0, FUN_0c0789a0, FUN_0c04b8c0 and FUN_0c145860. Two
  functions Ghidra never made, at 0x0C084A60 (`int (int)`) and
  0x0C08F020 (`void (int)`), are rows too and match as siblings of
  FUN_0c0849c0 and FUN_0c08f000. agg_0c450f24 is 0x164 bytes
  (aggregate_extents.tsv), DAT_0c2eeabc an int. Two leftover casts of
  already fixed callees (FUN_0c0cc660, FUN_0c110e60) are gone.
- **Released**: FUN_0c06ba80, FUN_0c13c200, FUN_0c092180, FUN_0c10fd60,
  FUN_0c10fbe0, FUN_0c081700 and `__kernel_rem_pio2f` without casts;
  FUN_0c059f20 and FUN_0c11d600, which header pass 3 had already
  released but nobody re-tried; FUN_0c0e73e0 and face_skin_update with
  `NOPROTO_` defines in place of `int (*)()` casts (FUN_0c100160 and
  face_static_copy are each called once, through r0, with r0 never
  read: their callers' files had no declaration).
- **Not applied**: DAT_0c2925d4 as a signed char (four matched functions
  read it unsigned; its row says so and keeps it a byte); FUN_0c061fe0's
  char parameter (its matched caller FUN_0c061f80 would extend what it
  passes); char parameters of the matched FUN_0c04e860, FUN_0c069960 and
  FUN_0c075180 (they narrow in the body; char parameters would make every
  caller extend). FUN_0c0ff100 stays void: its one r0 caller, FUN_0c100b80,
  does not match yet. DAT_0c32a918 was already `nec_copyright[622]`; the
  FUN_0c144200 text still needs `volatile` on the +0x208 / +0x21c stores,
  so it stays held with the others.
- **Result**: 13 more functions match (5,951 to 5,964), two of them new.
- **The held texts, decided**: adopt only redundant code that ordinary
  source could contain, marked `WORKAROUND` at the end of an existing
  line (a line of its own could move registers): FUN_0c07ea60's `r = 0`
  that is never read, FUN_0c049180's `c = 1` before a switch whose every
  arm sets c, and FUN_0c14c680's clears of an entry before it is filled
  (its `volatile` view of agg_0c41e010 is the one ten matched files
  already use). Declined, and held for a natural shape: flags, pointers
  or `volatile` that only steer branch prediction or if-conversion
  (FUN_0c145860, FUN_0c09f960, FUN_0c13d920, FUN_0c102dc0, FUN_0c144200),
  register steering (FUN_0c0b5e60's asm clobber, FUN_0c06ebc0's
  constant in a `char *`, FUN_0c0cbb40's local function pointer,
  FUN_0c1452e0's empty asm), FUN_0c0e5a40's `long long` copy and
  FUN_0c10e940's empty `if`. 5,967 functions match.

## 2026-11-02: round X, every open function again (6,007)

- **Coverage**: all 117 open functions, 51 agents (the big ones alone,
  the rest in twos and fours), each starting from every earlier agent's
  best text and notes (build/agents/make_round_x.py). Every claimed
  match went to a second agent that re-ran diff.py and classed the text
  under the held-text rule: clean, redundant (adopted with a
  `WORKAROUND` mark) or declined (held).
- **Result**: 55 claimed, 40 adopted (31 clean, 9 redundant).
  Six were held texts that now have a natural shape: FUN_0c102dc0,
  FUN_0c10e940, FUN_0c145860, FUN_0c09f960 and FUN_0c0cbb40 (its only
  asm is the fsqrt helper eleven files share). FUN_0c144200 matched but
  still reads and writes single fields through `volatile`; the review
  passed it and it was taken back out. 15 more are held under the rule
  (constants in pointer or int locals, `*&param`, one-shot loops and
  empty loops that only move a block boundary, selective `volatile`),
  listed at the end of build/agents/rtl-leads.md; one claim did not
  match on the re-check.
- **Leads for header pass 5** (rtl-leads.md): FUN_0c02c716 is five nops
  of padding before the real FUN_0c02c720, whose text matches byte for
  byte once the symbol starts there; FUN_0c141ce0 is
  `int (void *, int, int)` (releases 0c131000); FUN_0c145e80 needs a
  K&R guard (one caller passes three arguments, the other four);
  FUN_0c0ff100 needs a `NOPROTO_` guard (0c100b80); FUN_0c0b1680's
  family takes the float before the int; FUN_0c0b2c00's param_5 is a
  pointer.
- 6,007 functions match.

## 2026-11-02: header pass 5 (6,009)

- **Prototypes from the round X leads** (ghidra/symbols/signatures.tsv,
  each tried first on a header copy over the whole tree, 6007/6007;
  scripts in build/agents/hp5/): FUN_0c141ce0 is
  `int (void *, int, int)`; FUN_0c145e80 takes three parameters
  (`int, uint, void *`): its body writes r7 before any read, and the
  "four-argument caller" in 0c11f3a0.c was Ghidra's own decompile of its
  one caller FUN_0c144cc0, so no K&R guard is needed. FUN_0c0b1680,
  FUN_0c0b17e0 and FUN_0c0b1940 take the float before the int;
  FUN_0c126140, FUN_0c1261e0 and FUN_0c126260 take the float first;
  FUN_0c1263a0 is a flag word and ten floats (the three unused
  `undefined4` are gone); FUN_0c0b2c00's param_5 is a `char *`. The
  matched definitions of the first eight follow, with their parameters
  renumbered (registers unchanged).
- **A function boundary**: analysis had started FUN_0c02c716 in the nop
  padding after FUN_0c02c620's pool; the code table at 0x0C1631D8 points
  at 0x0C02C720. ApplyTypes now removes a function that starts in nop
  padding before a row's address, so the row for FUN_0c02c720 makes the
  function at its real start (a fresh `run.sh all` does the same).
- **Released**: FUN_0c131000 and FUN_0c144cc0 without casts.
- **Held**: FUN_0c02c720's round X text (w600/best_0c02c716.c) matches
  byte for byte at the new start, but it needs the block-local
  `vec3 *d = &agg_0c370c98` set before the struct copy (a constant
  address in a pointer, which only orders two loads; without it 2 lines
  differ) and a function-level `float *s` that its author chose so
  global.c leaves it unallocated. Not adopted under the held-text rule.
- **Tried, no change**: with the new prototypes the best texts of
  FUN_0c0b1560 (14 lines), FUN_0c0adf20 (17), FUN_0c0b2c00 (149) and
  FUN_0c145e80 (14) differ as before, now without casts. FUN_0c100b80
  with `#define NOPROTO_FUN_0c0ff100` in place of its cast is still 2
  lines off, so the guard (it comes with the first source/ file that
  defines it) is not in the header yet. FUN_0c0cffe0's char parameters
  were already a row; its text is 33 lines off.
- **Not done**: the leads header pass 4 left open (agg_0c41e010
  field_0x1b0 as a uint, FUN_0c061fe0's narrow parameters, the
  DAT_0c3e1bbc / DAT_0c3e1bd0 merge for FUN_0c08ce20) and FUN_0c0cffe0's
  part/item tables as `int[2][4]`: none releases a text yet.
- 6,009 functions match (75 open).

## 2026-11-02: round Y, two angles per batch (6,041)

- **Two agents per batch** (batches 651-682, the 75 open functions;
  workflow build/agents/round_yz.js): one wrote each function again from
  the game's assembly without the earlier texts' statement structure, the
  other started from the best earlier text and read GCC's RTL dumps to
  find the pass where our choice and the game's part. A reviewer reran
  diff.py on every claimed match and classed it under the held-text rule;
  I rechecked every picked text with diff.py (40 of 40 match) and read
  the lines that look like held patterns (build/agents/roundy-adopt/).
- **Adopted 32**: 28 clean, 4 with a redundant line (0c086840,
  0c0b2160, 0c1015c0, 0c13f360). FUN_0c02c720 now matches
  without the pointer header pass 5 held it for. FUN_0c06c320's text had
  two int flags for one condition; folded into `(b & 0x20) && lo == 0`
  it still matches, and that form is the one adopted. FUN_0c0b2160 keeps
  an empty `for (i = 0; i < 2; i++) { }`: the game has that empty
  counting loop (0x0C0B24BE), so it is game code, not padding.
- **Held 8 the reviewers passed** (the reasons are in rtl-leads.md):
  selective volatile (0c144200, 0c128940), block-local pointers holding
  a constant address beside direct use of the same table (0c039de0,
  0c093a00, 0c0d4780, 0c0d4da0, 0c0dff80), and a constant routed through
  a variable for its register (0c08e300). The reviewers declined matches
  for 0c0d6140, 0c043b80, 0c079320, 0c1468a0, 0c0b2c00, 0c0cfa80,
  0c0d4ee0, 0c1446c0, 0c114240 and 0c0ec3e0.
- **Which redundant lines are workarounds**: every redundant line
  adopted so far (these 4, round X's 9, the 3 of 2026-11-02's held-text
  decision) was compiled without it and diffed against the game
  (build/agents/roundy-adopt/unmark.py). Where the game's own
  instructions for the line disappear, the line is game code: 8 of them
  (0c086840 `else idx = 0`, 0c1015c0 `buf[1] = 0`, 0c129f00 `b.n = n`,
  0c14d7e0's second `&= k`, 0c04b8c0's repeated `state != -1` test,
  0c0acb60's first `r.y` store, 0c0aee00's `light` copy and negation,
  0c0c1320's two 0x99999999 stores). Their WORKAROUND mark is now
  "redundant, but the game does it too". The other 8 only move
  registers or blocks and keep the mark: 0c0b2160 `v = p[1]`, 0c13f360
  `z =`, 0c023680's counter wrap, 0c049180 `c = 1`, 0c07ea60 `r = 0`,
  0c09f960 `b = 0`, 0c14c680's entry clears, 0c053c00's reload.
- 6,041 functions match (43 open).

## 2026-11-02: round Z, one agent per batch (6,052)

- **The 43 open functions** (batches 683-699, build/agents/round_yz.js
  with one angle), each agent starting from round Y's best texts and the
  held reasons in rtl-leads.md. 14 claimed, 12 verified by review; I
  rechecked all 12 with diff.py (build/agents/roundz-adopt/).
- **Adopted 11**, among them clean shapes for 8 that round Y held or
  declined: 0c039de0 (a static inline accessor for the spot table),
  0c08e300 (a plain loop), 0c0d4780 (an `int *` table view that every
  access goes through), 0c0d6140, 0c079320 (the sound table at its own
  type), 0c1446c0, and 0c144200 and 0c128940 (a volatile view of the
  whole struct or table, as 0c1411a0.c and 0c14d500.c already have).
  FUN_0c0b2c00's `short m = -1; s *= m;` is game code: the game
  multiplies by a register holding -1 (muls.w), and `-s` compiles to
  neg. FUN_0c1020a0 has one WORKAROUND line (`m = 0xffff`, never read:
  without it only r2 and r3 swap).
- **Held**: 0c0d4da0 (a block-local `int *` holding &DAT_0c2ee9e8, read
  once; reading the table directly moves r13). Declined by review:
  0c0cffe0, 0c0d4ee0, 0c0dff80, 0c114240.
- 6,052 functions match (32 open; 14 of them within 10 diff lines, the
  permuter's next list).

## 2026-11-02: permuter P4, natural shapes, one hand-written routine (6,056)

- **Permuter P4** (decomp-permuter sh4, run_batch.py --seeds, 15 minutes
  each) on the 15 clean near misses within 10 diff lines
  (build/agents/make_p4_seeds.py; texts that already match with a
  declined trick are not seeds). Its two matches both use declined
  tricks: FUN_0c0e5a40 by `long long pa`, FUN_0c0dff80 by wrapping the
  body in a one-line `do { } while (0)`. Neither is adopted.
- **A natural-shapes round** (batches 700-708, build/agents/make_round_n.py,
  round_yz.js with the "clean" angle) on the 11 functions that match only
  with a declined trick and P4's other near misses, each with the
  declined reason and the permuter's closest output. Adopted 3:
  FUN_0c0d8100, FUN_0c0cffe0 (a static inline getter for its two
  tables, without the declined int locals) and FUN_0c114240, whose
  reviewer found the plain if/else that matches without the agent's
  `volatile` and flag. FUN_0c0d4da0 came back as round Z's held text and
  stays held. The other declined ones still need their trick: an int
  holding a table address (0c043b80), a pointer per branch (0c093a00),
  an always-0 row index (0c0d4ee0), the `tp = tgt` alias (0c0cfa80).
- **FUN_0c125380 is hand-written assembly**: a 24-byte data-cache
  write-back loop (`ocbp` over [r4, r4+r5) and once more at the end) with
  no frame, `sub` of a register holding 32 where GCC adds -32. Written by
  tools/match/to_asm.py as source/asm/0c125380.s.
- 6,056 functions match (28 open).

## 2026-11-03: libam named

- **File boundaries**: libam (nlam Version 2.220830 in the binary,
  0x0C114440-0x0C11B17F) is split into twelve files in
  `ghidra/known_files.tsv`, so the export writes them to
  `src/sdk/libam/`: boot.c (cartridge timing, G1 DMA wait, 4 functions),
  getcrc.c (the CRC, 1), maple.c (the Maple driver and MIE bring-up, 11),
  backup.c (the two SRAM banks, 9), eeprom.c (the EEPROM's two settings
  copies, 8), jamma.c (the JVS requests through the MIE, 16), romread.c
  (the cartridge control register and DMA end, 3), userbkup.c and
  usereep.c (the game's own two-copy areas of the SRAM and the EEPROM,
  14 and 9), nlasrvdt.c (copying the BIOS helper code and the calls into
  its table, 23), nlam.c (start-up, board and coin configuration,
  credits, wrappers, clock, LEDs, 46) and nlag1.c (the card units on
  Maple ports 1 and 2 and the MD5 code their key uses, 29). The link
  order is not alphabetical: each boundary sits where the code's subject
  changes, checked against which routines call which, and a routine that
  only one file uses (the old-I/O delay) starts that file. comread.c has
  no code in the binary. The names of boot.c, romread.c and nlag1.c are
  the least certain. The matched C stays in
  `source/_unmapped/0c114440.c` and `source/sdk/libam/libam.c`; nothing
  was moved.
- **Names**: all 173 libam functions have names of our own after what
  they do (159 new, the 14 earlier ones kept with their evidence
  checked against their bodies), and 50 of its globals (47 new: the BIOS helper
  table, the SRAM banks and the time words, the EEPROM image and its
  copies, the Maple frame and reply buffers, the MIE and card unit
  firmware, the card unit queues and buffers, the credit state). The
  MD5 routines carry the names of the public RFC 1321 code. Rows in
  `ghidra/symbols/nlam.tsv`; the routines identified with the help of
  reference material are kept outside this repository. No prototype was
  changed: new rows give the name only and keep the signature Ghidra
  had. The helper table is declared `unsigned int` (as the matched C
  uses it), because an untyped global gets retyped as a pointer and
  would then clash with the matched file's own declaration.
- **Left unnamed**: 24 routines nothing calls are not Ghidra functions
  yet (maple send variants, the uncalled SRAM, cartridge read and CRC
  routines, a few one-line getters) and stay so: making them would add
  functions to the match count. Some globals stay DAT_ where the matched
  C spells them twice (with and without the leading underscore) or they
  sit inside an aggregate.

## 2026-11-03: only intent in matched code; full comments on the odd lines (6,053)

- **The rule, sharpened**: matched code must say what the original
  programmers wrote. A line that exists only to steer GCC (its register
  choice, block layout or branch prediction) is not adopted, even marked.
- **Taken out of source/** (texts kept in
  build/agents/held-unadopted-2026-11-03.json): FUN_0c101000 (an asm input
  operand the game's code does not have), FUN_0c147b20 and FUN_0c06e600
  (`__builtin_expect`, which code of this era would not use).
- **Kept, with a full comment saying so**: 9 redundant lines with no game
  instruction of their own (each one only moves a register or a block when
  compiled without it). They stay because they are plausible original
  code: FUN_0c0b2160 ends in an empty counting loop the game has, which
  points at debug code compiled out of this build, and a store whose reader
  was compiled out stays in the source exactly like these. Their comments
  start "WORKAROUND (kept as possibly original; no game instruction of its
  own)" and say what the line does and what changes without it.
- **Game code that looks redundant**: the 8 lines whose instructions the
  game performs carry "redundant, but game code:" and name the
  instructions; the empty loop in FUN_0c0b2160 and FUN_0c0b2c00's multiply
  by a short holding -1 say why they are written so.
- **The two inline helpers** from rounds Z and N stay: written out by hand,
  FUN_0c039de0 gets another frame size and FUN_0c0cffe0 two fewer
  instructions, more than a register swap, so the original most likely had
  helpers there. Each helper's comment says so.
- 6,053 functions match (31 open).

## 2026-11-03: the system-library block mapped; uncalled code listed

- **0x0C12DA90-0x0C151E7F is three libraries in link order**: the NAOMI
  library (drawing, textures, palettes, system set-up, scalar maths,
  print, fog, lights, matrices, projection) up to 0x0C1358DF; the sy*
  system modules (timer, cache, interrupt chain and handlers, G2 DMA,
  hardware set-up, extension check, address checker, store queues, MMU)
  from 0x0C1358E0; KamuiTL from 0x0C139780 up to zlib. Placed by
  behavior, hardware registers, callers and order, checked against
  reference material kept out of the repo; the version strings are not
  referenced by the code (only the NAOMI library's build date is, from a
  function at 0x0C12F820), so they place nothing on their own. The
  NAOMI library probably starts earlier, near 0x0C126040.
- Names this corrects, when the modules are named: `_cleanup_r` at
  0x0C133EE0 is not newlib's (it pops the library's matrix stack), and
  `mtx_store`, `mtx_push_identity`, `mtx_translate_v` are the library's
  matrix routines, not game code (the last is now `mtx_translate_by_vec3`).
- Two zero-filled buffers sit in .text and are data: 8 KB at
  0x0C1397E0-0x0C13B7DF (a private stack) and 1 KB at
  0x0C151A80-0x0C151E7F. The function entry at 0x0C13B882 is bogus (the
  middle of a function).
- **126 functions in the block were never made functions** (found by
  their GCC prologues on 32-byte boundaries; listed in
  `ghidra/gap_functions.tsv` with what reaches each; four more the scan
  reported were already functions, which `ghidra/functions.tsv`, last
  written 2026-10-21, lists 2 bytes into their prologues). **21 are
  called**: loaded or branched to by known code (12), held in a function
  table in initialized data (3), or reached from code inside a known
  function's range that Ghidra did not follow (6). Those are made
  functions and matched like any other as their modules are named.
  **105 are uncalled**: nothing in
  the image loads, branches to or tabulates them (one, 0x0C132160, is
  reached only from another uncalled one). They are library routines the
  game never uses, left as bytes in the image and not counted as
  functions to match.
- The progress page gains an **Effort** section (`tools/effort.py`):
  calendar and active time, agent time, tokens, and an estimate of the
  same work by hand, refreshed from the session transcripts after every
  pipeline step (`progress/effort.json` keeps the last figures where the
  transcripts are not available).

## 2026-11-03: the sy* system modules named

- **File boundaries**: the system modules (0x0C1358E0-0x0C13977F) are
  fourteen files under `src/sdk/sy/` (`ghidra/known_files.tsv`), placed
  by the hardware each routine drives, which routines call which, and
  the order of their data: the initialized words in .data (0x0C32A70C to
  0x0C32A754), the build-info pointers in .rodata and the version
  strings each run in the same order as the code. sytmr.c (the SH-4
  timers, 10), sycache.c (cache control, range invalidate / write-back /
  prefetch and the hand-written line loops, 25), sychain.c (the interrupt
  hook table, 7), checker_vector.c (the address checker's hand-written
  vector handler, 1), exception_vectors.c (the hand-written exception
  and interrupt entries and dispatchers, 6), g2_dma.c (the four G2 DMA
  channels and the CPU copies over the G2 bus, 9), syhw.c (start-up and
  shut-down, 7), syextchk.c (the G2 expansion device probe, 6),
  address_checker.c (the ADDRESS CHECKER TRAP dump and its serial port
  routines, 8), syint.c (the interrupt set-up, 3), sysq.c (the store
  queue windows, 2), syg2.c (the FIFO waits and the G2 set-up, 5),
  symmu.c (UTLB entries, 2) and g2_lowlevel.c (FIFO status bits, channel
  suspend and reset, the DMA address guard, 6). The file names are the
  modules' own where the binary names them (syTmr, syCache, syChain,
  syExtChk, syInt, sySq, syG2, syMmu, syHw) and ours elsewhere.
- **The G2 code is three objects**, not one in three pieces: each piece
  owns a .data word (0x0C32A710, 0x0C32A750, 0x0C32A754) and the
  hardware set-up, extension probe and interrupt objects' words lie
  between them, which one object's data cannot do. The interrupt code is
  likewise split: the checker's vector handler, the entry and dispatch
  assembly, the address checker, and the C set-up that carries the syInt
  version string.
- **Names**: all 97 functions in the range have names of our own after
  what they do (94 new; cache_prefetch_range, int_break_handler and
  exception_entry kept), and 18 of its globals (the timer callbacks, the
  hook table, the G2 state and its counters, the system bus register
  defaults, the expansion probe flag, the interrupt stack words, the
  trap dump's frame, port choice and register list). Rows in
  `ghidra/symbols/sy_modules.tsv`; the four rows signatures.tsv had for
  this range moved there with their prototypes and evidence. No
  prototype was changed; the two new functions are declared `void (void)`.
  The routines identified with the help of reference material are kept
  outside this repository.
- **Gap functions made**: the two called starts that were not functions,
  0x0C1364E0 (clears every operand cache tag, run uncached) and
  0x0C138160 (the cache shut-down, run uncached), are functions now and
  open (6,053 matched, 33 open). The other two called starts listed for
  the range, 0x0C136260 and 0x0C136380, were already functions (matched),
  as is 0x0C1364A0, listed as uncalled: the table's function list was older
  than the project. The 23 uncalled starts stay bytes.
- **Left unnamed**: the store queue's UTLB entry arrays, the expansion
  probe's result words, the two .data words the start-up passes to the
  interrupt set-up, the words the entry assembly keeps in .text, and the
  cache RAM allocator's words (used only by uncalled code).

## 2026-11-03: the NAOMI library named

- **Where it starts**: at 0x0C12C780, not near 0x0C126040. The C at
  0x0C126040-0x0C12C77F is built like the game (every delay slot empty,
  ending with a cache write-back loop at 0x0C12C760); the library's C from
  0x0C12DAC0 fills its slots. The hand-written assembly between them belongs
  to the library: it works on the library's own state (the palette block at
  0x0C5D4800, the light table at 0x0C41BB4C and its .data words, the matrix
  stack at 0x0C41C860), and the library's matrix C calls most of it, one
  routine each. The library ends at 0x0C1358DF, before the sy timer module.
- **File boundaries** (`ghidra/known_files.tsv`, sixteen files under
  `src/sdk/naomi_library/`, our own names): asm_helpers.c (prefetch, fsca
  sine/cosine, the palette shadow write, 4), light_vertex_asm.c (the light
  set-up that picks the vertex lighting loop, 1), matrix_asm.c (the current
  matrix in the back FPU bank: transforms, stores, loads, products, stack,
  rotations, 39), vector_asm.c (float[3] vectors, 14), draw.c (polygon
  strips and modifier volumes, 17), texture.c (the texture slot table, the
  texture index map and bitmap, the built-in 8x8 texture, 19), palette.c
  (9), version.c (the build-date group, 1), system.c (graphics start-up,
  display changes, render configuration, event hooks, frame statistics,
  stopwatch, error handlers, 62), scalar_math.c (trigonometry on 16-bit
  angles and radians, exp/log/pow, rounding, square roots, the random
  generator, 35), print.c (the debug text overlay, 11), fog.c (6),
  header_bits.c (the polygon header OR words, 6), lights.c (11), matrix.c
  (the matrix stacks and the C side of the matrix code, 45) and
  projection.c (projections, screen points, the target rectangle and pixel
  clipping, with the two hand-written helpers after it, 25). Placed by the
  state each group owns (the .data from 0x0C32A580 to 0x0C32A708 runs in the
  same order as the code), the hardware and the 3D library routines it
  drives, and which routines call which; the split of the assembly at
  0x0C12D860 (matrices, then vectors) follows what the routines work on,
  nothing in the bytes marks it.
- **Names**: 295 of the 303 functions in the range and the two new ones
  have names of our own after what they do (285 new; mtx_pop, mtx_push, the
  three mtx_rot_*_s16, mtx_translate, vec3_length, mtx_store and
  mtx_push_identity kept; mtx_scale_v and mtx_translate_v renamed
  mtx_scale_by_vec3 and mtx_translate_by_vec3, because the old names were
  the reference's own with only the library prefix changed), and 73 globals (texture
  slot and index tables, palette state, text overlay state, the
  modifier-volume contexts, the render and timer words, the random
  generator state, the light table and its flags, the matrix stack slots,
  the saved projection matrices and rectangle, the projection factors in
  .data) plus the build-date text. Rows in the new
  `ghidra/symbols/naomi_library.tsv`; the rows face.tsv, motion.tsv,
  signatures.tsv and newlib.tsv had for this range moved there with their
  prototypes and evidence. No prototype was changed or added. The routines
  identified with the help of reference material are kept outside this
  repository.
- **Names corrected**: `_cleanup_r` (0x0C133EE0) is not newlib's: it lowers
  the matrix stack's depth by n through the assembly routine at 0x0C12D2E0
  without reloading the current matrix; now `mtx_stack_drop`. `mtx_store`,
  `mtx_push_identity` and `mtx_translate_v` (now `mtx_translate_by_vec3`)
  are the library's matrix routines, not game code.
- **Gap functions**: 0x0C12F860 (copies the clip stretch step; its address
  is handed to the 3D library at start-up and on a display change) and
  0x0C130EC0 (the empty error handler the error-mode routine installs for a
  non-zero mode) are functions now, and open. 0x0C132F20 was listed as
  called, but its only "reach" is a literal pool word (0x0C32A5BC at
  0x0C1323A4) read as a branch: it is uncalled, and the gap table says so.
  Three called starts in the light assembly were never made functions and
  are not in the gap table (no GCC prologue): 0x0C12CA14 and 0x0C12CB38,
  whose addresses the light set-up stores in the vertex lighting pointer at
  0x0C5D4810, and 0x0C12CD00, called from 0x0C12CB38. Named
  (light_vertex_single, light_vertex_multi, lights_reaching_sphere: the
  last returns the mask of lights whose range reaches a sphere), made
  functions and written as matching assembly by tools/match/to_asm.py:
  6056 of 6056 functions match.
- **Left unnamed**: eight functions in system.c whose effect the code does
  not show (0x0C1305E0, 0x0C130600 and 0x0C130FC0 pass two counts to a
  3D library routine; 0x0C130B00 and 0x0C130B20 subtract words of the frame
  statistics record; 0x0C130F20, 0x0C130FE0 and 0x0C1310E0 are one-line
  wrappers of 3D library state routines); globals declared with different
  types in different source files (0x0C41DF88, 0x0C41794C, 0x0C418458,
  0x0C5D4800, 0x0C41BB44); the lighting path word 0x0C32A5D8, which the
  matched C declares unsigned while the sign pass makes a named word int
  (it compares signed); and the aggregates (the matrix stack record
  0x0C41C860, the render configuration block 0x0C417A60, the ambient colour
  0x0C41C64C); and the uncalled assembly and gap starts, which stay bytes.

## 2026-11-03: KamuiTL named

- **The range**: 0x0C139780-0x0C151E7F, the graphics library that carries
  the version text "KamuiTL Ver 1.10[R4]": after the last sy module and
  before zlib. Its initialized .data is 0x0C32A75C-0x0C32AC6B and runs in
  the same order as the code, which is what most file boundaries rest on.
- **File boundaries** (`ghidra/known_files.tsv`, 25 files under
  `src/sdk/kamui/`, our own names; the export puts all 350 functions of the
  range there): asm_helpers.c (the hand-written copies, the OR-merge copy,
  the cache line write-back and the interrupt stack switch, 5),
  scene_render.c (13), fb_surfaces.c (2), strip_context.c (7),
  list_writes.c (3), list_buffers.c (20), hooks.c (23), display.c (8),
  pvr_settings.c (22), texture_surfaces.c (38), lib_state.c (6),
  vram_alloc.c (47), runtime_settings.c (4), cfg_lookup.c (1),
  hw_defaults.c (4), sync_timing.c (8), pvr_irq.c (23), chip_access.c (15),
  bg_plane_loaders.c (24), ta_regions.c (9), elan_commands.c (12),
  tile_split.c (7), transfer_queue.c (24), elan_status.c (15) and
  elan_setup.c (10). How sure: firm at both ends for lib_state.c,
  bg_plane_loaders.c, ta_regions.c, elan_commands.c, tile_split.c,
  transfer_queue.c and elan_status.c, because GCC emits a header's static
  functions in every object that includes it, used or not: identical
  copies of a 24-byte border-colour halt check (0x0C1489A0, 0x0C14D3A0,
  0x0C14E740, 0x0C14EC20, 0x0C1502C0, 0x0C1514C0) and of a store queue
  save/restore pair open six objects, and their .data words sit in code
  order. Firm starts for asm_helpers.c, pvr_irq.c, elan_setup.c and
  vram_alloc.c. Placed by single .data words for the scene_render.c /
  fb_surfaces.c split, strip_context.c, list_writes.c and list_buffers.c
  (likely). The objects with no .data in the middle (hooks.c, display.c,
  pvr_settings.c, runtime_settings.c, cfg_lookup.c, hw_defaults.c,
  sync_timing.c, the start of chip_access.c, the end of vram_alloc.c) are
  bounded by behaviour and reference material outside the repo only; the
  evidence column says so for each.
- **Names**: all 333 functions of the range but one, and the 16 called gap
  functions, have names of our own after what they do (none had a name
  before), plus 76 globals (the frame buffer pair word, the strip header
  tables, the library state pointer, the allocator lists and pools, the
  hook slots, the background plane loader table, the ELAN and transfer
  queue words, the store queue save pointer) beside the existing NEC text
  row. Rows in the new `ghidra/symbols/kamui.tsv`; the ten
  signatures.tsv rows for this range (and the NEC text row) moved there
  with their prototypes and evidence. No prototype was changed or added;
  the prototypes pass gave five of the new interrupt-status gap functions
  one int parameter from their bodies. Routines identified with the help
  of reference material are kept outside this repository.
- **Buffers in .text**: 0x0C1397E0-0x0C13B7DF is an 8 KB zero-filled stack
  that the interrupt entry switches to (its top is 0x0C13B7E0);
  0x0C151A80-0x0C151E7F is a 1 KB zero-filled store queue save stack (its
  pointer is set when the ELAN layer resets). Both are data, not code.
- **The bogus entry at 0x0C13B882**: the real routine starts at
  0x0C13B86C. It saves r15 in the word at 0x0C13B7E4, switches to the
  library stack, saves the FPU registers, MACL, MACH and PR, calls the
  handler whose address is in the word at 0x0C13B868 (the main graphics
  interrupt routine at 0x0C149DA0), restores and returns 0. The interrupt
  install routine at 0x0C148B80 stores 0x0C149DA0 in 0x0C13B868 and
  0x0C13B86C in the VBR table slot +0x224. Ghidra's function starts 0x16
  bytes in; source/asm/0c13b882.s carries the bytes before it as a stray
  word, and 0x0C13B86C-0x0C13B87F was in no function. Fixed: the types
  step now removes the function starts listed in the new
  `ghidra/bogus_entries.tsv` (address, evidence) before it applies the
  symbol rows, so the row at 0x0C13B86C (`irq_entry_on_private_stack`)
  makes the routine at its real start; its assembly was regenerated with
  tools/match/to_asm.py (it matches) and source/asm/0c13b882.s removed.
  The uncalled routine at 0x0C13B7E8, which calls a function on that
  stack, stays bytes.
- **Gap functions**: all 16 called claims checked and true (the eight
  do-nothing hook defaults whose addresses the hook defaults store, five
  interrupt-status handlers reached through the main interrupt routine's
  jump table, the three refusing background plane loaders in the loader
  table); the 36 uncalled ones have no word or branch reaching them. The
  gap table needed no correction. The 16 are functions now, and open. No
  hand-written entry was made a function (the only called one is
  0x0C13B86C above). 6056 of 6056 functions match.
- **Left unnamed**: 0x0C13B882 (above); globals declared with different
  types in different matched files (0x0C41DF88, 0x0C41DFFC); words whose
  use the code does not settle (0x0C32A760, 0x0C32A76C, 0x0C32ABE4,
  0x0C32AC40, 0x0C41EA04, 0x0C600D24); the aggregates (the library state
  block 0x0C41E010, the list bank records at 0x0C41E594, and 0x0C41DF94,
  0x0C32ABA4, 0x0C41E9F0, 0x0C41EA10, 0x0C41EB20, 0x0C606FFC); and the two
  .text words of the interrupt entry. With these names in place, the NAOMI
  library's one-line wrappers left unnamed (0x0C130F20, 0x0C130FE0,
  0x0C1310E0, and 0x0C1305E0 / 0x0C130600 / 0x0C130FC0, which set the stall
  limits) can now be named after the routines they wrap.

## 2026-11-03: the NAOMI2 library named

- **The range**: 0x0C0FBEE0-0x0C10425F, the library whose build string is
  "NAOMI2 LIBRARY Ver 0.03 SOFT1 R&D" (2004-04-05), with the NLOBJPUT
  module ("NLOBJPUT Ver 0.99") at its end; after the game's hand-written
  routine at 0x0C0FBE60 and before the hand-written ELAN routines at
  0x0C104260. It runs on past 0x0C1041A0 (the end in the build map): the
  float twin at 0x0C1041A0 and two uncalled setters are its C. No
  reference material for this library exists in or out of the repo, so
  every name comes from the code alone; the public homebrew library
  libnaomi (DragonMinded, `ta.h`) was read for the meaning of a few tile
  accelerator control-word bits (cited in 4 rows), never for code or
  names.
- **What it is**: an ELAN (the NAOMI 2 geometry processor) layer over
  KamuiTL: it compiles models into ELAN command lists (strip, light,
  material and matrix commands, written through the store queues), draws
  and links them per list, loads and releases their textures, sends the
  projection, keeps the double-buffered ELAN command area, and packs the
  lights. NLOBJPUT draws objects (polygon records of 0x50 bytes) with their
  textures, modifier volumes and lighting vectors.
- **File boundaries** (`ghidra/known_files.tsv`, two files under
  `src/sdk/naomi2_library/`): naomi2_library.c (96 functions) and
  object_put.c (NLOBJPUT, 54). The main file is one object as far as the
  bytes show: its .rodata starts with a bit mask table that only the light
  code at 0x0C101CE0-0x0C1026A0 uses, then runs in code order through the
  model code's tables and the version string, so the light code is not a
  later object than the model code. The finer splits the call clusters
  suggest (light commands, model compile, model draw, model textures,
  projection) have no byte evidence and are not made. NLOBJPUT's start
  (0x0C1029A0; 0x0C1029C0 or 0x0C102A00 are possible) rests on its
  .rodata, .data and .bss each starting right after the main file's.
- **Names**: all 150 functions (148 match; 0x0C0FC220 and 0x0C101000
  are still open) and 49 globals, ours, in the new
  `ghidra/symbols/naomi2_library.tsv` (149 function rows; model_size
  keeps its row in poly.tsv). Eleven globals that source/ knows through
  Ghidra's aggregate labels (the lighting vectors, the ELAN light modes,
  the tile split arguments, the matrix command record, the normal block
  scale) are left unnamed, since a name stops the aggregate pass from
  making the struct those files declare; the evidence names them by
  address. Each named global is declared with the type the
  matched files pin (`undefined4`, `undefined *`), as KamuiTL's are, its
  layout given in the evidence; where files pinned two different types
  (0x0C3DC1B0, 0x0C3DC1B8), the files with the other type keep the
  DAT_ spelling. Functions get bare names: the parameter lists in the
  evidence are not yet prototypes. 6075 of 6075 functions still match.
- **Uncalled code**: 39 more starts Ghidra never made (24 in the main
  file, 15 in NLOBJPUT: heap mark and release, light mode setters, colour
  setters, the module's two version-string getters, a busy delay, a
  hand-written matrix variant at 0x0C101AA0), all unreached (no load,
  branch or table entry; one apparent branch at 0x0C0FFE44 is a literal
  pool word). They are added to `ghidra/gap_functions.tsv`, which gains a
  `what_it_does` column, and are not counted as functions to match.
- **Corrections**: 0x0C100B80 is the projection send, not the library
  start-up (that is 0x0C100EC0; its version getter at 0x0C100EA0 is one of
  the uncalled starts); 0x0C1034E0 is an empty per-polygon hook, the
  NLOBJPUT date getter is the uncalled 0x0C103500; the "sound RAM" and
  "TA FIFO" hits of the hardware scan here were constants, not addresses.
  The ELAN routine after the library starts at 0x0C104260 (`mov r15,r1`),
  two bytes before Ghidra's entry; source/asm/0c104262.s carries that word
  before its label, to be fixed when that block is mapped.

## 2026-11-03: library B named (libspr, libintr, libsnd)

- **The range**: 0x0C10DB80-0x0C11443F, three Sega library modules built
  on 2004-04-05 and named by their own strings: libspr ("libspr Ver 0.21",
  17:12:21) with two objects after it, libintr ("libintr Ver 1.051",
  17:10:46) and libsnd ("libsnd Ver.1.05", 17:12:32). The routine at
  0x0C10DAF0 is hand-written ELAN code, not libspr's start; it is left for
  the ELAN block's naming. The NAOMI 1 interfaces of these three modules
  (reference material kept out of the repo) helped recognise routines;
  the public homebrew library libnaomi was read for AICA, G2 and interrupt
  register meanings only. The names and evidence are ours.
- **What it is**: libspr builds sprite records (work area, make, texture
  refresh, drop, palette-group swap) and writes sprite quads to the tile
  accelerator. The strip object after it sends textured and untextured
  strips through context records. The line object draws 3D lines as
  screen-space quads, with near-plane and screen clipping. libintr relays
  the PVR and on-chip interrupts to per-slot functions, installs them in
  the VBR vector area, and sets the INTC priority fields. libsnd talks to
  the AICA sound driver: G2 word access, the mailbox handshakes, DTPK bank
  slots, the driver request ring, the PCM ports and the sound RAM DMA
  queue.
- **File boundaries** (`ghidra/known_files.tsv`, five files):
  sdk/libspr/sprite.c (0x0C10DB80), sdk/libspr/strips.c (0x0C10EE20),
  sdk/libspr/lines.c (0x0C10FA00), sdk/libintr/irq_slots.c (0x0C110A60) and
  sdk/libsnd/libsnd.c (0x0C111120). The strip object has its own build
  date (17:12:23), two seconds after libspr's, which opens its .rodata. It
  may be a library of its own; it sits under libspr by position and build
  time. The line object has no version string and sits under libspr by
  position. libintr and libsnd are each one object as far as the bytes
  show; libsnd's .rodata runs in reverse code order.
- **Names**: 164 functions and 61 globals, in the new
  `ghidra/symbols/libspr.tsv` (68 functions), `libintr.tsv` (26) and
  `libsnd.tsv` (70). Ten of the functions are starts Ghidra never made
  that libintr's .data tables reach (the per-slot store routines). Their
  symbols rows now make them functions, and they are matched: 9 from the
  decompiler's text, and 0x0C110DE0 as "fetch the slot's INTEVT code,
  read VBR, store into the vector slot". The decompiler's form read VBR
  too early. 6085 of 6085 functions match. As with the NAOMI2 library,
  globals that source/ knows only through aggregate labels are left
  unnamed (the sprite coordinate extent, the 3D line piece record, the
  relay slot table). Two whose type AssignPointers changed after their row
  applied (the DMA ring head and tail) keep the DAT_ spelling in the two
  files that pin the older type.
- **Uncalled code**: 31 more starts Ghidra never made, all unreached:
  getters for the build date and version, setters, and the PCM and DMA
  routines nothing calls. They are added to `ghidra/gap_functions.tsv`
  (196 rows).
- **Likely original bugs**, kept as they are: snd_dma_channel_give_back
  never advances its handle pointer, so it checks the first handle 16
  times (only the uncalled sound_off_dma_release reaches it).
  intc_ipr_level_write accepts source 12, but irq_ipr_fields has 12
  entries, so source 12 would read the next .data word.
- **Fix to the NAOMI2 commit (2c85381)**: tools/rename.py had renamed two
  globals inside nine hand-written ELAN routines (source/asm/*.s) that
  were later left unnamed. Their relocations could no longer be checked,
  so those routines showed as `match*`. The names in source/ are back to
  `DAT_`/`_DAT_`, and `match*` is 0 again. From now on, a global whose
  name is dropped is reverted in the .s files too.

## 2026-11-03: the ELAN renderers and the network block named

- **The ELAN renderers** (0x0C104260-0x0C10DB7F, between NLOBJPUT and
  libspr): hand-written SH-4 assembly that NLOBJPUT runs to light,
  clip and send objects to the tile accelerator through the store queues.
  There are 145 entries and 20 globals, in the new
  `ghidra/symbols/elan_asm.tsv`, and 11 files in known_files.tsv
  (`sdk/naomi2_library/elan_*.c`). Each family is cut where zero fill sits
  between routines, since padding inside a family is nop fill. The
  families:
  - the two vertex-colour strip senders (the obj_vertex_hook kinds 1 and 2);
  - the lighting renderer and its offset-colour twin;
  - the modifier volume renderer;
  - the floating-colour, intensity, intensity-spill, bump and
    vertex-colour renderers;
  - the typed dispatch.

  Twins share a name except for the difference. The entries that take an
  alpha in fr4 end in `_alpha`; the two-byte entries before them load 1.0.
  No reference material covers this code, so the names come from the
  code.
- **Starts corrected**:
  - The vertex-colour senders start at 0x0C104260 and 0x0C104CF0
    (`mov r15,r1`), two bytes before Ghidra's entries. Their hook entries
    0x0C10426A and 0x0C104CFA, the ones obj_vertex_hook_select stores, are
    functions of their own now. Its text names them instead of "entry + 8".
  - 0x0C1089C0, 0x0C10A3C0, 0x0C10A430, 0x0C10B640 and 0x0C10B642 are
    starts Ghidra never made.
  - 0x0C107E02, 0x0C108A54, 0x0C10B664 and 0x0C10D1FA were points inside
    routines, not starts; they are in `ghidra/bogus_entries.tsv` with
    0x0C104262 and 0x0C104CF2.
  - ApplyTypes now makes new functions from the highest address down, so
    a routine with a row part way in ends at that row. The 23 assembly
    files were written again by tools/match/to_asm.py from the corrected
    starts.
  - Four words and buffers the renderers keep in .text (two slot tables, a
    scale word, the bump clip ring) have no rows, because code and data
    share those spans. The evidence gives their addresses.
- **The network block** (0x0C11CF60-0x0C12603F) is built with the game.
  It has three parts:
  - **libnl2http**: the HTTP client the game's VF.NET code uses. Its files
    are http.c, map.c and naomi_socket.c, named by the assert paths, plus
    text_util.c (ours) for the token lists and base64. 69 functions, in
    `ghidra/symbols/libnl2http.tsv`.
  - **NLASOCK** (WOW Entertainment, Ver.0.12): sockets that run on the
    DIMM network board, with 132 functions in `nlasock.tsv`. Requests go
    through slot records in a DIMM memory window and come back through a
    BIOS hook word and a 64-entry handler table. Its 25 files include one
    per socket request, because each request opens with its own
    byte-identical copy of a pack helper, as a static helper compiled into
    each file gives. The test program links NLASOCK in the same order, so
    it cannot confirm that split.
  - **NLADIMM** (Ver.0.06): the board record, the firmware version and
    the posted commands, with 12 functions in `nladimm.tsv`.

  0x0C125380, the hand-written purge loop formerly called
  dcache_write_back, is `dcache_range_purge`. It uses ocbp, so it
  invalidates as well as writing back. The one-row `naomi2lib.tsv` is gone.
- **The graphics library** (0x0C126040-0x0C12C77F): built with the game,
  over KamuiTL and the two NAOMI libraries, with no strings of its own.
  169 functions are in `gfxlib.tsv` and 12 files in `src/gfxlib/`
  (directory and file names ours). The modules are:
  - the stage water surface and the FFT it steps with;
  - render to texture and render-to-texture passes;
  - recorded list banks, model merging and screen primitives;
  - display-list capture, a VRAM pool and a code patcher.

  `work/test.bin` links the same objects, byte-identical, in another
  order and leaves some out. The places where an object's neighbours
  differ between the two programs are its file boundaries.
- **alAb** is not in this range. Its version text sits among vf4lib's
  strings, and its pointer sits in .rodata before the NAOMI2 library's,
  so its code comes before 0x0C0FBEE0.
- **Matching**: of the three reached starts Ghidra never made, the peer
  reply handler and the command 0x24 hook match from the decompiler's
  text. ctl_reply_take is held for the user: it matches with the id
  taken as an int and narrowed in the test, and with the call record
  reached through one typed pointer that both stores use. The game loads
  the record's address before the branch. The pointer is the only way the
  function reaches the record, but it is a constant address in a pointer
  local (build/agents/rtl-leads.md). 6090 of 6090 functions match. 27 more
  unreached starts go into `ghidra/gap_functions.tsv` (223 rows).
- **Likely original bugs**, all kept:
  - the accept and recvfrom address copy-backs copy the larger of the
    caller's length and 16, not the smaller;
  - sendto always sends 16 address bytes;
  - libnl2http's "bad host name" error can never be raised, and its state
    machine's state 8 falls into state 9;
  - the game-called water_colors_set never stores its second float;
  - verts_xform_in_place and dlist_hdr_and_mask disagree on a vertex
    field's size.

## 2026-11-03: matching round BB (natural shapes)

Seven agents took the 33 open functions (batches 720-732) with one
angle: find the shape an original programmer would have written, using
the library names that now exist. I re-checked every claimed match
against the game with diff.py and against the held-text rules.

- **Adopted, 7**: packed_chunk_vram_take (the shape of its matched
  sibling in vram_alloc.c; its `aligned = p->addr;` is game code, the
  diff is 47 lines without it), 0c06e600 (two `if ... return;` tests in
  one case merged into one `||` test, which settles an allocation tie),
  0c0b5e60 (a plain `for` loop with `lcg_next15() % 0x10000`, as its
  neighbour), fb_top_lines_skip_set (the whole-struct volatile view of
  the TA state its sibling uses), 0c029be0 (the credit snapshot as a
  typed struct), 0c0426a0 (the camera test as a `?:` condition), and
  pvrirq_sq_save (its already matched twin elanhw_sq_save with the callee
  renamed). With the five below, 6102 of 6102 functions match; 21 are unmatched.
- **Stale prior texts**: texts from earlier rounds call callees by FUN_
  names the header has since renamed, so those calls compile as
  undeclared (through r0). That alone hid several old matches and
  inflated starting diffs. Rename callees before measuring a prior.
- **Shapes taken from matched code** (the maintainer: "go with how it appears the
  original programmers wrote it"; we can change them when more is
  known): 0c0d8ac0 (an inner-loop `int *tb` beside direct uses, as its
  matched neighbour FUN_0c0d8c60 has), 0c043b80 (the table address in an
  int, as FUN_0c043960 reads it), 0c0d4ee0 (a row index that stays 0
  and block-local table pointers, as FUN_0c0d4560 has; the game really
  multiplies by that 0), packed_chunk_take_range_at (`tb`, as its
  sibling vram_take_range_at) and ctl_reply_take (the call record
  through its only pointer, loaded once before the branch as the game
  does). Each such line carries an end-of-line "ORIGINAL SHAPE (assumed
  from matched code; revisit if more turns up)" comment with the
  evidence. Still held: render_done_irq, whose match moves the declined
  second pointer into an invented inline helper.
- **Comment added**: `aligned = p->addr;` in packed_chunk_vram_take and
  vram_take_page_aligned_fit is never read afterwards, but the game
  performs the load (`mov.l @(12,r3),r7`), so both now say "redundant,
  but game code".
- **Not C**: 0c0ea8f4 has no frame, starts with `bra` and fills its
  delay slots; it is hand-written assembly, now `source/asm/0c0ea8f4.s`.
- **The older held texts under the same rule**: of the held texts whose
  functions were still open, three use shapes matched code has: 0c0d4da0
  (exactly the block-local `b`, `int *t` and `k = 0` of its matched caller
  FUN_0c0d4560), 0c0cfa80 (`float *tp = tgt`, as matched code writes
  `float *o = pt;` in field.c) and 0c093a00 (a block-local pointer to its
  work matrix, the block-local pointer-to-global idiom of matched game
  code). Adopted with ORIGINAL SHAPE comments. Still held, with no
  precedent: 0c06ebc0 (0x252 in a `char *`), 0c0dff80 (`char c[1]` and
  an assignment inside an expression), 0c0ec3e0 (a one-shot loop),
  0c0adf20 (a flag in a one-field struct), 0c0e5a40 (`long long`),
  0c101000 (an asm input), 0c147b20 (`__builtin_expect`), render_done_irq.
  6106 of 6106 functions match; 17 are unmatched.
- **test.bin takes today's names**: the test step carried main.bin's
  names over again (TransferFromMain), so 839 more test.bin functions
  have names (1,048 still placeholders) and 318 globals. The transfer
  kept a name but not the type where test.bin's analysis had already made
  a string, which exported as `extern string nec_copyright;` and stopped
  the test build; it now puts main.bin's declared type over an
  analysis-made string. test.bin's 46 matched functions still match.

## 2026-11-03: new angles on the 17 open functions (none matched)

- **Twins** (build/agents/twins.py: instruction sequences with
  registers masked, against every function): no open function has a
  matched near-copy above 0.62, except elan_lights_queue and
  elan_light_queue (0.73), which the agents had already used.
- **Compiler flags** (build/agents/flag_sweep.py): 28 flag sets over the
  16 best texts. The base flags are best for 14; -fgcse and -mspace each
  save 3-4 lines on one function. The libraries were built like the
  rest of the game, so what is left is the source's shape.
- **Permuter P5** from round BB's best texts (their callees renamed): two
  matches, both declined: 0c0e5a40 by `long long pa` again, and
  render_config_from_list_sizes by reusing `r` to hold param_4 for one
  of five otherwise identical lines (it only raises r's count over s).
- **0c0e5a40** has one caller, FUN_0c0e5dc0, which passes exactly
  (object, i); nothing reaches it any other way. The matched caller
  gets the same `+64` base register from plain `param_1 + offset` code.
  The one instruction left is where the object pointer's copy lands.
- **render_hang_recover**: its loop's link arms as inline helpers are
  worse in every form.
- **0c0dff80**: the flag store through one record at 0x0C4DD828 gives the
  game's re-read of the count (32 -> 9 lines), but the game addresses the
  flag table by its own constant, so the table is probably its own
  global and the re-read has another cause. A volatile count gives 3
  lines; matched code uses the count plainly, so it is not adopted.

## 2026-11-03: libbase, libdebug, libcore and alAb named

- **The ranges**: libbase 0x0C020000-0x0C04FDDF and the stretch from
  win_sys.c's tail to the NAOMI2 library (0x0C0ECE40-0x0C0FBEDF). Ten
  agents named one sub-range each from the game's own strings (debug-menu
  labels, task names, file names, error texts), with evidence, and
  proposed file boundaries from .data, .rodata, .bss and string order.
  Every one of the 915 unnamed functions there now has a name: 655 in the
  new `ghidra/symbols/libbase.tsv`, 94 in `libdebug.tsv`, 76 in
  `libcore.tsv`, 89 in `alab.tsv`, 1 in `naomi2_library.tsv`. There are
  492 data rows. 60 more data names were dropped because source/ knows those
  objects only through aggregate labels (as in the earlier merges); the
  evidence says "the block at <addr>" for them.
- **Files** (`ghidra/known_files.tsv`, 45 rows; `docs/source-tree.md`):
  - **console.c** is the HUD and its screen messages (gauges, timer,
    ROUND / READY / K.O. words, the ranking pages, the card panel). It runs
    from the game-message loader at 0x0C033620 to 0x0C0420FF. Its one
    assert is at line 7301. Its .bss is one interleaved block used across
    the whole span, which separate objects would lay out in code order.
  - **New libbase files of ours**: the task tree (task.c, the task
    records named by their own strings: REP_LOAD, ROB_CTRL, CAMERA ...),
    camera.c, the attract demo (adv_demo.c), title_disp.c, ending_roll.c,
    game_stats.c, appose.c, name_entry.c, light.c, stage_fog.c,
    bg_color.c, input.c, select.c, elan_heap.c and others.
  - **The stretch after win_sys.c is libdebug, not libcore.** It holds
    debug_menu.c (the debug windows' callbacks) and chara_load.c (the
    colour entries and the body and parts loaders). libcore is fmath.c
    from 0x0C0F1900 on, then fatal.c, tm_sys.c, sffs.c, malloc.c, clip.c,
    load.c, strbuf.c, fmath_asm.c and gz.c.
  - **alAb** is the naominet.jp client at 0x0C0F6140-0x0C0FBB5F, with its
    block heap after it (`sdk/alab/`).
  - Weak seams are marked in each row's evidence (chara_data.c,
    sound_table.c, stage_fog.c, the debug_menu / chara_load split).
- **Merge fixes**:
  - Seven addresses were named by two agents. The names kept are
    prize_kind, prize_fly_state, game_mes_table, texcol_reload_pending,
    naominet_ready, emblem_sprites and card_panel_row_y.
  - gz_buffer keeps its name. The agent's name for it came from the
    static heap record's own string.
  - The function at 0x0C0F4820 is main_heap_mallinfo: the type headers
    declare `mallinfo` as a typedef, so the function cannot use that
    name.
  - tools/rename.py's "name already in use" check now skips string
    literals and comments. A game string often names its own object
    ("heap_scr_obj").
  - The method-name table's enum-step label (`e_names_0c325544_names`)
    is now http_client_method_names in source/.
  - tools/rename.py no longer rewrites address spellings (FUN_/DAT_
    <addr>) in source_test/ or symbols_test/. Those spell test.bin's own
    addresses, which are other objects than main.bin's at the same
    address. This merge had renamed two test.bin functions by main.bin's
    names, so 44 of 46 matched. With the change, test.bin's 46 match
    again. Earlier merges did not touch source_test/.
- **Matching**: 6106 of 6106 functions match with the new names; the
  private-name and echo checks are clean.
- **New functions**: 60 starts that Ghidra never made are now functions
  and match:
  - **Rows**: libbase 55, libdebug 4, libcore 1. They include
    rep_save_init, the break-register setters, camera_menu_open,
    cam_mode_pointstage, dimm_dma_done_release and mothead_sound_init.
  - **The attract demo's tables**: they reach the 21 script commands
    (demo_cmd_00_bytes_store ... demo_cmd_20_end), the demo_scene_NN_init
    and the two scene draws. They are listed by
    build/game-names/demo_tables.py.
  - **How they matched**:
    - 34 from the decompiler's text.
    - 15 scene inits in one shape (a post-incremented short pointer
      through the phase list).
    - 10 by hand, five of them in shapes their matched siblings use and
      marked ORIGINAL SHAPE. Those five are demo_rob_update's entry
      struct, mode.c's fade pointer, mothead_sound_play's short casts,
      and camera_reset's record reset (for cam_mode_pointstage).
    - 0x0C04F5C0, the point-in-polygon test that coli_poly_contains_hit
      calls, as hand-written assembly.
  - **The unreached starts** have 27 rows in `ghidra/gap_functions.tsv`:
    0x0C033600, ten hand-written routines at 0x0C0F5640-0x0C0F5953,
    0x0C0FBE9A and others.
  - **ApplyTypes** now clears data that analysis left at a new row's
    start before it disassembles. Pointer data at 0x0C04F5C0 had stopped
    the function from being made.
  - **Totals**: 6166 of 6166 functions match and test.bin's 46 match.
    The open list is the same 17 as before.

## 2026-11-03: the camera, demo request and net status records typed

- **Three objects now have named structs** in `ghidra/types/vf4ft_types.h`.
  Each has a symbol row that names the object, so AssignAggregates leaves
  it alone. Every member keeps the type it had, and only the names change:
  - **camera** (`camera_state`, 0x74 bytes at 0x0C370CE0):
    - eye, target and goal as `vec3`;
    - flags, hold_frames, pitch / yaw / roll, sat_row, mode_return,
      mode_last;
    - the projection block (near_clip, far_clip, pers, aspect),
      pers_req;
    - the shake (`cam_shake`: x, y, step, state, kind and the pending
      request).
    
    The debug menu's four names (cam_rob_no, mode, pers, pers_req) stay.
    +0x0-+0x8 and +0x58-+0x64 keep offset names: nothing reads them by
    name.
  - **demo_rob_req** (`demo_rob_request[2]`): and_mask, or_mask, set,
    motion, flags. The four functions that cast it to local structs
    (demo_cmd_01 / 02, demo_rob_update, demo_start) now index it, and
    the borrowed local struct, with its ORIGINAL SHAPE note, is gone.
  - **net_status** (`net_status_rec[2]`): id, command, status, active,
    timer. It was reached both as a byte and as a 32-byte aggregate.
- **tools/rename_members.py** (spec in build/structs/camera.json) rewrites
  `object.member`, and `p->member` for locals declared with the type in
  the same function, then renames the type and the object. A use it
  cannot see keeps the old member name and fails to compile, so match.py
  reports it. 272 camera uses were rewritten.
- **Matching**: 6166 of 6166 functions match, test.bin 46 of 46.
- **Two Next items were already done**: MapSourceFiles has placed by
  callers since an earlier round ("callers", 16 functions), and the SDK
  libraries are all named. The Next list now says what is actually left:
  the game code's names and re-filing source/.
- **Likely original bugs**, all kept:
  - in the select screen, a dead `else if` and a layer freed twice;
  - the alAb firmware check's `&&`, which lets major-2 firmware with a
    high minor pass;
  - a trig benchmark loop indexed by the wrong counter;
  - the VRAM accounting doubled for two sprite sets;
  - a win-pose loop condition that is probably meant to be `&&`;
  - console_vprintf never sets the int it returns.

  The notes in build/game-names/ list the rest.

## 2026-11-03: the game code named (main.bin and test.bin)

- **Coverage**: every real function in both binaries now has a name.
  - main.bin: 6,180 of 6,183 (3,301 before).
  - test.bin: 1,973 of 1,980 (1,055 before).
  - The ten left are bogus entries at odd addresses. In main.bin these are
    0x0C0B046A, 0x0C0B058A and 0x0C0E08F4. In test.bin they are the
    misaligned and fall-through starts. They wait for the function-start
    pass (next, item 1).
  - The matches hold: main.bin 6,166 of 6,166, test.bin 46 of 46.
- **Method**: as for libbase.
  - 22 agents took main.bin sub-ranges of 0x0C050000-0x0C0ECE40 plus the
    assert-placed libraries. 6 agents took test.bin's own code.
  - The evidence is the game's strings, asserts, task names and callers.
  - main.bin's 3,728 rows went into new tables (libaction, libcard,
    libeffect, libenemy, libmission, libmotion, libnetwork, libpoly,
    librobdisp, libsound, libsprite) and into libdebug, naomi_library and
    newlib.
  - test.bin's 1,017 rows went into `ghidra/symbols_test/` (24 tables).
    Two of those rows override wrong twins that TransferFromMain had
    named from byte-identical main.bin routines: input_error_flags_get and
    board_float_get.
- **Files**: `ghidra/known_files.tsv` has 124 new rows, from libenemy at
  0x0C04FDE0 to the libc, newlib and libgcc objects at 0x0C157EA0.
  - The source map now places 6,176 functions (3,407 before).
  - Only 4 files are left in `src/_unmapped/`.
  - Some neighbours could not be told apart by their .data, .rodata or .bss
    order, so each such pair is one file: control.c holds move_script,
    msnm_disp.c holds msnm_map_disp, and effect_break.c holds stage_anim.
  - osage.c and osage_coli.c take their names from the assert paths.
  - The two hand-written assembly objects are named coli_sphere.c and
    poly_asm.c, like asm_helpers.c and matrix_asm.c. The export writes
    every object as C, and match.py, progress.py and the other tools read
    `.c` only. With `.s` names, the five assembly routines lost their
    addresses.
- **Data names and the matched source**:
  - **Bare rows**: a data row whose declaration is a bare identifier now
    names the address and leaves its type alone (ApplyTypes).
    AssignAggregates still lays out an object whose start carries such a
    name. This applies to 122 rows: aggregate starts, untyped rows, open
    `[]` dimensions, and arrays that would cover an object the code lays
    out. Each proposed declaration is kept in the row's evidence.
  - **Address spellings**: source/ spells the newly named objects by
    their address, and these were renamed too: 81 `agg_<a>_<a>` objects,
    4 `e_names_` tables and 2 `bitflags_` labels.
  - **Objects reached two ways**: source/ reaches 27 objects both as the
    laid-out object and through a separate DAT_ extern. Those keep the
    DAT_ spelling, and the exporter declares it as the member at that
    address.
  - **The source's types**: 579 rows take the type source/ declares, and
    the proposed type moves into the evidence.
  - **Analysis types**: 17 externs in source/ now use the type the
    analysis gives, `undefined *` from AssignPointers and char or short
    from AssignSigns. Examples are hit_wait_timer, mot_table2 and
    adv_fx_frame. While each was a DAT_ extern of its own file, the
    header and the source never met in one compile.
  - **Left unnamed**: three objects that different files declare with
    different types (audience_volume, break_floor_request,
    stage_sound_record).
- **A fix in the game text**: net_power_on_send's assert spells
  `"NAS_OK == net_info.auth_stat"`, so the original source called the
  state block net_info. The DAT_ restore must not touch string literals.
- The private-name and echo checks are clean.

## 2026-11-03: the 86 new function starts made and matched

- **Starts**: 86 functions Ghidra never made now have rows and match.
  - 49 came from the naming merge (build/names2/new-starts.tsv) and 37
    from the agents' notes. The notes ones include the libenemy tail
    0x0C054B60-0x0C054DBF, vp_version_get, the ROB_INFO, ROB_COLI_ATK
    and EFFECT task starts, eleven window key handlers (win_unfocus ...
    win_width_shrink), color_tune_yuv and seven empty stubs at
    0x0C0B4160.
  - Each begins with a compiler prologue after padding, an rts or a pool
    word (build/names2/starts.py prints the bytes).
  - 29 are reached by nothing (no word holding the address, no bra or
    bsr to it) and have rows in `ghidra/gap_functions.tsv`.
- **How they matched**:
  - 54 from the decompiler's text (auto_match.py, then adopt.py).
  - 30 by four agents (round CC, build/agents/ROUND-CC.md, batches
    800-803). Most took a matched sibling's shape: its local struct
    typedef, its char local, its loop. Some used the field's real width
    (a ushort move id, a sign-extended byte compared unsigned). Two are
    real arrays Ghidra split into aggregates: objbanks rows and
    dural_stage_scroll[2]. No tricks were used.
  - 2 needed globals nothing named: enemy_origin (0x0C420164, a vec3)
    and card_make_flag (0x0C2A01FA, the byte card_make_close clears).
    The agents had held both, because they matched only through a
    neighbouring symbol.
- **Prototypes from the matches**: enemy_ai_attack_kind_is_1 / 2 and
  enemy_ai_attack_frames_spare take an unsigned short move id, and
  frames_spare returns one. rob_item101_head_set takes the rob as a
  byte pointer, as rob_items_update does.
- **Bogus entries**:
  - main.bin has three: the fall-through bodies of model_verts_project
    and model_verts_project_scaled, and 0x0C0E08F4. The last is padding
    plus tex_pixels_yuv_adjust's shared exit, which that function's
    matched C (0x664 bytes) already covers; its .s is gone.
  - test.bin has seven, in the new `ghidra/bogus_entries_test.tsv`,
    which ApplyTypes now reads for test.bin. The true starts
    argb_strips_send and argb_offset_strips_send are 0x0C03F580 and
    0x0C040010.
  - ApplyTypes now follows the host routine's flow again after removing
    an entry, so its body takes back the code that falls through or
    branches there. This also mended the old main.bin entries at
    0x0C107E02 and 0x0C108A54.
- **Tool fixes**:
  - delay_slots.py's GCC-frame test looked 24 halfwords ahead and found
    the next function's prologue past a short routine. It grouped
    zanzou_vertices_write (assembly) with color_tune_yuv as "C, slots
    filled", giving color_tune_yuv the wrong flags.
  - delay_slots.py also let compiled C with a single branch join an
    assembly run. Two more ranges are now C, as their GCC frames say:
    0x0C0B0660 and 0x0C1358E0-0x0C135960. Matched files keep their own
    flags.
  - adopt.py counted match* (a relocation it could not check) as a broken
    file after a new declaration, so it gave a false warning on
    0c072320.c.
- **Totals**: 6,249 of 6,249 match (6,166 before: the new starts are 86
  more, and the 3 bogus entries are not functions), test.bin 46 of 46.

## 2026-11-03: header leads: char sides, objbank, fog_rec

- **round_winner and champion_side** (0x0C2925D2 / D4) are `char`. The
  code reads them signed 113 times (`(char)X`, `*(char *)&X`) and
  unsigned 44 times, and signs.tsv votes 32 to 7 for signed.
  `build/structs/to_char.py` retyped them without changing what any use
  reads: the signed casts are gone, and the unsigned reads are now
  `(uchar)X`.
- **dural_stage_scroll** is `float[2]`; `(&dural_stage_scroll)[i]` is
  now `dural_stage_scroll[i]`.
- **objbank**, the 0x2C-byte object bank record (state, nodes, names,
  count, set, tex_base, mem, tex_descs, tex_names, name, size), is now
  a type in `ghidra/types/vf4ft_types.h`.
  - objbanks is `objbank[21]`, objbanks_saved is `objbank[21]` and
    objbank_empty is one `objbank`. Eight files each read it through a
    local view of their own (`ob`, `ent`, `bank_ent`, `bank_t`,
    `int[11]`); those views are gone.
  - The row had kept objbanks untyped, because agg_0c3b6538 looked like
    an object of its own. It is bank 3: dust_models_find reads
    `objbanks[3].names` (+0x8C) and `.set` (+0x94) through one base at
    +0x7C. That is GCC's split of a far offset, base
    `((off + 4) & ~60) - 4`. The eight agg_0c3b65xx-67xx pieces are now
    part of the array.
  - objbank_free_step keeps its word-by-word view: it copies words 1-10
    of objbank_empty and then zeroes word 0, which is not a struct copy.
- **fog_rec** `{ushort argb[4]; float near_dist, far_dist, strength}`
  covers stage_fog.c's five .bss records: fog_offset 0x0C374288,
  fog_target 0x0C37429C, fog_fade 0x0C3742B4, fog_cur 0x0C3742C8 and
  fog_base 0x0C3742DC. It also covers the zero record fog_record_zero
  at 0x0C168068.
  - The field meanings come from fog_frame_update. It packs argb (each
    clamped to 0-255) for fog_col_ram_write_bytes, and calls
    fog_table_build(near, far, strength) and fog_density_pack_float(far).
  - The colour is unsigned, because fog_frame_update's mix converts it
    to float. stage.c's `short` view only copied the shorts.
- **ApplyTypes: anonymous types**.
  - The bug: CParser names an anonymous struct or union from a running
    count (`_union_41`), and that count moves whenever a header gains a
    `struct`. The objbank type moved it by one, so ct_data_s.dl was
    given the name an earlier run had given inflate_huft_s.word's
    union. The parser and the copy into the program then used that old
    union. zlib's trees.c stopped compiling, and four deflate functions
    differed.
  - Until now, collisions had missed only by luck. The program still
    held anonymous types numbered by a much older parse (29/30 for
    ct_data), matched to the new ones by equivalence.
  - The fix: ApplyTypes names each anonymous type after its place
    (`ct_data_s_dl`, `inflate_huft_s_word_what`), files it with the
    header types, and removes the counted ones left from earlier runs.
    A second run in a row changes nothing.
- **Left for later**: sound_test_state's +8 and the debug window record
  (see Next). `build/names2/stale_dat.py` also finds 51 source externs
  still spelled DAT_ for addresses a row has named. Most are typed rows
  (`char play_style`) whose file pins another type (`undefined1`), so
  they need a sign-aware rewrite like the one above. The rest are bare
  rows over aggregates.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the debug window record

- **win_rec** is the 0x58-byte debug window. It is now a type in
  `ghidra/types/vf4ft_types.h`, after fog_rec, together with **win_ref**.
  - win_ref is the 12-byte reference that win_ref_address and
    win_ref_resolve follow: a word, an arg and flags. With flags bit 0
    the word is a function called with arg. Unless bit 1 is set, the
    result is read as an address.
  - The fields:
    - title, items, count (ushort), name_cols, value_ref and tmpl.
      win_sys_init copies items and count from tmpl.
    - Five callbacks, each with its arg: open, focus, update, draw
      and close.
    - x, y, width and height, in text cells, at +0x44.
    - scroll and cursor (ushort) at +0x48 and +0x4A.
    - The flag bits open and focus, then the byte z (draw order) at
      +0x4D.
    - value_base at +0x50, and cycled (win_focus_cycle's mark) at
      +0x54.
- **The window array**: win_list (0x0C211088) holds 110 pointers and a
  NULL. They point one after another, 0x58 apart, into a single array
  from 0x0C322DC8 (MAIN_MENU, NETWORK, DEBUG_MALLOC ... CHALLENGE).
  win_sys_draw walks that array directly. It now has a row,
  `win_rec win_windows[110]`; the name is ours.
  - win_list is now `win_rec *[111]`, and win_focused and win_top are
    `win_rec *`.
- **The rewrite**: all 42 routines in win_sys.c and 8 in 0c0eaa00.c
  now take or return `win_rec *`. The 8 are the text, console, frame
  and background draws.
  - Field reads replace the offset casts. Bitfields replace the mask
    arithmetic on +0x4C and +0x54.
  - The handlers read naturally, for example
    `if (w->height < 0x3c) w->height = w->height + 1;`.
  - win_frame_draw and win_background_draw already had a local `w`,
    so their window parameter is `win`.
- **Two signedness facts the match fixed**:
  - `focus` is a signed `int : 1`. Its tests compile to `and #2`, but
    an unsigned field compiles to shlr + `and #1`.
  - `open` stays unsigned, because win_open_count adds the field and a
    signed bit would add -1.
- **Callbacks need full prototypes**. Ghidra exports a K&R `void (*)()`
  as `void (*)(void)`, so the first full run failed to compile win_sys.c
  ("too many arguments"). The five callbacks are declared
  `void (*)(struct win_rec *, int)`.
- **Left for later**: the routines the windows point their callbacks
  at, in the debug pages, still take the window as an int. So does the
  menu item record that `items` points at (+0x14 to +0x20 references,
  +0x44 limits).
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: sound_test_state

- **sound_test_rec** is now a type in `ghidra/types/vf4ft_types.h`,
  after win_rec. It is the 0x24-byte record that the four sound debug
  windows (SOUND_TEST, CV_TEST, ROBSOUND_CHG, BGM_CHG) edit, and the
  row types sound_test_state (0x0C37D734) with it. net_select_scratch
  starts right after it, at 0x0C37D758.
- **The field names come from the game's menus**. Each item record of
  those windows holds its label at +4 and its offset into the window's
  value record at +0x20; `build/structs/snd_items.py work/main.bin`
  dumps them. The fields:
  - code, name and num: the request word SOUND_TEST posts, and the
    sound_id_table entry, read as ushort.
  - cv_damage_off: rob_voice_request drops voice group 4 while it is
    set.
  - cv_type and cv_num: CV_TEST's group, signed, and its entry,
    unsigned.
  - title_se: attract_adv_se_play stays silent while it is set.
  - post_off at +0x0E: the one field with no item. Only
    sound_test_open clears it.
  - rob0_se_num, rob0_vo_num and bgm_num, the chosen list entries, and
    rob0_se_name, rob0_vo_name and bgm_name. The draw callbacks copy
    each entry's third word into the name fields. The lists'
    initialisers at 0x0C298F10, 0x0C299000 and 0x0C2990B4 show that
    word is the entry's name.
- **The rewrite**: ten routines in sound.c and 0c072320.c read the
  fields by name. The char and byte views of the record are gone, and
  so are both `DAT_0c37d748` externs (that address is +0x14, bgm_num).
  `build/structs/sound_test_source.py` does the rewrite.
  - sound_test_focus_update, cv_test_update and rob_voice_request read
    the record through a `sound_test_rec *st` local. The game loads its
    address into a callee-saved register on entry and keeps it across
    the call. Direct global reads load the address again after the
    call, and those three then differ. The other seven match reading
    the global directly.
- **Checks**: check_externs reports 95 differences, one fewer than
  before (the DAT_0c37d748 pin is gone). The agg_0c37d734 type stays in
  the export unused, as objbank's aggregates did.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the stale DAT_ externs

- **What they were**: 51 externs in 13 source files still pinned a
  `DAT_` name, with the type the file had when it matched, for an
  address that a row has named since then. All 51 were still in use.
- **The rename**: `build/names2/stale_rename.py` drops each extern
  line, because the header declares the name, and renames the uses.
  - Most compile and match unchanged: the scalars and pointers
    (vblank_count, play_style, demo_flags, elan_cmd_write_ptr ...)
    and every aggregate whose address is only taken (`&robs`,
    `&effect_slots`).
- **Uses that needed a shape**: these read the pin's element type
  against a row the header declares as an aggregate. Each now has the
  shape the file's matched sites use for the same name:
  - `(&X.field_0x0)[i]` for the per-side pairs. These are
    chara_se_tables, chara_voice_tables, level_change_flags,
    item_plate_hidden, item_award_mes_on, mission_of_side and
    mission_tally.
  - `(&X)[i].field_0x0` where the stride is the whole record: item_lottery
    (was `[i * 5]` over words) and msn_text_colors (was `[i * 3]`).
  - `.field_0x0` for the value reads of sound_state, net_info and
    fish_solo3.
  - rob_get: `(undefined *)&robs + i * 0x86c`.
- **Casts dropped**: the char rows lost the casts and byte views their
  pins needed (`build/names2/stale_casts.py`). Examples are
  `(char)rep_repeat_count`, the
  `*(undefined1 *)((char *)&rep_repeat_count + 0) =` store, and the
  `(char)` on side_entry_flags and sel_cursor elements.
- **The export**: 30 `DAT_` shim declarations are gone from
  vf4ft_data.h, and 3 from test.bin's. check_externs reports 74
  differences, down from 95. stale_dat.py finds none.
- **Leads it found**:
  - The seven per-side pairs above are arrays of two, not records. As
    `char[2]`, `int[2]` or `void *[2]` rows, every `.field_0x0` /
    `.field_0x1` and `(&X.field_0x0)[i]` use across the tree would
    read `X[0]` / `X[i]`.
  - 0x0C243078 is probably one replay record (rep_repeat_mode,
    rep_repeat_count, rep_fade_frames at +4). The matched code reads
    rep_fade_frames as `*(int *)((char *)&rep_repeat_count + 3)`, from
    rep_repeat_count's address. Naming it on its own would load a
    second address.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the per-side pairs as arrays

- **The rows**: seven per-side globals are now the two-element arrays
  their rows had proposed. The aggregate pass had laid each out as a
  two-field record. The arrays:
  - `int mission_of_side[2]` (the mission number, -1 for none) and
    `int mission_tally[2]`.
  - `uchar level_change_flags[2]`. The row proposed char, but the code
    ORs and returns it unsigned.
  - `char item_award_mes_on[2]` and `char item_plate_hidden[2]`.
  - `short *chara_se_tables[2]` (effect ids) and
    `char *chara_voice_tables[2]` (12 group counts, then the lists).
- **The rewrite**: `build/names2/pair_source.py` changed 116 lines,
  in 0c072320.c, card.c and 0c0877e0.c. The views became `X[i]`, and
  `X.field_0x0` / `.field_0x1` / `.field_0x4` became `X[0]` / `X[1]`.
  The views it replaced:
  - `((int *)&X)[i]`, `(&X.field_0x0)[i]` and `(char *)&X`.
  - The `{ int ba = (int)&level_change_flags; *(uchar *)(ba + i) |= 8; }`
    blocks (22 of them, and level_result_0_9's `b3` base), now
    `level_change_flags[i] |= 8;`. The match
    did not need the int base after all.
  - `((struct { int m[2]; } *)&mission_of_side)->m[i]`.
  - The `(undefined *)0xffffffff` compares on mission_of_side, now
    `-1`.
- **Prototypes**: mission_active_get returns the mission number as
  `int`; its callers already read it as one. chara_se_table_set and
  chara_voice_table_set take `short *` / `char *`.
  attack_result_sounds_play keeps its `undefined4 *tb` view of
  chara_se_tables, since its locals read the entries as words.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46. check_externs
  is unchanged at 74.

## 2026-11-03: the replay globals

- **No replay record**. The stale DAT_ externs entry guessed that
  0x0C243078 was one record, because the matched code reads
  rep_fade_frames as `*(int *)((char *)&rep_repeat_count + 3)` and
  `*(int *)(&rep_repeat_mode + 4)`. That guess was wrong.
  - GCC folds `&sym + 3` into a single pool word, `sym+3`, so those
    reads load 0x0C24307C exactly as `rep_fade_frames` would.
  - The pool shows the same: the ending replay states load 0x0C24307C
    directly. Every global in 0x0C243078-0x0C2430B7 has pool words of
    its own, and no routine reaches one from another's address
    (`build/structs/pool_refs.py work/main.bin lo hi` lists them).
  - So these are separate globals. The six reads now name
    rep_fade_frames.
- **Types the rows described**:
  - rep_repeat_mode, ending_rep_tasks_started and ending_rep_next are
    now `char`. Their evidence had said "laid out as char", but the
    rows and the file pins kept undefined1.
  - The `*(char *)&ending_rep_next` views are gone, and
    `ending_rep_next = 0xff` is now `-1`.
  - ending_rep_times is `int ending_rep_times[10][3]`, the countdowns
    at which states 0, 2 and 3 move on. ending_rep_setup reads
    `ending_rep_times[i][0]`. ending_rep_start and ending_rep_play read
    through an `int *` to the slot's row instead of their local
    three-int structs.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the debug menu item record

- **win_item** is the 0x48-byte menu item that a window's `items`
  point at (win_rec.items is now `win_item **`). The fields:
  - kind: 1 opens the window held in act's word, 2 resolves act (the
    "[robsound_set]" actions), 3 shows a value and 4 also edits it.
  - label at +4, and the win_ref act at +8.
  - flags at +0x14: the low nibble is the size (byte, short, word or
    float), 0x100 means from value_base and 0x2000 through a pointer.
  - bit at +0x16. mode at +0x17: the low nibble is the format (0-6
    float, 7-11 int, 12/13 string, 14 on/off, 15 enum), 0x30 the value
    count and 0x80 a string by its word.
  - names_flags at +0x18 (4 a bitmask list, 2 the names through a
    pointer), names at +0x1C, three win_ref values at +0x20 and
    limits at +0x44.
- **win_limits** (0x10) holds a clamp flag, then step, lo and hi, as
  ints or floats according to the item's format.
- **win_value** is the 8-byte {type, value} record that
  win_value_load and win_value_store move: type 0 an int, 1 a float, 2
  the word at the address and 3 the address itself. It replaces three
  local copies: win_sys.c's win_value and dbg_value, and 0c0eaa00.c's
  win_value with its `.v` member.
- **The rewrite** (`build/structs/win_item_source.py`): the item
  routines in win_sys.c take `win_item *` and `win_value *` and read
  fields, as does 0c0eaa00.c's win_bitmask_item_draw. These are
  win_contents_draw, win_item_select, win_item_values_access,
  win_value_load / _store, win_item_bit_get and win_value_step.
  win_item_row_draw is unmatched, so its row prototype now takes
  `win_item *`.
  - win_value_step keeps one ORIGINAL SHAPE, tagged in the source.
    Each case holds the limits pointer in the int `t`, tests it there,
    then copies it to a typed pointer. With a single `win_limits *`
    the game's register copy is gone and the float case differs.
    Three other shapes were tried, and all differ.
- **The debug pages' callbacks**: `build/structs/win_callbacks.py`
  lists the 51 functions the 110 windows name as callbacks. All but
  three are `(void)`: they ignore the window and arg passed to them.
  console_draw already took the window. texture_test_draw and
  debug_malloc_update took it as an int; they now take `win_rec *` and
  read x, y, width, height and value_base.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the file map corrected from test.bin's link order

test.bin links the same libraries as main.bin, but in another order and
with some objects left out. A linker can reorder or drop whole objects
(archive members), but it cannot move code inside one object. So
wherever test.bin separates two runs of code that main.bin places
together, they come from separate files.

- **test.bin has its own file table**: `ghidra/known_files_test.tsv`,
  192 rows with no overlaps. `build/names2/t_files_merge.py` builds it
  from the per-range candidates in `build/names2/t-file-candidates.tsv`.
  Two candidates for the same file across a seam are merged: on the side
  one row marks as outside its range, it gives way to the other row.
  MapSourceFiles used to give test.bin no table; now it reads this one.
  test.bin's source map went from 25 functions to 1,970 of 1,975, in
  191 files. The 5 left over sit in small gaps in newlib.
- **main.bin's rows split** (`build/names2/main_splits.py`):
  - naomi2_library.c is seven objects. test.bin links them as elan_cmd_ram,
    model_draw, naomi2_library, elan_lights, model_compile, model_tex,
    then object_put, and it leaves out the light commands. The light
    commands are elan_light_cmds.c (0x0C0FBEE0); the others are
    model_compile.c (0x0C0FC6E0), model_draw.c (0x0C0FE7E0), model_tex.c
    (0x0C0FFA40), naomi2_library.c (0x0C100A00, which keeps the version
    string), elan_cmd_ram.c (0x0C1019E0) and elan_lights.c (0x0C101CE0).
    model_compile and elan_lights are the same size in both binaries.
    All the new file names are ours. One thing is still unexplained:
    main.bin's .rodata opens with the light code's bit mask table
    (0x0C211584) before model_cmd_iter_next's table (0x0C211604), which
    is not the code's order.
  - text_util.c is token.c (0x0C11F3A0) and base64.c (0x0C11F540).
    test.bin links base64 alone.
  - asm_helpers.c is two objects. The uncalled store-queue copies and
    the prefetch are store_queue_asm.c (0x0C12C780), which test.bin
    leaves out; the arctangent, the fsca sine and cosine and the
    palette write stay asm_helpers.c (0x0C12C810).
  - matrix_asm.c starts at 0x0C12CF00, since test.bin keeps the uncalled
    XMTRX add with the matrix code. light_vertex_asm.c ends just before
    it.
  - source/asm has one .s per routine, so none of these changes affects
    matching. source/ itself is not re-filed yet (Next item 2).
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows given their proposed types (449 of 574)

These are the rows that took source/'s placeholder declaration. Each
kept the analysis's proposal in its evidence, e.g. `char
mirror_angle_flips` proposed as `char mirror_angle_flips[9][4]`. 574 rows
carry such a proposal. Most are arrays and plain retypes rather than
structs.

- **The tool** (`build/structs/retype.py`) handles one row at a time:
  - It gives the row, the header and every extern line in source/ the
    proposed declaration.
  - It rewrites the uses that spelled the old type's address arithmetic:
    `*(float *)&X` becomes `X`, `(&X)[i]` becomes `X[i]`, a byte step
    `&X + i * 0x14` becomes `X[i]`, and a bare scalar read of a new
    array becomes `X[0]`.
  - It keeps the row only when every file using it still matches and
    compiles with no new warning. Otherwise the files go back as they
    were.
  - `build/structs/srctyped_rows.py` classifies the rows and
    `srctyped_uses.py` lists how source/ spells them.
- **Kept: 449.**
  - 146 of the 159 same-size retypes (undefined4 to int or float,
    undefined1 to char, and so on).
  - 303 arrays and pointer tables, such as vp_seq_dash `short [4][10]`,
    vp_mode_funcs, rob_side_chara and poly_queue_time_limits.
  - `retype_rows2d.py` then turned the row steps through 2-D arrays into
    indexing (`&vp_chara_tables[0][0] + i * 9` became
    `vp_chara_tables[i]`). `retype_spell.py` spells the extern lines
    as the header does (uchar, ushort, uint). That also brought
    check_externs.py down from 74 to 44.
- **Put back: 125.** Their proposals stay in the evidence.
  - 72 differ. For the 12 same-size ones, the code reads them unsigned
    (e.g. `(uint)eye_index * 2`), so the proposed signed type is wrong.
    The rest have use spellings the tool does not rewrite.
  - 49 add a warning, mostly a pointer-type mismatch at a call.
  - Two passed the per-row check but overlapped the next row, which
    the export cannot lay out. `rob_item_models[192]` runs 0x20 bytes
    into rob_item_envelopes, and `sprite_sets[81][3]` runs 0x168 bytes
    into ltop_file. `build/structs/row_overlaps.py` now finds these.
  - Two are evidence against an array. source/ reaches the words after
    `leaf_kick_tuning` and `hbr_clock_hands` as separate DAT_ names. The
    element spellings (`leaf_kick_tuning[1]`) give a different pool and
    do not match, so these are separate globals.
- **DAT_ names inside the new arrays** (`build/structs/dat_in_arrays.py`):
  poly_queue_time_limits[1..4] and card_message_fade[1] are now spelled
  as elements, and they match.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: source/ re-filed by the source map (4,596 functions moved)

`source/_unmapped/` held the matched functions in 87 group files cut by
build flags and match order. `ghidra/source_map.tsv` places 5,058 of
them in their original files. `build/structs/refile.py` moves them, one
target file at a time:

- **The target file** is rebuilt from its functions in address order:
  the ones already there and the moved ones, since the linker keeps a
  file's code in source order. It also gets:
  - the KRPROTO_ / NOPROTO_ defines of the functions it names;
  - the pinned externs it uses;
  - the helpers it names (static inline functions, macros, local
    types), taken from the group each function came from. A helper
    already present under the same name is not copied twice.
  An existing file keeps its own header comment and match-flags.
- **Kept only if it matches.** A moved function that differs in its new
  file goes back to its group, and the rest move. The group files are
  checked by the full match afterwards.
- **K&R declarations for a function the file defines**: some callers
  matched only against a K&R declaration (`KRPROTO_sound_play_id`),
  while the definition is ANSI, and one file cannot hold both. Such a
  file gets `NOPROTO_` instead. The callers before the definition then
  see no declaration (the implicit one, with the same argument
  promotion), which is how a C file without forward declarations
  compiles. Most callers match that way. The ones that do not stay in
  `_unmapped` (sound_rep_log_play, card_panel_type1_draw,
  action_lever_update and others).
- **Result**:
  - 264 target files written (197 new), holding 4,596 functions.
  - 55 group files compiled to no functions and were removed.
  - 32 remain, holding 523 functions. 505 of those have a known file:
    - 18 were left out of their files, one by one;
    - card.c, card_mode.c, effect_etc.c, effect_rain.c and sprite.c
      did not move at all.
    Both lists are in `build/structs/refile-left.txt`.
  - vram_alloc.c's one `-mspace` function stays in its group, since
    the file is built without that flag.
- **Fixed on the way**: the top-level splitter
  (`build/structs/unmapped_toplevel.py`) ended an item at the end of
  any line whose braces balanced. A line with typedefs followed by a
  static inline function lost the function's body. An item now ends
  only on a line ending in `;` or `}`.
- **Totals**: 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: frame data checked in Flycast

- **Reading a Flycast save state**: `tools/flycast_state.py`.
  - The file is "FLYSAVE1", then a PNG thumbnail, then the machine
    state in RetroArch's RZIP container (zlib chunks).
  - Main RAM is found in the 92 MB state by the game's own code at
    0x20000 into it. In the maintainer's saves, 850,250 of 851,968 code words
    equal main.bin; the rest is code the game patches at run time.
- **Reading the fighters**: `tools/flycast_robs.py` prints both rob
  records (0x0C37BA30, 0x86C each) with:
  - the character, named from its own sound banks through the 0x4C-byte
    character record (+0x44);
  - the move's frame window and the current frame (+0x0A);
  - the stun;
  - the frames left, computed as `rob_frames_left` does.
  The character numbers: 0 akira, 1 sarah, 2 lau, 3 shun, 4 jeffry,
  5 pai, 6 jacky, 7 kage, 8 lion, 9 wolf, 10 aoi, 11 boze, 12 black,
  13 dural, 14 jdk, 15 muetai, 16-18 Akira variants. These are the
  files' internal names. Number 14 is Goh: a save of a Goh vs Jeffry
  fight read back as 14 and 4.
- **The frame difference**: `robs_frame_advantage_update` writes rob
  +0x16A every frame. It holds side 0's frames left minus side 1's,
  but only while either fighter is in a hit or guard state; otherwise
  it is 0. So a save has to be paused within the exchange.
- **Results**:
  - Akira's `AKI_TA_PU_JAB2HATI` blocked by character 11: the game's own
    value was +10, i.e. Akira 10 frames behind. framedata_AKI.tsv says
    -10 on guard.
  - `AKI_PU_02_VFX` on hit: Akira free one frame before the pause, the
    opponent at 4 stun frames left, which is +5. The table says +5.
    This one assumes the stun counts down one per frame.
  - The reimplemented `rob_frames_left` reproduces the game's stored
    value (14 - 4 = 10).
  - A third save, meant to be the shoulder ram, caught a different move
    with the opponent knocked down, which is not a state the game
    computes a difference for.

## 2026-11-03: net_info and itmtst_state

- **net_info_t** (0x44 bytes, `ghidra/types/vf4ft_types.h`, in a new
  network.c section) types net_info at 0x0C37D804. network.c's assert
  gives the object's name and one member's name, auth_stat. The other
  names come from what the code does with each member:
  - on (net_init, net_is_on);
  - error_level and error_frames: net_reply_dispatch counts the frames
    with an HTTP error and grades the count 0..3;
  - busy (more than 4 requests queued, or grade 3);
  - hours_open and service_state (net_service_hours_check,
    net_service_state_update, net_service_active), end_frames;
  - nam_state, the state nam_step runs on;
  - timeout and timeout_left, the request time-out;
  - the three retry waits nam_step counts down (http_wait, auth_wait,
    game_init_wait), each with the flag that asks its enter routine for
    no wait;
  - game_config_received, set at the end of net_power_on_reply_parse.
- **service_state is unsigned**: net_service_active compares it with
  `cmp/hi`. Typed int, it compiled to `cmp/gt` and the routine stopped
  matching, so the match fixes the type.
- **Spellings gone**: the `DAT_0c37d844` member and extern (+0x40,
  game_init_wait), and the three `((struct { char pad[0x40]; int f; } *)
  &net_info)->f` casts in nam_retry_wait_game_init_enter. They now read
  `net_info.game_init_wait` and still match.
- **itmtst_rec** (0x20 bytes) types itmtst_state at 0x0C4C52E4, the item
  test's state: rob0 and rob1, the chara and costume read from rob 0,
  that character's items and item_count from chara_item_table,
  saved_pause_flags and the step. itmtst_start clears +0x1C with a word
  store, but every other access reads the step as a byte, so it stays a
  char and the word store keeps its cast.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: sprite_put_state and sound_state

- **sprite_put_rec** (0x28 bytes) types sprite_put_state at 0x0C3B6438.
  It holds what sprite_put adds to every queued sprite, and each member
  has its own sprite_*_set / sprite_*_reset pair:
  - offset (a vec3; z on top of sprite_base_z), scale_x, scale_y, alpha;
  - col_override and alpha_override, which apply unless they hold the
    sentinels 0xFFFFFF / 0xFF000000;
  - flags_override (when non-zero) and pri_base.
  sprite_colors_reset stored the sentinels as `&DAT_00ffffff` and
  `&DAT_ff000000`, the constants read as addresses. It now stores
  0xffffff and 0xff000000 and still matches. The ~40 sprite_put calls
  that pass the same two values as arguments keep that spelling for now.
- **sound_rec** (0x60 bytes) types sound_state at 0x0C37D65C. The members
  named:
  - flags, the request and load bits sound_state_get hands out;
  - boot_result (aica_driver_boot), cmd_result, play_result,
    last_request, ring_result, fx_result;
  - req_peak, req_count and req_last, the per-frame request count
    (sound_request_post, sound_frame_update);
  - dtpk_free, libsnd_build;
  - drv_e8, drv_ec and drv_ec_peak, the driver words sound_frame_update
    reads every frame.
  The members no routine names (+0x04, +0x0C, +0x10, +0x1C-+0x28, +0x3C,
  +0x48, +0x49) keep their offsets.
- **The byte-offset views are gone**:
  - sound_system_init's local `struct S { int f0 ... }` cast;
  - the `undefined *p` views in sound_frame_update and
    sound_test_update;
  - the `(char *)p + 88` store in sound_request_post.
  Each now reads the record through a `sound_rec *` local, the shape the
  routines already had. All match.
- **Left open**: the attract sound setting at +0x74 (`DAT_0c37d6d0`).
  All four of its readers load it through its own pool word. A member
  past mov.l's displacement reach would get one too, so this does not
  settle whether it is part of the record. It stays a separate global.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: card_service_state and snow_wind

- **card_service_rec** (8 bytes) types card_service_state at 0x0C431390.
  It has two members:
  - open, the reception flag net_service_hours_check sets;
  - mode, which card_service_state_update moves: 1 open, then 3
    (play_style 1) or 2 (play_style 2) once reception closes, and -1
    closed.
- **snow_wind_rec** (0x34 bytes) types snow_wind at 0x0C38BFB8: vec3
  base, dir and wind, then two phase angles with their steps and two
  amplitudes. snow_wind_update sets
  wind = base + dir * amp * sin(phase + amp2 * sin(phase2)), and the
  flakes drift by wind.x and wind.z.
  - snow_wind_update now reads the record through a `snow_wind_rec *`
    instead of the local `struct S { float f[9]; short a ... }` cast.
  - snow_wind_init's two pad-struct casts (amp, amp2) are gone.
  Both still match.
- **What is left among the named objects with aggregate types** (69
  after net_info, itmtst_state, sprite_put_state and sound_state; 67
  now):
  - msnm_disp_state: 224 member uses; 0x164 bytes, whose init
    (msnm_disp_init) sets 31 + 49 handle words to -1.
  - rain_params: its init and rain_streak_setup reach it through float
    views (`g[0x16] = -g[0x19]`) and 12-byte block copies, so naming it
    means reworking those shapes.
  - Arrays the analysis split into members, e.g. prize_item_chance[2]
    and lottery_reels[3]. These belong to the source-typed rows item.
  - Effect records named only by their init routine (fish_seeker,
    sky_flash, sand_colors ...), the rest reached through pointers.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: msnm_disp_state and rain_params

- **msnm_disp_rec** (0x164 bytes) types msnm_disp_state at 0x0C450F24,
  the challenge map display.
  - Layout: `int objs[31]; int stills[49];`, then nine scalars:
    plate_wait, sel, plate, flags, selroute_frames, flash_frames,
    coin_frames, box_frames, hidden.
  - Why two arrays: msnm_disp_init sets two runs of handles to -1, in
    loops of 31 and 49.
  - Why objs / stills: every handle below +0x7C is started and stopped
    with aet_obj_*, every one from +0x7C with aet_still_*. None of the
    named accesses mixes them.
  - hidden (+0x160) is chlg_map_visible_set's hide flag. It was also
    spelled `DAT_0c451084` (that extern is gone): no new part is started
    while it is set.
  - The local views are rewritten as array indexes:
    - `struct { int a[31]; int b[49]; }`;
    - `hdl` (objs[n + 15]);
    - the `pad[0x80]` a / b / c triple (stills[i + 1 / 9 / 17]);
    - `struct T` (stills[i]);
    - the `pad[0x160]` byte;
    - the `char *` / `undefined *` views, including the one based at
      +0xBC.
    A scratch script did it one function at a time. Every routine still
    matches.
  - Kept as flat pointer views: chlg_round_lamp_show (`row + 0x138`) and
    the rank loop's `a[0x41]`, which index from objs into stills.
- **rain_params_rec** (at least 0xBC bytes; snow_bodies follows at
  +0xD8) types rain_params at 0x0C389CDC.
  - The pairs are per streak set. rain_streaks_scatter puts streaks 0-20
    in box 0 and 21-35 in box 1. rain_streak_setup gives a streak set 1's
    velocity, length and shape when its index is above 20.
  - The members: box[2][3], count[2], len[2], vel[2], shape[2][3] (the
    three corner offsets, from the table at 0x0C1A0FB4), exec_usec.
    Words only rain_params_init writes (+0x94-+0xA8, +0xB8) keep their
    offsets.
  - The float views (`g[0x16] = -g[0x19]`) and the 12-byte block copies
    are gone.
  - One loop keeps its base-plus-offset view, with an ORIGINAL SHAPE
    tag: rain_streak_setup's copy of set 1's corners. Spelled
    `rain_params.shape[1][i]`, it needs a `rain_params+0x70` pool word
    the game does not have, and the routine differs.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: fish_seeker and sky_flash

- **fish_seeker_rec** (0x5C bytes) types fish_seeker at 0x0C4586F8, the
  fish that swims to the edge of the camera's view and towards a
  fighter.
  - The scalar members: flags, state, rot_y / rot_x / rot_z (the draw
    angles), rate, spd, t and prm.
  - Five vec3s:
    - pos;
    - goal and edge: the view-edge point, clamped and raw
      (fish_seeker_target_update);
    - heading, which fish_seeker_steer blends towards want.
  - The views are gone:
    - fish_seeker_update's local `cam` type, whose layout this is;
    - fish_seeker_init's `float *f` (f[9] .. f[22]);
    - the byte views in fish_seeker_rob_landed and fish_seeker_draw.
    All still match. The helpers that take the record as an `int`
    (steer, move, target_update, view_check) still read it by offset.
- **sky_flash_rec** (0x1C bytes) types sky_flash at 0x0C458C6C, the
  flash of the unreferenced sky_flash_* routines.
  - The members: flags, pieces, frame, frames, delay, model, list,
    time_prev, time, variant.
  - With frames and delay typed short and list typed `int *`, these
    casts are gone and the routines still match:
    - the `*(short *)&p->field_0x4` and `*(short *)&p->field_0x6` casts;
    - the `*(int *)&p->field_0xc += 4` walk, now `p->list++`.
  - sky_flash_draw keeps its `(char)` reads of flags and pieces, and its
    one whole-word test of flags, pieces and frame.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: sand_colors and itmtst_stats

- **sand_colors_rec** (0x18 bytes) types sand_colors at 0x0C396CC0: one
  word per channel, grain_r/g/b (sand_grain_models_color_apply) then
  cloud_r/g/b (sand_model_color_set). The row was proposed as
  `int sand_colors[6]`.
- **itmtst_stats_rec** (0x1A4 bytes) types itmtst_stats at 0x0C4C5304,
  the item test's statistics.
  - item, frame, mode and sample, then the twelve sample rows in three
    columns: cpu, render and ta (+0x10, +0x40, +0x70).
  - best[8] (+0xA0), the best-of tables, each `{float val[4]; int
    who[4];}` and indexed [2 * m + v]: m picks the measure (CPU average,
    render peak, altex KB, obj KB), v is 1 for a slot-0 item with flag 4.
  - log (+0x1A0), the text itmtst_item_record appends to.
  - The four local views in rob_disp.c (stat_tab, rec_table, tab5304,
    tbl) are gone, and so is the `float *` view in
    itmtst_sample_store. itmtst_samples_average read the columns through
    `(char *)&DAT_0c4c5314 - 16`, DAT_0c4c5314 being cpu[0]; it now
    reads itmtst_stats.cpu / .render / .ta and still matches, and the
    two DAT_0c4c5314 externs are gone.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the reload queue, item_pending_mark, rob_root_offset, net_select_rec

- **sound_reload_rec** (0x14 bytes) types sound_reload_job: state,
  abort_step, size, addr, fp, list. sound_reload_queue_step's local
  `ldr` view and sound_reload_abort_step's `struct st` are gone.
- **sound_reload_entry** {slot, name} types sound_reload_queue as nine
  entries (sound_reload_queue_clear sets nine, sound_reload_queue_add
  fills at most eight). The add and clear loops walk an entry pointer
  in place of the `int *` pair steps.
- **item_pending_mark_rec** {sprite, x, y} types item_pending_mark.
- **rob_root_offset** is a vec3.
- **net_select_rec** (0x39 bytes, no padding) is one player's
  "cmd=select" answer: valid, mask, access_code[17], equip[8],
  color[12], items[16], emblem, emblem_dan. It types net_select_scratch
  and net_select_pending[2].
  - Every reply parser, net_select_pending_apply and
    net_select_reply_store now spell members. That covers the `+ 3`
    base views, the `idx * 0x39` byte steps and the four DAT_ names
    inside the record (DAT_0c37d75a, _77f, _78f, _790).
  - All of it still matches. GCC makes the same `+3` pool words from
    the member spelling.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: agg_ arrays and the next records (batches 4-6)

- **build/structs/agg_retype.py**: the agg_retype.py sibling of
  retype.py for the rows AssignAggregates typed `agg_<addr>` whose
  evidence proposes a scalar array.
  - It gives the row, the header and the externs the proposed
    declaration.
  - It rewrites `X.field_0xK` and `(&X.field_0x0)[e]` as indexing, then
    applies retype.py's rewrites.
  - It keeps a row only when every file using it still matches with no
    new warning.
  - Run on the 20 proposals whose element really is the datum (the
    byte-array proposals for records such as robs or step_fx were left
    for structs). 15 kept:
    - robs_push_dir float[3]; effect_heap_stats int[10];
    - snow_cas_timings float[10];
    - snow_spray_lists and sand_puff_lists uchar[2][81];
    - leaf_fly_models and leaf_shadow_models `undefined *[2]`;
    - vp_rank_weights short[8];
    - prize_item_chance, prize_money and lottery_reels;
    - net_bookmark_flags char[2]; objbank_mem_sizes int[21];
    - sprite_set_loaded char[81]; aet_file_table_hdr int[2].
- **By hand:**
  - sound_bank_files `char *[80]`: the `*(undefined4 *)(&X.field_0x0 + n * 4)`
    reads are now `sound_bank_files[n]`.
  - prize_item int[2] and fish_mot_list short[2], with their casts.
- **ground_frame_rec** {byte; short frame} types snow_ground_frame and
  sand_ground_frame. The analysis proposed short[2], but the game stores
  a byte at +0. snow_grid_build reaches both members through one pool
  word (0x0C38CD4C), which separate globals could not share, so each is
  one object.
- **Records:**
  - effect_slot[50]: on, name and the init / exec / draw / free hooks. The
    effect.c loops call `s->exec()` in place of `(*(code *)p->field_0xc)()`.
  - item_lottery_rec[2]: state, roll, result.
  - msn_text_color[10]: name, color, color2.
  - aet_stack_rec: n, top, e[30] of aet_stack_entry.
  - jisaku_marks int[2][3].
  - est_row[14]: `{short s[5]; float f[9];}`, the hit parameter row of
    est_data and est_data_default. The custom record at rob +0x380 has
    the same layout.
  - aet_file_list {count, rows}.
  - card_renew_text[27]: up to eight line pointers.
  - fish_path_rec (0x50) and fish_path_seg (0x1C), the path fish and its
    script step. They type fish_solo1..3. fish_path_follow's and
    fish_solo_setup's local views are gone, and the solo routines spell
    members.
- **Kept as they were, tagged ORIGINAL SHAPE:**
  - aet_stack_push's byte-offset entry address.
  - card_renew_text_draw's two word reads of the texts. The member
    spellings change its address arithmetic.
  - msn_text_color_by_name reads `(msn_text_colors + i)->name`;
    `msn_text_colors[i].name` swaps two registers.
  - jisaku_reset lost the permuter's `new_var` (`jisaku_marks[i][k]`
    matches).
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the wind and the stage record (batches 7-8)

- **wind_rec** (0x74 bytes) types bone_chain_wind and stage_wind:
  - the vector;
  - bias, cycle[8], ratio[8] (the debug menu's names), the x / y angles
    and phase[8].
  - **wind_preset** (0x20) types stage_wind_table[24], the per-stage rows
    stage_wind_set copies.
  - That loop clears from wave 2, so the third pair it copies is zeroed
    again; only waves 0 and 1 blow (the code is the game's).
  - osage.c's wave_set and stage.c's src_t / wind_t views are gone.
  - wind_update keeps its int offsets, tagged ORIGINAL SHAPE. Spelled by
    member, even with its `q = i * 4 + param_1` kept, it adds the base
    first and allocates differently.
- **stage_rec** (0x70 bytes, 24 records from 0x0C306208 with no row of
  their own) types stage_table `stage_rec *[24]`, stage_cur and
  stage_cur_entry.
  - Fields: name, flags, draw lists, object / collision / etc / effect
    files, light set numbers, sound banks, background colours, effect
    list, start positions, fog, placed objects, vertex hook, ring-out
    record, centre (y is the floor), flare, intro camera, light
    direction, shadow scale.
  - All 73 `*(T *)(stage_cur + K)` reads across 20 files are spelled by
    field (build: a scratch rewrite keyed by offset, then by hand for
    stage.c's locals, replay.c's table and the ring-out table).
  - Every file matched on the first try; one warning fewer
    (debug_menu.c), none new.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the effect pools, fences and floor tiles (batches 9-11)

- **step_fx_rec** (0x20) types step_fx[64]: flags, frame, frames, model,
  time, scale, pos. step_fx_spawn returns it, and step_fx_update's
  spark_ent view is gone.
- **audience_anim_set** (12 bytes, [21]): step, frames, first_obj, objs.
- **fish_model_set** (0x1C, [5]): first_obj, objs, clumps3 / clumps6 (the
  merged-model frames model_clump_frames_build makes), frames,
  x_range / y_range. fish_path_spawn's and fish_school_member_init's
  views are gone.
- **nickname_point** (16 bytes) types nickname_points[21][2]
  (name x 3 + counter, then side): count, state, work[3].
- **fx_rec** (0x3C) types rob_fx_pool[16] and down_fx[2].
  - Fields: flags, wait, slot, frames, frame, turn, model, time, speed,
    scale, frame_table, rob, pos, dir.
  - fx_anim_step / fx_anim_reset take fx_rec * and rob_fx_spawn returns
    one.
  - The rob_fx, down_fx and rob_fx_slot_spawn code spells members.
  - rob_fx_draw keeps the permuter's new_var / new_var2.
- **break_piece** (0x24) is one breakable stage piece: the fence
  segments (rome_fence_row0 / 1, yam_fence_row0 / 1, djo_fence_segs,
  25 each with the -1 end) and the BREAK floor's break_floor_tiles[401].
  - Fields: pos, turn, model, model2 (a fence's post, a tile's cracked
    object), anims (the break animations), idx, turn2, flags.
  - The two uses had the same offsets and meanings, so they share the
    type.
  - The fence routines take break_piece *: fence_rows_draw,
    fence_segment_break_update, djo_fence_break_check,
    djo_fence_fall_start, djo_fence_segments_draw.
  - rome_fence_rows / yam_fence_rows are `break_piece *[3]`.
  - **djo_fence_fall_rec** (0x1C) types djo_fence_fall[24]. Its time
    member is the old DAT_0C386E0C, whose extern is gone.
- **Tagged ORIGINAL SHAPE:**
  - fence_segment_break_set's `(djo_fence_fall + i)->time`;
    djo_fence_fall[i].time swaps two registers.
  - fence_segment_break_update takes the next neighbour as a pointer
    first (`n = param_1 + next`); `param_1[next]` adds the base before
    the offset.
- Warnings fall (0c0b06a0.c 149 to 126, nickname.c 33 to 31), none new.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the rope, rain, dust and load queue records (batches 12-13)

- **rope_link** (0x70) types rope_links[128].
  - Fields: state, len_prev, len_next, pos, old, vel, the matrix m[16].
  - rope_link_step takes rope_link * and reaches its neighbours as
    param_1[1] / param_1[-1].
  - rob_rope.c's fx_ent / ent views and the `T` macro are gone.
  - DAT_0C39862C and DAT_0C39BD98 were rope_links[0].m and
    rope_links[127].pos; their externs are gone.
- **rain_streak** (100 bytes) types rain_streaks[36]: pos, vel, base, vel0,
  shape[3], dist, len, idx.
  - rain_streak_setup / _respawn keep their float * walks (matched shapes
    such as `s = t - 7`); their callers pass &p->pos.x.
- **dust_rec** (0x24C) types dust_records[20]: flag, n, model, u / v, du /
  dv, uv[70].
  - uv is an array of {s, t} pairs. Spelled `float uv[70][2]`,
    dust_uv_base_save allocates its registers differently, which matches
    the original's pair view.
- **poly_load_entry** (0x114) types poly_queue[32]. It is
  load_poly_bg_service's local struct, moved to the header, with
  **obj_mem** {p, a, b} for its memory block.
  - poly_queue_reset and poly_queue_entry_alloc spell members.
  - load_poly_req / objbank_free_req still fill entries through word
    views; load_poly_req is permuter output.
- No new warnings (0c0b06a0.c 126 to 119).
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the NYC helicopters and fish_seeker's parameters (batches 14-15)

- **nyc_heli** (0x4c) types nyc_helis[4]: the path times (t0, t, tmax,
  trst), the rotor times, pos, dir (the light's direction), rot, the
  rotor angles, the light's aim (ry, rz) and the motion id.
  - nyc_light_init, nyc_heli_update, nyc_light_draw and nyc_heli_pos_get
    spell members; their local views are gone.
  - id is a signed char: nyc_light_init's read sign-extends.
    nyc_heli_update still reads it through a volatile view (tagged
    ORIGINAL SHAPE); without it the function differs.
  - nyc_heli_pos_get takes the record's address first; indexing straight
    into pos adds the offset before the index.
- **fish_seeker_prm** (0x1c) types fish_seeker_params: keep (the heading
  weight fish_seeker_steer uses), cruise, fast, close, step.
  fish_seeker_rec.prm points at it and fish_seeker_update spells members
  instead of prm[3..6].
- No new warnings (0c0b06a0.c 119 to 118).
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the vp work, collision and rob records (batches 16-18)

- **vp_work** (0x88) types vp_work_1p and vp_work_2p: flags (+0x2c), the
  move (+0x3c), the level parameters, both character tables, the debug
  renkei, the other side's record, the trait block, the rob, the
  history, the rob snapshot and the log.
  - vp_work_init, vp_round_init (its local view is gone), vp_judge_init,
    vp_work_disable, vp_both_enabled, vp_move_change_note and
    vp_rob_state_snapshot spell members.
  - vp_think and the vp_* helpers still take the record as an int.
- **rob_coli_rec** (0x460) types rob_coli_records[2]: the rob, the two
  sphere banks and their pointers, flags, the offsets (+0x310), the
  radii. rob_coli_clear's local view moved to the header;
  rob_coli_task_start spells members; DAT_0C37D12C was
  rob_coli_records[1] and its extern is gone.
- **rob_rec** (0x86c) types robs[2]. Only the members the evidence names
  are spelled (move_no, heading, flags, state, coli, ctrl, pos, the
  rob_old_push ring at +0x580); the rest is padding, since most code
  still reaches the rob through rob_get's pointer and an offset.
  make_old_task_frame, rob_get and rob_info_window_refresh use it.
- No new warnings (vp_main.c 48 to 43, rob_ctrl.c 54 to 50).
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the snow records and the NYC searchlights (batches 19-20)

- **snow_flake** (0x28) types snow_flakes[150], **snow_sway_circle** (16)
  types snow_sway[16], **snow_sheet** (0x28) types snow_sheets[50].
  - The views disagreed only on names. The "seed" at +0x24 / +0x20 is a
    random start angle (the DJO code turns it as spin). The sheets' +0x1c
    is a short angle (place) that the spot tables write as a word.
  - snow_flakes_draw's `&snow_yaw_delta + 4` view was snow_sway itself; it
    now reads snow_sway[p->sway].x.
  - All of effect_snow.c's snow routines spell members, the DJO ones too.
- **nyc_searchlight** (0x70): aim, the lamp's pos, the lit point at, the
  colour and ranges, pitch / yaw / roll, model, flags.
  - The old nyc_searchlights[4] is four objects. nyc_light_init loads
    0x0C386C6C and 0x0C386CDC as two pool constants (one array would be
    base + 0x70). The fixed pair was always reached through its own
    symbol.
  - So: nyc_searchlights[2] (the pair that follows the fighters),
    nyc_searchlights_fixed[2], nyc_heli_light0 and nyc_heli_light1 (new
    rows; agg_0c386b8c / c6c / cdc are gone).
  - nyc_heli_draw never reads the light it is passed, so the strided
    address for helicopters 2 and 3 is harmless.
  - nyc_light_update, nyc_light_draw, nyc_light_init and
    nyc_searchlights_update spell members.
- No new warnings.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the graphics library's state block (batch 21)

- **gfx_state_rec** types gfx_state (was agg_0c41e010 /
  agg_0c41e010_0c41e010). It is the generated layout member for member
  (the types the sign and pointer passes gave it), up to +0x578.
  - +0x57C is bank_build_index's own row, which source uses on its own,
    so the record stops before it.
  - Named from the rows' evidence: flags (+0x8), vram_size (+0x18),
    frame_idx (+0x354), vblank_ticks (+0x36C), credits_text (+0x524).
    The other members keep their offsets.
  - All 25 files that use it (sdk/kamui and gfxlib) were renamed; the
    match counts and warnings are the same as before in each.
- Every named object that was typed `agg_` now has a record type. The
  244 unnamed `agg_X_X` globals are left; naming them is data naming,
  not struct work.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows, second pass (14 retyped)

- Of the 125 rows put back, 14 now take a type. Each file that uses one
  still matches with the same warnings as before.
  - The proposal held for some. For the others the code reads the value
    with the other signedness (an extu or exts the proposal would
    change), so the row is declared that way and its note says so.
  - The 14: input_mute, nickname_rates, leaf_grid,
    enemy_ai_attack_frames, enemy_state_frames, enemy_pad_idle_frames,
    vp_frame_count, net_ready_at_stage, net_db_stat, net_log_slot_cur,
    alab_step_usec, poly_queue_cur, poly_queue_next, eye_index.
- Four that matched were left untyped: rob_item_models and sprite_sets
  overlap another row, and leaf_kick_tuning and hbr_clock_hands are
  separate globals in the game (their own pool constants), so one array
  type would be wrong.
- 111 rows are left: those four, 49 whose type adds a warning at a use
  (a pointer passed where the callee takes an integer, and the like),
  and 58 whose type changes the code.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows, third pass (26 retyped)

- 26 of the rows whose type added a warning now have it. The code around
  each was typed to match, not cast:
  - **aet_cur_obj, aet_cur_still, aet_cur_cache**: their accessors take
    and return `undefined *`, and so do the locals that hold them (and
    aet_still_quad_cache_store's still argument).
  - **mission_count, lottery_state, lottery_symbol, prize_got_item,
    prize_got_money, objbank_freeing, mdl_keep_node_req,
    texdesc_free_slot, sand_timing_last_frame**: these are counts, states
    and indices, so they are `int`.
    - The `(undefined *)` and `(int)` casts around them are gone
      (build/structs/intify.py).
    - lottery_symbol_get, mdl_keep_node_get / _set and objbank_free_start
      now take or return int; their callers already passed integers.
  - **mission_reward_items** was `undefined *[10][19]`. Its words are item
    numbers (-1 for none, 1 to 106), so it is `int`.
  - **Tables read through a local:** snow_flake_tris, fx_saved_mtx,
    sand_grain_models, rain_drip_models, rain_splash_models,
    enemy_script_stand, msn_text_font_wide / _narrow, msn_text_state_ptr,
    net_side_suffixes, osage_edit_coli_index and card_renew_times. The
    local now has the table's element type.
    - The rain bakes keep one flat index, tagged ORIGINAL SHAPE, because
      `[i][j]` does not match.
  - **adv_fx_anims** is a record, adv_fx_anim (a short frame and a float
    clock), in place of the proposed `char[6][8]` and the two local
    typedefs that spelled it.
  - **net_req_buf** is `int[4]`, the type strbuf_init takes.
- Every file matches, and each one's warnings are the same as before or
  fewer.
- 85 rows are left.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows, fourth pass (15 retyped)

- **rob_eval_defs** is `bone_def *`:
  - rob_pose_eval passes bone_defs.
  - rob_eval_ik's second argument is the bone_def it walks. Its byte
    offsets are now the members bend, pos, rot and children.
  - The local `ikbone` typedef that spelled bone_def is gone.
- **mot_channel_pool, rob_bone_links_pool, mot_dir, mothead_table**:
  - Their alloc and set accessors take and return pointers, and the
    callers' slot reads say `*(undefined **)`.
  - The two load addresses system.c passes keep a cast at that boundary,
    and so does mot_dir_get's byte offset.
- **txn_tune_list, txn_color_list, txn_swap_list**: the put functions
  take `void *`, so the callers' casts are gone. tex_txn.c's pinned
  DAT_0C322764 says `undefined *` as the header does.
- **stage_fog_record** is `fog_rec *`, the stage record's +0x38.
- **mot_paths_block**: mot_paths_bind returns `void *`, the block it
  allocates.
- **shadow_volume_draw_fn**: the function address is held as
  `undefined *`.
- **break_buf**: stage_anim_buffers_carve takes the buffer as
  `undefined *` and casts where it stores the addresses in int slots.
- **break_fall_list_end** is `undefined **`, a cursor into break_fall_list.
  The draw loop steps it with `++`, not `*(int **)&` arithmetic.
- **fish_school, snow_grid**: the locals that walk them are pointers.
  - In snow_cell_normal_packed and snow_body_press a pointer local does
    not match, so those two keep an int base, tagged ORIGINAL SHAPE.
- **leaf_records** was tried and put back. Every leaf routine (grid
  update, cell add and remove, the tri and quad builders) takes the
  record as `int`, so it needs a leaf record type first.
- 70 rows are left.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows, fifth pass (31 retyped)

- **A retype.py rewrite bug, fixed.** The rule for `&X + i * s` (byte
  arithmetic on a one-byte old type) gave `X[i]` for a 1-D array, a
  value where the source wants an address. A row that used it could
  only mismatch, so no committed row has that spelling. Now:
  - `*(E *)(&X + i * s)` gives `X[i]`.
  - `(E *)(&X + i * s)` and a bare `&X + i * s` give `&X[i]` (1-D),
    or the row `X[i]` for more dimensions.
  - An integer element read through an integer type of its own size,
    `*(undefined4 *)(&X[i])`, gives `X[i]`. A float read as an int is a
    bit copy and stays.
- **26 rows the bug had put back** now match with the same warnings:
  - control_dir_moves, control_buffer_kind_masks, rob_load_slots,
    rob_cmd_table_peak.
  - card_unit_cmd_cooldown, item_award_mes_timer, item_award_kind,
    card_upgraded.
  - rain_extra_model_ids, rain_sphere_splash_model_ids.
  - lottery_bonus_odds, lottery_symbol_values, msn_text_image_mirror,
    mission_air_combo_seen, mission_down_seen, quest_meter_changed,
    prize_item_fallback, msnm_listed_states, msnm_hyper_counts.
  - net_test_map_keys, net_test_map_keys2, net_month_days.
  - chara3_item_defaults, chara3_item_values, extra_sound_banks,
    pow10_small.
- **osage_rob, osage_node_next, osage_node_first, bone_chain_nodes_cur,
  osage_coli_list** are `undefined *`. The casts at their uses are gone
  or moved to where an int slot takes the address. osage.c's warnings
  went from 138 to 131.
- **rob_hand_models, rob_hand_build_src, rob_hand_build_dst** stay
  untyped. They hold model handles, and the model API (model_draw,
  model_copy_to_addr) takes those as `int` in 11 files.
- 39 rows are left.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows, sixth pass (14 retyped)

- **retype.py covers more index spellings:**
  - An index with casts or in parentheses: `(char)param_1`,
    `(uint)*pbVar4`, `((int)vp_rand() % n)`, `(uint)quest_count[i]`.
  - `((&X) + (i * 4))`, with its extra parentheses.
  - A cast in any spelling of the same type: `ushort` and
    `unsigned short`, `uint` and `undefined4`.
  - A new extern is spelled as the header spells it (uchar, ushort,
    uint), so check_externs.py stays at 44.
- **8 rows retype.py keeps now:** attack_point_masks, vp_drink_inputs,
  vp_exc_p_or_k, vp_exc_kicks, vp_stance_by_player, msnm_char_items,
  msnm_char_item_counts, card_mark_models.
- **6 rows with one use spelled by hand:**
  - **card_flags** (`uint[2]`): card_mode_player_reset clears it as
    `card_flags[param_1] = 0`. It used to reach it through the byte
    offset it shares with card_mode_slots.
  - **quest_count** (`ushort[2]`): quest_judge passes `quest_count[i]`
    to card_hdr_b3_high_set, whose argument is a byte.
  - **quest_total_targets** (`int[16]`): indexed by `quest_count[i]`.
  - **rob_draw_mode** is `uint[2]`, not the proposed int. rob_draw
    switches on it unsigned, and an int does not match there. rob_draw's
    local `tab` is a `uint *`.
  - **rob_card_item_lists** is `int[2][4]`, not `int[8]`.
    rob_card_item_obj_list steps it 16 bytes per side.
  - **light_mot_pairs** is `short[4][4]`, not `short[16]`.
    light_mot_paths_bind passes one 8-byte row per choice.
  - Warnings went down by one each in rob_disp.c and effect_mot.c.
- **vp_mash_inputs** stays put back. As `short[2][2]` it matches only
  while vp_input_yoroke_mash keeps the dead `i = c * 4` it no longer
  uses.
- 25 rows are left.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows, seventh pass (7 retyped, 3 closed)

- **retype.py follows nested strides.** In `&X + i * s0 + j * s1 ..`,
  each term whose stride is the next dimension's row size becomes one
  index, so `&rob_zanzou_trails + a * 0x18c + b * 0x2c` is
  `rob_zanzou_trails[a][b]`. A partial index used as an address gives a
  pointer to its first element.
- **Retyped:**
  - rob_zanzou_trails (`int[2][9][11]`).
  - nickname_thresholds (`short[7][8][3]`), msnm_combo_variants
    (`undefined4[57][8]`) and rob_base_mtx (`float[2][16]`). Their get
    functions return `undefined *`, so the row each returns is cast
    there.
  - msnm_money_rewards (`int[8][5]`): `msnm_money_rewards[param_2][param_1]`
    for the flat `(param_2 * 5 + param_1) * 4`.
  - input_log (`char[2][168]`): the per-side `(char *)&input_log + i * 0xa8`
    is `input_log[i]`.
  - leaf_fly_shapes is `float[64][3][3]`, not `float[576]`: 64
    triangles of three points, one per mtx_xform_points call. That
    function takes `float (*)[3]`, so effect_leaf.c has two fewer
    warnings.
- **Closed with the matched type, the proposal noted as wrong:**
  - sky_state is `uint`, not char. sky_objs_exec sets its bits through
    a 32-bit bit-field word.
  - leaf_active_side is `uchar`, not int. The matched code reads one
    byte; as an int, 6 of effect_leaf.c's functions differ.
  - mission_last_health stays `short`, not int. mission_health_check
    compares and stores it as the rob's 16-bit health.
- **Put back because they need a record type first:**
  - osage_nodes: its 44-byte node is `bone_ent`, a typedef local to one
    rob_disp.c function, and osage.c reaches the same nodes by byte
    offset. As `int[2][100][11]` it matches but adds 28 warnings at the
    osage_node_link_set calls.
  - item_zanzou_items: item_zanzou_setup writes it through a local
    `Bent` record at byte offsets.
  - screen_noise_records (0x420-byte `fx_outer` records),
    msn_text_images (40-byte entries), mot_path_slots (8-byte slots) and
    texcol_scratch_a (16-byte color list entries).
- 15 rows are left.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the source-typed rows closed out, and 440 functions refiled

- **hbr_clock_hands, leaf_kick_tuning** are closed as separate globals,
  not arrays. The code reaches the next words through their own pool
  words.
- **13 rows stay open, each waiting on something else:**
  - Nine need a record type first: osage_nodes, item_zanzou_items,
    screen_noise_records, msn_text_images, mot_path_slots,
    texcol_scratch_a and leaf_records, plus rob_item_models and
    sprite_sets, whose proposed sizes overlap the next symbol.
  - rob_hand_models, rob_hand_build_src and rob_hand_build_dst hold
    model handles, which the model API takes as int.
  - vp_mash_inputs matches only with a dead store kept.
- **440 functions moved out of `_unmapped/`** into the files
  ghidra/source_map.tsv places them in:
  - sprite.c 90, card_mode.c 69, card.c 71, effect_etc.c 182,
    effect_rain.c 28.
  - Six group files had no functions left and are gone; 26 remain.
  - 65 placed functions stay in `_unmapped/`.
- **Three refile.py fixes** made this possible:
  - **A helper whose return type has a struct body**
    (`static inline struct { char c[1]; } *byte_view(..)`) was read as
    data, so the target file never got it. card_mode.c did not compile.
  - **Parameter order.** keys_vec3_sample, sprite_anim_scale_eval and
    sprite_anim_alpha_eval were defined `(int *, float *, float *, float)`.
    Their callers, which have no declaration, pass the float first. On
    the SH-4 floats and integers go in separate registers, so both
    orders compile alike, but in one file the definition's prototype
    reaches the callers. They now take `(float, int *, float *, float *)`,
    as the callers load them.
  - **A moved function that brings a NOPROTO_/KRPROTO_ define** for a
    callee it calls stays where it is when a function already in the
    file stops matching. rain_sheets_draw calls mtx_rot_y_s16 through r0
    (an implicit int declaration), while effect_rain.c's
    rain_splash_draw calls it through r1 (the void prototype). One file
    cannot hold both, so rain_sheets_draw probably belongs to another
    file.
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-03: the 17 unmatched main.bin functions, measured again

- **Every best text was compiled again against today's tree**, its old
  global names translated to today's symbols. The struct and refile
  passes moved none of them: the smallest gaps are still 2 lines
  (elan_lights_queue, legacy_io_start, 0x0C0E5A40), then 3 to 10 for
  seven more, up to 148 for 0x0C03D9A0. render_done_irq (0x0C148C80)
  matches only with the steering text that is held.
- **elan_lights_queue (0x0C0FC220).** The game builds at -O1 with GCC
  3.2.3, where no scheduling pass runs, so the game's zeroing of i at
  the top of the if block is where the source wrote `i = 0;`. Written
  there, it lands in the game's place. But CSE then stores i's register
  into dl.size, where the game loads a fresh zero. GCC 3.2's CSE drops
  that equivalence only at a label or a volatile asm, and the game's
  block has neither. What would match is steering, so it stays at 2
  lines.
- The full list, with each one's gap, is in build/agents/rtl-leads.md.
- **All 17 are used.** Each one's address sits in a pool word of a
  matched caller (pvr_irq_service, robs_hits_resolve, light_set_send,
  winner_mes_draw and others), so none is dead code.

## 2026-11-04: 52 more functions out of `_unmapped/`, with local declarations

- **Why they would not move.** Each was in the middle of its target
  file's address run, so no file boundary was missing. What differed
  was how a callee was declared:
  - the target file calls mtx_rot_y_s16, mtx_pop, obj_draw and others
    undeclared (its NOPROTO_ defines);
  - the moved function calls them with a prototype or a K&R
    declaration.
  - With the game's compiler a value-returning callee is called through
    r0 and a void one through r1, so the declaration each caller saw
    shows in the game's bytes.
- **The two kinds of caller alternate function by function** in
  effect_etc.c: undeclared, declared, undeclared, and so on. A
  declaration at file scope stays in force for the rest of the file,
  and a linker cannot interleave two object files. Tested on GCC 3.2.3,
  a declaration inside one function changes that function's calls only.
  It is the one shape that gives both in one file.
- **The 52 now declare those callees at the top of their bodies**, as
  their group file saw them, tagged ORIGINAL SHAPE with that evidence:
  - effect_etc.c 33, object_put.c 4, rob_item.c 3, card.c 3,
    mot_tools.c 2, select.c 2;
  - rob_action.c, console.c, debug_menu.c, msnm_disp.c and sound.c,
    one each.
  - Eight of the 52 needed no declaration; their batch had failed
    because of the others.
  - 13 group files had nothing left and are gone; 13 remain.
- **13 functions stay in `_unmapped/`:**
  - Eight saw the callee undeclared where their target declares it,
    which a block cannot undo; they need the file's declarations
    reordered: obj_put_lit and its five siblings, rain_sheets_draw,
    card_syu_not_both_2.
  - level_result_10_19 and msn_panel_draw: the local declaration
    conflicts with a definition in the same file.
  - card_image_streaks_clear (it breaks level_result_20_up) and
    prize_item_choose differ for reasons not found yet.
  - node_list_insert_after is built -mspace, unlike vram_alloc.c.

## 2026-11-04: seven of those by declaration order

- **rain_sheets_draw** is in effect_rain.c. It calls mtx_rot_y_s16
  undeclared (through r0), and every later caller calls it as void
  (through r1). The file now declares it at file scope right after
  rain_sheets_draw. Tested on GCC 3.2.3, a declaration after an implicit
  use compiles, and only calls after it change.
- **obj_put_lit, _f, _world, _world_f, obj_put_second_path and
  obj_put_second_path_float** are in object_put.c. They come after the
  definitions of obj_light_vecs_to_model, obj_light_vecs_to_model_view
  and obj_mtx_push_proj_view, yet the game calls all three from them as
  returning a value (r0).
  - Those three are now defined without a return type (old-style
    implicit int). The game's bytes for them are the same either way,
    because they return nothing.
  - Their callers before the definitions that the game calls as void
    (obj_put_typed and its three siblings) declare them void inside
    their bodies.
  - Tested on GCC 3.2.3: a block-scope void declaration before an
    implicit-int definition compiles, and gives r1 before and r0 after.
- **card_syu_not_both_2 stays.** Its difference is the argument: the game
  passes it without the zero extension the `byte` prototype gives. It
  sits right after that prototyped definition, between prototyped
  callers, so one file cannot give it that call without several guesses
  stacked together.
- Four more group files are gone; nine remain, holding six functions
  the source map places.

## 2026-11-04: the export missed four matched functions

- **What showed it.** The unmatched list had 21 main.bin functions,
  not 17. The four extras matched in source/, but the export to src/
  had not recognized them.
- **obj_light_vecs_to_model, its `_view` twin and
  obj_mtx_push_proj_view.** ghidra/ExportSource.java's definition
  pattern needed a return type before the name, so their definitions
  without one were skipped. src/ got the decompiler's text instead.
  - The pattern now takes a definition without a return type, and a
    comment at the end of its signature line.
  - The header declares such a function `int`, which is what the
    definition is. No other file calls these three.
- **fish_path_update.** fish_solo_setup, moved in from a group file,
  ended in `  }}`. The pattern reads a body up to the first closing
  brace at the start of a line, so fish_solo_setup took
  fish_path_update with it, and src/effect_etc.c defined
  fish_path_update twice. The brace now has its own line.
- 17 main.bin and 78 test.bin functions are unmatched (test.bin's copy
  of object_put.c had the same three).
- 6,249 of 6,249 match, test.bin 46 of 46.

## 2026-11-04: five of the last six out of `_unmapped/`

- **card_image_streaks_clear and level_result_10_19** are in card.c.
  - card_image_streaks_clear is defined old-style there, as its group
    file had it. Its first move failed for a reason in the move, not in
    the function: the move had dropped the old-style declaration, and a
    void function left undeclared returns int to its later callers
    (level_result_20_up then called it through r0, not r1).
  - level_result_10_19 needed no declaration once it sat in its place.
- **prize_item_choose** is in mission.c. prize_item_chance_get, its one
  caller's callee, is now an old-style definition: the game passes it
  the char unextended, as a caller that sees no prototype does.
- **msn_panel_draw** is in msnm_disp.c. msn_panel_base_create and
  msn_count_draw are now old-style definitions. msn_panel_draw comes
  before them and calls them with no prototype (through r1, so as void),
  and GCC 3.2.3 refuses a prototyped definition with promoted parameter
  types after such a declaration.
- **node_list_insert_after** is in vram_alloc.c, between
  node_list_push_front and node_list_unlink, with no -mspace.
  - The game leaves its else label unaligned. Every other function in
    the file aligns the label after a branch to 32 bytes.
  - The cause is in GCC 3.2.3's sh.c. machine reorg runs
    shorten_branches once before the constant-pool fixup. At that point
    barrier_align gives every label after a barrier the full cache-line
    alignment, and final.c keeps the larger value later. So a label
    that survives machine reorg is always aligned.
  - A label escapes only when delay-slot filling makes a new one. It
    does that when it deletes the first instruction at the branch
    target as redundant and sends the branch past it.
  - node_list_push_front, just before it, shows this. Its else branch
    reads `*param_1` a second time, the load is deleted, and its label
    is unaligned. 129 labels in default-flag files are unaligned the
    same way.
  - node_list_insert_after's else branch now reads the link a second
    time into the same variable. That compiles to the game's 28 bytes
    and is tagged as the shape kept for the match.
- **card_syu_not_both_2 is the one function left** (its argument
  extension, entry above). Seven group files had no functions left,
  and are gone: 0c0200c0.c, 0c072320.c, 0c0877e0.c, 0c0a93a0.c,
  0c0b06a0.c, 0c0e0980.c and 0c101b80.c. They held only the externs and
  inline helpers their functions had used.

## 2026-11-04: four source-typed rows get their record types

- **msn_text_images** is `msn_text_image[25]`. Each 40-byte image has a
  sprite, the tag's name, a width and height, an offset, an angle, flip
  bits and a colour pair (the data reads "P", "K", "G", "1".."9",
  "R1".."R9", "YOGA".."ROLL2684"). msnm_text.c's three uses index it,
  and msn_text_image_draw lost its local copy of the type.
- **mot_path_slots** is `mot_path_slot[3]`: a bone record and a motion
  record, held as the addresses the rob bone calls take.
  - mot_path_eval takes a `mot_path_slot *`.
  - mot_paths_bind walks the slots by pointer.
  - nyc_heli_update matches with the slot in a pointer local. Spelled
    `mot_path_slots[i].bone`, it keeps the table's address in one more
    register.
- **texcol_scratch_a** is `texcol_src[16]`: the 16-byte source colour
  entries (id, two floats, a value scaled by 180 / 32768) that
  texcol_lists_combine had typed locally.
- **screen_noise_records** is `screen_noise_rec[2]`: per side a level,
  an angle and one rolling bar (`screen_noise_bar`: its line, its speed,
  257 offsets and two floats).
  - noise_roll_bar_add takes the record and works through the bar's
    members, not byte offsets.
  - stage_screen.c has two fewer warnings.

## 2026-11-04: the source-typed rows, all closed

- **rob_item_models** is `rob_item_model[2][4]`. Its records are 0x14
  bytes, not the 0x18 the analysis proposed. That is why the proposed
  size ran into rob_item_envelopes. rob_item_envelopes, the 0x18-byte
  ones, is now `rob_item_env[2][4]`. rob_items_from_part_test indexes
  both and lost its two local types.
- **sprite_sets** is `sprite_set[81]` {name, texture base, sprite list}.
  - Its proposed size ran over ltop_file, which was set 0x33's own name
    inside the table ("spr_fire.bin", the L-TOP stream). That row is
    gone, and ltop_frame_read reads `sprite_sets[0x33].name`.
  - Eight functions in sprite.c and aetset.c lost their local views of
    the set.
  - sel_ft_sprite_files_load matches with the set in a pointer local,
    as nyc_heli_update does.
- **leaf_records** is `leaf_rec *` (0x44 bytes: position, velocity,
  normal, spin, floor, packed normal, three angles, cell, id, mode,
  source, shape). The grid and spawn code index it; the functions that
  walk leaves as integers keep that view. effect_leaf.c has one fewer
  warning.
- **item_zanzou_items** (`zanzou_item[2][10]`) and **osage_nodes**
  (`osage_node[2][100]`) have their record types.
  - rob_disp.c's code starts from the typed tables but keeps its byte
    views.
  - The shapes there are register-sensitive: item_zanzou_setup's
    offsets in locals, rob_osage_init's sums.
  - osage_node is rob_osage_init's local bone_ent, named.
- **rob_hand_models, rob_hand_build_src, rob_hand_build_dst** are `int`
  (rob_hand_models `int[2][32]`). They hold model handles, and
  model_draw, model_size and model_copy_to_addr take handles as int.
  The proposed pointers were wrong.
- **vp_mash_inputs** is `short[2][2]`. It no longer needs the dead
  `i = c * 4`. The game computes the scaled index before the
  destination, and a row pointer, `m = vp_mash_inputs[c]`, does the
  same.
- No source-typed rows are left.

## 2026-11-04: members named in vp_work, rob_rec and gfx_state_rec

- **vp_work** (the CPU opponent's state, 0x88 bytes): the members get
  the names the vp_main.c function rows give their offsets. Examples:
  - the tech roll and rise types, the dodge and guard types
  - the step counters of the stroke, run and escape sequences (0xff
    from vp_round_init)
  - the mode and idle frame counts and the two reaction delays
  - the frame disadvantage
  - the stroke result bits, the mode and exception-mode indexes
  - the running and queued input sequences, the combo list, the
    chain step

  tools/rename_members.py rewrote 17 uses in vp_main.c.
- **rob_rec** (0x86c bytes): eight members carved out of its padding,
  from the rows that read them:
  - side, character and costume (vp_chara_table_get, rob_osage_init)
  - the opponent's record at +0x34 (vp_round_init)
  - the bone matrices at +0x54 (rob_hand_draw, item_zanzou_calc)
  - the body model at +0x540 (rob_osage_init)
  - the stance at +0x550 (vp_stance_get)
  - the item slots at +0x56c (rob_items_clear)

  source/ reads these at byte offsets, so no uses change.
- **gfx_state_rec**: five of its 886 offset members, from the KamuiTL
  rows:
  - the tile grid's width and height, tile_cols and tile_rows (+0x468 by
    +0x46c, tile_split_bitmap_update and tile_regions_write)
  - the two chips' tile split words, split_word_a and split_word_b
    (+0x498, +0x4d0, 0x38 apart)
  - kept_list (+0x42c, 0x20 + the list bank_lists_seed keeps open)

  The rename tool rewrote 47 uses; nine more went through a cast
  pointer and were renamed alongside. The hook slots at +0x230 and the
  per-frame split words at +0x41c are arrays the analysis split into
  members, so they stay for the array pass.
- Compile-time offset checks hold every new member at its offset, and
  every file that uses them matches.

## 2026-11-04: header leads closed; test.bin's diff.py compiles again

- **dummy_tri_link_on_lists** (0x0C0FF100): naomi2_library.c now
  defines `NOPROTO_dummy_tri_link_on_lists` and calls it plainly, in
  place of the `((int (*)(uint))...)` cast. The game calls it through r0,
  as for an undeclared function, and the file matches either way. Its
  define gives the header the guard (ExportSource adds one for every
  NOPROTO_ or KRPROTO_ define in source/).
- **gfx_state_rec +0x1b0** is `uint bg_depth`, the background plane's
  depth word:
  - bg_plane_defaults_set and the bg_plane_from_* loaders store it.
  - render_start_ready_frame and the two background writers send it,
    masked with ~0xf, to ISP_BACKGND_D (0xa85f8088).
  - The 18 anonymous `struct { char pad[0x1b0]; int f; }` casts that
    spelled it are now the member, in bg_plane_loaders.c, pvr_irq.c,
    scene_render.c and tex_render.c. All four match.
- **script_peek_move's narrow parameters** were already in place: it is
  defined `(char, char *, short)`, and its one caller declares it with
  NOPROTO_. Only the Next list still named the lead.
- **test.bin's single-function compiles were broken**. diff.py could not
  compile any test.bin function, while match.py, which compiles whole
  source_test/ files with their own externs, never noticed.
  - The cause: TransferFromMain carried four main.bin data rows still
    named by their main.bin address (DAT_0c5cb000, DAT_0c5caff0,
    DAT_0c3f16a0, DAT_0c3f169c, the coin globals of credit.c) to their
    test.bin addresses.
  - Two of those names are also Ghidra's default names for other data
    in test.bin, so src_test/include declared them twice with different
    types.
  - The transfer now skips placeholder names: an address name only
    describes the program it came from.
  - Real names for the four rows are data naming, still open.

## 2026-11-04: round DD, test.bin's own code: 56 more functions match

- **Round DD** (batches 900-907, eight agents) took the 72 unmatched
  test.bin functions no agent had seen: testmode.c's menus and tests,
  card_test.c, and a few library copies.
  - It left out the five copies of functions main.bin has not matched
    either (render_hang_recover, fb_layout_compute and the others).
  - The single-function compiles this round depends on are the ones the
    transfer fix (previous entry) restored.
  - 55 texts match. Each was re-run through diff.py, and none uses
    volatile, asm, `__builtin_expect` or a goto.
  - card_menu_end_hook matches as the decompiler wrote it.
  - adopt.py moved all 56 into source_test/ (testmode/testmode.c,
    testmode/card_test.c and library files).
- **What fixed them**: plain C with the real types.
  - Record structs over card_menu_state's 0x2c-byte units and the
    user_back_data blocks.
  - The exact format strings, including the tail-merged ones such as
    " %s\n".
  - Switches with their fallthroughs.
  - A callee's real argument list: elan_lists_start's fifth argument.
- **One fix while adopting**: card_test_format_step called
  card_record_blank_unpack with no argument. Another agent matched
  card_record_blank_unpack as taking the unit, which the game leaves in
  r4. With `card_record_blank_unpack(param_1)` it matches in its file.
  Both calls of card_test_write_run lost their casts, since its
  definition sits in the same file.
- **The coin globals of credit.c are named**: io_service_buttons and
  io_service_buttons_2 (the nodes' service buttons and coin switches),
  coins_pending[4] (coins read and not yet credited) and
  coin_meter_timers[4] (legacy_io_meters' meter pulses). test.bin now
  gets these names from the transfer too.
- **Still open in test.bin (16 of DD's 72)**: most are blocked by data
  test.bin has no rows for:
  - the stage tables at 0x0C086120 and 0x0C086180
  - the menu row tables
  - 0x0C091084 / 0x0C09108C / 0x0C09104A
  - 0x0C086620
  - the backup time-out settings at 0x0C302048
  - 0x0C345B8C

  Each was matched or nearly matched only through a stand-in symbol or
  a hard-coded address, and held for that reason.
  - 0c0222c0 and 0c024b00 are `match*`: every byte is equal, but the
    `__udivsi3_i4` relocation has no test.bin address to check.
  - Rows for those globals would release most of the 16.

## 2026-11-04: test.bin's test-mode data gets rows

- **49 data rows** in ghidra/symbols_test/menus.tsv for the globals
  round DD's held texts reached through stand-ins. Each row was checked
  against the bytes in work/test.bin and the code's pool words:
  - The stage tables: stage_names[24] ("COLOSSEUM" .. "GREAT WALL",
    with the debug stages) and stage_shown[24], like chara_names and
    chara_shown.
  - The menus' { x, y, text } item tables:
    - game assignments (17 items)
    - time-out setting
    - the daily and weekly closing schedules
    - the two network menus
    - backup clear
  - The { 0, 0, text } option tables difficulty_text indexes by each
    setting: energy, time limit, VS finish, OFF/ON, continue, stage
    select, enemy level.
  - The input test's { format, bit } rows, and the card menu's words,
    rows and colors.
  - The game-assignment settings at 0x0C091000-0x0C09100F, each mapped
    to its menu row through difficulty_text.
  - effect_skip (0x0C091023), which has its own references.
    close_minutes is therefore [7], one per day.
  - The daily closing hour and minute and the schedule type.
  - The network test's request count, buffer (0x0C345388, 1,024 bytes
    up to the next referenced word) and unlock state.
  - The top menu's hidden button code: unlock_code_buttons[12], ending
    in -1, its step and timer. Entering it sets net_log_unlocked, which
    gives NETWORK TEST its TIME OUT SETTING and ACCESS LOG items.
  - The CHALLENGE pages' layout steps (11, 8, 5).
- **button_word**, button_word_update's { now, old, on, off } record, is
  a type now. main.bin's merged record (agg_0c41fbb0, one of the
  unnamed aggregates) is `button_word input_merged`. tools/rename_members.py
  rewrote input.c's four uses. test.bin's copy (0x0C345B8C) gets the
  same row.
- **match.py reads .s files' function headers too.** test.bin's
  libgcc helpers (`__udivsi3_i4`, `__sdivsi3_i4`, `__movstr_i4_*`) are
  exported as libgcc/*.s, which load_symbols skipped, so a call to one
  could not be checked: `match*`. main.bin's sit in an _unmapped .c
  file and were never affected. bookkeep_play_totals and
  backup_data_clear_run (round DD, every byte equal) now give `match`.
- **effect_skip is a uchar**: difficulty_text reads it with extu.b,
  which the held text's `(unsigned char)close_minutes[7]` had stood in
  for. card_item_names[2] ("WRITE TEST", "LOCK ON/OFF") is a row too.
- **Result: 15 more test.bin functions match** (102 to 117).
  - Eleven of round DD's held texts match once their stand-ins are
    replaced by the new names. Nothing else changed in nine of them; for
    the other two (bookkeep_play_totals, backup_data_clear_run) only
    match.py's .s fix was needed.
  - Round DD2 (six agents, one function each) matched four more.
    test_menu, input_test and close_setting_card needed only the names.
    challenge_hiscore needed a record struct over user_back_data+0x24c4,
    as its sibling challenge_total_bookkeep has, and `++k > j`.
  - Still open:
    - card_menu: 26 lines. card_off_on_texts is not hoisted out of the
      outer loop; loop.c's threshold puts the game's outer loop at 120
      insns or fewer.
    - game_assignments: the game keeps the count 17 in a register across
      two calls, where cse1 propagates the constant. The two texts that
      match add a dead branch or a one-pass loop only to split the block,
      so they were declined. 0x0C09100A and 0x0C09100B, stored to
      user_back_data +0x32b7 and +0x11, still need names.

## 2026-11-04: round NN, the unnamed aggregates and the HUD's globals get names

- **207 of the 239 unnamed aggregate objects** (`agg_<type>_<addr>`) are
  named, each with a typedef in ghidra/types/vf4ft_types.h and a data
  row (inferred, with its evidence) in its library's tsv. Eight agents
  read each object's users (build/structs/agg_scan.json lists them) and
  proposed the object, type and member names. build/structs/agg_name.py
  (private scratch) applied them:
  - It builds each typedef from the exported `agg_` layout member for
    member, so only names and comments change and the layout stays the
    one the matched code compiles against.
  - It rewrites source/ with tools/rename_members.py.
  - It checks every name against the rows, the types and the SDK list.
- **153 of console.c's 156 plain `DAT_` globals** (the HUD block) get
  rows. Each takes the type its externs pin, and the name is replaced in
  source/. The block is the file's statics in an unusual order, not a
  struct. Mostly the HUD's positions, offsets, alphas, digit sprite
  tables and format strings: oldgauge_timer_digits, rob_status_mark_x,
  card_rank_digits_x, mode_sel_box_alpha_alt, format_03d,
  quest_start_text and so on.
- **gfx_state_rec**: 111 more offset members named (326 uses through the
  object, 240 through `(gfx_state_rec *)&gfx_state` casts that
  rename_members.py does not see).
- **tools/rename_members.py** failed on a spec that renames only the type
  and object. An empty member list made the member pattern match any
  `object.field` with an empty group, which raised KeyError ''. Because
  it writes file by file, four objects ended up part-renamed. The member
  passes now run only when there are members, and re-running the four
  specs finished them.
- **The first pipeline run did not compile.** match.py stops at the
  first file that fails, so build/structs/nn/compile_all.py (private
  scratch) now compiles every file to list them all. Two causes:
  - **Member uses in forms the rename tool does not follow.** These are
    `(&obj)[i].field`, `(&obj)->field`, `((type *)x)->field`, `p[i].field`
    for a `type *p`, and the macro aliases of gfx_state in
    bg_plane_loaders.c and hw_defaults.c. 56 such uses were renamed,
    across list_buffers.c, light.c, control.c, camera.c, pvr_irq.c and
    the two alias files.
  - **Two new struct rows overlapped older rows inside them.** Ghidra
    then left the address undefined and exported
    `extern undefined graphics_work` (and the same for
    replay_cam_state). The older rows are now members, and their rows
    are gone:
    - pal_entry_format (+1) and the shadow-changed byte (+0) of
      graphics_work. palette.c and the hand-written 0c12c90c.s use
      `graphics_work.pal_entry_format` / `_graphics_work+1`.
    - DAT_0c41f060, the replay_cam_history ring index (+4), now
      replay_cam_state.history_pos, next to last_eye and last_height.
      Written as members, though, they unmatched replay_cam_reset and
      replay_cam_eye_pick. The game loads +4..+6 through pool words of
      their own, while it reaches the rest of the struct through its
      base, and gcc folds members into the base. So those two functions
      keep the loose DAT_0c41f060-62 externs, tagged ORIGINAL SHAPE.
      cam_mode_replay matches with `&replay_cam_state.history_pos`. The
      three bytes may really be separate globals in the original. If so,
      the record is two objects around them, and that is still open.
- **18 stale `DAT_` externs**: matched code reached the start of a newly
  named object through a DAT_ name. Each now uses the object's first
  member where the types agree (card_units.unit0_flags,
  board_settings.country, poly_header_or_words.control_or, and so on),
  and a cast of the object's address where they do not (net_rare_syuchu,
  the EEPROM save headers, vertex_buffer_desc).
- **Left for later, from the agents' notes:**
  - Some aggregates are slices of objects that already have names, so
    they should be merged into those rows rather than given new ones:
    - water_state +0x1c (agg_0c41347c)
    - three slices inside gfx_state (agg_0c41e20c, agg_0c41e2cc,
      agg_0c41e53c)
    - side_entry_flags, user_back_data, rep_highlights, sel_cursor,
      sel_screen, sel_card_event_done, system_param_value, gz_stream
    - heap_texdata (agg_0c3600d0)
    - itmtst_stats +0x17c, sound_bank_files +0xbc, aet_draw_state +0x7c
    - net_game_config's slices
    - agg_0c29fd1c, which belongs in guard_motion_tables, so that row
      needs widening.
  - Objects that are only ever written are not named, and neither are
    agg_0c375ae4 and agg_0c3d77c4.
  - intrude_timer (DAT_0c292554) is declared int in mode.c and
    undefined4 in console.c.
  - The code reaches past the end of three exported layouts:
    legacy_io_inputs, mie_request and lights_elan_modes.
  - pvr_render_pass_start's row has its arguments in the wrong order.

## 2026-11-04: gfx_state_rec's split arrays folded

- **16 arrays** replace the runs of word and filler members the analysis
  split them into:
  - area_offsets[2] (gfx_ta_areas: region, objlist, isp, tsp) and
    area_ends[2]
  - bg_plane[62]
  - field_read_offsets[2]
  - hooks[9] (gfx_hook: fn, arg)
  - fb_addrs[2]
  - slot_states[4], slot_list_start[5][4], slot_list_bytes[5][4] and
    slot_split_words[4]
  - ta_bufs[2] (gfx_ta_buf, 0x38 bytes per TA buffer: base,
    region_base, the list bases and limits, split_word and the list ends)
  - opb_codes[5], list_limits[5], info_words[4], param_used[2] and
    list_high_water[5]

  build/structs/nn/fold_arrays.py (private scratch) does the fold. The
  elements keep the word types the members had: undefined4 throughout,
  except field_read_offsets, which is int because texture_surfaces.c
  shifts +0x21c arithmetically. A constant-index use is therefore the
  same access as before. 115 uses were respelled where the compiler
  reported the old member names.
- **One respelling changed meaning.** render_stats_read stepped from
  `&gfx_state.field_0x37c` by `i * 0x10`, and that member was a filler
  byte, so the step was in bytes. As an element address the step is in
  words. The game also uses separate pool words for +0x37c and +0x380
  rather than the struct base, so the match needs
  `(&gfx_state.slot_list_start[0][0])[i * 4]` (tagged ORIGINAL SHAPE).
  `slot_list_start[i][0]` does not match. The address-arithmetic
  respellings elsewhere cast to `int *` / `uint *` first, so their step
  was already in words.
- **143 of 240 views of &gfx_state** now name the arrays. These are
  casts to a padded struct or a local view type (rgn_agg, agg_view,
  fb_lay, ringstate and so on). build/structs/nn/view_fold.py (private
  scratch) recognizes them with the agents' patterns:
  - It rewrites only views based on &gfx_state itself. A view based on
    another pointer keeps its address arithmetic.
  - A volatile view stays volatile through
    `((volatile gfx_state_rec *)&gfx_state)->`. The agents' texts
    dropped volatile on 355 of the 608 uses, so they were not applied
    as written.
  - It matches each file after the rewrite and puts back the original
    text of any function that stops matching (fb_pair_install,
    ta_lists_point_at_buffer).
  - Four files keep their views:
    - list_buffers.c and pvr_irq.c call the hook through a view that
      types it as a function pointer, while gfx_hook.fn is undefined4.
    - chip_access.c passes a view pointer where an integer is expected.
    - bg_plane_loaders.c still differs in two functions after the
      revert.
  - About 320 casts of &gfx_state to view types remain across source/,
    not all of them over the arrays.
- check_externs.py now reports 34 differences. list_capture.c and
  scene_render.c reach slot_list_start and slot_list_bytes through
  `DAT_0c41e38c` / `DAT_0c41e3dc` externs, which the export now types
  as `undefined4[5][4]`.

## 2026-11-04: struct cleanup, the placeholder rows take their aggregates

- **Short layouts finished:**
  - light_elan_mode_rec gains its fourth word. The row is now
    `light_elan_mode_rec lights_elan_modes[16]`, and elan_lights.c
    names mode_a/b/c instead of casting.
  - legacy_io_inputs is `bytes[16]`.
  - mie_request is a subcommand byte plus `args[31]`.
- **pvr_render_pass_start takes `fb_write_rec *`.** Its callers pass
  `&fb_write_block`, and scene_render.c names write_ctrl, line_stride,
  write_start, clip_x and clip_y.
- **gfx_hook.fn is a function pointer** (`void (*fn)(undefined4)`), so
  hooks.c, list_buffers.c, lib_state.c, scene_render.c and tex_render.c
  call it without a cast.
- **Alias slices.** Two merge and match: `sound_bank_files[48]` in
  sound.c and `itmtst_stats.log` in rob_disp.c.
  - The gfx_state slices at +0x1fc, +0x2bc and +0x52c stay separate
    objects. The game loads them through pool words of their own (for
    example 0x0C41E20C, then displacement +0x28).
  - Folded into gfx_state, they unmatch fb_surfaces.c,
    runtime_settings.c and pvr_irq.c.
- **Views of &gfx_state.** build/structs/nn2/views2.py (private scratch)
  translates the remaining inline and local-typedef views into
  gfx_state_rec member paths. It keeps volatile and reverts per function.
  - It rewrote 35 more views: display.c 8, pvr_irq.c 13, sync_timing.c
    4, hooks.c 4, pvr_settings.c 3, elan_setup.c 2 and list_buffers.c 1.
  - 145 casts remain, and the match needs them. A view gives GCC a base
    register plus displacement that member paths don't. For example,
    spg_timing_load reaches `r4+64`, and the member path loses it.
- **Ten placeholder rows take the aggregate at their address.** These
  were rows typed `undefined1` / `undefined4` while Ghidra's agg_ object
  carried the uses. The row now has the real type, the agg_ uses are
  respelled through the row's name, and every touched file still
  matches (build/structs/nn2/merge_rows.py, rep_fold.py and tails.py,
  private scratch):
  - side_entry_flags `char[3]`. DAT_0c370c86 is its third byte, which
    sel_stage_init sets and the stage cursor clears.
  - sel_cursor `char[2]`, sel_card_event_done `int[2]`, and gz_stream
    as zlib's own `z_stream`.
  - user_back_data_rec, the 0x18-byte settings head of the backup image:
    format_stamp, no_continue, win_streak_limit and the debug-menu
    names. The rankings rows still start at +0x18.
  - sel_screen_rec, 36 members over 0x138 bytes. DAT_0c41fd6e is
    chara_scenes_unloaded, and DAT_0c41fd80 is fade_alpha, past the
    0x130 that the analysis laid out.
  - static_heap_rec `{addr, size, label}` for heap_texdata. About 20
    rows in libbase.tsv already say "laid out as static_heap_rec" and
    can take it.
  - system_param_value_rec, the 0x34-byte head of the 0x58-byte record.
  - water_state_rec, the head of the 0x24e0-byte water patch.
  - rep_highlight_rec rep_highlights[11], 16 bytes each, with the
    layout rep_highlight_store's local ent struct already used. The clear,
    free and alloc walks and the ending_rep_* reads name the members.
- **guard_motion_tables is 86 pointers, not 8.** hit_reaction_move_pick
  indexes it below 0x56, and main.bin holds 86 there. The agg at +0x40
  was GCC's SH address split for entry 27: +0x6c becomes base +0x40
  plus displacement 0x2c. `guard_motion_tables[27]` matches.
- **aet_draw_state's tail stays its own object**, named aet_draw_tail
  (+0x7c, the last 0x3c bytes).
  - Folded into aet_draw_state, it unmatches aet_objs_draw,
    aet_last_alpha_put and aet_image_layer_draw.
  - The split of +0xb0 would be base +0x80 plus 0x30, but the game
    loads +0x7c, so the original reached it through an address of its
    own.
- **Named:**
  - card_net_settings (decided_no_card, decided_one_card,
    decided_more_cards).
  - motion_test, the debug menu's motion test.
  - jvs_board_status (boards_in_use, boards_present).
  - intrude_timer, the challenger countdown. It is int, because
    intrude_disp tests it signed.
  - rob_debug_value, the number drawn at rob_debug_value_x/_y.
- **13 aggregates stay unnamed.** Each is only zeroed, by one function
  whose literal pool is its only reference: rob_coli_task_start (7 float
  pairs), graphics_bring_up (2), demo_start (1) and
  vars_0c41f310_clear (2, plus DAT_0c28edac).
- **Open layout questions:**
  - game_stats.c writes card_net_settings +0x40 / +0x44, which is
    card_close_settings +0 / +4. Those are a count and a frame sum, so
    either card_close_settings starts 8 bytes early or they are a
    separate stats pair.
  - motion_test is really 0x64 bytes: a 0xC header and two 0x2C side
    entries. Its layout covers 0x24, and the debug-menu globals mot_num
    (+0x14) and ctrl_coma (+0x34) have no rows.

## 2026-11-04: _unmapped/ is empty, legacy_io_start matches, card_net_settings is 0x80 bytes

- **card_syu_not_both_2 matches in card.c** (144 of 144), and
  source/_unmapped/ is gone. The extu.b at the argument came from the
  old `uint` loop index; with a `byte` index the game's code falls out.
  The int result held in `bits` and the flag in `both` carry an ORIGINAL
  SHAPE comment: testing the call directly, `n += bits == 2` and
  `return n != 2` each compile differently.
- **static_heap_rec for the static heap table** (build/structs/nn2/heap_fold.py,
  private scratch). 23 rows take `{addr, size, label}`, five as arrays:
  rob_load_heaps[2][4], rob_obj_ofs[3], objmem_rob_hand[3],
  newopt_obj_ofs[2] and objmem_heap_chr[3]. The four look-alike types
  (field_load_rec, nick_comm_buf_rec, nick_game_buf_rec, spr_cmn_top_rec)
  are gone. The uses read `.addr` / `.label` and `[i]` instead of
  `+ i * 0xc` casts; rob_ctrl.c keeps its flat walk of the 8 records.
  13 files still match.
  - The committed header lacked static_heap_rec's `label`: tails.py's
    skip check matched `char *label` in another struct. Added.
- **motion_test is 0x64 bytes**: a 0xC header (flags, the saved
  debug switches and camera mode, the play mode, the frame step) and
  two 0x2C `motion_test_side` entries. The debug menu's mot_num and
  ctrl_coma are side 0's motion and frame. motion_test_restart walks the
  sides through `motion_test_side *e` with the game's `i * 0x2c` byte
  offset (ORIGINAL SHAPE: `&motion_test.side[i]` compiles differently).
- **card_net_settings is one 0x80-byte record** (0x0C375AE4 up to
  card_log_session). The evidence: play_time_account loads the pair at
  0x0C375B24 with `mov.l @(0,r3)`. GCC writes an explicit zero
  displacement only for a split base (here card_net_settings +0x40, low
  part 0); for an object's first word it writes `@r3`, and the
  card_close_settings form gave exactly that (7 of 8). So +0x40 / +0x44
  are streak_challenge_count and streak_challenge_frames, and the
  closing schedule follows as the member `close` at +0x48
  (card_close_settings_rec, now 0x38 bytes). The row at 0x0C375B24 is
  gone; network.c reads `card_net_settings.close.*` and game_stats.c
  names the pair. Both files match.
- **The record tails** (build/structs/nn2/tail_fold.py):
  - system_param_value_rec to 0x58: main_board_id[17] at +0x34 and
    dimm_board_id[17] at +0x45 (the "MAIN BOARD ID %s" / "DIMM BOARD ID
    %s" serials), so their two rows go; system_common.c also names
    `&system_param_value.media_kind` for DAT_0c360214.
  - water_state_rec to 0x24e0: the second color, the look values at
    +0x24, the real and imaginary grids (17 x 17 floats at +0x44 and
    +0x4c8), the 64 wave records at +0x94c (the water_waves row goes)
    and the 20-byte cells at +0xe4c.
  - user_back_data's rest stays as separate rows: misc_play_stats has a
    pool word of its own, and as a member at +0xB7C it would be reached
    through the split base +0xB40.
- **gfx_state_rec's unnamed members**: 184 remain, and only 7 are used
  anywhere; all 7 are only written. Each now has an evidence comment
  naming its writers (+0x1ec, +0x27c, +0x2a4, +0x2b4, +0x334, +0x338,
  +0x364). The other 177 are never touched in source/.
- **legacy_io_start matches** (round CC, three agents on the six
  smallest gaps, natural shapes only; build/agents/wCC/, notes in
  rtl-leads.md). The fix was the header, not the source:
  mie_ident_text is `char[128]`, not a scalar `undefined1`.
  - The game's `exts.b` after the copied byte comes from reload_cse.
    It may turn the second read of buf[i] into a sign-extend only when
    the store provably cannot alias buf. GCC's alias check proves that
    only when both sides are declared objects, and the
    `((char *)&mie_ident_text)[i]` cast hid the declaration.
  - jamma.c is 16 of 16; legacy_io.c reads `s = mie_ident_text;` and
    still matches.
  - The 16 unused frame bytes below buf stay a guessed `int pad[4]`
    (ORIGINAL SHAPE comment).
  - The general lesson: a missing exts.b, or a re-load where the game
    reuses a value, can mean a global typed as a scalar where the
    original has an array.
- **The other five stay open**, each now with its cause pinned to a
  compiler pass (rtl-leads.md, round CC):
  - elan_lights_queue (2): cse1 ties dl.size's zero to i's register.
  - aet_still_quad_retransform (2): local-alloc ties the param copy.
  - sprite_clip_push (3): combine folds the game's exts.b into the
    load. Its text is cleaner now.
  - attack_connect (4): a local-alloc priority tie over r0.
  - win_item_row_draw (4): the game has a CSE block end between two
    reads.

  No natural text closes any of them, and no new trick texts were
  written.
- **16 main.bin functions are open.**

## 2026-11-04: player_messages and the texture-memory accounting get their real types

- **player_messages is `player_message[2]`**, two 0x10C-byte records:
  - set flag, selection, four 61-byte texts, width, four heights.
  - The selection rows (player_message_sel_1p / _2p) become
    `&player_messages[side].sel`.
  - judge_mes_draw and player_message_text_set name the members instead
    of casting.
  - console.c still matches. GCC folds `[i].heights[j]` into the same
    `(i * 0x43 + j) * 4` the cast text spelled out.
  - player_message_draw's text is now natural too. With the type
    alone its gap stayed at 12 lines, a register tie (r5/r3 against
    r1/r2); round DD below matches it.
- **sprite_set_vram_used is `int[81]`.**
  - The `sprite_set_vram_tail` object at +0x13c was GCC's SH split of
    entry 0x50 (`((0x140 + 4) & ~60) - 4` = +0x13c, then displacement 4).
  - `sprite_set_vram_used[0x50]` in the emblem loaders gives exactly
    the game's code, so the tail row and type are gone.
  - console.c and sprite.c index the array directly.
- **Five texture-memory words get names.** vram_used_total_update
  loads each through a pool word of its own, so they are separate
  globals, not tex_vram_free's tail:
  - common_obj_vram_used (KB, load_obj.c)
  - stage_obj_vram_used and stage_ef_obj_vram_used (stage.c)
  - sprite_vram_used_total
  - vram_used_total

  load_obj.c and stage.c use the names.
- **vram_used_total_update drops from 36 changed lines to 12** with
  those globals.
  - Summing straight into sprite_vram_used_total, as the decompiler
    shows it, gives the game's tail: loop.c keeps the global in a
    register, which leaves the copy `mov r3,r1`.
  - It still emits the initial zero store, which the game lacks, and
    the loop registers are shifted by one (r6/r7 against r5/r6).
- **The other cast-indexed globals in the open texts** are offsets into
  neighboring objects or translation leftovers, not wrong types:
  - stage_screens +148/+404/+660 are the water tables after it;
  - msn_text_state_ptr's are the objects after it;
  - prize_*, hit_light_flags, elan_cmd_write_ptr and break_floor_model
    are used as declared.
- **gfx_state_rec's unnamed members, read in Flycast.**
  - build/structs/nn2/gfx_state_dump.py and gfx_state_words.py (private
    scratch) read gfx_state from the three save states.
  - offsets.py takes member offsets from the cross compiler itself.
  - gfx_writers.py finds the game functions that store through a pool
    word inside gfx_state.
- **Most of the "177 never-used members" were padding of members typed
  too narrow.** 18 named members were one byte followed by three
  unnamed bytes, yet hold word values in every state:
  - 0x00180000 for the ISP and TSP parameter sizes;
  - 0x001cf800 for the parameter area;
  - SPG register images such as 0x00150208.

  No source/ code reaches them by member name, only through views, so
  they are now `undefined4` (gfx_widen.py). lib_state.c then stores
  `g->isp_param_size = isp` without its casts.
- **The second pass** (gfx_widen2.py):
  - +0x14 and +0x1c are words lib_state_set_up_from_cfg stores (0x10000
    and the 8 MB default). Nothing reads them, so they keep their
    field names.
  - +0x30c is `core_reg98_value`: 0, or 0x800408 on revision-6 chips,
    which pvr_regs_defaults_and_vram_clear writes to core register 0x98.
  - +0x45c / +0x460 get evidence comments (0x600000 and its copy, from
    ta_memory_defaults_set).
- **ta_memory_defaults_set names its members.** Its
  `p = (char *)&gfx_state + 0x43c` was GCC's split base for +0x454, so
  plain `gfx_state.ta_mem_size` gives the same code.
- **views2.py, rerun, turned 18 more views into member paths**:
  ta_regions.c 16, among them tile_cols, tile_limit_x/y, double_regions
  and modvol_merge; scene_render.c 2, fb_r_sof2.
  - Its per-function revert left bg_plane_loaders.c at 21 of 24. The
    file was restored from the backup and matches; the views stay there.
- **123 unnamed bytes remain**, all byte runs that are zero in all three
  states, with no store or load to them found in the game's code.
- **Round DD: four open functions match, all natural** (five agents;
  texts in build/agents/wDD/, notes in build/agents/rtl-leads.md):
  - **player_message_draw (0c041980), console.c.** The local `k` was
    the decompiler's. Indexing `player_messages[i].sel` directly in
    the test, `texts[...]` and `heights[...]` gives the `&sel` address
    a longer life, so it gets the game's r5, and postreload folds the
    re-reads into the loaded register.
  - **render_config_from_list_sizes (0c101000), naomi2_library.c.** It
    is written like its matched sibling ta_list_sizes_apply (NAOMI
    library system.c): the same view of the configuration block, and
    the five shares computed in both arms of the
    `render_frames_in_flight == 3` test. That NAOMI2 library range is
    now complete. test.bin's copy (0c039560) is built from the same
    source.
  - **card_menu (0c0251e0, test.bin), testmode.c.** It reads the menu
    header through `int *p = (int *)card_menu_state;`, as its matched
    sibling card_menu_input does, and reaches the unit record as
    card_test_units_step does. Reload re-loads p from its constant at
    each use, which moves reload's spill-register rotation onto the
    game's registers.
- **Still open after round DD**, each traced to a pass:
  - prize_fly_draw 130 lines: global-alloc fr13/fr15, plus a tie in
    case 4.
  - render_hang_recover 78: loop.c's movable threshold.
  - break_floor_model_build 10: a global-alloc priority tie. The error
    trap's `for (;;) ;` takes the top block frequency, so both hoisted
    constants round to priority 0 and pseudo order decides.
  - move_tcmd_33_slot_effect 8: reload's round-robin choice of the
    shift-count registers.
  - fb_layout_compute 8: the delay slot, unchanged since round BB.
  - vram_used_total_update 12, or 8 with a static inline sum helper
    (held: no sibling in sprite.c uses one).
  - 0c147480 9: combine merging pointer adds.
  - 0c0adf20 3: a cse1 label at the done store. Its match through a
    dead side argument is declined.
  - game_assignments (test.bin) 46: the count 17 in r8. It still
    matches only through the declined dead branch.
    - An agent proposes names for two of its bytes: challenge_only at
      0x0c09100a, stored to the user_back_data offset main.bin's
      CHALLENGE ONLY setting uses, and button_type at 0x0c09100b. They
      are not applied, since no data rows exist yet
      (wDD/inc_0c020cc0/).

- **game_assignments (test.bin 0c020cc0) is adopted, by the maintainer's
  decision**, with an ORIGINAL SHAPE comment:
  - The game keeps the item count 17 in r8 across both menu calls.
    With a plain `n = 17` the compiler writes 17 into each call
    instead.
  - The matching text chooses the count with
    `if (net_log_unlocked == 1) n = 17;`. That is how the matched
    network_test in the same file chooses its own count (2, or 4 when
    unlocked).
  - Read that way, the original had two counts that are both 17 in
    this build. Nothing confirms that, so the comment says so.
  - Its two unnamed bytes get rows: challenge_only (0x0c09100a, stored
    to the user_back_data offset of main.bin's CHALLENGE ONLY
    setting) and button_type (0x0c09100b, user_back_data+0x11).
- **Kept as they are**: render_done_irq stays held (its match needs a
  second pointer to gfx_state that no matched code shows).
  vram_used_total_update's inline sum helper stays on file as a
  starting text only, since neither text matches.

## 2026-11-04: gfx_state is volatile; the configuration block's list shares

- **render_config_block's five list shares are `float list_shares[5]`**
  (+0x2c).
  - The local `struct snd_mix` views in ta_list_sizes_apply and
    render_config_from_list_sizes are gone. Their "total" was
    vertex_area_size and their "mode" bypass_list. The defaults set-up
    stores all five shares by index.
  - Both functions keep a local pointer to the block, now typed
    `render_config_block *s = &render_config;`. Without the pointer
    neither matches.
- **gfx_state was declared volatile in the original.** Two experiments
  on the 25 files that use it:
  - **Volatile as a whole object**, with the code unchanged: 434 of 437
    functions still match.
  - **No volatile at all**, with the casts stripped: 66 functions stop
    matching.

  So the volatile is real, and it belongs on the whole object. The
  decompiler had rebuilt it one access at a time as casts on RAM.
  - New file ghidra/volatile_globals.tsv (name and evidence).
    ExportSource now writes those globals `volatile`, and every file's
    own extern says so too.
  - The three functions that broke read a value the original evidently
    kept or reached another way. Each now has a natural form:
    - fb_w_ctrl_alpha_cutoff builds the new word in a local, stores it
      and passes it.
    - lists_send_to_ta reads frame_idx once.
    - render_stats_read takes the slot tables as
      `(int *)&gfx_state.slot_list_start[..]`, as its matched siblings
      frame_lists_send_and_rearm, scene_lists_close and
      ring_slot_lists_record do. The game's indexed loads show those
      reads were not volatile.
- **The casts that only re-added the qualifier are gone:** 566
  `((volatile gfx_state_rec *)&gfx_state)->` and
  `(*(volatile gfx_state_rec *)&gfx_state).` became `gfx_state.`.
- **views3.py turned 101 of the remaining 114 views into member paths.**
  - A view of the same size now becomes a plain member path.
  - A cast through a member's address does not work as a replacement.
    It loads the address as one constant (`@rN`), where the game keeps
    GCC's split base (`@(0,rN)`).
  - Four background-plane loaders now write
    `gfx_state.bg_plane[n++] = x`, as the matched bg_plane_append_*
    helpers do. The nine that start with fixed-offset stores keep their
    `o = 0xa4` shape: neither plain indexing nor a pointer to bg_plane
    matches them.
- **A lead from short_line_timing (+0x528):** the game reads it through
  a pool word of its own (`@rN`), never off a split gfx_state base.
  - By the rule that a word reached through its own pool word is its
    own object, it was probably a separate global in the original. Its
    18 reads keep their cast.
  - The 13 views that stayed and the 14 typed sub-views (sub52c and
    others in pvr_irq.c and runtime_settings.c) may show the same, a
    state block made of several objects.

## 2026-11-04: gfx_state ends at +0x514; three objects follow it

- **Two scans settle where gfx_state ends.**
  - build/structs/nn2/gfx_bases.py lists every pool word the game loads
    inside the old 0x57C bytes, with the displacements used from it.
  - gfx_reach.py follows the adds of constants from each pool word to
    the offsets it reaches.
- **gfx_state's own pool word reaches +0x000 to +0x510 and nothing
  past it.** That covers GCC's split bases, which run to +0x4fc. Each
  of the three ranges after it is reached only through a pool word of
  its own:
  - +0x514 to +0x528: five words, all set by lib_state_set_up_from_cfg.
  - +0x528: one word, read at offset 0 only.
  - +0x52c to +0x57c: reached through +0x574, including the +0x56c base
    inside it.
- **They are now separate globals:**
  - lib_info (0x0C41E524, lib_info_rec: info_words[4], credits_text);
  - short_line_timing (0x0C41E538);
  - render_stats (0x0C41E53C, render_stats_rec: the stall, overflow and
    time counters).

  gfx_state_rec is 0x514 bytes. lib_state_set_up_from_cfg clears
  gfx_state with memset(..., 0, 0x514), exactly that size, so it is now
  written `sizeof(gfx_state_rec)`.
- **Which of them are volatile was tested, not assumed:**
  - short_line_timing is volatile: spg_timing_ntsc re-reads it for each
    value and stops matching without the qualifier.
  - render_stats is not volatile. Two of its functions stop matching
    with the qualifier, and its accesses carry their own: volatile
    pointers where the interrupt code needs a re-read, plain elsewhere.
  - lib_info matches either way, so it stays plain.
- **The casts and views that the overlap needed are gone:**
  - short_line_timing's 18 casts in sync_timing.c, and its three
    DAT_0c41e538 users, now read `short_line_timing`;
  - pvr_irq.c's separate agg_0c41e53c_0c41e53c is gone;
  - the sub52c/sub56c and inline views in runtime_settings.c, pvr_irq.c
    and tex_passes.c are now render_stats members;
  - lib_state.c fills the info record through a `lib_info_rec *`.
- The extent row for gfx_state (aggregate_extents.tsv) now says 0x514.
  Its old 0x580 came from the split bases, and the last of those,
  0c41e58c, is bank_build_index's own address.
- The +0x040 pool word is not a separate object: gfx_state's own base
  reaches +0x44 to +0x78 as well. It is a sub-block whose address some
  functions take.


## 2026-11-04: user_back_data is the whole 0x32d4-byte backup image

This reverses the 2026-11-03 decision that user_back_data's rest "stays
as separate rows".

- **The rows inside the image were GCC split bases, not records.**
  - Nine of them start where GCC's split base sits, at an offset with
    (offset + 4) % 64 == 0: play_stats (+0xb3c), misc_play_stats
    (+0xb7c), game_phase_stats (+0x13bc), play_length_stats (+0x14fc),
    card_net_settings (+0x15fc), card_close_settings (+0x163c),
    card_log_session (+0x167c), user_back_net (+0x1f3c) and
    ledger_day_state (+0x237c).
  - Each of their types opened with filler before its first member.
  - The rows that are real array starts don't fit the pattern: the
    rankings at +0x18 and +0x590, the ledger records at +0x1f48 and the
    course ranking at +0x2ca4.
  - The old reason, that misc_play_stats "has a pool word of its own",
    doesn't hold. That pool word is user_back_data's split base for
    +0xb84 .. +0xba4. select.c, mode.c and game_stats.c match 193 of 193
    with misc_play_stats written as a member at +0xb7c.
  - The image is also copied as one block (user_back_reset) and saved as
    one (user_back_init / user_back_flush).
  - build/structs/nn2/obj_reach.py is gfx_reach.py for any object
    (RB=base, RS=size). user_back_data's own pool word reaches +0x23c2.
- **user_back_data_rec is now 0x32d4 bytes**
  (build/structs/nn2/ub_flatten.py, ub_close.py, ub_tail.py):
  - The split-base records are flat members at their real offsets. A
    few were renamed so the name says what they are at image level:
    card_log_stamp, ledger_day, ledger_record, net_log_slot_next and
    net_timeout.
  - The regions whose address the code takes are members of their own
    type:
    - time_ranking_rows and point_ranking_rows, 50 rows of 28 bytes;
    - card_item_log and card_data_log;
    - ledger_daily_records, 30 days of nine words;
    - item_pending_table;
    - course_ranking, 8 courses of 0xa8 bytes. Ranking page 3 shows 32
      rows in groups of four.
  - card_close_settings_rec is gone as well. Its filler held the 10
    discarded card codes and their next slot (+0x164c, the mode.c and
    card.c views), and the weekday tables after its by_weekday flag
    (+0x167c, +0x1698, the network.c view) lay past its end. They are
    now discarded_codes[10], discarded_code_next, close_hour,
    close_minute, close_by_weekday, close_weekday_hour[7] and
    close_weekday_minute[7].
  - The image ends with alab_play_starts, alab_play_continues and
    alab_play_ends (two ints each, +0x32bc .. +0x32d4). alab_play_start's
    pointer sum is now `user_back_data.alab_play_starts[i]++`.
  - The views at +0x1f3c in network.c are net_log_slot_next.
- **The rows inside the image are gone**, and their descriptions are now
  the members' comments. user_back_data has an extent row (0x32d4).
- One function needed a change: card_log_clear writes the pending
  table's three words through the table pointer it already holds. That
  pointer is the base the game adds to, the way matched
  item_pending_sort's caller in card_item.c does with `g->`.
- **This pipeline run is a full `ghidra/run.sh all`.** The partial run
  (types, prototypes, export) keeps the labels of rows that have been
  removed. That is why card_close_settings, removed on 2026-11-03, still
  had an extern in vf4ft_globals.h.
  - The fresh import also dropped two old aggregate types that only the
    old database still had: agg_0c41e2cc (gfx_state +0x2bc) and
    agg_0c41e20c (+0x1fc). Both are split bases again, and fb_surfaces.c
    and runtime_settings.c declared them. Their fields are
    read_ctrl_word, vertex_timeout and render_timeout, and both files
    match with those member paths.
  - The image also holds labels that have no row: the debug-menu
    settings at +0x2388 .. +0x238d (win_bg_trans, win_bg_color,
    mission_off_on, mission_rate, replay_message, mission_rate_test) and
    at +0x32b4 .. +0x32ba (net_ver .. effect_skip), and the DAT_
    halfwords at +0x238e .. +0x23c0. Inside a filler array they lost
    their types. win_bg_trans was at the start of one and took its type,
    undefined1[180], which broke win_background_draw. They are now typed
    members (build/structs/nn2/ub_labels.py, with their old header
    types), and no label sits at a filler's start.
- **gfx_state_rec's 112 filler bytes are 10 `unused_*` arrays.**
  `gfx_reach.py 0 514` lists no access that lands in any of them, and
  the member before each is a plain word, not an array.


## 2026-11-04: the split-base sweep; the open functions' texts recompiled

- **build/structs/split_rows.py looks for rows like those in
  user_back_data across the whole game.** It flags a row when two things
  hold:
  - every register loaded with the row's exact address is used only for
    `@(d,rN)` accesses: never `@rN`, never indexed, copied, added to or
    passed on;
  - an earlier row lies a GCC split-base distance before it,
    (distance + 4) % 64 == 0.

  Of the 1998 rows the code loads by address, 6 are used only through
  displacements, and 4 of those also have the distance. Three are split
  bases; their own row text already said whose tail they are:
  - frame_stats_times = frame_stats +0x7c. Its two words are the
    record's +0xa8 / +0xac, so frame_stats_rec now runs to +0xb0 with
    vertex_input_ticks and drawing_ticks.
  - net_nushi_value = net_game_config +0x57c and net_masa_ryuko_values =
    +0x5bc. net_game_config was an untyped label and is now
    net_game_config_rec, 0x5e8 bytes. It has rare_syuchu (+0x40, a real
    member start: net_rare_value_get indexes it), kumite_values[75]
    (+0x47c, network.c's view), nushi_on, masa_on, ryuko_on and
    struggle_revise (+0x5b8, +0x5cc, +0x5e0, +0x5e4).
    network.c's byte-pointer uses of `&net_game_config` are now
    `(undefined *)&net_game_config`.
  - The fourth, line3d_alpha_header, is a 32-byte polygon header whose
    distances from its neighbors are a coincidence.
  - Its own pool words reach no further than +0x5e0. The view at
    +0x8a8 + i * 0x200 (net_pr_texts_clear) is the text slots after
    net_message_text (+0x6a8), not part of the config.

  The four rows inside these records are gone (build/structs/nn2/
  split_fold.py). system.c matches 62 of 62 and network.c 168 of 168.
  The pipeline ran as `ghidra/run.sh all` again.
- **The open functions' stored texts don't match the new headers.**
  build/agents/rerun_texts.py recompiles every candidate the agent
  rounds left (202 texts for the 14 functions), appended to a copy of
  the function's file.
  - Only 7 of the 14 newest texts still compile. The others use names
    that changed since, such as agg_0c3744e8 (user_back_data),
    agg_0c41e010 (gfx_state) and DAT_ rows that have been named. The
    older rounds' texts fail the same way. build/agents/rerun_errors.py
    lists each newest text's stale names.
  - Of the 7, none matches. elan_lights_queue still differs by the 2
    lines rtl-leads.md explains.
  - Testing a text again after a rename needs the text carried over to
    the current names first. That is a by-address rename step, which
    doesn't exist yet.

## 2026-11-04: user_back_data's tables and counters named

- **The views over user_back_data are now members of the image.** Each
  rewrite was kept only if its file still matched.
  - build/structs/nn2/ub_tables.py adds the stats tables, by their
    readers and writers:
    - best_win_streak[16] (+0xb08);
    - chara_best_24e / _250 / _24a / _24c, four short[16] tables at
      +0xba8 that game_disp shows;
    - course_results[5][16] and course_results_no_continue[5][16]
      (+0xc28, +0xd68);
    - chara_play_counts[2][16] (+0xea8), stage_select_counts[30]
      (+0xf28) and vs_chara_counts[16][16] (+0xfa0);
    - card_level_counts[16] (+0x1448);
    - stage_vs_rounds_flag40[30] and stage_vs_rounds[30] (+0x1528,
      +0x15a0);
    - mission_offered[30] and mission_cleared[30] (+0x23c4, +0x2400).

    mode.c's `judge_rec` and `rec`, console.c's `big` and the pointer
    views in card.c, mission.c, select.c, card_item.c and game_stats.c
    are gone. clear_stats_add matches only when written like its sibling
    lose_stats_add, through a `user_back_data_rec *p`.
  - build/structs/nn2/ub_ranking.py adds ranking_row_rec, 28 bytes:
    name[21], chara +0x15, value +0x18. time_ranking_rows and
    point_ranking_rows are ranking_row_rec[50]. The insert, slot-find and
    draw functions in user_back.c and console.c index rows instead of
    copying a 28-byte `ent` through a padded struct.
  - build/structs/nn2/ub_counters.py names the 19 labels that were left
    inside the image:
    - +0x238e..+0x2398: card_match_result_record's JISAKU counts, one per
      jisaku_flag bit, then the same for equal levels above 18;
    - +0x239a..+0x23a6: the lottery and bonus-time counts (bonus time won,
      games played in it, shortest and longest drawn, daily syuchu marks);
    - +0x23b8..+0x23c2: the prize matches by why. Kind 3 is never set in
      this build, and kind 0 is the last word. prize_give's items given
      sits among them.
    - chlg_map_hiscores (+0x24c4): the 36 0x38-byte hiscore records of the
      eight-stage challenge map, which end exactly at course_ranking.

    card.c, mission.c and msnm.c use the members.
- **Two kinds of view still don't fit the members:**
  - msnm_disp.c's chlg_map_node_get matches only with its own symbol at
    0x0C3769AC. Spelled as `user_back_data.chlg_map_hiscores + i * 0x38`,
    as user_back_data + 0x24c4, or even as the absolute address, GCC
    schedules the address load before `sts macl`, not after it. Only a
    plain symbol gives the game's order. So the original file probably
    reached the records through a name of their own, and DAT_0c3769ac
    stays there.
  - card.c's level_change_flags_compute and card_mode.c's card_mode_init
    read card_1p_use by side as `&user_back_data + i` and then +0x1618.
    Neither `(&user_back_data.card_1p_use)[i]` nor its char form matches,
    because the side index goes in before the displacement.

## 2026-11-04: the stored agent texts carried over to the current names

- **build/agents/rename_texts.py [--all]** rewrites the open functions'
  stored texts by address and writes them to build/agents/renamed/, where
  rerun_texts.py picks them up.
  - A text has no declarations of its own. It was written against its
    file and the shared src/include headers of the time. So the step reads
    those from the commit just before the text's mtime.
  - Each name that has since gone is resolved by its address, in this
    order:
    - a name the file, vf4ft_globals.h or vf4ft_functions.h declares
      there now (older texts call their own function FUN_<address>);
    - a member path into the current typed row that holds the address;
    - a vf4ft_data.h label.

    Old agg_<address> struct types become the type now at that address,
    and their field_0x names become the members at those offsets. A cast
    keeps the old type wherever the new one differs. One field that
    changes signedness (fb_layout_compute's field_0x218, now
    field_read_offsets[0]) is renamed and flagged.
  - It also adds extern lines for the vf4ft_data.h labels a text uses,
    since source/ files don't include that header. This, not a rename,
    is why stage_extras_setup's text failed.
- **rerun_texts.py's one-function diff** compiled with vf4ft_data.h, so a
  file extern declared there with another type broke the diff. Those
  showed up as DIFF-FAIL. The file's name now gets a private alias.
- **Result:** 393 texts for the 14 open functions.
  - 92 of the 191 renamed texts compile and are measured; only 8 of the
    202 originals do. Every open function except 3 now has a measured
    text.
  - None match. The closest are elan_lights_queue (2 lines, as
    rtl-leads.md explains), vram_used_total_update (8),
    cfg_setting_or_default (19), move_tcmd_33_slot_effect (20) and
    aet_still_quad_retransform (28).
  - Still no compiled text:
    - render_done_irq reaches render_stats as gfx_state +0x52c, past the
      record's new end;
    - sprite_clip_push and win_item_row_draw were written against
      prototypes that have changed since. That is a signature change,
      not a rename, so the step leaves it.

## 2026-11-04: round EE; render_done_irq and fb_layout_compute match

- **Every open function has a measured text again.** Three had none:
  - render_done_irq: a clean start text without the held second pointer,
    with render_stats as its own global, gets 3 lines.
  - sprite_clip_push: the round CC text needs its
    `#define KRPROTO_sprite_clip_push` line, then 3.
  - win_item_row_draw: the round CC text under today's prototype (the old
    int parameters as casts) gets 4. Its recorded size, 0x14e, stops at a
    jump table inside the body; the function runs to 0x6d0.

  build/agents/rerun_texts.py now measures to the next function start when
  ours runs past the recorded size. It also gives the one-function diff the
  file's static inline helpers the text calls.
- **Round EE** (build/agents/ROUND-EE.md, batches 950-954, five agents,
  natural shapes only, priors from the renamed texts): 2 of 14 match.
  - **render_done_irq** (0c148c80) MATCHES with plain `gfx_state.`
    accesses throughout. The else arm is a jump target, so CSE reloads
    &gfx_state there, which is what the held second pointer stood in for.
    The held text is retired.
  - **fb_layout_compute** (0c147b20) MATCHES with plain `gfx_state.` in
    place of the casts. The `__builtin_expect` is no longer needed and the
    `&` of two tests is a plain `&&`. Its two closing stores sit in a
    `do { } while (0);`, as matched elan_lights.c writes its
    list_end_block_count update (twice). Without a block end there, the
    pending pop of fb_reg_images_pack's 116-byte argument (`add #116,r15`)
    is never flushed. Adopted with an ORIGINAL SHAPE comment.

  Both are adopted through build/agents/roundee-adopt/.
  LESSON: since gfx_state became `volatile gfx_state_rec`, texts that mix
  volatile casts with plain views, or hold a second pointer to it, should
  be retried with plain member accesses.
- **Still open** (rtl-leads.md, Round EE):
  - sprite_clip_push matches only with a one-field `struct { char v; }`
    local. That keeps the char unpromoted, so combine doesn't fold the
    game's exts.b. No matched code has that shape, and rounds Y and Z
    declined the same thing, so it stays held.
  - render_hang_recover's k loop is probably a real `for` loop (the k==sel
    arm's fresh constant loads), but loop.c then hoists the send_mask
    arm's constants too.
  - The rest are register ties settled after code generation, with the
    instruction order already right: elan_lights_queue,
    aet_still_quad_retransform, stage_extras_setup, attack_connect,
    win_item_row_draw, move_tcmd_33_slot_effect, vram_used_total_update,
    cfg_setting_or_default, break_floor_model_build, prize_fly_draw.
- **The held texts, reviewed under the "as the original programmers wrote
  it" rule:** none is adopted. Each is a trick the policy declines
  (`long long`, an always-false test that only makes a label, an asm or
  label barrier, the one-field char struct) or a starting text only (the
  vram sum helper). The two texts left in build/agents/held/ belong to
  functions that already match.
- rename_texts.py now also renames `KRPROTO_`/`NOPROTO_` guard names
  (prize_fly_draw's prior had lost its guard: 148 lines, 130 with it).

## 2026-11-04: the gfx_state views become members; sprite_queue_state

- **gfx_state views** (build/agents/ROUND-FF-VIEWS.md, three agents over
  15 files, each rewrite kept only if its file still matched): the
  views in ta_regions.c, pvr_settings.c, bg_plane_loaders.c, pvr_irq.c,
  list_buffers.c, lib_state.c, runtime_settings.c, elan_setup.c,
  display.c, hw_defaults.c, fb_surfaces.c, scene_render.c,
  list_capture.c, tex_passes.c and tex_render.c are now plain
  `gfx_state.` / `render_stats.` members. The pad structs, `char *b`
  bases, `#define g` views, `(struct big *)` casts and `+0x52c` /
  `+0x56c` offsets are gone.
  - The nine bg_plane loaders that kept `o = 0xa4` match as
    `gfx_state.bg_plane[n++] = ...` with `n` from 0, the running-index
    form the file's other loaders already use. The 0xa4 is how GCC lays
    out that address, not a local of the original.
  - **Types, from the views' evidence** (each checked on every user):
    - vo_start_x / vo_start_y1 / vo_start_y2 are `int`: display.c's
      signed compares (cmp/gt, cmp/pl) need it.
    - slot_list_start is `int [5][4]`: list_capture.c's
      `(cursor - *a) >> 2 << 2` is signed. slot_list_bytes stays
      unsigned (its compares are cmp/hi).
    - mv_hdr is `uint *`, the header block lists_send_to_ta patches.
- **Views that stay (the game reads these without volatile).** A
  volatile memory reference is not combined into `@(r0,Rn)` addressing,
  and GCC does not reuse a volatile load. The game does both here:
  - the 48-word bg_plane copies in pvr_irq.c ta_input_finish and
    tex_passes.c list_input_end_with_passes (`mov.l @(r0,r7),r1`);
  - slot_list_start and slot_list_bytes in scene_render.c
    (lists_send_to_ta through the DAT_0c41e38c / DAT_0c41e3dc aliases,
    frame_lists_send_and_rearm, scene_lists_close) and runtime_settings.c
    render_stats_read (already ORIGINAL SHAPE). list_capture.c reads the
    same arrays volatile.

  Whole-struct `volatile` cannot say both. The original probably
  declared the record plainly and reached it through volatile pointers in
  the interrupt code; nothing in the game's code says which.
  scene_lists_close also keeps `q = (volatile int *)((char *)&gfx_state
  + 0x3fc)` for code shape: the game addresses used_lists from that
  anchor (`@(56,r8)`).
- **sprite_queue_state** (0x0C4DD82C, sprite_queue_state_rec: short
  count, char marks[0x300]) replaces sprite_queue_count and the
  `(tab *)&sprite_queue_count` views in sprite.c. The code reaches the
  marks from the count's address (+2 + count), so it is one object.
  sprite_next_blend_normal keeps its local pointer, now typed as the
  record; the bitfield view of one mark stays.
- **sprite_clip_push** stays held. The game re-reads the count after the
  mark store and sign-extends it separately (`mov.b @r5,r6; exts.b
  r6,r2`), so the value lived in an unpromoted 8-bit register. Every
  plain way of reading it does worse:
  - a `char`, `short` or `int` local: 49 lines;
  - the global read directly: 55 lines, because CSE reuses the compare's
    load.

## 2026-11-04: every kamui file rechecked; gfx_state stays volatile as a whole

- **The question:** is gfx_state a volatile record, as now, or a plain
  one that only the interrupt code reaches as volatile? The views kept
  in the previous entry (non-volatile bg_plane and slot_list reads)
  would fit the second.
- **A plain record** (no qualifier on gfx_state, the 25 files' externs
  and the header changed): 101 functions in 22 files stop matching. Most
  are not interrupt code: 20 of the 24 bg_plane loaders, 13 of 20 in
  list_buffers.c, 9 of 13 in scene_render.c, 6 of 8 in sync_timing.c.
- **Volatile per member** (build/structs/memvol/memvol.py; greedy, every
  member volatile to start, then each made plain if no file that names
  it loses a match): 430 of 430 match only with **88 of the 140 members
  volatile**. That set includes values set once at start-up and only
  read afterwards: vram_size, fb_width / fb_height, display_lines,
  tile_cols / tile_rows, ta_mem_size. The 52 that can be plain are
  members accessed once per function, or not at all, so nothing tells
  either way. memvol.log names, for each volatile member, the functions
  that need it.
- **Reading:** volatile is a property of the whole record, not of the
  fields the interrupts share. Either the original declared the state
  volatile, or every access went through a volatile pointer or macro.
  That is how gfx_state is declared now, so it stays.
  - The six views that read it plainly are places where the original
    code took a plain pointer into the record: the 48-word bg_plane
    copies and the slot_list reads.
  - They read like a programmer's own cast, for example a
    `(Uint32 *)&state.bg[0]` source for a copy loop. They stay as they
    are.
  - Nothing in source/ or the headers changed.

## 2026-11-04: user_back_data in plain members; four sub-records

- **The job:** Next item 3, the views of the backup image left after the
  tables entry. Across the 14 files that changed, the casts and offset
  arithmetic on user_back_data / user_back_defaults go from 159 to 3
  (testmode.c 133 to 0, card_log.c 12 to 0, game_stats.c 4 to 0).
- **Records typed** (build/structs/ub_records.py):
  - the 32 card item and 32 card data log records;
  - the ten pending-item entries;
  - card_use[2] by side;
  - the 36 challenge map hiscores and the 8 x 3 course rankings, as
    mission_score_rec (0x38 bytes).
- **Four sub-records** (build/structs/ub_nest.py). The game loads each of
  these bases once into a callee-saved register and reaches the members
  from it: play_time_account, testmode's bookkeeping and billing pages,
  and backup_data_clear_run's block copies. As flat members, every one of
  those functions needed a base-pointer view. As records they are plain
  `&user_back_data.bookkeep` and `b->starts[i]`, and the block copies are
  plain struct assignments.
  - rankings, +0x18: the ranking rows and best_win_streak;
  - bookkeep, +0xb48, 0xad0 bytes: p1_games .. stage_vs_rounds;
  - challenge, +0x24c0, 0xdf4 bytes: the hiscores, the course rankings
    and testmode's challenge totals;
  - billing, +0x32bc: starts, continues, ends by player.
- **Names from testmode's pages** (agent C, build/agents/ROUND-FF-UB.md):
  - total_time, play_time, vs_play_time, p1_time and net_test_timeout;
  - ledger_day_rec, the 30 day records of nine words;
  - prize_item_counts[8];
  - the chlg_* challenge totals, and mission_score_rec ranks[5];
  - the halfword counters unsigned.

  Its other renames are not adopted yet (the 1P / VS game counters at
  +0xb4c / +0xb50, the chara_best_* tables as MAX COMBO / MAX DAMAGE /
  MAX WALL COMBO / MAX WALL DAMAGE, and field_0x16b4 as the advertise
  connect switch).
- **user_back_defaults is a user_back_data_rec** in both programs, so
  user_back_reset is `user_back_data = user_back_defaults;`. test.bin's
  two globals were undefined4 and are now typed rows in
  ghidra/symbols_test/libbase.tsv.
  - user_back_defaults_once sets card_use[0], card_use[1] and
    net_connect of the defaults. These were three bare DATs; the typed
    record put them in check_externs' list.
  - Under test.bin, user_back.c now compiles and gives 8 of 10.
    user_back_defaults_once matches there too, but user_back_init and
    user_back_flush are not in test.bin's matched set.
- **The views that stay**, each with its reason in the code:
  - nam_req_game_init_enter (network.c) reads the time-out through the
    address of the net words held in an int. Six plain forms and a net
    sub-record were tried; each one swaps r1 and r2.
  - card_log_clear (user_back.c) steps the table pointer one entry at a
    time. An entry pointer or an index swaps two registers.
  - chlg_map_node_get's own symbol for the hiscores (msnm_disp.c, an
    existing ORIGINAL SHAPE). It is now declared as the 36 records and
    indexed (`&DAT_0c3769ac[iVar2]`, no `* 0x38`).
    - check_externs lists it as the 30th name (29 before): the exporter
      writes `undefined` for a DAT inside a record-array member.
  - The card data log snapshots stay bytes until the card record they
    copy is typed (the member comment in the header).

## 2026-11-04: the 12 open functions parked; trick texts declined

- **The ruling** (the maintainer): decline every text that matches only with a
  shape we do not accept, test the three natural leads, and park the
  rest. Every held text was re-measured against today's headers first.
  Three still match: 0c0adf20, 0c0e5a40 and 0c0dff80. The 0c06ebc0 and
  0c0ec3e0 texts no longer compile because of later renames.
- **Declined** (the texts stay in build/agents for reference):
  - 0c0adf20 stage_extras_setup: an inline `match_time_limit(int side)`
    with a VS arm that no caller takes. Nothing in the matched code
    computes `match_point * 7200 + 7200`.
  - 0c0e5a40 aet_still_quad_retransform: `unsigned long long pa`, and
    one `int *sz` reused for the copy, the +0x40 address and the result.
  - 0c0dff80 sprite_clip_push: `struct { char v; } c;`.
  - 0c06ebc0 attack_connect: `char *o = (char *)0x252;`.
  - 0c0ec3e0 win_item_row_draw: the one-shot
    `for (;;) { k = ...; break; }`.
- **The three leads, all negative:**
  - **0c0ec3e0:** `do { } while (0)`, the spelling matched elan_lights.c
    uses and fb_layout_compute adopted, gives no block end here. The
    front end folds it, so CSE still reuses the byte at +24: 11 lines
    with a byte test, 4 as now. Only a real loop's notes clear it.
  - **0c06ebc0:** the move history as a typed member (`short
    history[16]` at +0x252, index at +0x222, `move_no` at +8; a test-only
    record, build/ only) is 9-25 lines, against 4 now. The game indexes
    `c * 2 + p` and keeps 0x252 as the base register; the member form
    folds the offset into `add #2` plus a constant.
  - **0c0e5a40:** the +0x40 base is not a sub-record. SH-4
    `mov.l @(disp,Rn)` reaches at most +60, so GCC splits +0x5c into +64
    and +28. The matched caller makes the same `mov r4,r8; add #64,r8`
    from plain offsets. The 2 lines are only where the parameter copy
    lands.
- **Parked**, with the cause each one is traced to (build/agents/rtl-leads.md):

  | Function | Lines | Cause |
  |---|---|---|
  | 0c0fc220 elan_lights_queue | 2 | cse1 const-0 class |
  | 0c0e5a40 aet_still_quad_retransform | 2 | local-alloc ties the copy |
  | 0c0adf20 stage_extras_setup | 3 | CSE tie at the done store |
  | 0c0dff80 sprite_clip_push | 3 | combine folds the exts.b |
  | 0c06ebc0 attack_connect | 4 | local-alloc r0 life |
  | 0c0ec3e0 win_item_row_draw | 4 | no block end before the +24 re-read |
  | 0c0688c0 move_tcmd_33_slot_effect | 8 | reload's round-robin spill choice |
  | 0c147480 cfg_setting_or_default | 9 | reload move2add |
  | 0c0b8620 break_floor_model_build | 10 | global-alloc priority round-down |
  | 0c0dfea0 vram_used_total_update | 12 (8 with the held helper) | a register held across the loop |
  | 0c150ba0 render_hang_recover | 78 | loop.c movable threshold |
  | 0c03d9a0 prize_fly_draw | 130 | float register priorities |

  The two test.bin copies (cfg_setting_or_default, render_hang_recover)
  follow their main.bin functions. No more agent rounds go to these. One
  reopens when a header change gives new evidence, as typing
  mie_ident_text as an array and player_message as a table did for
  0c116860 and 0c041980.

## 2026-11-04: the card record and the card image typed

- **Types** (ghidra/types/vf4ft_types.h, after card_service_rec):

  | Type | Size | Where |
  |---|---|---|
  | card_header_rec | 0x1C | image +0, record +0 and +0x64 |
  | card_body_rec | 0x48 | image +0x1C, record +0x1C |
  | card_image_rec | 0x68 | card_images[2], card_image_get |
  | card_fmt2_header_rec, card_fmt2_body_rec | 0x1C, 0x3A | record +0x90 / +0xAC / +0xE6 |
  | card_record_rec | 0x1E0 | card_records[2], card_record_get |

  The evidence for each member is in build/structs/card_layout.md
  (private scratch). test.bin's card_records takes the type through
  test_transfer.tsv.
- **Source**: 21 files rewritten against scratch headers by three agents
  (build/agents/ROUND-CARD.md). About 1,300 raw offsets, the DAT_
  symbols inside the two arrays and the local views (ent, plr, K, R, h28,
  b72, the `{pad[0x104]; int f}` records) became members, and every file
  stayed at N of N. The nine functions that stopped matching against the
  typed getters did arithmetic on the getter's result
  (`card_record_get(i) + 0x104`), which now counts in records.
- **Casts kept**, each family noted on its member in the header:

  | Family | Sites | Why |
  |---|---|---|
  | body +0x40 (misc0..+0x43) as one uint | 24, plus bitfield views | the code loads and stores the word |
  | body +0x1C (curse..) as one uint | 8 | the same |
  | header ftf_flags low byte | 8 | byte loads and stores; the accessors read the halfword |
  | body +0x38 (emblem_dan..) as one uint | 4 | the same |
  | color_1p / color_2p stored byte by byte | 12 | card_format1_check's own shape |
  | money read as short | 2 | four other readers match it unsigned |
  | in_use low byte, as a value cast | 2 | rob_color_build, rob_item |

- **What the word casts point at**: the body's +0x1C, +0x38 and +0x40
  words were bitfield words in the code; the byte members are how the
  card stores them. A union of the bytes with `struct { uint :29; uint
  f:3; }` at +0x38 and `struct { uint :11; uint b11:1; uint b12:1; }` at
  +0x40 (tested in a scratch header) makes card_syu_bits_get and
  card_misc1_bits_3_4 match as plain members. A cast of the member's
  address goes through GCC's address expansion and swaps r0 and r1; a
  real bitfield component goes through the member-reference path, which
  gives the game's order. Next item 3.
- **Kept as they were**:
  - card_renew_backup stays char[2][480]: typed, card_mode_renew's two
    memcpy calls compile inline.
  - console_frame_draw and net_vs_parts_draw keep record pointers in int
    locals. Typed, the in_use load swaps base and index, and
    console_frame_draw's frame shrinks by 4.
  - card_mode_renew reads access_key as word 1 from &card_word0; neither
    member form matches, so the four words at +0x80 may have been one
    array or record.
- **Checked live in Flycast**: Flycast 2.7 built from source with
  ENABLE_GDB_SERVER (off in release builds; it runs the interpreter, so
  it is slow) lets gdb attach to the running game.
  - main RAM at 0x0C020000 is main.bin.
  - sffs_dir's entries read live match sffs_entry (block 0x1000, alive
    -1, mode 0100644, 2004 timestamps, names at +0x80).
  - A breakpoint on dimm_fopen logged the select screen's loads:
    aet_file_size opens each AET scene for its size before read_file
    loads it, each scene beside its spr_ sheet; sound_pack_open opens
    bg_vf4ftsel.bin; ltop_frame_read reopens spr_fire.bin again and again.
  - MAME 0.289 (with segadimm and jvs13551 complete) boots only to the
    BIOS: the driver is "preliminary".

## 2026-11-04: the card's bitfield words are unions

- **Two designs, built side by side** on copies of the 22 card files
  (build/agents/ROUND-BF.md). Both kept every file at N of N.

  | Design | Casts and views left on these words |
  |---|---|
  | unions of the stored bytes, the word and named bitfields | 0 (from 107) |
  | bitfield members only, byte readers cast | 52 |

- **Why bitfields alone cannot work**: GCC 3.2.3 takes a bitfield's
  access width from the containing object's alignment (get_best_mode
  with SLOW_BYTE_ACCESS), not from the declared type. Every body bitfield
  is a word access, yet the game also reads the same bits as bytes (the
  slot lock, `mov.b`) and halfwords (quest0_b0_get, `mov.w`), and tests
  them as word masks (`tst` against 0x300, 0x800). The original had byte
  and halfword members beside the bitfields.
- **The types** (ghidra/types/vf4ft_types.h):

  | Word | Union | Members |
  |---|---|---|
  | header +0x1A | card_ftf_half | `b[2]`, `h`, ushort bits ex_ver, ticket, skill_sel, recover |
  | body +0x1C | card_curse_half | curse / emblem bytes, `h`, ushort bits slot_lock, color_1p_hi0-3, emblem |
  | body +0x38 | card_syu_word | emblem_dan, syuutyuu; `w`; quest_year, emblem_dan, mission_day, prize_misses, revise, user_class |
  | body +0x3C | card_quest_word | quest0, quest1; `w`; state, kind, limit_day, limit_month, param_a-c, category |
  | body +0x40 | card_misc_word | `b[4]`, `w`; equip, colour and net-colour flags, color_2p_hi0-3, tenpo_value2, foul_first / second, disp, lottery_count |

  - The header's bits are ushort: a uint member makes card_header_rec
    4-aligned, and the halfword read-modify-writes at +0x1A become word
    ones.
  - The +0x18 "word" was never one. Through a card_record_rec, which is
    4-aligned, GCC widens a store to ex_ver to the word at +0x18
    (card_mode_entry_read now writes `e->header.ftf.f.ex_ver = 1;`).
  - +0x1C is a 2-byte union, so name stays a plain member at +0x1E. GCC
    widens its ushort bitfields to the game's word accesses.
- **What became plain**: all 45 word casts of b997fc7, the anonymous
  bitfield views, and the syu / quest / ftf / nickname accessors in
  card.c (`card_images[n].body.syu.f.user_class`). quest0_b0_get reads
  `quest.b.quest0 & 7`.
- **Shapes**:
  - The mask tests read `misc.w` (`& 0x300`, `& 0x800`, `(w & 0xc) ==
    0xc`). A bitfield test compiles to shift and and.
  - The two level-colour functions and card_cpu_play_end_side take a
    `card_misc_word *m` local for the net-colour flag. The game builds the
    full address (`add #92; mov.l @r1`). A member reference splits it
    (`add #64; mov.l @(28,r1)`).
  - item_award_rule_check and item_slot_unlock set slot_lock with `|=`
    and `&= ~`.

## 2026-11-04: the placeholder count no longer counts members

- **The progress page's "Placeholder variables" went from 36 to 125**
  with the card types. The pattern for Ghidra's in_ / unaff_ /
  extraout_ names also matched the member in_use: 89 new hits, plus 3
  older member names. It now skips names after `.` or `->`
  (tools/progress.py). The two history points it inflated are corrected
  by those 92 hits, to 33, which is what the fixed measure gives now.

## 2026-11-04: the last card casts go; the card data log and bookkeeping names

- **Color words are unions** (card_color_word: `uchar b[4]` / `uint w`;
  card_color_half: `uchar b[2]` / `ushort h`). card_format1_check stores
  the two low bytes of each color word with byte loads at +0, +1; the
  shift form gets the low byte but compiles the high byte to
  `mov.l @(8,r13); shlr8` where the game has a byte load at +9. Only a
  byte lvalue gives that. The 12 sites read `color_1p.b[0]` and so on;
  every other user reads `.w` or `.h` (card.c and card_item.c too).
- **money**: the two signed reads in mission.c are the value cast
  `(short)r->body.money`.
- **card_fmt2_body_rec misc** is `uchar misc[3]`: card_unpack_format2's
  loop reads `b->misc[i]`, card_format2_upgrade `misc[0..2]`.
- **in_use's low byte** stays a value cast (`(uchar)r->in_use`, rob_color
  and rob_item); nothing reads it any other way.
- **card_data_log_snap** (20 bytes) types card_data_log_rec's
  `snapshots[2]`: all_win, all_lose, level, exp, point, color_1p,
  color_2p, rank_pnt, tenpo_pnt. card_snapshot_clear takes one. In
  card_data_log_restore the two word compares (`*(int *)&q->rank_pnt`,
  `*(ushort *)&q->exp & m`) are plain member compares: GCC merges
  `&&`-chained compares of adjacent fields into one wider compare, which
  is what the game shows (the 0xffff mask on both sides is its mark). So
  the original compared member by member; the two dead locals went.
- **Bookkeeping names**, each from the test-mode page that prints it with
  its label (test.bin loads paired with the strings):
  - +0xb48 `games_total` (was p1_games): "NUMBER OF GAMES", incremented
    at both 1P and VS ends; +0xb4c `games_1p` "1P GAMES"; +0xb50
    `games_vs` "VS GAMES". Each is the divisor of its average time.
  - chara_data_page_draw: +0xba8 `chara_max_combo` (rob +0x24e), +0xbc8
    `chara_max_damage` (+0x250), +0xbe8 `chara_max_wall_combo` (+0x24a),
    +0xc08 `chara_max_wall_damage` (+0x24c).
  - +0x16b4 `advertise_connect`: game_assignments stores the ADVERTISE
    CONNECT setting (item 10, OFF/ON) there, and net_message_request_step
    sends its request in MODE_MAIN_ADVERTISE only when it is nonzero. The
    card log comment above it belonged to card_log_stamp at +0x16b8 (the
    old card_log_session aggregate's +0x3C); no card log routine touches
    +0x16b4.

## 2026-11-05: the AET scene files are decoded and checked against the running game

- **Flycast's GDB server takes no watchpoints.** v2.7 answers Z2 / Z3 / Z4
  (and Z1) with an empty reply (`core/debug/gdb_server.cpp`,
  insertMatchpoint: only software breakpoints, a `trapa` patch). Live
  checks use breakpoints and memory reads; a watchpoint would need a
  Flycast patch.
- **Live dumps**: with the game in attract mode and then on character
  select, a gdb script read aet_sets and the AET stack, saved each loaded
  file as the game holds it (relocated, with its run-time state), and the
  AET module's state (aet_objs, aet_stills, their caches): five snapshots,
  eight files. aet_objs +0x58 frame, +0x5C start, +0x60 end are ints.
- **tools/aet.py** decodes all 30 files (header, scenes, compositions,
  layers of 0x8C bytes, eight tracks of Hermite keys, images, pictures,
  markers), with every field name taken from the matched aetset.c,
  aet_play.c and aet_anim_if.c. Its `check` mode compares a file with a
  live dump:
  - the words the parse says the loader relocates are exactly those that
    differ by the load address (17,194 over the eight files, none missing
    or extra);
  - every other differing word is a layer track float (+0x14 .. +0x30) or
    the visible byte (+0x34) that aet_layer_tracks_eval writes;
  - replaying the draw pass from the object records (port of
    aet_key_curve_eval in the game's instruction order, its three `fmac`s
    included) gives the stored floats bit for bit: 810 words per attract
    snapshot, 1,125 per select snapshot. The values come from the object's
    current frame word (aet_objs_step runs before aet_objs_draw); one frame
    back fails on the animating objects. Only round-to-nearest with a
    single rounding in `fmac` matches every word (as Flycast runs it).
  - A corrupted pointer word and a corrupted layer float each make the
    check fail.
- **adv_rank_aet.bin is broken**: its header is at +0x438, and its first
  word is 0, so aet_file_relocate would read through a null pointer. No
  code loads its set (2) by number.
- The data never uses markers, parent layers, box layers, a time scale
  other than 1 or more than one picture per image, so those code paths
  are unchecked. The structures (docs/asset-formats.md, "2D scenes") are
  ready for a typing round of aet_play.c and aet_anim_if.c.

## 2026-11-05: the AET structures are typed in the AET source

- **23 types** in the aet_play.c section of the header, from the layout
  tools/aet.py proved against the running game: the file structures
  (aet_file_header, aet_scene, aet_scene_tex, aet_comp, aet_layer and its
  item union, aet_track, aet_key, aet_marker, aet_image, aet_picture,
  aet_table3_item), the set record (aet_set_rec with its +0x80
  aet_set_info), the quad (aet_quad, aet_quad_vertex), aet_box,
  aet_draw_state_rec, and the object and still records with their start
  blocks (aet_obj_rec, aet_obj_param, aet_still_rec, aet_still_param,
  aet_still_cache_rec). Sizes are checked with a negative-array test.
- aetset.c, aet_play.c and aet_anim_if.c use members; 88 prototypes and
  the eight AET globals (aet_sets, aet_draw_state, aet_objs, aet_stills,
  aet_still_caches and the three current pointers) are typed. The six
  callers (adv_demo, cns_nickname, console, select, title_disp,
  msnm_disp) build their start blocks as aet_obj_param / aet_still_param
  (40-odd stack arrays) and pass layer names without casts.
- **aet_draw_tail is gone**: it was a view of aet_draw_state +0x7C, and
  every use now matches through aet_draw_state's members. Three DAT_
  externs inside the record went too.
- **What stays a view**, each because no member form matches:
  - the relocation adds (`p->X = (T *)((int)p->X + base)`): C cannot add
    a byte delta to a typed pointer;
  - aet_file_relocate's word helpers (`int *`) and its scene loop, whose
    member form swaps r0 / r1;
  - aet_set_free, aet_scene_table2_get and aet_set_scene_count reach the
    set record at +0x88 / +0x8C through an offset (`mov.w #0x88,r0;
    mov.l @(r0,r1)`); the member forms give `add #124; mov.l @(12,r1)`;
  - aet_quad_build's vertex address (`sts macl,r2; add r10,r2`; every
    member form adds through r1);
  - aet_stack_entry.addr is an int, so the loaded file is cast from it.
- aet_draw_state_rec.measuring is uchar: as char, aet_bounds_layer_add's
  box read folds into `fmov @(r0,r9)`.

## 2026-11-05: the stage collision files are decoded

- **tools/stage_coli.py** decodes the 18 `st_*_col.bin` files: a 0x4C
  header, an x / z grid of 2-unit cells with null-ended polygon lists,
  12-byte vertices, 0x3C-byte polygons (kind bits 0x100 floor, 0x200 wall,
  0x400 ring-out ground, bit 0 breakable; slope and heading words, plane,
  five vertex pointers) and the breakable-wall records. Field names come
  from field.c, rob_coli.c and the hit-test routines; unread words stay
  unknown. The files are built for address 0 (`field_coli_set(0, buf)`).
- **Offline check**: a port of field_coli_set and its helpers changes
  exactly the parse's 16,963 pointer words; every byte of 17 files is in
  one structure (st_test0, which no stage names, has a 4-byte gap); a port
  of field_floor_height through coli_segment_hit gives the ring height at
  the center and start positions of all 24 stage records.
- **Live check from three of the maintainer's save states** (UMI, YAM twice), read
  with tools/flycast_state.py: the game's copy equals the relocated file,
  and both fighters' floor heights and four wall-probe distances match the
  port (within 1e-6, at the position before the frame's shift). A
  corrupted pointer in a copy of a dump makes the check fail.
- Found: the open rings ship with their four walls switched off in the
  file (CAS and NYC 16 of 48 faces), which is how fighters fall out
  there; st_cas serves six stage records; st_test0 is an older layout the
  current code cannot read.
- Not ported: coli_sweep_hit's plane push. Not exercised by any file: the
  group table.

## 2026-11-05: the sound sequences (DTPK sections 0-3) are decoded

- Read from the ARM7 sound driver (aicadrv.bin, disassembled with
  arm-none-eabi-objdump), every field with the driver address that
  handles it: section 0 channel setups (32-byte parts: channel, program,
  note / velocity ranges, volume, pan, transpose, detune), section 1 the
  program table (programs of type 0 keyboard, 1 note map, 2 index, up to
  four layers), section 2 velocity curves, section 3 the sequence table
  the game's request words `{class A8-AD, id, number}` look up across all
  eight loaded banks. Scripts are event streams or looping songs; events
  are MIDI with running status plus the driver's `Dx n v e` note-on,
  `A0 dd vv` control and `A8-AD` sequence starts. One tick is 44 samples
  (TIMA 0xD4, 1,002.27 Hz) and there is no tempo command.
- **tools/dtpk_seq.py** dumps setups, programs and sequences, writes MIDI
  (one MIDI tick per driver tick) and checks all 80 banks: 3,218 scripts
  and 6,379 events parse to their ends; sections 0-3 are covered except
  zero padding and 8 leftover `2f 00` end-of-track bytes from a MIDI
  conversion; all 181 sequence starts and 4,997 tones resolve, and 5,480
  of 5,506 note-ons reach a tone (the 26 others are harbor's song, copied
  from sea's, playing a channel harbor's setup lacks).
- **The 57 "not settled" ambience waves** are in section 1 programs of
  their own banks, which tools/dtpk.py did not read. Their programs lay
  waves out as octave ladders, which gives 52 of them a rate (8,003 to
  22,250 Hz) that the stage songs confirm; 5 (dryleaf 6, 7, snow 0-2)
  stay unsettled and are written at their note-60 rate. tools/dtpk.py now
  writes them so (manifest `rate_source`); regenerating with the old and
  new dtpk.py differs in exactly 40 WAVs, all among the 57, plus the
  manifest.
- The old section 4 reading was wrong in two places: its "last index 2" is
  the program type byte, and the layer header's +2 / +3 is a velocity
  range, not a key range.
- Not traced: fifteen `A0` control codes, the `Ax` handler, a few setup,
  part and program bytes, sections 5 and 6. The music chunks run 0.27%
  longer than 1,714 ticks, so the real tick may be slightly over 44
  samples.

## 2026-11-05: sequences played through the banks' own waves

- The two MIDI exports sounded wrong, and MIDI was the wrong form for them:
  `bg_vf2_akira aa:10:1` plays a type 2 (index) program, where note k is
  chunk k of the recorded stream, so a General MIDI player plays a rising
  scale; `se_stage_sea a8:7f:0` plays pitched copies of surf and wind
  waves. The docs now say what MIDI keeps.
- **tools/dtpk_seq.py --wav** keys on voices as the driver does and mixes
  them as Flycast's AICA model does. New driver reads: tone +1 bit 7 marks
  a one-shot that the driver keys off once the play position reaches the
  loop start (0x3B24, 0xA74); tone +7 `DISDL`, +8 level, +0x0C / +0x0E
  the envelope registers, +0x10 .. +0x19 the filter, +0x1E / +0x1F the DSP
  send, +0x30 the LFO register (0x3B8C-0x3C98); the `FNS` table at 0x8050
  rounds (music at 32,019.6 Hz); the total level formula
  (0x4270); part +9 `DISDL`; header +0x1A the resident voice limit (32);
  looped voices freed 48 dB down (0xA7C).
- Checks: akira's right channel is the left 792 samples later
  (correlation 1.000000); mid-chunk the left matches the chunk resampled to
  the floating-point floor; `check` passes as before and `--midi` output is
  byte-identical.
- Open, for a recording from the game: the 44-sample tick makes each akira
  chunk overlap the next by 4.1 ms (`--tick 44.1048` joins them).
  (Settled: the game overlaps them; gymnasium recording, below.)
  (Committed as 0f75e67 with a second open item, a buzz on 130 sea notes
  held past their wave's end; withdrawn below.)

## 2026-11-05: the ADPCM decoder rounded negative steps down

- tools/aica_adpcm.py added `(step * mul) // 8` with a signed `mul`, which
  rounds negative deltas toward minus infinity: a +-1 nibble at step 127
  gave +15 / -16. The chip's formula, as Flycast's `DecodeADPCM` has it,
  applies the sign to the truncated magnitude. The lost unit drifted every
  ADPCM wave downward: sea and character effects that fade out ended at
  -4,600 to -18,300 instead of near 0, and some waves clipped more (a
  vo_akira wave 74 samples against 13).
- Found through the WAV renderer: the "buzz" on held sea notes was the
  24-sample end loops sitting at that drifted DC, which RMS counted as
  loud; decoded right they hold 8 to 10 units, silence. A recording from
  Flycast (`pw-record`, sea stage) was made for the buzz question; it
  cannot arbitrate between decoders, since Flycast plays with its own.
- Fixed decoder: identical to a transcription of Flycast's on all 2,444
  ADPCM waves (both channels). work/sound regenerated.

## 2026-11-05: a Flycast recording settles the music tick

- The gymnasium stage plays `bg_vf41b19 aa:2b:1` (a spectrogram match of
  a Flycast recording against all 32 music streams: 0.67 against a median
  of 0.37), a chunked stream like `bg_vf2_akira`: tone 0 a 1,704-sample
  intro, tones 1-32 every 1,053 ticks, looping.
- Tracked against `stream.wav`, the recording steps forward 2.69 ms
  (sd 0.17, 22 joins) at every chunk join: the chunks overlap, as the
  44-sample tick predicts, and do not join end to start (that would be 0).
  The tracker reads 2.484 ms on a `--tick 44` render (2.493 predicted).
  `--tick 44` stays the default; the Unknown item is closed.
- Flycast plays the chunks 657 ppm slow: `UpdatePitch` drops the fraction
  of `(1024 | FNS) >> 1` (743 for 743.5), 31,998.6 Hz. Its ticks average
  44.02 samples (from the steps at that rate); not traced.
- Open, renderer: the recording is mono (L = R), and the second stream
  runs 17 ticks behind the first, not the script's 18. (Both done below.)

## 2026-11-05: the renderer mixes mono and models the start race

- Traced why the gymnasium's second stream trails by 17 ticks, not 18:
  the start routine runs a new script's first events at once [0x7D80 ->
  0x71D0], and every message a stream sends goes to a ring [0x2B1C] that
  the main loop drains at the top of its next pass [0x668]. A start sent at
  its script's time 0 (from the host request) waits the rest of a pass; if
  the timer tick [0x7108] falls in it, the started script counts from
  after it. Starts sent from the slot loop (end of pass, just after the
  tick) do not. The main loop never waits for the tick, so this is a race
  that depends on the pass length; the recording shows the late case. The
  0x71B0 tick catch-up only runs in MIDI-clock sync mode.
- tools/dtpk_seq.py: `--start-lag TICKS` (default 1, also for `--midi`):
  a start sent at its script's time 0 takes effect that many ticks later;
  `--mono`: (L + R) / 2 to both channels, as the chip's mono bit (Flycast's
  mix). `--start-lag 0` reproduces the previous output exactly.
- Checked: the render's comb is now 748 samples (17 ticks) and 0.25 s
  windows of the recording match it with a median correlation of 0.82,
  against 0.66 for the 18-tick render. Akira's right channel is now 748
  samples behind the left. `check` passes.

## 2026-11-05: the start race goes both ways

- A 300 s Flycast recording (boot, menus, player select, three stages;
  stereo) through scratch tools/tracks.py: the music tracks found by
  spectrogram match against the 32 streams, then per play the left-right
  correlation at 17 and 18 ticks, window by window (center-panned fight
  sound makes lag 0 the overall peak, so the two lags are compared
  directly). `bg_vf41b61`: 17 ticks (749 samples); `bg_vf41b18`: 18;
  `bg_vf41b22`: 18 (791-795); `bg_vf4ftsel` (player select) is one
  stream. Two short `bg_vf41b10` hits were false (one in the silent mono
  boot, one at the b18 start).
- The earlier sea-stage recording (stereo) also holds `bg_vf41b03` under
  the ambience: 18 ticks (791 samples). With the gymnasium's 17, five
  plays: 17 twice, 18 three times. The race is real and goes either way;
  `--start-lag` default back to 0 (the script's delta), 1 for the late
  case.
- Open: the gymnasium recording was mono and the others stereo; which
  setting or state chose mono is not known.
  (2026-11-09, the maintainer: his USB headset was probably attached for that
  recording. Not confirmed. The recordings capture the default sink's
  monitor (`pw-record -P '{ stream.capture.sink=true }'`), so with the
  headset attached they capture the headset's sink, not the HDMI one. A
  downmix there gives (L + R) / 2 on both channels, the same as the
  chip's mono bit, so the recording cannot tell the two apart. Against:
  WirePlumber's saved profile for the headset is stereo playback with a
  mono microphone. The journal no longer reaches 2026-11-05. Test: with
  the headset attached, play a left-only tone and record the same way.
  Until then, nothing shows the game itself chose mono.)

## 2026-11-05: the smooth_ files decoded (stance poses)

- `smooth_<chr>.bin` is the rsmooth table: 16 blocks of 0x5C8 bytes in
  `e_rsmooth_no` order, each 23 bone matrices (`rsmooth_bone_list`) and
  the two hand-shape floats. `rob_skeleton_update` picks a block by the
  stance byte rob +0x285 (+1 for the `_M` twin on rob +0x3A6 bit 4), and
  the end-of-move blend eases toward it with weight rob +0x3D4, rising
  from 0 to 1 over the move's last 9 `rsmooth_frames` (rob +0x0A is the
  move frame, +0x2E4 the move length; the `rsmooth_frames` note had the
  two swapped and is fixed).
- What the game reads, from the matched code: the 3x3 rotations of the 16
  bones that take the rotation lerp in `rob_eval_output` (flag 0x800,
  not 0x2008000), the positions of `k_hara` and the six `t_` targets
  (`rsmooth_targets_set`, then the two position blends), and the hand
  values (bones 13 / 21 +0x2C, into the hand records). The rest is
  exporter output.
- Character id 12 is BLACK, `smooth_blk` and `ctrl_VAN` (the `ctrl_`
  names in main.bin run in id order). It is the only fighter that
  reaches the SP_ blocks (rob +0x550, its second control set), and
  `smooth_blk` is the only file that has all four. The other files hold
  0xCDCDCDCD fill (MSVC debug fill) there, whole or partial; jac and pai
  hold shifted data.
- The `_M` blocks are exact mirrors (positions x negated, l / r swapped;
  rotations with fixed per-bone signs; hands swapped) within 1e-5, but
  jac SY_KAMAE (0.002) and lion TA_KAMAE (posed separately).
- `tools/smooth.py`: per-file checks and a JSON dump; docs/asset-formats.md
  has the section. Not applied in pose.py or gltf.py (listed under Not
  emulated).

## 2026-11-05: body slots from the game's table; the wrists emulated

- The slot table: the fighter's 31 bone matrices (rob +0x4C) belong to
  the bones at 0x0C18BCB8, and the first 24 go to the body slots by the
  s16 table at 0x0C18BCD8. `rig.body_slots` reads both; `pose.SLOTS` (a
  hand-made table) is gone.
- Fixed: slot 2 is `k_mune`, not `t_mune`. The two share a position
  (`k_mune` is `t_mune`'s child at offset 0) and the bind matrices cannot
  tell them apart, but `k_mune` has its own motion rotation, which 438
  motions use. The chest pieces (126 nodes) and slot-2 envelope weights
  were off in those motions (about 1 cm in AKIRA_MAKE frame 30).
- The wrists (`tekubi`, slots 8 / 13) are emulated (`rig.wrist`): the
  hand's matrix before its rotation, turned about X by the twist of the
  hand's local Z axis (`rob_eval_output`, flags 0x8000000 / 0x4000000,
  stored at +0x240 / +0x380). Checked against the game's own matrices in
  three Flycast save states: 12 hands, within 2e-4 (one 16-bit angle
  step); the transposed reading misses by up to 1.5. One of those frames
  has the rsmooth blend running (+0x3A6 bit 3): its hand is blended, its
  wrist follows the unblended rotation, as the code says.
- pose.py: the 16 wrist stand-ins (8 sets) are gone; 10 approximations
  are left (XRAY slot 0, JDK / MUE leg slots 123-126). gltf.py: the
  wrists are joints under the forearms. gltf_check.py re-skins chr_aki,
  chr_jef, chr_dur within 2e-6 of pose.py (chr_mue: the known 5-bone face
  groups, 0.26 mm). All 65 character sets build.

## 2026-11-05: the game's posing passes in pose.py and gltf.py

- `rig.pose_passes` follows `rob_eval` (0x0C095300) pass by pass:
  `rob_eval_ik`, `rob_pose_mirror`, `rob_eval_matrix` (the root blends),
  `rob_bone_pos_blend` (the IK ends blend), `rob_eval_output` (the IK
  again, the rotation blends, the look-at). `rig.pose` uses it whenever
  an option is set or the fighter is not on rig 0.
- Found: `rob_eval_ik` takes lengths, rest offsets and bend signs from
  `bone_defs` (rig 0) for every fighter and only the flags from the
  fighter's own rig, so every fighter's IK angles are solved with rig 0's
  proportions. This was Jeffry's 0.07 miss; the default pose of the 11
  other fighter rigs moves 0.08 to 0.17 (limbs) with the fix. A pose
  costs 0.8 ms instead of 0.3 on those rigs.
- Emulated: the mirror (three tables at 0x0C2A05C4-0x0C2A0614), the
  end-of-move blend to the `smooth_` stance pose (the plain case), the
  motion-change blend, the head's look-at (`look_target_clamp`) and the
  eyes' (`eyes_look_at`, tools/face.py). pose.py takes `--mirror`,
  `--smooth`, `--from MOTION FRAME`, `--look X Y Z [FADE]`, `--eyes X Y
  Z`; gltf.py all but `--from`.
- Checked against the 31 matrices in six save states: within 1e-4
  everywhere (Akira with the smooth blend and the look-at running: 6e-5,
  through pose.py's own option path; Jeffry mirrored: 7e-5), but Jeffry's
  `gblctr`, 0.065 low: the terrain floor lift, not emulated. Unchecked
  live: the motion-change blend (its weights 0 and 1 reproduce the two
  poses exactly) and the eye look-at; no save state has either running.
- gltf_check.py re-skins chr_aki, chr_jef, chr_dur within 3e-6 of
  pose.py, also with `--mirror --smooth --look --eyes`. All 65 character
  sets build.

## 2026-11-05: readability wave 1, the fighter code reads like source

The readability pass starts: matched C keeps its bytes but loses the
decompiler's names and types (`param_N`, `iVarN`, `undefined4`,
`DAT_0c......`, `*(short *)(p + 0x318)`). The progress page gains a tile
for it: matched functions with none of those left, 1,040 of 6,254 before
this wave, 1,472 after.

- **rob_rec is complete** (ghidra/types/vf4ft_types.h): 279 typed
  members over the whole 0x86C bytes, from every matched access (offset,
  width, sign) and the live checks in docs/asset-formats.md; 194 named,
  85 kept as `field_0xNNN` with their width, 169 bytes of padding nothing
  touches. Sub-records: rob_action_rec (a word or {move, lever, type}),
  rob_combo_rec, rob_react_row_rec, rob_reach_rec (rob_attack_reach's
  state with four rob_limb_rec), rob_hand_rec, rob_old_rec. Nine existing
  members changed width or sign to what most code reads (side, chara,
  costume, move_no, body_model, stance, ...).
- **Nine agents** rewrote libaction, libenemy, libmotion and librobdisp
  (19 files, 1,086 functions), each against its own copy of the headers
  and checked function by function: every parameter and local named, raw
  offsets on the fighter and the other records as members, parameters
  typed (`rob_rec *rob`, `control_rec *ctrl`, `vp_work *work`). No
  `param_N` or `xVarN` is left in those libraries; compiler warnings in
  them fell from 643 to 302 (most of the rest are calls the new
  prototypes leave warning).
- **475 globals named** (rows with evidence in libaction, libenemy,
  libmotion and librobdisp .tsv), and 60 new types: control_rec,
  control_history_rec, control_set_rec, input_log_rec, the cloth and osage
  records, vp_rob_snap_rec, chara_sphere_rec, item and color records.
- **Found while merging**: a name given to an address inside another
  object splits it in Ghidra. `rob_cmd_table_recs` was control_objects[0]
  +0x5C and `input_log_word` input_log[0] +4: both are now written from
  their base array and have no row. `shadow_piece_on` is 15 ints, not 16
  (its 16th would be shadow_erase), and shadow_erase is an int: the code
  only reads and writes it as a word. Tables whose length the code does
  not show take it from the gap to the next symbol (each row says so).
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; no existing global changed type except shadow_erase;
  check_private_names 0 hits.
- **Next in the pass**: the existing types the agents could not change
  (rob_rec.ctrl as control_rec *, rob_rec.items, vp_work's pointers,
  rob_coli_rec's flag words, osage_param), the globals more than one file
  shares (gravity at 0x0C371988 and 30 others), and the call sites the
  new prototypes left warning; then waves 2 to 4 (libmission, libcard,
  libeffect; libbase and the smaller game libraries; the SDK and system
  libraries).

## 2026-11-05: readability wave 2, mission, card and effect code

The second wave of the readability pass, together with a second pass over
the fighter files: 17 agents, each on its own copy of the headers, then
one merge. Matched functions with no decompiler names or types left went
from 1,472 to 3,054 of 6,254 (49%).

- **Shared types the first wave could not change** are now typed:
  rob_rec.ctrl is `control_rec *` and rob_rec.hit_params `est_row *`;
  rob_coli_rec.rob, vp_work.rob and itmtst_rec.rob0 / rob1 are
  `rob_rec *`; vp_work's snap, log and level_params point at their
  records; osage_param has its layout (rot_type, limit, stiffness,
  damping, gravity, rest_ry, rest_rz); osage_node.ang is two shorts;
  script_choice +0xC / +0xE are the saved allow-frames and window;
  hit_est_record is `est_row *`. The fighter files that did arithmetic on
  those pointers were rewritten to members, and the casts the new types
  made redundant are gone.
- **Second pass over the 19 fighter files**: decompiler names and types
  in them fell from 1,548 to 328 (the rest are extern lines for globals
  other files share, and ORIGINAL SHAPE declarations), warnings from 302
  to 107. enemy_work is an `enemy_work_rec` (0x154 bytes, 238 raw
  offsets became members); rob_eval_rob is `rob_rec *` (about 100 casts
  gone).
- **libmission, libcard, libeffect** (26 files, 1,436 functions):
  decompiler names and types fell from 13,074 to 395, warnings from 679
  to 319. New records include msnm_work_rec (the whole 0xB2C-byte
  mission work block and its parts), mission_rec, chlg_result_rec, the
  nickname records, card_md5_rec, the card mode and renew records, and
  the rain, sand, snow, leaf, sky, bird, fish, audience and aquarium
  effect records.
- **Totals**: 265 globals named, 103 existing globals retyped (arrays
  with their element types, pointers to their records), 227 new types,
  1,180 prototypes typed. Two hair-color tables really start 4 bytes
  later than their old rows (the code indexes them [chara][costume]
  [hair - 1]), so their rows moved; mot_slot_pending is gone, being byte
  +0x11 of each mot_slots record. Tree-wide warnings fell from 2,613 to
  2,140.
- **Found while merging**: Ghidra's type parser does not know the
  exported header's `code` type, and it exports a function pointer
  member declared with `()` as `(void)`, so a pointer the code calls
  with arguments needs its full prototype in the type
  (msnm_try_type_rec.frame / round_end). Names an agent gives at an
  address that already has a placeholder row (rob_fx_wait_frames,
  step_fx_min_move in signatures.tsv) rename that row.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.
- **Next in the pass**: wave 3 (libbase, libsprite, libnetwork, libdebug,
  libpoly, libcore, libsound, libnl2http and the sy library), which will
  also clear the call sites in those files that this wave's prototypes
  left warning (network.c, console.c, mode.c); then the SDK libraries and
  the test program, and last the globals several files share.

## 2026-11-05: readability wave 3, the game's base, sprite, network and core libraries

The third wave of the readability pass: 12 agents on libbase, libsprite,
libnetwork, libdebug, libpoly, libcore, libsound, libnl2http and the sy
modules (70 files, 1,806 functions), then one merge and a fix pass over
the call sites the new prototypes touched in files earlier waves had
finished. Matched functions with no decompiler names or types left went
from 3,054 to 4,198 of 6,254 (67%).

- **Per library**: decompiler names and types in the wave's files fell
  from 14,696 to 845. What is left is mostly extern lines for globals
  several files share, `player_input_get`'s untyped result (17 files add
  +0xC / +0x14 to it by hand), and the `sprintf` prototype's `undefined4`
  arguments; all three wait for the shared-globals step.
- **New records** include player_input_rec (0xE4 bytes, `player_input` is
  four of them: pressed, released, held and repeat state, the input
  history and the per-button hold and repeat counters), rep_frame_rec
  (the 0xBC-byte replay frame; `rep_ring` holds 360, and the replay save
  and apply copy named fighter members instead of raw offsets),
  coli_poly_rec (the 0x3C-byte stage collision polygon the hit tests and
  relocation walk), txn_entry (a texture-color list entry: texture,
  brightness, saturation, hue, swap texture and two colors), the console
  message, life gauge and win-streak records, the nickname plate and
  select-screen grid records, the light and fog event records, and the
  sprite, AET, socket and DMA records of the other libraries.
- **Totals**: 373 globals named, 261 existing globals retyped, 114 new
  types, 803 prototypes typed. The fix pass changed no prototype: each new
  one fit what its callers pass, so 21 callers' locals took the real types
  (zanzou trail and model node pointers, the effect heap as `heap` and its
  statistics as `mallinfo`). Tree-wide warnings fell from 2,140 to 1,691.
- **Found while merging**: a symbol row's array length must be decimal
  (`char nsock_send_buf[4096]`; the row parser rejects `[0x1000]`); a
  blank line between a definition's signature and its `{` still compiles
  and matches but drops the function from source/ in the export (6,253
  instead of 6,254 taken from source/; the exporter only reads a brace on
  the very next line); a file that declares a global itself (mode.c's
  `rank_page`, mission.c's `net_check_log`) stops compiling when another
  file's agent retypes it, so those lines go.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.
- **Next in the pass**: wave 4 (the SDK libraries, the C runtime and the
  test program), then the globals several files share (player input,
  task records, fonts, the `sprintf` prototype).

## 2026-11-05: readability wave 4, the SDK libraries, the C runtime and the test program

The fourth wave of the readability pass: 15 agents on the graphics,
socket, DIMM, JAMMA and board libraries, the game-built graphics library,
the NAOMI and NAOMI2 libraries, the C runtime and the test program (196
files, 1,799 functions), then one merge and a fix pass over the call sites
the new prototypes touched. Matched functions with no decompiler names or
types left went from 4,198 to 5,481 of 6,254 (88%).

- **Per library**: decompiler names and types in the wave's files fell
  from 18,898 to 640. Most of what is left is in the ELAN command code
  (register words written through raw pointers) and the extern lines for
  globals several files share.
- **New records** include surface_rec (a texture or frame surface),
  tex_slot_rec and render_target_rec (the game-built library's render
  targets and passes), strip_context_rec, sock_slot_rec (a socket slot),
  compiled_model_rec and model_cmd_iter_rec (a compiled NAOMI2 model and
  the walker over its commands), light_table_entry and
  elan_light_params_rec, the block heap and the network client's job,
  settings and HTTP connection records, the scanf stream, and the test
  program's menu, input-test and card-test records.
- **Totals**: 149 globals named, 233 existing globals retyped, 102 new
  types, 1,263 prototypes typed; the test program retypes 22 of its
  globals. The fix pass typed the callers of the new prototypes (28 files;
  every `sprintf((uint)buf, ...)` / `atoi((int)text)` cast is gone where
  the caller's value could carry its real type) and changed two
  prototypes (`nsock_connect_start` takes the address as `void *`,
  `sprite_quad_send_unrotated` a `sprite_quad_args *`). Tree-wide warnings
  fell from 1,691 to 933, and the test program's from 275 to 7 (main's
  new `text_draw_formatted`, `sprintf` and `atoi` signatures carry over
  to test.bin).
- **Found while merging**:
  - `atoi`'s callers need a `long` return: with `int`, GCC orders
    net_power_on_reply_parse's literal pool the other way (an unsigned
    32-bit type also matches). newlib's atoi and atol compile to the same
    body, so the function is most likely `atol`; it keeps the name atoi
    for now.
  - `mtx_stacks_start` returns `int` (with `void`, hardware_boot keeps
    its result in another register).
  - The export wrote only main-RAM globals to vf4ft_globals.h (a limit
    from when every named global was a debug-menu one), so the 33
    registers this wave named (ELAN, PVR, DMA, RTC, the LED latch) left
    their files undeclared; ExportSource now writes named globals at any
    mapped address, as vf4ft_data.h already did for unnamed ones.
    `elan_ctrl_reg` joins volatile_globals.tsv (elan_ctrl_bit0_off_or_back
    needs it). Addresses Ghidra does not map at all (the BIOS ROM, the
    save SRAM) stay file-local `#define`s.
  - A typed data row cannot sit on bytes Ghidra disassembles: the jump
    stub code_jump_patch copies (0x0C12C740) is three name-only rows.
  - The `lights_xform_by_current` rows had its test backwards: a
    positive `lighting_path` selects the 16-light routine, zero the
    single-light one.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.
- **Next in the pass**: the globals and records several files share
  (player input, task records, fonts, the light record's +0x5C / +0x68
  members, compiled model vs bounding sphere, tex slot vs surface, the
  strip context), and `atoi` → `atol`.

## 2026-11-05: readability wave 5, shared globals and the last leftovers

The fifth wave of the readability pass: eight agents on the globals
several files share (which no single file's agent could name) and on the
functions earlier waves skipped or left partly done, then one merge.
Matched functions with no decompiler names or types left went from 5,481
to 5,924 of 6,254 (95%).

- **Groups**: two on the game code (one also owned every shared game
  global, across 37 files), one each on the graphics code (24 files),
  the board, boot and DIMM code (11) and the JAMMA, G2 and sy code (8),
  and three on the remaining leftovers (stage collision and model
  compile; the NAOMI2 lights, console and loaders; the enemy AI, model
  drawing, tasks and input).
- **Totals**: 83 globals named (among them the cartridge, G1 DMA, Holly
  FIFO and SH-4 timer registers), 232 existing globals retyped (the enemy
  AI scripts, the demo and appose tables, the camera tables, the sound id
  table), 42 new types (stage collision records, list banks, the demo
  script and command records, the sound tables, six function-pointer
  typedefs for the routine tables) and 15 prototypes. Eight addresses
  that looked like globals were constants (request words and flag masks).
  `lights_elan_alt_colour*` became `*_color`; `level_third_sprites` was a
  name inside `level_table` and is gone. Tree-wide warnings fell from 933
  to 785 (the test program's stay at 7).
- **Found while merging**:
  - `cart_data_reg`, `g1_dma_status_reg` and `holly_fifo_status_reg` join
    volatile_globals.tsv (dimm_read_unlocked, g1_dma_spin_idle and its
    six SRAM wrappers, and the sy FIFO wait differ without it).
  - `frame_check_rec` was 0x70 bytes, 8 more than the frame-check tool
    uses (its furthest read is the defender side's word at +0x64); the
    new `texcol_body_lists` row at 0x0C3D7890 sat inside it and the export
    fell back to `undefined frame_check`. It is 0x68 bytes now.
  - The debug menu's ROB item (0x0C29283D) was a pointer in Ghidra's
    analysis, covering the two flag bytes after it; it is a `uchar` (the
    side mothead_sound_play watches).
  - A row's evidence held `*/`, which closed the export's comment and
    broke vf4ft_globals.h; the merge script now refuses such evidence.
  - msnm_disp.c read `coin_viewer_name_index` through a file-local alias
    the matcher could not resolve, so chlg_coin_layer_viewer had been
    `match*` (relocation unchecked) since wave 2; with the shared name it
    is a full match.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match, none `match*`; check_private_names 0 hits.
- **Next in the pass**: the records several groups share (player input,
  task records, fonts, `rob_get`'s return, the light record's +0x5C /
  +0x68 members, compiled model vs bounding sphere, tex slot vs surface,
  the strip context), `atoi` → `atol`, and the splits this wave found
  where an object's element or member form does not match (the ambient
  sound programs, the replay camera state and history, the rob-info
  window arrays, the light key values, the rain sheets, the challenge map
  high scores inside `user_back_data`, `rank_char_usage`).

## 2026-11-05: readability wave 5b, round 1: the records many files share

The records and prototypes that dozens of files share, which no
file-by-file agent could change: four agents, each owning one family and
every file in the round that uses it, then one merge. Tree-wide warnings
fell from 785 to 459 (most were casts between a shared record and the raw
types its users had pinned); readable functions 5,924 to 5,929.

- **rob_get** returns `rob_rec *`: 335 `(rob_rec *)` casts gone in 38
  files.
- **Player input**: `player_input_get` returns `player_input_rec *`, and
  about 85 byte-offset reads of its result (`*(uint *)(p + 0x14)`) are
  member reads (`p->now`, `->pressed`, `->repeated`). `ascii_font` is a
  `sprite_font` (its +0xC is the cell height: 16, 16, 24, 32, 16, 24 in
  the six fonts).
- **Tasks**: the 21 module task records are `task_rec` (they were
  `undefined`), the node lists `task_node_rec` arrays.
- **DIMM board info**: `dimm_board_info_fetch` returns
  `dimm_board_info_rec *`, now laid out in full (0x80 bytes: the id word,
  the feature bits at +0x1C that gate the extended commands, the
  addresses, a flat copy area).
- **atoi is atol**: renamed (atol.c), since its callers only match with
  a long return; test.bin's twin follows through the transfer table.
- **Models, texture slots, strip contexts**: `mdl_block` and
  `model_list_slot` were partial views of `compiled_model_rec` (bounding
  sphere at +8, four list slots) and are gone; `tex_slot_rec` starts with
  an embedded `surface_rec`; the six strip contexts and `draw_ctx_ptr`
  are `strip_context_rec`.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.
- **Next**: round 2, the object splits (ambient sound programs, replay
  camera state and history, rob-info windows, light key values, rain
  sheets, sand puffs, the challenge map high scores, `rank_char_usage`,
  the sy hook heads), the ELAN command blocks, the light record's +0x5C /
  +0x68 members, and the shared globals round 1 left pinned in files it
  did not own (`player_buttons`, the EUC text font, `dimm_busy_word`,
  `tex_slot_table`, the model holders).

## 2026-11-05: readability wave 5b, round 2a: object splits and the light records

Two agents: one held the ELAN command blocks, the light records and the
camera code; the other held the game objects whose parts the game loads
through literal-pool words of their own. Readable functions 5,929 to
5,940; tree-wide warnings 459 to 458.

- **Splits**: where the game loads part of an object through its own
  literal, the original code named that part as a separate global, so the
  object is split and the pieces tile the old extent:
  `ambient_program_ch0..3`; `rain_sheet0..4_offset` and `_pos`;
  `light_key_values[3]` + `light_key_spot[11]`; the rob-info debug
  windows' data words (`rob_info0_rob` / `rob_info1_rob`,
  `coli_info0_coli` / `coli_info1_coli`, `ctrl_info0_ctrl` /
  `ctrl_info1_ctrl`); the usage table as `rank_char_usage[13]`,
  `rank_dural_chara`, `rank_dural_usage`, `rank_char_usage_tail[2]`; the
  sy hook heads as `irq_hook_heads[144]`, `vblank_in_hook_list`,
  `irq_hook_list_910`, `hblank_hook_list`, `irq_hook_heads_upper[45]`.
  The usage-table and hook-head loops still index past their shortened
  arrays, as the bytes require.
- **Name-only aliases** where a split is impossible: the replay camera
  code loads 0x0C41F05C as the state base, so `replay_cam_state` stays and
  its bytes +4..+6 get aliases (`replay_cam_history_pos`,
  `replay_cam_last_eye`, `replay_cam_last_height`); `camera_near_clip`
  joins `pers` / `mode` / `cam_rob_no`. An alias inside a typed object
  must be a bare-name symbol row: a typed row there clears the container
  in Ghidra (the first export had `float camera`).
- **Sand**: `DAT_0c48b904` was `sand_puffs - 0x28` (the literal for
  `&sand_puffs[cursor - 1]`), not an object; `sand_dirty_cells` is
  `ushort[50]` and `sand_puffs` `sand_puff_rec[80]`.
- **Lights**: `light_sets` is `light_src_rec[6][16]` (`hit_light` was
  set 1 light 13, and that form matches, so its row is gone);
  `stage_light_table`, `light_table`, `light_anim_lights` and
  `nyc_beam_glows` have their record types; the +0x5C / +0x68 members
  were already right (view position, view direction).
- **ELAN blocks**: `elan_proj_cmd_rec` and `elan_tile_clip_cmd_rec`;
  `list_block_ref` and the light ELAN mode / color records are typed. The
  end, filler and backwards-written light blocks stay word writes (no
  record fits their write order).
- **Other**: `euc_text_font` is a `sprite_euc_font`; `player_buttons`
  `ushort[2]`; `control_context_funcs` `control_context_entry[8]`.
- **Not done**: `chlg_map_hiscores` (DAT_0c3769ac inside
  `user_back_data`): splitting the save record needs ten files outside
  the round.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.

## 2026-11-06: readability wave 5b, round 2b: the last shared globals

Four agents on disjoint files (a file two features share went to one of
them; the other wrote the exact edit, applied at the merge). Readable
functions 5,940 to 5,952: of the matched C, only `chlg_map_node_get`
still reads a `DAT_` (inside `user_back_data`, whose split needs the
save-record files). Tree-wide warnings 458 to 415.

- **DIMM link**: `dimm_busy_word` int, `xfer_recv_bounce` `char[8192]`
  (exactly up to the next global; transfers refuse spans over 0x2000);
  `dimm_cmd_args`, `ctl_call_block`, `peer_request_rec`,
  `peer_start_table_rec` laid out.
- **Texture slots**: `tex_slot_table` is `tex_slot_rec *`, `texidx_map`
  `texidx_map_entry *` (its `use_count` is a reference count), with the
  snapshot / bitmap globals typed.
- **ELAN pointers**: `elan_cmd_write_ptr` and `last_link_cmd` are
  `uint *`, the dummy triangle lists likewise; the ELAN list functions and
  the light block queues hold no undefined type any more;
  `screen_map_cmds_queue` builds `elan_proj_cmd_rec` blocks.
- **Models**: `obj_node.model` / `model2`, the hand, ground, grain and
  zanzou model holders are `compiled_model_rec`; `text_font_surface` is a
  `surface_rec`; `mtx_store_translation` takes `float *`; 20 prototypes
  take `compiled_model_rec *` (asm ones through their symbol rows).
  `make_osage_param_text` reads clean.
- **Chara and stage**: `chara_costume_swap` `char[20]`, the alt-model
  flags `char[2]`; `stage_rec.effect_list` `effect_slot *`,
  `static_heap_rec.addr` `void *`, `field_coli` `field_coli_rec *`;
  `load_poly` reads clean (`load_poly_obj` `obj_header *`,
  `poly_load_entry` typed).
- **Lights and text**: `light_set_ambient` `ambient_entry[6]` (its color
  is `float color[3]`), `rob_light_num` / `stg_light_num` short;
  `sprite_text_put` / `sprite_text_printf` take `sprite_euc_font *` (45
  casts gone); `msnm_work` is `msnm_work_rec *` (about 400 casts gone in
  msnm.c) and its `try` member is `cur_try`.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.
- **Left**: `chlg_map_hiscores` (a `user_back_data` split);
  `msnm_work_rec` +0xA34 as a union with the challenge result;
  `sel_chara_info` (16 entries of 88 bytes run over the globals after
  it); a few globals pinned identically in many files (`frame_parity`,
  `render_frames_in_flight`, `tile_split_word`, `blend_pair_bits`).

## 2026-11-06: readability wave 5b, round 3: the save record and the last pins

One agent on the 27 files that use `user_back_data`, `msnm_work` or
`sel_chara_info`, plus four fixes of mine. Readable functions 5,952 to
5,953: no matched C function reads a `DAT_` any more, and the six left
are asm. Tree-wide warnings 415 to 409.

- **Save record**: `user_back_data_rec` has no undefined member left;
  `bitflags_0c3744fc` is `option_bits`. It cannot be split into globals
  of its own: `chlg_map_node_get` only matches through a bare symbol at
  0x0c3769ac, while `msnm_course_result_store` only matches through the
  member. So `chlg_map_hiscores` is a second name for
  `user_back_data.challenge.chlg_map_hiscores` (a bare-name row; the
  export now types an alias from a member nested any depth, not only one
  level), and `msnm_disp.c` uses it (ORIGINAL SHAPE note).
- **Mission work**: `msnm_work_rec` +0xA34 is `union msnm_result_area`
  (`score_parts` or the challenge `chlg` result).
- **Select screen**: `sel_chara_info` is `sel_chara_info_rec[15]`
  (88-byte records that used to run over the globals after it). The 15
  layer-name tables inside the records keep their enums, but ApplyTypes
  now drops their placeholder `e_names_*_names` labels inside a curated
  object, so they are no globals of their own.
- **Pins**: `frame_parity` int, `render_frames_in_flight`,
  `tile_split_word`, `blend_pair_bits` uint; 21 identical pins in 20 files
  dropped.
- **Collision hits**: `coli_segment_hit` / `coli_sweep_hit` return
  `coli_poly_rec *`; their callers in `rob_coli.c` read members and
  `field_floor_height` takes `coli_poly_rec *poly_out` (a struct copy).
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.

## 2026-11-06: readability round 4: the matrix API and the warning sort

The tree's 409 compiler warnings, sorted: 186 are the implicit-declaration
pairs the `NOPROTO_` shapes need (168 in `effect_etc.c`; they stay), about
50 came from the matrix routines taking a matrix as `int`, `uint *`,
`undefined4` or `float *` by turn, 16 from the osage parameter tables'
type, and the rest are one-offs (a decompiler cast at a call, an `int`
parameter that is a pointer). This round fixed the first two groups and
the round 2b leftovers. Warnings 409 to 320, of which 194 are the deliberate implicit-declaration
ones.

- **Matrix API**: every matrix the matrix library takes or returns is a
  `float *` (`mtx_store*`, `mtx_set_*`, `mtx_push`, `mtx_mul_by*`,
  `mtx_premul_*`, `xmtrx_to_*` / `xmtrx_from_*`, `mtx_inverse_lu`,
  `mtx_lu_pivoted`, `mtx_overwrite_translation`); asm ones through their
  rows (checked against the asm: `mtx_premul_two` reads two matrices,
  `mtx_stack_drop_in` a stack record). `mtx_stack_rec` holds `float *`
  base and top, and `mtx_stack_records` is `{active, main, view, proj}`
  (`.view_top` reads `.view.top`). 252 casts are gone and 72 written (mostly `(float *)` at block views),
  180 fewer in all;
  the int matrix locals of the rob, osage, zanzou and motion code are
  `float *` stepped by 16 floats (`rob->mtx + bone * 0x10`), the bone
  eval records' matrix pointers load as `float *`; struct-copy views
  (`mtx_t`, `blk`, ORIGINAL SHAPE) pass as `(float *)`.
  `proj_x_model_matrix` is `float[16]`, `osage_node.mtx` `float *`,
  `shun_item_hand_offset` `float[3]`. `mot_tools.c` 71 warnings to 0,
  `rob_disp.c` 29 to 0.
- **Osage tables**: the 16 `osage_alt_params_*` are `osage_param[64]`
  (the bounded ones are exactly 64 records; four have a tail noted in
  their rows); `osage.c` 16 warnings to 0.
- **Display lists**: the ELAN `{addr, length}` records are
  `list_block_ref`: `model_list_link_queue` / `model_volume_link_queue`
  take one, `dummy_tri_list_build` builds one and the `dummy_tri_*`
  globals point to them, the model_draw callers and the three
  `effect_etc.c` block writers keep a `list_block_ref` local.
  `model_part0_call_write_at` takes and returns the write address as
  `uint` like its sibling `model_matrix_cmds_write_at`.
- **Small ones**: `txn_list_current` is `txn_entry *` (three pins and
  their casts gone); `dimm_cmd_post` names the DIMM board's registers
  (file-local defines, as `libam.c` and `http.c` do).
- **Left**: `j_ex_00` as `osage_param[2][100]` (384 debug-menu labels sit
  inside it, so typing it needs a decision on those aliases);
  `elan_write_ptr_get` / `_set` stay `uint` (all three callers do
  address arithmetic on the value); the remaining one-off warnings.
- **Checked**: `ghidra/run.sh all` and `test`; 6,254 of 6,254 and 119 of
  119 match; check_private_names 0 hits.

## 2026-11-06: readability round 5: j_ex_00 typed, the one-off warnings

Round 4's two leftovers. Warnings 320 to 199. Of the 320, 190 were the
implicit-declaration pairs the `NOPROTO_` shapes need and 6 the in-function
`extern` declarations (ORIGINAL SHAPE); both groups stay. Of the other 124,
121 are fixed: 70 by root types (records, fields, prototypes) and by dropping
stale decompiler casts, 51 by writing the int-to-pointer conversion where an
API keeps addresses as words. 3 stay.

- **j_ex_00**: the osage parameter table is `osage_param j_ex_00[2][100]`
  (100 records per rob, 2,400 bytes a rob). The debug menu edits it from its
  first byte (`*j_ex_00`) and labels each field (`*j_ex_01`, `*j_ex_00_y` and so on):
  383 labels that were globals of their own. `ApplyTypes` now drops a
  debug-menu label (an absolute target in `ghidra/debug_menu.tsv`) strictly
  inside a typed data row when the row's own address is a menu target and
  no symbol row keeps the name, and logs each one. Inside the other typed
  objects the menu reaches into (`camera`, `robs`, `card_images`,
  `stage_wind`, `user_back_data`, `motion_test`; 73 labels) a menu label
  names a member and stays as its alias. `rob_disp.c` indexes the table by
  side (`j_ex_00[side]`); the int parameter arithmetic stays.
- **Duplicate record types gone**: `msn_mission_rec` was `msnm_try_rec`
  (its `+0x3c` is now `special`), `msn_counters_rec` was
  `msnm_try_setting_rec`, `chlg_route_rec` was `msnm_course_results_rec`,
  and `poly_test_camera` is a `camera_state` (`poly_test_camera_rec` was its
  layout twin). `msnm_try_hyper_pick` and `msnm_input_stats_add` take their
  records and read them by field; `chlg_route_cursor_set` / `_preview_reset`
  take the try and setting records.
- **Fields and prototypes**: the background plane and sand puff colors are
  `uint`; `frame_surface_pair` holds `surface_rec *` and the render
  configuration points at it; `stage_anim_rec` channels / links are
  `mot_channel *` / `int *`; `stage_sound_record_default` is a
  `stage_sound_rec`; `mtx_stack_setup` / `mtx_stacks_start` take a `float *`
  buffer; `vsnprintf` a `const` format; the gz heap callbacks have zlib's
  `alloc_func` / `free_func` shapes; `localtime_r` has newlib's prototype;
  `level_result_0_9` takes `uint` with `uchar` locals like its two siblings;
  the net info locals are `net_info_t *`.
- **Casts written**: libsnd's image (`(int)sound_pack_header`), the vertex
  buffer descriptor and transfer queue words, the texture data word, the
  bone block word, the IRQ slot word, the sprite page and key blocks, the
  elan block copy, the palette group table, as the sibling callers already
  write them. On the changed lines, casts 80 to 72. `render_stats_read` and
  `list_seg_append_padded` keep their int parameters (a pointer one changes
  `render_stats_read`'s bytes; five other callers pass the address as an int).
- **Left**: `http_ring_next`'s `const` function pointer (ORIGINAL SHAPE);
  `rob_debug_reload` calls `rob_items_setup` (int parameters) and
  `rob_items_from_part_test` (char parameters) through one pointer;
  `inflate`'s `adler32` (the export drops `const` from `check_func`).
- **Checked**: `ghidra/run.sh all` and `test`; the export reproduces the
  edited headers (0 real changes; against the last export 3 retypes and the
  383 `j_ex_*` labels gone); 6,254 of 6,254 and 119 of 119 match;
  check_private_names 0 hits; 0 C functions unreadable; overlaps 102.

## 2026-11-06: live checks in Flycast: blend, eye look-at, wind

The maintainer played on the GDB build of Flycast while `tools/flycast_live.py`
watched through its GDB server (breakpoints that log and continue, or one
that halts and saves the 32 MB of RAM; memory reads only).

- **Motion-change blend**: checked. Two RAM images mid-blend (Kage's
  stance into his jab, weights 0.2 and 0.4): `rig.pose_passes` within 1e-4
  of all 31 matrices at the stored weight, with the head look-at running.
- **Found: the held root offset.** The game keeps the previous pose with
  its root x / z and root offset when that move's state word has bit 0x10
  (the stance's flags 0x1018); `pose_passes` always cleared them, 4 cm off.
  `rig.pose_passes(held=)` and `pose.py --from` (from the previous move's
  flags) now follow it: our own keep matches the game's captured one to
  5e-7 (root) and 6e-5 (positions), the blend end to end within 1.5e-4.
- **Found: rob +0x428 is overwritten.** The look-at point is the held
  target +0x434 placed in the fighter's frame; +0x428 ends the frame as the
  clamped point in the neck's frame (asset-formats.md).
- **Eye look-at**: checked. Sarah's `SARA_SYORI_A`, frames 1 and 2:
  `face.eye_look` gives the game's angles (exact, and one 16-bit step off in
  pitch), with the head as the slot-3 matrix (rob +0x54). Only three moves
  use timed command 41.
- **Stage wind**: checked in two YAM save states: the record is the table
  row with waves 2-7 cleared, the vector follows from the stored phases.
- **Hit results**: not yet. The session logged 3 plain hits (0x1001); the
  counter, 0x800 and 0x4000-0x10000 cases need a session that plays them.
- **Move words**: 951 requests, bit 15 never set; lever bytes on action
  types 1, 2, 3, 9. Not enough to name the flags.
- **tools/flycast_live.py**: `hits` mode logs `attack_hit_resolve`,
  `attack_recovery_apply` and `rob_action_start` as JSON lines; `snap`
  mode halts on a blend or an eye look-at and saves RAM; `clear` removes
  its breakpoints after a dropped client. A killed gdb leaves Flycast's
  breakpoints in (its server neither removes them nor resumes on a
  dropped socket) and gdb in batch mode ignores SIGINT; SIGTERM ends gdb
  with the target running and can leave them in, so `clear` follows every
  stop.

## 2026-11-06: hit results checked live, CPU against CPU

Playing the hit situations on the slow GDB build was not practical, and the
attract-mode fights are replays (no hit is resolved in 10 minutes of them).
The game's own debug switch `cpu_on_off` (0x0C2926F0, 4 = BOTH ON) makes
the AI drive both fighters; `tools/flycast_live.py` cpu mode writes it.

- 239 hits over nine CPU-vs-CPU pairings (VS mode): every result word
  `attack_hit_resolve` stored matches what the matched C gives for the
  logged defender (`tools/flycast_hitcheck.py`, 0 mismatches): plain 0x1001
  (70), counter rows 0 / 1 / 2 (0x4003 16, 0x8003 52, 0x10003 5), the
  defender-state 0x801 (39), airborne 0x1005 (55), and both together
  (0x805, 0x8007). The row boundaries hold at 15
  and 25, and the row follows the defender's own attack damage.
- Guards 0x20 (60) and guard knockbacks 0x80 (2) through
  `attack_recovery_apply`'s other callers.
- `VF4LIVE_WORDS=0` drops the per-frame `rob_action_start` breakpoint,
  which slowed the interpreter to about 6 frames a second.
- Not checked: the recovery end `attack_recovery_apply` then sets (the log
  has the rows, not the frame after); the move-word flags.

## 2026-11-06: the move-word flags

- The bits above the motion id in a script's move word are the action
  word's lever byte (16-23) and action type (24-31), the same layout as
  `rob_action_rec`: `|0x500` is a type 5 throw, `|0xe00` type 14.
- Checked live: 2,708 requests in CPU-vs-CPU fights (tools/flycast_live.py,
  its request breakpoint now on `action_reaction_record_reset`, which
  `rob_action_start` calls only with a request pending, so the game runs
  at speed) and 951 from the earlier session. The requests carry the
  script's type and lever; a preset throw gains lever bit 0x08
  (`move_word | 0x80000` in coli_attack.c), seen on both the thrower
  (type 5) and the thrown (type 6). Bit 15 never set.
- The 19 hits of the same run match (tools/flycast_hitcheck.py).

## 2026-11-06: the stage wind in the physics

- `tools/physics.py` emulates the stage wind (`physics.Wind`): the stage's
  `stage_wind_table` row with waves 2-7 cleared, stepped once a game frame
  as `wind_update` does, added to every swing-bone joint (with the reset's
  settling steps) and every cloth particle beside the velocity, as
  `osage_node_calc` and `cloth_node_integrate` do. It was taken as zero.
- `pose.py` and `gltf.py` take `--wind STAGE [FRAME [HEADING]]`: FRAME game
  frames into the swing (the phases run on from boot), HEADING the
  fighter's rob +0x0C, which turns the world-space force into the fighter's
  frame (the only term of the simulation that frame changes).
- At its peak NYC, CAS and YAM blow 0.0059 a frame, twice the gravity:
  Sarah's ponytail moves up to 0.6 in her stance, Aoi's hair 0.16. The
  glTF with `--wind 1 0 -8192` re-skins within 1e-6 of pose.py's OBJ
  with the same wind (gltf_check.py; 0.375 from the calm one).
- Not checked live frame by frame (the matched C is the source); the wind
  record and vector were checked live earlier.
- Corrected: Jeffry's `gblctr` miss (0.065, 2026-11-05) was put down to a
  terrain floor lift without evidence. The only lift in the passes is the
  attack reach (`rob_eval_matrix`, `t_hara`: `reach.current[0] x
  reach.out`, plus `chara_data` +0x24 while reach flag 4), zero in all
  eight states and RAM images on hand, and the terrain height is the
  placement's y. The Jeffry state was overwritten, so the cause stays
  open.

## 2026-11-06: the height lift (Jeffry's 0.065)

- Jeffry's `gblctr` miss is his character record's height offset (+0x24,
  0.065): `rob_eval_matrix` moves `t_hara` up by it while reach flag 4 is
  on, which `rob_attack_reach` keeps on for WOLF (0.05), JEFFRY and, in a
  fight, PAI, AOI and SHUN (-0.02 to -0.06). For the negative ones
  `rob_eval_output` raises the thigh IK aims by as much (SHUN, AOI adjust
  further) so the feet stay down.
- `rig.pose_passes(lift=)`, `rig.lift_on`; `pose.py` and `gltf.py` apply it
  (`--no-lift` for the states that clear the flag).
- Checked live in three new save states, Jeffry against Aoi: Jeffry's
  `K_UFRYARAKO` 6.7e-5 (0.065 without), Aoi's `AOI_PU_01_VFX` 1.2e-4
  (0.175 without), Aoi with the flag off 8.6e-5. The Jeffry glTF re-skins
  within 9e-7 of pose.py.
- Open: Jeffry in the guard `AKI_HIKINA_GU` with the attack reach running
  misses by 0.16 at the legs; the reach covers the `gblctr` part, not
  the legs. Motion flag 0x80, also set there, is not read by the posing
  code.
- Fixed the symbol note of `rob_eval_byte_flag`: it gates the thigh aims,
  not the arms.

## 2026-11-06: the attack reach (the guard leg miss)

- The open Jeffry miss (0.16 at the legs in the guard `AKI_HIKINA_GU`) is
  the attack reach: `rob_eval_output` raises the thigh and shoulder IK
  aims by `reach.current[1..4] x reach.out`, and `rob_eval_matrix` the
  hips by `current[0] x out`. The right leg was near full stretch, so a
  0.0126 aim turned the thigh 0.18.
- `rig.pose_passes(reach=(current, out))`; `pose.py` and `gltf.py` take
  `--reach C0 C1 C2 C3 C4 OUT`. With the state's reach the guard matches
  within 1e-4, and every save state on hand matches within 2e-4 but the
  look-at and blend cases the plain check does not pass and JDK's
  `VFX_DD_121_N` (0.027 at the legs, no reach or limb target).
- JDK's 0.027 is not a posing error: both matrix buffers hold motion frame
  34 exactly (9e-5, every IK target and angle) while the frame counter
  reads 31, just wrapped from 61 (the snapshot ring shows 58-61).

## 2026-11-07: pose-build time traced live

- `tools/flycast_live.py` trace mode logs every `rob_pose_eval` (the time
  argument, fr4, beside the rob's frame, frame position, speed, move,
  state and snapshot ring) and every `rob_move_change`, without halting.
- Goh against Kage, CPU against CPU, 1,446 pose builds: every pose is built
  at `frame_pos / 30` (one frame less in `rob_eval`), as the tools assume.
  Move 317 (`VFX_DD_121_N`) is Goh's round-start pose, a 61-frame loop that
  wraps 61 to 1.
- So JDK's saved state is the odd one out (counter 31, ring 61, pose 34);
  its `game_flags` 0x10CA9 differ from every fight state's (0x110005 /
  0x10005): saved outside a fight. Nothing to change in the tools.
- Not emulated: the reach's limb targets (`reach.limbs`).
- The guard's glTF with the same `--reach` re-skins within 9.3e-7 of
  pose.py.

## 2026-11-07: item swing-bone parameters from chara_item_table

- `physics.item_records` scanned 0x0C2B0000-0x0C2DE000 for item records
  and kept only those whose swing table lay at 0x0C2DE000 or above: 133
  models. The game takes its records from `chara_item_table` (0x0C2DD3D8),
  which names 1,287 model items (kind 1); every scanned record is among
  them. Records the scan missed include Aoi's 0x2A0001 / 0x2B0001,
  Sarah's 0x580001 / 0x590001 and JDK's hat 0x620001 (its list at
  0x0C2D846C), whose chains ran with zero parameters.
- `physics.item_spec` picks the table as `rob_osage_items_init` does: flag
  bit 1 or no table, dense (no table: the debug edit table 0x0C2DD868,
  stiffness 0.02); else a sparse list, which now writes only the entries
  it names (used or not) and keeps the others.
- Checked against a fight's RAM: JDK's (costume 1) three hat parameters,
  zero before, now equal the game's j_ex_30-32; Kage, Sarah: equal.
  Over the 65 sets, all-zero parameters in use fell in nine (Shun 15 to
  6, Wolf 19 to 4, Black 17 to 0).
- New: `tools/physics_live.py` (one step of `physics.py` from the game's
  own chain and cloth state against the next frame) and phys mode in
  `tools/flycast_live.py` (RAM on consecutive frames at `osage_calc_all`).

## 2026-11-07: swing bones and cloth checked live, cloth step fixed

- Four consecutive frames of RAM at `osage_calc_all` (Lau vs Pai, both on
  CPU, a windy stage); `tools/physics_live.py` runs one `physics.py` step
  from each frame's game state and compares with the next.
- Wind: equal to 1e-10. Chains: all parameters equal; tips within 2.2e-5
  (48 of 54 steps within 1e-6; the others look like a one-unit angle
  difference in the game's atan, truncation being the best fit).
- Cloth missed by up to 1.1e-2. Particle-by-particle against the game
  (neighbours from the game's frames) found four differences, from the
  assembly integrator `cloth_node_integrate_asm` the game runs
  (`cloth_asm_flags` 3), not the C twin: links below, right, left, above
  (we did left first); each link sags as the chains' do (`pull`); the
  force direction goes through the last anchor's matrix; no sideways add
  (no table sets one). Fixed in `Cloth.step`: all 378 particles within
  9.2e-7.
- Ruled out on the way: `coli_list_push_core` saves and restores the XF
  bank, so the collision push leaves XMTRX alone.

## 2026-11-07: the chain residual

- `physics.atan2s` is now the game's `atan2_s16` (0x0C0F1B40: the octant
  test, the rational `atan_kernel` in float32, truncation to s16). The
  four captured frames give the same results as with the exact atan.
- The game's stored aim angles (node +0x28 / +0x2A) against ours: the 6
  chain steps that miss start at one joint whose truncated angle is one
  unit off and carry down the chain (float32 matrices with `fsca` in the
  game, float64 in the tools). Matching it would mean emulating both;
  the gain is 2e-5, so it stays.
- Quirk: `osage_node_rotate` packs `y << 16 | z` with z sign-extended, so
  the stored y angle reads -1 whenever z is negative. Only the z angle is
  read anywhere (osage_calc_all, JDK's hat).

## 2026-11-07: live-checked names promoted

- The rule (below Next) lets a runtime check confirm a name that has no
  string evidence. 31 symbols whose behaviour the Flycast checks of
  2026-11-05 to 2026-11-07 compared against the game are now `named`, the
  check appended to their evidence, in ghidra/symbols/*.tsv and in the
  src/ headers alike:
  - the chains, wind and cloth (phys mode, tools/physics_live.py):
    osage_calc_all, wind_update, stage_wind, osage_wind, stage_wind_table,
    osage_chain_calc, osage_node_calc, osage_node_rotate, atan2_s16,
    atan_kernel, cloth_calc_all, cloth_step, cloth_node_move,
    cloth_node_integrate_asm, link_constrain_core, cloth_asm_flags,
    cloth_slots, osage_chains, j_ex_00, chara_item_table,
    rob_osage_items_init;
  - the hits, requests and pose builds: attack_hit_resolve,
    rob_action_start, rob_pose_eval, rob_move_change;
  - the eyes: move_tcmd_41_eye_look, eyes_look_at;
  - the posing passes in the save states: rob_eval_ik, rob_eval_matrix,
    rob_eval_output, rob_attack_reach.
- Left inferred: what a check only passed through (attack_recovery_apply,
  whose recovery end was not logged; action_reaction_record_reset, used
  as a breakpoint only; the blend routines, matched as a whole by
  rig.pose_passes, not one by one); cpu_on_off has no symbol row.

## 2026-11-07: the 12 parked functions re-measured after the readability passes

- Every stored text (413) carried over to today's names
  (build/agents/rename_texts.py) and rebuilt. Where a carried text drifted,
  the cause was the carry-over, not the code: `stage_screens` is now
  `stage_screen_slot[2]`, so the old `stage_screens + n * 0x12` stepped
  records instead of 72 bytes (3 lines became 40); `bitflags_0c3744fc`
  became `user_back_data.option_bits` and the tool picked the debug menu's
  global at that address; aet_still_quad_retransform's first parameter is
  now `aet_still_rec *`. Ported by hand, they measure as before.
- break_floor_model_build (12) and prize_fly_draw (124) measure the same in
  a worktree of the 11-04 tree (58dd8b7) as today; the 10 and 130 in the
  parking table came from older texts.
- So no header change since 11-04 moved any of the 12, and they stay
  parked. aet_still_quad_retransform is still 2 with the typed
  `param_1->layer->item.image` form (the parameter copy lands before the
  `d` initialisation instead of after it).

## 2026-11-07: a new round on the parked functions; two adopted

Nine agents, one per function, the nine within 9 lines (texts and notes in
the session scratchpad, round/<function>/; the measuring copies of
rerun_texts.py report a match as COMPILE-FAIL, since chk.py -v names a
function only when it differs: fixed there, check matches with chk.py).

- **Adopted, 2 (6,256 of 6,266):**
  - **attack_connect** (0x0c06ebc0): the history store `attacker->attack_history[c]`
    through the typed member. The cause was regclass, not local-alloc:
    indexing the typed member marks the register holding 0x252 as a
    pointer, so the c * 2 + p sum takes r0 as the index; with plain char
    offsets the priority tie gave r0 to the constant. The prototype is now
    `(rob_rec *attacker, rob_rec *defender)` (the symbol row), and the ten
    calls in robs_hits_resolve drop their `(int)` / `(byte *)` casts.
  - **vram_used_total_update** (0x0c0dfea0): the running total stored to
    sprite_vram_used_total on every pass (the r7 pseudo the game's loop
    registers skip is that store's shadow register in loop.c), summed in an
    inner `{ int *used = sprite_set_vram_used; ... }` block, the shape
    sprite_sets_init and sprite_set_load use; tagged ORIGINAL SHAPE. The
    block's end note stops loop.c's try_swap_copy_prop (16 lines without
    it).
- **Matched, adopted by the maintainer's ruling (ORIGINAL SHAPE, 6,258 of 6,266):**
  - **elan_lights_queue**: matches with `int first = 0;` as the start of
    both light loops (`i = first;` in the if block). The zero set outside
    the CSE block keeps dl.length's zero off i's register, and reload
    rematerializes `mov #0,r8`. No matched sibling has such a start index.
    The natural sibling-shape text without it is 2 lines (round/
    elan_lights_queue/t10_0c0fc220.c, also without the old lo/am trick).
  - **win_item_row_draw**: matches with `names = 0; break;` in case 15
    (`mode > 3`): a store nothing reads, which keeps the branch alive
    through cse1 so the +24 byte is re-read; flow deletes it later. The
    natural else-if form is 5 lines.
- **Not moved:** aet_still_quad_retransform 2 (CSE and local-alloc both tie
  the parameter copy; only a mode that cannot tie avoids it; t09 is the
  readable reference text), stage_extras_setup 3 (needs a CSE flush between
  `done = 0` and the side-0 store; every natural label is gone before cse1),
  sprite_clip_push 3 (combine keeps the exts.b unless the count is re-read
  through a pointer with two sets), move_tcmd_33_slot_effect 8 (reload's
  round-robin; the typed text t01 is 12), cfg_setting_or_default 9 (a
  vanished q = p copy, reload_cse_move2add; the helper-free text t17 is 9).
- **`-g` tried:** all 6,254 matched functions still match with `-g`, and it
  changes one open function: the plain store-in-loop
  vram_used_total_update matches with it (16 without). No other text of
  the 451 measured changes. One case does not settle the build flags; a
  lead if another function turns up a line-note difference.
- break_floor_model_build (12), render_hang_recover (78) and prize_fly_draw
  (124) were not in this round.

## 2026-11-07: round 2 on the open functions; render_hang_recover adopted

Seven agents, one per function: the three not in round 1 and the four
whose round-1 agents left a lead (aet_still_quad_retransform left out: no
plain copy avoids the tie). Texts and scripts in the session scratchpad,
round2/<function>/.

- **render_hang_recover adopted by the maintainer's ruling (78 lines to a match;
  6,259 of 6,266).** Three of its prefetches take a copy of the address,
  `{ void *a = (void *)p; __asm__ __volatile__ ("pref @%0" : : "r" (a)); }`,
  the form elan_commands.c and transfer_queue.c use (an SDK prefetch macro
  would expand so); elan_status.c's other functions pass p directly, which
  gives 84 here. Tagged ORIGINAL SHAPE. Its other shapes follow its file:
  a second `banks` pointer to list_banks held across the transfer loop
  and a block-local table pointer for the job-count decrement (as
  elan_status_service), a block-local VRAM base for the two fill stores
  (indexed store @(r0,r11); folded, 18). Its test.bin copy (0x0c07abe0)
  is closed too (2026-11-09 entry below).
- **Closer, not matched:**
  - break_floor_model_build 12 to 8: a block-scope
    `extern void mtx_rot_y_s16(uint angle);`, as nyc_searchlight_draw and
    nyc_light_draw declare it (effect_etc.c defines NOPROTO for it), puts
    the call address in r1. The 8 left are the hoisted constants 1 and 0
    trading r4 and r5: both get global-alloc priority 1 and the tie goes
    to the lower allocno; two more insns in the j-loop at lreg that are
    gone by final would break it (diagnostic diag01 matches that way).
    round2/break_floor_model_build/t01_0c0b8620.c.
  - sprite_clip_push 3 to 2 with `uchar c;` and `sprite_clip_count++;`
    (no `(char)c` cast): only the game's exts.b against our extu.b is
    left. The game sign-extended a QI register loaded on its own, a chain
    combine cannot fold; none of the GCC sources of such a register fits
    straight-line code.
  - stage_extras_setup stays 3 but no longer needs the declined inline
    helper: `frames = user_back_data._1p_match_point * 7200; lim = frames +
    7200;` (one expression: 17), with the per-side records typed in the
    text (`stage_screen_tex_rec DAT_0c2a2618[2][2]`, two
    `stage_screen_desc [2]` lists). round2/stage_extras_setup/t23.
- **Not moved:** move_tcmd_33_slot_effect 8 (an instrumented cc1 shows
  only two pick patterns; the game's r0 r0 r7 needs a register-occupying
  pseudo in each arm that leaves no trace), cfg_setting_or_default 9
  (CSE's output is the game's shape; combine must not merge the pointer
  steps, and a block-local pseudo must hold r2), prize_fly_draw 124
  (global-alloc priorities put y first whatever the case code; the agent
  suggests checking whether any matched function shows global.c or
  local-alloc.c choosing out of stock 3.2.3 order, which would point at a
  different compiler build).

## 2026-11-09: render_hang_recover's test.bin copy closed

test.bin's render_hang_recover (0x0c07abe0) builds from main.bin's C
unchanged, ORIGINAL SHAPE prefetch copies included, and test.bin now
matches 120 of 120 functions. It failed only on types: test.bin had the names from the main.bin
transfer but declared them undefined4. ghidra/symbols_test/kamui.tsv now
types seven of the globals the function reads as main.bin does (gfx_state,
xfer_pending[3], xfer_current, list_kind_sent, bank_render_index,
list_end_block_count, tile_split_word), and adds blank_end_block at
0x0c344a08, read from the function's literal pool (bank_render_index + 4,
the same layout as main.bin). No other test.bin source reads these
globals. The C is in source_test/sdk/kamui/elan_status.c. The only test.bin
function still open is cfg_setting_or_default's twin (0x0c0714c0).

## 2026-11-09: the compiler check: stock 3.2.3's float allocation holds

The round-2 lead for prize_fly_draw was a compiler build whose global.c
orders allocnos differently from stock GCC 3.2.3. It does not hold up.

- **prize_fly_draw's mismatch.** Three floats live across the calls: y
  (41 uses, live 426 insns), h (13, 490) and x (the 337.0, 9 uses, 594).
  Stock priorities are y 948, h 570, x 282, so global.c takes y, h, x and
  gives them fr13, fr14, fr15. The game has x in fr13 and y in fr15: the
  full reverse, which is also declaration order and longest-live-first.
  Any compiler that produced it would have to reverse gaps of 1.66x and
  2x.
- **Scan.** main.bin has 285 functions that keep two to four floats in
  fr12-fr15 (the prologue pushes; build/agents/compiler_check/fscan.py).
  The 250 of them in source/ (51 files) were compiled with `-dl -df -dg`
  (fgreg.py, output fgreg2.out, summary fan.py):
  - In 125 matched functions global.c's order alone sets which of fr12-fr15
    each float gets. In 94 of them the order is not declaration order.
  - 79 adjacent pairs in 72 of those functions have a stock priority gap of
    1.66x or more, and every one keeps stock order.
  - 99 of the functions give a longer-lived float the later register, so
    longest-live-first is ruled out.
  - 38 of the functions are in console.c, prize_fly_draw's own file:
    prize_trail_sparks_draw, prize_plate_draw, prize_bar_draw and
    round_marks_draw among them. round_marks_draw allocates a busy float
    (live 110) before a long-lived one with few uses (live 454), the case
    prize_fly_draw would need reversed. It matches.
- **Result.** The game's compiler ranks floats as stock 3.2.3 does. The 124
  lines come from prize_fly_draw's text: in the game's source y has a lower
  priority than x, through fewer or less frequent uses or a shorter
  reach, in a form not yet found. The remaining 7 main.bin functions and
  test.bin's cfg_setting_or_default copy stay parked with their traced
  causes (the 2026-11-07 entry). Scan dumps are written to the current
  directory by GCC; fgreg.py runs each compile in its own scratch
  directory for that reason.

## 2026-11-09: prize_fly_draw: the text is right except for three allocation orders

One focused round on the lead from the compiler check. Text:
build/agents/prize_fly_draw_r3/start.c (130 lines with stock GCC).

- **The proof.** A diagnostic copy of GCC 3.2.3
  (build/toolchain/work/obj-pfd, never used for matching; patched sources
  saved as global_pfd.c and local_alloc_pfd.c) can reorder named pseudos
  after the priority sort. With three orders forced, match.py reports
  prize_fly_draw byte-exact (1152 bytes), so the text produces every
  instruction already:
  - global.c: x, h, y (161, 162, 163) instead of stock y, h, x. This alone
    takes 130 lines to 32. It gives x fr13, h fr14, y fr15.
  - local-alloc, case 4: prize_fly_land_frame's address, then
    prize_fly_frame's, then sin_radians' (r8, r9, r10). Stock puts
    sin_radians first (priority 125 against 121 and 117; it would lose
    with a span of 24 insns instead of 22). This takes 32 to 16.
  - local-alloc, trail loop: (float)k before the 0.1 constant in
    `u = 1.0 - (float)k * 0.1`. CORRECTION (round 4, 2026-11-09): not
    needed. The first local-alloc override matched quantities through a
    stale reg_qty and moved other blocks; with that fixed, GA_ORDER plus
    LA_ORDER alone give a match, and stock already gives the loop's
    registers. Only two decisions differ. The 6 lines diff.py prints at the
    end are where its window stops: the literal pools are the same word for
    word.
- **Why stock does not get there with this text.** Global priority is
  floor_log2(refs) x frequency / live length. y has 41 refs; its refs
  outside the switch alone give it about 516. h's ref inside the trail
  loop (frequency 5000) gives it at least about 500. x, with 9 refs, can
  reach about 500 at most. So no text where x, h and y are each one
  variable with these refs puts x first. The block frequencies cannot
  move it: `if (trail)` compares with zero (no prediction) and the switch
  splits evenly.
- **Tried without effect** (stock): y split into a per-case temporary
  (130, 156 when all cases), y initialised later or after h (130), a
  separate drawn-position variable (132), six case-4 line layouts (no
  change: local-alloc counts insns, not lines), six spellings of the
  trail alpha (no change). Flags: -fno-guess-branch-probability gives
  224; -fno-thread-jumps, -fno-defer-pop, -fno-cprop-registers,
  -fno-omit-frame-pointer change nothing.
- **What would change it.** Each order depends on refs, block frequency
  and live span counted in the RTL before reload. The original must have
  had RTL that differs from ours there but compiles to the same
  instructions: for example copies between variables that end up in one
  register (reload then deletes them), which add refs or length. A text
  that does this for x, or for the case-4 addresses, without a declined
  trick is the open question. Parked again on that question.

## 2026-11-09: round 4 on the open functions; break_floor_model_build adopted

Seven agents, one per open function, with the diagnostic compiler.
Texts, dumps and scripts in build/agents/r4/<function>/ (gitignored);
the shared brief is build/agents/r4/BRIEF.md.

- **break_floor_model_build adopted by the maintainer's ruling (6,260 of 6,266).**
  The j-loop switch moves the pointer in each case (`*d = 0; d++;
  break;`) instead of one `d++` after the switch. Post-reload crossjumping
  merges those tails again, so the code is the same, but at local-alloc
  the two extra increments make the hoisted constant 0 live 102 insns
  (priority 0, the constant 1 stays 1), which puts -31, 1, 0 in r6, r5, r4
  as the game has them. Before, both constants had priority 1 and the
  lower allocno won (forced with GA_ORDER=379,374 it matched, confirming
  that tie was the whole difference). Note: 3.2.3's
  allocate_reg_life_data resets REG_N_REFS and REG_LIVE_LENGTH but not
  REG_FREQ, so frequencies add up over two life passes. The block-scope
  `extern void mtx_rot_y_s16(uint angle);` is tagged ORIGINAL SHAPE (the
  siblings nyc_searchlight_draw and nyc_light_draw declare it so).
- **stage_extras_setup 3 to 2:** `done = 0;` moved before the stage chain
  gives the side-0 store its own `mov #0`. What is left is cse1 reusing
  done's zero for the dural_alt_active store: the game has a label between
  them, which only the declined side-argument helper reproduces.
- **Not moved, causes pinned:**
  - aet_still_quad_retransform 2: cse1 (make_regs_eqv keeps the parameter
    canonical) and then local-alloc's combine_regs both fold the
    parameter copy; not an allocation order. A `long long` copy (declined)
    matches.
  - sprite_clip_push 2: combine re-simplifies any sign extension of a
    promoted char; the game's exts.b survives only from an unpromoted QI
    register (the declined struct wrapper matches). No other function in
    main.bin has this load/exts.b shape.
  - move_tcmd_33_slot_effect 8: reload's spill-register rotation
    (allocate_reload_reg) starts at r0 after the casesi dispatch; the
    game's r0, r0, r7 needs extra reload allocations in the arms that
    leave no code. Argued from dumps and reload1.c, not forced.
  - cfg_setting_or_default 9 (and its test.bin copy): combine merges the
    three pointer steps; the game also needs a vanished r2 quantity with
    reload_cse_move2add's constant case. 1,300 generated variants, best 9.
    A helper-free 9-line text is gen2/h0072.c.
  - prize_fly_draw 130: as above, two forced orders give a match; no text
    found that makes stock global.c allocate x first (h's trail-loop use
    and x's 9 refs alone rule it out). Unexplained against the compiler
    check.

## 2026-11-09: sprite_clip_push matched by retyping sprite_clip_rects (6,261 of 6,266)

Round 5 (build/agents/r5/sprite_clip_push/). The fix is the global's
type, not the function: sprite_clip_rects is 20 six-byte records
(sprite_clip_rect: x0, y0, x1, y1, mode, hdr), not char [20][6].
- With a record, the store `sprite_clip_rects[sprite_clip_count].x0 = x0`
  expands its offset in address context: the count is loaded into a QI
  register and sign-extended on its own (the game's exts.b), CSE cannot
  reuse the compare's sign_extend(mem), so the count is read again after
  the marks store, and the final `sprite_clip_count++` reuses the QI
  register. A field at a constant offset is added to the symbol before the
  variable offset, which gives the game's address shape; with char
  [20][6] the column joins the variable offset instead.
- The same type lets sprite_clip_apply read plain fields; its old
  `(&sprite_clip_rects[0][0])[o + 2]` text is replaced. Both match, so the
  type rests on two functions. sprite.c: 93 of 93.
- The function is plain C (K&R char parameters, as the file's other K&R
  callers need: KRPROTO_sprite_clip_push). No struct wrapper, no ORIGINAL
  SHAPE line. Field names are ours (from the callees' parameters).

## 2026-11-09: aet_still_quad_retransform matched (6,262 of 6,266)

Round 5 (build/agents/r5/aet_still_quad_retransform/). The game's zero
stores to d before the parameter moves are not source code: GCC puts
them there. Round 4's text had `float d[2] = {0, 0};`; the original
has no initializer.
- `float d[2]` is a DImode pseudo. Without an initializer its first
  mention is a partial store (d[0] through a subreg), so it is live into
  the entry block, and flow's initialize_uninitialized_subregs (run under
  `if (optimize)` after life1) puts `(set (reg:DI d) (const_int 0))` on
  the entry edge, before the parameter copies. still crosses the calls
  and gets r8; quad_index stays in memory, which gives the game's stack
  store reusing d's frame address.
- Matched precedent for the same mechanism: screen_quad_tex_elan_queue
  (gfxlib/screen_prims.c), whose game code also has inserted DI zero
  stores before its parameter moves.
- The text is plain C (parameters used directly, d assigned before it
  is read). No ORIGINAL SHAPE line. aet_play.c: 111 of 111.
- Round 4's cse1 and local-alloc reading was right for a text with the
  initializer; the initializer was the wrong statement. Lead for other
  functions: zero stores before the parameter moves mean a small
  register-mode array or struct with no initializer.

## 2026-11-09: round 5 on prize_fly_draw and stage_extras_setup

prize_fly_draw (build/agents/r5/prize_fly_draw_anomaly/): no stock match,
and none along this route.
- global.c's real REG_FREQ is flow1's count plus recompute's count
  (reg_scan is the last place it is zeroed; confirmed to the unit on
  leaf_swirl_apply and __ieee754_asinf). For x, h and y both passes count
  the same refs, so the real priorities (y 187.8, h 113.8, x 56.4) are
  the old estimates divided by 5: same order, same gaps.
- The game's order needs h's freq at least 3055 (now 1858) and x's at
  least 3703 (now 1116). The only source is refs that combine removes
  between flow1 and recompute, and none of x's nine instructions has a
  form combine folds. Splitting y, pass 0, conflicts and reload are all
  closed (details in the agent's notes).
- The compiler check was redone with real priorities (125 functions, 78
  pairs with a gap of at least 1.66x): every one keeps stock order. The
  old emulator (w659a/prio.py, compiler_check/fgreg.py) looked up flow1's
  block frequencies by lreg's block numbers; it did not affect
  prize_fly_draw. obj-pfd now has GA_DEBUG=1 and LA_DEBUG=1, which print
  the real priorities.

stage_extras_setup (build/agents/r5/stage_extras_setup/t03_side_flags.c):
stock match with no helper, using one flag per side (done0, done1), each
side checking the other has not taken the alternative model.
- cse1 makes done1's register the chosen register for zero (its last
  mention is later), so side 0's 0-store uses it; flow deletes the dead
  `done1 = 1`, and local-alloc moves the init next to the store, which
  gives the game's `mov #0,r1; mov.b r1,@r9`.
- Judgment calls: side 0's `!done1` is always true when it runs, side
  1's `done1 = 1` is never read, and the two sides order the flag store
  differently (the symmetric order is 3 lines). No sibling uses a per-side
  flag pair. Adopted 2026-11-09 on the maintainer's ruling, tagged ORIGINAL SHAPE
  on the flag declarations (6,263 of 6,266; stage_screen.c 46 of 46).

## 2026-11-09: move_tcmd_33_slot_effect matched (6,264 of 6,266)

Round 6 (build/agents/r6/move_tcmd_33_slot_effect/). Two causes:
- The hit-light index is written as `rob->mtx + arg[2] * 16` (typed
  pointer arithmetic) inside the address, not `<< 6`: expand_expr holds
  the multiply back until force_operand, which gives the game's order
  (index load, field load, then shift).
- The hit-light block is repeated in cases 1, 2 and 3, not run once
  after the switch. Each copy gets its own scratch reload, which moves
  reload's spill-register rotation (allocate_reload_reg, last_spill_reg),
  and flow2's cross-jumping then merges the copies into one, so they
  leave no code: the "allocations that leave no trace" round 4 predicted.
  An instrumented reload1.c (RL_TRACE, RL_BUMP) proved the rotation is
  the whole difference. `case 4:` is needed too: the game's jump table
  has four entries. Matched precedents for cross-jumped reloads that move
  the rotation: script_peek_move (control.c, repeated case bodies),
  vp_attack_pick, seg_clip_to_screen, win_item_values_access.
- Written as an in-function statement macro HIT_LIGHT, as
  robs_start_pos_put's SET in the same file, tagged ORIGINAL SHAPE on
  the maintainer's ruling (the bytes cannot tell a macro from pasted copies).
  The signature is now `(rob_rec *rob, short *arg)`, as move_tcmd_31.

## 2026-11-09: round 6 on prize_fly_draw and cfg_setting_or_default (both open)

prize_fly_draw (build/agents/r6/prize_fly_draw/): 130 lines to 78 with
stock GCC, by the move_tcmd_33 mechanism.
- Round 6 first showed that no text can give the game's x, h, y order
  while x has only its nine visible refs (fr12 is closed to all three by
  case 3's and case 4's constants; every conflict of y is one of x's;
  x's priority tops out near 56, h's floor is 62, y's 96).
- That bound assumed nothing is cross-jumped. v/h_2345.c repeats the
  tail every case converges on (the clamp `if (y < 0.0) y = 0.0;` and the
  champion_side x - y / x + y pair with their card_icon_pos_set calls,
  as an in-function macro) at the end of every break path, 10 copies;
  flow2's cross-jumping merges them back to the game's one copy. Stock
  then gives y its game register: update_equiv_regs doubles y's live
  length at each `y = 0.0`, which drops it below h. x 44.2 against h
  45.2 is what is left of the global order; an integer tie would put x
  first (lower allocno). With the orders forced
  (GA_ORDER=161,162,163 LA_ORDER=295,293,301) the text is byte-exact.
- v/k_2345_c4ifelse.c (case 4's sprite call as an if/else of two calls)
  gets the global order right with stock GCC (x and h tie at 43; 50
  lines), but it computes the id address in each arm, which the game
  does not: proof of the mechanism only.
- Case 4's local-alloc order (sin's address before the two frame
  addresses) is untouched by the copies: priorities 27, 24, 23.
- Open: about 16 more instructions of merged duplicate code where x and
  h are both live, or a lower trail-loop frequency for h; and case 4's
  order.

cfg_setting_or_default (build/agents/r6/cfg_setting_or_default/): still
9 lines after about 400 more texts; three constraints on the source.
- The last store sits in the bra delay slot, so it is not volatile; for
  combine to leave `p += 4` unmerged before it, p must be live after it.
- A use of p must stand between each `p = C0` and its `p += 4`.
- The game's `add #-12; add #4` is reload_cse_move2add's constant case:
  a literal `p = C0` at the rewind while no register holds C0.
- Lead (main/u2.c): a one-trip loop around each half gives all three
  and puts the counter in r2, the register the game skips. With
  -funroll-loops the write half is the game's but for the counter's
  own instructions. Open: the form or flag that leaves no counter.

## 2026-11-09: cfg_setting_or_default matched in both programs (6,265 of 6,266; test.bin 121 of 121)

Round 6 follow-up (build/agents/r6/cfg_setting_or_default/). Stock GCC,
stock flags. The game's code keeps traces of two tests that left no
instructions.
- The text reads a fourth word (`next = p[4]`, not volatile), keeps an end
  pointer (`end = p + 3` after the rewind `p -= 3`) and has two tests
  that set ret to the 1 it already holds: `if (next) ret = 1;` after
  the pointer is set and `if (p == end && next) ret = 1;` after the
  three stores.
- What they do in the compiler: each use of p stands between a pointer
  set and its `p += 4`, so combine merges none of the three pairs (the
  game's `mov.l C0,r1; add #4,r1`, `add #-12; add #4` and the unmerged
  last store); `end` takes r2 in local-alloc, so the three words take r3,
  r7, r6; after reload, reload_cse deletes the second `ret = 1` as a
  no-op (r11 already holds 1), flow2's cleanup_cfg drops the empty
  branch and life analysis deletes the compares, the fourth read and
  `end`. cse1 folds the first test early; its join label is what keeps
  the last `ret = 1` alive until then.
- So the rewind is a plain `p -= 3`, not move2add's constant case. The
  one-trip-loop lead explained the same constraints but no -O1 pass
  removes a real back edge (best loop text 11 lines).
- Adopted on the maintainer's ruling, tagged ORIGINAL SHAPE on the `next` line:
  the tests are a dead-branch shape, taken here because one text explains
  four separate compiler decisions; read as a probe whose default became
  1. The exact wording is not in the bytes (181 nearby forms match,
  among them a null check on the constant pointer).
- test.bin's copy (0x0c0714c0) builds from the same C:
  source_test/sdk/kamui/cfg_lookup.c. test.bin has no open function left.

## 2026-11-09: prize_fly_draw matched; every function matches (6,266 of 6,266)

Round 7 (build/agents/r7/prize_fly_draw/). Stock GCC 3.2.3, the file's
flags. main.bin: 6,266 of 6,266; test.bin: 121 of 121.
- On round 6's text (the converging tail repeated on every break path,
  78 lines), three changes: case 6 falls into `default:`; case 4 has
  `z = 0.0;` and `lim = 6.0;` after `h = 30.0;`, with the test
  `if (s > lim)`; the first sin result is assigned to z.
- The mechanism: a float local that only ever holds one constant is
  equivalent to it, gets no register, and reload deletes every one of
  its init insns. So `z = 0.0;` and `lim = 6.0;` are real insns at
  local-alloc and global.c that leave no code.
- Case 4's local order: with m such insns between the two sin calls the
  spans are 74+2m (land address), 76+2m (frame address) and 20+2m (sin's
  address). m = 0 gives priorities 24, 23, 27 (sin first, wrong); m = 1
  gives 23, 23, 24; m = 2 gives 23, 22, 22, and the tie goes to the lower
  quantity: land, frame, sin as the game has them.
- The global order: x 47.07 (refs 41, freq 1092, live 1160) against h
  47.99 (12, 1638, 1024) tie at 47 as ints and x is the lower allocno; y
  38.96 follows. Case 6 falling into default takes one tail copy out and
  the two assignments add one insn net where x and h are both live. The
  tie is exact: one insn either way loses it.
- Adopted on the maintainer's ruling, tagged ORIGINAL SHAPE: the PRIZE_FLY_SIDE
  macro on every break path (macro or pasted is not in the bytes); `z =
  sin_radians(...)` for the call whose result the game drops, read as z
  once animated and then switched off; `lim`, whose name and form are
  ours (a 452.0 local also matches). The text is not unique. Rerun the
  match after any edit to this function.
- Tried and closed in round 7: no game-consistent form of the trail test
  or loop lowers h's frequency (predict.c fires no heuristic on it); h's
  12 refs are all visible instructions; a second live-length doubling
  for h needs a dead set.

What rounds 4 to 7 showed, for anyone reopening a match: code that
leaves no instructions still steers the compiler. The cases found were
copies merged by cross-jumping (move_tcmd_33_slot_effect, prize_fly_draw),
tests and assignments deleted after allocation (cfg_setting_or_default,
prize_fly_draw), a zero store GCC inserts for an uninitialized array
(aet_still_quad_retransform), a second flag that changes which register
holds a constant (stage_extras_setup), a global typed as an array where
the original had records (sprite_clip_push), and a block-scope
declaration (break_floor_model_build).

## 2026-11-10: the page's measures fixed; uncalled code made functions; assembly annotated

The maintainer asked for every tile on the progress page to be complete. Three
things were in the way: faults in how the page measured, code nothing
calls that had been left as bytes, and the hand-written assembly.

**Measures (tools/progress.py, ghidra/TextLayout.java).**
- TextLayout judged an undefined byte at an odd address by a misaligned
  word, so 50 KB of nop padding counted as code.
- A function now counts with its literal pool and tables: the byte range
  its source reproduces. match.py writes ghidra/matched_extents.tsv on a
  whole-tree run; TextLayout and progress.py measure .text against it.
- The placed tile ignored "file by match with source" and showed 0.
- The eight libgcc / newlib assembly routines got their files
  (libgcc/lib1funcs.c, newlib/libc/machine/sh/*.c: the originals are
  .asm / .S; every exported file is named .c). No file is unknown.

**Uncalled code (build/agents/gaps/, NOTES.md there).** The 2026-11-03
decision to leave uncalled routines as bytes is reversed.
- 365 routines made functions and named (25 symbols TSVs, rows marked
  "made a function 2026-11-09"; build/agents/gaps/gen_rows.py rewrites
  them): 319 compiled C, 45 hand-written entries, and libgcc's 12-byte
  block move at 0c159220.
- Matched: 246 C routines (in address order in 91 source files) and all
  46 assembly entries (45 new source/asm files). Open: 73 C routines,
  named and made but not in source/ (40 with a parked text in
  build/agents/gaps/open/, 33 with none; list in work/open_list.txt).
- Not everything outside functions was uncalled: 0c04f8c4 / 0c04fae6 are
  the exits of coli_segment_hit / coli_sweep_hit, and 0c10b56a /
  0c10c6cc the specular bodies of intensity_tri_send /
  intensity_spill_tri_send, cut off by to_asm.py. They are entries of
  their own now (..._exit, ..._spec).
- to_asm.py faults found: trace() does not start a walk at a branch
  target that is also a delay slot; region() does not grow when a branch
  target equals the current end; a computed jmp is not followed; a
  routine that runs into another file's routine is refused. Not fixed
  yet; the files affected were rebuilt by hand or by scratch scripts.
- Corrections: msnm_combo_cut_short (0c0a23a0) is byte-identical to
  msnm_combo_active; net_rare_syuchu_rec was 4 bytes short
  (syuchu_value2 at +0x38); 0c12cdd0 is a run-of-points projector, not a
  light routine; osage.c ends at 0c0dc4ff, osage_asm.c is
  0c0dc500-0c0dca3f, stage.c starts at 0c0dca40.
- text_layout: 4 instruction bytes outside (jump_stub at 0c12c740, the
  template code_jump_patch copies as data) and 1,044 undefined (46 in
  four tails of existing routines to merge into their .s files; the rest
  are the pools and tables of the 73 open routines).

**Assembly (tools/match/asm_readable.py, build/agents/asm/).** 301 routines
were written in assembly by their authors; their source is the listing
in source/asm/. The rule for "reads like source": every entry described
with In: and Out:, no 8 instructions without a comment and one comment
per 10 at least, no raw word without saying what it is, no placeholder
names. The page counts hand-written routines by it.
- A two-file pilot, then 12 batches: all 161 files annotated; 301 of 301
  still match; 146 pass, the other 15 wait only on names for addresses
  that have none (names_needed_*.tsv). With the 45 new files: 191 of 206
  files, 316 of 347 routines.
- About 2,700 raw words that were really instructions are mnemonics now.
- The batches' findings (findings_*.md) correct about a hundred
  descriptions and prototypes; not applied yet. Among them: tan_s16
  returns cos / sin; coli_segment_hit / coli_sweep_hit run from r6's
  point to r5's ("from" and "to" reversed); envelope_skin_nouv / _sq
  return 0; lights_reaching_sphere's cone belongs to type 4 and its test
  rejects nothing; and probable bugs in the original (the first group's
  TSP word misses poly_header_or_words word 1 in four senders;
  float_colour_flat_copy_spec overwrites the light direction;
  bump_group_send skips an `and #3`).
- localtime_r and the four tmu0 timer routines have the shape of compiled
  code: candidates for C source instead of listings.

Page now: 99.9% of code in functions, placed and named; 99.5% rebuilt
from matching source (6,558 of 6,631 functions); 99.5% of matched
functions read like source.

## 2026-11-10: every uncalled routine matched (6,631 of 6,631); assembly names and tails

- **The 73 open uncalled C routines** (build/agents/gaps/r2/, six groups):
  all closed. What decided most: statement shape (one statement per
  step on a header word, not one expression; early returns nested or
  not; a count kept in the parameter), parameter and member types
  (uchar against char pixels, short members, float members where the
  record had bytes), and reading the matched siblings. The guess that
  several setters were bit-field stores was wrong: plain mask
  statements match and bit-fields do not.
- Types changed for them: graphics_work_rec (mark_depth, work_a_base,
  work_b_base), frame_stats_rec (shown_buffer_addr, hidden_buffer_addr,
  render_stalls, vertex_stalls), light_table_entry (atten[3], range_inv,
  range_sq). light_vec_4c_set is light_atten_set.
- Shapes that follow matched code already in the tree, placed as they
  are: the stc sr / ldc sr interrupt-mask lines (cart_dma_read,
  tmu0_restart_at_rate; 175 such lines in source/), the request_view
  typedef of dimm_command_post (dimm_command_cancel), a volatile local
  where the game stores a status read to the stack and reloads it
  (cart_dma_read).
- **line3d_fog_set** adopted on the maintainer's ruling, tagged ORIGINAL SHAPE:
  the flag-1 branch ORs `0 << 22` into both headers. The two statements
  leave no code, but the game loads both headers before storing either,
  which needs them; read as one clear-then-set field setter used with 0
  and with 2.
- Routines that look unfinished or wrong in the original, recorded as
  they are: cache_ram_block_release does not undo cache_ram_blocks_take
  (compares the tag with the unshifted block number, never looks at
  entry 0, can read one past the array); draw_ctx_tex_filter_set stores
  a hardware code where context_hw_words_compute expects an index;
  credit_state_export is a second copy of credit_snapshot.
- **Assembly**: 16 addresses the listings reference got names (rows
  marked 2026-11-10); four routine tails merged into their files. 206 of
  206 source/asm files pass asm_readable.py.
- match.py now also checks data an assembly file keeps in .text
  (`.type name,@object` with a `.size`); such data is compared like a
  function but not counted as one.

Page: 100.0% found, placed and named; 99.999% rebuilt from matching
source (6 bytes: the 4-byte jump_stub template and one stray halfword);
100.0% reads like source. Strictly, 156 bytes of .text data and routine
tails are in no source file yet (the list is in the next entry's work).

## 2026-11-10: all of .text is in the sources; the findings applied; the page reads 100%

- **Data and tails (build/agents/tails/).** The 156 bytes of .text that
  no source reproduced are in source/asm now: routine tails (pool words,
  the rest of two jump tables, a double constant) added to their
  routines, and 16 data objects with `.type name,@object` (the collision
  floats, coli_shape_handlers, the typed-send slot pairs, the jump_stub
  template and its two words, the 8 KB irq_private_stack and the 1 KB
  sq_save_stack, two unreached code fragments kept as objects:
  exception_return_bare_0c1372e8 and movstr_i4_odd_exit_0c1591e0).
  build/agents/tails/scan.py: 0 bytes outside a source file; what is
  left is zero-or-nop padding. Data a source file keeps in .text is
  marked `data` in ghidra/matched_extents.tsv and leaves the page's
  denominator like other data.
- **Findings applied (build/agents/findings/APPLIED.md, one line each).**
  105 applied, 5 in part, none refuted. Renames: credit_state_export ->
  credit_snapshot_dup, frame_stats_word_48/4c_get ->
  frame_stats_render_stalls_get / _vertex_stalls_get,
  elan_cmd_write_ptr_advance -> elan_cmd_alloc, ext_device_found ->
  ext_device_status, coli_test_from / _to -> coli_test_end / _start (the
  segment routines run from the third argument's point to the
  second's). Prototypes: the ELAN entries take (obj, header[, alpha]);
  their 132 inner routines are void f(void) working on the caller's
  registers; hand-written routines declared (void) that read r4 take
  their argument. Eight entries that set no return value stay declared
  int: void moves the C caller's call register and breaks the match.
  Types: obj_send_header_rec (half vector, light_dir[3]),
  graphics_work_rec, rob_coli_rec's touch fields, dimm_request_rec with
  arg[32] (both request_view typedefs gone), aet_file_table_hdr_rec,
  cpu_ol_target_rec, frame_pixel_format as int, ten ELAN record types and
  the nine clip rings typed with them.
- **Pipeline fixes that came with it.** ExportSource skips `.global`
  data labels in .s files; AssignCallEffects counts a push as a save only
  before the register's first write and sees the frame setup in a
  conditional branch's delay slot (101 compiled routines lost a
  convention line they should not have had), with
  ghidra/call_effects_overrides.tsv for routines whose body Ghidra cuts
  short; PruneCallingConventions.java frees unused calling-convention
  names (Ghidra keeps at most 126); ApplyTypes keeps a measured
  convention when a row's declaration is applied.
- Page: 100.0 / 100.0 / 100.00 / 100.000 / 100.0. main.bin 6,631 of
  6,631, test.bin 121 of 121.

## 2026-11-10: the ORIGINAL SHAPE audit placed; the tree is built with -g

The maintainer asked for each "ORIGINAL SHAPE (assumed ...)" tag to be re-tested:
wording the match needed but the bytes do not prove. Five groups
(build/agents/shapes/, RESULTS.md) tested the 115 tags: 84 can be plain
C, 25 are corroborated, 6 stay assumed. This entry is the placing
(build/agents/shapes/PLACED.md has a line per tag and per extra change).

- **Outcomes.** 81 tags replaced by plain C, 2 in part, 1 left on
  the maintainer's choice (prize_fly_draw's `z = sin_radians(...)`: a bare call
  also matches), 25 kept with the comment saying what corroborates the
  shape, 6 kept assumed. Nothing was backed out. 34 tags are left: 26
  read `ORIGINAL SHAPE (corroborated 2026-11-10: ...)` (the 25, and the
  zero locals of cam_mode_pointstage, which are forced) and 8 read
  `ORIGINAL SHAPE (assumed ...)` (the 6, the int pointer left in
  card_renew_text_draw, and the z line). Two of the 6 are narrower:
  motion_test_restart's offset is `i * sizeof(motion_test_side)`, and
  stage_extras_setup needs no more than a second zero-valued local.
- **What the replacements were.** Most tags were wrong types or
  declarations, not shapes:
  - One function, are_etc_draw, written the decompiler's way kept seven
    NOPROTO_ / KRPROTO_ defines and 105 block-scope declarations alive in
    effect_etc.c (33 tags). Written as its sibling are_etc_update writes
    the mode test, the file matches with the header's prototypes.
  - Types: atan2_s16 and atan_s16 return short; light_mot_path_eval and
    fish_mot_path_eval take `uchar slot`; the replay camera is one record
    at 0x0c41ef20 (replay_cam: history[20], the picks, the counters; the
    19-entry history, the state record and three byte aliases are gone);
    `short fade_color[3]` for the menu's FADE_R / G / B; cv_type, cv_coma,
    snd_type, snd_coma as short[8]; mot_id_skip_unloaded is short (short,
    short); snow_grid is `snow_cell_rec *`; cloth_slots is
    `cloth_slot_rec [2][8]`; ik_cos_law takes its three floats first;
    item_anims_setup takes ints; the camera's projection members are a
    camera_proj_rec member (camera.proj), which holds in all six files
    that use them and turns four block copies into structure assignments.
  - Statement shape: loops with an entry pointer or an index where the
    text had pointer tricks (card_log_clear, msn_text_color_by_name,
    aet_stack_push, wind_update, rain_streak_setup, the osage tables).
  - K&R definitions that were only needed because a caller above them
    passed arguments in another order or type (msn_count_draw,
    msn_panel_base_create, prize_item_chance_get) are prototyped.
- **-g.** vram_used_total_update is plain only with `-g` (the block it
  had kept the loop pass from swapping a copy). The maintainer ruled that the whole
  tree is built with -g: tools/match/match.py's FLAGS carries it and every
  tool that compiles goes through that list. Before any other change both
  programs' whole match was the same line for line with and without it
  (6,631 of 6,631; 121 of 121).
- **Declarations.** A sweep took each remaining NOPROTO_ / KRPROTO_
  define out and kept the removal when the file still matched: 30 more
  defines gone, 68 left, each needed. The block-scope declarations left
  are the six in object_put.c (corroborated: a file-scope void declaration
  is a hard error against the value-returning definition) and lines.c's
  prefetch_line_at. Compiler warnings over source/: 202 to 16.
- **Other simplifications the audit verified**: vram_take_range_at's tb
  view, two prefetch address copies, the first do { } while (0) of
  light_single_elan_block_queue (the second is forced), the rain draw
  functions' flat table pointers, ex_osage_new_update's block locals.
- **Descriptions corrected**: cfg_setting_or_default sets no return
  value for another key of the HARDWARE section (only other sections get
  the default); legacy_io_start's callers; game_assignments'
  challenge_only is written only by the reset item; the rows of every
  retyped global and function.
- **Assembly.** mtx_rot_z_rad (0c12d4e0, rotation about Z by radians,
  uncalled) is an entry of its own in 0c12d4d0.s with its row: 6,632
  functions. Ghidra gives the scaling code the three radian entries share
  to it, so ghidra/call_effects_overrides.tsv keeps mtx_rot_y_rad's
  measured list. Stale comments in twelve listings now use the names the
  records have (rob_coli_rec's touch fields, graphics_work_rec's light
  and work-area fields, the light record's atten and range_sq) and the
  prototypes as they are.
- **Pipeline.** `ghidra/run.sh all` (3 minutes 15) each time rows were
  removed, since a partial run keeps the labels of removed rows; then
  `test`. floats.tsv, prototypes.tsv, returns.tsv, signs.tsv and
  signs_words.tsv were checked out after each run but for three intended
  lines. build/agents/tails/scan.py read three columns of
  matched_extents.tsv, which has four since match.py marks data; fixed.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files (364 of 364 routines); tails scan 0
  bytes outside a source file; check_private_names.py 52 near copies.

## 2026-11-10: the first judged live run; the "observed" level

The maintainer ran the game in the Flycast debug build with the observer attached
(docs/live_observation.md): 30.8 minutes, attract mode, a coin, the
select screens and fights, 36 of the 60 modes. The log entered 2,494 of
the 6,632 routines. Each was then read against its name and description
(41 batches, the digest, the raw log and the C) and given a verdict:

| Verdict | Routines | Meaning |
|---|---:|---|
| observed | 1,716 | the log confirms a specific claim of the description |
| reached | 737 | it ran, but the log shows nothing the code does not compel |
| conflict | 41 | the log contradicts the name or the description |

- **The level.** `observed` is a third confidence level between `named`
  and `inferred` (ghidra/symbols/*.tsv, ApplyTypes.java,
  ExportSource.java, README.md). tools/observe_apply.py puts the verdicts
  of ghidra/live_verdicts.tsv into the symbol tables: the observation
  sentence (`observed live 2026-11-10: ...`) is appended to the evidence
  and an inferred row is raised. 1,591 rows are now `observed`; 123
  observed routines were already `named` and keep that level; two libgcc
  helpers have no row. The page has a sixth tile, "Code whose name is
  confirmed" (named or observed), and the naming bar and the library
  table show the level. docs/live_observation.md section 12 has the
  verdict rules.
- **The 41 conflicts** are corrected, each checked against the matched C
  first, with a `corrected from the live log 2026-11-10: ...` sentence.
  They were description and name errors, not code errors: mot_dir_get and
  move_record take the motion number minus 1; listbuf_list_used_words
  returns words (was `_bytes`); tile_split_bitmap_update's argument is a
  buffer, not a chip; the script "height" ops read the wall distance
  (script_wall_dist, ops 54-57); the fighter's `frame_advantage` field is
  own frames left minus the opponent's, so above 0 is a disadvantage
  (now frame_disadvantage; the nickname THROW 1 and ROWDY rules had the
  sign reversed); jisaku_round_end runs at every round's judge;
  fog_task_init is the FOG task's start routine, not a once-only default;
  sound_request plays in the attract demo when the setting allows;
  bonus_lottery_step runs only at the deciding round; rob_hand_draw
  indexes by side; attract-mode shadows go through the volume path;
  sprite_clip_push takes tile corners, not a width and height;
  surface_data_to_vram's middle path is the mipmapped 16-bit one;
  g1_dma_spin_idle guards the backup SRAM; water_spectrum_build's result
  is not purely real.
- **The judges' side notes** (build/agents/observed/side_notes.md, about
  300 items found while reading the code) are applied to the descriptions
  by eight groups, again each checked against the code first: about 680
  evidence edits in all. Among them: the sprite "priority" is the
  rotation angle (six routines, a field and a global renamed to `angle`);
  demo_aet_layer_rec.priority is the play flags; rob_rec.react_move and
  the reaction rows' third value are waits given to hit_wait_timer_set
  (react_wait); six card body accessors named by their fields (the quest
  deadline's day, month, year, hour, minute; the prize miss count);
  tex_slot_build_hw_words's description, cut at a "|" by an old import,
  restored; stale raw addresses and placeholder names replaced by the
  present names.
- **Faults of the original game recorded "as the game has it"**:
  sprite set 6's skip list is not ascending and its walk stalls after 14
  of 80 entries; a still's depth never steps (two stored values are
  dead); light_set_send builds 16 records into a 14-record buffer (the
  last two land on extra_light and what follows); http_client_recv_step's
  retry branch can never be taken; _tr_flush_block's set_data_type call
  is dead; the three ELAN light queue routines test mask bits 0-2 for
  lists 0, 2, 4; load_poly_req's queue walk and poly_queue_reset's dead
  branch; the mission HUD's versus branches are unreachable.
- **Renames**: 36 routines and globals (tools/rename.py) and nine struct
  members; names in ghidra/live_verdicts.tsv follow.
- **Left from this round** (build/agents/observed/structural_todo.tsv, 97
  items handed back by the groups): 46 prototypes (placeholder parameter
  names and types, nine void script ops whose r0 the caller reads, the
  cloth capsule core's int return), 28 parameter renames, six type
  changes (heap.unknown_40 is a struct mallinfo; the stage screen
  callbacks take the screen; one struct for the 0x6C-byte bone work
  record), two routines matched in the wrong source file, and small
  fixes to docs and tools. Also wider than this round: descriptions that
  still carry placeholder labels of globals named since (about 80 in
  libeffect.tsv alone).
- **The observer's limits**, as the judges met them: 16-byte peeks,
  nothing re-read on return, three hits and a routine is retired (often
  three idle calls in the first frame), float parameters 5 to 8 and the
  back FPU bank not logged, no bytes in ELAN memory. The harness in the
  companion repository (VF4debug: the original image under an SH-4
  interpreter of our own, driven by script) is being given a log without
  those limits; its first milestone boots the game and plays whole
  matches unattended and entered 2,757 routines, 623 of them not in this
  run.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files (364 of 364 routines);
  check_private_names.py 52 near copies.

## 2026-11-10: the structural items of the judged run; placeholder labels swept from the descriptions

The 97 items the correction groups handed back
(build/agents/observed/structural_todo.tsv) are applied in one serial
pass, each kept only if every function of its file still matched: 96
applied, 1 reverted.

- **Prototypes (45 of 46)**. The nine script ops whose result
  move_script_exec reads are `int f(ctrl) { return script_x(...); }`;
  control_ctx_none takes the control record; coli_capsule_z_push_core
  returns its 0 / 1; rob_fx_slot_spawn takes the bone matrix;
  date_to_seconds_1950, the dtpk_overwrite_* routines, elan_lights_queue
  and elan_matrix_cmds_write take the pointers they use (their callers
  lost the casts); mot_channel_sample and mot_sample_times16 take a
  mot_channel (times16 is `short *`: the game sign-extends the times);
  the libgcc struct movers take `void *` (ApplyKnownNames.java accepts
  it). **Reverted**: xfer_job_retire as void. Its four callers in
  elan_status.c call it through r0, which GCC does for a routine that
  returns a value; it stays int without a return statement, and its row
  says why.
- **Types (6)**. One struct for the 0x6C-byte bone work record
  (bone_work_rec), used by every routine of mot_tools.c that takes it
  and by rob_rec.bones, rob_rec.face_bones, stage_anim_rec.bones,
  mot_path_slot.bone and rob_eval_bones; the stage screen callbacks take
  the screen; heap.unknown_40 is `struct mallinfo stats`; one full
  stage_anim_rec for stage_anim_objs and stage_anim_obj2.
- **Parameters (28)** renamed after what they hold (the mission text
  routines' "counters" is the try's setting record; mot_dir_get and
  move_record take an index; the sprite routines' pri is angle).
- **Moves (2)**: sprite_record_make_by_pos and strip_state_clear are
  matched in the libspr files the source map names.
- **Found on the way**: analysis had taken the words after
  coli_poly_contains_hit's first two instructions for pointers, so the
  routine was 4 bytes long in the project (and call_on_private_stack 20).
  ApplyTypes.java has a step 4b that clears an unreferenced pointer a
  row's routine falls through into and follows the code on (116 and 120
  bytes now). 83 new data rows: the 81 per-set sprite cell tables
  (sprite_cells_<set>), zlib_version_text, leaf_cfg_buf.
  tools/movescript.py and tools/mothead.py follow the renames
  (wall_dist, card_flag, the reaction waits), and docs/asset-formats.md.
- **The sweep.** Descriptions still named globals and routines by
  placeholder label or raw address though the address has a name now.
  Of 4,475 such tokens in the evidence columns, 2,063 are replaced by the
  present name (235 labels, 1,828 raw addresses, 36 of them as an array
  element or member checked against the declared size), in 1,226 rows of
  35 files, each read first. 2,412 are left: 656 resolve to nothing, 405
  already stand beside the name, 372 are the row's own address, 330 are
  address ranges, 180 are inside observed-live sentences (which are not
  edited), and 427 were skipped on reading (position remarks,
  container-plus-offset with no member name, bare values).
- **Left, seen on the way**: 199 function rows whose declaration differs
  from the definition in source/ (159 still with param_N / undefined
  names; the exported header is right for most, the rows are stale); 15
  more definitions in a file other than the one the source map names
  (two in lib_0c10dba0.c, three in libam.c, ten in naomi2_library.c);
  dtpk_insert_* and __movstrSI12_i4 with the int-for-pointer fault the
  listed siblings had; descriptions that still say a routine "was never
  made a function" about routines that got rows on 2026-11-09.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files (364 of 364 routines);
  check_private_names.py 52 near copies; prize_fly_draw rerun, matches.

## 2026-11-10: the second judged run, from the harness: 3,185 routines observed

The companion harness (VF4debug: the original image under an SH-4
interpreter of our own, hardware as protocol models, driven by script) ran
62 unattended scenarios: boot in five regions, every fighter and stage,
the course to its ending, continue and game over, versus with its round
endings, the challenge and mission modes, settings, the debug windows.
They entered 4,026 of the 6,632 routines. Its call log holds what a
breakpoint log could not: exact counts per routine and call edge, which of
a routine's own instructions ran, and for kept calls the bytes behind
every pointer at entry and at return, the callees, and every store with
the bytes before and after.

- **Judged** (51 batches, build/agents/observed2/): the 2,185 routines the
  harness entered that were never judged (1,691) or were reached or in
  conflict in the first round. 1,626 observed, 498 reached, 61 conflicts.
  With the first round 4,042 routines have a verdict
  (ghidra/live_verdicts.tsv): 3,342 observed, 633 reached, 67 with a
  conflict on record. In src/ 3,185 routines are `observed` and 214
  `named`: 51% of the 6,632 by count.
- **Applied so far**: the observation sentences, and 466 description and
  stale-name fixes the judges wrote as exact old and new text
  (build/agents/observed2/apply_round2.py). The other 123 notes rows
  (renames, prototypes, parameters, types, faults) and the conflicts'
  closing sentences are the next pass.
- **What the richer log settled**: most first-round conflicts now hold
  against their corrected texts (mot_dir_get, move_record,
  listbuf_list_used_words, sprite_clip_push, sound_request,
  vp_round_init, vp_frame_disadv_calc, rob_hand_draw, rob_shadow_draw,
  coli_capsule_z_push_core, water_spectrum_build, g1_dma_spin_idle among
  them); the libgcc dividers (the double quotient before truncation is in
  the record); routines whose effect is a hardware register write; whole
  chains recomputed from the image's tables (the sand and snow grids, the
  spot light packing, key tracks, base matrices).
- **Routines "nothing calls" that are called**: main_heap_get,
  elan_heap_get and cpu_usage_us_ptr are reached through function
  pointers in debug window records (win_ref_resolve), 330 to 416 times
  each. Wherever a description says "uncalled", a debug window record may
  hold the address.
- **Names from the game itself**: debug menu items label four routines
  ("[rob_pos_reset]", "[clear_opt_material]", "[pol_send_itm1p]",
  "[pol_send_itm2p]").
- **Faults of the original game** found this round are recorded "as the
  game has it" in the descriptions; the next pass places the 31 the judges
  filed. Among them: a replay restores the floor-break flag but not the
  tile; the z step of a fighter's velocity adds the vertical push; game_boot
  reads its settings before the backup is loaded; the older AI's timer
  stores back a stale flags word and its sidestep pick is dead code; a
  snow spray factor of 0.0032372 beside 0.0031372; every sand grain gets
  the unturned normal; the debug texture test overruns its list by 15,612
  bytes.
- **What no scenario ran** is listed in
  build/agents/observed2/coverage_gaps.md for the harness's next
  scenarios: mission and trial rounds in which the task is performed,
  walls near enough for the wall-distance ops, failing hardware paths, a
  boot from a saved backup, and so on.
- **The harness since**: models of the card readers and of the network
  board (no network connection is made) and 58 more scenarios raise the
  routines entered to 4,698; the card and network libraries are judged
  against those in the next round.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files; check_private_names.py 52 near
  copies.

## 2026-11-10: the second round's structural notes, conflicts and faults placed

The part of the second judged round that was not exact-text: 123 notes
rows, the 61 conflicts, and the items logged for this pass
(build/agents/observed2/notes_structural.tsv, coverage_gaps.md).

- **Renames (25)**: robs_min_distance_push is robs_max_distance_pull (it
  acts at 10.0 or more apart and pulls the fighters together);
  move_cmd_40_velocity_scale is _velocity_length; round_time_seconds and
  match_time_seconds are _frames (they count sixtieths of a second); seven
  color-menu routines are their window's update or focus-update slot, not
  its draw slot; the mission input getters are _dir / _press / _held;
  debug_frame_stub is debug_boot_stub; chlg_map_visible_set takes a hidden
  flag.
- **Four names from the game** (confidence named; the debug menu item's
  label and function word are in the image): rob_pos_reset,
  clear_opt_material, pol_send_itm1p, pol_send_itm2p. Seventeen more
  routines already carry a debug menu label as their name and still stand
  at inferred or observed; and five routines described as uncalled are
  the function words of debug menu items (user_back_save_now,
  item_set_take_all, item_set_rewards_give_all,
  card_blank_write_request_1p / _2p): corrected, not renamed.
- **Prototypes, parameters, types**: 17 of 18 prototypes, 14 parameters,
  13 of 15 types. dimm_file is laid out and dimm_files_init matches as
  plain member stores (its pointer-constant shape is gone); sand_grains,
  the snow sheet spots, the noh and umi records, the mission score tables
  and rule records have their types; snow_spray_model_ids has three rows,
  shadow_piece_tables 38 entries. bios_helper_08 is int with no return
  statement (the shape of bios_coin_frame beside it; `return (*f)()` does
  not match), so mode.c no longer hides its prototype. Reverted:
  cart_control_clear as void (its callers call it through r0). Not
  applied: gz_buffer as a static_heap_rec (ApplyTypes cannot drop the old
  gz_buffer_size label), and a tenth member for the camera mode enum (the
  enum is the game's nine names; the C writes 9 with a comment).
- **Conflicts**: all 60 symbol rows and _start's header carry the
  correction and a `corrected from the live log 2026-11-10` sentence.
- **Faults of the original game**: 31 routines carry one clause each,
  ending "as the game has it", with the run that showed it or "from the
  code".
- **Settled**: action type 12 is the ring-out reaction (requested by
  rob_ring_out_start and by rob_floor_check's action 3); the gfx_state
  members render_busy (+0x208) and ta_busy (+0x20c) are named right and
  render_pipeline_choose's description had them the wrong way round.
- **Text**: 330 rows said their source file twice; one statement now.
  The card unit model and the network board model contradicted or
  sharpened 21 descriptions (card status bytes, lock and unlock, result
  codes; chunked replies, the lookup result -1, the firmware check):
  corrected, each clause ending `seen with the card unit model` or `seen
  with the network board model`.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files; check_private_names.py 52 near
  copies.

## 2026-11-10: the third judged round: cards and the network (3,865 routines observed)

The harness got models of the two card reader units and of the network
board (the board's socket commands answered inside the process and a
scripted stand-in for the servers; no network connection is made) and 58
scenarios that use them. With the 62 older ones they enter 4,698 of the
6,632 routines.

- **Judged** (23 batches, build/agents/observed3/): the 657 routines those
  scenarios entered for the first time and the 305 card, mission, network
  and I/O routines the second round could only call reached. 687
  observed, 254 reached, 21 conflicts. All rounds together: 4,699
  routines with a verdict (ghidra/live_verdicts.tsv), 4,029 observed, 598
  reached, 72 with a conflict on record. In src/ 3,865 routines are
  `observed` and 218 `named`: 61.6% of the 6,632 by count.
- **Applied so far**: the observation sentences and 155 exact-text
  description fixes. The 44 other notes rows and the 21 conflicts'
  sentences are the next pass.
- **ghidra/run.sh types** now runs tools/observe_apply.py after
  tools/mode_table.py: that generator writes ghidra/symbols/gen_mode.tsv
  afresh, which had been dropping the observation sentences of the 62 mode
  handlers at every run.
- **What the two models settled**: the card key hash and the access code
  (an MD5 and a card-deck stream cipher: a model written from the
  descriptions reproduces every recorded call); the card checksums; the
  level rules at every rank band; nickname marks through a card and back;
  the power-on, start and end exchanges with the server, reply parsing
  field by field, and each failure path's return code; the retry waits
  derived from the cabinet serial. Most first- and second-round conflicts
  in these libraries now hold against their corrected texts.
- **A correction to an earlier correction**: the card unit's lock request
  0xD9 is queued two frames after a card enters play, not "about a second"
  as a description said after the card model's first report; the judged
  logs show it in all 28 runs.
- **Faults of the original game** found this round (to be recorded "as the
  game has it" in the next pass): a select reply clears player 1's
  pending record whichever player it is for; the header parser takes 16
  headers into 15 slots; a card mode started in ADVERTISE is never run, so
  an unlock is never sent; the firmware upload reads 16 bytes past the
  image; the prize burst's fade goes negative; the card item lottery's
  walk for player 2 runs on into the next table (predicted from the code
  in the second round, now seen); a nine-digit card key loses its last
  digit before hashing.
- **What no scenario ran** is in build/agents/observed3/gaps3.md: a
  lottery win and everything behind it, quest cards whose rank or fighter
  fits the opponent, mission rounds in which the task is performed, card
  unit start-up failures, counters at their limits.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files; check_private_names.py 52 near
  copies.

## 2026-11-10: the third round's structural notes, conflicts and faults placed

The part of the third judged round that was not exact-text: 44 notes rows,
the 21 conflicts, and the items the judges reported outside the notes.

- **Renames (15)**: the card accessors that carried bit positions are
  named by their fields (card_body_quest_param_a / _b / _c and
  _category, card_hdr_quest_count, card_win_count_draw,
  card_lose_count_draw); quest_excellent_check;
  quest_cond_1_15_own_shop_streak; http_lookup_retry_step (it runs once
  per pass through the retry state, not per frame).
- **Prototypes, parameters, types**: card_money_reply_apply takes the
  reply text; card_reply_store an `int *` reply; net_game_config_rec has
  its 128-byte struggle_revise text and its 17 numbers as members;
  card_mark_motion_rec is its full 16 bytes; ticks_between and
  tmu0_ticks_to_usec are unsigned. Reverted: naominet_conn_idle_poll with
  a record pointer (one register differs; `int conn_word` stays and the
  row says what it is).
- **Conflicts**: all 21 rows carry the correction and a `corrected from
  the live log 2026-11-10` sentence.
- **Faults of the original game**: 15 more routines carry a clause ending
  "as the game has it" (the twelve the judges filed and three found while
  placing them: mie_upload_firmware reads 8 bytes past its image as the
  card unit upload reads 16; card_access_code_make drops a nine-digit
  key's last digit; net_reply_quest_apply reads tokens into a ten-word
  array with no limit).
- **Text**: network.c's card getters give card image offsets with the
  body offset in parentheses; the lock request is "two frames" after a
  card enters play everywhere; tmu0_elapsed_ticks and its sibling count
  from the timer's second load (asm header comments corrected, in both
  programs); 924 British spellings changed to US in 611 rows and the type
  headers (50 inside observed-live sentences are left as judged).
- **Left**: thirteen debug-menu aliases inside card_images
  (`_1p_have_0c37e8c5` and the like) come from ApplyEnums and stay as
  labels because ApplyTypes drops menu labels inside a curated object only
  when the menu edits it from its first byte: a change to that rule would
  remove them. Five more offset-named card accessors could be named by
  field (card_body_quest0_b0 state, _b3 kind, card_body_syu_b11 revise,
  _low5 mission day, _top3 user class).
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files; check_private_names.py 52 near
  copies; prize_fly_draw rerun, matches.

## 2026-11-10: 17 debug-menu names promoted; traces scrubbed for release

- **Named (17)**: dbg_stg_light_set, dbg_rob_light_set, window_trap,
  item_change, debug_load_texcol_name, clear_material,
  debug_console_clear, dbg_efc_dust_model_remake, rob_pol_send,
  rob_pol_send2, opttst_start, opttst_all_start, make_osage_param_text,
  dbg_card_init, dbg_card_save, dbg_sample_set, set_recv_data. Each name
  is a debug menu label in the image and the menu item's function word is
  the routine (build/agents/observed3/menu_names.py checks both). Three
  differ from the label by an underscore for a space ("[opttst start]",
  "[opttst_all start]") or by case ("[SET_RECV_DATA]"). src/ now has 235
  routines named and 3,854 observed (11 of the 17 were observed before, 6 inferred).
- **bios_helper_08** carries ORIGINAL SHAPE (corroborated): int with no
  return statement, as bios_coin_frame beside it.
- **For a public release**: the maintainer's first name, home directory
  paths and the address of the server the emulator run reached are out of
  the tracked text. Decided: a fresh history for the public repository
  (one commit of the tree), the "checked against reference material"
  wording stays. Open: the license files (a permissive license for the
  tools, a notice for the reconstructed game source, third-party notices
  for zlib, newlib and libgcc), and a clean-clone test of the README.

## 2026-11-10: clean-clone test; a first pass and a repeat pass now leave the same tree

A fresh clone was taken through the README's Reproduce steps (extract,
`ghidra/run.sh all`, the compiler build, the match).

- **Result**: main.bin 6,632 of 6,632 and test.bin 121 of 121 match from
  a clean clone, and every `.c` file under `src/` and `src_test/` came
  out as committed. Six generated files did not, from three causes:
  1. `ghidra/prototypes.tsv`, `returns.tsv`, `signs.tsv` and
     `signs_words.tsv` are logs of what a pass did this time ("set char"
     on the pass that types a global, "typed char: left alone" on every
     later one), so a first pass never writes what a repeat pass does.
     They are no longer tracked (.gitignore); the passes still write them
     and ExportSource, unadopt.py and merge_aggregate.py still read them.
  2. The exported header guarded mtx_load_identity three ways (NOPROTO_ /
     KRPROTO_ / prototype) only on a repeat pass: ExportSource took the
     "mixed" callees from the log's wording, and on the pass that first
     makes such a callee void the log says "set void". ExportSource now
     counts that row as mixed too, so the header does not depend on which
     pass it is.
  3. `vf4ft_types.h` carried two struct types (stage_anim_obj,
     stage_anim_obj2_rec) that only the long-lived working project still
     held: aggregate layouts of 2026-10 that the typed record replaced.
     The working project was rebuilt from scratch, which drops them.
- **Checked**: after the rebuild, the tracked files after the first pass
  and after a second pass (types, prototypes, export, test) are the same,
  byte for byte (build/agents/release/rebuild.sh).

## 2026-11-10: stale symbol rows and misfiled definitions

- **Rows**: 183 function rows whose declaration differed from the
  definition in source/ (143 still with param_N / undefined) now carry
  the definition's prototype (build/agents/release/stale_rows.py lists
  them, fix_rows.py applied them; 114 were names or spelling only).
  Twelve rows of routines defined in `source/asm/*.s` keep param_N: they
  have no C definition to follow.
- **const kept**: eight newlib string routines (strchr, strrchr, strstr,
  strcat, strncpy, strncmp, memcmp, memmove) are defined with the const
  their public signatures carry, and still match. zError stays `char *`
  (its callers assign the result to plain pointers). mode_req_main and
  mode_get_main are `int` as defined: mode_req_main's assert tests
  `req >= 0`, which an enum without negative members would not need.
- **Misfiled (15)**: every definition that sat in a file other than the
  one the source map names is moved to that file (seven out of
  naomi2_library.c into model_compile.c, model_tex.c, elan_cmd_ram.c and
  object_put.c; six out of libam.c into boot.c, nlam.c, irq_slots.c and
  libsnd.c; two out of lib_0c10dba0.c). The map was right in each case:
  its boundaries come from ghidra/known_files.tsv, and the three holding
  files predate the split of the libraries into objects. libam.c and
  lib_0c10dba0.c held nothing afterwards and are gone.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files; check_private_names.py 52 near
  copies; prize_fly_draw rerun, matches.

## 2026-11-10: fourth judged round (the `reached` routines against the fuller log); public snapshot

- **Published**: the tree is public at
  github.com/djanice1980/VF4-Final-Tuned-Rev-B-Decomp-Source, as one
  commit with no earlier history (build/agents/release/publish.sh adds
  one commit per publication). LICENSE is the plain MIT text; what it
  covers is in NOTICE.md.
- **The round**: the harness's milestone 4 (158 scenarios, a log that
  keeps own stores apart and in order, callee arguments and results, and
  the globals a call read) was judged for the 598 routines that were
  `reached` and the 47 the game newly entered: 645 routines in ten
  batches (build/agents/observed4/). Calls the harness itself made to
  routines were not used as evidence.
- **Verdicts**: 134 observed (106 of the 598 changed, 28 of the 47 new),
  510 reached, 1 conflict. The verdict file now has 4,163 observed, 510
  reached and 73 conflicts on record for 4,746 routines; src/ has 3,988
  routines observed and 235 named of 6,632.
- **What decided them**: the new scripted scenarios (mission checks that
  count, the challenge course's quit by START from real inputs, the
  older AI's replies to repeated attacks and throws, the rope's physics
  under a poked switch, boots from saved and damaged backup memory, bad
  I/O board replies) and, with no new code run, the fuller record
  (fields past the 256 bytes kept, own stores no longer cut, callees'
  arguments).
- **Conflict**: enemy_state_hame calls enemy_state_clear at its entry on
  every call, so its state and step are zeroed each frame and the ending
  coded after the step tests is dead code, as the game has it; the
  description said the state ends when its script is done.
- **Fault of the original game**: sram_sync_from_bank1 stores the CRC of
  bank 0's (damaged) data in bank 1 and copies bank 1 over bank 0, so
  after a repair from bank 1 both banks hold good data under a CRC that
  matches neither until the next sram_sync_from_bank0, as the game has
  it.
- **Text and types**: 46 description and stale-name fixes applied by
  exact text; level_change_items_rec has its eight item ids (was six,
  with the last two reached by indexing past a member); move_tcmd_24
  takes the entry as `short *` like move_tcmd_38; legacy_io_poll's
  unused third parameter is said.
- **Rulings made for evenness** (in build/agents/observed4/BRIEF.md): a
  routine that writes fixed literals to registers is reached, one whose
  register values are computed and fit the described formula is
  observed; an unconditional wrapper stays reached unless what it passed
  or got back is a run-time value with the described meaning; one leg of
  a comparison is enough when the values seen separate the described
  sense from its opposite; a described sub-mechanism that ran while the
  main effect did not is reached.
- **For the harness's next milestone**:
  build/agents/observed4/harness_requests.md (a clear that changes bytes
  is not kept for it; late records carry no reads; the mission, quest
  and save-copy scenarios still wanted).
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files; check_private_names.py 52 near
  copies; prize_fly_draw rerun, matches.

## 2026-11-12: fifth judged round: the routines the game never runs; the "exercised" level

- **The level**: `exercised` sits between inferred and observed. The
  game never enters the routine, so no log of a run can bear it out;
  instead the VF4debug harness called it with inputs chosen from its
  description, the expected results written down before the call, and
  the results bore the description out. It tests the description
  against the code, not how or whether the game would use the routine
  (README, Annotation conventions). tools/observe_apply.py reads
  ghidra/exercise_verdicts.tsv beside ghidra/live_verdicts.tsv; an
  observed verdict outranks an exercised one; ExportSource writes
  `confidence: exercised` from the row's sentence ("exercised DATE:
  ..."); the status page has the level as its own column and segment.
- **The round**: 1,886 routines in 30 batches (build/agents/exercised/,
  BRIEF.md has the rules and the harness's quirks: a call restores the
  whole CPU after it, so a routine that leaves its result in XMTRX
  needs a driver; va_list is five words; floats go in as their bits).
  The pilot batch x01 (C library math, character classes, the scanner,
  zlib) was committed on 2026-11-10, the other 29 on 2026-11-12.
- **Verdicts**: 1,572 exercised, 274 untested, 40 conflicts. Untested
  means calling it shows nothing the description claims: empty
  routines, constants, wrappers of one literal, routines whose
  substance is a device's answer. src/ now has 1,530 routines at
  `exercised`; the other exercised verdicts are on routines already
  named or observed and leave them so.
- **Conflicts**: all 40 corrected from what the calls showed, each with
  "corrected 2026-11-12 from harness calls: ..." after it. Among them:
  mtx_project_points reads 24-byte input records (the vertices of
  light_vertex_single), not 16; lists_send_to_ta's switch is
  mv_hdr_on; rom_crc_feed feeds signed bytes; attack_move_find steps
  away from its second argument; url_is_power_on is an exact match.
- **Text**: 193 more description and stale-name fixes from the judges'
  notes, by exact text (build/agents/exercised/apply_round5.py).
- **Faults of the original game**: 29 recorded "as the game has it",
  among them model_list_compile's count store, the background-plane
  loaders' copied vertex words, dither_argb8888_in_place never
  dithering alpha, peer_start_put_table's short write and
  pal_find_free_blocks. Two were read from the code, not run, and say
  so.
- **Declarations**: 20 returns and parameters the calls showed (asin,
  floor, copysign, isnan, scalbn, sscanf_counted in the pilot;
  ta_list_sizes_set_up, mtx_determinant, mtx_stack_drop,
  mtx_rot_vec_to_vec, tmu2_ticks_between, now_seconds_get,
  heap_realloc, realloc, strbuf_scanf, the line3d_quads wrappers,
  item_has_88 and _83), each kept only if its file still matched
  (build/agents/exercised/protos_try.py). script_op_62_move_if_opponent
  is script_op_62_move_if_chara (it tests the rob's own character).
- **Structural notes** (build/agents/exercised/notes_structural.tsv),
  placed the same day: hand-written routines take full declarations in
  their symbol rows instead of param_N guesses (mtx_add and
  mtx_subtract take float *mtx; the four coli push-point forms take
  (float *point, osage_coli_shape *shape); elan_matrix_cmds_write_front
  names out and params); __movstrSI12_i4 takes (void *dst, void *src).
  ghidra/known_functions.tsv is applied by `run.sh annotate`, so a
  change there needs `run.sh all`, not the types-to-export passes.
  frame_buffer_read_fifth takes void *out; pcm_header_field's channels
  is a local, unset when the header's low two bits are 3 (it returns
  what the register held, as the game has it), and matches as one.
- **Ghidra stays at 12.1.2**: 12.2's DataTypeWriter (rewritten around a
  dependency graph) does not count an array of structs as a dependency,
  so vf4ft_types.h wrote aet_quad before aet_quad_vertex and no longer
  compiled, and it wrote 31 by-value structs twice. Opening the project
  in 12.2 also upgrades its database for good, so the project was
  rebuilt from scratch with `ghidra/run.sh all` (3.5 minutes): src/
  came out the same apart from this round's changes and two unused
  unnamed externs. The language variant is a per-version user
  extension: a new Ghidra needs ghidra/lang/install.sh again.
  ApplyTypes.java removes stale types with dtm.remove(t), which both
  versions have.
- **For the harness's next milestone**:
  build/agents/exercised/harness_requests.md.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 210 of 210 files; console.c 192 of 192;
  check_private_names.py 52 near copies.

## 2026-11-12: the last listings that were compiled code; convention lines checked against every listing; to_asm.py's faults

- **Five listings are C now.** The four TMU0 routines
  (tmu0_free_run_start, tmu0_elapsed_ticks, ticks_between,
  tmu0_ticks_to_usec) match as plain C with the tree's flags plus
  `-fno-omit-frame-pointer`: the frame-pointer prologue and epilogue with
  no frame in use was that flag. The rest of sytmr.c matches without it,
  so they were a file of their own: source/sdk/sy/sytmr0.c (file name
  ours; known_files and known_files_test split at 0c1359a0 / 0c05fe00).
  It is the only file in the tree that takes the flag. localtime_r is
  source/newlib/libc/time/lcltime_r.c, written from the decompiled bytes
  (not newlib's text) and matching with the tree's flags; THIRD-PARTY.md
  says newlib's file carries no notice of its own (adapted from tzcode,
  in the public domain). source/asm/ has 205 files, 359 routines.
- **Convention lines against the listings**:
  tools/match/asm_writes.py walks every hand-written routine's listing
  from its entry (branches, delay slots, calls and tail branches, the
  FPU state along each path: under fschg an fmov's even frN is a pair
  and an odd one the other bank, after frchg the text's registers are
  the caller's back bank) and lists each routine whose listing writes a
  scratch register ghidra/call_effects.tsv says it keeps. Of the 96
  routines whose Ghidra body is shorter than their listing, three were
  wrong where it mattered, now rows in call_effects_overrides.tsv:
  argb_strips_send and argb_offset_strips_send (four pushes that fall
  into their _from_hook entry; Ghidra measured the 10 bytes and said
  everything but r1 is kept) and vec_to_local_regs_alt (four
  instructions that branch into vec_to_local_regs, which leaves the
  point in fr4-fr6). The others were short by a pool or matched already.
  Three override rows are narrower than the walk on purpose
  (vec_to_local_into's r5 is stepped back; vertex_colour_group_send's
  hook targets; _start never returns); the tool lists them apart and
  exits 1 only for the rest.
- **to_asm.py**: trace() walks on from a branch target that is also a
  delay slot; region() takes in the instruction at a branch target equal
  to the current end; a jmp or jsr through a register loaded with an
  address inside the routine is followed; flow into another file's
  routine (a tail call or a branch into it) leaves the routine instead
  of refusing it, and another file's pool stays there (render() already
  writes those loads as .word). Nothing is regenerated with it (the
  listings are annotated by hand); tools/match/to_asm_selftest.py runs
  its region and listing steps over every file in source/asm/: 180
  spans the same as the file before, 188 after; the 13 short end in data
  no instruction loads pc-relative, the 4 errors are files that start
  with data.
- **Checked**: main.bin 6,632 of 6,632; test.bin 121 of 121;
  asm_readable.py 205 of 205 files; console.c 192 of 192;
  check_private_names.py 52 near copies; asm_writes.py exit 0.

## Next

In order of value per effort:

1. **The open functions**: none. main.bin matches 6,632 of 6,632
   (every routine in .text, called or not), every byte of .text but
   padding is in a source file, and test.bin matches 121 of 121
   (2026-11-10). The ORIGINAL SHAPE audit is placed: 34 tags are left,
   26 corroborated and 8 assumed (build/agents/shapes/PLACED.md).
   The observed-live tier is in place (2026-11-10: 3,988 routines
   observed, 235 named, of 6,632, after four judged rounds), and the
   exercised tier for code the game never runs (2026-11-12: 1,530
   routines, 879 left inferred);
   build/agents/observed2/coverage_gaps.md,
   build/agents/observed3/gaps3.md,
   build/agents/observed4/harness_requests.md and
   build/agents/exercised/harness_requests.md list what no scenario ran
   yet; 510 routines stay reached until the harness has scenarios for
   them. 1,252 routines are code nothing refers to: running the game
   cannot observe them, so the most they can reach is exercised (274
   routines stay untested by calls). The to_asm.py faults, the
   compiled-looking listings and the convention lines of cut-short
   bodies are closed (2026-11-12); nothing else is left here.
2. **The gfx_state views that stay**: settled (2026-11-04, every kamui
   file rechecked). gfx_state is volatile as a whole, and the six
   plain-pointer views are the original code's own casts. Nothing is
   left to do here unless new evidence turns up.
3. **The card words**: done (2026-11-04). Every card cast is plain C
   or a value cast, the card data log snapshots are typed, and the
   bookkeeping names are confirmed. Nothing is left here.
4. **Live checks in Flycast** (the GDB build, 2026-11-04): no
   watchpoints (2026-11-05: breakpoints and memory reads only); the AET
   loader and layer evaluation are checked (tools/aet.py); the
   motion-change blend, the eye look-at and the stage wind are checked
   (2026-11-06, tools/flycast_live.py), and so are the hit results
   (CPU against CPU, tools/flycast_hitcheck.py), and the move-word
   flags (the action type and lever bytes). Nothing is left here.
5. **The remaining asset formats**: done (2026-11-05): stage collision
   (tools/stage_coli.py) and the sound sequences (tools/dtpk_seq.py; 5
   ambience waves stay unsettled) and the `smooth_` stance poses
   (tools/smooth.py). Nothing is left undecoded.
6. **The AET structures**: done (2026-11-05). The AET source uses
   members; the views left are listed in that entry.

A logged run of the game (Flycast with the observer, or the VF4debug
harness) raises a name from "inferred" to "observed" when the log bears
out its description; "named" stays for names the game itself gives.
