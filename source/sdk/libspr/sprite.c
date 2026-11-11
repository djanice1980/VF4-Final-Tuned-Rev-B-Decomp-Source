/*
 * sdk/libspr/sprite.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float sprite_size_texels[8]; /* 0x0c21184c */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_height; /* 0x0c32a700 */
extern uint obj_palette_table; /* 0x0c3dc328 */
extern int sprite_error; /* 0x0c3e0344 */
extern int sprite_record_count; /* 0x0c3e0348 */
extern uint *sprite_record_bitmap; /* 0x0c3e034c */
extern short *sprite_index_to_record; /* 0x0c3e0350 */
extern int sprite_index_count; /* 0x0c3e0354 */
extern uint sprite_records; /* 0x0c3e0358 */
extern unit_rgb obj_light_color; /* 0x0c3258c0 */
extern sprite_coord_recip_pair sprite_coord_recip; /* 0x0c3e033c */
extern sprite_coord_extent_pair sprite_coord_extent; /* 0x0c3e035c */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

char * libspr_build_date_get(void)
{
  return libspr_build_date;
}

void sprite_work_partition(uchar *work, int record_count, int index_count)
{
  int i;

  sprite_error = 0xffffffff;
  sprite_coord_extent_store(640.0,480.0);
  sprite_index_count = index_count;
  sprite_record_count = record_count;
  sprite_index_to_record = (short *)work;
  work = (uchar *)((uint)(work + index_count * 2 + 3) & 0xfffffffc);
  sprite_record_bitmap = (uint *)work;
  work = (uchar *)((uint)(work + ((record_count + 0x1f) / 32) * 4 + 3) & 0xfffffffc);
  sprite_records = (uint)work;
  for (i = 0; i < index_count; i++) {
    sprite_index_to_record[i] = -1;
  }
  for (i = 0; i < (int)(sprite_record_count + 0x1f) / 32; i++) {
    ((int *)sprite_record_bitmap)[i] = 0;
  }
}

uint sprite_record_make(int list, int pal_numbers, int n, int by_hdr_index)
{
  int pos;
  uint *tab;
  tex_slot_rec *tex_slot;
  int off;
  int entry;
  uint status;
  texidx_map_entry *map_ent;
  int rec_no;
  int t;
  int pal_no;
  sprite_record_rec *rec;
  uint kind;
  short *group;
  int block;
  int size;

  status = 0;
  pos = n;
  entry = list + n * 0x10;
  if (by_hdr_index == 1) n = *(int *)(entry + 0xc);
  n = n + texidx_base;
  { int b = (int)texidx_map; map_ent = (texidx_map_entry *)(n * 8 + b); }
  rec_no = n;
  if (map_ent->use_count == 0) map_ent->slot_no = texidx_bit_take();
  texidx_map[n].use_count = texidx_map[n].use_count + 1;
  n = texidx_map[n].slot_no;
  tex_slot = tex_slot_table + n;
  if (sprite_index_to_record[rec_no] == -1) {
    t = sprite_record_bit_take();
    if (t != -1) {
      sprite_index_to_record[rec_no] = t;
      rec_no = t;
    }
    else {
      sprite_error = 3;
      return status | 2;
    }
  }
  else {
    rec_no = sprite_index_to_record[rec_no];
  }
  if (tex_slot->in_use == 0) {
    status |= tex_slot_create_and_load((short *)entry,n,*(int *)(entry + 8));
    tex_slot_words_or_fallback(n);
    tex_slot->slot_id = n;
  }
  tex_slot->in_use = tex_slot->in_use + 1;
  tab = &sprite_records;
  off = rec_no * 0x20;
  sprite_record_header_fill((sprite_record_rec *)(off + *tab),n);
  if (pal_numbers != 0) {
    pal_no = *(int *)(pal_numbers + pos * 4);
    rec = (sprite_record_rec *)(off + *tab);
    kind = rec->header[3] >> 0x1b & 7;
    rec->pal_group = -1;
    if (pal_raw_mode == 0) {
      if (obj_palette_table != 0 && kind - 5 < 2 && pal_no < graphics_work.pal_group_count) {
        rec->pal_group = pal_no;
        group = (short *)graphics_work.pal_group_table;
        t = obj_palette_table + pal_no * 0xc;
        group = group + pal_no * 4;
        if (group[2] != 0) {
          group[2] = group[2] + 1;
          block = group[0];
        }
        else {
          *(char *)(group + 1) = *(char *)(t + 2);
          *((char *)group + 3) = (uint)(*(int *)(t + 8) + 0xf) >> 4;
          group[2] = 1;
          group[3] = pal_no;
          size = *(int *)(t + 8);
          block = pal_find_free_blocks(*(short *)(t + 2),size);
          if (block >= 0) {
            group[0] = block;
            pal_mark_blocks_used(block,size);
            palette_shadow_write(block << 4,*(uint **)(t + 4),size);
          }
          else block = 0;
        }
        rec->header[3] = rec->header[3] & 0xf81fffff | block << 0x15;
      }
    }
    else if (kind - 5 < 2) {
      rec->header[3] = rec->header[3] & 0xf81fffff | pal_no << 0x15;
    }
  }
  return status;
}

