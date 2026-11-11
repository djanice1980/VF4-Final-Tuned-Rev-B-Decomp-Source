# Asset formats

What is inside the 908 files of the SimpleFlashFS volume (`docs/assets.md`).
Textures and sprites are decoded to PNG, models to OBJ, motions to keyframe curves; the other families are identified
with the evidence noted, and their contents are still to do. Sound is
decoded, each wave at the rate the sound driver plays it.

```bash
python3 -I tools/pvr_tex.py work/assets/unpacked work/textures             # 10,434 textures
python3 -I tools/spr_tex.py work/main.bin work/assets work/sprites         # 905 sprite pages + cells
python3 -I tools/mdl.py work/assets/unpacked work/textures work/models    # 215 model sets (needs the textures)
```

| Family | Files | Status | Evidence |
|--------|------:|--------|----------|
| `obj_` + `tex_` | 216 + 216 | **decoded** (textures) | descriptors tile every `tex_` file exactly |
| `mdl_` | 216 | **decoded** to OBJ; characters posed / animated to glTF | Naomi 2 Elan command lists in four slots; all 14,315 blocks walk every slot to its exact end |
| `spr_` | 57 | **decoded** (pages + cells) | the program's sprite registry; pages tile all 57 files |
| `*_aet.bin` | 30 | **decoded**: 2D scene trees with key curves | the loader's relocation and the layer values of the running game reproduced word for word (`tools/aet.py`) |
| `bg_`, `se_`, `vo_` | 32, 33, 15 | **decoded** to WAV at the driver's rates; sequences to event lists and MIDI | wave table tiles all 80 banks; ADPCM / PCM16 / PCM8, stereo, loops; rate from the driver pitch code; every sequence parses to its end (`tools/dtpk_seq.py`) |
| `motdat_` | 21 | **decoded** (5,756 motions to keyframe curves) | the directory in `motdat_EP.bin`; every channel stream ends where the next begins |
| `mothead_` | 20 | **decoded**: move records (5,707 moves: flags, stance, start and timed commands, branches) | one record per motion of a bank; the 98 command handlers are named in `src/`; branch scripts decode through `tools/movescript.py` (`tools/mothead.py`) |
| `st_*_col` | 18 | **decoded**: floor, wall and ring-out polygons on an x / z grid | `field_coli_relocate` moves exactly the 16,963 pointer words the parse finds; every byte parsed but 4 in the unused `st_test0`; the floor port gives the start height of every stage; the game's copy, floor heights and wall probes match in three save states (`tools/stage_coli.py`) |
| `ctrl_` | 17 | decoded: command-input scripts (Move scripts) | `tools/movescript.py` |
| `smooth_` | 16 | **decoded**, applied by `pose.py` / `gltf.py --smooth`: stance poses for the end-of-move blend | 16 blocks of 23 bone matrices and two hand values; mirror pairs and the parts the game reads check (`tools/smooth.py`) |
| `aicadrv.bin` | 1 | identified: AICA sound CPU program | starts with the 8 ARM exception-vector branches |

## Textures (`obj_<set>.bin` + `tex_<set>.bin`)

`<set>` is a stage (`stg_djo`), character costume (`chr_aki`), item
(`itm_lau_joha`) or effect (`ef_are`).

- `obj_` header word 2 is the offset of the **texture descriptor table**:
  16-byte records until the first one that is not a texture

  | Offset | Field |
  |--------|-------|
  | +0x00 | `short` width (8..1024) |
  | +0x02 | `short` height |
  | +0x04 | format: bits 0-7 pixel format, bits 8-11 texture type |
  | +0x08 | offset of the data in `tex_` |
  | +0x0C | unknown (0 in the sets checked) |

- `tex_` is the texture data with no header, each texture at 32-byte
  alignment. 182 of 215 sets tile exactly with the base sizes; the rest
  tile once mipmap padding and the 32-byte alignment are counted.
- The format word is the PowerVR2 / Kamui one, as `tune_texture.c`
  (0x0C0EAC40) reads it: pixel formats 0 ARGB1555, 1 RGB565, 2 ARGB4444,
  4 bump; types `0x100` twiddled, `0x200` twiddled + mipmaps, `0x300` VQ,
  `0x400` VQ + mipmaps, `0xD00` twiddled rectangle (`0x900` rectangle is
  supported by the code but unused).
- Twiddled = Morton order, y in the low bit; non-square ones continue with
  the longer side's bits. VQ = a 2 KiB codebook (256 entries of 2x2 texels,
  texel k at x = k >> 1, y = k & 1), then one index byte per 2x2 block.
- **Mipmaps** are stored smallest first. The top level of a 2^k texture
  starts at texel `MIP[k]` = 1, 2, 6, 0x16, 0x56, ... (twiddled) or index byte
  `VQ_MIP[k]` = 0, 1, 2, 6, 0x16, ... (VQ). The twiddled table was measured:
  the commonly quoted one (3, 4, 8, 0x18, ...) leaves a grid of wrong texels,
  and 2 texels earlier is the smoothest decode for 19 of 20 such textures (the
  20th is flat). The VQ table is smoothest for 452 of 457.
- **Rows are stored bottom-up.** Decoded as-is, the NYC helipad's painted
  "AM2 R&D" and the skyline are upside down (not mirrored); the decoder
  flips rows, so every PNG is upright.

Totals: 10,434 textures. Most are VQ (`rgb565_vq` 5,358, `argb4444_vq`
2,542); 710 twiddled rectangles; 457 VQ and 20 twiddled mipmapped; 2 bump.

## Models (`obj_<set>.bin` + `mdl_<set>.bin`)

The loader is `load_poly.c` (0x0C0E8540): `obj_` stays on the heap, `mdl_`
is read into a work buffer, `pol_uti.c` 0x0C0E7120 turns `obj_`'s offsets
into pointers, and each model block is copied unchanged into main RAM
(0x0C0E6E60) with the node's +0x28 pointing at it.

**`obj_`, the node tree.** Header: `u32` offsets to [0] a zero-ended list of
nodes, [1] a parallel list of name strings (`CH_AKI_k_hara_o_mune`,
`ST_NYC_STAGE00`, ...), [2] the texture descriptors (above), [3] an optional
list.

| Node offset | Field |
|-------------|-------|
| +0x04 | position (3 floats) |
| +0x10 | rotation (3 floats, radians) |
| +0x1C | scale (3 floats) |
| +0x2C / +0x30 / +0x34 | first child / next sibling / parent (file offsets) |
| +0x28, +0x2C | cleared at load, then the model copies (the second only for nodes of non-zero type) |
| +0x3C | type (below) |
| +0x80, +0x8C, +0x90 | lists fixed up for skinned bodies (not decoded) |
| +0x88 | bone pointers: 0x7B for type 0x13, 0xA2 for type 0x15 |

| Type | Nodes | Meaning (from the names) |
|-----:|------:|--------------------------|
| 0 | 17,079 | plain node or rigid piece (13,137 with geometry) |
| 0x15 | 1,323 | skinned body root, 162 bones; no geometry |
| 0x13 | 58 | body root with 123 bones (`*8`/`*9` and `XRAY` sets) |
| 0x12 | 1,361 | `envelope`: the skinned mesh, in body space |
| 0x10 | 24 | envelope whose skin carries no UVs (Dural, `XRAY`, and the bodies of `chr_mue`, `chr_jef`, `chr_ja2`, `chr_ka2`, `chr_li2` and some `itm_` hats); see Skinning |
| 0x11 | 4 | Dural body root |
| 0x0F | 55 | cloth (`hachimaki`, `nuno`) |
| 0x0E | 73 | hand shapes (`SHAPE_*_TE`) |
| 0x0D | 30 | the skinned face (`KAO_*_face_skl_*`), see Faces |

**`mdl_`, the geometry.** One `u32` offset per node (by node index, 0 =
none), to either `{ -1, n }` (no geometry; n is 1, 3, 5, 7 or 17) or a
model block:

| Block offset | Field |
|--------------|-------|
| +0x00 | 0x100 |
| +0x08 | bounding sphere x, y, z, r |
| +0x18 | block size |
| +0x40 / +0x48 / +0x50 / +0x58 | four list slots: (offset counted from that word, size) of a Naomi 2 Elan command list; the lists follow one another from +0x60 |

The list uses only two Elan commands, with the layouts in Flycast's
`elan_struct.h` (the Elan emulation): **GMP** material records (PCW
command 5, 64 bytes; `tex0` at +0x24 is an index into the set's texture
descriptors, within range in all 215 sets) and **ICH** vertex lists
(command 7, 32 bytes of ISP/TSP/TCW, vertex type and count, then the
vertices). Slot 0 holds the opaque lists (40,303), slot 1 the translucent
ones (13,305); slots 2 and 3 are empty in this game. All 14,315 blocks walk
every slot to its exact end, and the last list ends at the block size:
43,215 GMP and 53,608 ICH lists. Vertex types: 0x0A position + normal + UV
(47,170 lists), 0x4A with two ARGB colors (4,261), 0x02 position + normal
only (2,122), 0x42 position + normal + colors (42), 0x10A with UV and bump
data (13). **Correction**: until the cloth pass `mdl.py` read slot 0 only,
so every translucent list (hair, cloth, glass, foliage: 3,954 blocks had all
their geometry there, 1,270 part of it) was missing from the exports; the
"walk to their exact end" check had covered slot 0 alone. A vertex
starts with `s8` nx, ny, nz and flags (bit 29 strip, 30 fan, 31 end of
strip), then x, y, z as floats. The first two vertices of a strip have
strip 0 / fan 0, the third strip 1 / fan 1 (Flycast's `isThird`), the rest
strip 1 / fan 0; a fan vertex (strip 0 / fan 1) would make a triangle with the
strip's first vertex and the previous one, but none occurs in the character
or stage sets. Each list's ISP word sets the cull mode (bits 27-28), and the
strip winding matches it: measured against the vertex normals, it opposes
them in every cull mode 2 list and agrees in every cull mode 3 list (cull 1,
"small", has no orientation test).

Checks: the NYC rooftop assembles with its helipad reading "AM2 R&D" from
above (so neither geometry nor UV is mirrored); texture v counts from the
first stored row, the bottom of the flipped PNG, so it is OBJ's v as is.
48 vertices (Judgement 6 / Dural items, and one quad on the NYC floor) store
v as NaN (0x7FC00000); `mdl.py` writes those as 0.

**Characters are stored in pieces.** Stage pieces have identity transforms
and world-space vertices. A character's rigid pieces (head, chest, hands,
feet, ...) are each in their own bone's space, and the envelope (the skinned
clothing and skin) is in a T-pose; `mdl.py` writes them as stored. Posing
them needs the skeleton and a motion: see the next section and
`tools/pose.py`.

Output: `<set>.obj` (one object per node, one material per texture and list
type; VUR colors as OBJ vertex colors), `<set>.mtl` pointing at the
texture PNGs, `<set>.json`. 4,828,121 vertices and 3,449,611 triangles in all.

## Skeletons and motion

```bash
python3 -I tools/motion.py work/assets work/main.bin work/motion        # 5,756 motions to JSON curves
python3 -I tools/rig.py dump work/main.bin work/motion                   # characters + 13 body rigs
python3 -I tools/pose.py work/main.bin work/assets/unpacked work/textures \
    work/motion/motdat_mask_AKI.json chr_aki AKI_SY_KAMAE_VFX 0 work/models/posed/aki_kamae.obj
```

All of this is in `main.bin` and was read from its code (names in
`ghidra/symbols/motion.tsv`).

**Characters and rigs.** Character records (0x4C bytes at 0x0C1678E4, read
by `chara_data_get()`) hold the name, a height offset (+0x24, the lift
below), the body rig index (+0x31) and a costume table (+0x48; see Faces). The 13 body rigs
(`body_rigs` at 0x0C1664A0) are 47 bones each; AKIRA, JACKY, KAGE, BOZE and
JDK share rig 0. A bone is 0x2C bytes:

| Offset | Field |
|--------|-------|
| +0x00 | name: `gblctr`, `t_hara`, `k_hara`, `c_mune`, `j_mune`, `t_mune`, `j_kata_l`, `j_ude_l`, `k_te_l`, ... |
| +0x04 | flags (below) |
| +0x08 | IK bend sign (0: negative) |
| +0x0C | rest position; for chain bones the length along x |
| +0x18 | rest rotation, radians |
| +0x24, +0x28 | child count, child index bytes |

The prefixes: `k_` rigid key bones, `c_` chain controllers, `j_` IK joints,
`t_` targets and ends, `s_te` the hand-shape value, `STN_` attach points.

**Evaluation** (`rob_eval_ik` / `rob_eval_matrix` / `rob_eval_output`, a
matrix stack walked from bone 0; child = parent x local; rotations Z, then
Y, then X, with angles wrapped to 16 bits):

| Flag | Step |
|------|------|
| 0x100 / 0x200 | rest translate / rotate |
| 0x400 / 0x800 | motion translate (channels 0-2) / rotate (channels 3-5) |
| 0x2000000 | this matrix is the global frame (`gblctr`) |
| 0x80 + 0x1000000 | aim: turn local X at the first child's target, a motion position in the global frame (`mtx_aim_x`) |
| 0x80 + 0x800000 | chain root: aim at the grandchild's target, then bend by the law-of-cosines angle (`ik_cos_law`) of (child length, distance, grandchild length) |
| 0x80 + 0x200000 | chain middle: translate by its length, bend by pi minus the angle of (own length, child length, distance) |

So a motion animates root, hips, controllers and rigid bones directly and
places targets (wrists, ankles, chest and head aim points) in the global
frame, and the arms, legs, spine and neck are solved. In TEST_MAE the hips
are at 1.159, the ankles at 0.146 and the toes at 0.027 above the ground.

**The passes** (`rob_eval`, 0x0C095300; `rig.pose_passes`). The game poses
a fighter in four passes, and what it takes from the fight goes in between:

1. `rob_eval_ik` samples each bone's 16-bit angles and position and solves
   the IK angles (`mtx_to_euler` of the solved matrix). It takes the lengths,
   rest offsets and bend signs from `bone_defs` (0x0C18BF54, rig 0) and only
   the flags from the fighter's own rig, so **every fighter's IK is solved
   with rig 0's proportions**; the rigs share structure, flags and bends and
   differ only in their positions (Jeffry's and Wolf's are 1.05 times rig
   0's). Solving with the fighter's own rig, as `rig.py` did, puts limbs
   0.08 to 0.17 off on the 11 other rigs that fighters use.
2. `rob_pose_mirror` (rob +0x318 bit 4, the fighter on the other side)
   mirrors the angles and positions by three tables: position pairs
   (0x0C2A0614), angle flips (0x0C2A05C4: per bone and axis keep, negate or
   -0x8000 - a) and angle pairs (0x0C2A05E8).
3. `rob_eval_matrix` rebuilds the matrices from the angles and positions
   with the fighter's own rig and takes the IK end positions (the flag
   0x2000 bones). The root `k_hara` (0x8000) blends here: the motion-change
   blend from the previous motion's kept root (`root_mtx_blend_prev`), else
   the end-of-move blend toward the stance pose (`root_pos_rsmooth_blend`).
   `rob_bone_pos_blend` then blends the IK ends the same way.
