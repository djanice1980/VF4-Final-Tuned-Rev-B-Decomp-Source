/*
 * sdk/kamui/strip_context.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint shadow_scale_off; /* 0x0c32a768 */
extern uint strip_header_words[16]; /* 0x0c32a770 */
extern uchar vertex_kind_table; /* 0x0c32a7b0 */
extern uint list_cursor_slot; /* 0x0c41df8c */
extern uint strip_list_type; /* 0x0c41df90 */
extern uint strip_vertex_kind; /* 0x0c5d5cac */
extern list_write_rec list_write_state; /* 0x0c41e000 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern strip_header_src strip_header_shadow; /* 0x0c5d5c3c */

uint uv_size_code(int side)
{
  uint code;

  if (side == 8) {
    code = 0;
  }
  else if (side == 0x10) {
    code = 1;
  }
  else if (side == 0x20) {
    code = 2;
  }
  else if (side == 0x40) {
    code = 3;
  }
  else if (side == 0x80) {
    code = 4;
  }
  else if (side == 0x100) {
    code = 5;
  }
  else if (side == 0x200) {
    code = 6;
  }
  else if (side == 0x400) {
    code = 7;
  }
  return code;
}

void strip_header_build(void)
{
  typedef struct { uint w[24]; } poly_src;
  uint *p;
  uint w;
  int v;
  uint t;
  uint lo;

  p = strip_header_words;
  w = ((poly_src *)&strip_header_shadow)->w[0];
  t = ((w & 0x20000000) >> 24) | ((w & 0x1000000) >> 20) | ((w & 0x40) >> 3) | ((w & 0x20) >> 3) | ((w & 0x10) >> 3);
  lo = w & 4;
  v = *(int *)(&vertex_kind_table + (t << 2 | lo));
  strip_vertex_kind = v;
  *p++ = w;
  w = (w >> 24) & 7;
  strip_list_type = w;
  if (!(w & 1)) {
    *p++ = ((poly_src *)&strip_header_shadow)->w[4];
    *p++ = ((poly_src *)&strip_header_shadow)->w[5];
    *p++ = ((poly_src *)&strip_header_shadow)->w[6];
    switch (v) {
    case 0:
      p += 4;
      break;
    case 1:
      *p++ = ((uint *)&strip_header_shadow.color1_0)[0];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[1];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[2];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[3];
      break;
    case 2:
      p += 4;
      *p++ = ((uint *)&strip_header_shadow.color1_0)[0];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[1];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[2];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[3];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[4];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[5];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[6];
      *p++ = ((uint *)&strip_header_shadow.color1_0)[7];
      break;
    case 3:
      *p++ = ((poly_src *)&strip_header_shadow)->w[17];
      *p = ((poly_src *)&strip_header_shadow)->w[18];
      p += 3;
      break;
    case 4:
      *p++ = ((poly_src *)&strip_header_shadow)->w[17];
      *p = ((poly_src *)&strip_header_shadow)->w[18];
      p += 3;
      *p++ = ((poly_src *)&strip_header_shadow)->w[8];
      *p++ = ((poly_src *)&strip_header_shadow)->w[9];
      *p++ = ((poly_src *)&strip_header_shadow)->w[10];
      *p++ = ((poly_src *)&strip_header_shadow)->w[11];
      *p++ = *(uint *)&strip_header_shadow.vol2_color_0;
      *p++ = *(uint *)&strip_header_shadow.vol2_color_1;
      *p++ = *(uint *)&strip_header_shadow.vol2_color_2;
      *p++ = *(uint *)(&strip_header_shadow.vol2_color_2 + 1);
      break;
    case 16:
      *p++ = (int)(strip_header_shadow.color1_0 * 255.0) << 24 | (int)(strip_header_shadow.color1_1 * 255.0) << 16 | (int)(strip_header_shadow.color1_2 * 255.0) << 8 | (int)(strip_header_shadow.color1_3 * 255.0);
      *p = (int)(strip_header_shadow.color2_0 * 255.0) << 24 | (int)(strip_header_shadow.color2_1 * 255.0) << 16 | (int)(*(float *)&strip_header_shadow.color2_2 * 255.0) << 8 | (int)(((float *)&strip_header_shadow)[15] * 255.0);
      p += 3;
      break;
    }
  }
  else {
    *p = ((poly_src *)&strip_header_shadow)->w[16];
    p += 7;
  }
  list_write_state.list_type = strip_list_type;
  list_write_state.header_bytes = (char *)p - (char *)strip_header_words;
}