int sprite_record_make_by_pos(int list, int pal_numbers, int n)
{
  int r;

  r = sprite_record_make(list,pal_numbers,n,0);
  return r;
}

void sprite_record_make_by_hdr_index(int list, int pal_numbers, int n)
{
  sprite_record_make(list,pal_numbers,n,1);
  return;
}

uint sprite_records_make_list(int list, int pal_numbers)
{
  uint r;
  int i;
  int p;
  r = 0;
  for (i = 0, p = list; *(int *)(p + 4) != -1; p += 0x10, i++) {
    r |= sprite_record_make(list,pal_numbers,i,0);
  }
  return r;
}

uint sprite_records_make_list_by_hdr_index(uchar *list, int pal_numbers)
{
  uint r;
  int i;
  uchar *q;
  r = 0;
  for (i = 0, q = list; *(int *)(q + 4) != -1; i++, q += 0x10) {
    r |= sprite_record_make((int)list,pal_numbers,i,1);
  }
  return r;
}

uint sprite_record_tex_bits_refresh(int list, int n, int by_hdr_index)
{
  int off;
  int index;
  uint keep;
  int p;
  uint r;

  r = 0;
  if (by_hdr_index == 1 && list != 0) {
    n = *(int *)(list + 0xc);
  }
  n += texidx_base;
  index = n;
  if (texidx_map[n].use_count == 0) {
    r = 0xffffffff;
  }
  else {
    n = texidx_map[n].slot_no;
    {
      short *tbl = sprite_index_to_record;
      off = tbl[index];
    }
    {
      int base = sprite_records;
      off *= 0x20;
      p = off + base;
    }
    keep = ((sprite_record_rec *)p)->header[3] & 0x7e00000;
    tex_slot_bits_into_poly_hdr(((sprite_record_rec *)p)->header,n);
    off += sprite_records;
    ((sprite_record_rec *)off)->header[3] = keep | ((sprite_record_rec *)off)->header[3];
  }
  return r;
}

uint sprite_records_tex_bits_refresh_list(int list)
{
  uint r;
  int i;
  int p;

  r = 0;
  for (i = 0, p = list; *(int *)(p + 4) != -1; p += 0x10, i++) {
    r |= sprite_record_tex_bits_refresh(list,i,0);
  }
  return r;
}

uint sprite_records_tex_bits_refresh_list_by_hdr_index(int list)
{
  uint r;
  int i;
  int *p;

  r = 0;
  i = 0;
  for (p = (int *)list; p[1] != -1; p += 4) {
    r |= sprite_record_tex_bits_refresh(list,i,1);
    i++;
  }
  return r;
}

