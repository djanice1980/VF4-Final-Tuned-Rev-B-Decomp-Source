/*
 * sdk/kamui/ta_regions.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int opb_words_by_code[4]; /* 0x0c32ac30 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern uint tiles_skipped_by_a; /* 0x0c600d34 */
extern uint tiles_skipped_by_b; /* 0x0c601694 */

int objlist_bytes_for_tiles(int tiles)
{
  int *t = opb_words_by_code;
  return (t[gfx_state.opb_codes[0]] + t[gfx_state.opb_codes[1]] + t[gfx_state.opb_codes[2]] + t[gfx_state.opb_codes[3]] + t[gfx_state.opb_codes[4]]) * tiles * 4;
}

void ta_lists_point_at_buffer(int across, int down, int buf)
{
  uint v;
  
  *(volatile int *)0xa85f8008 = 1;
  v = *(volatile int *)0xa05f8138;
  v = *(volatile int *)0xa25f8138;
  *(volatile int *)0xa85f8008 = 0;
  pvr_reg_write_both(0x124,gfx_state.ta_bufs[buf].ol_base);
  pvr_reg_write_both(300,gfx_state.ta_bufs[buf].ol_limit);
  pvr_reg_write_both(0x128,gfx_state.ta_bufs[buf].isp_base);
  pvr_reg_write_both(0x130,gfx_state.ta_bufs[buf].isp_limit);
  pvr_reg_write_both(0x164,gfx_state.ta_bufs[buf].opb_start);
  v = down - 1; v <<= 16; v += across - 1; pvr_reg_write_both(0x13c,v);
  v = gfx_state.opb_codes[0] & 3 | (gfx_state.opb_codes[1] & 3) << 4 |
          (gfx_state.opb_codes[2] & 3) << 8 |
          (gfx_state.opb_codes[3] & 3) << 0xc;
  if (gfx_state.chip_rev == 6) {
    v = v | (gfx_state.opb_downward & 1) << 0x14;
    v |= (gfx_state.opb_codes[4] & 3) << 0x10;
  }
  else {
    v |= (gfx_state.opb_downward & 1) << 0x10;
  }
  pvr_reg_write_both(0x140,v);
  pvr_reg_write_both(0x144,0x80000000);
}

uint objlist_put_never_drawn_triangle(uint addr, int list_base)
{
  uint *p;

  p = (uint *)(addr | 0xa5000000);
  p[0] = ((addr - list_base) >> 2) + 2 | 0x80200000;
  p[1] = 0xf0000000;
  p += 2;
  p[0] = 0x800000;
  p[1] = 0x24900000;
  p[2] = 0;
  ((float *)p)[3] = 0.0;
  ((float *)p)[4] = 0.0;
  ((float *)p)[5] = 1e-10;
  p[6] = 0x80ff0000;
  ((float *)p)[7] = 0.0;
  ((float *)p)[8] = 1280.0;
  ((float *)p)[9] = 1e-10;
  p[10] = 0x8000ff00;
  ((float *)p)[11] = 2560.0;
  ((float *)p)[12] = 0.0;
  ((float *)p)[13] = 1e-10;
  p[14] = 0x800000ff;
  return addr;
}

uint objlist_put_black_triangle(uint addr, int list_base)
{
  uint *p;
  
  p = (uint *)(addr | 0xa5000000);
  p[0] = (addr - list_base >> 2) + 2 | 0x80200000;
  p[1] = 0xf0000000;
  p += 2;
  p[0] = 0xe0800000;
  p[1] = 0x20800000;
  p[2] = 0;
  ((float *)p)[3] = 0.0;
  ((float *)p)[4] = 0.0;
  ((float *)p)[5] = 1e-10;
  p[6] = 0;
  ((float *)p)[7] = 0.0;
  ((float *)p)[8] = 1280.0;
  ((float *)p)[9] = 1e-10;
  p[10] = 0;
  ((float *)p)[11] = 2560.0;
  ((float *)p)[12] = 0.0;
  ((float *)p)[13] = 1e-10;
  p[14] = 0;
  return addr;
}

