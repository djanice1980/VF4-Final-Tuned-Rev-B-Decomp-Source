/*
 * vf4lib/libbase/src/bg_color.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar bg_color_custom_rgb; /* 0x0c2943a8 */
extern uint *bg_color_top; /* 0x0c2943b4 */
extern int bg_color_depth; /* 0x0c2943b8 */
extern ushort rand_int_table[100]; /* 0x0c41f558 */
extern float rand_unit_table[100]; /* 0x0c41f620 */
extern uint bg_color_custom[3]; /* 0x0c29439c */

void bg_color_set(uint *colors)
{
  bg_color_top[0] = colors[0];
  bg_color_top[1] = colors[1];
  bg_color_top[2] = colors[2];
  bg_plane_corner_colors(colors[0],colors[1],colors[2]);
  return;
}

void bg_color_push(uint *colors)
{
  int n;
  n = bg_color_depth;
  n++;
  if (n <= 0xf) {
    bg_color_top += 3;
    bg_color_depth = n;
  }
  bg_color_set(colors);
}

void bg_color_pop(void)
{
  int n = bg_color_depth;
  if (--n >= 0) {
    bg_color_top = bg_color_top - 3;
    bg_color_depth = n;
  }
  bg_color_set(bg_color_top);
}

void rand_tables_fill(uint seed)
{
  rand_int_table_fill(seed);
  rand_unit_table_fill(seed);
  return;
}

void rand_int_table_fill(uint seed)
{
  int i;

  lcg_seed_set(seed);
  i = 0;
  do {
    rand_int_table[i] = lcg_next15();
    i = i + 1;
  } while (i < 100);
  return;
}

void rand_unit_table_fill(uint seed)
{
  int i;
  lcg_seed_set(seed);
  for (i = 0; i < 100; i++) {
    float *p = &rand_unit_table[i];
    *p = lcg_next_unit();
  }
}

int rand_int_table_next(void)
{
  rand_int_table_pos++;
  if (rand_int_table_pos >= 100) {
    rand_int_table_pos = 0;
  }
  return (short)rand_int_table[rand_int_table_pos];
}

float rand_unit_table_next(void)
{
  rand_unit_table_pos++;
  if (rand_unit_table_pos >= 100) {
    rand_unit_table_pos = 0;
  }
  return rand_unit_table[rand_unit_table_pos];
}

float text_center_left_x(char *text, int char_width, float width)
{
  size_t len;

  len = strlen(text);
  return (width - (float)((len - 1) * char_width)) * 0.5;
}

void bg_color_custom_apply(void)
{
  bg_color_custom[0] =
       ((uint)bg_color_custom_rgb << 0x10 | (uint)bg_color_custom_green0 << 8 | (uint)bg_color_custom_blue0 |
       0xff000000);
  bg_color_custom[1] =
       ((uint)bg_color_custom_red1 << 0x10 | (uint)bg_color_custom_green1 << 8 | (uint)bg_color_custom_blue1 |
       0xff000000);
  bg_color_custom[2] =
       ((uint)bg_color_custom_red2 << 0x10 | (uint)bg_color_custom_green2 << 8 | (uint)bg_color_custom_blue2 |
       0xff000000);
  bg_color_set(bg_color_custom);
  return;
}

int date_stamp_now(void)
{
  tm t;
  int y;
  get_now_tm(&t);
  y = t.tm_year - 50;
  return (((y + y) & 0xff) << 19) + ((t.tm_mon + 1) << 16) +
         (t.tm_mday << 11) + (t.tm_hour << 6) + t.tm_min;
}