void sprite_record_tex_bits_refresh_by_pos(int list, int n)
{
  sprite_record_tex_bits_refresh(list,n,0);
  return;
}

void sprite_record_tex_bits_refresh_by_hdr_index(int list, int n)
{
  sprite_record_tex_bits_refresh(list,n,1);
  return;
}

int sprite_record_drop(int list, int pal_numbers, int n, int by_hdr_index)
{
  int idx;
  int slot;
  tex_slot_rec *tex_slot;
  int r;
  short rec_no;
  int p;
  uint kind;
  uint group;
  texidx_map_entry *q;
  int entry;
  int index;

  entry = list + n * 0x10;
  if (by_hdr_index == 1) {
    n = *(int *)(entry + 0xc);
  }
  index = n + texidx_base;
  n = index;
  if (texidx_map[index].use_count != 0) {
    texidx_map[index].use_count--;
    q = &texidx_map[index];
    if (q->use_count != 0) return 0;
    slot = q->slot_no;
    { int b = (int)tex_slot_table; tex_slot = (tex_slot_rec *)(slot * 0x3c + b); }
    if (tex_slot->status == 0) {
      r = surface_memory_return_by_kind(&tex_slot->surface);
      if (r != 0) {
        sprite_error = 2;
        return r;
      }
    }
    tex_slot_table[slot].in_use = 0;
    tex_slot_table[slot].slot_id = -1;
    rec_no = sprite_index_to_record[index];
    texidx_bit_clear(slot);
    sprite_record_bit_clear(rec_no);
    texidx_map[n].slot_no = 0xffffffff;
    sprite_index_to_record[index] = -1;
    if (pal_raw_mode == '\0') {
      p = rec_no * 0x20 + sprite_records;
      kind = ((sprite_record_rec *)p)->header[3] >> 0x1b & 7;
      group = (ushort)((sprite_record_rec *)p)->pal_group;
      if (obj_palette_table != 0) {
        if (1 < kind - 5) return 0;
        if ((int)group >= graphics_work.pal_group_count) return 0;
        pal_group_unref(group);
      }
    }
  }
  return 0;
}

int sprite_record_drop_by_pos(int list, int pal_numbers, int n)
{
  int r;

  r = sprite_record_drop(list,pal_numbers,n,0);
  return r;
}

void sprite_record_drop_by_hdr_index(int list, int pal_numbers, int n)
{
  sprite_record_drop(list,pal_numbers,n,1);
  return;
}

uint sprite_records_drop_list(int list, int pal_numbers)
{
  int p;
  uint r;
  int i;

  r = 0;
  for (i = 0, p = list; *(int *)(p + 4) != -1; p += 0x10, i++) {
    r |= sprite_record_drop(list,pal_numbers,i,0);
  }
  return r;
}

uint sprite_records_drop_list_by_hdr_index(int list, int pal_numbers)
{
  uint r = 0;
  int i;
  int p;

  for (i = 0, p = list; *(int *)(p + 4) != -1; i++, p += 16)
    r |= sprite_record_drop(list,pal_numbers,i,1);
  return r;
}

void sprite_coord_extent_store(float width, float height)
{
  sprite_coord_recip.x_recip = 1.0 / width;
  sprite_coord_recip.y_recip = 1.0 / height;
  sprite_coord_extent.space_width = width;
  sprite_coord_extent.space_height = height;
  return;
}

void sprite_coord_extent_get(float *width, float *height)
{
  *width = sprite_coord_extent.space_width;
  *height = sprite_coord_extent.space_height;
  return;
}