void tile_regions_put_flush_entry(uint addr)
{
  uint entry[6];
  
  entry[0] = 0x10000000;
  entry[1] = 0x80000000;
  entry[2] = 0x80000000;
  entry[3] = 0x80000000;
  entry[4] = 0x80000000;
  entry[5] = 0x80000000;
  vram32_write(addr,entry,0x18);
  return;
}

void tile_regions_write(int across, int down, int slot, uint unused, int buf, int column_order)
{
  typedef struct { uint pad : 2; uint x : 6; uint y : 6; uint pad2 : 14; uint flush : 1; uint presort : 1; uint zkeep : 1; uint last : 1; } rgn_ctrl;
  typedef struct { rgn_ctrl c; uint l[5]; } rgn_ent;
  rgn_ent e;
  rgn_ent fl;
  rgn_ent e1;
  rgn_ent e2;
  rgn_ent e3;
  uint inA;
  uint inB;
  int stride0;
  int stride1;
  int stride2;
  int stride3;
  int base0;
  int base1;
  int base2;
  int base3;
  uint mask0;
  uint mask1;
  uint mask2;
  uint mask3;
  int stride4;
  int base4;
  uint mask4;
  int dest;
  int cntA;
  int cntB;
  uint nA;
  uint nB;
  int flag;
  uint n;
  uint never_drawn_tri;
  uint black_tri;
  int use_never_drawn;
  uint tag;
  uint lastA;
  uint lastB;
  uint i;
  uint x;
  uint y;
  uint m;
  uint idx;
  int sz;
  int d;
  int *t;

  use_never_drawn = 0;
  if (gfx_state.presort_flag == 1) {
    flag = 1;
  }
  else {
    flag = 0;
  }
  if (gfx_state.ta_busy == 0) {
    ta_lists_point_at_buffer(across,down,buf);
  }
  n = across * down;
  sz = n * 0x18;
  dest = gfx_state.ta_bufs[buf].region_base + slot * sz;
  if ((gfx_state.flags & 0x80) != 0) {
    tile_regions_put_flush_entry(dest);
    dest = dest + 0x18;
  }
  if ((gfx_state.flags & 0x20) != 0) {
    if (gfx_state.double_regions != 0) {
      never_drawn_tri = objlist_put_never_drawn_triangle(dest + sz * 4,gfx_state.ta_bufs[buf].base);
      use_never_drawn = 0;
      if (gfx_state.opb_codes[0] == 0 && gfx_state.opb_codes[2] != 0) {
        if (flag != 0) {
          use_never_drawn = 1;
        }
      }
      black_tri = objlist_put_black_triangle(dest + sz * 4 + 0x44,gfx_state.ta_bufs[buf].base);
    }
    else {
      black_tri = objlist_put_black_triangle(dest + sz * 2,gfx_state.ta_bufs[buf].base);
    }
  }
  else {
    black_tri = objlist_put_black_triangle(dest + sz,gfx_state.ta_bufs[buf].base);
  }
  if (gfx_state.opb_codes[0] == 0) {
    mask0 = 0x80000000;
  }
  else {
    mask0 = 0;
  }
  if (gfx_state.opb_codes[1] == 0) {
    mask1 = 0x80000000;
  }
  else {
    mask1 = 0;
  }
  if (gfx_state.opb_codes[2] == 0) {
    mask2 = 0x80000000;
  }
  else {
    mask2 = 0;
  }
  if (gfx_state.opb_codes[3] == 0) {
    mask3 = 0x80000000;
  }
  else {
    mask3 = 0;
  }
  if (gfx_state.opb_codes[4] == 0) {
    mask4 = 0x80000000;
  }
  else {
    mask4 = 0;
  }
  t = opb_words_by_code;
  stride0 = t[gfx_state.opb_codes[0]] << 2;
  stride1 = t[gfx_state.opb_codes[1]] << 2;
  stride2 = t[gfx_state.opb_codes[2]] << 2;
  stride3 = t[gfx_state.opb_codes[3]] << 2;
  stride4 = t[gfx_state.opb_codes[4]] << 2;
  base0 = gfx_state.ta_bufs[buf].ol_base;
  base1 = base0 + (t[gfx_state.opb_codes[0]] * n << 2);
  base2 = base1 + (t[gfx_state.opb_codes[1]] * n << 2);
  base3 = base2 + (t[gfx_state.opb_codes[2]] * n << 2);
  base4 = base3 + (t[gfx_state.opb_codes[3]] * n << 2);
  tag = gfx_state.ta_bufs[buf].split_word;
  nA = 0;
  nB = 0;
  i = 0;
  cntA = 0;
  cntB = 0;
  for (; i < n; i++) {
    if (column_order == 0) {
      x = i / gfx_state.tile_rows;
      y = i % gfx_state.tile_rows;
    }
    else {
      x = i % gfx_state.tile_cols;
      y = i / gfx_state.tile_cols;
    }
    if (buf != 0) {
      x = gfx_state.tile_cols - x - 1;
    }
    tile_split_owner(x,y,&inA,&inB,tag);
    m = 0;
    if (gfx_state.tile_limit_on == 1 &&
        (gfx_state.tile_limit_x < x || gfx_state.tile_limit_y < y)) {
      m = 0x80000000;
    }
    idx = y * gfx_state.tile_cols + x;
    *(uint *)&e.c = 0; /* WORKAROUND (kept as possibly original; no game instruction of its own): the entry is cleared here and then completely filled below. The game has no stores of its own for the clears; they make the loop big enough that loop.c stops hoisting, as the game's code shows. Kept as plausible original clear-then-fill code. */
    e.l[0] = 0x80000000;
    e.l[1] = 0x80000000;
    e.l[2] = 0x80000000;
    e.l[3] = 0x80000000;
    *(uint *)&e.c = (flag & 1) << 29 | (y & 0x3f) << 8 | (x & 0x3f) << 2;
    e.l[0] = mask0 | m | (idx * stride0 + base0);
    e.l[1] = mask1 | m | (idx * stride1 + base1);
    e.l[2] = mask2 | m | (idx * stride2 + base2);
    e.l[3] = mask3 | m | (idx * stride3 + base3);
    if (list4_regions_from_list2 != 0) {
      e.l[4] = e.l[2];
    }
    else {
      e.l[4] = mask4 | m | (idx * stride4 + base4);
    }
    if (gfx_state.modvol_merge != 0) {
      if (gfx_state.modvol_merge_mode == 1) {
        e.l[3] = e.l[1];
      }
      else {
        e.l[1] = e.l[3];
      }
    }
    if ((gfx_state.flags & 0x20) != 0) {
      if (gfx_state.double_regions != 0) {
        e1.c = e.c;
        e2.c = e.c;
        e3.c = e.c;
        e.c.last = 0;
        e1.c.last = 0;
        e2.c.last = 0;
        e.c.zkeep = 0;
        e1.c.zkeep = 1;
        e2.c.zkeep = 1;
        e3.c.zkeep = 1;
        e.c.flush = 1;
        e1.c.flush = 1;
        e2.c.flush = 1;
        e3.c.flush = 0;
        e1.c.presort = 0;
        if (use_never_drawn != 0) {
          e.l[0] = never_drawn_tri;
        }
        e1.l[0] = 0x80000000;
        e1.l[1] = 0x80000000;
        e1.l[2] = never_drawn_tri;
        e1.l[3] = 0x80000000;
        e1.l[4] = 0x80000000;
        e2.l[0] = 0x80000000;
        e2.l[1] = 0x80000000;
        e2.l[2] = e.l[2];
        e2.l[3] = e.l[3];
        e2.l[4] = 0x80000000;
        e.l[2] = 0x80000000;
        e.l[3] = 0x80000000;
        e3.l[0] = 0x80000000;
        e3.l[1] = 0x80000000;
        e3.l[2] = 0x80000000;
        e3.l[3] = 0x80000000;
        e3.l[4] = 0x80000000;
        if (inA != 0) {
          d = dest + cntA * 0x60;
          vram32_write_chip_a(d,(uint *)&e,0x18);
          vram32_write_chip_a(d + 0x18,(uint *)&e1,0x18);
          vram32_write_chip_a(d + 0x30,(uint *)&e2,0x18);
          if (i == n - 1) {
            e3.c.last = 1;
          }
          vram32_write_chip_a(d + 0x48,(uint *)&e3,0x18);
          cntA++;
        }
        if (inB != 0) {
          d = dest + cntB * 0x60;
          vram32_write_chip_b(d,(uint *)&e,0x18);
          vram32_write_chip_b(d + 0x18,(uint *)&e1,0x18);
          vram32_write_chip_b(d + 0x30,(uint *)&e2,0x18);
          if (i == n - 1) {
            e3.c.last = 1;
          }
          vram32_write_chip_b(d + 0x48,(uint *)&e3,0x18);
          cntB++;
        }
      }
      else {
        e1.c = e.c;
        e.c.last = 0;
        e.c.zkeep = 0;
        e.c.flush = 1;
        e1.c.zkeep = 1;
        e1.c.flush = 0;
        e1.l[0] = 0x80000000;
        e1.l[1] = 0x80000000;
        e1.l[2] = 0x80000000;
        e1.l[3] = 0x80000000;
        e1.l[4] = 0x80000000;
        if (inA != 0) {
          d = dest + cntA * 0x30;
          vram32_write_chip_a(d,(uint *)&e,0x18);
          if (i == n - 1) {
            e1.c.last = 1;
          }
          vram32_write_chip_a(d + 0x18,(uint *)&e1,0x18);
          cntA++;
        }
        if (inB != 0) {
          d = dest + cntB * 0x30;
          vram32_write_chip_b(d,(uint *)&e,0x18);
          if (i == n - 1) {
            e1.c.last = 1;
          }
          vram32_write_chip_b(d + 0x18,(uint *)&e1,0x18);
          cntB++;
        }
      }
    }
    else {
      if ((i == n - 1) & (i == cntA)) {
        e.c.last = 1;
      }
      if (inA != 0) {
        d = dest + cntA * 0x18;
        vram32_write_chip_a(d,(uint *)&e,0x18);
        lastA = *(uint *)&e.c;
        cntA++;
      }
      else {
        ((uint *)&tiles_skipped_by_a)[nA] = *(uint *)&e.c;
        nA++;
      }
      if (inB != 0) {
        d = dest + cntB * 0x18;
        vram32_write_chip_b(d,(uint *)&e,0x18);
        lastB = *(uint *)&e.c;
        cntB++;
      }
      else {
        ((uint *)&tiles_skipped_by_b)[nB] = *(uint *)&e.c;
        nB++;
      }
    }
  }
  gfx_state.ta_bufs[buf].ends_pending = cntA != n - 1;
  gfx_state.ta_bufs[buf].end_a = dest + cntA * 0x18 - 0x18;
  gfx_state.ta_bufs[buf].end_b = cntB * 0x18 + dest - 0x18;
  gfx_state.ta_bufs[buf].last_a = lastA | 0x80000000;
  gfx_state.ta_bufs[buf].last_b = lastB | 0x80000000;
  fl.l[0] = black_tri;
  fl.l[1] = 0x80000000;
  fl.l[2] = 0x80000000;
  fl.l[3] = 0x80000000;
  fl.l[4] = 0x80000000;
  if (nA != 0) {
    ((uint *)&tiles_skipped_by_a)[nA - 1] |= 0x80000000;
    for (i = 0, d = dest + cntA * 0x18; i < nA; i++) {
      *(uint *)&fl.c = ((uint *)&tiles_skipped_by_a)[i];
      vram32_write_chip_a(d,(uint *)&fl,0x18);
      d += 0x18;
    }
  }
  if (nB != 0) {
    ((uint *)&tiles_skipped_by_b)[nB - 1] |= 0x80000000;
    for (i = 0, d = dest + cntB * 0x18; i < nB; i++) {
      *(uint *)&fl.c = ((uint *)&tiles_skipped_by_b)[i];
      vram32_write_chip_b(d,(uint *)&fl,0x18);
      d += 0x18;
    }
  }
}