int context_hw_words_compute(strip_context_rec *ctx)
{
  uint isp;
  uint tsp;
  uint tex;
  uint ctrl;
  uint flags;
  uint v;
  uint t;
  int filter_codes[4];

  isp = ctx->isp_tsp_word;
  tsp = ctx->tsp_word;
  tex = ctx->tex_word;
  ctrl = ctx->ctrl_word;
  filter_codes[0] = 0;
  filter_codes[1] = 2;
  filter_codes[2] = 1;
  filter_codes[3] = 3;
  flags = ctx->change_mask;
  if ((flags & 1) != 0) {
    isp = isp & 0x1fffffff | ctx->depth_compare << 29;
  }
  if ((flags & 2) != 0) {
    isp = isp & 0xe7ffffff | ctx->cull_mode << 27;
  }
  if ((flags & 0x20) != 0) {
    if (ctx->z_write_off != 0) {
      isp = isp | 0x4000000;
    }
    else {
      isp = isp & 0xfbffffff;
    }
  }
  if ((flags & 8) != 0) {
    isp = isp & 0xfdffffff | (ctx->tex_offset_bits >> 1) << 25;
  }
  if ((flags & 0x200) != 0) {
    isp = isp & 0xfeffffff | ctx->gouraud << 24;
  }
  if ((flags & 8) != 0) {
    isp = isp & 0xff7fffff | (ctx->tex_offset_bits & 1) << 23;
  }
  if ((flags & 0x800000) != 0) {
    isp = isp & 0xffbfffff | ctx->uv16 << 22;
  }
  if ((flags & 0x4000000) != 0) {
    if (ctx->isp_bit20_on != 0) {
      isp = isp | 0x100000;
    }
    else {
      isp = isp & 0xffefffff;
    }
  }
  if ((ctx->list_type & 1) != 0) {
    isp = isp & 0x1fffffff | ctx->volume_word;
  }
  if ((flags & 0x40) != 0) {
    switch (ctx->src_blend) {
    case 0:
      tsp = tsp & 0x1fffffff | 0xa0000000;
      tsp = tsp & 0xe3ffffff | 0x10000000;
      break;
    case 1:
      tsp = tsp & 0x1fffffff | 0x80000000;
      tsp = tsp & 0xe3ffffff | 0x14000000;
      break;
    case 2:
      tsp = tsp & 0x1fffffff | 0xc0000000;
      break;
    case 3:
      tsp = tsp & 0x1fffffff | 0x40000000;
      break;
    case 4:
      tsp = tsp & 0x1fffffff | 0xe0000000;
      break;
    case 5:
      tsp = tsp & 0x1fffffff | 0x60000000;
      break;
    case 6:
      tsp = tsp & 0x1fffffff | 0xa0000000;
      break;
    case 8:
      tsp = tsp & 0x1fffffff | 0x80000000;
      break;
    case 10:
      tsp = tsp & 0x1fffffff | 0x20000000;
      break;
    case 11:
      tsp = tsp & 0x1fffffff;
      break;
    }
  }
  if ((flags & 0x80) != 0) {
    switch (ctx->dst_blend) {
    case 0:
      tsp = tsp & 0x1fffffff | 0xa0000000;
      tsp = tsp & 0xe3ffffff | 0x10000000;
      break;
    case 1:
      tsp = tsp & 0x1fffffff | 0x80000000;
      tsp = tsp & 0xe3ffffff | 0x14000000;
      break;
    case 2:
      tsp = tsp & 0xe3ffffff | 0x18000000;
      break;
    case 4:
      tsp = tsp & 0xe3ffffff | 0x1c000000;
      break;
    case 6:
      tsp = tsp & 0xe3ffffff | 0x14000000;
      break;
    case 7:
      tsp = tsp & 0xe3ffffff | 0xc000000;
      break;
    case 8:
      tsp = tsp & 0xe3ffffff | 0x10000000;
      break;
    case 9:
      tsp = tsp & 0xe3ffffff | 0x8000000;
      break;
    case 10:
      tsp = tsp & 0xe3ffffff | 0x4000000;
      break;
    case 11:
      tsp = tsp & 0xe3ffffff;
      break;
    }
  }
  if ((flags & 0x1000000) != 0) {
    if (ctx->src_select != 0) {
      tsp = tsp | 0x2000000;
    }
    else {
      tsp = tsp & 0xfdffffff;
    }
  }
  if ((flags & 0x2000000) != 0) {
    if (ctx->dst_select != 0) {
      tsp = tsp | 0x1000000;
    }
    else {
      tsp = tsp & 0xfeffffff;
    }
  }
  if ((flags & 0x100) != 0) {
    v = ctx->fog_mode & 3;
    tsp = tsp & 0xff3fffff | v << 22;
  }
  if ((flags & 0x40000) != 0) {
    if (ctx->colors_clamped != 0) {
      tsp = tsp | 0x200000;
    }
    else {
      tsp = tsp & 0xffdfffff;
    }
  }
  if ((flags & 0x400) != 0) {
    if (ctx->alpha_on != 0) {
      tsp = tsp | 0x100000;
    }
    else {
      tsp = tsp & 0xffefffff;
    }
  }
  if ((flags & 0x800) != 0) {
    if (ctx->tex_alpha_off != 0) {
      tsp = tsp | 0x80000;
    }
    else {
      tsp = tsp & 0xfff7ffff;
    }
  }
  if ((flags & 0x1000) != 0) {
    tsp = tsp & 0xfffe7fff | ctx->uv_clamp << 15;
  }
  if ((flags & 0x2000) != 0) {
    tsp = tsp & 0xfff9ffff | ctx->uv_flip << 17;
  }
  if ((flags & 0x4000) != 0) {
    v = filter_codes[ctx->tex_filter];
    tsp = tsp & 0xffff9fff | v << 13;
  }
  if ((flags & 0x8000) != 0) {
    if (ctx->tsp_bit12_on != 0) {
      tsp = tsp | 0x1000;
    }
    else {
      tsp = tsp & 0xffffefff;
    }
  }
  if ((flags & 0x10000) != 0) {
    v = ctx->mip_bias & 0xf;
    if (v == 0) {
      v = 4;
    }
    tsp = tsp & 0xfffff0ff | v << 8;
  }
  if ((flags & 0x20000) != 0) {
    v = ctx->shade_op & 3;
    tsp = tsp & 0xffffff3f | v << 6;
  }
  if ((ctx->tex_offset_bits >> 1) == 1 && ctx->surface != 0) {
    tsp = tsp & 0xffffffc7;
    tsp = tsp | uv_size_code(ctx->surface->width) << 3;
    tsp = tsp & 0xfffffff8;
    tsp = tsp | uv_size_code(ctx->surface->height);
  }
  if ((ctx->tex_offset_bits >> 1) == 1 && ctx->surface != 0) {
    tex = tex & 0x7fffffff;
    tex = tex | ctx->surface->format_flags << 31;
    tex = tex & 0xbfffffff;
    tex = tex | (ctx->surface->format_flags & 8) << 27;
    tex = tex & 0xc7ffffff;
    tex = tex | ctx->surface->pixel_format & 0x38000000;
    t = tex & 0x38000000;
    if (t == 0x28000000 || t == 0x30000000) {
      tex = tex & 0xf81fffff | (ctx->pal_group & 0x3f) << 21;
    }
    else {
      tex = tex & 0xfbffffff;
      tex = tex | (~(ctx->surface->format_flags & 4) & 4) << 24;
      tex = tex & 0xfdffffff;
      tex = tex | (ctx->surface->format_flags & 0x10) << 21;
    }
    tex = tex & 0xffe00000 | ctx->surface->tex_address >> 3;
  }
  if ((flags & 0x100000) != 0) {
    v = ctx->header_type;
    if (v == 0) {
      ctrl = ctrl & 0x1fffffff | 0x80000000;
    }
    if (v == 1) {
      ctrl = ctrl & 0x1fffffff | 0x80000000;
    }
    if (v == 2) {
      ctrl = ctrl & 0x1fffffff | 0xa0000000;
    }
  }
  if ((flags & 0x200000) != 0) {
    ctrl = ctrl & 0xf8ffffff | ctx->list_type << 24;
  }
  ctrl = ctrl | 0x800000;
  if ((flags & 0x8000000) != 0) {
    ctrl = ctrl & 0xfff3ffff | ctx->strip_len_code << 18;
  }
  else {
    ctrl = ctrl & 0xfff3ffff | gfx_state.strip_len_code << 18;
  }
  if ((flags & 0x10000000) != 0) {
    ctrl = ctrl & 0xfffcffff | ctx->clip_kind << 16;
  }
  if ((ctx->volume_word & 0x60000000) != 0 && (ctx->list_type & 1) != 0) {
    ctrl = ctrl & 0xffffff3f | 0x40;
  }
  else {
    ctrl = ctrl & 0xffffff3f;
  }
  if ((flags & 0x10) != 0 && (ctx->two_volume & 1) != 0) {
    ctrl = ctrl & 0xffffff7f | ctx->two_volume << 7;
    ctrl = ctrl & 0xffffffbf | shadow_scale_off << 6;
  }
  if ((flags & 0x400000) != 0) {
    ctrl = ctrl & 0xffffffcf | ctx->color_format << 4;
  }
  if ((flags & 8) != 0) {
    ctrl = ctrl & 0xfffffff7 | (ctx->tex_offset_bits >> 1) << 3;
  }
  if ((flags & 8) != 0) {
    ctrl = ctrl & 0xfffffffd | (ctx->tex_offset_bits & 1) * 2;
  }
  if ((flags & 0x200) != 0) {
    if (ctx->gouraud != 0) {
      ctrl = ctrl | 4;
    }
    else {
      ctrl = ctrl & 0xfffffffb;
    }
  }
  if ((flags & 0x800000) != 0) {
    ctrl = ctrl & 0xfffffffe | ctx->uv16;
  }
  ctrl = ctrl & 0xe78f00ff;
  isp = isp & 0xffd00000;
  ctx->isp_tsp_word = isp;
  ctx->tsp_word = tsp;
  ctx->tex_word = tex;
  ctx->ctrl_word = ctrl;
  return 0;
}

