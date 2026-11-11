/*
 * vf4lib/libbase/src/name_entry.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sprite_put_xyz_scaled_alpha_angle_flags
#define KRPROTO_sprite_put_flags_xyz
#define KRPROTO_sprite_font_text_put
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int mode_timer; /* 0x0c292550 */
extern float mode_timer_float; /* 0x0c292578 */
extern char champion_side; /* 0x0c2925d4 */
extern undefined4 course_time_total; /* 0x0c2925f8 */
extern float name_entry_glow; /* 0x0c41f464 */
extern short name_entry_anim_timer; /* 0x0c41f468 */
extern char name_entry_cursor; /* 0x0c41f46a */
extern char name_entry_cursor_prev; /* 0x0c41f46b */
extern char name_entry_state; /* 0x0c41f46c */
extern char name_entry_count; /* 0x0c41f46d */
extern short name_entry_frames; /* 0x0c41f478 */
extern float name_entry_list_y; /* 0x0c41f47c */

static inline char **name_table_0c167f8c()
{
  return name_entry_banned;
}

void name_entry_setup(void)
{
  int i;
  char *buf;
  card_body_rec *body;
  
  bg_plane_corner_colors(0x303030,0,0xd0d0d0);
  name_entry_cursor = '\0';
  name_entry_cursor_prev = '\0';
  name_entry_anim_flags = 0;
  name_entry_count = '\0';
  name_entry_frames = 0;
  { char *p; for (i = 0, p = name_entry_name; i < 4; i++) p[i] = 0; }
  for (i = 0; i < 3; i++) name_entry_picks[i] = 0xff;
  body = &card_image_get(champion_side)->body;
  buf = name_entry_card_name;
  card_body_name_get(body,buf,0x15);
  if (*buf != '\0') {
    mode_timer_float = 1200.0;
    mode_timer = 0x4b0;
    if (msnm_active() == 0) {
      time_ranking_insert((int)ranking_chara,course_time_total,buf);
    }
    else {
      msnm_ranking_name_set(0);
    }
    name_entry_state = '\x03';
  }
  else {
    name_entry_state = '\x01';
  }
  name_entry_list_y = 96.0;
}

void name_entry_banned_reset(char *name, char *picks)
{
  char i;
  int hits;
  char *e;
  hits = 0;
  for (i = 0; 0 != name_entry_banned[i]; i++)
  {
    if (name[0] == name_table_0c167f8c()[i][0])
    {
      hits |= 1;
    }
    if (name[1] == name_entry_banned[i][1])
    {
      hits |= 2;
    }
    if (e, name[2] == name_table_0c167f8c()[i][2])
    {
      hits |= 4;
    }
    if (hits == 7)
    {
      name[0] = 'A';
      name[1] = 'A';
      name[2] = 'A';
      picks[0] = 0;
      picks[1] = 0;
      picks[2] = 0;
      break;
    }
    hits = 0;
  }

  if ((name[0] == 'X') && (name[1] == 'X'))
  {
    name[0] = 'A';
    name[1] = 'A';
    name[2] = 'A';
    picks[0] = 0;
    picks[1] = 0;
    picks[2] = 0;
  }
}

void name_entry_cursor_move(char *cursor, int dir, int count, int columns)
{
  char k = dir;
  char n = count;
  char d = columns;
  switch (k) {
  case 3:
    if ((*cursor + 1) % d == 0) {
      *cursor = *cursor - d + 1;
    } else {
      if (++*cursor < n) break;
      *cursor = d * (n / d);
    }
    break;
  case 2:
    if (*cursor % d == 0) {
      *cursor += d - 1;
      if (*cursor < n) break;
      *cursor = n - 1;
    } else {
      if (--*cursor >= 0) break;
      *cursor = d - 1;
    }
    break;
  case 0:
    if (name_entry_letters[(int)*cursor].code == 'G') *cursor = 24;
    else if (name_entry_letters[(int)*cursor].code == 'H') *cursor = 33;
    else if (name_entry_letters[(int)*cursor].code == 'I') *cursor = 34;
    else if (*cursor == 33) *cursor = 25;
    else if (*cursor == 34) *cursor = 26;
    else {
      *cursor -= d;
      if (*cursor < 0) {
        *cursor += d * (n / d) + d;
        if (*cursor >= n) *cursor -= d;
      }
    }
    break;
  case 1:
    if (name_entry_letters[(int)*cursor].code == 'Y') *cursor = 6;
    else if (name_entry_letters[(int)*cursor].code == 'Z') *cursor = 33;
    else if (name_entry_letters[(int)*cursor].code == '&') *cursor = 34;
    else if (*cursor == 33) *cursor = 7;
    else if (*cursor == 34) *cursor = 8;
    else {
      if ((*cursor += d) >= n) *cursor %= d;
    }
    break;
  }
  if (*cursor < 0) *cursor = 0;
  if (*cursor >= n) *cursor = n - 1;
}