uint sprite_quad_to_list(sprite_quad_args *args)
{
  typedef union { float f; uint i; } float_bits;
  sprite_record_rec *rec;
  uint *dst;
  uint *qacr;
  int translucent;
  int list;
  int idx;
  uint depth_cmp;
  uint color;
  uint v;
  float alpha;
  float half_w;
  float half_h;
  float inv_z;
  float sin_a;
  float cos_a;
  float xs[4];
  float ys[4];
  int i;
  float scale_x, scale_y;
  uint flip;
  float u0, u1, v0, v1;
  float_bits fb;
  uint hu0, hu1, lv0, lv1;

  if (args->index >= sprite_index_count) {
    return 0xffffffff;
  }
  idx = sprite_index_to_record[args->index];
  if (idx == -1) {
    return 0;
  }
  rec = (sprite_record_rec *)(sprite_records + (idx << 5));
  switch (args->list) {
  case 0:
    translucent = 0;
    break;
  default:
    alpha = args->alpha;
    translucent = 0;
    if (alpha < 1.0 || rec->has_alpha != 0) {
      translucent = 1;
    }
    break;
  case 2:
  case 4:
    alpha = args->alpha;
    translucent = 1;
    break;
  }
  half_w = rec->width_texels * args->zoom_x * 0.5;
  half_h = rec->height_texels * args->zoom_y * 0.5;
  inv_z = 1.0 / args->z;
  fsca_sincos16(args->angle,&sin_a,&cos_a);
  sprite_corners_from_anchor(args,xs,ys,half_w,half_h,sin_a,cos_a);
  scale_x = proj_rect_px_width * sprite_coord_recip.x_recip;
  scale_y = proj_rect_px_height * sprite_coord_recip.y_recip;
  for (i = 0; i < 4; i++) {
    xs[i] = xs[i] * scale_x;
    ys[i] = ys[i] * scale_y;
  }
  rec->header[0] &= 0xf8fcffff;
  rec->header[1] &= 0x1fffffff;
  rec->header[2] &= 0x3278fff;
  if ((args->attr & 0x800) == 0) {
    rec->header[2] |= 0x800000;
  }
  if ((args->attr & 0x1000) != 0) {
    rec->header[2] |= 0x2000;
  }
  if ((args->attr & 0x10000) != 0) {
    rec->header[2] |= 0x1000;
  }
  depth_cmp = ((uint)args->attr >> 8) & 7;
  if (depth_cmp == 0) {
    depth_cmp = 4;
  }
  rec->header[1] = depth_cmp << 29 | rec->header[1];
  if (translucent != 0) {
    if (args->list == 4) {
      rec->header[2] |= 0x94100000;
      rec->header[0] |= 0x4000000;
    }
    else {
      rec->header[2] = (blend_pair_bits ^ 0xfc) << 24 | rec->header[2] | 0x100000;
      rec->header[0] |= 0x2000000;
    }
  }
  else {
    rec->header[2] |= 0x20080000;
    alpha = 1.0;
  }
  if ((args->attr & 0x2000) != 0) {
    color = argb_pack_unit_floats(alpha,obj_light_color.r,obj_light_color.g,obj_light_color.b);
  }
  else if ((args->attr & 0x20000) != 0) {
    color = unit_float_to_byte(alpha) << 24 | args->base_color;
  }
  else {
    color = argb_pack_unit_floats(alpha,1.0,1.0,1.0);
  }
  rec->base_color = color;
  flip = args->attr & 0x30;
  u0 = args->u0;
  u1 = args->u1;
  if ((flip & 0x10) != 0) {
    u0 = 1.0 - u0;
    u1 = 1.0 - u1;
  }
  v0 = args->v0;
  v1 = args->v1;
  if ((flip & 0x20) == 0) {
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
  }
  switch (args->attr & 0xc000) {
  case 0x8000:
    rec->header[0] |= 0x20000;
    break;
  case 0xc000:
    rec->header[0] |= 0x30000;
    break;
  }
  fb.f = u0;
  hu0 = fb.i & 0xffff0000;
  fb.f = u1;
  hu1 = fb.i & 0xffff0000;
  fb.f = v0;
  lv0 = fb.i >> 16;
  fb.f = v1;
  lv1 = fb.i >> 16;
  list = args->list;
  if (list == -1) {
    list = translucent * 2;
  }
  dst = (uint *)list_write_ptrs[list];
  qacr = (uint *)0xff000038;
  qacr[0] = (uint)dst >> 24;
  qacr[1] = (uint)dst >> 24;
  dst = (uint *)((uint)dst & 0x3ffffff | 0xe0000000);
  if ((args->attr & 0x20000) != 0) {
    dst[0] = rec->header[0] | poly_header_or_words.control_or | 4;
  }
  else {
    dst[0] = rec->header[0] | poly_header_or_words.control_or;
  }
  dst[1] = rec->header[1];
  dst[2] = rec->header[2] | poly_header_or_words.tsp_or;
  dst[3] = rec->header[3];
  dst[4] = rec->base_color;
  dst[5] = unit_float_to_byte(alpha) << 24 | args->offset_color;
  __asm__ volatile ("pref @%0" : : "r" (dst));
  dst += 8;
  dst[0] = 0xf0000000;
  dst[1] = ((uint *)xs)[0];
  dst[2] = ((uint *)ys)[0];
  ((float *)dst)[3] = inv_z;
  dst[4] = ((uint *)xs)[1];
  dst[5] = ((uint *)ys)[1];
  ((float *)dst)[6] = inv_z;
  dst[7] = ((uint *)xs)[2];
  dst[8] = ((uint *)ys)[2];
  ((float *)dst)[9] = inv_z;
  dst[10] = ((uint *)xs)[3];
  dst[11] = ((uint *)ys)[3];
  dst[12] = idx | 0x53500000;
  dst[13] = hu0 | lv0;
  dst[14] = hu1 | lv0;
  dst[15] = lv1 | hu1;
  __asm__ volatile ("pref @%0" : : "r" (dst));
  dst += 8;
  __asm__ volatile ("pref @%0" : : "r" (dst));
  dst += 8;
  list_write_ptrs[list] = list_write_ptrs[list] & 0xfc000000 | (uint)dst & 0x3ffffff;
  return 0;
}

