/*
 * vf4lib/libmission/src/msnm_text.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern msn_text_image msn_text_images[25]; /* 0x0c1a0488 */
extern uint msn_text_font_wide[10]; /* 0x0c1a0870 */
extern uint msn_text_font_narrow[10]; /* 0x0c1a0898 */
extern uint msn_text_tags[8]; /* 0x0c1a08c0 */
extern int msn_text_cmd_chars[39]; /* 0x0c1a0958 */
extern int msn_text_image_mirror[25]; /* 0x0c1a09f4 */
extern float msn_text_measure_box[11]; /* 0x0c1a0a58 */
extern char msn_text_debug_box; /* 0x0c2a1e7c */
extern char msn_text_debug_text; /* 0x0c2a1e7d */
extern char * msn_text_test_string; /* 0x0c2a1e80 */
extern char * msn_text_state_ptr; /* 0x0c2a1e84 */
extern msn_text_color msn_text_colors[10]; /* 0x0c1a08e0 */

void msn_text_init(void)
{
  msn_text_color_reset();
  msn_text_custom_colors_reset();
  ((msn_text_state_rec *)msn_text_state_ptr)->mirror = 0;
  ((msn_text_state_rec *)msn_text_state_ptr)->alpha = 1.0f;
  msn_text_layout_reset();
  return;
}

void msn_text_box_draw(float *box, char *text)
{
  float alpha;

  if ((1.1920929e-07 <= __builtin_fabsf(box[9])) || (msn_text_debug_box != '\0')) {
    alpha = box[9];
    if (msn_text_debug_box != '\0') {
      alpha = 0.8;
    }
    sprite_colors_set(0,*(uint *)(box + 10));
    sprite_put_xyz_scaled_alpha(0,*box,box[1],box[7],box[2] * 0.125,box[3] * 0.125,alpha);
    sprite_colors_reset();
  }
  if (msn_text_debug_text != '\0') {
    text = msn_text_test_string;
  }
  msn_text_measure_begin();
  msn_text_color_reset();
  msn_text_run(box,text);
  msn_text_align_compute(box);
  msn_text_color_reset();
  msn_text_run(box,text);
  msn_text_layout_reset();
  return;
}

float msn_text_width(char *text)
{
  msn_text_measure_begin();
  msn_text_color_reset();
  msn_text_run(msn_text_measure_box,text);
  return ((msn_text_state_rec *)msn_text_state_ptr)->layout.width_max;
}

void msn_text_mirror_set(int on)
{
  ((msn_text_state_rec *)msn_text_state_ptr)->mirror = on;
  return;
}

void msn_text_alpha_set(float alpha)
{
  ((msn_text_state_rec *)msn_text_state_ptr)->alpha = alpha;
  return;
}

void msn_text_alpha_reset(void)
{
  ((msn_text_state_rec *)msn_text_state_ptr)->alpha = 1.0f;
  return;
}

void msn_text_align_set(uint flags)
{
  ((msn_text_state_rec *)msn_text_state_ptr)->layout.align = flags;
  return;
}

void msn_text_custom_color_set(int index, uint color, uint color2)
{
  index -= 6;
  if ((uint)index < 4) {
    ((msn_text_state_rec *)msn_text_state_ptr)->custom_colors[index] = color;
    ((msn_text_state_rec *)msn_text_state_ptr)->custom_colors2[index] = color2;
  }
}

void msn_text_custom_colors_reset(void)
{
  int i;
  msn_text_state_rec **pp;
  unsigned int color, color2;
  i = 0;
  pp = (msn_text_state_rec **)&msn_text_state_ptr;
  color = msn_text_colors[0].color;
  color2 = msn_text_colors[0].color2;
  for (; i < 4; i++) {
    (*pp)->custom_colors[i] = color;
    (*pp)->custom_colors2[i] = color2;
  }
}

void msn_text_color_reset(void)
{
  ((msn_text_state_rec *)msn_text_state_ptr)->color = 0;
  return;
}

void msn_text_run(float *box, char *text)
{
  typedef struct { float x; float y; } v2;
  char tag[32];
  char ch[3];
  v2 pos;
  v2 next;
  int inside_tag;
  int start;
  int i;
  int n;
  uint single;
  msn_text_state_rec *state;

  inside_tag = 0;
  start = 0;
  i = 0;
  state = (msn_text_state_rec *)msn_text_state_ptr;
  pos.x = box[0] + box[4] + state->layout.offset_x;
  pos.y = box[1] + box[5] + state->layout.offset_y;
  for (; text[i] != 0; i++) {
    if (msn_text_is_single_byte(text[i])) {
      single = 1;
      ch[0] = text[i];
      ch[1] = 0;
      ch[2] = 0;
    } else {
      single = 0;
      ch[0] = text[i];
      ch[1] = text[i + 1];
      ch[2] = 0;
    }
    if (ch[0] == '<') {
      inside_tag = 1;
      start = i + 1;
    }
    if (inside_tag && ch[0] == '>') {
      n = i - start;
      if (n > 0) {
        strncpy(tag, text + start, n);
        tag[n] = 0;
        msn_text_tag_run(tag, box, &pos.x, &next.x);
        pos = next;
      }
      inside_tag = 0;
    } else if (!inside_tag) {
      msn_text_glyph((byte *)ch, single, box, &pos.x, &next.x);
      if (!single) i++;
      pos = next;
    }
  }
}