4. `rob_eval_output` solves the IK again toward those ends, blends the other
   rotations (`mtx_rot_lerp`; the wrists come from the unblended rotation)
   and turns the head (`j_kubi`) at the look-at point: the opponent's head
   (rob +0x434, in world space) in the fighter's placed frame (translate by
   rob +0x84, turn by minus the heading +0x0C), held by `look_target_clamp`
   to +-0x3800 of yaw and -0x800 to 0x3000 of pitch in the chest frame,
   blended in by rob +0x4E0. `rob_attack_reach` stores that placed point at
   rob +0x428, but `look_target_clamp` and the `vec_to_local` after it write
   over it in place, so at the end of a frame +0x428 holds the clamped point
   in the neck's frame, not the input: read +0x434 and place it.

The motion-change blend runs 7 frames (5 when the move has flag 0x4000)
from the previous motion's last pose, placed at the new motion's root x / z
at its first frame. The weights step once a motion frame (the mode's timer
rate is 1). Checks: `rig.pose_passes` against the 31 matrices (rob +0x4C) in
six save states (Akira, Jeffry mirrored, and others; one in its end-of-move
blend with a look-at) matches within 1e-4 everywhere but Jeffry's `gblctr`,
0.065 off: the height lift, emulated since (`rig.pose_passes` `lift=`).
`rob_eval_matrix` moves `t_hara` (bone flag 0x4000) up by the character
record's +0x24 while reach flag 4 (rob +0x3F1) is on, plus
`reach.current[0] x reach.out` (the attack reach, below), so the
body rises from the hips while `gblctr` stays. `rob_attack_reach` keeps
flag 4 on for an offset above 0.001 (JEFFRY 0.065, WOLF 0.05) or below 0
(PAI -0.02, AOI -0.04, SHUN -0.06) except in some states (rob +0x20 0x800
or 0x18000000, some crouching, rob +0x24 0x40, +0x2C 0x10; a negative one
also in some menu modes). For a negative one `rob_eval_output` raises the
thigh IK aims (`j_momo_l` / `_r`, bones 27 / 34) by as much (SHUN 0.02
less, AOI 0.025 more) unless the byte 0x0C2A04FC is set, so the feet stay
down. Checked live (2026-11-06, Jeffry against Aoi, three save states):
Jeffry's `K_UFRYARAKO` 6.7e-5 (0.065 at `gblctr` without the lift), Aoi's
`AOI_PU_01_VFX` 1.2e-4 (0.175 without), both stances within 2e-4 but the
head (the look-at, not passed), Aoi with flag 4 off 8.6e-5 without it.
The terrain height is the placement's y (rob +0x88), outside the fighter's
frame.
**Attack reach** (`pose_passes` `reach=`, the `--reach` option):
`rob_attack_reach` eases `reach.current` (5 floats, rob +0x414) toward a
pose for the opponent and sets `reach.out` (+0x44C); `rob_eval_matrix`
moves `t_hara` up by `current[0] x out`, and `rob_eval_output` raises the
IK aims of the thighs (`j_momo_r` / `_l`, bones 34 / 27) by `current[3]`
/ `[4] x out` and of the shoulders (`j_kata_r` / `_l`, 18 / 10) by
`current[1]` / `[2] x out`. A leg near full stretch turns a lot for a
small aim change: Jeffry's guard `AKI_HIKINA_GU` (common bank; `current`
-0.5, 0, 0, 0.5, -0.5, `out` 0.025) missed by 0.18 at the right thigh for
a 0.0126 aim, and matches within 1e-4 with the reach (2026-11-06). Its
limb targets (`reach.limbs`, +0x450, which `rob_eval_output` blends the
same aims toward) are not emulated. JDK's down loop `VFX_DD_121_N` (state
0x1C08, move 317, 61 frames) looked like a 0.027 miss at frame 30: both
matrix buffers (rob +0x4C and +0x50, which `robs_frame_step` swaps each
frame) hold the pose of motion frame 34 exactly (9e-5, every IK target and
16-bit angle), while rob +0x0A / +0x70 read 31 and the snapshot ring
(rob +0x588) shows frames 58 to 61 then the wrap. The posing passes are
right, and so is their time: a live trace (2026-11-07, `flycast_live.py`
trace mode, Goh against Kage, CPU against CPU, 1,446 pose builds with move
317's own loop and wrap among them) has `rob_pose_eval` build every pose at
`frame_pos / 30` (fr4), one frame less in `rob_eval`, and move 317 (Goh's
round-start pose, 61 frames) wrap 61 to 1. The saved state is the odd one
out: its counter, ring and pose disagree, and its `game_flags` (0x10CA9)
differ from every fight state's (0x110005 / 0x10005), so it was saved
outside a fight.
The motion-change blend was checked live (2026-11-06, two RAM images
`tools/flycast_live.py` took mid-blend: Kage from `KAGE_TA_KAMAE_VFX` into
`KAG_PU_01_VFX`, weights 0.2 and 0.4, the head look-at running at 0.8 and
0.6): every matrix within 1e-4 at the weight rob +0x3D0 holds after the
frame (the one the frame used). The game's captured blend-from data (bone
record +0x40 matrix, +0x20 position) equal the previous motion's keep at
the old snapshot's frame (old_cur +8) less one, once the keep follows the
state word: when rob +0x20 has bit 4 (0x10, from the move record's flags;
the stance's 0x1018 has it) `root_mtx_keep` leaves the root's x / z and
`bone_pos_keep` the root offset in the keep. `rig.pose_passes(held=)` does
that and `pose.py --from` sets it from the previous move's flags; end to
end, the blend from our own keep matches within 1.5e-4 (3.2e-2 without).

**Motion directory** (`motdat_EP.bin`, loaded at boot into
`motep_load_addr`; `mot_dir_get()`): from +8, 0x18 bytes per motion id - 1,
six offsets into that motion's bank. Motion ids are global: `mot_banks`
(0x0C18BD18, 21 entries) gives each bank's range (CMN 1-1029, AKI
1030-1292, ... EP 5708-5756), and a bank file is a count, the motion names
(sorted) and the data.

| Offset | Stream |
|--------|--------|
| [0] | `s16` frame count |
| [1] | (bone, channel mask) byte pairs, bones ascending; mask bits 0x40 0x20 0x10 position x y z, 0x08 0x04 0x02 rotation x y z |
| [2] | key counts per channel: `u8` from +1, or `u16` from +2 when the first byte is 0 |
| [3] | key times in frames: `u8` under 256 frames, else `u16` |
| [4] | key codes, 2 bits per key, high bits first |
| [5] | key data: IEEE half floats without denormals (`half_to_float`) |

**Move records**: each motion of a fighter bank also has a move record in
`mothead_<bank>.bin` (see Move records below); `motion.py` writes the face
motion its command 36 sets as the motion's `"face"` (259 motions).

A channel with count c has c + 2 keys: frame 0, the c stored times, and the
last frame. A key's code is how many halfwords it has: 0 none (value 0),
1 value, 2 one tangent then value, 3 in, out, value. Between keys the value
is a cubic Hermite with tangents per second; time is frame / 30.

Checks: decoding all 5,756 motions the way the loader does, every stream
ends where the next begins, up to 4-byte padding (an empty time stream
still takes 4 bytes). Six motions (`K_AOITAKO`, `K_KM_WR_E_NA`, two
`BLK_O_CMB_STOP`, two `JDK_TA_YA_CATCH`) set mask bit 0x01, which the loader
does not read; their leftover bytes are that channel. TEST_MAE holds
the rig's rest values (`c_mune` y 1.57, `c_kubi` pi, -pi/2), and posed
frames look right: Akira's stance, Sarah's `SARAH_KI_UP` from chamber to a
vertical kick and back.

**Skinning** (`envelope_update`, `envelope_skin`). The body node (type 0x15)
lists 162 matrix slots at +0x88 (0 envelope, 1 `j_mune`, 2 `mune`, 3 `kao`,
4-13 arms, 14-22 legs, then extras). An envelope node (type 0x12) has 162
bind matrices at +0x88 (row-vector layout, the inverse T-pose of each slot:
inverting them gives `mune` at hips + 0.389 = `t_mune`, `kata`/`ude`/`te` at
shoulder / elbow / wrist, which confirms the slot-to-bone pairing) and skin
data at +0x80: groups of `[n, 0, n x (weight, slot, slot x 0x40, 0)]`, each
followed by vertex records `[c, 0, normal, position, u, v, c x (offset in the
model block, header flags)]` ended by 0. Akira's body envelope: 55 groups,
1,709 source vertices, 3,056 outputs, exactly its vertex count.

`envelope_update` handles two envelope types the same way up to the skin:
each used slot's matrix times its bind matrix (`mtx_mul_into`). Type 0x12 then
runs `envelope_skin`, or as shipped (`DAT_0c322730` = 1) the same code through
the SH-4 store queues (`envelope_skin_sq`); type 0x10 runs
`envelope_skin_nouv`, whose vertex records are 32 bytes, `[c, 0, normal,
position, c x (offset, flags)]`, without u, v, and which writes only the
header and position, so the model keeps its own UVs. All 24 type 0x10 and 996
type 0x12 envelopes with a model parse to their exact end, and their outputs
are exactly their model's vertices. Type 0x10 is used by Dural, the `XRAY`
bodies, the main body envelope of `chr_mue`, `chr_jef`, `chr_ja2`, `chr_ka2`
and `chr_li2`, and a few `itm_` hats and sashes; until it was decoded those
skins were missing (MUE had a hole at the neck, Jeffry no shoulders, Jacky and
Lion no jacket or trousers seams).

The fighter fills slots 1-22 from a fixed table, whatever meshes a set has
(`rig.body_slots`). Its 31 bone matrices (rob +0x4C) belong to the bones
listed at 0x0C18BCB8, and `rob_skeleton_update` copies the first 24 into the
body slots by the `s16` table at 0x0C18BCD8:

- 1 `j_mune`, 2 `k_mune`, 3 `j_kubi`;
- 4-7 `k_waki_l`, `j_kata_l`, `j_ude_l`, `k_te_l`, 8 the left wrist;
- 9-13 the right arm the same way;
- 14 `k_kosi`, 15-18 and 19-22 the legs.

Every set's node names agree with it. `k_mune` sits at `t_mune` (its parent,
offset 0) with its own motion rotation, which 438 motions use. The tools had
taken slot 2 as `t_mune` from the bind matrices, which cannot tell the two
apart, until the table was read.

The wrists (`tekubi`, slots 8 and 13) are the matrices `rob_eval_output`
makes from the hand bones (flags 0x8000000 left, 0x4000000 right), stored
at entries 9 and 14 (+0x240 and +0x380):

- the hand's matrix after its translation, before its rotation (so in the
  forearm's frame at the wrist);
- turned about X by atan2(|m.z|, m.y) - 90 degrees, where m is the hand's
  local Z axis through its unblended motion rotation (`mtx_xform_dir`, an
  `ftrv`).

So a wrist follows the forearm and takes only the hand's twist about it.
Three Flycast save states hold the game's own matrices: in all 12 hands the
emulation (`rig.wrist`) matches within 2e-4, one 16-bit angle step. Eight
sets weight envelope vertices to them (aki, ak2, je2, jef, kag, pa2, wol,
wo2: 16-79 per wrist).

Slot 1 has no node in any body; the game fills it from code. It is the spine
base `j_mune`: `chr_je2`'s envelope uses it, and its bind matrix puts it at
the hips with `k_mune`'s bind rotation (slot 2 is the same frame 0.41
higher). Most sets never use it and leave its bind matrix the identity.

`pose.py` poses a set: rigid pieces take their bone's matrix, envelope
vertices `sum(weight x bone x bind)`, and the face and eyes ride slot 3 (see
Faces).

**glTF** (`tools/gltf.py`, one `.glb` per character set with all its
motions):

```bash
python3 -I tools/gltf.py work/main.bin work/assets/unpacked work/textures work/motion chr_aki work/gltf/chr_aki.glb
python3 -I tools/gltf_check.py work/gltf/chr_aki.glb    # structure; --pose ANIM FRAME OBJ compares with pose.py
```

- joints are the 47 rig bones in the rig's tree; the `STN_*` attach points
  have no parent in the rig (the game places them from code) and are
  unanimated root joints;
- rigid pieces are one skin, each vertex weighted 1.0 to its bone, placed in
  the rest pose (frame 0 of the `*_SY_KAMAE*` stance); each envelope is its
  own skin with the game's bind matrices as inverse bind matrices and the
  skin record's weights, so the viewer computes what `envelope_skin` does;
- animations are baked: glTF has no IK, so each frame's solved local
  transforms are keyed at frame / 30 s (constant channels get one key);
- the wrists (slots 8 and 13) are joints under the forearm joints, keyed
  with the wrist matrix in the forearm's frame;
- the swing bones are extra root joints keyed with their simulated world
  matrix (envelope weights and rigid pieces on `ex_*` slots use them), and
  every cloth particle is a root joint at the particle, turned by the
  shortest arc from its rest normal to its current one, with each cloth
  vertex weighted to its particle;
- the face rig, the skinned and rigid faces and the eyes hang under the head
  joint (see Faces);