void name_entry_input(void)
{
  int done;
  uint move_bits;
  player_input_rec *in;
  int i;
  
  in = player_input_get(champion_side);
  if ((uchar)(name_entry_state - 1) > 1) return;
  if (name_entry_frames < 30) return;
  name_entry_cursor_prev = name_entry_cursor;
  done = 0;
  move_bits = 0;
  if (name_entry_anim_timer < 32) {
    if (name_entry_state == 1) {
      if (((in->pressed | in->repeated) & 0x10) != 0) {
        name_entry_cursor_move(&name_entry_cursor,3,0x23,9);
        move_bits |= 8;
      }
      else if (((in->pressed | in->repeated) & 0x20) != 0) {
        name_entry_cursor_move(&name_entry_cursor,2,0x23,9);
        move_bits |= 4;
      }
      else if (((in->pressed | in->repeated) & 0x80) != 0) {
        name_entry_cursor_move(&name_entry_cursor,0,0x23,9);
        move_bits |= 1;
      }
      else if (((in->pressed | in->repeated) & 0x40) != 0) {
        name_entry_cursor_move(&name_entry_cursor,1,0x23,9);
        move_bits |= 2;
      }
    }
    else if (name_entry_state == 2) {
      if (((in->pressed | in->repeated) & 0x10) != 0) {
        if (name_entry_cursor == '!') name_entry_cursor = '\"';
        else if (name_entry_cursor == '\"') name_entry_cursor = '!';
        move_bits |= 8;
      }
      else if (((in->pressed | in->repeated) & 0x20) != 0) {
        if (name_entry_cursor == '!') name_entry_cursor = '\"';
        else if (name_entry_cursor == '\"') name_entry_cursor = '!';
        move_bits |= 4;
      }
    }
  }
  if ((move_bits & 0xf) != 0) {
    sound_flag_noop(0);
    sound_play_id(0x3af,0);
  }
  if (name_entry_cursor != name_entry_cursor_prev) {
    name_entry_anim_flags = name_entry_anim_flags & 0xfffffff0 | move_bits;
    name_entry_anim_timer = 0x26;
    name_entry_glow = 0.7;
  }
  if ((in->pressed & 0xf) != 0) {
    if (name_entry_cursor == '\"') {
      done = 1;
    }
    else if (name_entry_cursor == '!') {
      if (name_entry_state == 2) {
        name_entry_state = 1;
        name_entry_count--;
        name_entry_picks[name_entry_count] = 0xff;
        name_entry_name[name_entry_count] = 0;
      }
      else if (name_entry_count > 0) {
        name_entry_count--;
        name_entry_picks[name_entry_count] = 0xff;
        name_entry_name[name_entry_count] = 0;
      }
    }
    else if (name_entry_count < 3) {
      name_entry_picks[name_entry_count] = name_entry_cursor;
      name_entry_name[name_entry_count] = name_entry_letters[name_entry_picks[name_entry_count]].code;
      name_entry_count++;
      if (name_entry_count == 3) {
        name_entry_state = 2;
        name_entry_cursor = '\"';
      }
    }
    sound_flag_noop(0);
    sound_play_id(0x3ae,0);
  }
  if (mode_timer == 0) done = 1;
  if (done) {
    name_entry_state = 4;
    if (name_entry_count == 0) {
      for (i = 0; i < 3; i = (char)(i + 1)) {
        name_entry_picks[i] = 0x1b;
        name_entry_name[i] = 0x2d;
      }
    }
    name_entry_banned_reset(name_entry_name,name_entry_picks);
    mode_timer_float = 180.0;
    mode_timer = 0xb4;
    if (msnm_active() == 0) {
      time_ranking_insert(ranking_chara,course_time_total,name_entry_name);
    }
    else {
      msnm_ranking_name_set(name_entry_name);
    }
  }
}

