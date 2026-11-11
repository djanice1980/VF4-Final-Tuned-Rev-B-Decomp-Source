/*
 * gfxlib/screen_prims.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float scr_proj_m00; /* 0x0c416260 */
extern float scr_height_f; /* 0x0c416264 */
extern float scr_width_f; /* 0x0c416268 */
extern float scr_half_width; /* 0x0c41626c */
extern float scr_proj_m11; /* 0x0c416270 */
extern float scr_unproj_x; /* 0x0c416274 */
extern float scr_unproj_y; /* 0x0c416278 */
extern float scr_half_height; /* 0x0c41627c */
extern char mtx_stack_selected; /* 0x0c41c79c */
extern float proj_rect_width; /* 0x0c41c950 */
extern float proj_rect_height; /* 0x0c41ca20 */
extern elan_matrix_param_rec elan_matrix_params; /* 0x0c3dc188 */

uint elan_write_ptr_get(void)
{
  return (uint)elan_cmd_write_ptr;
}

void elan_write_ptr_set(uint addr)
{
  elan_cmd_write_ptr = (uint *)addr;
  return;
}

uint model_matrix_cmds_write_at(uint dst)
{
  uint end;

  end = (uint)elan_matrix_cmds_write((uint *)dst,&elan_matrix_params);
  return end;
}

uint model_part0_call_write_at(uint addr, compiled_model_rec *model)
{
  list_block_ref part;
  uint *dst = (uint *)addr;

  part.addr = (int)model + model->parts[0].offset + 0x40U;
  part.length = model->parts[0].size;
  dst[0] = 0x08000800;
  dst[1] = 0x18000000;
  dst[2] = 0;
  dst[3] = 0;
  dst[4] = part.addr & 0x1fffff8 | 0x80000000;
  dst[5] = 1;
  dst[6] = part.length;
  dst[7] = 0;
  __asm__ __volatile__ ("pref @%0" : : "r" (dst));
  return (uint)(dst + 8);
}

void raw_block_queue_lists_0_2(uint list_mask, uint *words)
{
  list_block_ref block;
  uint *d;
  int i;

  d = elan_cmd_write_ptr;
  block.addr = (uint)d;
  for (i = 0; i < 8; i++)
    *d++ = *words++;
  d += 8;
  block.length = (uint)d - block.addr;
  if (list_mask & 1) elan_list_block_queue(0,&block);
  if (list_mask & 4) elan_list_block_queue(2,&block);
  elan_cmd_write_ptr = d;
}

void screen_rect_flat_queue(int list, int x, int y, int width, int height, uint color, uint hdr_word1, uint hdr_word2, float z)
{
  list_block_ref block;
  uint col;
  float x0;
  float x1;
  float y0;
  float y1;
  uint *p;

  col = color;
  x0 = (float)x;
  x1 = (float)(x + width);
  y0 = (float)y;
  y1 = (float)(y + height);
  p = elan_cmd_write_ptr;
  block.addr = (uint)p;
  p[0] = 0x80000002;
  p[1] = hdr_word1;
  p[2] = hdr_word2;
  p[3] = 0;
  p = p + 8;
  p[0] = 0xe0000002;
  ((float *)p)[1] = x0;
  ((float *)p)[2] = y0;
  ((float *)p)[3] = z;
  p[6] = col;
  p = p + 8;
  p[0] = 0xe0000002;
  ((float *)p)[1] = x1;
  ((float *)p)[2] = y0;
  ((float *)p)[3] = z;
  p[6] = col;
  p = p + 8;
  p[0] = 0xe0000002;
  ((float *)p)[1] = x0;
  ((float *)p)[2] = y1;
  ((float *)p)[3] = z;
  p[6] = col;
  p = p + 8;
  p[0] = 0xf0000002;
  ((float *)p)[1] = x1;
  ((float *)p)[2] = y1;
  ((float *)p)[3] = z;
  p[6] = col;
  p = p + 16;
  block.length = (int)p - (int)block.addr;
  elan_list_block_queue(list,&block);
  elan_cmd_write_ptr = p;
}

