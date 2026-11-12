# VF4 Final Tuned: annotated source reconstruction

A preservation project: reconstruct readable, annotated C source for
*Virtua Fighter 4: Final Tuned (Ver. B)* (Sega AM2, Naomi 2, 2004) from
the SH-4 program on its GD-ROM.

Status page: https://djanice1980.github.io/VF4-Final-Tuned-Rev-B-Decomp-Source/
(how much is found, placed, named and rebuilt from matching C).

**Where the code is.** `src/` is the readable view: every one of the
6,632 routines as C, each with its address, confidence and description
in a comment; `src/vf4lib/` is the game's own code (fighters, effects,
missions, network, menus, sound, cards; for example
`src/vf4lib/libaction/src/rob_ctrl.c`), the rest the libraries it was
linked with. `source/` is the same code as bare C (and `source/asm/`
the hand-written assembly) that compiles back to the game's bytes;
`tools/match/match.py` checks every routine against your dump.
`src_test/` and `source_test/` are the same for the test-mode program.

This repo contains **no Sega code or data files**: no binaries, assets,
SDK files, SDK source or SDK identifiers. It holds the extraction tools,
notes, Ghidra scripts and source reconstructed from your own dump. Names
are ours unless the game binary itself carries them (its assert paths,
version strings and debug text) or they come from public code (zlib,
libgcc); library code is identified from the binary, and where it was
checked against reference material that material stays out of the repo.
Everything under `work/` is rebuilt from your own dump and stays out of
git.

## Reproduce

Needs the MAME `vf4tuned` set: `gds-0036f.chd` + `vf4tuned.zip` (the
317-0387-COM PIC).

```bash
sudo pacman -S mame-tools ghidra sh4-elf-binutils
tools/extract.sh "/path/to/VF4 Final Tuned Rev B"
ghidra/run.sh all
python3 -I tools/sffs.py extract work/vf41_s.dec work/assets   # optional: the 908 asset files
python3 -I tools/pvr_tex.py work/assets/unpacked work/textures   # textures to PNG
python3 -I tools/spr_tex.py work/main.bin work/assets work/sprites   # sprite pages + cells
python3 -I tools/mdl.py work/assets/unpacked work/textures work/models   # models to OBJ (after pvr_tex)
python3 -I tools/motion.py work/assets work/main.bin work/motion   # 5,756 motions to keyframe JSON, with each move's face motion
python3 -I tools/rig.py dump work/main.bin work/motion   # characters + body rigs
python3 -I tools/mothead.py work/assets work/main.bin work/motion work/moves   # 5,707 move records to JSON
python3 -I tools/movescript.py work/assets work/main.bin work/motion work/moves   # command-input scripts (ctrl_), per-move commands and frame data, branch scripts into the JSON
python3 -I tools/gltf.py work/main.bin work/assets/unpacked work/textures work/motion chr_aki work/gltf/chr_aki.glb   # skinned, animated glTF with cloth, faces and eyes
python3 -I tools/dtpk.py work/assets work/sound   # sound banks to WAV at the driver's rates
```

### Matching against the game's compiler

