/*
 * sdk/kamui/elan_commands.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint elan_present; /* 0x0c32ac44 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern uint elan_reg_d0_value; /* 0x0c32ac4c */
extern uint bank_build_index; /* 0x0c41e58c */
extern uint direct_send_held; /* 0x0c41e590 */
extern uint direct_send_hold_depth; /* 0x0c41e9e0 */
extern xfer_job_rec *xfer_current; /* 0x0c41e9e4 */
extern uint list_kind_sent; /* 0x0c41e9e8 */
extern uint bank_render_index; /* 0x0c41e9ec */
extern uint xfer_dma_queued; /* 0x0c41e9f8 */
extern uint xfer_trace_next; /* 0x0c41e9fc */
extern int xfer_done_count; /* 0x0c41ea00 */
extern uint xfer_trace_last; /* 0x0c41ea08 */
extern uint xfer_next_ticket; /* 0x0c41ea0c */
extern uint sq_save_top; /* 0x0c41eb28 */
extern tile_bitmap_ptrs tile_owner_bitmaps; /* 0x0c41ea10 */
extern uint elan_read_pending; /* 0x0c32ac50 */
extern uint elan_read_done_count; /* 0x0c32ac54 */
extern uint elan_read_bank; /* 0x0c32ac58 */
extern elan_rev_rec elan_rev_config; /* 0x0c41eb20 */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline float fsca_cos(uint a)
{
  register float s __asm__ ("fr2");
  register float c __asm__ ("fr3");
  __asm__ ("fsca fpul,dr2" : "=f" (s), "=f" (c) : "y" (a));
  return c;
}