- materials: the texture, MASK for punch-through and BLEND for translucent
  lists, the sampler wrap from the TSP clamp / flip bits, single sided for
  cull mode 2 / 3 lists (cull 2 triangles flipped to glTF's counter-clockwise)
  and double sided otherwise; base color by the GMP rule (paramSelect bit 0
  takes the GMP diffuse, white in every character list, else the VUR vertex
  colors, which are 0 in the envelopes and unused), so `COLOR_0` is not
  written for them; texture v is flipped to glTF's top-left origin.

Checks: `gltf_check.py` re-skins the file from its own data and every vertex
lands within 1e-6 of `pose.py`'s (Akira and Sarah, mid-motion frames), except
where a face skin group has 5 face bones (2 groups in MUE's and Jeffry's
faces, 1 in Wolf's: 1-4 vertices per face): glTF viewers read one set of 4
joints, so those keep the largest 4 renormalized (0.25-0.45 mm off) and
`gltf.py` lists each case; the
structure checks pass on every set; three.js's `GLTFLoader` loads them with no
warnings (`tools/viewer.html`).

**Options.** `pose.py` and `gltf.py` take what the game takes from the
fight as options: `--mirror`, `--smooth` (the end-of-move blend to the
move's stance pose), `--from MOTION FRAME` (`pose.py` only: the
motion-change blend), `--look X Y Z [FADE]` (the head), `--eyes X Y Z`
(the eyes, see Faces) and `--wind STAGE [FRAME [HEADING]]` (the stage
wind on the swing bones and cloth, next section); the height lift
(above) is on unless `--no-lift`, and `--reach C0 C1 C2 C3 C4 OUT` sets the
attack reach (above). **Not emulated**: the reach's limb targets,
the leg and reach adjustments toward a lower opponent (zero on flat
ground), the end-of-move blend's down and turned stances and its hand
shapes. The swing bones, cloth and faces are emulated (next sections).

## Swing bones and cloth (`tools/physics.py`)

Two simulations, read from the rob code in `main.bin` (0x0C0D2120-0x0C0DCA00);
`pose.py` and `gltf.py` run both at the game's 60 Hz, two steps per motion
frame, after the rig and before skinning, starting from each motion's first
frame.

**Swing bones** (the `ex_*` slots: belt tails, sash ends, headband knots,
hair). A body node's +0x80 list holds one command list per chain of
`(tag, record)` pairs: tag 1 the root `{u32 parent slot, float offset[3],
s16 rx, ry, rz}`, tag 3 a joint and tag 0 the last joint `{float length, ?,
u32 slot, u32 parameter index}`; 482 chains in the 65 character sets. Each
fighter has one 100-entry parameter table, `{u16 type, s16 limit, float
stiffness, float damping, float gravity, s16 rest ry, rz, shape
*collision}`: the main body's entries come from a per-character, per-costume
table (a switch in 0x0C0D24C0), each item's from its item record (0x18
bytes: flags, model id = set << 16 | node, swing table, collision list,
cloth list; the records of `chara_item_table` 0x0C2DD3D8, 16 bytes per
character {item count, 12-byte card item entries with a kind and record
per costume, the four costume items per costume}, the model items being
kind 1). An item whose flags have bit 1 set, or with no swing table, takes
a dense table (no table: the debug edit table 0x0C2DD868, stiffness 0.02
everywhere) and overwrites only the entries its chains use; any other
item's table is a list of `{index, parameter}` records that overwrites the
entries it names, used or not, and leaves the rest as earlier bodies set
them (`rob_osage_items_init`). Some costumes and items tie joints of neighbouring chains
together with a maximum distance (skirts, robes; 0x0C0D2460).

Per frame, from the parent slot: the root moves to parent x offset and turns
by its angles; each joint adds the wind and its velocity, is pulled toward the next and
previous joints by gravity and held within its length of them, is pushed
along the current X axis by its stiffness, held within reach of the previous
joint and of a cross-linked one, collided (spheres and z / x / y capsules in
a slot's frame) and kept 0.01 above the floor, then aimed (type 0 yaw and
pitch, 1 the other order, 2 yaw, 3 pitch; clamped to the limit), stored in
its slot, and stepped out by its length; velocity is the move divided by
(damping + 1 + `air_reg`), minus gravity (0x3B443E72 per frame, set at
boot). A reset hangs each joint along its rest angles and settles it for two
steps.

**Cloth** (type 0x0F nodes: Akira's and Kage's headband tails and scarf,
Boze's robe, ...). The node's +0x80 data: `cols, rows`, one anchor per column
(up to 4 weighted slot x bind matrices, a position and a direction), the
particles (`cols x rows`, 0x50 bytes: flags, position, normal, velocity, rest
position, force direction, spring lengths up / below / left / right), two
maps from particles to the vertices of the cloth's two faces, and the bind
matrices. The parameters (`map order, strength, damping, sideways,
collision`) come per cloth node from a per-character list (0x0C0D87A0) or the
item record. Per frame row 0 follows its anchors, the last anchor's matrix
staying current, and each later row, solved from a copy, adds strength x
the force direction through that matrix, its velocity and the wind, then
links to the particle below, right, left and above, in that order (the
assembly integrator 0x0C0DC520, which `cloth_asm_flags` 3 picks over the C
twin; it adds no sideways length, which no table sets anyway): each link
moves the point by (n - old) x (-(n - old).y x gravity / length) and keeps
it within the length. Then it is collided, kept above the floor (the rob's
+0x7C plus 0.01), and gets velocity = move / (damping + 1 + `air_reg`) minus
gravity. The anchors read the slots after the chains' step, so a cloth hung
on a chain joint follows its new pose.

Live check (2026-11-07, `tools/physics_live.py` on four consecutive frames
of a Lau vs Pai fight on a windy stage, `flycast_live.py` phys mode): one
step from the game's own state against the next frame. The wind matches to
1e-10, every chain parameter matches, the 54 chain steps to 2.2e-5 (tips;
48 of them within 1e-6), and all 378 cloth particles to 9.2e-7. The chain
misses start at a joint whose truncated aim angle comes out one unit off
the game's (the stored angles, node +0x28 / +0x2A, show it) and carry down
the chain: the game builds the matrices in float32 with `fsca` sine and
cosine, the tools in float64, and near a whole unit that tips the
truncation. `physics.atan2s` is the game's `atan2_s16` (its rational
kernel in float32); it changes none of these. The node's stored y angle
(+0x28) reads -1 whenever the z angle is negative (`osage_node_rotate`
returns `y << 16 | z` with z sign-extended); only the z angle is ever read
(JDK's hat collision switch). Before the cloth fix the
cloth missed by up to 1.1e-2 (links left before right, no sag, the force
direction not turned).

Checks: the swing bones start exactly at their rest pose (parent x the node's
own transform) and swing a few centimetres in motion; on every set's longest
motion (600-1,315 frames) no value goes non-finite and nothing strays more
than 1.13 m from the hips (Kage's scarf tails). The glTF re-skin matches
`pose.py` at every vertex with the physics on.

**Stage wind** (`physics.Wind`, the `--wind` option; none without it).
`stage_wind_set` copies the stage's `stage_wind_table` row (0x0C306D18, 0x20
bytes: `bias`, `cycle` / `ratio` for three waves, `xang`, `yang`) into
`stage_wind` and clears waves 2-7, so the row's third pair never blows; it
leaves the phases, which run on from boot. Once a frame `osage_calc_all`
runs `wind_update` before the chains (and `cloth_calc_all` runs after it):
each wave's phase steps by cycle x 0.104719736 (a cycle of 1 is a swing a
second), z = bias + the sum of (sin(phase) + 1) x ratio, and the force
0x0C2FF5BC is (0, 0, z) turned by `xang` about x, then `yang` about y.
`osage_node_calc` adds it to each joint with the velocity (the reset's
settling steps too) and `cloth_node_integrate` to each particle. Only ROME
(0), NYC (1), CAS (2), YAM (4), GARDEN (17) and HBR (19) have wind; at its
peak it is up to 0.0059 a frame (NYC, CAS, YAM), twice the gravity, and
moves Sarah's ponytail up to 0.6 in her stance. The game simulates in world
space and the tools in the fighter's placed frame, where only the wind
differs: it comes in turned by the fighter's heading (rob +0x0C), the
option's HEADING. Checked live in two YAM save states: `stage_wind` is
YAM's row with waves 2-7 cleared, its vector the stored phases' sum turned
by y then x, the force a copy, and the two phases moved 1.5 : 1 between the
saves, the two speeds. Not checked live: the wind's effect on a chain
frame by frame (the matched C is the source).

**Not emulated**: the debug
offsets `air_reg`, `strength`, `yoko` (0 in the shipped data); the game
runs the cloth step through an assembly variant (`FUN_0c0dc520`, selected by
`DAT_0c2ff6f0` bit 0), taken to be the same as the C one (`FUN_0c0db920`)
it replaces; cloth normals follow the grid by a cross product, not the
exact `FUN_0c134340` order; the wrist slots (`tekubi`), which `rob_eval_output`
fills, follow the hand; MUE's two cloth parameter lists are applied to the
same nodes in turn and the second is used; costume 0 / 1 is taken from the
set name (`xxx` / `xx8` 0, `xx2` / `xx9` / `sh4` 1).

## Faces (`tools/face.py`)

Every fighter but Dural has two faces per costume, both drawn in the head
frame (matrix slot 3), plus two eyes. Read from the rob code (names in
`ghidra/symbols/face.tsv`).

**Costume table** (character record +0x48):

| Offset | Field |
|--------|-------|
| +0x20 x costume + 0x08 | pointer to 4 rigid face model ids (`set << 16 \| node`): the costume's `t_kao_00..03` |
| +0x80 + 4 x costume | the skinned face model (a type 0x0D node, `KAO_*_face_skl_*`) |
| +0x88 | the face rig: 33 bone records in `bone_defs` (0: LAU's, record 645) |
| +0x8C | `float[3]` face offset (PAI -0.035, 0, -0.055; WOLF z 0.005; else 0) |

The costume tables and the sets agree exactly: for every set with a face, its
table entry names its own `KAO_` node and `t_kao_00..03`. JDK and MUE use
Akira's face rig, BOZE Jacky's. The `*8` / `*9` sets have no faces.

**Which face.** While a face motion is set (rob +0x3AC) the skinned face is
drawn (`face_draw_skinned`), otherwise the rigid face `face_rigid_pick()`
chose: 0 neutral, 1 for ten frames after a voice cue, 2 while a state flag is
held (set by 0x0C075600 / 0x0C076520, unidentified), 3 a blink (a 1 in 100
chance per frame, 0-8 frames long). A move sets the face motion with its
command 36 (Move records below); it is usually the `*_KAO` motion right
after the body motion, of the same length: intros (`TOJYO`), win poses (`SYORI`),
`FUKKATU`, `MAKE`, the Quest item `GET` / `GIVE`, and a few shared ones
(`WLF_AP_SHOWUP` uses `WOLF_TOJYO_A_KAO`).

**Face rig** (`face_rig_eval`, `face_rig_walk`): from the identity, per bone
by its flags:

| Flag | Step |
|------|------|
| 0x20000000 | scale by the next of (10, 10, 10), (0.01626, 0.01626, 0.01626) (`face_rig_scales`): the root and `n_point_all` |
| 0x100 / 0x200 | rest translate / rotate (the root: -pi/4, pi/2, pi/4) |
| 0x400 | motion translate x 0.1 |
| 0x800 | motion rotate; bones 31 / 32 (`c_eye_r` / `c_eye_l`) copy their angles to the eyes |
| 0x1000 | store the matrix (31 of the 33 bones) |

then `face_rig_offset()` premultiplies each stored matrix by translate(the
face offset). The bones are `n_blue_point` (lids, brow centre, cheeks) and
`n_green_point` (brows, lids, lips, mouth corners, teeth) groups, `n_ago`
(jaw) and `n_kubi`; `o_face` / `o_eye_*` have no parent and are never
evaluated. The `*_KAO` motions key every point's translation at 10 x its rest
position, so with no keys moving the face is the rest face.

**Skin** (`face_skin` on node +0x80): groups `{ n, 0, n x (float weight,
bone, float bind[16]) }` (for n = 1, `{ 1, 0, 0, bone, bind[16] }`), each
followed by vertex records `{ c, 0, float normal[3], float position[3], c x
(model block offset, header flags) }` ending in 0, then 0; n = 0 ends. A
vertex is `sum(weight x bone x bind) x position`, its normal packed into the
header's low 24 bits (x, y, z bytes x 127). Then `face_static_copy()` copies
`{ offset, header, x, y, z, normal[3] }` records over some of the skinned
vertices. Akira: 102 groups, 961 source vertices, 2,118 outputs (the whole
model), 106 static. A bone has the same bind matrix in every group, in all 30
faces.

**Eyes** (`eyes_draw`, `eye_draw`): the table at 0x0C2B0B1C, 0x20 bytes per
character id x 2 + costume, gives each eye's model id and offset (SHUN has
none; his faces carry their eyes). An eye is drawn at head x translate(offset)
x translate(node position) x rotate(node angles) x rotate(eye angles). The
node angles are radians (half the eye nodes carry 1.5708 or 3.1416), but
`eye_draw` truncates them to integers and uses them as 16-bit angle units, a
1/65536-of-a-turn rotation: in effect none, and the eye meshes face forward
without it. The eye angles come from the face motion's `c_eye_r` (eye 0) and
`c_eye_l` (eye 1), or
from `eyes_look_at()` (0x0C093340, while a move's timed command 41 sets
rob +0x52D): from the midpoint of the eyes in the head frame, the yaw
atan2(z, x) held to -0x1200 to 0x1000 and the pitch held to +-0x1C00, the
yaw scaled down by 1 - |pitch| / 0x1C00; the eyes take minus each.
`pose.py` / `gltf.py --eyes X Y Z` aim them at a point. A move start zeroes
them. The head frame is the slot-3 matrix (rob +0x54 + 3 x 0x40, as
`pose.py` draws the face), not bone entry 3 of rob +0x4C. Only three moves
set the command: Sarah's `SARA_SYORI_A` and Jacky's `JAC_SYORI_B` win poses
(bit 0, the camera's eye, camera +0x0C) and the attract move `ADV_A18_AKI`
(bit 1). Checked live (2026-11-06, `SARA_SYORI_A` frames 1 and 2):
`face.eye_look` gives the game's rob +0x52E angles, 0 / -0x1C00 exactly and
-436 / 2962 against -436 / 2961.

Checks: the rest skinned face matches its rigid `t_kao_00` to a median of
0.005 mm and at most 0.2 mm for Akira (also Blackie, Jacky, JDK, Shun);
the others are at most 9 mm off (median under 2 mm);
MUE, which uses Akira's rig, is off by up to 4 cm at rest but matches exactly
at frame 0 of its own face motion (its motions carry its own positions).
Posed with `TOJYO_A`, Akira closes his eyes at frame 90 and shouts at 135.

**In the tools.** `pose.py` draws the skinned face when the motion has a face
motion, else the rigid neutral face, and always the eyes. `gltf.py` puts
under the head joint a `face` node (parent of the face rig joints `face_*`,
carrying the static vertices), a `face_rigid` node (the rigid neutral face),
and `eye_*` joints. The skinned face uses the game's bind matrices as inverse
bind matrices. Each animation keys the face it does not show to scale 0, as
the game switches per move. The node defaults follow the rest motion, which
has no face motion, so the rigid face shows. The face motion's rig and eye
angles are baked per frame. The blink and the other rigid faces (`t_kao_01..03`)
are runtime choices and are not in the glTF (they are in the OBJ export).
Viewers built on three.js should not frustum-cull the skinned meshes: three.js
caches a skinned mesh's bounds from the first pose it renders, and a face
first rendered at scale 0 gets a zero radius (`tools/viewer.html` turns
culling off).

## Move records (`tools/mothead.py`)

```bash
python3 -I tools/mothead.py work/assets work/main.bin work/motion work/moves   # 5,707 moves to JSON
```

Every motion of a fighter bank has a move record in `mothead_<bank>.bin`, read
by the rob code (names in `ghidra/symbols/move.tsv`). The file starts with one
`u32` offset per motion of the bank (id - first, 0 for none); offsets are from
the file start. A record is 20 bytes (`move_start`, 0x0C0659E0):

| Offset | Field |
|--------|-------|
| +0x00 | `u32` flags, kept at rob +0x324 |
| +0x04 / +0x05 | `u8` (rob +0x284), `u8` (rob +0x285; bit 6 a side flag) |
| +0x08 | command list `{ u32 argument offset, u32 command }` ending in 0, run once at the move start through `move_commands` (0x0C1746D4, 52 handlers) |
| +0x0C | timed list `{ u32 entry offset, u32 command }` ending in 0; an entry is `{ s16 frame, arguments }`, fired once the move reaches the frame by `move_timed_run` through `move_timed_commands` (0x0C1747A4, 46 handlers) |
| +0x10 | branch list `{ u32 branch offset, 0 }` ending in 0 |

`move_start` also sets rob +0x2E4 to the motion's frame count (`mot_frames`),
which several commands clamp their frames to.

**Branches** (`move_branch_scan`, `move_branch_try`) are how strings and
cancels work: `{ u8 flags, s8 input, s8 input2, s8 condition, u16 value, s16
start, s16 end, u16, u32 script offset }`, live from frame `start` (-1: from
the beginning) to `end`. `input` and `input2` index `input_patterns`
(0x0C174590, 54 `{ value, mask, kind }`: kind 0 none of `value`'s bits in the
frame's input, 1 the new presses masked equal `value`, 2 the input masked
equal `value`; 0 always matches); `condition` indexes `input_conditions` (a
button combination or direction held at least `value` frames; see Input
below); `movescript.py` adds them to the JSON as text; then the branch's
script, byte code
for `move_script_exec` (opcodes below 0x50, handlers at 0x0C29F6CC), picks the
next move (Move scripts below; `movescript.py` adds each branch's decoded
script to the JSON as `script_code`).

**Commands.** The 98 handlers were not functions in the Ghidra project
(reached only through the two tables); `ghidra/code_tables.tsv` now seeds them
in the discover step, so each is in `src/` with a name and a note on what it
writes. Most set fields of the rob (the fighter state); the field names below
are what their handler does with them, or the rob offset when that is all it
says. The ones with a clear role: 1 `frames` (three frame marks, the third
the end), 3 `attack` (the attack: its damage at +8 is scaled by the attack
rate, `damage_scale`: v x (1000 + 8 x rate) / 1000), 4 `frame_table` and 5-13
(a table of frame values, single rows patched by 6-13), 15 `throw_damage`, 20
`damage_by_distance` (damage interpolated by the distance to the opponent), 36
`face_motion`; timed 1 `sound`, 2 `voice` (also the voice face), 8 `launch`
(a velocity from a heading and speed, then airborne), 13 `hand_shape` (the
`s_te` hand-shape value of either hand), 14 `rigid_face`, 15 `damage_tick`, 17
`camera` (a scripted camera), 22 `approach` (moves toward a point by the
opponent by a frame), 33 `slot_effect` (an effect at a matrix slot), 39
`limb_target` (a target for one of four limbs), 40 `stop`, 41 `eye_look`, and
the set / clear pairs for the flag words at rob +0x20, +0x24, +0x28, +0x2C and
+0x318. Five immediate and two timed handlers return at once; commands 5, 27,
37, 46 and 47 and timed 32, 36, 37 and 45 never occur. types: b / B s8 / u8, h / H s16 / u16, i s32, x u32 (hex), f float.

| # | Name | Argument fields (type @ offset) | Uses |
|--:|------|------|-----:|
| 0 | `follow` | `move` i@0x0, `rob_2de` h@0x4, `rob_2e0` h@0x6, `rob_2e2` h@0x8 | 260 |
| 1 | `frames` | `rob_2e6` h@0x0, `rob_2e8` h@0x2, `end` h@0x4 | 3,726 |
| 2 | `cmd02` | `rob_32` B@0x0, `rob_2ee` h@0x2, `rob_2f0` h@0x4, `rob_2f2` h@0x6, `rob_34c` x@0xc, `rob_354_add` f@0x10, `rob_350` x@0x14 | 229 |
| 3 | `attack` | `rob_30` B@0x0, `side` B@0x1, `rob_287` B@0x2, `rob_289` B@0x3, `rob_28a` B@0x4, `rob_28b` B@0x5, `rob_28c` B@0x6, `damage` h@0x8, `rob_2a4` h@0xa, `rob_2a6` h@0xc, `rob_328` f@0x10, `rob_32c` f@0x14 | 1,817 |
| 4 | `frame_table` | `t00` h@0x0, `t01` h@0x2, `t02` h@0x4, `t03` h@0x6, `rob_364` f@0x8, `rob_368` f@0xc, `t08` h@0x10, `t09` h@0x12, `t10` h@0x14, `t11` h@0x16, `t12` h@0x18, `t13` h@0x1a, `t14` h@0x1c, `t15` h@0x1e, `t16` h@0x20, `t17` h@0x22 | 1,403 |
| 5 | `frame_rows` | `t00` h@0x0, `t01` h@0x2, `t02` h@0x4, `t03` h@0x6, `t04` h@0x8, `t05` h@0xa, `t06` h@0xc, `t07` h@0xe, `t08` h@0x10, `t09` h@0x12, `t10` h@0x14, `t11` h@0x16 | 0 |
| 6 | `frame_row_06` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 1 |
| 7 | `frame_row_07` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 1 |
| 8 | `frame_row_08` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 1 |
| 9 | `frame_row_09` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 2 |
| 10 | `frame_row_10` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 1 |
| 11 | `frame_row_11` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 15 |
| 12 | `frame_row_12` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 49 |
| 13 | `frame_row_13` | `value` h@0x0, `frame` h@0x2, `extra` h@0x4 | 3 |
| 14 | `nop` | - | 6 |
| 15 | `throw_damage` | `damage` h@0x2, `rob_2f6` h@0x4, `rob_294` h@0x6, `rob_296` h@0xa, `rob_298` h@0xc | 863 |
| 16 | `cmd16` | `rob_2f2` h@0x0, `rob_2f6` h@0x2, `rob_294` h@0x4, `rob_296` h@0x6 | 853 |
| 17 | `cmd17` | `rob_2fa` h@0x0, `rob_358` x@0x4, `rob_35c` x@0x8, `rob_200` f@0xc | 26 |
| 18 | `nop` | - | 29 |
| 19 | `list_374` | pointer kept on the rob | 152 |
| 20 | `damage_by_distance` | `near` B@0x0, `middle` B@0x2, `near_distance` f@0x4, `middle_distance` f@0x8, `far_distance` f@0xc | 29 |
| 21 | `cmd21` | `rob_300` h@0x0, `rob_302` h@0x2, `rob_304` h@0x4, `rob_306` h@0x6, `rob_28f` B@0x8, `rob_308` h@0xa | 76 |
| 22 | `cmd22` | `rob_330` x@0x0, `rob_338` x@0x4 | 1,650 |
| 23 | `cmd23` | `rob_334` x@0x0 | 39 |
| 24 | `cmd24` | `rob_2f6` h@0x0 | 50 |
| 25 | `cmd25` | `rob_1bd` B@0x0, `rob_29c` h@0x2, `rob_290` B@0x4, `rob_29e` h@0x6 | 86 |
| 26 | `cmd26` | `rob_33c` x@0x0, `rob_30a` h@0x4 | 51 |
| 27 | `list_378` | pointer kept on the rob | 0 |
| 28 | `list_37c` | pointer kept on the rob | 2 |
| 29 | `list_380` | pointer kept on the rob | 80 |
| 30 | `cmd30` | `rob_348` x@0x0, `rob_29a` h@0x4 | 3 |
| 31 | `cmd31` | `rob_2ec` h@0x0 | 60 |
| 32 | `cmd32` | `rob_2f8` h@0x0 | 481 |
| 33 | `flags24` | `flags` x@0x0 | 905 |
| 34 | `flags28` | `flags` x@0x0 | 1,753 |
| 35 | `flags2c` | `flags` x@0x0 | 449 |
| 36 | `face_motion` | `motion` h@0x0 | 259 |
| 37 | `cmd37` | `rob_30e` h@0x0, `rob_310` h@0x2 | 0 |
| 38 | `hands_flag` | - | 3,200 |
| 39 | `flag28_bit15` | - | 81 |
| 40 | `velocity_scale` | `scale` f@0x0, `rob_20c` f@0x4 | 27 |
| 41 | `cmd41` | `bit10` B@0x0 | 22 |
| 42 | `flags3a6` | `flags` H@0x0 | 504 |
| 43 | `cmd43` | `rob_36c` f@0x0 | 44 |
| 44 | `list_384` | pointer kept on the rob | 39 |
| 45 | `cmd45` | `rob_312` h@0x0, `rob_314` h@0x2 | 17 |
| 46 | `nop` | - | 0 |
| 47 | `cmd47` | `rob_3aa` h@0x0 | 0 |
| 48 | `cmd48` | `rob_178` B@0x0 | 21 |
| 49 | `nop` | - | 6 |
| 50 | `flag344` | `bit` B@0x0 | 1 |
| 51 | `nop` | - | 175 |

| # | Name | Argument fields (after the s16 frame) | Uses |
|--:|------|------|-----:|
| 0 | `random_follow` | `every` B@0x2, `chance` B@0x3, `move` i@0x4 | 21 |
| 1 | `sound` | `sound` h@0x2 | 2,542 |
| 2 | `voice` | `voice` h@0x2 | 2,583 |
| 3 | `tcmd03` | `rob_2f6_add` h@0x2 | 304 |
| 4 | `set_flag20` | `bit` h@0x2 | 249 |
| 5 | `clear_flag20` | `bit` h@0x2 | 247 |
| 6 | `toggle_side` | - | 1,020 |
| 7 | `tcmd07` | - | 80 |
| 8 | `launch` | `heading` h@0x2, `speed` f@0x4, `rise` f@0x8 | 13 |
| 9 | `sound_flag` | `sound` h@0x2 | 166 |
| 10 | `state33` | `op` h@0x2 | 689 |
| 11 | `nop` | - | 36 |
| 12 | `tcmd12` | `rob_12` h@0x2 | 152 |
| 13 | `hand_shape` | `hands` H@0x2, `value` f@0x4, `until` h@0x8 | 1,074 |
| 14 | `rigid_face` | `face` h@0x2 | 597 |
| 15 | `damage_tick` | `damage` h@0x2 | 381 |
| 16 | `tcmd16` | `rob_577` B@0x2 | 12 |
| 17 | `camera` | `camera` h@0x2, `kind` B@0x4, `length` h@0x6 | 114 |
| 18 | `tcmd18` | `op` B@0x2, `value` B@0x3, `distance` f@0x4 | 9 |
| 19 | `adjust_177` | `delta` b@0x2 | 10 |
| 20 | `adjust_177_if` | `when` B@0x2, `delta` b@0x3 | 17 |
| 21 | `adjust_177_opponent` | `when` B@0x2, `delta` b@0x3 | 66 |
| 22 | `approach` | `until` h@0x2, `flags` H@0x4, `min` f@0x8, `max` f@0xc, `target_x` f@0x10, `target_z` f@0x14, `offset` f@0x18, `rise` f@0x1c | 37 |
| 23 | `tcmd23` | `kind` h@0x2, `reaction` H@0x4, `mask` x@0x8, `distance` f@0xc | 198 |
| 24 | `tcmd24` | `value` h@0x2, `compare` h@0x4, `rob_64` x@0x8 | 51 |
| 25 | `set_flag24` | `bit` h@0x2 | 308 |
| 26 | `clear_flag24` | `bit` h@0x2 | 251 |
| 27 | `set_flag28` | `bit` h@0x2 | 26 |
| 28 | `clear_flag28` | `bit` h@0x2 | 90 |
| 29 | `set_flag2c` | `bit` h@0x2 | 9 |
| 30 | `clear_flag2c` | `bit` h@0x2 | 9 |
| 31 | `opponent_108` | `value` h@0x2, `mask` x@0x4 | 215 |
| 32 | `opponent_108_add` | `value` h@0x2, `mask` x@0x4 | 0 |
| 33 | `slot_effect` | `mode` h@0x2, `slot` h@0x4, `length` h@0x6 | 64 |
| 34 | `tcmd34` | `rob_550` h@0x2 | 317 |
| 35 | `tcmd35` | `op` h@0x2 | 27 |
| 36 | `set_flag318` | `bit` h@0x2 | 0 |
| 37 | `clear_flag318` | `bit` h@0x2 | 0 |
| 38 | `tcmd38` | `until` h@0x2 | 20 |
| 39 | `limb_target` | `until` h@0x2, `release` B@0x4, `limb` B@0x5, `kind` B@0x6, `x` f@0x8, `y` f@0xc, `z` f@0x10 | 26 |
| 40 | `stop` | - | 8 |
| 41 | `eye_look` | `look` B@0x2 | 3 |
| 42 | `tcmd42` | - | 2 |
| 43 | `nop` | - | 9 |
| 44 | `tcmd44` | `frames` h@0x2, `rob_340` x@0x4, `rob_370` x@0x8 | 9 |
| 45 | `tcmd45` | - | 0 |

Checks: the argument sizes measured from the gaps between referenced offsets
agree with the fields each handler reads (the +2 cases are alignment).

**What the attack commands set** (the rob fields, from their readers; names in
`ghidra/symbols/move.tsv`):

- `frames` (1): +0x2E6 the first active frame, +0x2E8 the end of the active
  window (exclusive), +0x2EA the recovery end, the frame the move can be left.
  `attack_active_flag` sets rob +0x20 bit 14 inside the window for moves whose
  record flags have bit 14 (the attacks, 1,819 moves).
- `attack` (3): the attack flags, the attacking limb (an index into a limb
  table that `rob_attack_reach` uses to lean the body toward the opponent
  during the window; mirrored left / right with the fighter's side), the level
  (high 1 / 7, mid 2 / 4, low 3 / 5 / 6 / 13 by the game's three tests
  `attack_is_high` / `_mid` / `_low`; 8-10 throws, 12 / 14 down attacks), the
  damage, and flags that change the default hit stun.
- `frame_table` (4) and the row patches (6-13): rows of `{ stun, recovery end,
  reaction wait }` for guard, hit, counter hit, a defender-state row, a fifth
  row, and the hit-result bits 0x800 / 0x4000 / 0x8000 / 0x10000 (the hit
  result at (rob +0x3C) +0x3C: 1 hit, 2 counter hit, 0x10 lethal). A stun of 0
  is computed from the damage: guard 10 + 2/3 of it (6 + under 15), clamped to
  10..26 (`guard_stun`); hit 8 + 4/5 of it, kept within the frames left - 2 ..
  + 6 (`hit_stun`). A recovery of -1 is the move's +0x2EA.

**Hit results** (`attack_hit_resolve`, checked live 2026-11-06 on 239 hits of
CPU-vs-CPU fights, `tools/flycast_live.py` hits mode with `cpu_on_off` 4 and
`tools/flycast_hitcheck.py`; 0 mismatches): a plain hit is 0x1001. It is a
counter hit when the defender is in an attack (its move flags 0x4000 with
state bit 0) or has flags2c 0x20: then state bit 2 (the defender-state row,
seen on recoveries) gives 0x800 (0x801), else 2 and one of three damage
rows picked by the **defender's** own attack damage (rob +0x2A0, the move
that was countered, not the attacker's): up to 15 row 0 (0x4003), 16-24
row 1 (0x8003), 25 and over row 2 (0x10003) when the backup option word has
0x10000 (set in the maintainer's settings, 0x21011100), else row 1. A defender with
state 0x40 (airborne) adds 4 (0x1005, 0x805, 0x8007). 0x2000 comes from the
attacker's relation flags, 0x10 when the hit is lethal. The guard results
are 0x20 (`hit_result_guard`) and 0x80 (`hit_result_guard_knockback`).

**Frame data.** The game computes frame advantage itself for the training
display (`frame_advantage`, 0x0C0F0680): with the attacker at its first active
frame, advantage = stun - (recovery end - first active frame), on guard, on hit
and on counter hit (half the damage again). `mothead.py` runs the same
formula for every attack (`startup`, `advantage` in the JSON) and
`movescript.py` writes `framedata_<bank>.tsv`: command, startup, active
window, recovery end, level, damage and the three advantages. Akira's jab
comes out 12 frames, +1 on guard, +5 on hit (the familiar VF4 jab); his
Tetsuzankō 4 6 6 P+K 13 frames, -24 / +6 / +6. The formula assumes neither
fighter in a special state and the default options, and the commands column
lists only moves started from the neutral lists (about half; the rest are
strings and branches). Checked in Flycast against the running game
(`tools/flycast_robs.py` reads a save state):
- Akira's `AKI_TA_PU_JAB2HATI` blocked: the game's own frame difference
  (rob +0x16A) was 10 frames against him, and the table says -10 on guard.
- `AKI_PU_02_VFX` on hit: he was free while the opponent had 4 stun
  frames left one frame later, which is +5, and the table says +5.
The training display that `frame_advantage` serves is the debug
frame-check tool, not part of normal play. In a normal fight the game
keeps the live difference in rob +0x16A, through
`robs_frame_advantage_update`. Akira's `AKIRA_TOJYO_A` sets face
motion 1060 and plays a voice at frame 30 and sounds at 46 and 139.

## Move scripts (`tools/movescript.py`)

```bash
python3 -I tools/movescript.py work/assets work/main.bin work/motion work/moves   # ctrl_*.txt, commands_*.txt, framedata_*.tsv, branch scripts
```

The command input is a byte code (`move_script_exec`, 0x0C05E760; names in
`ghidra/symbols/script.tsv`), run on a player's input object: +0x80 its input
history (a ring of 64 frames, 0x28 bytes each: direction and button bits,
held and pressed buttons at +0x0A / +0x0C, counters from +0x10, a time at
+0x14), +0x84 its fighter (rob +0x34 the opponent), +0x54 a script register
("state"), +0x5C the fighter's control sets. While the opcode byte at pc is
below 0x50 it calls `move_script_ops[op]` (0x0C29F6CC, `{ handler,
instruction size }`). An instruction is `{ u8 op, s8 b1, u16 h2 }` and, by
size, `u32 w4, w8, wC`. A handler returns 0 to go on or ends the run: 1 / 2 a
move was chosen (rob +0x64 = the move word: motion id in the low 15 bits,
flags above), 3 no match, 4 / 5 wait and resume next frame. The flags
above the motion id are the action word's own bytes (`rob_action_rec`):
bits 16-23 the lever byte, bits 24-31 the action type that indexes
`rob_action_handlers`, so `movescript.py`'s `|0x500` is a type 5 throw,
`|0xe00` type 14, `|0x2NN` / `|0x3NN` types 2 / 3 with lever NN. Checked
live (2026-11-06, 3,659 requests from a two-player session and CPU-vs-CPU
fights): the requests carry the script's type and lever (`OUT_DW_F00_UM_L`
`|0xe00` arrives as type 14, lever 0), except that a preset throw of type
5 gains lever bit 0x08 on the way (`coli_attack.c`: `move_word | 0x80000`;
`JDK_TA_NA_IKAZUTI` `|0x500` arrives as type 5 lever 0x08, its victim's
`_YA` as type 6 lever 0x08). Bit 15 (0x8000) was never set.

Matching runs backward in time like a regular expression with backtracking:
an input test that fails on the current history frame steps one frame back
while its window (op 13) lasts (`script_step_back`); a test that fails with
no window left resumes the last choice point (ops 10 / 11, a 32-entry stack
of input state and pc; `script_backtrack`), or fails the run when there is
none. So `new_press 6 / allow 3, 4 / input 5 / input 6 / allow 4 / input 4 / input 5
/ move AKI_TA_TETUZAN` in the P+K group is 4 6 6 P+K, checked newest first.

There are two ways in: a script from a pc, advancing by each instruction's
size; or, from a branch window (`move_branch_take`), a list of instruction
pointers, each successful step taking the next one (`script_advance`), the
list ending with a null pointer.

**Control sets** (`ctrl_<chr>.bin`, offsets from the file start): one `u32`
offset per set, then the end offset and 0. Fighters with stances have several
sets, chosen by rob +0x550 (timed move command 34): LEI and SHN 5, KAG, LAU,
SAR and VAN 2, the others 1 (29 in all). A set is `{ u32 script[6], u32 table,
u32 count }`: script 2 is the neutral command list (0x0C05CE80 runs it every
frame), 0 runs with state = 0x2000000 | direction, 4 / 5 with state =
direction; ops 12 / 77 index the table in branch lists (ctrl_AKI's count of 4
runs past the end of the file, where the game reads 0 or the next bytes and
treats them as no entry). Offset 0x10 is a stub (op 0) for unused slots.

| Op | Size | Mnemonic | Operands | Op | Size | Mnemonic | Operands |
|---:|---:|------|------|---:|---:|------|------|
| 0 | 4 | `fail` | - | 40 | 4 | `if_not_state_bit` | b1 |
| 1 | 8 | `resume_at` | w4j | 41 | 12 | `jump_state_any` | w4x w8j |
| 2 | 8 | `move` | w4m | 42 | 12 | `jump_state_none` | w4x w8j |
| 3 | 8 | `move_keep` | w4m | 43 | 4 | `if_cond_ge` | b1c h2 |
| 4 | 4 | `input` | h2n | 44 | 8 | `jump_cond_ge` | b1c h2 w4j |
| 5 | 4 | `allow` | h2n b1 | 45 | 8 | `move_restart` | w4m |
| 6 | 4 | `input_bit` | b1n | 46 | 8 | `move_masked` | w4m |
| 7 | 4 | `not_input_bit` | b1n | 47 | 8 | `set_resume` | w4j |
| 8 | 4 | `new_press` | h2n | 48 | 4 | `if_free` | - |
| 9 | 4 | `new_press_bit` | b1n | 49 | 8 | `set_rob338` | w4x |
| 10 | 8 | `choice` | w4j | 50 | 4 | `if_entry_current` | - |
| 11 | 8 | `choice_pc` | w4j | 51 | 4 | `if_rate_ge` | b1 |
| 12 | 8 | `continue_with` | w4j | 52 | 8 | `state_set` | w4x |
| 13 | 4 | `window` | b1 | 53 | 8 | `state_add` | w4x |
| 14 | 4 | `wait_here` | - | 54 | 8 | `if_wall_dist_le` | b1 w4f |
| 15 | 4 | `clear_wait` | - | 55 | 8 | `if_wall_dist_ge` | b1 w4f |
| 16 | 8 | `wait_frame` | b1 h2 w4r | 56 | 8 | `if_opp_wall_dist_le` | b1 w4f |
| 17 | 8 | `before_frame` | b1 h2 w4r | 57 | 8 | `if_opp_wall_dist_ge` | b1 w4f |
| 18 | 4 | `flush_input` | - | 58 | 4 | `nop` | - |
| 19 | 8 | `jump` | w4j | 59 | 16 | `move_or_hold` | b1 h2 w4m w8m wCj |
| 20 | 8 | `if_state_eq` | w4s | 60 | 8 | `move_if_side` | w4m |
| 21 | 12 | `jump_state_eq` | w4s w8j | 61 | 8 | `move_if_other_side` | w4m |
| 22 | 8 | `if_state_ge` | w4x | 62 | 8 | `move_if_opponent` | b1 w4m |
| 23 | 8 | `if_state_le` | w4x | 63 | 4 | `escape` | - |
| 24 | 16 | `switch_held_dir` | w4j w8j wCj | 64 | 4 | `hold` | - |
| 25 | 4 | `history_mark` | b1 | 65 | 8 | `jump_cond_le` | b1c h2 w4j |
| 26 | 12 | `buffer` | h4 h6 w8j | 66 | 4 | `if_frame_flag` | b1 |
| 27 | 8 | `if_bit` | b1 w4r | 67 | 4 | `if_not_frame_flag` | b1 |
| 28 | 8 | `if_not_bit` | b1 w4r | 68 | 8 | `jump_move_eq` | h2 w4j |
| 29 | 8 | `if_opp_bit` | b1 w4r | 69 | 4 | `raw_bit` | b1n |
| 30 | 8 | `if_opp_not_bit` | b1 w4r | 70 | 4 | `input_toward` | - |
| 31 | 8 | `load` | b1 w4r | 71 | 4 | `clear_all` | - |
| 32 | 8 | `store` | b1 w4r | 72 | 4 | `input_reset` | - |
| 33 | 8 | `load_opp` | b1 w4r | 73 | 12 | `set_rob1e8` | w4x w8j |
| 34 | 8 | `store_opp` | b1 w4r | 74 | 4 | `clear_branches` | - |
| 35 | 8 | `if_distance_le` | w4f | 75 | 4 | `if_card_flag` | b1 |
| 36 | 8 | `if_distance_ge` | w4f | 76 | 4 | `if_not_card_flag` | b1 |
| 37 | 4 | `if_rob62` | b1 | 77 | 12 | `move_if_card_flag` | b1 w4m w8j |
| 38 | 8 | `cancel_move` | w4m | 78 | 8 | `wait_frame_resume` | b1 h2 w4r |
| 39 | 4 | `if_state_bit` | b1 | 79 | 4 | `if_cond_le` | b1c h2 |

Operands: b1 s8, h2 u16, h4 / h6 s16 / u16, w4 / w8 / wC u32; suffixes: m a
move word, j a jump target, r a rob field offset, f a float, x hex, n an input
mask or bit (printed in numpad notation, below), s the state (its low nibble
the buttons), c a condition of `input_conditions`.

**Input** (`input_sample`, 0x0C05C6C0, every frame). The input word has the
pad's buttons in bits 0-3 and directions in 4-7, and the sampler adds six
relative directions:

| Bits | Meaning |
|------|---------|
| 0x01 0x02 0x04 0x08 | G, P, K, E (a fourth button) |
| 0x10 0x20 | the two raw horizontal directions |
| 0x40 0x80 | down, up |
| 0x100 0x200 | forward, back (0x10 / 0x20 by the fighter's facing, rob +0x18 bit 5) |
| 0x400 0x800 | up, down by the fighter's side flag (rob +0x318 bit 3, which timed command `toggle_side` flips) |
| 0x1000 0x2000 | the same by the opponent's side flag |

The button order comes from `input_combo_counters` (0x0C29F678), the hold
counters in the order P (2), K (4), G (1), P+K, G+P, G+K, G+P+K, and the
directions from the commands: crouching moves (`_SY_`) follow 2, 3 or 1, and
Akira's Tetsuzankō reads 4 6 6 P+K. Bit numbers 8-13 in the bit tests name the
six added bits. A history entry is 0x28 bytes: +0 the rob +0x18 flags, +4 the
previous input, +6 the releases, +8 the sticky raw directions, +0x0A the new
presses (`input_new_presses`, which holds a partial press a few frames so
near-simultaneous buttons count together), +0x0C the input, +0x0E a flag,
+0x10 eleven counters (frames each direction and button combination has been
held). `input_conditions` (0x0C29F94C, 16 entries) names those counters: 1-7
the button combinations, 8-15 a direction (8 2 4 6 9 7 3 1) held at least
(`if_cond_ge`, branch conditions) or at most (`if_cond_le`) N frames. The
script register holds the buttons just pressed when a command list starts, so
`jump_state_eq 0x6` is the P+K group.

`movescript.py` writes `commands_<chr>.txt`: for every move, the inputs on
each path to it, read back to front with neutral dropped, plus the state's
buttons, in numpad notation (6 forward, 2 down, 3 down-forward...; 2s / 8s and
2o / 8o the side-relative directions). Checks against the known commands:
Akira's jab P (`AKI_PU_01_VFX`), crouching jab 2P (`AKI_SY_PU_JAB`) and
Tetsuzankō 4 6 6 P+K (`AKI_TA_TETUZAN`). The listing is a reconstruction from
the paths, not the move list: a path that falls back from the G+P group into
the P attacks shows the G+P form too, timing tests (`wait_frame`,
`before_frame`) and state tests are left out, and moves reached only through
other conditions show `-`.

Checks: from the set scripts and tables, following every jump and choice,
17,056 instructions decode in the 17 files with no byte that is not an opcode
and no two instructions overlapping; that covers 88-97% of each file (the
rest is the set table and code nothing reaches). All 925 branch scripts end in
a move (834) or at a null pointer after `clear_branches` (91). Read against
the move names the scripts make sense: Akira's list has `input_bit 8` (up)
for `SKZSH_NG` in the P group (bit 7: up) and 3 3 P for `AKI_TA_PU_YOUHOU`; his
`AKI_HIKINA_NA` branches at frames 11-20 to `AKI_HIKI2MAWA_R_NA` on input
pattern 48 (input 2s, the side-relative down).

## Sprites (`spr_*.bin`)

No header: the layout is in the program. A **sprite registry** in `.data`
(81 entries at 0x0C322154) holds `{ char *name, u32 base_id, desc * }`, with
`base_id` stepping by 0x78. Each description is:

- page descriptors, 16 bytes `{ short w, h; u32 format; u32 offset; u32 0 }`
  (the texture descriptor above), ended by a record of -1;
- cells, 20 bytes `{ u32 page_id; float u0, v0, u1, v1 }`. The page id is
  global (a file owns `base_id .. base_id + pages - 1`), and the run after one
  file's pages can belong to another file (after `spr_sel_stg_cav`'s pages
  come `spr_sel_stg_djo`'s cells), so cells are matched by id.

Checks: the pages tile all 57 files exactly (unpacked size for the two
`.gz` ones). 2,942 cells, all with UVs in 0..1, cover 98-100% of each page's
visible pixels (90% for VQ pages, whose blocks bleed alpha). 13 stage-select
files share one size, so single size matches prove nothing; an earlier
reading of the registry as `{desc, name, id}` matched one of them and was
wrong for 41 files. The registry also names 24 sprite files that are not on
the disc (`spr_sel_st_*`, `spr_title*`, ...; see `docs/assets.md`).

Output: `<file>_p<n>_<w>x<h>.png` per page and `<file>.json` with the pages
and cells (UVs measured from the top of the PNG).

## Sound banks (`bg_`, `se_`, `vo_`)

```bash
python3 -I tools/dtpk.py work/assets work/sound        # 2,483 waves + 32 music streams to WAV
python3 -I tools/dtpk_seq.py work/assets/se_stage_sea.bin            # setups, programs, sequences
python3 -I tools/dtpk_seq.py all work/assets work/sound/seq          # the same for all 80 banks
python3 -I tools/dtpk_seq.py work/assets/se_stage_sea.bin --wav sea.wav --seq a8:7f:0    # played through the bank's waves
python3 -I tools/dtpk_seq.py work/assets/se_stage_sea.bin --midi sea.mid --seq a8:7f:0   # the events only
python3 -I tools/dtpk_seq.py work/assets/bg_vf41b19.bin --wav gym.wav --seq aa:2b:1 --mono   # as the game mixes it
python3 -I tools/dtpk_seq.py check work/assets                       # parse, coverage, references, ambience rates
```

DTPK containers, measured on all 80 banks and read from the sound driver
(`aicadrv.bin`; no public description found):

| Offset | Field |
|--------|-------|
| +0x00 | `"DTPK"` |
| +0x04 | bank / track number (`bg_vf1_sarah` 1, `bg_vf2_akira` 2, ...; 0 for voices) |
| +0x08 | file size (all 80 match) |
| +0x10 | `0x00FFFF02` music and effects, `0x00FFFF03` voices |
| +0x1A | most voices the bank's resident parts may hold (32 in all 80); past it the oldest is taken (0x1248, 0x1D38) |
| +0x20 | eight section offsets; 0-3 and 7 always present, 4-6 when used |

**Section 7, the wave table**: `u32` last index (count - 1), then 16-byte
records `{A, B, C, D}`. The driver writes `A >> 16` to AICA channel
register 0 (0x4430, 0x5B10).

| Field | Meaning | Evidence |
|-------|---------|----------|
| A bits 0-22 | sample offset in the file (AICA 23-bit start address) | the samples tile every file exactly from the end of the table (to 32-byte padding) |
| A bits 23-24 | AICA `PCMS`: 0 PCM16, 1 PCM8 (signed), 2 4-bit ADPCM | bytes per sample 0.5 / 1 / 2 match the loop end for every record; each decodes smooth only in its format |
| A bit 25 | AICA `LPCTL`, loop enable | register 0 bit 9; the stream player sets it explicitly |
| B | loop start (low 16 bits), loop end (high 16 bits), in samples | |
| C bit 7 | stereo: the right channel follows the left | tiling holds only if stereo records take 2 x D |
| D | bytes per channel | |

The count is the last index: reading it as the count dropped one wave per
bank (for music, the final chunk). The ADPCM is the AICA / Yamaha 4-bit kind
(`tools/aica_adpcm.py`, the chip's formula as Flycast's `DecodeADPCM` has
it): decoded it is as smooth as real audio (roughness 0.20 against about
1.2 for the raw bytes). The sign applies to the truncated delta,
`(step * (2n + 1)) >> 3`; the decoder rounded the signed product down
until 2026-11-05, which lost one unit on most negative steps and drifted
waves downward (effects that fade out ended at -4,600 to -18,300; now
within a few units of 0, and identical to Flycast's on all 2,444 ADPCM
waves). One-shots
set LPCTL on a 24-sample loop at their end (`loop end = samples - 4`), which
the envelope cuts off; real loops (ambient effects) have LPCTL and a longer
loop; 24 effects with loop points from 0 have LPCTL clear and are one-shots.
Music banks are one stream cut into chunks of equal size (0x6B34 bytes for
`bg_vf2_akira`) that join without jumps; `stream.wav` is the chunks in order.

Sections 0-3 drive the waves: section 0 maps MIDI channels to programs,
section 1 holds the programs, section 2 the velocity curves and section 3 the
sequences the game starts. All offsets below are from the start of the
section unless a record says otherwise.

**Section 0, channel setups** (read at bank load 0x11F4, and by `A0 19 n`
0x12D8): `u8` count, pad, `u16` offset of the default setup, then `u16`
offsets of setups 0 .. count - 1. The default setup is applied when the
bank loads; `A0 19 n` keeps the parts with flag bit 0 and adds setup n's
(0x1340 -> 0x19E0, 0x1390). A setup is `u8` last part index (0xFF none),
`u8` bank level (0xFF keep), 6 bytes, then 32-byte parts:

| Part offset | Field | Evidence |
|-------------|-------|----------|
| +0x00 | flags: bit 0 resident (kept by `A0 19`, its voices get flag 0x40), bit 1 program from section 4, bit 7 set in all | 0x19E0, 0x3B18, 0x3030 |
| +0x01 | MIDI channel (low nibble) | 0x13D0 |
| +0x02, +0x03 | program bank and program in section 1 | 0x3060, 0x3088 |
| +0x04, +0x05 | note range | 0x38B0 |
| +0x06, +0x07 | velocity range | 0x38B0 |
| +0x08 | DSP send: low nibble AICA `ISEL`, high nibble `IMXL` (CC 12 / 13 set it) | 0x50B4, 0x50E8 |
| +0x09 | direct level (AICA `DISDL`) for tones whose +7 has bit 7 | 0x3C7C |
| +0x0A | volume (CC 7) | 0x51D0 |
| +0x0B | pan (CC 10, through the table at 0x7E4C) | 0x51EC |
| +0x0C | `s7` transpose, semitones | 0x4330 |
| +0x0D | `s7` detune, 1/64 semitone | 0x4330 |

Bytes +8 .. +15 are copied into the driver's channel record; +14 and +15
are not traced. Every music bank's default setup has one part, channel
15, flags 0x83, playing the section 4 program.

**Section 1, program table** (0x3050): `u8` last bank, pad, `u16` offsets
of bank tables; a bank table is `u8` last program, pad, `u16` offsets of
programs (from the bank table). CC 0 sets a part's bank, a program change
its program (0x3020).

**Programs** (section 1, and the single program of section 4): `u8` type
(0x395C), bytes 2-5 copied into the channel record (0x14B0), `u16` offsets
of up to four layers at +8 / +10 / +12 / +14 (from the program, 0 none;
0x38F4). Every layer sounds. A layer is a 0x40-byte header (+6: velocity
curve, an index into section 2, 0x3F34) followed by 64-byte tones from
+0x40. The type picks the tone:

| Type | Layer header | Tone choice | Evidence |
|------|--------------|-------------|----------|
| 0 keyboard | +0 `u8` last tone, +2 / +3 velocity range | first tone whose +0 (top note) is at least the note; the pitch follows the note | 0x3970-0x39D4, 0x4320 |
| 1 note map | +0x40 low note, +0x41 high note | one tone per note through `u16` offsets (from the layer) at +0x42 | 0x3A14 |
| 2 index | +0 `u16` last tone | tone = note + 128 x ext (ext from the 4-byte note-on) | 0x39D8 |

In types 1 and 2 tone +0x20 / +0x21 is the velocity range and the pitch
does not follow the note. The playback rate of a tone is

`P = 256*(note - 60) [type 0 only] + 256*s7(part[0xC]) + 4*s7(part[0xD]) + 256*s8(tone[0xA]) + 4*s8(tone[0xB])`

plus LFO (tone +0x1C) and pitch bend, and **rate = 44100 x 2^(P/3072)**
(0x4314, 0x4340, 0x33DC). Tone +2 is the wave: 0x100 + n is wave n of
section 7, or of the bank whose number is in header word +0x50 when its
bit 31 is set (0x4518; no bank sets it); below 0x100 a wave built into the
driver (0x4458).

**Section 2, velocity curves** (0x2FD0): `u32` last index, then 128-byte
curves. Every bank has two (0x104 bytes).

**Section 3, sequence table** (0x7B00): `u8` last entry, 3 pad bytes, then
4-byte entries `{u16 list offset, u8 id, u8 class}`; a list is `u8` last
number, 3 pad bytes, `u32` script offsets (0 none). The game starts a
sequence with a request word `{class, id, number}` (class 0xA8-0xAD) that it
writes to the ring at AICA 0x800400 (read at 0x06F0; class A8-AD goes to
the sequencer at 0x76A0). The search covers all eight loaded banks (bank
records at 0xEB00), so a request does not name a bank. The names in
`main.bin`'s sound-id table (12-byte records at 0x0C17E2AC: `u32` request
word, then the name) map requests to banks, e.g. 0x000110AA is
`C_SE_VF2_AKIRA_AKIRA` (class 0xAA, id 0x10, number 1).

A script starts with a header byte h (0x7B8C, 0x7D80) and runs in one of
16 sequencer slots (0xF498, 0x30 bytes each; 0x71D0):

| h | Script |
|---|--------|
| bit 7 set | event stream: `[h, id, priority]` when the next byte is below 0x80, else `[h]`; the events follow |
| bit 6 | events go to the synthesizer (0x7320) |
| bit 5 | events go to MIDI out (0x7320, queue 0x2BAC) |
| bit 4 | takes over a running script of group h & 15 (0x7C4C) |
| bit 7 clear | song: `[h, id, priority, pad]`, then a list of `u32` words (0x737C) |

Song list words, offsets from the header:

| Word | Action |
|------|--------|
| `0x00oooooo` | play the stream at +o, then go on |
| `0x80ssddvv` | send the message `ss dd vv` (the first one also sets the bank level, 0x740C) |
| `0x90oooooo` | continue the list at +o (all 8 songs loop this way) |
| other | end, after playing the stream at +o when o is not 0 |

**Events** (0x7280): MIDI channel messages with running status; 0xFF ends
the stream. When the last data byte of an event has bit 7 set no delta
follows; otherwise a delta in ticks follows, one byte, or two when the
first has bit 7 (`(b0 & 0x7F) << 7 | b1`).

| Status | Bytes | Meaning | Evidence |
|--------|-------|---------|----------|
| `8x` | n | note off | 0x3650 |
| `9x` | n v | note on (v 0 off) | 0x382C |
| `Ax` | a b | passed to the synthesizer's Ax handler, not traced | 0x4608 |
| `Bx` | c v | controller, table below | 0x4DAC, 0x4DD4 |
| `Cx` | p | program change | 0x3E28, 0x3020 |
| `Dx` | n v e | in event streams: note on with ext e (`9x n v`, e in the low byte; tone index of type 2) | 0x72BC |
| `Dx` | p | in songs: channel pressure, ignored | 0x3ED4 |
| `Ex` | b | pitch bend (one byte, into the second data field) | 0x72E4, 0x3ED8 |
| `Fx` | a b | ignored | 0x3F1C |
| `A0` | dd vv | driver control, table below | 0x4648, 0x76A0 |
| `A8`-`AD` | id n | start sequence `{class, id, n}` | 0x76A0, 0x7B00 |

Controllers the driver acts on (0x4DD4; all others are ignored): 0 bank,
1 modulation, 2 breath, 7 volume, 10 pan, 12 DSP send select, 13 DSP send
level, 0x40 damper, 0x41 mono (clears the note stack), 0x78 all sound off;
0x27, 0x2A, 0x2C, 0x2D set volume, pan and the DSP sends without updating
the playing voices.

Driver control `A0 dd vv` (synthesizer table 0x46CC, sequencer table
0x76C4). An `A0` a sequence sends gets bit 7 set in its low byte (0x7500)
so it does not stop its own sender.

| dd vv | Meaning | Evidence |
|-------|---------|----------|
| 00 00 | nothing | 0x4774 |
| 00 01 / 02 / 03 | all banks: like 11 / 12 / 13 | 0x4774, 0x7734 |
| 00 0C / 0D / 0E | route this sequence to the synthesizer / MIDI out / both | 0x75FC, 0x7608, 0x7614 |
| 00 11 | bank: all voices off, sequences stopped | 0x4774, 0x7750 |
| 00 12 | bank: voices of setup parts off, parts reset, songs stopped | 0x4774, 0x7750 |
| 00 13 | bank: voices of resident parts off, event streams stopped | 0x4774, 0x7750 |
| 02 / 03 | pause / resume the bank's songs | 0x795C, 0x79AC |
| 09 r | fade-in rate for the next song (table 0x79FC) | 0x78A4, 0x7DC4 |
| 0A r | fade out the bank's songs | 0x78DC, 0x7164 |
| 19 n | select setup n (section 0) | 0x4A40 |
| 1C v | bank level | 0x4A8C |
| 20 p | from a sequence: yield to a running copy | 0x7518 |
| 24 v | from a sequence: sets the word at 0xC3B0 | 0x7620 |

`PORTSEOFF` in the sound-id table is the request `A0 00 13` (number 0).

**Timing**: one tick is one AICA timer A period. The driver writes 0xD4 to
`TIMA` (0x15E0, 0x0C84): 0x100 - 0xD4 = 44 samples at 44,100 Hz, **1,002.27
ticks per second**. The tick routine (0x7108, from the FIQ at 0x0CE4)
advances every slot by a fixed 0x10000 (0x7D88); there is no tempo command.
`tools/dtpk_seq.py --midi` writes division 441 and tempo 440,000 us so one
MIDI tick is one driver tick. MIDI keeps the events, not the sound: with
type 1 and 2 programs the note number picks a tone, not a pitch (in the
music banks note k is chunk k, so a General MIDI player plays a rising
scale), and the ambience songs play pitched copies of effect waves. To
hear a sequence, use `--wav`.

What the 80 banks use: 3,218 scripts (970 streams with header 0xC0, 1,120
each with 0xD1 and 0xD2, 8 songs with 0x40), 6,379 events: 4,866 4-byte
`Dx` note-ons (ext 0: 4,349, ext 1: 517), 1,292 `9x`, 181 sequence starts
(`A9` 2, `AA` 179), 33 `A0 00 13`, 7 `A0 00 00`; no controllers, program
changes, bends or `Fx`. Song lists hold 16 messages (`A0 00 12`, `A0 19
00`) and 8 loops. Music banks play the section 4 program on channel 15:
`aa:10:1` (`bg_vf2_akira`) starts the left stream (tones 0-42) and 18 ticks
later the right one (tones 43-85, the same waves with tone +6 pan 0x1F
against 0x0F); each 4-byte note-on starts one chunk, 1,714 ticks apart.
The right stream's chunks play 18 ticks after the left's, or 17: the
start race below.
The 8 songs are the stage ambiences `a8:xx:0` of `se_stage_cave`,
`dryleaf`, `gwochina`, `harbor`, `nodance`, `sea`, `snow` and `temple`.

**Playing a sequence** (`--wav`): `tools/dtpk_seq.py` runs the sequencer as
`--midi` does and keys on AICA voices the way the driver does, then mixes
them as Flycast's AICA model does (`core/hw/aica/sgc_if.cpp`). Per voice:
the tone and part from the note-on (setups switched by `A0 19 n`, parts
reset by `A0 00 12`); the rate from the pitch formula with the chip's
octave / `FNS` (the table at 0x8050 rounds: all 768 entries, so the music
plays at 32,019.6 Hz, not the nominal 32,010); the total level `TL = (255 - L) >> 1` with `L` the
velocity curve at the velocity times tone +8, part volume and the bank
level for setup parts (0x4270; the channel and master factors are taken as
127); pan and `DISDL` from the tone or the part; the envelope from tone
+0x0C / +0x0E, stepped as the chip steps it; levels in 1/16 octave steps.
Voices end:

- at a note-off of their channel and note (0x2208), or an `A0 00 1x` that
  covers them;
- if one-shot (tone +1 bit 7), at the first tick after the play position
  reaches the loop start (0xA74). The driver checks one voice per call, so
  in the game this can come a few ticks later;
- a looped voice past attack once its envelope is more than 0x200 down
  (48 dB): the driver frees it (0xA7C, threshold work +6 = 32 << 4);
- the oldest resident voice when header +0x1A voices are already held, and
  the oldest of all at 64.

Starts (the race): the driver's start routine runs a new script's first
events at once [0x7D80 -> 0x71D0], wherever the main loop is, and every
message a stream sends, start requests and note-ons alike, goes to a
256-entry ring [0x2B1C] that the main loop drains only at the top of its
next pass [0x668 -> 0x2AE4 -> 0x45C0]. The timer interrupt [0x7108] only
counts slots down; the slot loop [0x71D0] fires what is due at the end
of each pass, so a start sent from there reaches the drain within the
same tick. A start sent at its script's own time 0 waits for the rest of
a pass, and if a tick falls in that time the started script counts from
after it: one tick late. Nothing in the main loop waits for the tick, so
which happens depends on how long a pass runs. Recordings show both:
of five plays of two-stream music in three Flycast recordings, the
second stream ran 17 ticks behind in two (`bg_vf41b19`, `bg_vf41b61`)
and 18 in three (`bg_vf41b03`, `bg_vf41b18`, `bg_vf41b22`); in stereo
recordings the left-right lag is read directly (749 and 791-795
samples at 44.1 kHz). `--start-lag` (default 0, the script's own
delta) picks the outcome: a start sent at its script's time 0 takes
effect that many ticks later. In the music banks `aa:xx:1` starts the
first stream at its time 0, whose chunks start one tick late when the
race goes that way, and the second 18 ticks on (15 in `bg_vf41b42`,
`b55`, `b62`) from the slot loop; the 0x71B0 catch-up only runs in MIDI-clock sync mode
(byte 0xFE with mode 2, [0xE70]), so in the game a tick is 44 samples.

`--mono` mixes `(L + R) / 2` into both channels, as the chip's mono bit
does (Flycast `sgc_if.cpp`); the gymnasium recording is mono.

Not rendered: the DSP (every ambience voice has a send, so the game adds
reverb), the filter and LFOs (no tone of the two banks below uses them),
pan patterns (layer +0x28), and the right half of stereo waves (a voice
plays one channel; the driver keys no second one that we have found).

`bg_vf2_akira aa:10:1` comes out as the stream, left and right 18 ticks
(792 samples) apart (17 with `--start-lag 1`). Mid-chunk the render matches the chunk resampled to
the floating-point floor. Each chunk is a one-shot that reaches its loop
start 141 samples (3.2 ms) after the next chunk starts, because 1,714
ticks of 44 samples are shorter than the chunk. `--tick 44.1048` makes them
join end to start. The game overlaps them: see `bg_vf41b19` below.

`se_stage_sea a8:7f:0` (`A_AMB_SE01_SEA` in the sound-id table) holds 130
of the 596 notes of two passes past the end of their wave. These tones are not
one-shots and their envelopes have no decay (D1R, D2R 0), so the voice
sits in its 24-sample end loop until the note-off. That loop holds the
faded-out end of the wave (about 8 to 10 units of 32,768), so the held
notes are silent there. (Before the ADPCM fix the waves drifted to
-4,600 to -18,300 by their end and the loops looked loud by RMS, but
that was DC.)

`bg_vf41b19 aa:2b:1` is the gymnasium stage's music, and a Flycast
recording of that stage (`pw-record`, 85.8 s) settles the tick. The bank
works like `bg_vf2_akira`: tone 0 is a 1,704-sample intro (the last wave in
`stream.wav`), then tones 1-32 (waves 0-31, 33,720 samples) start 1,053
ticks apart and loop from tone 1; tones 33-65 are the same waves for the
second stream. Tracked against `stream.wav` (0.25 s windows, sub-sample
peaks), the recording follows each chunk at one rate and steps forward
2.69 ms (sd 0.17 over 22 joins) at every join: the next chunk starts
before the previous one has played out. An exact join gives no steps; the
same tracker on a `--tick 44` render reads 2.484 ms against 2.493
predicted. So the game overlaps the chunks: `--tick 44` is right, and
`--tick 44.1048` is only for comparison.

What the recording shows about Flycast rather than the game: its
`UpdatePitch` shifts `1024 | FNS` right for negative octaves with no
fraction bits (1,487 >> 1 = 743), so it plays these chunks at 31,998.6 Hz.
Within a chunk the recording runs 657 ppm slow against 32,019.6, which
matches. At that rate the steps imply ticks of 44.02 samples on average.
Flycast's timer counts once per sample and the driver reloads `TIMA` in its
interrupt handler, so a handler that runs a sample late now and then would
add this; that is not checked.

The recording is mono (left and right identical, so the chip was in mono
and the two streams sum), and the second stream runs 17 ticks behind the
first (814 samples at 48 kHz; fits on 10 windows gave 808-829), not the
18 of the script's delta: the start race above, in its late case. With
`--mono` and `--start-lag 1` the render has the same comb
(748 samples at 44.1 kHz), and 0.25 s windows of the recording match it
with a median correlation of 0.82 (quartiles 0.64, 0.88), against 0.66
(0.49, 0.73) with `--start-lag 0`; the rest is the fight on top.

Checks (`tools/dtpk_seq.py check`): every script parses to its 0xFF or the
end of its list (`se_stage_dural` has no section 3: offset 0); the parsed
records cover sections 0-3 except zero padding of 1-5 bytes and, after the
last stream of the 8 stage banks, the bytes `2f 00` (the end-of-track meta
event of a standard MIDI file, left over from conversion); all 181 starts
name a script in their own bank; 5,480 of 5,506 note-ons reach a tone
(4,860 with the default setup, 620 after `A0 19 00`), and all 4,997 tones
name a wave that exists. The 26 others are `se_stage_harbor`'s song
playing channel 3, which its setup 0 lacks: the song is `se_stage_sea`'s
(same notes), whose setup has a channel 3 part.

Unknown: the `A0` codes 01, 04, 05, 07, 10, 11, 18, 1A, 1B, 28, 29, 2F,
30, 38, 39 (acted on, not traced), what the `Ax` handler does, setup
bytes +2 .. +7, part bytes +14, +15, program bytes 1 and 4-7, and
sections 5 and 6, and how long a main-loop pass runs (which decides the
start race; 2 of 5 recorded plays went late). A music chunk is 54,888
samples at 32,019.6 Hz (the chip's `FNS` for pitch -6 +29/64) = 1.7142 s
but 1,714 ticks are 1.7101 s, 0.24 % shorter, so each chunk overlaps the
next by 4.1 ms; the gymnasium recording shows the game does overlap them.

**Section 4**: one program in the format above (76 banks, always type 2 at
+0 with one layer at +0x10), used by parts with flag bit 1 (0x3030). The
"last index 2" read before is the type byte; the layer header's +2 / +3 is
a velocity range, and tone +0x20 / +0x21 too. Tone fields the driver reads:

| Tone offset | Field |
|-------------|-------|
| +0x00 | top note (type 0) |
| +0x01 | bit 7: one-shot: the driver keys the voice off once the play position reaches the loop start (voice flag 8, 0x3B24; checked through the AICA monitor at 0xA74) |
| +0x02 | `u16` wave: 0x100 + n is wave n of the bank, below 0x100 a built-in driver wave |
| +0x04 | `u16` start skip in samples (0x44A4 moves the start and loop points; 0 in every tone) |
| +0x06 | pan: bit 7 set (3,340 tones) takes the part's pan, else the low 5 bits (0x43C0) |
| +0x07 | direct level `DISDL`: bit 7 takes the part's +9, else the low 4 bits (0x3C74) |
| +0x08 | tone level, a factor of the total level (0x3F78, 0x4270) |
| +0x0A, +0x0B | pitch: `s8` semitones, `s8` 1/64 semitones |
| +0x0C | `u16` to AICA +0x10: `AR`, `D1R`, `D2R` (0x3B8C) |
| +0x0E | `u16` to AICA +0x14: `RR`, `DL`, `KRS`, `LPSLNK` (0x3B90) |
| +0x10 .. +0x19 | filter: `FLV0`-`FLV4` (`u16` & 0x7FF, x 4) and its rates from the odd bytes; `FLV0` 0 turns the filter off (0x3BB4) |
| +0x1C | LFO |
| +0x1E, +0x1F | DSP send: bit 7 of +0x1E takes the part's +8, else +0x1F (`ISEL`, `IMXL`) (0x3C88) |
| +0x30 | `u16` to AICA +0x1C, the LFO register (0x3B94) |
| +0x20, +0x21 | velocity range (types 1 and 2) |

**Section 5**, a second voice kind (9 effect banks): `u32` last index, `u32`
offsets to entries; an entry is 8 bytes (entry[3] = number of sub-records)
then sub-records `[len, ?, u16 wave, key lo, key hi, ...]`. Sub-record + 0xC
holds `(count, offset)` pairs, one per parameter, pointing at key-sorted
points (0x6F48). Parameter 4 is the pitch, points `[key, ?, u16 value, s16
slope]`.

**Playback rate**, from the driver's pitch code:

- Tones (0x4314, 0x33DC): `P = 256*(note - 60) + 256*s8(tone[0xA]) +
  4*s8(tone[0xB])`, plus track transpose and detune from the sequence and an
  LFO. The AICA register gets octave `P / 0xC00` and FNS from the table at
  0x8050, which is `1024*(2^(v/3072) - 1)` (checked entry by entry), so
  **rate = 44100 x 2^(P/3072)**.
- Section 5 (0x6BCC, 0x6EFC): **rate = 44100 x 2^((value - 0xC00)/384)**.
- A wave is written at the rate its tones give at note 60 (most common,
  ties to the earliest tone: the character banks pair each tone with a
  variant a semitone lower). `manifest.tsv` lists every tone's rate.

| Banks | Rate |
|-------|-----:|
| VF1 / VF2 / VF4 music (tone pitch -6, +29) | 32,010 Hz |
| The three Final Tuned tracks (-12, 0) | 22,050 Hz |
| All voices (-18, +29) | 16,005 Hz |
| Effects | per tone: 44,100, 41,625, 32,010, 22,050, 16,005, 11,025 ... Hz |

Checks: the older music is 40 cents flat at 22,050 Hz and within 5 cents of
in tune at 32,010 Hz (645 cents higher); voice F0 at 16,005 Hz is 381 Hz
(Aoi, Pai) and 239-341 Hz (the men), normal for shouting.

The 57 stage-ambience waves `tools/dtpk.py` once marked `none` are in
section 1 type 0 programs of their own bank (it read only sections 4 and
5), played by the stage's song `a8:xx:0` (all eight stage ambience banks).
These programs lay waves out as a ladder of key zones: each tone covers the
12 notes up to its top note (59, 71, 83, ...) and is pitched one octave
lower than the one before, so all tones of the layer give the same rate at
the lowest note of their zone. That rate is what the waves are written at
(`rate_source` `ladder`, 52 waves): `se_stage_cave` 1-5 22,250 Hz;
`dryleaf` 0-5, `harbor` 0-7, `sea` 0-7, `temple` 9-14 16,005 Hz; `gwochina`
0-3 8,003 Hz; `harbor` 8-12, `nodance` 0-4, `sea` 8-12 22,050 Hz. The songs
agree: 22 waves are played only at that rate and 4 mostly; 11 (`sea` and
`harbor` programs 0:1-0:3) are played as pitched instruments over a range
of notes; 15 are not played. Not settled, written at their note-60 rate
(`note60`): `se_stage_dryleaf` 6 (11,025 Hz) and 7 (2,756 Hz), and
`se_stage_snow` 0, 1, 2 (22,050, 16,005, 22,050 Hz), single tones the
songs play at several pitches (snow 0 not at all). The wave link word +0x50
is 0 in every bank, so no bank takes waves from another.

## 2D scenes (`*_aet.bin`, `tools/aet.py`)

```bash
python3 -I tools/aet.py work/assets/ftdemo_aet.bin              # readable dump
python3 -I tools/aet.py all work/assets work/aet                # all 30 files to JSON
python3 -I tools/aet.py check FILE MEM BASE [STATE]             # against a RAM dump
AET_SNAP_OUT=work/aet_live/sel1 gdb -q -batch -nx -x tools/flycast_aet_snap.py   # dump from Flycast (GDB build)
python3 -I tools/aet.py checkdir work/aet_live/sel1 work/assets # check every set in a dump
```

The menu, select-screen and attract overlays. The program keeps the 30
file names in `aet_file_table` (set 0 `nick_sk_base` .. 29 `sel_stg_ran`);
`aet_set_load` reads a file whole into the AET stack memory and
`aet_file_relocate` adds the buffer address to every non-zero offset in it.
There is no texture data: a scene names sprite sets (`spr_*.bin`) and
texture numbers. All offsets are from the start of the file. The field names
come from the matched C (`libsprite/src/aetset.c`, `aet_anim_if.c`,
`aet_play.c`); a word nothing reads is listed as unknown.

- **header**, four offsets: the scene count; the scene table (one offset per
  scene); a second table of one offset per scene (only
  `aet_scene_table2_get` reads it and nothing calls that; in every file each
  points at the scene's name string, `AET_FTDEMO_LINE`); the pair table, two
  words per scene: the texture table and the sprite-set list.
- **scene**, 0x18 bytes: composition count and offset (0x2C each; the last
  one is the one an object plays, `aet_main_comp_get`), image count and
  offset (0x18 each), and a third count and offset (0x18 each, never
  relocated; 0 in all 83 scenes).
- **composition**, 0x2C bytes: +0x08 / +0x0C start and end frame (float),
  +0x20 visible byte (set at run time by `aet_comp_visible_update`), +0x24
  layer count, +0x28 layer offset. +0x00 .. +0x04 and +0x10 .. +0x1C are not
  read; they hold 640 / 480 (or the size of the piece), 60.0 or 30.0, and
  three floats 0 .. 1.
- **layer**, 0x8C bytes: +0x00 name; +0x04 / +0x08 start and end frame;
  +0x0C frame offset and +0x10 time scale (a composition layer draws its
  child at `(frame - start) * scale + offset`); +0x14 .. +0x30 eight floats
  the game writes each frame from the eight tracks (anchor x, y, position
  x, y, scale x, y in percent, rotation in degrees, opacity in percent);
  +0x34 visible byte (run time); +0x35 blend (4 additive, any other value
  normal); +0x37 a byte that suppresses the draw; +0x38 flags (bit 0: shown
  only from start to end frame); +0x3A type (0 image, 1 composition,
  2 a measured box); +0x3C the image or composition; +0x40 parent layer;
  +0x44 .. +0x80 eight tracks `{ key count, key offset }`; +0x84 / +0x88
  marker count and offset. Bytes +0x36 and +0x3B are not read (always 0).
- **key**, 16 bytes `{ frame, value, in slope, out slope }`. Between two keys
  the value is the cubic Hermite blend of the two values with the earlier
  key's out slope and the later key's in slope (`aet_key_curve_eval`);
  before the first key and after the last the value holds.
- **marker**, 8 bytes `{ float frame; name }`.
- **image**, 0x18 bytes: width, height, picture rate, picture count, picture
  offset, and +0x14 the current picture (run time,
  `aet_image_picture_set`). **Picture**, 0x14 bytes: an index into the
  scene's texture table and `u0, v0, u1, v1`.
- texture table: texture numbers indexed by picture; sprite-set list: set
  numbers ended by -1.

Every byte of 29 files lies in one of these structures except 1-3 zero
bytes padding strings. The data uses less than the code allows: no markers,
no parent links, no type-2 layers, no third-table entries, one picture per
image, time scale 1 everywhere; compositions form a tree under the main one
and one child composition can sit under several layers. `adv_rank_aet.bin`
(1,149 bytes, set 2, which no code loads by number) is broken: its header is
at +0x438 (`--header 448,44c,464,45c` reads it: one scene, three layers)
and its first word is 0, so `aet_file_relocate` would read the scene count
through a null pointer.

Totals: 83 scenes, 607 compositions, 3,530 layers, 1,308 images, 48,349
keys in the 29 files with a header.

**Live check.** `check` compares a file with the copy the game holds
(dumped from Flycast in attract mode and on character select: `ftdemo`,
and `sel_cha_sml`, `sel_cha_icon`, `sel_ft_cha1-3`, `sel_ft`,
`sel_profname`, five snapshots) and with the AET object and still tables:

- the words the parse says the loader relocates are exactly the words that
  differ by the load address (3,232 in `ftdemo`, 8,801 in `sel_ft`, 17,194
  over the eight files; none missing, none extra);
- every other differing byte is a layer track float or visible byte;
- each object's scene, composition and layer pointer lands on a structure
  start, the layer is the one its name finds, and replaying the draw pass
  (`aet_objs_draw` down to `aet_layer_tracks_eval`, children at their child
  frame, stills at frame 0) gives the stored floats bit for bit: 810 words
  in `ftdemo`, 1,125 per select snapshot. The frame drawn is the object's
  frame word (the task steps, then draws); one step back fails on the
  animating objects. One attract snapshot was taken inside the draw pass
  (`aet_cur_obj` is object 1 of objects 0-5) and fits the previous pass plus
  part of object 1's draw;
- the bits match only with round-to-nearest and a single rounding in `fmac`
  (round-to-zero misses 4-8 words, a rounded product 1). That is how Flycast
  ran the code; real hardware was not measured.

## Stage collision (`st_*_col.bin`, `tools/stage_coli.py`)

```bash
python3 -I tools/stage_coli.py work/assets/st_djo_col.bin --obj djo.obj   # readable dump, OBJ of the polygons
python3 -I tools/stage_coli.py all work/assets work/coli                  # all 18 files to JSON + OBJ, counts
python3 -I tools/stage_coli.py check work/assets                          # offline proof (below)
python3 -I tools/stage_coli.py floor work/assets/st_yam_col.bin 0 12.6 0  # field_floor_height at a point
python3 -I tools/flycast_state.py STATE work/main.bin --dump ram.bin
python3 -I tools/stage_coli.py live ram.bin                               # against the game's copy
```

The floor, walls and ring-out ground the fighters collide with. Each stage
record names its file (+0x1C); `stage_coli_load` reads it whole into the
buffer of the static heap record `field_load` (the record `{ addr, size
0x8000, name }` is at 0x0C360000; the buffer was at 0x0CDA01A0 in the saved
states), keeps the address in `field_coli` and calls `field_coli_set(0,
buffer)`. The first argument is the address the file was built for: 0, so
every pointer in the file is an offset from its start, and
`field_coli_relocate` adds the buffer address to each one. Field names come
from the matched C (`libbase/src/field.c`, `libaction/src/rob_coli.c`) and
the hand-written hit tests (`coli_segment_hit`, `coli_sweep_hit` and their
helpers, `source/asm/0c04f520.s` .. `0c04f900.s`); a word nothing reads is
listed as unknown. Coordinates are world units: the ring floor is at y 4.0
(12.6 on the raised YAM, DES and BAN rings), the fighters start at x = -1
and +1.

- **header**, 0x4C bytes (`mode.c` copies exactly 19 words of it around
  `rep_load_end`): +0x04 version 0x3F2 (the hit tests stop with "Collision
  data version error!!" on any other); +0x08 vertices; +0x0C polygons;
  +0x10 grid; +0x14 cell table; +0x18 the breakable-wall root, or 0; +0x1C
  a group table, or 0; +0x24 the block-pointer array; +0x30 polygon count;
  +0x34 cell count; +0x40 the entry count of the group table and the block
  array. At run time `field_coli_set` clears +0x44 and +0x48 (nothing reads
  them) and stores the first block pointer in +0x28. Not read: +0x00 (the
  offset of the block array in 13 files, 0x78 / 0xA8 short of it in the
  four with breakable walls, 0x24C8 in the two debug files), +0x20 (0),
  +0x2C (the vertex count), +0x38 (the breakable-wall count), +0x3C (1).
- **grid**, 0x14 bytes: x and z of the corner of cell 0, 1 / cell size
  (0.5: 2-unit cells), `u16` width, `u16` row count (not read), `u16` cell
  count, `u16` pad. A point's cell is `trunc((x - x0) * scale) +
  trunc((z - z0) * scale) * width`, none unless 0 <= cell < count. Grids
  are 10 x 10 to 15 x 15 cells centered on the ring; `st_debug0` has one
  100-unit cell.
- **vertex**, 12 bytes: x, y, z.
- **polygon**, 0x3C bytes: +0x00 off (non-zero: every test skips the
  polygon unless the caller passes flag 2; `coli_flag_list_set` writes 1
  when a breakable wall breaks); +0x04 kind bits: 0x100 floor, 0x200 wall,
  0x400 ring-out ground, 0x80000000 also out (`rob_floor_check` counts a
  sphere over 0x400 or 0x80000000 as over a drop), bit 0 a breakable wall;
  +0x0A edge count (byte); +0x0C slope, the normal's angle from straight
  up as a 16-bit angle (0 flat, 0x4000 vertical, -0x8000 facing down;
  `rob_wall_coli_side` pushes a fighter out of anything steeper than
  0x1000); +0x0E the normal's heading, `atan2_s16(n.x, -n.z)` (the wall
  side the fighter hit is the fighter's facing minus it); +0x10 the breakable wall it
  belongs to, or 0; +0x14 a pointer, relocated when set (0 in every file);
  +0x18 the unit normal and +0x24 the plane distance (`n . p = d`, the
  front is `n . p > d`); +0x28 five vertex pointers, the last repeating the
  first: a point is inside when `(v[i+1] - v[i]) x (p - v[i]) . n >= 0` for
  every edge. +0x08, +0x09 and +0x0B are not read (0).
- **cell lists**: null-ended lists of polygon pointers, before the cell
  table, which holds one list pointer per cell; cells with the same
  polygons share a list. `coli_cell_table_relocate` relocates every
  non-zero word from the lowest list up to the table.
- **breakable walls** (+0x18): the root word points at a null-ended array
  of records, 0x14 bytes each: +0x01 the wall's number (stored to
  0x0C292808 when it breaks), +0x04 a null-ended list of its polygons;
  +0x00, +0x02 (0) and three floats at +0x08 (near the panel) are not read.
- **group table** (+0x1C): lists of records whose two polygon lists
  `coli_groups_relocate` relocates; no file has one.
- **block array** (+0x24): one pointer, to the last 0x100 bytes of the file,
  all zero; nothing reads the block.

How the game uses it: `coli_segment_hit(out, from, to, flags)` walks the
grid cells from `to` back toward `from` in steps of at most 1 unit, tests
the polygons of each cell that pass the flag filter and that the segment
crosses from behind (`from`) to in front (`to`), keeps the hit nearest `to`
and stops after the first cell with a hit. The filter: a switched-off
polygon needs flag 2; 0x400 / 0x80000000 need 0x40; a wall needs 4 and not
0x20, and 0x10 unless its slope is non-zero; a floor needs 8, or 0x40 and a
non-zero slope. `field_floor_height` casts from the fighter's y + 0.5 down to
-1000.01 with flags 9 (0x49 with the ground flag) and answers -1000 and kind
0x400 when nothing is hit; `rob_floor_check` also casts four probes 20 units
out at the fighter's facing + 0, 90, 180 and 270 degrees with flags 7 and
keeps a distance as a wall only when the polygon hit is on.
`coli_sweep_hit` (the fighters' spheres against walls) tests only the start
point's cell. `coli_segment_hit_any` tests only the list of cell 0, the
corner cell, which holds ring-out polygons (in VIP also two of its tall
skirts; in `st_debug0`, whose one cell is the whole stage, everything).

What the files hold: every polygon is a quad. A stage is a square ring
floor (half-size 5.2 to 8.2 units), in most files four vertical
floor-kind skirts down its sides (seen only with the ground flag), walls
on the ring's edges (vertical faces; in six files also a flat top face of the
wall kind), the ring-out ground below (y 3.0 to 3.4; 12.0 on BAN and DES,
0.0 on YAM, -8.0 on VIP; none in `st_debug0`), four vertical out faces and
a downward-facing out lid closing the box. Kinds over the 18 files: 69 floor,
136 wall (80 with the unread bit 0x1000), 176 breakable, 127 out, 6
0x80000000 (DES), plus `st_test0`'s 7. The open rings (BAN, DURAL, GARDEN,
NOH, UMI, VIP) keep their four walls switched off in the file, and CAS and
NYC 16 of their 48 wall faces: the probes see them as open edges, so the
fighters fall out there. DJO, ROME and YAM have 24 breakable walls of two
faces each, DES 24 of one face, `st_debug0` four.

Totals: 18 files, 108,888 bytes, 750 vertices, 521 polygons, 2,238 cells,
980 cell lists (11,450 entries), 100 breakable walls, 16,963 pointers.
`st_cas` serves six stage records (CAS, TEST2, VF01-03, HLO), `st_nyc` two
(NYC, TEST3), `st_debug0` two (DEBUG, ADV); no record names `st_test0`. It
is older data: its grid has a third origin float, so under the current code
the scale word reads -500 and no point finds a cell, two walls have a zero
normal, and its kinds use bits 0x8000 / 0x10000 no other file has.

**Offline check** (`check`):

- the words a port of `field_coli_set` changes, relocating each file to
  0x0CDA01A0, are exactly the 16,963 pointer words of the parse, each moved
  by the load address and landing on the start of a structure of the right
  type (vertex, polygon, list, record ...); the only other words that equal
  a structure offset are counts, header +0x00 and breakable numbers;
- every byte of 17 files lies in exactly one structure; `st_test0` has a
  4-byte gap at +0x60 (`01 00 01 00`, the end of its old grid);
- every polygon's vertices lie on its plane (within 7e-6), its centroid is
  inside it, its closing pointer repeats v[0], and its slope and heading
  words equal the angles of its normal (within 1 unit; `st_test0` aside);
- every polygon whose x / z box meets a cell's square is in that cell's
  list (the lists hold more than that, never less), except two of DES's
  0x80000000 polygons (a lid at y 20 and a ground at y 11), which are in
  no list at all, so no test can reach them;
- the floor port gives the ring height (4.0 or 12.6), on a 0x100 floor, at
  the center and both start positions of all 24 stage records; walking out
  from the center the floor turns to ring-out ground at the ring's edge, and
  the four probes from the center hit the walls (5.0 to 8.2 units away);
- the stage models agree where they can be compared: the commonest vertex
  height over the ring of `stg_dur`, `stg_gdn`, `stg_nyc`, `stg_rom`,
  `stg_sui`, `stg_vip` is 4.00, and `stg_yam` has 1,056 vertices at 12.60.

**Live check.** `live` reads a dump of main RAM (from a Flycast save state,
`tools/flycast_state.py --dump`). In the three saved states (UMI; YAM
twice, mid-fight) `field_coli` is the `field_load` buffer, every pointer
word is the file's plus the buffer address (689 and 2,744), no other word
differs from the port of `field_coli_set` (+0x28 included; +0x44 / +0x48
are 0), `coli_floor_poly` and the fighters' last wall polygon (collision
record +0x98) are polygon starts, each fighter's floor height (+0x7C) is the
port's, and the fighter's four probe distances (+0x12C, +0x13C) and bits (+0x15C)
are the port's within 1e-6, taken at the fighter's position less the frame's shift
(collision record +0x370), since `robs_coli_shift_apply` moves the fighter after
the floor check.

## Stance poses (`smooth_*.bin`, `tools/smooth.py`)

```bash
python3 -I tools/smooth.py work/main.bin work/assets/smooth_*.bin                    # checks, a line a file
python3 -I tools/smooth.py work/main.bin work/assets/smooth_*.bin --json smooth.json  # the blocks as JSON
```

At the end of a move the game eases the fighter into a stored stance pose,
the "rsmooth" blend (`rob_skeleton_update`, 0x0C092780, in `mot_tools.c`). Each
fighter's file is loaded whole into the heap "smooth_load_addr" and is 16
blocks of 0x5C8 bytes, in the order of `e_rsmooth_no`:

| Offset | Field |
|--------|-------|
| +0x000 | 23 matrices, 4x4 floats (row-major, translation in row 3), one per bone of `rsmooth_bone_list` (0x0C18BE68) |
| +0x5C0 / +0x5C4 | left / right hand shape (floats, 0 to 0.2) |

The block is picked by the stance byte rob +0x285 (the move record's +0x05):
0, 0x80, 1, 0x81 give TA_KAMAE, SY_KAMAE, TA_GUARD, SY_GUARD; 3 gives A_DOWN,
or U_DOWN when rob +0x20 has 0x800000; any other stance has no blend. Each
block has an `_M` twin, used when rob +0x3A6 has 0x10. Character id 12
(BLACK: `smooth_blk`, `ctrl_VAN`) with rob +0x550 set, its second control set,
takes SP_TA_KAMAE and SP_SY_KAMAE for stances 0 and 0x80 instead.

The blend runs over the last `rsmooth_frames` (9) frames of a move at least
9 frames longer than its motion-change blend: the weight on the table's pose,
rob +0x3D4, is 1 - (length - frame) / 9, with the move frame rob +0x0A and the
length rob +0x2E4, so it rises from 0 to 1 at the move's end (and stays 1
past it). What the game reads:

- **Rotations** of the 16 bones with flag 0x800 and without 0x2008000 (all
  but `k_hara` and the `t_` bones): `rob_eval_output` takes
  `mtx_rot_lerp(table, motion, weight)` on the 3x3 part. Bones with flag
  0x40000000 keep the motion's while rob +0x3A6 has 0x1000.
- **Positions** of the root `k_hara` and the six `t_` IK targets:
  `rsmooth_targets_set` turns each translation into the fighter's ground
  frame and stores it at bone +0x2C. `root_pos_rsmooth_blend` and
  `bone_pos_rsmooth_blend` then move toward it. When the fighter lies down or
  faces away (+0x3A6 0x5040, 0x8000, 0x400), it also rebuilds the matrices of
  `c_mune` and `k_kosi` into `rsmooth_root_mtx`.
- **Hands**: the two floats go to `s_te_l` and `s_te_r` +0x2C (flag
  0x10000000). `rob_eval_output` blends the hand value toward them and stores
  10 times them in the hand records (rob +0x504, +0x518), which
  `rob_hands_step` eases toward.

The rest (the other rotations and positions, row and column 3) is exporter
output that nothing reads. `pose.py` and `gltf.py --smooth` apply the
rotations and positions (`rig.pose_passes`; within 6e-5 of the game in a
save state); not the hands or the down and turned stances. What the files hold:

- Only `smooth_blk` has every SP_ block. In the other 15, 0xCDCDCDCD (an MSVC
  debug fill: memory the exporter never wrote) covers SP_SY_KAMAE and all but
  the last 32 words of SP_SY_KAMAE_M. It also covers:
  - SP_TA_KAMAE and its `_M` in akira, dural, jdk, jef, lau, mue and wolf;
  - the `_M` alone in boz;
  - 28 words of the `_M` in jac and pai, whose SP_TA_KAMAE holds shifted data
    (not rotations).

  aoi, kage, lion, sarah and shun keep a distinct pose there that no code
  reads.
- An `_M` block is its pair mirrored: positions with x negated and the l / r
  bones swapped; rotations as A R B with fixed signs per bone (A and B
  diagonal, ±1); the hands swapped. This holds within 1e-5, except jac
  SY_KAMAE (within 0.002) and lion TA_KAMAE, whose `_M` is posed separately
  (hips 0.028 lower, up to 0.09 apart).
- `k_hara`'s rotation is all zero in akira and lau TA_KAMAE (not read).

## Others

- `aicadrv.bin` (63,456 bytes): the ARM7 program for the AICA sound
  processor (begins with the 8 ARM exception-vector `b` instructions).