int context_header_select(strip_context_rec *ctx)
{
  strip_header_shadow.ctrl_word = ctx->ctrl_word;
  strip_header_shadow.isp_tsp_word = ctx->isp_tsp_word;
  (*(uint *)((char *)&strip_header_shadow + 20)) = ctx->tsp_word;
  if ((strip_header_shadow.ctrl_word >> 0x18 & 7) == 0) {
    (*(uint *)((char *)&strip_header_shadow + 20)) = (*(uint *)((char *)&strip_header_shadow + 20)) & 0x3ffffff | 0x20000000;
  }
  strip_header_shadow.tex_word = ctx->tex_word;
  strip_header_shadow.color1_0 = ctx->face_color[0];
  strip_header_shadow.color1_1 = ctx->face_color[1];
  strip_header_shadow.color1_2 = ctx->face_color[2];
  strip_header_shadow.color1_3 = ctx->face_color[3];
  strip_header_shadow.color2_0 = ctx->offset_color[0];
  strip_header_shadow.color2_1 = ctx->offset_color[1];
  *(float *)&strip_header_shadow.color2_2 = ctx->offset_color[2];
  (*(float *)((char *)&strip_header_shadow + 60)) = ctx->offset_color[3];
  strip_header_shadow.odd_list_word = ctx->volume_word;
  strip_header_build();
  return 0;
}