The game was built with GCC 3.2.3 (`-O1 -ml -m4-single-only`, and
`-fno-delayed-branch` for the game's own code and libam;
`python3 -I tools/match/delay_slots.py --groups` maps which code is built
which way). The matching tools add `-g` (tools/match/match.py's FLAGS):
it changes no instruction, and one function is plain C only with it. To
build that compiler and check reconstructed C against the game:

```bash
docker build -t vf4ft-gcc-build tools/toolchain
mkdir -p build/toolchain/dl build/toolchain/work
curl -fL -o build/toolchain/dl/gcc-core-3.2.3.tar.gz https://ftp.gnu.org/gnu/gcc/gcc-3.2.3/gcc-core-3.2.3.tar.gz
docker run --rm --user $(id -u):$(id -g) -v "$PWD/build/toolchain/dl:/dl:ro" -v "$PWD/build/toolchain/work:/work" -v "$PWD/tools/toolchain:/tc:ro" vf4ft-gcc-build /tc/build-gcc.sh 3.2.3
python3 -I tools/match/match.py
```

(GNU's `md5.sum` for gcc-3.2.3 lists `1472071074e445a1cab2aec492394bde` for
the tarball.)

The C library is newlib 1.8.2 (November 1999; its printf family and malloc are
not newlib's). To check that identification yourself:

```bash
mkdir -p build/newlib/dl build/newlib/src-1.8.2
curl -fL -o build/newlib/dl/newlib-1.8.2.tar.gz https://sourceware.org/pub/newlib/newlib-1.8.2.tar.gz
tar -xzf build/newlib/dl/newlib-1.8.2.tar.gz -C build/newlib/src-1.8.2 --strip-components=1
tools/match/scan_newlib.sh 1.8.2
```

(sourceware's `md5.sum` lists `b4df8090127da174e42852dd200a42db` for
newlib-1.8.2.tar.gz.) newlib's sources stay in `build/`; nothing from them is in
this repository.

That verifies the CHD against MAME's hash, pulls the GD-ROM files,
decrypts the DIMM image, and splits out `work/main.bin` (game, base
0x0C020000) and `work/test.bin` (test mode). `ghidra/run.sh all` builds
the Ghidra project, annotates it and regenerates `src/`. See
`docs/progress.md` for status and next steps, `docs/assets.md` for the
asset filesystem, `docs/asset-formats.md` for what is inside the files.

To watch an exported character, serve the repository over local HTTP
(browsers do not load `.glb` files from a `file://` page) and open the
viewer; it loads three.js 0.160.0 from cdn.jsdelivr.net, so it needs a
network connection:

```bash
python3 -m http.server 8642 --bind 127.0.0.1
```

Then browse to
`http://localhost:8642/tools/viewer.html?file=../work/gltf/chr_aki.glb`
(replace `chr_aki` with any exported set). The `.glb` files also open in
Blender (File > Import > glTF 2.0).

## Layout

| Path | Contents |
|------|----------|
| `tools/` | GD-ROM reader, DIMM decryptor (port of MAME), extraction script |
| `docs/`  | image layout, recovered source tree, hardware notes |
| `ghidra/` | headless import and annotation scripts; `*.tsv` are the extracted tables they apply (`prototypes.tsv`, `returns.tsv`, `signs.tsv` and `signs_words.tsv` are logs the passes write on each run, and are not in git) |
| `src/`   | reconstructed source, arranged like the original `/p71/prj71p/vf4e2/` tree; `src/include/` holds the recovered types, enums, named globals and a declaration of every function |
| `src_test/` | the test-mode program (`work/test.bin`) the same way: `ghidra/run.sh test` |
| `source_test/` | the same for the test-mode program (`VF4FT_PROGRAM=test python3 -I tools/match/match.py`) |
| `source/` | hand-written C that compiles to the game's exact bytes (checked by `tools/match/match.py`); exported into `src/` in place of the decompiler's version |
| `progress/` | `index.html`: progress page, regenerated after every `ghidra/run.sh` step and published to GitHub Pages by `.github/workflows/pages.yml` when pushed; `history.tsv` holds its data points |
| `work/`  | (ignored) extracted and decrypted binaries, Ghidra project |

## Annotation conventions

Every reconstructed function carries its original address and how
sure we are of it:

```c
/* 0x0C0A1234  libbase/src/mode.c
 * confidence: named   (named | observed | exercised | inferred | placeholder)
 * Switches the top-level game mode; called from the main loop.
 */
```

- **named**: the name comes from the binary (assert string, debug text, table).
- **observed**: the name comes from analysis, and a logged run of the game
  bore out what the description says; the comment ends with what was seen
  (`observed live <date>: ...`). See `docs/live_observation.md`.
- **exercised**: the name comes from analysis, the game never runs the
  routine, and calling it in a harness with inputs chosen from the
  description gave the results the description predicts
  (`exercised <date>: ...` says what went in and what came out). This
  tests the description against the code; it says nothing about how or
  whether the game would use the routine.
- **inferred**: the name comes from analysis; the evidence is in the comment.
- **placeholder**: `FUN_0c0a1234`-style, not yet understood.

## License

The project's own work (tools, scripts, documents, and the names,
comments and descriptions it wrote) is under the MIT License: `LICENSE`.
The reconstructed source expresses a program that belongs to its owners;
the project claims no rights over it and grants none: `NOTICE.md`. Code
from other projects (zlib, newlib, libgcc, a MAME routine, Ghidra's
SuperH4 language files) keeps its own terms: `THIRD-PARTY.md`.