void name_entry_cursor_draw(int side, int item)
{
  int side_no;
  int i;
  short turn;
  float frame;
  float scale;
  float alpha;
  float x, y, dx, dy, base_x, base_y;

  side_no = (char)side;
  base_x = (name_entry_letters[(char)item].x - 12.0) + 40.0;
  base_y = (name_entry_letters[(char)item].y - 18.0) + 40.0;
  if ((((player_input_get(champion_side)->pressed & 0xf) != 0) || (name_entry_state == 4)) &&
      (name_entry_anim_timer < 1)) {
    name_entry_anim_timer = 0xc;
  }
  if (name_entry_anim_timer < 1) {
    x = base_x;
    y = base_y;
    if ((unsigned char)(name_entry_state - 1) < 2) {
      if (pause_frame_held() == 0) {
        if ((name_entry_anim_flags & 0x10) != 0) {
          name_entry_glow = name_entry_glow + 0.012;
          if (name_entry_glow >= 0.7) {
            name_entry_anim_flags = name_entry_anim_flags & ~0x10;
            name_entry_glow = 0.7;
          }
        }
        else {
          name_entry_glow = name_entry_glow - 0.012;
          if (name_entry_glow <= 0.2) {
            name_entry_anim_flags = name_entry_anim_flags | 0x10;
            name_entry_glow = 0.2;
          }
        }
      }
      alpha = name_entry_glow;
    }
    else {
      alpha = 0.7;
    }
    sprite_colors_set(name_entry_side_colors[side_no],0);
    sprite_put_xyz_alpha_flags(0x2bc1,x,y,0.007,alpha,0x100a);
    sprite_colors_reset();
    sprite_put_xyz(name_entry_side_sprites[side_no],x + name_entry_side_sprite_dx[side_no],y + -8.0,0.006);
    return;
  }
  dx = 0.0;
  dy = 0.0;
  turn = 1;
  if ((name_entry_anim_flags & 4) != 0) {
    dx = -64.0;
    turn = -1;
  }
  else if ((name_entry_anim_flags & 8) != 0) {
    dx = 64.0;
  }
  else if ((name_entry_anim_flags & 1) != 0) {
    dy = -60.0;
    turn = -turn;
  }
  else if ((name_entry_anim_flags & 2) != 0) {
    dy = 60.0;
  }
  frame = name_entry_anim_timer - 26;
  if (frame > 0.0) {
    for (i = 0; i < 6; i++) {
      short angle;
      x = base_x - (dx * frame) / 12.0;
      y = base_y - (dy * frame) / 12.0;
      scale = (__builtin_fabsf(frame - 6.0) * 0.5) / 6.0 + 0.5;
      angle = (float)((turn << 15) / 12) * frame;
      alpha = 0.5 - (float)i * 0.05;
      sprite_colors_set(name_entry_side_colors[side_no],0);
      sprite_put_xyz_scaled_alpha_angle_flags(0x2bc1,x,y,(float)i * 0.0001 + 0.007,scale,scale,angle,alpha,0x100a);
      sprite_colors_reset();
      sprite_put_xyz_alpha(name_entry_side_sprites[side_no],x + name_entry_side_sprite_dx[side_no],y + -8.0,0.006,alpha);
      frame = frame - 1.0;
      if (frame < 0.0) {
        return;
      }
    }
    return;
  }
  frame = name_entry_anim_timer;
  x = base_x;
  y = base_y;
  if (frame >= 6.0) {
    alpha = ((20.0 - (frame - 6.0)) * 0.4) / 20.0 + 0.3;
  }
  else {
    alpha = 0.7;
  }
  sprite_colors_set(name_entry_side_colors[side_no],0);
  sprite_put_xyz_alpha_flags(0x2bc1,x,y,0.007,alpha,0x100a);
  sprite_colors_reset();
  scale = ((26.0 - frame) * 0.5) / 26.0 + 1.0;
  alpha = (1.0 - __builtin_fabsf(13.0 - frame) / 13.0) * 0.5;
  sprite_colors_set(name_entry_side_colors[side_no],0);
  sprite_put_xyz_scaled_alpha_flags(0x2bc1,x,y,0.007,scale,scale,alpha,0x100a);
  sprite_colors_reset();
  sprite_put_xyz(name_entry_side_sprites[side_no],x + name_entry_side_sprite_dx[side_no],y + -8.0,0.006);
}