void ta_memory_defaults_set(void)
{
  
  pvr_softreset_pulse();
  gfx_state.ta_mem_size = 0x800000;
  gfx_state.field_0x45c = 0x600000;
  gfx_state.field_0x460 = gfx_state.field_0x45c;
  gfx_state.ta_bufs[0].base = gfx_state.ta_mem_size >> 1;
  gfx_state.ta_bufs[1].base = 0;
  gfx_state.ta_bufs[0].split_word = 0;
  gfx_state.ta_bufs[1].split_word = 0;
  gfx_state.opb_downward = 1;
}

void ta_buffers_lay_out(uint region_units, uint objlist_units, uint isp_units, uint tsp_units, uint list_ctrl, int unused)
{
  typedef struct { int a; int b; int c; int d; int e; int f; int g; int h; char pad[24]; } fb_ent;
  int t;
  int sz;
  int i;

  gfx_state.list_ctrl = list_ctrl;
  {
    int lo = list_ctrl & 1;
    t = lo ^ 1;
  }
  gfx_state.opb_downward = list_ctrl >> 16 & 1;
  gfx_state.opb_codes[0] = list_ctrl >> 17 & 3;
  gfx_state.opb_codes[1] = (list_ctrl & 0x180000) >> 19;
  gfx_state.opb_codes[2] = (list_ctrl & 0x600000) >> 21;
  gfx_state.opb_codes[3] = (list_ctrl & 0x1800000) >> 23;
  gfx_state.opb_codes[4] = (list_ctrl & 0x6000000) >> 25;
  gfx_state.param_area_size = (gfx_state.ta_buf_size >> 2) * 3;
  sz = objlist_bytes_for_tiles(gfx_state.tile_cols * gfx_state.tile_rows);
  for (i = 0; i < 2; i++) {
    gfx_state.ta_bufs[i].region_base = gfx_state.ta_bufs[i].base + gfx_state.ta_buf_size;
    gfx_state.ta_bufs[i].region_base -= 0x44;
    if ((gfx_state.flags & 0x20) != 0) {
      if (gfx_state.double_regions != 0) {
        gfx_state.ta_bufs[i].region_base -= 0x44;
        gfx_state.ta_bufs[i].region_base -= gfx_state.tile_cols * gfx_state.tile_rows * 96;
      }
      else {
        gfx_state.ta_bufs[i].region_base -= gfx_state.tile_cols * gfx_state.tile_rows * 48;
      }
    }
    else {
      gfx_state.ta_bufs[i].region_base -= gfx_state.tile_cols * gfx_state.tile_rows * 24;
    }
    if ((gfx_state.flags & 0x80) != 0) {
      gfx_state.ta_bufs[i].region_base -= 24;
    }
    if (gfx_state.opb_downward == 0) {
      gfx_state.ta_bufs[i].ol_base = gfx_state.ta_bufs[i].base + gfx_state.param_area_size & ~31;
      gfx_state.ta_bufs[i].ol_limit = gfx_state.ta_bufs[i].region_base - sz & ~127;
      gfx_state.ta_bufs[i].opb_start = gfx_state.ta_bufs[i].ol_base + sz;
    }
    else {
      gfx_state.ta_bufs[i].ol_base = gfx_state.ta_bufs[i].region_base - sz & ~127;
      gfx_state.ta_bufs[i].ol_limit = gfx_state.ta_bufs[i].base + gfx_state.param_area_size & ~31;
      gfx_state.ta_bufs[i].opb_start = gfx_state.ta_bufs[i].ol_base;
    }
    gfx_state.ta_bufs[i].isp_base = gfx_state.ta_bufs[i].base;
    gfx_state.ta_bufs[i].isp_limit = gfx_state.ta_bufs[i].base + gfx_state.param_area_size - 0x100;
  }
  tile_regions_write(gfx_state.tile_cols,gfx_state.tile_rows,0,1,0,t);
  tile_regions_write(gfx_state.tile_cols,gfx_state.tile_rows,0,1,1,t);
  gfx_state.ta_bufs[0].split_word = -1;
  gfx_state.ta_bufs[1].split_word = -1;
}

void pvr_softreset_pulse(void)
{
  pvr_reg_write_both(8,1);
  pvr_reg_write_both(8,0);
  return;
}

void empty_fn_0c14d340(void)
{
  return;
}

void empty_fn_0c14d360(void)
{
  return;
}

void empty_fn_0c14d380(void)
{
  return;
}
