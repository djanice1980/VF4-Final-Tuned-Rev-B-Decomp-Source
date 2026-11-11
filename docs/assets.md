# Assets: the SimpleFlashFS volume

The 236 MB after the program in the DIMM image (`VF41_S.BIN`) is one
**SimpleFlashFS** volume, Sega's name from its header. `tools/sffs.py` lists
and extracts it by the same walk as the game's own directory loader
(`sffs_load_dir`, 0x0C0F2E80). `VF41_S2.BIN` decrypts to a byte-identical
copy (SHA1 `5b385f6b26fef97a769581769feb3319f39dd52c` for both).

```bash
python3 -I tools/sffs.py list    work/vf41_s.dec docs/assets.tsv
python3 -I tools/sffs.py extract work/vf41_s.dec work/assets
```

## Format

| Offset | Content |
|--------|---------|
| 0x000000-0x3DFFFF | Naomi header, main program, test program |
| 0x3E0000-0x7FFFFF | erased (0xFF) |
| 0x800000 | volume header: `SimpleFlashFS`, `1999.5.5`, 4, 0x00800000, block 0x1000, 0x20020820 |
| 0x801000-0xE2B7FFF | 908 files: 0x100-byte header + data each, packed in 4 KiB blocks |
| 0xE2B8000 | free-space record (rest of the volume) |
| 0xE2B9000-0xEFFFFFF | erased |

File header (struct `sffs_entry` in `ghidra/types/vf4ft_types.h`):

| Offset | Field |
|--------|-------|
| +0x00 | bytes from here to the volume end (0x0EFFF000); -1 in erased flash ends the walk |
| +0x04 | space taken, header + data rounded up to 4 KiB; -1 marks free space |
| +0x08 | live flag: -1 live, 0 deleted (the loader drops deleted entries) |
| +0x0C | data size |
| +0x10 | unknown (not a 16-bit sum, CRC-32 or Adler-32) |
| +0x14 | modification time, Unix seconds |
| +0x18 | st_mode, 0100644 or 0100664 |
| +0x1C | unknown |
| +0x20-0x7B | -1 |
| +0x7C | -1 on disk; in RAM the loader stores the header's image offset |
| +0x80 | file name, up to 128 bytes |

## Contents

- 908 files, 227,100,493 bytes stored; 650 are gzip (285,157,428 bytes unpacked), 3 of them empty placeholders (`*_scr_test.bin.gz`); none deleted.
- Modification times run from 2003-10-17 to 2004-10-26.
- 843 files have their name as a literal string in the program; the other 65 (`motdat_*`, `mothead_*`, `ctrl_*`, ...) are opened under names built at run time.
- `docs/assets.tsv` lists every file with offset, size, mode, date and SHA1, so
  an extraction can be checked against it.

| Prefix | Files | Bytes stored |
|--------|------:|------:|
| `obj` | 216 | 12,458,776 |
| `mdl` | 216 | 47,728,061 |
| `tex` | 216 | 67,792,925 |
| `spr` | 57 | 36,652,374 |
| `se` | 33 | 11,676,256 |
| `bg` | 32 | 23,733,696 |
| `sel` | 23 | 636,664 |
| `motdat` | 21 | 11,174,280 |
| `mothead` | 20 | 801,552 |
| `st` | 18 | 108,888 |
| `ctrl` | 17 | 113,984 |
| `smooth` | 16 | 378,880 |
| `vo` | 15 | 13,023,488 |
| `nick` | 2 | 18,388 |
| `chlg` | 1 | 434,260 |
| `adv` | 1 | 1,149 |
| `line` | 1 | 28,864 |
| `ftttl` | 1 | 175,276 |
| `ftdemo` | 1 | 99,276 |
| `aicadrv.bin` | 1 | 63,456 |

## Named by the program, not on the disc (106)

Whole filename strings in the program with no file in the volume: test and
debug assets, unused character variants, a PS2 stage (`mdl_stg_ps2.bin.gz`),
and select-screen sprites under older names (`spr_sel_st_*`, where the disc
has `spr_sel_stg_*`). Traces of cut or development content.