void screen_sprite_flat_queue(int list, int x, int y, int width, int height, uint color, uint hdr_word1, uint hdr_word2, float z)
{
  list_block_ref block;
  uint *p;
  float x0 = (float)x;
  float x1 = (float)(x + width);
  float y0 = (float)y;
  float y1 = (float)(y + height);
  p = elan_cmd_write_ptr;
  block.addr = (uint)p;
  p[0] = 0xa0000000;
  p[1] = hdr_word1;
  p[2] = hdr_word2;
  p[3] = 0;
  p[4] = color;
  p[5] = 0;
  p += 8;
  p[0] = 0xf0000000;
  ((float *)p)[1] = x0;
  ((float *)p)[2] = y0;
  ((float *)p)[3] = z;
  ((float *)p)[4] = x1;
  ((float *)p)[5] = y0;
  ((float *)p)[6] = z;
  ((float *)p)[7] = x1;
  ((float *)p)[8] = y1;
  ((float *)p)[9] = z;
  ((float *)p)[10] = x0;
  ((float *)p)[11] = y1;
  p += 24;
  block.length = (char *)p - (char *)block.addr;
  elan_list_block_queue(list,&block);
  elan_cmd_write_ptr = p;
}

void screen_sprite_textured_queue(int list, int tex_index, int x, int y, int width, int height, uint color, uint offset_color, uint hdr_word, uint tex_bits, float z)
{
  float uv[4];
  list_block_ref block;
  uint *d;
  tex_slot_rec *slot;
  float x0, x1, y0, y1;
  ushort u0, u1, v0, v1;

  x0 = (float)x;
  x1 = (float)(x + width);
  y0 = (float)y;
  y1 = (float)(y + height);
  uv[0] = 0.0f;
  uv[1] = 1.0f;
  uv[2] = 0.0f;
  uv[3] = 1.0f;
  u0 = ((ushort *)uv)[1];
  u1 = ((ushort *)uv)[3];
  v0 = ((ushort *)uv)[5];
  v1 = ((ushort *)uv)[7];
  slot = texidx_slot_get(tex_index);
  d = elan_cmd_write_ptr;
  block.addr = (uint)d;
  d[0] = 0xa0000009;
  d[1] = hdr_word;
  d[2] = tex_bits | slot->size_word;
  d[3] = slot->tex_control_word;
  d[4] = color;
  d[5] = offset_color;
  d += 8;
  d[0] = 0xf0000000;
  ((float *)d)[1] = x0;
  ((float *)d)[2] = y0;
  ((float *)d)[3] = z;
  ((float *)d)[4] = x1;
  ((float *)d)[5] = y0;
  ((float *)d)[6] = z;
  ((float *)d)[7] = x1;
  ((float *)d)[8] = y1;
  ((float *)d)[9] = z;
  ((float *)d)[10] = x0;
  ((float *)d)[11] = y1;
  d[13] = u0 << 16 | v0;
  d[14] = u1 << 16 | v0;
  d[15] = u1 << 16 | v1;
  d += 24;
  block.length = (int)d - (int)block.addr;
  elan_list_block_queue(list,&block);
  elan_cmd_write_ptr = d;
}

void screen_unproject_consts_set(int width, int height)
{
  int old_stack;
  float m [16];
  float m00;
  float m11;
  float kx;
  float ky;

  scr_half_width = (float)width * 0.5;
  scr_half_height = (float)height * 0.5;
  old_stack = (int)mtx_stack_selected;
  mtx_stack_select(3);
  mtx_store_wordwise(m);
  m00 = m[0];
  m11 = m[5];
  mtx_stack_select(old_stack);
  kx = (-1.0 / scr_half_width) / m00;
  ky = (1.0 / scr_half_height) / m11;
  scr_unproj_x = kx;
  scr_unproj_y = ky;
  scr_proj_m00 = m00;
  scr_proj_m11 = m11;
  scr_width_f = (float)width;
  scr_height_f = (float)height;
  return;
}

void screen_map_cmds_queue(float scale_x, float scale_y, float center_x, float center_y)
{
  list_block_ref block;
  int i;
  int first;
  int list;
  float kx;
  float ox;
  float ky;
  float oy;
  float t;
  elan_proj_cmd_rec *cmd;

  screen_unproject_consts_set((int)proj_rect_width,(int)proj_rect_height);
  kx = -scr_proj_m00 * scale_x * scr_width_f * 0.5;
  ky = -scr_proj_m11 * scale_y * scr_height_f * 0.5;
  ox = (1.0 - center_x * scale_x) * scr_width_f * 0.5;
  t = center_y * scale_y;
  oy = (1.0 - t) * scr_height_f * 0.5;
  cmd = (elan_proj_cmd_rec *)elan_cmd_write_ptr;
  for (i = 0; i < 2; i++) {
    first = i == 0;
    list = (first != 1) * 2;
    block.addr = (uint)cmd;
    cmd->cmd = 0x08000300;
    cmd->scale_x = kx;
    cmd->offset_x = ox;
    cmd->scale_y = ky;
    cmd->offset_y = oy;
    cmd = cmd + 2;
    block.length = (int)cmd - (int)block.addr;
    elan_list_block_queue(list,&block);
  }
  elan_cmd_write_ptr = (uint *)cmd;
}