void name_entry_rank_line_draw(void)
{
  char rank;
  char *suffix;
  int time;
  int seconds;
  int hundredths;
  int minutes;
  int tens;
  int place;
  char text[20];
  
  time = course_time_total;
  if (time > 0) {
    minutes = time / 3600;
    seconds = time / 60 - minutes * 60;
    hundredths = ((time % 60) * 100) / 60;
  }
  else {
    minutes = 0;
    seconds = 0;
    hundredths = 0;
  }
  rank = *(char *)&name_entry_rank;
  if (msnm_active() != 0) {
    rank = msnm_rank_get();
  }
  place = rank + 1;
  tens = place / 10;
  if (tens != 1) {
    suffix = rank_suffixes[place - tens * 10];
  }
  else {
    suffix = "TH";
  }
  sprintf(text,"RANKING    %s",suffix);
  sprite_font_text_put(&ascii_font,text,112.0,424.0,0.04,0);
  sprite_colors_set(0xffff00,0);
  sprintf(text,two_digit_format,rank + 1);
  sprite_font_text_put(&ascii_font,text,204.0,424.0,0.04,0);
  sprite_colors_reset();
  if (msnm_active() == 0) {
    sprintf(text,"CLEAR TIME");
    sprite_font_text_put(&ascii_font,text,304.0,424.0,0.04,0);
    sprite_colors_set(0xffff00,0);
    sprintf(text,"%02d\'%02d\"%02d",minutes,seconds,hundredths);
    sprite_font_text_put(&ascii_font,text,432.0,424.0,0.04,0);
    sprite_colors_reset();
  }
  else {
    sprintf(text,"POINT");
    sprite_font_text_put(&ascii_font,text,304.0,424.0,0.04,0);
    sprite_colors_set(0xffff00,0);
    sprintf(text,"%d",msnm_points_get());
    sprite_font_text_put(&ascii_font,text,432.0,424.0,0.04,0);
    sprite_colors_reset();
  }
}