void msn_text_glyph(byte *ch, uint single_byte, float *box, float *pos, float *next)
{
  uchar narrow = single_byte;
  int r;
  int x;
  int y;
  msn_text_image *font;
  float shift;
  float scale_x;

  font = (msn_text_image *)msn_text_font_wide;
  if (narrow != 0) {
    font = (msn_text_image *)msn_text_font_narrow;
  }
  scale_x = 1.0;
  shift = 0.0;
  r = memcmp(ch,msn_text_arrow_char,3);
  if (r == 0) {
    scale_x = 0.5;
    shift = font->w * 0.25;
  }
  msn_text_advance(font,box,pos,next);
  r = msn_text_measuring();
  if (r == 0) {
    msn_text_color_apply(((msn_text_state_rec *)msn_text_state_ptr)->color);
    sprite_alpha_set(((msn_text_state_rec *)msn_text_state_ptr)->alpha);
    sprite_scale_set(scale_x,1.0);
    x = (int)((float)(int)*pos + shift);
    y = (int)pos[1];
    sprite_text_put(&euc_text_font,(float)x,(float)y,box[8],1,ch);
    sprite_scale_reset();
    sprite_alpha_reset();
    sprite_colors_reset();
  }
  return;
}

void msn_text_image_draw(int image, float *pos)
{
  msn_text_image *img;
  int x;
  int y;

  if (((msn_text_state_rec *)msn_text_state_ptr)->mirror != '\0') {
    image = msn_text_image_mirror[image];
  }
  img = &msn_text_images[image];
  if (msn_text_measuring() == 0) {
    x = (int)(pos[0] + img->x);
    y = (int)(pos[1] + img->y);
    sprite_colors_set(img->color,img->color2);
    sprite_alpha_set(((msn_text_state_rec *)msn_text_state_ptr)->alpha);
    sprite_put_xyz_scaled_alpha_angle_flags(img->sprite,(float)x,(float)y,0.007,1.0,1.0,1.0,(int)img->angle,img->flags);
    sprite_alpha_reset();
    sprite_colors_reset();
  }
}

bool msn_text_is_single_byte(int c)
{
  return 0x5d < (byte)((char)c + 0x5fU);
}

bool msn_text_advance(msn_text_image *item, float *box, float *pos, float *next)
{
  float x;
  float right;
  float t;

  x = *pos + item->w;
  *next = x;
  next[1] = pos[1];
  t = box[2] + *box;
  right = t - box[4];
  if (x > right) {
    msn_text_newline(box,pos,right);
    *next = *pos + item->w;
    next[1] = pos[1];
    return 1;
  }
  msn_text_width_note(*next);
  return 0;
}

void msn_text_color_apply(int index)
{
  unsigned int color;
  unsigned int color2;

  color = msn_text_colors[index].color;
  color2 = msn_text_colors[index].color2;
  if (5 < index) {
    int i = index - 6; msn_text_state_rec *state = (msn_text_state_rec *)msn_text_state_ptr; color = state->custom_colors[i]; color2 = state->custom_colors2[i];
  }
  sprite_colors_set(color,color2);
}

void msn_text_tag_run(char *tag, float *box, float *pos, float *next)
{
  typedef struct { char *name; char whole; char pad[3]; } tag_ent;
  typedef struct { int w[2]; } xy;
  tag_ent *t;
  int i;
  size_t name_len;
  size_t arg_len;
  char arg [128];

  for (i = 0; i < 4; i++) {
    t = &((tag_ent *)msn_text_tags)[i];
    if (t->whole != 0) {
      if (strcmp(tag,t->name) == 0) break;
    }
    else {
      name_len = strlen(t->name);
      if (strncmp(t->name,tag,name_len) == 0) {
        arg_len = strlen(tag + name_len + 1);
        strcpy(arg,tag + name_len + 1);
        arg[arg_len] = 0;
        break;
      }
    }
  }
  *(xy *)next = *(xy *)pos;
  if (i == 4) return;
  switch(i) {
  case 0:
    msn_text_color_by_name(arg);
    break;
  case 1:
    msn_text_newline(box,next,*pos);
    break;
  case 2:
    msn_text_image_tag(arg,box,pos,next);
    break;
  case 3:
    msn_text_command_tag(arg,box,pos,next);
    break;
  }
  return;
}