void elancmd_check_or_halt(int ok)
{
  if (ok == 0) {
    *(volatile int *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
}

int elan_probe(void)
{
  if (*(volatile int *)&elan_id_reg == -0x1e530000) {
    elan_layer_reset();
    elan_present = 1;
    return 0;
  }
  elan_present = 0;
  return 8;
}

void elan_layer_reset(void)
{
  short_line_timing = 0;
  list4_regions_from_list2 = 0;
  bank_render_index = 0;
  bank_build_index = 0;
  list_end_block_count = 0;
  xfer_next_ticket = 0;
  xfer_done_count = 0;
  xfer_trace_next = 0;
  xfer_trace_last = 0xffffffff;
  xfer_current = (xfer_job_rec *)0x0;
  list_kind_sent = 0xffffffff;
  xfer_dma_queued = 0;
  xfer_dma_finished = 0;
  sq_save_top = (uint)sq_save_stack;
  direct_send_held = 0;
  direct_send_hold_depth = 0;
  tile_owner_bitmaps.buffer0 = 0;
  tile_owner_bitmaps.buffer1 = 0;
  elan_reg_d0_value = 0;
  xfer_job_pool_init();
  bank_lists_seed();
  return;
}

int elan_ram_span_get(int unused, uint *base_out, uint *size_out)
{
  elan_ram_window(base_out,size_out);
  return 0;
}

int elan_list_block_queue(int list, list_block_ref *block)
{
  if (((elan_rev_config.flags & 0x10) == 0) || (list != 1 && list != 3)) {
    list_seg_append(list,(uint *)block,0);
  }
  return 0;
}

void elan_light_pack_directional(int color, uint dir_x, int dir_y, int dir_z, uint mode0, uint mode1, uint mode2, uint *out)
{
  uint *d = out;
  char a;
  char b;
  char c;
  uint w;
  uint w2;
  uint w3;

  if ((elan_rev_config.flags & 8) != 0) {
    a = 0x7f;
    if ((int)dir_x <= 0xffff) {
      if ((int)dir_x > -0x10000) {
        a = (int)dir_x >> 9;
      }
      else {
        a = -0x7f;
      }
    }
    b = 0x7f;
    if (dir_y <= 0xffff) {
      if (dir_y > -0x10000) {
        b = dir_y >> 9;
      }
      else {
        b = -0x7f;
      }
    }
    c = 0x7f;
    if (dir_z <= 0xffff) {
      if (dir_z > -0x10000) {
        c = dir_z >> 9;
      }
      else {
        c = -0x7f;
      }
    }
    d[7] = 0x08000400;
    d[6] = (mode0 << 5 & 0xe0) | color << 8;
    d[5] = (uchar)a << 16 | (b << 8 & 0xff00) | (uchar)c | (mode2 & 0xf) << 24 | (mode1 << 28 & 0x30000000);
  }
  else {
    a = 0x7f;
    if ((int)dir_x <= 0xffff) {
      if ((int)dir_x > -0x10000) {
        a = (int)dir_x >> 9;
      }
      else {
        a = -0x80;
      }
    }
    b = 0x7f;
    if (dir_y <= 0xffff) {
      if (dir_y > -0x10000) {
        b = dir_y >> 9;
      }
      else {
        b = -0x80;
      }
    }
    c = 0x7f;
    if (dir_z <= 0xffff) {
      if (dir_z > -0x10000) {
        c = dir_z >> 9;
      }
      else {
        c = -0x80;
      }
    }
    if ((int)dir_x > 0xffff) {
      w = 0x080f0400;
    }
    else {
      if ((int)dir_x > -0x10000) {
        w = ((int)dir_x >> 1 & 0xf0) << 12 | 0x08000400;
      }
      else {
        w = 0x08010400;
      }
    }
    if (dir_y > 0xffff) {
      w2 = w | 0xf0;
    }
    else {
      if (dir_y > -0x10000) {
        w2 = w | (dir_y >> 5 & 0xfU) << 4;
      }
      else {
        w2 = w | 0x10;
      }
    }
    if (dir_z > 0xffff) {
      w3 = w2 | 0xf;
    }
    else {
      if (dir_z > -0x10000) {
        w3 = (dir_z >> 5 & 0xfU) | w2;
      }
      else {
        w3 = w2 | 1;
      }
    }
    d[7] = w3;
    d[6] = (mode0 << 5 & 0xe0) | color << 8;
    d[5] = (uchar)a << 16 | (b << 8 & 0xff00) | (uchar)c | (mode2 & 0xf) << 24 | (mode1 << 28 & 0x30000000);
  }
  ((float *)d)[4] = 0.0;
  ((float *)d)[3] = 0.0;
  ((float *)d)[2] = 0.0;
  d[1] = 0;
  d[0] = 0;
}

uint float_high_half(float value)
{
  union { float f; uint u; } v;
  v.f = value;
  return v.u >> 0x10;
}

void elan_light_pack_point(elan_light_params_rec *light, uint *out)
{
  uchar c[3];
  union { float f; uint i; } a;
  union { float f; int i; } scale;
  union { float f; uint i; } b;
  uint m;
  uint k;
  uint t;
  int v;
  float n;
  float z;
  if (elan_rev_config.flags & 8) {
    v = light->dir_fixed[0];
    c[0] = v > 0xffff ? 0x7f : v > -0x10000 ? (uint)v >> 9 : 0x81;
    v = light->dir_fixed[1];
    c[1] = v > 0xffff ? 0x7f : v > -0x10000 ? (uint)v >> 9 : 0x81;
    v = light->dir_fixed[2];
    c[2] = v > 0xffff ? 0x7f : v > -0x10000 ? (uint)v >> 9 : 0x81;
    scale.f = 0.03125;
  }
  else {
    v = light->dir_fixed[0];
    c[0] = v > 0xffff ? 0x7f : v > -0x10000 ? (uint)v >> 9 : 0x80;
    v = light->dir_fixed[1];
    c[1] = v > 0xffff ? 0x7f : v > -0x10000 ? (uint)v >> 9 : 0x80;
    v = light->dir_fixed[2];
    c[2] = v > 0xffff ? 0x7f : v > -0x10000 ? (uint)v >> 9 : 0x80;
    scale.f = 0.00390625;
  }
  scale.i -= 1;
  out[7] = 0x08000400;
  out[6] = (light->mode[0] << 5) & 0xe0 | light->color << 8;
  out[5] = c[0] << 16 | ((char *)c)[1] << 8 & 0xff00 | c[2] | (light->mode[2] & 0xf) << 24 | (light->mode[1] << 28) & 0x30000000 | light->on << 31 | 0x40000000;
  ((float *)out)[4] = light->pos[0];
  ((float *)out)[3] = light->pos[1];
  ((float *)out)[2] = light->pos[2];
  if (light->far_dist != light->near_dist) {
    if (light->far_dist <= 1.0) light->far_dist = 1.0;
    if (light->far_dist > 1e6) light->far_dist = 1e6;
    if (light->near_dist <= 1.0) light->near_dist = 1.0;
    if (light->near_dist > 1e6) light->near_dist = 1e6;
  }
  if (light->far_dist == light->near_dist) {
    out[1] = float_high_half(0.0) << 16 | float_high_half(1.0);
  }
  else {
    if (light->on != 0) {
      a.f = -1.0 / (light->far_dist - light->near_dist);
      a.f = a.f * scale.f + a.f;
      m = 0xff000000 / (uint)(int)(1.0 / scale.f) | 0xff800000;
      a.i &= m;
      k = 0xffff0000;
      a.i &= k; /* redundant, but game code: the next use masks with k again, but the game performs both ands. */
      b.f = 0.5 - (light->far_dist + light->near_dist) * a.f * 0.5;
      b.f = b.f * scale.f + b.f;
      b.i &= m;
      b.i &= k;
      out[1] = a.i & k | b.i >> 16;
    }
    else {
      b.f = -light->near_dist / (light->far_dist - light->near_dist);
      a.f = -b.f * light->far_dist;
      a.f = a.f * scale.f + a.f;
      m = 0xff000000 / (uint)(int)(1.0 / scale.f) | 0xff800000;
      a.i = a.i & m;
      k = 0xffff0000;
      b.f = b.f * scale.f + b.f;
      b.i &= m;
      b.i &= k;
      out[1] = a.i & k | b.i >> 16;
    }
  }
  out[0] = float_high_half(0.0) << 16 | float_high_half(1.0);
}

void elan_light_pack_spot(elan_light_params_rec *light, uint *out)
{
  char n[3];
  int c;
  int e0;
  int e1;
  int e2;
  int v;
  int v1;
  int v2;
  int v3;
  int u1;
  int u2;
  int u3;
  union { float f; uint i; } k;
  union { float f; uint i; } t;
  union { float f; uint i; } scale;
  uint w;
  uint w2;
  uint w3;
  uint m1;
  uint lo;
  uint hm;
  uint m2;
  uint m3;
  int d0;
  int d1;
  int d2;
  int s;
  float mag;
  float x;
  float y;
  float z;
  float a;
  float b;
  float oa;
  float ob;
  float lim;

  if ((elan_rev_config.flags & 8) != 0) {
    v = light->dir_fixed[0];
    if (v > 0xffff) {
      e0 = 0x7f;
    }
    else {
      if (v > -0x10000) {
        e0 = (uint)v >> 9;
      }
      else {
        e0 = -0x7f;
      }
    }
    n[0] = e0;
    e1 = 0x7f;
    if (light->dir_fixed[1] <= 0xffff) {
      if (light->dir_fixed[1] > -0x10000) {
        e1 = (uint)light->dir_fixed[1] >> 9;
      }
      else {
        e1 = -0x7f;
      }
    }
    n[1] = e1;
    e2 = 0x7f;
    if (light->dir_fixed[2] <= 0xffff) {
      if (light->dir_fixed[2] > -0x10000) {
        e2 = (uint)light->dir_fixed[2] >> 9;
      }
      else {
        e2 = -0x7f;
      }
    }
    n[2] = e2;
    scale.i = 0x3cffffff;
    out[7] = 0x08000400;
  }
  else {
    v = light->dir_fixed[0];
    if (v > 0xffff) {
      e0 = 0x7f;
    }
    else {
      if (v > -0x10000) {
        e0 = (uint)v >> 9;
      }
      else {
        e0 = -0x80;
      }
    }
    n[0] = e0;
    e1 = 0x7f;
    if (light->dir_fixed[1] <= 0xffff) {
      if (light->dir_fixed[1] > -0x10000) {
        e1 = (uint)light->dir_fixed[1] >> 9;
      }
      else {
        e1 = -0x80;
      }
    }
    n[1] = e1;
    e2 = 0x7f;
    if (light->dir_fixed[2] <= 0xffff) {
      if (light->dir_fixed[2] > -0x10000) {
        e2 = (uint)light->dir_fixed[2] >> 9;
      }
      else {
        e2 = -0x80;
      }
    }
    n[2] = e2;
    scale.i = 0x3b7fffff;
    v1 = light->dir_fixed[0];
    if (v1 > 0xffff) {
      w = 0x080f0400;
    }
    else {
      if (v1 > -0x10000) {
        w = (v1 >> 1 & 0xf0) << 12 | 0x08000400;
      }
      else {
        w = 0x08010400;
      }
    }
    v2 = light->dir_fixed[1];
    if (v2 > 0xffff) {
      w2 = w | 0xf0;
    }
    else {
      if (v2 > -0x10000) {
        w2 = w | (v2 >> 5 & 0xfU) << 4;
      }
      else {
        w2 = w | 0x10;
      }
    }
    v3 = light->dir_fixed[2];
    if (v3 > 0xffff) {
      w3 = w2 | 0xf;
    }
    else {
      if (v3 > -0x10000) {
        w3 = (v3 >> 5 & 0xfU) | w2;
      }
      else {
        w3 = w2 | 1;
      }
    }
    out[7] = w3;
  }
  out[6] = (light->mode[0] << 5 & 0xe0) | light->color << 8;
  out[5] = (uchar)n[0] << 16 | (n[1] << 8 & 0xff00) | (uchar)n[2] | (light->mode[2] & 0xf) << 24 | (light->mode[1] << 28 & 0x30000000) | light->on << 31 | 0x40000000;
  ((float *)out)[4] = light->pos[0];
  ((float *)out)[3] = light->pos[1];
  ((float *)out)[2] = light->pos[2];
  if (light->far_dist != light->near_dist) {
    if (light->far_dist <= 1.0f) light->far_dist = 1.0f;
    if (light->far_dist > 1000000.0f) light->far_dist = 1000000.0f;
    if (light->near_dist <= 1.0f) light->near_dist = 1.0f;
    if (light->near_dist > 1000000.0f) light->near_dist = 1000000.0f;
  }
  if (light->far_dist == light->near_dist) {
    out[1] = float_high_half(0.0f) << 16 | float_high_half(1.0f);
  }
  else {
    if (light->on != 0) {
      k.f = -1.0f / (light->far_dist - light->near_dist);
      k.f = k.f * scale.f + k.f;
      m1 = 0xff000000 / (uint)(int)(1.0f / scale.f) | 0xff800000;
      k.i &= m1;
      hm = 0xffff0000;
      k.i &= hm;
      t.f = 0.5f - (light->far_dist + light->near_dist) * k.f * 0.5f;
      t.f = t.f * scale.f + t.f;
      t.i &= m1;
      t.i &= hm;
      out[1] = k.i & hm | t.i >> 16;
    }
    else {
      t.f = -light->near_dist / (light->far_dist - light->near_dist);
      k.f = -t.f * light->far_dist;
      k.f = k.f * scale.f + k.f;
      m1 = 0xff000000 / (uint)(int)(1.0f / scale.f) | 0xff800000;
      k.i = k.i & m1;
      hm = 0xffff0000;
      t.f = t.f * scale.f + t.f;
      t.i &= m1;
      t.i &= hm;
      out[1] = k.i & hm | t.i >> 16;
    }
  }
  if ((elan_rev_config.flags & 8) != 0) {
    if (n[0] != -0x80) {
      mag = (n[0] / 127.0f) * (n[0] / 127.0f);
    }
    else {
      mag = 1.0f;
    }
    if (n[1] != -0x80) {
      float q = n[1] / 127.0f;
      mag = q * q + mag;
    }
    else {
      mag = mag + 1.0f;
    }
    if (n[2] != -0x80) {
      float q = n[2] / 127.0f;
      mag = q * q + mag;
    }
    else {
      mag = mag + 1.0f;
    }
  }
  else {
    c = n[0] << 4;
    u1 = light->dir_fixed[0];
    if (u1 > 0xffff) {
      d0 = c | 0xf;
    }
    else {
      if (u1 > -0x10000) {
        d0 = c | (u1 >> 5 & 0xf);
      }
      else {
        d0 = c | 1;
      }
    }
    c = n[1] << 4;
    u2 = light->dir_fixed[1];
    if (u2 > 0xffff) {
      d1 = c | 0xf;
    }
    else {
      if (u2 > -0x10000) {
        d1 = c | (u2 >> 5 & 0xf);
      }
      else {
        d1 = c | 1;
      }
    }
    c = n[2] << 4;
    u3 = light->dir_fixed[2];
    if (u3 > 0xffff) {
      d2 = c | 0xf;
    }
    else {
      if (u3 > -0x10000) {
        d2 = c | (u3 >> 5 & 0xf);
      }
      else {
        d2 = c | 1;
      }
    }
    if (d0 < 0) d0 = -d0;
    if (d1 < 0) d1 = -d1;
    if (d2 < 0) d2 = -d2;
    s = ((d2 >> 8 & 4) + (d1 >> 9 & 2) + (d0 >> 10)) * 4;
    d0 += 0x01000010 >> s & 1;
    d1 += 0x00010100 >> s & 1;
    d2 += 0x10101000 >> s & 1;
    mag = (d0 * 0.00048828125f) * (d0 * 0.00048828125f);
    y = d1 * 0.00048828125f;
    mag = y * y + mag;
    z = d2 * 0.00048828125f;
    mag = z * z + mag;
  }
  mag = fsqrt_asm(mag);
  if (light->cone_angle[0] == light->cone_angle[1]) {
    *out = float_high_half(0.0f) << 16 | float_high_half(1.0f);
  }
  else {
    oa = 0.0f;
    ob = oa;
    if ((elan_rev_config.flags & 8) != 0) {
      do {
        a = fsca_cos((ushort)(int)((oa + light->cone_angle[0]) * 65536.0f / 6.283185f));
        b = fsca_cos((ushort)(int)((ob + light->cone_angle[1]) * 65536.0f / 6.283185f));
        if (a < 0.0f) a = -a;
        if (b < 0.0f) b = -b;
        k.f = 1.0f / (a - b);
        k.f = k.f * scale.f + k.f;
        m2 = 0xff000000 / (uint)(int)(1.0f / scale.f) | 0xff800000;
        k.i &= m2;
    k.i &= 0xffff0000;
        if (a > b) {
          ob += b > 0.75f ? 0.1f : 0.01f;
          oa += 0.01f;
          x = 1.0f;
        }
        else {
          oa += a > 0.75f ? 0.1f : 0.01f;
          ob += 0.01f;
          x = 1.0f;
        }
        lim = x / scale.f - x;
      } while (k.f >= lim || k.f <= -lim);
    }
    else {
      a = fsca_cos((ushort)(int)((light->cone_angle[0] + oa) * 65536.0f / 6.283185f));
      b = fsca_cos((ushort)(int)((light->cone_angle[1] + ob) * 65536.0f / 6.283185f));
    }
    a *= mag;
    b *= mag;
    if ((elan_rev_config.flags & 8) == 0) {
      a = 1.0f - a;
      b = 1.0f - b;
    }
    k.f = 1.0f / (a - b);
    k.f = k.f * scale.f + k.f;
    m3 = 0xff000000 / (uint)(int)(1.0f / scale.f) | 0xff800000;
    k.i &= m3;
    k.i &= 0xffff0000;
    t.f = -b;
    t.f = t.f * k.f;
    t.i &= m3;
    t.i &= 0xffff0000;
    *out = k.i & 0xffff0000 | t.i >> 16;
  }
}

void elan_reg_d0_value_put(uint value)
{
  elan_reg_d0_value = value & 7;
  return;
}

void fb_pixel_addrs_by_chip(ushort *positions, uint *addrs, uint count)
{
  uint bank;
  int stride;
  uint **bitmap_slot;
  uint *bitmap;
  uint rows;
  uint i;
  uint x;
  uint y;
  uint col;
  uint other_chip;
  uint chip_bit;
  uint m = 0x80000000;
  
  bank = gfx_state.render_slot & 1;
  stride = gfx_state.field_read_offsets[1];
  bitmap_slot = (uint **)&(&tile_owner_bitmaps.buffer0)[bank];
  bitmap = *bitmap_slot;
  if (bitmap == 0) {
    tile_split_bitmap_update(bank);
    bitmap = *bitmap_slot;
  }
  rows = 1 << *bitmap++;
  for (i = 0; i < count; i++) {
    x = positions[i * 2];
    y = positions[i * 2 + 1];
    col = x >> 5;
    other_chip = (bitmap[(y >> 5) % rows] << col & m) == 0;
    chip_bit = other_chip != 0;
    addrs[i] =
         (chip_bit << 25) +
         gfx_state.fb_addrs[bank] +
         ((x * 2 & 0xfffffffc) + y * stride);
  }
}

uint elan_vram_read_batch(uint *words, uint count)
{
  uint buf[8];
  uint i;
  volatile uint *sq;
  uint old;

  *(volatile uint *)0xff000038 = 8;
  *(volatile uint *)0xff00003c = 8;
  sq = (volatile uint *)0xe1000000;
  elancmd_check_or_halt(words != 0);
  elancmd_check_or_halt(count < 9);
  if (count != 0) {
    for (i = 0; i < 8; i++) {
      buf[i] = (words[i] & 0x3fffffc) + 0xa5000000;
    }
    if (elan_read_bank != 0) {
      for (i = 4; i < count; i += 2) {
        while (*(volatile uint *)&elan_cmd_queue_level >= elan_rev_config.queue_limit) ;
        sq[6] = (i + *(volatile uint *)&elan_read_bank + 1) & 0xf;
        sq[5] = (buf[i + 1] & 0x1ffffffc) | 2;
        sq[4] = 0x08000e00;
        sq[2] = (i + *(volatile uint *)&elan_read_bank) & 0xf;
        sq[1] = (buf[i] & 0x1ffffffc) | 2;
        sq[0] = 0x08000e00;
        { void *a = (void *)sq; __asm__ __volatile__ ("pref @%0" : : "r" (a)); }
      }
    }
    else {
      for (i = 0; i < count && i < 4; i += 2) {
        while (*(volatile uint *)&elan_cmd_queue_level >= elan_rev_config.queue_limit) ;
        sq[6] = (i + 1) & 0xf;
        sq[5] = (buf[i + 1] & 0x1ffffffc) | 2;
        sq[4] = 0x08000e00;
        sq[2] = i & 0xf;
        sq[1] = (buf[i] & 0x1ffffffc) | 2;
        sq[0] = 0x08000e00;
        { void *a = (void *)sq; __asm__ __volatile__ ("pref @%0" : : "r" (a)); }
      }
    }
  }
  if (elan_read_pending != 0) {
    while (elan_done_count_reg == elan_read_done_count) {
      busy_wait_scaled(0.0001);
    }
    if (elan_read_bank != 0) {
      words += 7;
      *words = elan_read_result_50;
      *--words = elan_read_result_4c;
      *--words = elan_read_result_48;
      *--words = elan_read_result_44;
      *--words = elan_read_result_40;
      *--words = elan_read_result_3c;
      *--words = elan_read_result_38;
      *--words = elan_read_result_34;
    }
    else {
      words += 7;
      *words = elan_read_result_60;
      *--words = elan_read_result_5c;
      *--words = elan_read_result_58;
      *--words = elan_read_result_54;
      *--words = elan_read_result_50;
      *--words = elan_read_result_4c;
      *--words = elan_read_result_48;
      *--words = elan_read_result_44;
    }
  }
  if (count != 0) {
    elan_read_done_count = elan_done_count_reg;
    if (elan_read_bank != 0) {
      for (i = 0; i < count && i < 4; i += 2) {
        while (*(volatile uint *)&elan_cmd_queue_level >= elan_rev_config.queue_limit) ;
        sq[6] = (i + 5) & 0xf;
        sq[5] = (buf[i + 1] & 0x1ffffffc) | 2;
        sq[4] = 0x08000e00;
        sq[2] = (i + 4) & 0xf;
        sq[1] = (buf[i] & 0x1ffffffc) | 2;
        sq[0] = 0x08000e00;
        { void *a = (void *)sq; __asm__ __volatile__ ("pref @%0" : : "r" (a)); }
      }
    }
    else {
      for (i = 4; i < count; i += 2) {
        while (*(volatile uint *)&elan_cmd_queue_level >= elan_rev_config.queue_limit) ;
        sq[6] = (i + 1) & 0xf;
        sq[5] = (buf[i + 1] & 0x1ffffffc) | 2;
        sq[4] = 0x08000e00;
        sq[2] = i & 0xf;
        sq[1] = (buf[i] & 0x1ffffffc) | 2;
        sq[0] = 0x08000e00;
        { void *a = (void *)sq; __asm__ __volatile__ ("pref @%0" : : "r" (a)); }
      }
    }
    if (count != 0) {
      while (*(volatile uint *)&elan_cmd_queue_level >= elan_rev_config.queue_limit) ;
      sq[7] = 0x08000000;
      sq[6] = 0x08000000;
      sq[5] = 0x08000000;
      sq[4] = 0x08000000;
      sq[2] = 0xc;
      sq[1] = 0xffffffff;
      __asm__ __volatile__ ("pref @%0" : : "r" (sq + 1));
    }
  }
  old = elan_read_pending;
  elan_read_pending = count;
  elan_read_bank = elan_read_bank ^ 4;
  return old;
}