int sprite_record_pal_group_replace(int index, int pal_no)
{
  typedef struct { short first_block; char kind; char block_count; short use_count; short group_no; } pal_group_ent;
  sprite_record_rec *rec;
  uint word;
  uint kind;
  pal_group_ent *group;
  int src;
  int size;
  uint old;
  int block;
  graphics_work_rec *g;
  rec = (sprite_record_rec *)(sprite_records + sprite_index_to_record[index + texidx_base] * 0x20);
  word = rec->header[3];
  kind = word >> 0x1b & 7;
  if (pal_raw_mode != '\0') {
    if (kind - 5 < 2) {
      old = word;
      old >>= 0x15;
      old &= 0x3f;
      rec->header[3] = word & 0xf81fffff | pal_no << 0x15;
    }
    return old;
  }
  if (obj_palette_table != 0 && kind - 5 < 2) {
    old = (ushort)rec->pal_group;
    pal_group_unref(old);
    rec->pal_group = -1;
    g = &graphics_work;
    if (pal_no < g->pal_group_count) {
      rec->pal_group = pal_no;
      group = (pal_group_ent *)g->pal_group_table;
      src = obj_palette_table + pal_no * 0xc;
      group += pal_no;
      if (group->use_count != 0) {
        group->use_count++;
        block = group->first_block;
      }
      else {
        group->kind = *(char *)(src + 2);
        group->block_count = (uint)(*(int *)(src + 8) + 0xf) >> 4;
        group->use_count = 1;
        group->group_no = pal_no;
        size = *(int *)(src + 8);
        block = pal_find_free_blocks(*(short *)(src + 2),size);
        if (block >= 0) {
          group->first_block = block;
          pal_mark_blocks_used(block,size);
          palette_shadow_write(block << 4,*(uint **)(src + 4),size);
        }
        else block = 0;
      }
      rec->header[3] = rec->header[3] & 0xf81fffff | block << 0x15;
    }
  }
  return old;
}