void name_entry_frame(void)
{
  name_entry_letter_rec *letters;
  name_entry_letter_rec *letter;
  float (*pos)[2];
  short secs;
  char tens;
  char ones;
  char i;
  char mode;
  char slot;
  name_entry_letter_rec *list_letters;
  int count;
  int n;
  float y;
  float step;
  short *digits;
  float *px;
  float *py;
  int *pid;
  char *picks;
  
  name_entry_input();
  if ((pause_frame_held() == 0) && (0 < name_entry_anim_timer)) {
    name_entry_anim_timer = name_entry_anim_timer + -1;
  }
  sprite_put_xyz_scaled_alpha(0,0.0,0.0,0.08,80.0,60.0,0.4);
  if (name_entry_state == 3) {
    sprite_offset_set(0.0,20.0,0.0);
  }
  sprite_put_xyz_scaled(0x2c43,0.0,52.0,0.03,58.0,1.0);
  sprite_put_xyz(0x2c44,464.0,12.0,0.03);
  sprite_put_xyz_scaled(0x2c45,528.0,12.0,0.03,14.0,1.0);
  sprite_offset_reset();
  mode = name_entry_state;
  if (((unsigned char)(mode - 1) < 2) | (mode == 4)) {
    sprite_put_xyz_scaled_alpha(0,0.0,0.0,0.06,80.0,8.0,0.5);
    sprite_list_put(name_entry_input_sprites,0.02);
    if (name_entry_state != 4) {
      secs = mode_timer / 60;
      tens = secs / 10;
      if (tens > 9) {
        tens = 9;
      }
      ones = secs % 10;
      digits = name_entry_digit_sprites;
      sprite_put_xy(digits[tens],428.0,24.0);
      sprite_put_xy(digits[ones],448.0,24.0);
    }
    sprite_put_xyz_scaled_alpha(0,0.0,336.0,0.06,80.0,10.0,0.5);
    letters = name_entry_letters;
    for (i = 0; letters[i].box_sprite != -1; i++) {
      letter = &name_entry_letters[i];
      px = &letter->x;
      py = &letter->y;
      pid = &letter->box_sprite;
      sprite_put_flags_xyz(*pid,*px,*py,0.04,0x1000);
      sprite_put_xyz_alpha_flags(letter->label_sprite,*px + 8.0,*py + 8.0,0.05,0.8,0x1000);
      if (name_entry_cursor == i) {
        { void (*set_colors)(uint,uint) = sprite_colors_set; set_colors(0xffffff,0xffffff); }
        sprite_next_blend_set(0);
        sprite_put_xyz_alpha_flags(letter->label_sprite,*px,*py,0.03,name_entry_glow / 1.5,0x1000);
        sprite_next_blend_normal();
        sprite_colors_reset();
      }
    }
    name_entry_cursor_draw(champion_side,(int)name_entry_cursor);
    i = 0;
    list_letters = name_entry_letters;
    pos = name_entry_pick_pos;
    do {
      picks = name_entry_picks;
      slot = picks[i];
      if (slot == -1) break;
      if ((name_entry_state != 4) || ((mode_timer & 2) == 0)) {
        sprite_put_flags_xyz(list_letters[slot].pick_sprite,pos[i][0],pos[i][1],0.04,0x1000);
      }
      i++;
    } while (i < 3);
    if ((unsigned char)(name_entry_state - 1) < 2) {
      sprite_put_xyz_alpha_flags(name_entry_letters[name_entry_cursor].pick_sprite,name_entry_pick_pos[i][0],
                   name_entry_pick_pos[i][1],0.04,name_entry_glow,0x1000);
    }
    name_entry_rank_line_draw();
  }
  else {
    sprite_put_xyz_scaled_alpha(0,0.0,0.0,0.06,80.0,12.0,0.5);
    sprite_put_xyz_scaled_alpha(0,0.0,386.0,0.06,80.0,12.0,0.5);
    if (3 < name_entry_rank) {
      step = ((float)(name_entry_rank + -4) * -36.0 - name_entry_list_y) + 96.0;
      if ((-1.0 < step) && (step < 0.0)) {
        name_entry_list_y = step + name_entry_list_y;
      }
      else if ((-1.0 < step * 0.0625) && (step * 0.0625 < 0.0)) {
        name_entry_list_y = name_entry_list_y - 1.0;
      }
      else {
        name_entry_list_y = float_floor(step * 0.0625) + name_entry_list_y;
      }
    }
    y = float_floor(name_entry_list_y);
    sprite_put_xyz_scaled_alpha(0x2c4b,0.0,385.0,0.04,80.0,1.0,1.0);
    sprite_clip_push(0,3,0x13,0xb,0,2);
    count = 50;
    if (msnm_active() != 0) {
      count = 32;
    }
    for (i = 0; i < count; i++) {
      if ((60.0 <= y) && (y <= 384.0)) {
        if (name_entry_rank == i) {
          sprite_colors_set(0,0xff0000);
          sprite_put_xyz_scaled_alpha(0,0.0,y + 2.0,0.07,80.0,4.5,0.5);
          sprite_colors_reset();
        }
        sprite_put_xyz_scaled_alpha(0x2c4b,0.0,y + 36.0,0.04,80.0,1.0,1.0);
        n = i;
        if (msnm_active() != 0) {
          n = n % 4 - 1;
        }
        if (n != -1) {
          rank_place_ordinal_draw((short)n,48.0,y + 4.0);
        }
        if (msnm_active() != 0) {
          rank_row_score_draw(i,y + 4.0);
        }
        else {
          rank_row_time_draw(i,y + 4.0);
        }
      }
      y = y + 36.0;
    }
    sprite_put_xyz_alpha(0,-8.0,-8.0,0.04,0.0);
    sprite_clip_push(0,0,0x13,0xe,2,0);
    sprite_list_put(name_entry_ranking_sprites,0.02);
    if ((player_input_get(champion_side)->now & 0x8000) != 0) {
      mode_timer_float = 0.0;
      mode_timer = 0;
    }
  }
  name_entry_frames = name_entry_frames + 1;
  return;
}
