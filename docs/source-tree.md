# Original source tree (recovered from strings in main.bin)

The game was built at Sega AM2 under `/p71/prj71p/vf4e2/`. Project
`vf4e2` reads as "VF4 Evolution 2", Final Tuned's working title. These
paths survive in `assert()` messages, so only files that assert appear
here. The real tree is larger.

## Game code: `vf4lib/`

| Library      | Files seen | Likely area |
|--------------|-----------|-------------|
| `libaction`  | coli_attack.c | hit/attack collision |
| `libbase`    | cns_nickname.c, console.c, field.c, load_obj.c, mode.c, replay.c, static_heap.c, system_common.c, user_back.c | game modes, stages, replays, heaps |
| `libcard`    | card.c, card_common.c, card_mode.c, nickname.c, sol.c | VF.NET / player IC card |
| `libcore`    | gz.c, load.c, malloc.c, tm_sys.c | loading, decompression, allocator, time |
| `libdebug`   | win_sys.c | debug windows |
| `libeffect`  | effect.c, effect_etc.c, effect_malloc.c, effect_rain.c | particles and weather |
| `libenemy`   | libenemy.c, vp_main.c | CPU opponent AI ("Ver.1.10 2004-03-04 Wangan-Kage") |
| `libmission` | msnm.c, msnm_func.c | mission / training modes |
| `libmotion`  | mot_tools.c | character motion data |
| `libnetwork` | network.c | VF.NET networking |
| `libpoly`    | load_poly.c, load_poly_bg.c, pol_uti.c, put_poly.c, tune_texture.c | models, stages, textures |
| `librobdisp` | rob_color.c, rob_hand.c | character ("rob") display: colors, hands |
| `libsound`   | sound.c, sound_cmn.c | sound requests |
| `libsprite`  | aet_anim_if.c, aetset.c, sprite.c | 2D UI (AET animation scenes) |

Files of ours (no assert names them), placed by link order, data order
and callers (`ghidra/known_files.tsv` gives the evidence for each):

- `libbase`: start_asm.c, system.c, card_log.c, camera.c, game_stats.c,
  title_disp.c, ending_roll.c, adv_demo.c, task.c, appose.c,
  chara_data.c, name_entry.c, light.c, stage_fog.c, bg_color.c, input.c,
  sound_table.c, select.c, field_coli_asm.c, elan_heap.c. console.c is the
  HUD and its screen messages, and runs from 0x0C033620 to 0x0C0420FF.
- `libdebug`: debug_menu.c (the debug windows' callbacks) and
  chara_load.c (the character colour entries and the body and parts
  loaders). This code sits between win_sys.c and tm_sys.c, so the
  library is placed by position.
- `libcore`: fmath.c, fatal.c, sffs.c, clip.c, strbuf.c, fmath_asm.c.

## Platform code: `naomi2lib/`

| Library     | Files seen |
|-------------|-----------|
| `libnl2http` | http.c, map.c, naomi_socket.c (the HTTP client the game's VF.NET code uses; `src/naomi2lib/libnl2http/src/`, with two files of ours, token.c for its token lists and base64.c; test.bin links base64.c without token.c, so they are separate objects) |

Built with the game but naming no file of its own (no asserts, no
strings): a graphics library over KamuiTL and the NAOMI and NAOMI2
libraries at 0x0C126040-0x0C12C77F, in `src/gfxlib/` (directory and file
names ours): the stage water surface and the FFT it steps with, render to
texture, render-to-texture passes, recorded list banks, model merging,
screen-space quads and sprites, display-list capture. The test-mode
program links the same objects in another order, which gives the file
boundaries.

## Linked SDK and middleware (identified by version strings)

These are Sega SDK libraries, not game code. We record their interfaces
and skip line-by-line reconstruction until the game code is done.

| Library | Version / build |
|---------|-----------------|
| NAOMI2 LIBRARY (SOFT1 R&D) | Ver 0.03, Apr 5 2004 |
| NAOMI LIBRARY (AM R&D) | Ver 0.8, Apr 5 2004 |
| KamuiTL (PowerVR2 graphics) | Ver 1.10[R4], Apr 5 2004 |
| NLOBJPUT (model output) | Ver 0.99 |
| DNLSPRITE / libspr | Ver 0.21 |
| libsnd | Ver 1.05 |
| libintr | Ver 1.051 |
| nlajamma | Ver 1.01 (the JAMMA / JVS I/O library: all 36 functions identified, `src/sdk/nlajamma/`) |
| syCache / syChain / syExtChk / syG2 | 2.01 / 1.50am_am2c / 2.04 / 1.02.02 |
| alAb | Ver 1.01, Oct 19 2004 (the naominet.jp network client at 0x0C0F6140-0x0C0FBB5F and its block heap, `src/sdk/alab/`, file names ours) |
| NLADIMM / NLASOCK (WOW Entertainment) | 0.06 / 0.12, Oct 19 2004 (the DIMM network board: NLASOCK's socket requests in `src/sdk/nlasock/`, NLADIMM's board record and commands in `src/sdk/nladimm/`) |
| ELAN renderers (hand-written assembly after NLOBJPUT) | no version string; `src/sdk/naomi2_library/elan_*.c`, kept as `source/asm/*.s` |
| Mcardz / Mioz / nlam (I/O firmware blobs) | 2001 |
| zlib | 1.1.4, matched to the public source: all 71 functions and 29 tables named, `src/zlib/` |