int context_volume2_header_select(strip_context_rec *ctx)
{
  strip_header_shadow.vol2_tsp_word = ctx->tsp_word;
  strip_header_shadow.vol2_tex_word = ctx->tex_word;
  strip_header_shadow.vol2_color_0 = ctx->face_color[0];
  strip_header_shadow.vol2_color_1 = ctx->face_color[1];
  strip_header_shadow.vol2_color_2 = ctx->face_color[2];
  ((struct { char pad[0x5c]; float f; } *)&strip_header_shadow)->f = ctx->face_color[3];
  strip_header_build();
  return 0;
}

int strip_len_code_set(uint code)
{
  gfx_state.strip_len_code = code;
  return 0;
}

int pvr_shadow_scale_write(int scale)
{
  if (scale < 0) shadow_scale_off = 1; else shadow_scale_off = 0;
  pvr_reg_write_both(0x74,(1 - shadow_scale_off) * 0x100 | scale & 0xff);
  return 0;
}

int * strip_header_to_list(void)
{
  *(int **)&list_cursor_slot = (int *)(list_write_ptrs + strip_list_type);
  copy_by_pairs_or_words((uint *)**(int **)&list_cursor_slot,strip_header_words,strip_header_word_count << 2);
  **(int **)&list_cursor_slot = strip_header_word_count * 4 + **(int **)&list_cursor_slot;
  return *(int **)&list_cursor_slot;
}