void msn_text_newline(float *box, float *pos, float x_reached)
{
  msn_text_state_rec *state;
  msn_text_width_note(x_reached);
  msn_text_line_count_add();
  state = (msn_text_state_rec *)msn_text_state_ptr;
  *pos = *box + box[4] + state->layout.offset_x;
  pos[1] = pos[1] + box[6];
  return;
}

void msn_text_color_by_name(char *name)
{
  int i;
  msn_text_color *c;

  for (i = 0; i < 10; i++) {
    c = &msn_text_colors[i];
    if (strcmp(name,c->name) == 0) {
      ((msn_text_state_rec *)msn_text_state_ptr)->color = i;
      return;
    }
  }
}

void msn_text_image_tag(char *name, float *box, float *pos, float *next)
{
  int i;
  msn_text_image *img;

  for (i = 0; i < 0x19; i++) {
    img = &msn_text_images[i];
    if (strcmp(name,img->name) == 0) {
      msn_text_advance(img,box,pos,next);
      msn_text_image_draw(i,pos);
      return;
    }
  }
  return;
}

void msn_text_command_tag(char *cmd, float *box, float *pos, float *next)
{
  typedef struct { char ch; char pad[3]; int image; int image_alt; } cmd_ent;
  typedef struct { uint w[2]; } xy;
  int i;
  int j;
  int image;
  char alt;
  char c;
  cmd_ent *e;

  i = 0;
  alt = 0;
  for (; cmd[i] != '\0'; i++) {
    c = cmd[i];
    for (j = 0; j < 0xd; j++) {
      e = (cmd_ent *)&msn_text_cmd_chars + j;
      if (e->ch == c) {
        if (j == 0xb) {
          alt = 1;
        }
        else if (j == 0xc) {
          msn_text_glyph(plus_sign_text,1,box,pos,next);
          *(xy *)pos = *(xy *)next;
          alt = 0;
        }
        else {
          image = e->image;
          if (alt) image = e->image_alt;
          if (image != -1) {
            msn_text_advance(&msn_text_images[image],box,pos,next);
            msn_text_image_draw(image,pos);
            *(xy *)pos = *(xy *)next;
          }
          alt = 0;
        }
        break;
      }
    }
  }
}

void msn_text_layout_reset(void)
{
  msn_text_state_rec *state;

  state = (msn_text_state_rec *)msn_text_state_ptr;
  state->layout.align = 0;
  state->layout.measuring = 0;
  state->layout.line_count = 0.0f;
  state->layout.width_max = 0.0f;
  state->layout.offset_x = 0.0f;
  state->layout.offset_y = 0.0f;
}

void msn_text_measure_begin(void)
{
  msn_text_state_rec *state;

  state = (msn_text_state_rec *)msn_text_state_ptr;
  state->layout.measuring = 1;
  state->layout.line_count = 0.0;
  state->layout.width_max = 0.0;
}

void msn_text_align_compute(float *box)
{
  msn_text_state_rec *state;
  float t;
  float x;
  float *line_height;
  msn_text_layout_rec *layout;

  state = (msn_text_state_rec *)msn_text_state_ptr;
  layout = &state->layout;
  if ((state->layout.align & 1) != 0) {
    t = box[4];
    x = (*box + box[2]) - state->layout.width_max;
    state->layout.offset_x = (t + x) * 0.5 - t;
    if (state->layout.offset_x < 0.0) {
      state->layout.offset_x = 0.0;
    }
  }
  if ((layout->align & 2) != 0) {
    line_height = &box[6];
    x = (layout->line_count + 1.0) * *line_height;
    layout->offset_y = (box[3] - x) * 0.5 - box[5];
    if (layout->offset_y < 0.0) {
      layout->offset_x = 0.0;
    }
  }
  layout->measuring = 0;
  return;
}

uchar msn_text_measuring(void)
{
  return ((msn_text_state_rec *)msn_text_state_ptr)->layout.measuring;
}

void msn_text_width_note(float x)
{
  msn_text_state_rec *state = (msn_text_state_rec *)msn_text_state_ptr;
  if (state->layout.width_max < x) {
    state->layout.width_max = x;
  }
  return;
}

void msn_text_line_count_add(void)
{
  msn_text_state_rec *state = (msn_text_state_rec *)msn_text_state_ptr;
  state->layout.line_count = state->layout.line_count + 1.0;
  return;
}