void sprite_corners_from_anchor(sprite_quad_args *args, float *xs, float *ys, float half_w, float half_h, float sin_a, float cos_a)
{
  float *p;
  int i;
  float a, b, c, d;
  float dx, dy;
  float x, y;
  float ox, oy;
  float t[8];
  a = half_w * cos_a;
  b = half_h * sin_a;
  c = half_w * sin_a;
  d = half_h * cos_a;
  t[0] = 1.0;
  t[1] = 1.0;
  t[2] = 3.0;
  t[3] = 1.0;
  t[4] = 3.0;
  t[5] = 3.0;
  t[6] = 1.0;
  t[7] = 3.0;
  p = t;
  if (((uint)args->attr & 0xf) == 0) {
    ox = 1.0;
    oy = 1.0;
  }
  else {
    uint m = (uint)args->attr;
    ox = (float)(m & 3);
    oy = (float)(m >> 2 & 3);
  }
  for (i = 0; i < 4; i++) {
    dx = *p++;
    dy = *p++;
    dx -= ox;
    dy -= oy;
    x = args->x;
    y = args->y;
    if (dx != 0.0) {
      x = dx * a + x;
      dx = -dx;
      y = dx * c + y;
    }
    if (dy != 0.0) {
      x = dy * b + x;
      y = dy * d + y;
    }
    *xs++ = x;
    *ys++ = y;
  }
}

uint argb_pack_unit_floats(float alpha, float red, float green, float blue)
{
  uint a;
  uint b;
  uint c;
  uint d;
  
  alpha *= 255.0;
  red *= 255.0;
  green *= 255.0;
  blue *= 255.0;
  a = __fixunssfsi(alpha);
  b = __fixunssfsi(red);
  c = __fixunssfsi(green);
  d = __fixunssfsi(blue);
  if (a > 0xff) a = 0xff;
  if (b > 0xff) b = 0xff;
  if (c > 0xff) c = 0xff;
  if (d > 0xff) d = 0xff;
  a = a << 8 | b;
  a = a << 8 | c;
  a = a << 8 | d;
  return a;
}

void sprite_record_header_fill(sprite_record_rec *rec, int slot)
{
  uint kind;
  float *sizes;
  uint tsp;
  float *dims;

  rec->header[0] = 0xa0000009;
  rec->header[1] = 0;
  rec->header[2] = 0x4c0;
  rec->header[3] = 0;
  tex_slot_bits_into_poly_hdr(rec->header,slot);
  kind = rec->header[3] >> 0x1b & 7;
  rec->has_alpha = 0;
  switch(kind) {
  case 0:
  case 2:
  case 5:
  case 6:
    rec->has_alpha = 1;
  }
  dims = &rec->width_texels;
  sizes = sprite_size_texels;
  tsp = rec->header[2];
  dims[0] = sizes[tsp >> 3 & 7];
  dims[1] = sizes[tsp & 7];
  return;
}

int sprite_record_bit_take(void)
{
  uint v;
  int i;
  int bit;
  uint mask;

  for (i = 0; i < (int)(sprite_record_count + 0x1f) / 32; i++) {
    v = ((uint *)sprite_record_bitmap)[i];
    if (v != 0xffffffff) {
      bit = 0;
      mask = 1;
      while (1) {
        if ((v & mask) == 0) {
          ((uint *)sprite_record_bitmap)[i] |= mask;
          return i * 0x20 + bit;
        }
        mask = mask * 2;
        bit++;
      }
    }
  }
  return -1;
}

void sprite_record_bit_clear(int rec_no)
{
  uint m;
  uint *p;

  m = 1 << (rec_no & 0x1f);
  ((uint *)sprite_record_bitmap)[rec_no >> 5] &= ~m;
  return;
}

uint unit_float_to_byte(float value)
{
  uint result;

  result = __fixunssfsi(value * 255.0);
  if (0xff < result) {
    result = 0xff;
  }
  return result;
}