void screen_quad_tex_elan_queue(uint list_in, int tex_in, int x_in, int y_in, int width, int height, float z_in, uint color_in, uint offset_color, uint hdr_bits, uint hdr_word)
{
  list_block_ref call;
  list_block_ref b;
  uint list;
  list_block_ref *h;
  int n;
  uint *d;
  tex_slot_rec *slot;
  int tex;
  int x;
  int y;
  float z;
  uint color;
  float kx, ky;
  float x0, x1, y0, y1;
  list = list_in;
  tex = tex_in;
  x = x_in;
  y = y_in;
  z = z_in;
  color = color_in;
  mtx_push(0);
  mtx_load_identity();
  matrix_cmds_queue_on_lists(1 << list);
  mtx_pop(1);
  d = elan_cmd_write_ptr;
  slot = texidx_slot_get(tex);
  kx = scr_unproj_x;
  ky = scr_unproj_y;
  x0 = ((float)x - scr_half_width) * z * kx;
  x += width;
  x1 = ((float)x - scr_half_width) * z * kx;
  y0 = ((float)y - scr_half_height) * z * ky;
  y += height;
  y1 = ((float)y - scr_half_height) * z * ky;
  b.addr = (uint)d;
  d[0] = 0x08000500;
  d[1] = 0x2020;
  d[2] = 0x200;
  d[3] = color;
  d[4] = offset_color;
  d += 8;
  d[0] = 0x88000708;
  d[1] = 0;
  d[2] = slot->size_word;
  d[3] = slot->tex_control_word;
  d[6] = 0x4a;
  d[7] = 4;
  d += 8;
  d[0] = 0;
  ((float *)d)[1] = x0;
  ((float *)d)[2] = y0;
  ((float *)d)[3] = z;
  d += 4;
  ((float *)d)[0] = 0.0;
  ((float *)d)[1] = 0.0;
  d += 2;
  d[0] = color;
  d[1] = offset_color;
  d += 2;
  d[0] = 0;
  ((float *)d)[1] = x1;
  ((float *)d)[2] = y0;
  ((float *)d)[3] = z;
  d += 4;
  ((float *)d)[0] = 1.0;
  ((float *)d)[1] = 0.0;
  d += 2;
  d[0] = color;
  d[1] = offset_color;
  d += 2;
  d[0] = 0x60000000;
  ((float *)d)[1] = x0;
  ((float *)d)[2] = y1;
  ((float *)d)[3] = z;
  d += 4;
  ((float *)d)[0] = 0.0;
  ((float *)d)[1] = 1.0;
  d += 2;
  d[0] = color;
  d[1] = offset_color;
  d += 2;
  d[0] = 0xa0000000;
  ((float *)d)[1] = x1;
  ((float *)d)[2] = y1;
  ((float *)d)[3] = z;
  d += 4;
  ((float *)d)[0] = 1.0;
  ((float *)d)[1] = 1.0;
  d += 2;
  d[0] = color;
  d[1] = offset_color;
  d += 2;
  n = (char *)d - (char *)b.addr;
  b.length = n; /* redundant, but game code: b.length is never read, but the game stores it (mov.l r3,@(16,r14)). */
  d = (uint *)((uint)((char *)d + 0x1f) & 0xffffffe0);
  call.addr = (uint)d;
  d[0] = 0x08000800;
  d[1] = hdr_bits | 0x18000000;
  d[2] = hdr_word;
  d[3] = 0;
  d[4] = b.addr & 0x1fffff8 | 0x80000000;
  d[5] = 1;
  d[6] = n;
  d[7] = 0;
  d += 16;
  h = &call;
  h->length = (char *)d - (char *)call.addr;
  elan_list_block_queue(list, h);
  elan_cmd_write_ptr = d;
}
