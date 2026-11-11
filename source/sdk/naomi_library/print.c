/*
 * sdk/naomi_library/print.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int text_rotated; /* 0x0c32a5bc */
extern float text_depth; /* 0x0c32a5c0 */
extern uchar *text_glyph_bitmap; /* 0x0c32a5c4 */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_left; /* 0x0c32a704 */
extern float proj_rect_px_top; /* 0x0c32a708 */
extern int text_cell_h; /* 0x0c417ad0 */
extern char text_format_buf[512]; /* 0x0c417ad4 */
extern uint text_flags; /* 0x0c417cd8 */
extern int text_cell_w; /* 0x0c417ce0 */
extern short text_glyph_u_table[257]; /* 0x0c5d49a0 */
extern display_state_rec display_state; /* 0x0c41bb38 */
extern text_sprite_header_rec text_sprite_header; /* 0x0c5d4960 */

void text_glyphs_set_source(void *bitmap)
{
  text_glyph_bitmap = bitmap;
  return;
}

void text_depth_set(float z)
{
  text_depth = 1.0f / z;
  return;
}

void text_sprite_ctrl_bits16_set(int value)
{
  text_sprite_header.control_word &= 0xfffcffff;
  text_sprite_header.control_word |= value << 16;
}

void text_overlay_setup(void)
{
  int i;
  union { float f; uint u; } v;

  text_cursor_x = 0;
  text_cursor_y = 0;
  text_margin_x = 0;
  text_init_word = 0xffffffff;
  text_init_short = 8;
  text_cell_w = 8;
  text_cell_h = 8;
  text_flags = 0;
  text_init_float_a = 0.0;
  text_init_float_b = 1.0;
  text_glyph_step = 0.0078125;
  for (i = 0; i < 0x101; i++) {
    v.f = (float)i * 0.0078125;
    text_glyph_u_table[i] = v.u >> 16;
  }
  surface_set_up_and_place(&text_font_surface,8,0x400,0x902);
  surface_data_to_vram(&text_font_surface,text_glyph_bitmap,0,0);
  text_sprite_header_fill();
  return;
}

void text_rotated_set(int rotated)
{
  text_rotated = rotated;
  return;
}

void text_list_choice_set(uchar list)
{
  text_sprite_header_rec *header;

  header = &text_sprite_header;
  header->list_choice = list;
  header->control_word &= 0xf8ffffff;
  header->tsp_word &= 0x03ffffff;
  if (list != 0) {
    header->tsp_word |= 0x20000000;
  }
  else {
    header->control_word |= 0x02000000;
    header->tsp_word |= 0x94000000;
  }
}

void text_sprite_tex_word(void)
{
  text_sprite_header.tex_word =
       text_font_surface.format_flags << 0x1f | (text_font_surface.format_flags & 8) << 0x1b |
       text_font_surface.pixel_format & 0x38000000 |
       (~text_font_surface.format_flags & 4) << 0x18 |
       (text_font_surface.format_flags & 0x10) << 0x15 | text_font_surface.tex_address >> 3;
  return;
}

void text_sprite_header_fill(void)
{
  text_sprite_header.control_word = 0xa2000009;
  text_sprite_header.isp_word = 0x80000000;
  text_sprite_header.tsp_word = 0x949004c7;
  text_sprite_tex_word();
  text_sprite_header.base_color = 0xffffffff;
  text_sprite_header.list_choice = 0;
  return;
}