- **mdl** (27): `mdl_adv_fly.bin.gz`, `mdl_chr_ao3.bin.gz`, `mdl_chr_bl3.bin.gz`, `mdl_chr_ds2.bin.gz`, `mdl_chr_dst.bin.gz`, `mdl_chr_kg2.bin.gz`, `mdl_chr_kgm.bin.gz`, `mdl_chr_test.bin.gz`, `mdl_ef_adv.bin.gz`, `mdl_itm_du2_atam.bin.gz`, `mdl_itm_du2_face.bin.gz`, `mdl_itm_du2_joha.bin.gz`, `mdl_itm_du2_kaha.bin.gz`, `mdl_itm_dur_atam.bin.gz`, `mdl_itm_dur_face.bin.gz`, `mdl_itm_dur_joha.bin.gz`, `mdl_itm_dur_kaha.bin.gz`, `mdl_itm_mot_atam.bin.gz`, `mdl_itm_mot_face.bin.gz`, `mdl_itm_mot_joha.bin.gz`, `mdl_itm_mot_kaha.bin.gz`, `mdl_stg_adv.bin.gz`, `mdl_stg_hlo.bin.gz`, `mdl_stg_ps2.bin.gz`, `mdl_stg_test.bin.gz`, `mdl_stg_test2.bin.gz`, `mdl_stg_test3.bin.gz`
- **o** (1): `o_st_dm0_etc.bin`
- **obj** (27): `obj_adv_fly.bin.gz`, `obj_chr_ao3.bin.gz`, `obj_chr_bl3.bin.gz`, `obj_chr_ds2.bin.gz`, `obj_chr_dst.bin.gz`, `obj_chr_kg2.bin.gz`, `obj_chr_kgm.bin.gz`, `obj_chr_test.bin.gz`, `obj_ef_adv.bin.gz`, `obj_itm_du2_atam.bin.gz`, `obj_itm_du2_face.bin.gz`, `obj_itm_du2_joha.bin.gz`, `obj_itm_du2_kaha.bin.gz`, `obj_itm_dur_atam.bin.gz`, `obj_itm_dur_face.bin.gz`, `obj_itm_dur_joha.bin.gz`, `obj_itm_dur_kaha.bin.gz`, `obj_itm_mot_atam.bin.gz`, `obj_itm_mot_face.bin.gz`, `obj_itm_mot_joha.bin.gz`, `obj_itm_mot_kaha.bin.gz`, `obj_stg_adv.bin.gz`, `obj_stg_hlo.bin.gz`, `obj_stg_ps2.bin.gz`, `obj_stg_test.bin.gz`, `obj_stg_test2.bin.gz`, `obj_stg_test3.bin.gz`
- **spr** (24): `spr_adv_demo_a.bin`, `spr_adv_demo_b.bin`, `spr_adv_env.bin`, `spr_sel.bin`, `spr_sel_cha.bin.gz`, `spr_sel_cha2.bin.gz`, `spr_sel_cha3.bin.gz`, `spr_sel_mov.bin`, `spr_sel_st_are.bin`, `spr_sel_st_ban.bin`, `spr_sel_st_cas.bin`, `spr_sel_st_cav.bin`, `spr_sel_st_djo.bin`, `spr_sel_st_dur.bin`, `spr_sel_st_gdn.bin`, `spr_sel_st_hbr.bin`, `spr_sel_st_noh.bin`, `spr_sel_st_nyc.bin`, `spr_sel_st_rom.bin`, `spr_sel_st_sui.bin`, `spr_sel_st_umi.bin`, `spr_sel_st_yam.bin`, `spr_title.bin`, `spr_title2.bin`
- **tex** (27): `tex_adv_fly.bin.gz`, `tex_chr_ao3.bin.gz`, `tex_chr_bl3.bin.gz`, `tex_chr_ds2.bin.gz`, `tex_chr_dst.bin.gz`, `tex_chr_kg2.bin.gz`, `tex_chr_kgm.bin.gz`, `tex_chr_test.bin.gz`, `tex_ef_adv.bin.gz`, `tex_itm_du2_atam.bin.gz`, `tex_itm_du2_face.bin.gz`, `tex_itm_du2_joha.bin.gz`, `tex_itm_du2_kaha.bin.gz`, `tex_itm_dur_atam.bin.gz`, `tex_itm_dur_face.bin.gz`, `tex_itm_dur_joha.bin.gz`, `tex_itm_dur_kaha.bin.gz`, `tex_itm_mot_atam.bin.gz`, `tex_itm_mot_face.bin.gz`, `tex_itm_mot_joha.bin.gz`, `tex_itm_mot_kaha.bin.gz`, `tex_stg_adv.bin.gz`, `tex_stg_hlo.bin.gz`, `tex_stg_ps2.bin.gz`, `tex_stg_test.bin.gz`, `tex_stg_test2.bin.gz`, `tex_stg_test3.bin.gz`

The program also keeps a development-host command, `shell mv tmp.bin
../data/param/osage_coli_%s.bin`, next to the `osage_coli_%s.bin` and
`osage_param_%s.bin` name formats it builds at run time.