int strip_hdr_ctrl_bits4_set(int value)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[0] & 0xffffffcf) | (value << 4);
  ((poly_src *)&strip_header_shadow)->w[0] = word;
  strip_header_words[0] = word;
  return 0;
}

int strip_hdr_depth_compare_set(int mode)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[4] & 0x1fffffff) | (mode << 29);
  ((poly_src *)&strip_header_shadow)->w[4] = word;
  strip_header_words[1] = word;
  return 0;
}

int strip_hdr_cull_mode_set(int mode)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[4] & 0xe7ffffff) | (mode << 27);
  ((poly_src *)&strip_header_shadow)->w[4] = word;
  strip_header_words[1] = word;
  return 0;
}

int strip_hdr_z_write_off_set(int on)
{
  typedef struct { uint w[24]; } poly_src;

  if (on != 0) {
    ((poly_src *)&strip_header_shadow)->w[4] |= 0x4000000;
  }
  else {
    ((poly_src *)&strip_header_shadow)->w[4] &= ~0x4000000;
  }
  strip_header_words[1] = ((poly_src *)&strip_header_shadow)->w[4];
  return 0;
}

int strip_hdr_src_blend_set(int mode)
{
  typedef struct { uint w[24]; } poly_src;

  switch (mode) {
  case 0:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0xa0000000;
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x10000000;
    break;
  case 1:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0x80000000;
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x14000000;
    break;
  case 2:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0xc0000000;
    break;
  case 3:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0x40000000;
    break;
  case 4:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0xe0000000;
    break;
  case 5:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0x60000000;
    break;
  case 6:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0xa0000000;
    break;
  case 8:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0x80000000;
    break;
  case 10:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0x20000000;
    break;
  case 11:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff;
    break;
  }
  strip_header_words[2] = ((poly_src *)&strip_header_shadow)->w[5];
  return 0;
}