uint * text_glyph_quad_emit(int x, int y, int ch, uint *dst)
{
  float z;
  float x1;
  float y1;
  int s0;
  int s1;
  int c;
  int v;
  uint *p = dst;
  z = text_depth;
  c = (char)ch;
  if (text_rotated == 0) {
    x1 = (float)x + (float)text_cell_w;
    y1 = (float)y + (float)text_cell_h;
    {
      short *q = &text_glyph_u_table[c];
      s0 = q[0];
      s1 = q[1];
    }
    p[0] = 0xf0000000;
    ((float *)p)[1] = (float)x;
    ((float *)p)[2] = (float)y;
    ((float *)p)[3] = z;
    ((float *)p)[4] = x1;
    ((float *)p)[5] = (float)y;
    ((float *)p)[6] = z;
    ((float *)p)[7] = x1;
    __asm__ __volatile__ ("pref @%0" : : "r" (p));
    p += 8;
    ((float *)p)[0] = y1;
    ((float *)p)[1] = z;
    ((float *)p)[2] = (float)x;
    ((float *)p)[3] = y1;
    p[4] = c << 24 | c << 16 | c << 8 | c;
    p[5] = s0;
    s0 |= 0x3f800000;
    p[6] = s0;
    s1 |= 0x3f800000;
    p[7] = s1;
  }
  else {
    v = x;
    x = (-y - text_cell_h) + display_state.screen_width;
    y = v;
    x1 = (float)x + (float)text_cell_h;
    y1 = (float)y + (float)text_cell_w;
    {
      short *q = &text_glyph_u_table[c];
      s0 = q[0];
      s1 = q[1];
    }
    p[0] = 0xf0000000;
    ((float *)p)[1] = (float)x;
    ((float *)p)[2] = (float)y;
    ((float *)p)[3] = z;
    ((float *)p)[4] = x1;
    ((float *)p)[5] = (float)y;
    ((float *)p)[6] = z;
    ((float *)p)[7] = x1;
    __asm__ __volatile__ ("pref @%0" : : "r" (p));
    p += 8;
    ((float *)p)[0] = y1;
    ((float *)p)[1] = z;
    ((float *)p)[2] = (float)x;
    ((float *)p)[3] = y1;
    p[4] = c << 24 | c << 16 | c << 8 | c;
    p[5] = s1;
    p[6] = s0;
    s0 |= 0x3f800000;
    p[7] = s0;
  }
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p += 8;
  return p;
}

void text_newline_keeps_margin(void)
{
  text_flags = text_flags | 1;
  return;
}

void text_flag_bit0_clear(void)
{
  text_flags &= ~1;
  return;
}

void text_cell_size_set(int width, int height)
{
  text_cell_w = width;
  text_cell_h = height;
  return;
}

void text_draw_string(char *text)
{
  uint list_ptr;
  uint *qacr;
  uint *dst;
  int x;
  int y;
  uint keep_margin;
  int list;

  list = (text_sprite_header.list_choice == 0) * 2;
  list_ptr = list_write_ptrs[list];
  x = text_cursor_x;
  y = text_cursor_y;
  keep_margin = text_flags & 1;
  qacr = (uint *)0xff000038;
  qacr[0] = list_ptr >> 0x18;
  qacr[1] = list_ptr >> 0x18;
  dst = (uint *)(list_ptr & 0x3ffffff | 0xe0000000);
  *dst = text_sprite_header.control_word;
  dst[1] = text_sprite_header.isp_word;
  dst[2] = text_sprite_header.tsp_word;
  dst[3] = text_sprite_header.tex_word;
  dst[4] = text_sprite_header.base_color;
  __asm__ volatile ("pref @%0" : : "r" (dst));
  dst = dst + 8;
  while (*text != '\0') {
    if (*text == ' ') {
      x = x + text_cell_w;
      text++;
    }
    else if (*text == '\n') {
      x = 0;
      if (keep_margin != 0) {
        x = text_margin_x;
      }
      y = y + text_cell_h;
      text++;
    }
    else if (*text == '\r') {
      x = 0;
      if (keep_margin != 0) {
        x = text_margin_x;
      }
      text++;
    }
    else {
      dst = text_glyph_quad_emit(x,y,*text++,dst);
      x = x + text_cell_w;
    }
  }
  text_cursor_x = x;
  text_cursor_y = y;
  list_write_ptrs[list] = list_write_ptrs[list] & 0xfc000000 | (uint)dst & 0x3ffffff;
}

void text_cursor_to_cell(int column, int row)
{
  int t;
  if (text_rotated == 0) {
    t = (int)((float)(column * text_cell_w) + proj_rect_px_left);
    text_cursor_x = t;
    text_margin_x = t;
    text_cursor_y = (int)((float)(row * text_cell_h) + proj_rect_px_top);
  }
  else {
    t = (int)((float)(column * text_cell_w) + proj_rect_px_top);
    text_cursor_x = t;
    text_margin_x = t;
    text_cursor_y = (int)((float)(row * text_cell_h + (int)display_state.screen_width) -
            (proj_rect_px_left + proj_rect_px_width));
  }
}

void text_color_set(uint color)
{
  text_sprite_header.base_color = color;
  return;
}

uint text_draw_formatted(char *format, ...)
{
  va_list ap;
  __builtin_stdarg_start(ap, format);
  vsnprintf(text_format_buf, 0x200, format, ap);
  text_draw_string(text_format_buf);
  return 0;
}