int strip_hdr_dst_blend_set(int mode)
{
  typedef struct { uint w[24]; } poly_src;

  switch (mode) {
  case 0:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0xa0000000;
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x10000000;
    break;
  case 1:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0x1fffffff | 0x80000000;
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x14000000;
    break;
  case 2:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x18000000;
    break;
  case 4:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x1c000000;
    break;
  case 6:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x14000000;
    break;
  case 7:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0xc000000;
    break;
  case 8:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x10000000;
    break;
  case 9:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x8000000;
    break;
  case 10:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff | 0x4000000;
    break;
  case 11:
    ((poly_src *)&strip_header_shadow)->w[5] = ((poly_src *)&strip_header_shadow)->w[5] & 0xe3ffffff;
    break;
  }
  strip_header_words[2] = ((poly_src *)&strip_header_shadow)->w[5];
  return 0;
}

int strip_hdr_fog_mode_set(int mode)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[5] & 0xff3fffff) | (mode << 22);
  ((poly_src *)&strip_header_shadow)->w[5] = word;
  strip_header_words[2] = word;
  return 0;
}

int strip_hdr_uv_flip_set(int bits)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[5] & 0xfff9ffff) | (bits << 17);
  ((poly_src *)&strip_header_shadow)->w[5] = word;
  strip_header_words[2] = word;
  return 0;
}

int strip_hdr_uv_clamp_set(int bits)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[5] & 0xfffe7fff) | (bits << 15);
  ((poly_src *)&strip_header_shadow)->w[5] = word;
  strip_header_words[2] = word;
  return 0;
}

int strip_hdr_tex_filter_set(int filter)
{
  typedef struct { uint w[24]; } poly_src;
  int codes[4];
  uint word;

  codes[0] = 0;
  codes[1] = 2;
  codes[2] = 1;
  codes[3] = 3;
  filter = codes[filter];
  word = (((poly_src *)&strip_header_shadow)->w[5] & 0xffff9fff) | (filter << 13);
  ((poly_src *)&strip_header_shadow)->w[5] = word;
  strip_header_words[2] = word;
  return 0;
}

int strip_hdr_tsp_bit12_set(int on)
{
  typedef struct { uint w[24]; } poly_src;

  if (on != 0) {
    ((poly_src *)&strip_header_shadow)->w[5] |= 0x1000;
  }
  else {
    ((poly_src *)&strip_header_shadow)->w[5] &= ~0x1000;
  }
  strip_header_words[2] = ((poly_src *)&strip_header_shadow)->w[5];
  return 0;
}

int strip_hdr_shade_op_set(int op)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[5] & 0xffffff3f) | (op << 6);
  ((poly_src *)&strip_header_shadow)->w[5] = word;
  strip_header_words[2] = word;
  return 0;
}

int strip_hdr_gouraud_set(int on)
{
  typedef struct { uint w[24]; } poly_src;

  if (on != 0) {
    ((poly_src *)&strip_header_shadow)->w[5] |= 0x200000;
  }
  else {
    ((poly_src *)&strip_header_shadow)->w[5] &= ~0x200000;
  }
  strip_header_words[2] = ((poly_src *)&strip_header_shadow)->w[5];
  return 0;
}

int strip_hdr_pal_group_set(uint group)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;
  uint format;

  format = ((poly_src *)&strip_header_shadow)->w[6] & 0x38000000;
  if (format == 0x28000000 || format == 0x30000000) {
    word = (((poly_src *)&strip_header_shadow)->w[6] & 0xf81fffff) | ((group & 0x3f) << 21);
    ((poly_src *)&strip_header_shadow)->w[6] = word;
    strip_header_words[3] = word;
  }
  return 0;
}

int strip_hdr_ctrl_bits18_set(int value)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[0] & 0xfff3ffff) | (value << 18) | 0x800000;
  ((poly_src *)&strip_header_shadow)->w[0] = word;
  strip_header_words[0] = word;
  return 0;
}

int strip_hdr_ctrl_bits16_set(int value)
{
  typedef struct { uint w[24]; } poly_src;
  uint word;

  word = (((poly_src *)&strip_header_shadow)->w[0] & 0xfffcffff) | (value << 16) | 0x800000;
  ((poly_src *)&strip_header_shadow)->w[0] = word;
  strip_header_words[0] = word;
  return 0;
}
