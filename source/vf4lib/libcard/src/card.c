/*
 * vf4lib/libcard/src/card.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_card_image_streaks_clear
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern short level_thresholds[21][4]; /* 0x0c188320 */
extern char play_style; /* 0x0c2925cc */
extern char champion_side; /* 0x0c2925d4 */
extern int win_streak; /* 0x0c2925dc */
extern int jisaku_first_match; /* 0x0c2a0200 */
extern int jisaku_flag; /* 0x0c2a0204 */
extern int jisaku_slot; /* 0x0c2a0208 */
extern int jisaku_idle_frames[2]; /* 0x0c2a020c */
extern int jisaku_lever_frames[2]; /* 0x0c2a0214 */
extern int jisaku_guard_frames[2]; /* 0x0c2a021c */
extern int card_close_phase; /* 0x0c2a0224 */
extern int card_user_count; /* 0x0c37e3b0 */
extern uint card_flags[2]; /* 0x0c37e40c */
extern char card_dbg_renpai[2]; /* 0x0c37e414 */
extern char card_dbg_rensho[2]; /* 0x0c37e418 */
extern int card_dbg_secret; /* 0x0c37e4a4 */
extern char card_dbg_point[2]; /* 0x0c37e4a8 */
extern uchar level_change_flags[2]; /* 0x0c2a01fc */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern card_service_rec card_service_state; /* 0x0c431390 */
extern int jisaku_marks[2][3]; /* 0x0c431398 */

static inline byte lvl_of(uint side)
{
  return card_images[side].header.level;
}

byte card_body_rensho_get(card_body_rec *body)
{
  return body->exp >> 4;
}

void card_body_rensho_set(card_body_rec *body, int count)
{
  body->exp = body->exp & 0xf | (short)((count & 0xf) << 4);
}

byte card_body_renpai_get(card_body_rec *body)
{
  return body->exp & 0xf;
}

void card_body_renpai_set(card_body_rec *body, int count)
{
  body->exp = body->exp & 0xf0 | (byte)count & 0xf;
  return;
}

void card_body_name_clear(card_body_rec *body)
{
  memset(body->name,0,0x14);
  return;
}

void card_body_name_set(card_body_rec *body, char *name)
{
  int diff;
  char *dst;

  dst = body->name;
  strncpy(dst,name,0x14);
  diff = memcmp(dst," ",2);
  if (diff == 0) {
    *dst = '\0';
  }
  return;
}

void card_body_name_get(card_body_rec *body, char *buf, uint size)
{
  if (size < 0x15) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/card.c",0x256,"should be buf_size >= 21, but %d",
             size);
  }
  snprintf(buf,size,card_name_format,0x14,body->name);
  return;
}

int card_body_name_compare(card_body_rec *body, char *name)
{
  int diff;

  diff = strncmp(body->name,name,0x14);
  return diff;
}

bool card_body_name_empty(card_body_rec *body)
{
  return body->name[0] == '\0';
}

uint card_body_syu_low5_get(card_body_rec *body)
{
  return body->syu.f.mission_day;
}

void card_body_syu_low5_set(card_body_rec *body, uint value)
{
  body->syu.f.mission_day = value;
  return;
}

uint card_body_syu_b11_get(card_body_rec *body)
{
  return body->syu.f.revise;
}

void card_body_syu_b11_set(card_body_rec *body, uint value)
{
  body->syu.f.revise = value;
  return;
}

uint card_body_prize_misses_get(card_body_rec *body)
{
  return body->syu.f.prize_misses;
}

void card_body_prize_misses_set(card_body_rec *body, uint count)
{
  uint misses;

  misses = count & 0xffff;
  if (0x1e < misses) {
    misses = 0x1e;
  }
  body->syu.f.prize_misses = misses;
  return;
}

void card_body_syu_top3_set(card_body_rec *body, int value)
{
  body->syu.f.user_class = value;
  return;
}

ushort card_body_quest0_b0_get(card_body_rec *body)
{
  return body->quest.b.quest0 & 7;
}

void card_body_quest0_b0_set(card_body_rec *body, uint value)
{
  body->quest.f.state = value;
  return;
}

uint card_body_quest0_b3_get(card_body_rec *body)
{
  return body->quest.f.kind;
}

void card_body_quest0_b3_set(card_body_rec *body, uint value)
{
  body->quest.f.kind = value;
}

uint card_body_quest_limit_day_get(card_body_rec *body)
{
  return body->quest.f.limit_day;
}

void card_body_quest_limit_day_set(card_body_rec *body, uint value)
{
  body->quest.f.limit_day = value;
  return;
}

uint card_body_quest_limit_month_get(card_body_rec *body)
{
  return body->quest.f.limit_month;
}

void card_body_quest_limit_month_set(card_body_rec *body, uint value)
{
  body->quest.f.limit_month = value;
  return;
}

uint card_body_quest_param_a_get(card_body_rec *body)
{
  return body->quest.f.param_a;
}

void card_body_quest_param_a_set(card_body_rec *body, uint value)
{
  body->quest.f.param_a = value;
  return;
}

uint card_body_quest_param_b_get(card_body_rec *body)
{
  return body->quest.f.param_b;
}

void card_body_quest_param_b_set(card_body_rec *body, uint value)
{
  body->quest.f.param_b = value;
  return;
}

uint card_body_quest_param_c_get(card_body_rec *body)
{
  return body->quest.f.param_c;
}

void card_body_quest_param_c_set(card_body_rec *body, uint value)
{
  body->quest.f.param_c = value;
  return;
}

ushort card_body_quest_category_get(card_body_rec *body)
{
  return body->quest.f.category;
}

void card_body_quest_category_set(card_body_rec *body, int value)
{
  body->quest.f.category = value;
  return;
}

uchar card_body_emblem_get(card_body_rec *body)
{
  return body->curse.b.emblem;
}

void card_body_emblem_set(card_body_rec *body, int emblem)
{
  body->curse.b.emblem = (uchar)emblem;
  return;
}

byte card_body_emblem_dan_get(card_body_rec *body)
{
  return body->syu.b.emblem_dan >> 4;
}

void card_body_emblem_dan_set(card_body_rec *body, uint value)
{
  int s = 4;
  body->syu.b.emblem_dan = body->syu.b.emblem_dan & 0xf | (value & 0xf) << s;
}

byte card_body_quest_limit_year_get(card_body_rec *body)
{
  return body->syu.b.emblem_dan & 0xf;
}

void card_body_quest_limit_year_set(card_body_rec *body, int value)
{
  body->syu.b.emblem_dan = body->syu.b.emblem_dan & 0xf0 | (byte)value & 0xf;
  return;
}

uint card_body_quest_limit_hour_get(card_body_rec *body)
{
  return ((int)body->temp & 0xfcU) >> 2;
}

void card_body_quest_limit_hour_set(card_body_rec *body, int value)
{
  body->temp = body->temp & 3 | (uchar)value << 2;
}

uchar card_body_quest_limit_minute_get(card_body_rec *body)
{
  return temp_band_starts[(int)body->temp & 3];
}

void card_body_quest_limit_minute_set(card_body_rec *body, int value)
{
  byte temp = (byte)value;
  int band;
  band = 0;
  if (temp > 14) {
    if (temp <= 29) band = 1;
    else { band = (temp > 44); band += 2; }
  }
  body->temp = body->temp & 0xfc | (band & 3);
  return;
}

uint card_body_color_1p_part_get(card_body_rec *body, uchar part)
{
  return (body->color_1p.w >> (part * 5)) & 0x1f;
}

uint card_body_color_2p_part_get(card_body_rec *body, uchar part)
{
  return (body->color_2p.w >> (part * 5)) & 0x1f;
}

void card_body_tenpo_pnt_set(card_body_rec *body, int points)
{
  body->tenpo_pnt = (ushort)points;
  return;
}

void card_body_rank_pnt_set(card_body_rec *body, int points)
{
  body->rank_pnt = (ushort)points;
  return;
}

byte card_hdr_char_num_get(card_header_rec *header)
{
  return header->char_num & 0xf;
}

void card_hdr_char_num_set(header, chara) card_header_rec *header; byte chara;
{
  if (chara > 0xf) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/card.c",0x35e,"char_num too large\n");
    chara = 0xd;
  }
  header->char_num = header->char_num & 0xf0 | chara & 0xf;
}

byte card_format2_hdr_char_num_get(card_fmt2_header_rec *header)
{
  return header->char_num & 0xf;
}

void card_hdr_quest_count_set(card_header_rec *header, byte value)
{
  header->char_num = header->char_num & 0xf | value << 4;
}

uint card_hdr_quest_count_get(card_header_rec *header)
{
  return ((int)header->char_num & 0xf0U) >> 4;
}

ushort card_hdr_nickname_get(card_header_rec *header)
{
  return header->nickname;
}

void card_hdr_nickname_set(card_header_rec *header, int nickname)
{
  header->nickname = nickname;
  return;
}

uint card_hdr_ftf_ticket_get(card_header_rec *header)
{
  return header->ftf.f.ticket;
}

void card_hdr_ftf_ticket_set(card_header_rec *header, int ticket)
{
  header->ftf.f.ticket = ticket;
}

bool card_hdr_ftf_skill_test(card_header_rec *header, uint skill)
{
  uchar mask;
  int clear;
  if (skill > 1) return 0;
  mask = 1 << skill;
  clear = (mask & header->ftf.f.skill_sel) == 0;
  return clear == 0;
}

void card_hdr_ftf_skill_set(card_header_rec *header, uchar skill0, uchar skill1)
{
  int bits = 0;
  if (skill0 != 0) bits = 1;
  if (skill1 != 0) bits |= 2;
  header->ftf.f.skill_sel = bits;
}

ushort card_hdr_recover_get(card_header_rec *header)
{
  return header->ftf.f.recover;
}

void card_hdr_recover_set(card_header_rec *header)
{
  header->ftf.f.recover = 1;
  return;
}

void card_hdr_recover_clear(card_header_rec *header)
{
  header->ftf.f.recover = 0;
  return;
}

void card_hdr_card_id_set(card_header_rec *header, uint id)
{
  header->card_id[0] = (char)id;
  header->card_id[1] = (char)(id >> 8);
  header->card_id[2] = (char)(id >> 0x10);
  return;
}

uint card_hdr_card_id_get(card_header_rec *header)
{
  return header->card_id[0] | header->card_id[1] << 8 | header->card_id[2] << 16;
}

uint card_hdr_item_test(card_header_rec *header, unsigned char item)
{
  return (uint)header->item_have[item >> 3] & 1 << (item & 7);
}

void card_hdr_item_put(card_header_rec *header, uint item, int on)
{
  uint n;
  uint idx;
  int bit;

  n = item & 0xff;
  idx = n >> 3;
  bit = 1 << (n & 7);
  if (on != 0) {
    header->item_have[idx] |= bit;
  }
  else {
    header->item_have[idx] &= ~bit;
  }
}

uint attack_point_height_class(int attack_point)
{
  uint height_class;
  
  height_class = 3;
  switch(attack_point) {
  case 1:
  case 7:
    height_class = 0;
    break;
  case 2:
  case 4:
    height_class = 1;
    break;
  case 3:
  case 5:
  case 6:
  case 0xd:
    height_class = 2;
    break;
  case 0:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xe:
    height_class = 3;
    break;
  case 0xf:
  default:
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/card.c",0x406,"bad ENattack_point_no %d\n",
             attack_point);
  }
  return height_class;
}

void attack_height_counts_clear(short *counts)
{
  counts[0] = 0;
  counts[1] = 0;
  counts[2] = 0;
  return;
}

void attack_height_count(short *counts, int attack_point)
{
  ushort *p;
  ushort *q = (ushort *)counts;

  switch (attack_point_height_class(attack_point)) {
  case 0:
    if (*q != 0xffff) *q = *q + 1;
    break;
  case 1:
    p = q + 1;
    if (*p != 0xffff) *p = *p + 1;
    break;
  case 2:
    p = q + 2;
    if (*p != 0xffff) *p = *p + 1;
    break;
  }
  return;
}

uint card_sides_mask(void)
{
  uint mask = 0;
  card_record_rec *rec[2];
  int users;

  rec[0] = card_record_get(0);
  rec[1] = card_record_get(1);

  users = card_user_count_get();
  if (users >= 2) {
    mask = 3;
  }
  else if (users == 1) {
    if (rec[0]->in_use != 0) mask = 1;
    if (rec[1]->in_use != 0) mask = 2;
    if (rec[0]->in_use == 0 && rec[1]->in_use == 0) {
      int t;
      if ((uchar)champion_side == 0) t = 1; else t = 0;
      mask = t == 0;
      mask = 2 - mask;
    }
  }
  else {
    mask = 0;
  }
  return mask;
}

void card_images_clear_unused(void)
{
  int i;
  int j;

  for (i = 0; i < 2; i++) {
    if (card_records[i].in_use == 0) {
      card_images[i].header.level = 0;
      card_images[i].header.all_win = 0;
      card_images[i].header.all_lose = 0;
      for (j = 0; j < 8; j++) {
        card_images[i].header.item_have[j] = 0;
      }
      for (j = 0; j < 3; j++) {
        card_images[i].header.card_id[j] = 0;
      }
      card_images[i].header.nickname = 0;
      ((struct { char p[26]; uchar ftf_lo; } *)&card_images[i].header)->ftf_lo = 0;
      card_images[i].body.rank_pnt = 0;
      card_images[i].body.tenpo_pnt = 0;
      card_images[i].body.color_1p_packed.h = 0;
      card_images[i].body.color_2p_packed.h = 0;
      card_images[i].body.exp = 0;
      card_images[i].body.point = 0;
      card_images[i].body.color_1p.w = 0;
      card_images[i].body.color_2p.w = 0;
      for (j = 0; j < 4; j++) {
        card_images[i].body.equip_1p[j] = 0;
        card_images[i].body.equip_2p[j] = 0;
      }
      card_images[i].body.curse.b.curse = 0;
      card_images[i].body.curse.b.emblem = 0;
      card_body_name_clear(&card_images[i].body);
      card_images[i].body.other_col = 0;
      card_images[i].body.temp = 0;
      card_images[i].body.my_tenpo_id = 0;
      card_images[i].body.money = 0;
      card_images[i].body.syu.b.emblem_dan = 0;
      card_images[i].body.syu.b.syuutyuu = 0;
      card_images[i].body.quest.b.quest0 = 0;
      card_images[i].body.quest.b.quest1 = 0;
      card_images[i].body.misc.b[0] = 0;
      card_images[i].body.misc.b[1] = 0;
      card_images[i].body.misc.b[2] = 0;
    }
  }
  return;
}

void card_match_result_record(rob_rec *rob_a, rob_rec *rob_b, int a_lost, int b_lost)
{
  byte side_a;
  byte side_b;
  byte level_a;
  byte level_b;
  int cards;
  int unused_side;
  card_image_rec *images;
  struct { int v; } s;
  side_a = rob_a->side;
  side_b = rob_b->side;
  images = card_images;
  level_a = images[side_a].header.level;
  level_b = images[side_b].header.level;
  cards = 0;
  unused_side = 0;
  if (card_records[side_a].in_use != 0) cards = 1;
  else unused_side = 0; /* redundant, but game code: unused_side is already 0 here, but the game has this else arm (its bra and mov #0). */
  if (card_records[side_b].in_use != 0) cards++;
  else unused_side = 1;
  if (cards == 0) return;
  if (cards == 1) {
    if (unused_side == 0) level_a = 0;
    else level_b = 0;
  }
  if ((jisaku_flag & 1) != 0) user_back_data.jisaku_no_touch_matches++;
  if ((jisaku_flag & 2) != 0) user_back_data.jisaku_no_lever_matches++;
  if ((jisaku_flag & 4) != 0) user_back_data.jisaku_no_g_matches++;
  images = card_images;
  if (images[side_a].header.level == images[side_b].header.level && images[side_a].header.level > 0x12) {
    if ((jisaku_flag & 1) != 0) user_back_data.jisaku_no_touch_high_matches++;
    if ((jisaku_flag & 2) != 0) user_back_data.jisaku_no_lever_high_matches++;
    if ((jisaku_flag & 4) != 0) user_back_data.jisaku_no_g_high_matches++;
  }
  rank_points_update();
  if (a_lost == 0) {
    card_image_all_win_inc(side_a);
    card_image_all_lose_inc(side_b);
  }
  else {
    card_image_all_win_inc(side_b);
    card_image_all_lose_inc(side_a);
  }
  if (card_records[side_a].in_use != 0) {
    s.v = a_lost;
    level_result_apply(side_a, s.v, level_a, level_b);
    level_high_odd_fixup(side_a, level_a, level_b);
    if (card_images[side_a].header.level != level_a) card_image_level_colors_apply(rob_a, side_a, level_a);
  }
  if (card_records[side_b].in_use != 0) {
    s.v = b_lost;
    level_result_apply(side_b, s.v, level_b, level_a);
    level_high_odd_fixup(side_b, level_b, level_a);
    if (card_images[side_b].header.level != level_b) card_image_level_colors_apply(rob_b, side_b, level_b);
  }
}

void card_image_level_colors_apply(rob_rec *rob, uint side_in, int old_level)
{
  uchar *tables[2];
  uint side = side_in & 0xff;
  uint level;
  int i, j, diff;
  int force = 0;
  card_image_rec *images;
  uint a, b, c, d;
  ushort *colors;
  uchar *cur;

  tables[0] = level_costume0_colors[rob->chara];
  tables[1] = level_costume1_colors[rob->chara];
  if (lvl_of(side) > 32) level = 32;
  else level = lvl_of(side);
  if (old_level > 29 || lvl_of(side) > 29) force = 1;
  for (i = 0; i < 2; i++) {
    images = card_images;
    diff = 0;
    cur = tables[i];
    colors = (ushort *)(level * 8 + (int)cur);
    a = colors[0];
    b = colors[1];
    c = colors[2];
    d = colors[3];
    if (force == 0) {
      if ((int)level > old_level) {
        for (j = level - 1; j >= old_level; j--) {
          if (*(ushort *)(tables[i] + j * 8) != a) {
            diff = 1;
            break;
          }
          if (*(ushort *)(tables[i] + j * 8 + 2) != b) {
            diff = 1;
            break;
          }
          if (*(ushort *)(tables[i] + j * 8 + 4) != c) {
            diff = 1;
            break;
          }
          if (*(ushort *)(tables[i] + j * 8 + 6) != d) {
            diff = 1;
            break;
          }
        }
      } else if ((int)level < old_level) {
        if (*(ushort *)(tables[i] + old_level * 8) != a) diff = 1;
        if (*(ushort *)(tables[i] + old_level * 8 + 2) != b) diff = 1;
        if (*(ushort *)(tables[i] + old_level * 8 + 4) != c) diff = 1;
        if (*(ushort *)(tables[i] + old_level * 8 + 6) != d) diff = 1;
      }
    }
    if ((diff | force) != 0) {
      card_misc_word *m = &images[side].body.misc;
      if (m->f.net_colors == 0) {
        costume_colors_set(side, i, a, b, c, d);
      }
    }
  }
}

void card_level_colors_apply(int side_in, int old_level)
{
  int side = (short)side_in;
  uint side_index = side & 0xff;
  uchar *tables[2];
  card_image_rec *images;
  uint level;
  int i;
  int diff;
  uint a;
  uint b;
  uint c;
  uint d;
  ushort *colors;
  uchar *cur;
  tables[0] = level_costume0_colors[card_record_char_get(side_index)];
  tables[1] = level_costume1_colors[card_record_char_get(side_index)];
  if (card_records[side].header.level > 0x20) {
    level = 0x20;
  }
  else {
    level = card_records[side].header.level;
  }
  for (i = 0; i < 2; i++) {
    images = card_images;
    diff = 0;
    cur = tables[i];
    colors = (ushort *)(level * 8 + (int)cur);
    a = colors[0];
    b = colors[1];
    c = colors[2];
    d = colors[3];
    if (*(ushort *)(tables[i] + old_level * 8) != a) diff = 1;
    if (*(ushort *)(tables[i] + old_level * 8 + 2) != b) diff = 1;
    if (*(ushort *)(tables[i] + old_level * 8 + 4) != c) diff = 1;
    if (*(ushort *)(tables[i] + old_level * 8 + 6) != d) diff = 1;
    if (diff) {
      card_misc_word *m = &images[side].body.misc;
      if (!m->f.net_colors) {
        card_costume_colors_set(side, i, a, b, c, d);
      }
    }
  }
}

void level_high_odd_fixup(uchar side, uchar level, uchar other_level)
{
  if (level > 0x20 && (level & 1)) {
    card_images[side].header.level = 0;
  }
  else if (other_level > 0x20 && (other_level & 1)) {
    card_images[side].header.level = other_level - 1;
  }
}

void level_result_20_up(uint side_in, int lost, uint level_in, uint other_in)
{
  uchar side = side_in;
  uchar level = level_in;
  uchar other_level = other_in;
  uchar saved_exp;
  char saved_point;
  uchar row;
  uint result;
  char point;
  uchar rensho;
  uchar renpai;
  int pairing;

  saved_exp = card_images[side].body.exp;
  saved_point = card_images[side].body.point;
  if (net_ready_latched() == 0) {
    return;
  }
  if (level == 30) {
    return;
  }
  if (level == 29) {
    if (other_level == 29) {
      return;
    }
    if (other_level != 28) {
      return;
    }
    if (lost == 0) {
      if (card_syu_not_both_2() != 0) {
        return;
      }
      card_image_win_count(side);
      row = level_row_index(29);
      { card_image_rec *e = card_images;
      point = e[side].body.point;
      rensho = card_body_rensho_get(&card_images[side].body);
      renpai = card_body_renpai_get(&card_images[side].body); }
      result = level_threshold_test(row,point,rensho,renpai);
      if ((result & 8) == 0) {
        return;
      }
      if (level_change_flags_none() != 0) {
        card_image_streak_restore(side,saved_exp,saved_point);
        return;
      }
      card_images[side].header.level = 30;
      card_image_streaks_clear(side);
      level_change_flags[side] |= 8;
    }
    else {
      if (card_syu_not_both_2() != 0) {
        return;
      }
      if (level_change_flags_none() != 0) {
        card_image_streak_restore(side,saved_exp,saved_point);
        return;
      }
      card_images[side].header.level = 28;
      card_image_streaks_clear(side);
      level_change_flags[side] |= 0x10;
    }
  }
  else if (level == 28) {
    pairing = level_28_pairing(28,other_level);
    if (pairing == 1) {
      if (lost == 0) {
        if (net_ready_latched() == 0) {
          fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/card.c",0x609,"assertion `%s\' failed.\n","0");
          return;
        }
        if (card_syu_not_both_2() != 0) {
          return;
        }
        if (card_misc1_bits_3_4(side) != 0) {
          return;
        }
        if (level_change_flags_none() != 0) {
          card_image_streak_restore(side,saved_exp,saved_point);
          return;
        }
        card_image_streaks_clear(side);
        card_images[side].header.level = 29;
        level_change_flags[side] |= 8;
        return;
      }
      if (card_syu_not_both_2() != 0) {
        return;
      }
      card_image_loss_count(side);
      row = level_row_index(level);
      { card_image_rec *e = card_images;
      point = e[side].body.point;
      rensho = card_body_rensho_get(&card_images[side].body);
      renpai = card_body_renpai_get(&card_images[side].body); }
      result = level_threshold_test(row,point,rensho,renpai);
      if ((result & 0x10) == 0) {
        return;
      }
      if (level_change_flags_none() != 0) {
        card_image_streak_restore(side,saved_exp,saved_point);
        return;
      }
      card_images[side].header.level = 27;
      card_image_streaks_clear(side);
      level_change_flags[side] |= 0x10;
    }
    else if (pairing == 2) {
      if (lost == 0) {
        card_image_win_count(side);
      }
      else {
        card_image_loss_count(side);
      }
      row = level_row_index(level);
      { card_image_rec *e = card_images;
      point = e[side].body.point;
      rensho = card_body_rensho_get(&card_images[side].body);
      renpai = card_body_renpai_get(&card_images[side].body); }
      result = level_threshold_test(row,point,rensho,renpai);
      if ((result & 8) != 0) {
        if (level_change_flags_none() != 0) {
          card_image_streak_restore(side,saved_exp,saved_point);
          return;
        }
        card_images[side].header.level = 29;
        card_image_streaks_clear(side);
        level_change_flags[side] |= 8;
        return;
      }
      if ((result & 0x10) == 0) {
        return;
      }
      if (level_change_flags_none() != 0) {
        card_image_streak_restore(side,saved_exp,saved_point);
        return;
      }
      card_images[side].header.level = 27;
      card_image_streaks_clear(side);
      level_change_flags[side] |= 0x10;
    }
    else {
      return;
    }
  }
  else {
    if (level > 27) {
      return;
    }
    if (level != other_level) {
      return;
    }
    if (lost == 0) {
      card_image_win_count(side);
    }
    else {
      card_image_loss_count(side);
    }
    row = level_row_index(level);
    { card_image_rec *e = card_images;
    point = e[side].body.point;
    rensho = card_body_rensho_get(&card_images[side].body);
    renpai = card_body_renpai_get(&card_images[side].body); }
    result = level_threshold_test(row,point,rensho,renpai);
    if ((result & 8) != 0) {
      if (level_change_flags_none() != 0) {
        card_image_streak_restore(side,saved_exp,saved_point);
        return;
      }
      card_images[side].header.level = level_row_levels[row + 1];
      card_image_streaks_clear(side);
      level_change_flags[side] |= 8;
    }
    else if ((result & 0x10) != 0) {
      if (level_change_flags_none() != 0) {
        card_image_streak_restore(side,saved_exp,saved_point);
        return;
      }
      card_images[side].header.level = level_row_levels[row - 1];
      card_image_streaks_clear(side);
      level_change_flags[side] |= 0x10;
    }
  }
}

void card_loss_streak_count(int side)
{
  card_point_dec(side);
  card_renpai_inc(side);
  return;
}

void card_streaks_clear(uchar side)
{
  card_body_rensho_set(&card_records[side].body, 0);
  card_body_renpai_set(&card_records[side].body, 0);
  card_records[side].body.point = 0;
}

void card_loss_apply(uint side)
{
  uint other;
  uint level;
  uint other_level;
  uchar row;
  char point;
  uchar rensho;
  uchar renpai;
  int t;
  int u;

  other = 1 - (side & 1);
  if (net_free_play_is_on() == 0) {
    item_set_steal_logged(side,other);
  }
  if (card_records[side].header.level >= 0xee) {
    level = 0;
  }
  else {
    level = card_records[side].header.level;
  }
  if (card_records[other].header.level >= 0xee) {
    other_level = 0;
  }
  else {
    other_level = card_records[other].header.level;
  }
  card_all_lose_inc(side);
  card_rank_pnt_dec((uchar)side);
  card_tenpo_pnt_dec((uchar)side);
  if (level > 9) {
    if (level > 18) {
      if (level == 30) {
        return;
      }
      if (level == 29) {
        if (other_level == 29) {
          return;
        }
        if (other_level != 28) goto done;
        if (card_syu_not_both_2() != 0) {
          return;
        }
        if (net_ready_latched() == 0) {
          return;
        }
        card_records[side].header.level = other_level;
        card_streaks_clear((uchar)side);
        goto done;
      }
      if (level == 28) {
        t = other_level == 29;
        u = t | (other_level == 28);
        if (u == 0) goto done;
        if (t > 0) {
          if (card_syu_not_both_2() != 0) {
            return;
          }
        }
        if (net_ready_latched() == 0) {
          return;
        }
        card_loss_streak_count(side);
        row = level_row_index(level);
        { card_record_rec *e = card_records;
        point = e[side].body.point;
        rensho = card_body_rensho_get(&card_records[side].body);
        renpai = card_body_renpai_get(&card_records[side].body); }
        if ((level_threshold_test(row,point,rensho,renpai) & 0x10) == 0) goto done;
        card_records[side].header.level = 27;
      }
      else {
        if (level > 27) goto done;
        if (level != other_level) {
          return;
        }
        if (net_ready_latched() == 0) {
          return;
        }
        card_loss_streak_count(side);
        row = level_row_index(level);
        { card_record_rec *e = card_records;
        point = e[side].body.point;
        rensho = card_body_rensho_get(&card_records[side].body);
        renpai = card_body_renpai_get(&card_records[side].body); }
        if ((level_threshold_test(row,point,rensho,renpai) & 0x10) == 0) goto done;
        card_records[side].header.level = level_row_levels[row - 1];
        card_streaks_clear((uchar)side);
        goto done;
      }
    }
    else {
      uchar *exp;
      card_record_rec *rec;
      if (level != other_level) goto done;
      card_loss_streak_count(side);
      row = level_row_index(level);
      { card_record_rec *e = card_records;
      rec = &e[side];
      exp = &rec->body.exp;
      point = rec->body.point;
      rensho = card_body_rensho_get(&card_records[side].body);
      renpai = card_body_renpai_get(&card_records[side].body); }
      if ((level_threshold_test(row,point,rensho,renpai) & 0x10) == 0) goto done;
      if (--rec->header.level < 10) {
        *exp = 0;
      }
    }
  }
  else {
    uchar gain;
    uchar total;
    card_record_rec *e;
    if (level > other_level) {
      gain = 2;
    }
    else {
      gain = 5;
    }
    e = &card_records[side];
    gain += e->body.exp;
    total = gain;
    e->body.exp = total % 100;
    e->header.level = (e->header.level * 100 + total) / 100;
    if (e->header.level < 10) goto done;
    e->header.level = 10;
  }
  card_streaks_clear((uchar)side);
done:
  if (card_records[side].header.level < level) {
    card_level_colors_apply((short)side,level);
  }
}

uchar level_28_pairing(uchar level, uchar other_level)
{
  uchar pairing;

  pairing = 0;
  if ((level == 0x1c) & (other_level == 0x1d)) pairing = 1;
  if ((level == 0x1c) & (other_level == 0x1c)) pairing = 2;
  return pairing;
}

void level_result_apply(byte side, int lost, byte level, byte other_level)
{
  void (*handler)(uint,int,uint,uint);

  if (level <= 9) {
    handler = level_result_0_9;
  } else if ((byte)(level - 10) <= 9) {
    handler = level_result_10_19;
  } else {
    handler = level_result_20_up;
  }
  (*handler)(side,lost,level,other_level);
}

void level_result_10_19(uint side_in, int lost, uint level_in, uint other_in)
{
  uchar side = side_in;
  uchar level = level_in;
  uchar other_level = other_in;
  uchar old_level;
  uchar row;
  uint result;
  uchar new_level;
  char point;
  uchar rensho;
  uchar renpai;

  old_level = card_images[side].header.level;
  if (level != other_level) {
    return;
  }
  if (lost == 0) {
    card_image_win_count(side);
  }
  else {
    card_image_loss_count(side);
  }
  row = level_row_index(level);
  { card_image_rec *e = card_images;
  point = e[side].body.point;
  rensho = card_body_rensho_get(&card_images[side].body);
  renpai = card_body_renpai_get(&card_images[side].body); }
  result = level_threshold_test(row,point,rensho,renpai);
  if ((result & 8) != 0) {
    card_images[side].header.level++;
    card_image_streaks_clear(side);
  }
  else if ((result & 0x10) != 0) {
    if (--card_images[side].header.level < 10) {
      card_images[side].body.exp = 0;
    }
    card_image_streaks_clear(side);
  }
  new_level = card_images[side].header.level;
  if (new_level > old_level) {
    level_change_flags[side] |= 8;
  }
  if (new_level < old_level) {
    level_change_flags[side] |= 0x10;
  }
}

int level_promotion_due(uint side_in)
{
  typedef struct { short rensho_up; short point_up; int downs; } ent;
  uchar side;
  int due;
  uint level;
  uchar other_level;
  uchar row;
  ent *entry;
  card_image_rec *images;
  short t;
  int rensho;
  short point;
  
  side = side_in;
  due = 0;
  images = card_images;
  level = images[side].header.level;
  other_level = images[(side & 1) ^ 1].header.level;
  row = level_row_index(level);
  if (card_syu_bits_get(side) != 2) {
    if (level > 0x1d) return 1;
    entry = &((ent *)&level_thresholds)[row];
    if ((level == 0x1c) & (other_level == 0x1d)) {
      if (card_syu_not_both_2() != 0) return 1;
    }
    if (entry->point_up != -1) {
      t = entry->point_up - 1;
      point = card_images[side].body.point;
      if (t <= point) due = 1;
    }
    if (entry->rensho_up != -1) {
      rensho = card_body_rensho_get(&card_images[side].body);
      t = entry->rensho_up - 1;
      if (t <= rensho) due = 1;
    }
  }
  return due;
}

void card_image_streaks_clear(side) uchar side;
{
  card_body_rensho_set(&card_images[side].body, 0);
  card_body_renpai_set(&card_images[side].body, 0);
  card_images[side].body.point = 0;
}

void card_image_loss_count(byte side_in)
{
  uint side;
  side = side_in;
  if ((card_images[side].header.level < 0x13) || (net_ready_latched() != 0)) {
    card_image_point_dec(side);
    card_image_renpai_inc(side);
  }
}

void card_image_win_count(uint side_in)
{
  uchar side;
  uchar level;
  side = side_in;
  level = card_images[side].header.level;
  if (level < 0x13 || level_change_flags_none() == 0 || jisaku_flag_get() == 0) {
    level = card_images[side].header.level;
    if (level < 0x13 || (net_ready_latched() != 0 && level_promotion_due(side) == 0)) {
      if (card_misc1_bits_3_4(side) == 0) {
        card_image_point_inc(side);
        card_image_rensho_inc(side);
      }
    }
  }
}

uint level_row_index(uint level_in)
{
  uchar level = level_in;
  uint row;
  
  row = 0;
  if (level != '\n') {
    if (level == '\v') {
      row = 1;
    }
    else if (level == '\f') {
      row = 2;
    }
    else if (level == '\r') {
      row = 3;
    }
    else if (level == '\x0e') {
      row = 4;
    }
    else if (level == '\x0f') {
      row = 5;
    }
    else if (level == '\x10') {
      row = 6;
    }
    else if (level == '\x11') {
      row = 7;
    }
    else if (level == '\x12') {
      row = 8;
    }
    else if (level == '\x13') {
      row = 9;
    }
    else if (level == '\x14') {
      row = 10;
    }
    else if (level == '\x15') {
      row = 0xb;
    }
    else if (level == '\x16') {
      row = 0xc;
    }
    else if (level == '\x17') {
      row = 0xd;
    }
    else if (level == '\x18') {
      row = 0xe;
    }
    else if (level == '\x19') {
      row = 0xf;
    }
    else if (level == '\x1a') {
      row = 0x10;
    }
    else if (level == '\x1b') {
      row = 0x11;
    }
    else if (level == '\x1c') {
      row = 0x12;
    }
    else if (level == '\x1d') {
      row = 0x13;
    }
    else if (level == '\x1e') {
      row = 0x14;
    }
  }
  return row;
}

uint level_threshold_test(uint row, char point, uchar rensho, uchar renpai)
{
  typedef struct { short rensho_up; short point_up; short renpai_down; short point_down; } ent;
  ent *entry;
  uint result;
  result = 0;
  entry = (ent *)&level_thresholds + (uchar)row;
  if (entry->rensho_up != -1 && rensho >= entry->rensho_up) result = 8;
  if (entry->renpai_down != -1 && renpai >= entry->renpai_down) result |= 0x10;
  if (entry->point_up != -1 && entry->point_up <= (short)point) result |= 8;
  if (entry->point_down != -1 && -point >= entry->point_down) result |= 0x10;
  return result;
}

void level_result_0_9(uint side_in, int lost, uint level_in, uint other_in)
{
  uchar side = side_in;
  uchar level = level_in;
  uchar other_level = other_in;
  uint old_level;
  int gain;
  card_image_rec *image;

  old_level = card_images[side].header.level;
  if (level > other_level) {
    if (lost == 0) gain = (float)(win_streak + 1) * 1.5 + 5.0;
    else gain = 2;
  }
  else {
    if (lost == 0) gain = win_streak * 3 + 0x12;
    else gain = 5;
  }
  if (gain > 100) gain = 100;
  image = &card_images[side];
  gain += image->body.exp;
  image->body.exp = gain % 100;
  image->header.level = (image->header.level * 100 + gain) / 100;
  if (image->header.level > 9) {
    image->header.level = 10;
    card_image_streaks_clear(side);
  }
  if (old_level < card_images[side].header.level) {
    level_change_flags[side] |= 8;
  }
}

void card_image_streak_restore(int side, int exp, int point)
{
  card_image_rec *image = &card_images[side];
  image->body.exp = (uchar)exp;
  image->body.point = (char)point;
  return;
}

byte level_change_flags_none(void)
{
  int i;
  byte none;
  none = 1;
  for (i = 0; i < 2; i++) {
    byte *p = level_change_flags;
    if (p[i] & 1) none = 0;
    if (p[i] & 4) none = 0;
  }
  return none;
}

void jisaku_flag_update(void)
{
  int held;
  
  if (((((level_change_flags[0] & 1) == 0) && ((level_change_flags[1] & 1) == 0)
       ) && (held = pause_frame_held(), held == 0)) && (play_style == '\x02')) {
    jisaku_flag = jisaku_check();
  }
  return;
}

void jisaku_debug_print(void)
{
  text_cursor_to_cell(0x14,0x1e);
  {
    int *p = jisaku_idle_frames;
  text_draw_formatted("NOT_TOUCH_1 %d\n",p[0]);
  text_draw_formatted("NOT_TOUCH_2 %d\n",p[1]);
  }
  {
    int *p = jisaku_lever_frames;
  text_draw_formatted("NOT_LEVER_1 %d\n",p[0]);
  text_draw_formatted("NOT_LEVER_2 %d\n",p[1]);
  }
  {
    int *p = jisaku_guard_frames;
  text_draw_formatted("NOT_G_1     %d\n",p[0]);
  text_draw_formatted("NOT_G_2     %d\n\n",p[1]);
  }
  text_draw_formatted("JISAKU_0 %d\n",jisaku_marks[0][0]);
  {
    int *p = jisaku_marks[0];
  text_draw_formatted("JISAKU_1 %d\n",p[1]);
  text_draw_formatted("JISAKU_2 %d\n",p[2]);
  text_draw_formatted("JISAKU_0 %d\n",p[3]);
  text_draw_formatted("JISAKU_1 %d\n",p[4]);
  text_draw_formatted("JISAKU_2 %d\n\n",p[5]);
  }
  text_draw_formatted("PTR %d\n\n",jisaku_slot);
  text_draw_formatted("JISAKU_FLAG %d\n\n",jisaku_flag);
  if ((jisaku_flag & 1) != 0) {
    text_draw_formatted("NOT_TOUCH\n");
  }
  if ((jisaku_flag & 2) != 0) {
    text_draw_formatted("NOT_LEVER\n");
  }
  if ((jisaku_flag & 4) != 0) {
    text_draw_formatted("NOT_G_BUTTON\n");
  }
  text_draw_formatted("RENSHO %d\n",card_body_rensho_get(&card_images[0].body));
  text_draw_formatted("RENPAI %d\n",card_body_renpai_get(&card_images[0].body));
  text_draw_formatted("POINT  %d\n",card_images[0].body.point);
  text_draw_formatted("RENSHO %d\n",card_body_rensho_get(&card_images[1].body));
  text_draw_formatted("RENPAI %d\n",card_body_renpai_get(&card_images[1].body));
  text_draw_formatted("POINT  %d\n",card_images[1].body.point);
}

uint jisaku_check(void)
{
  int idle;
  int *marks;
  uint flags;
  
  flags = 0;
  marks = jisaku_marks[jisaku_slot];
  if (marks[0] == 0) {
    idle = jisaku_idle_count();
    marks[0] = idle;
  }
  jisaku_lever_count();
  jisaku_guard_count();
  if ((jisaku_marks[0][0] != 0) && (jisaku_marks[1][0] != 0)) {
    flags |= 1;
  }
  if ((jisaku_marks[0][1] != 0) && (jisaku_marks[1][1] != 0)) {
    flags |= 2;
  }
  if ((jisaku_marks[0][2] != 0) && (jisaku_marks[1][2] != 0)) {
    flags |= 4;
  }
  return flags;
}

int jisaku_idle_count(void)
{
  uint *frames;
  uint *frames_again;
  player_input_rec *pad;
  int side;
  int idle;
  idle = 0;
  side = 0;
  do
  {
    pad = player_input_get(side);
    if (pad->now == 0)
    {
      frames = jisaku_idle_frames;
      jisaku_idle_frames[side] = frames[side] + 1;
    }
    if ((((int) (frames_again = jisaku_idle_frames)[side]) < 0xf0) && ((pad = player_input_get(side), pad->now != 0)))
    {
      jisaku_idle_frames[side] = 0;
    }
    if (0xef < ((int) jisaku_idle_frames[side]))
    {
      idle = 1;
      if (1)
      {
      }
    }
    side = side + 1;
  }
  while (side < 2);
  return idle;
}

void jisaku_lever_count(void)
{
  player_input_rec *pad;
  int side;
  
  side = 0;
  do {
    pad = player_input_get(side);
    if ((pad->now & 0xf0) != 0) {
      jisaku_lever_frames[side] = jisaku_lever_frames[side] + 1;
    }
    side = side + 1;
  } while (side < 2);
  return;
}

void jisaku_guard_count(void)
{
  player_input_rec *pad;
  int side;
  
  side = 0;
  do {
    pad = player_input_get(side);
    if ((pad->now & 1) != 0) {
      jisaku_guard_frames[side] = jisaku_guard_frames[side] + 1;
    }
    side = side + 1;
  } while (side < 2);
  return;
}

void jisaku_reset(void)
{
  int side;
  jisaku_first_match = 1;
  jisaku_flag = 0;
  side = 0;
  do
  {
    jisaku_idle_frames[side] = 0;
    jisaku_lever_frames[side] = 0;
    jisaku_guard_frames[side] = 0;
    jisaku_marks[side][0] = 0;
    jisaku_marks[side][1] = 0;
    jisaku_marks[side][2] = 0;
    side = side - -1;
  }
  while (side < 2);
  return;
}

void jisaku_match_start(void)
{
  uint slot;
  
  if (jisaku_first_match == 1) {
    jisaku_slot = 0;
    jisaku_first_match = 0;
  }
  else {
    slot = jisaku_slot & 1 ^ 1;
    jisaku_slot = slot;
    jisaku_marks[slot][0] = 0;
    jisaku_marks[slot][1] = 0;
    jisaku_marks[slot][2] = 0;
  }
  return;
}

void jisaku_round_reset(void)
{
  int side;
  
  side = 0;
  do {
    jisaku_idle_frames[side] = 0;
    jisaku_lever_frames[side] = 0;
    jisaku_guard_frames[side] = 0;
    side = side + 1;
  } while (side < 2);
  return;
}

void jisaku_round_end(void)
{
  int i;
  for (i = 0; i < 2; i++) {
    if (jisaku_lever_frames[i] == 0) {
      int (*a)[3] = jisaku_marks;
      a[(int)jisaku_slot][1] = 1;
    }
    if (jisaku_guard_frames[i] == 0) {
      int (*a)[3] = jisaku_marks;
      a[(int)jisaku_slot][2] = 1;
    }
  }
  jisaku_flag_update();
  return;
}

void card_cpu_play_end_side(uint side_in)
{
  uchar old_level;
  card_image_rec *image;
  card_image_rec *image2;
  card_image_rec *image3;
  net_info_t *info;
  uint side;
  ushort total;
  card_misc_word *misc;

  side = side_in & 0xff;
  image = &card_images[side];
  old_level = image->header.level;
  info = (net_info_t *)net_info_get();
  if (net_is_on() != 0 && net_service_active() != 0 && info->nam_state == 0) {
    image->header.flags |= 8;
    misc = &image->body.misc;
    misc->w |= 0xf;
  }
  image2 = &card_images[side];
  if (image2->header.level < 10) {
    total = (signed char)rank_round + image2->body.exp + image2->header.level * 100 + 1;
    if (total > 999) total = 999;
    image3 = &card_images[side];
    image3->body.exp = total % 100;
    image3->header.level = total / 100;
    if ((uchar)(total / 100) > 9) {
      image3->header.level = 10;
      card_image_streaks_clear(side);
    }
    if (card_images[side].header.level > old_level) {
      user_back_data.bookkeep.card_level_counts[card_images[side].header.level]++;
      user_back_data.bookkeep.card_level_counts[old_level]--;
    }
  }
  card_mode_start_result(side);
}

void card_cpu_play_end(void)
{
  int ok;
  uint side;
  
  side = 0;
  do {
    int flags = (int)side_entry_flags;
    if (((*(uchar *)(flags + side) >> 2 & 1) != 0) &&
        (ok = card_unit_ok(side), ok != 0)) {
      if (card_records[side].in_use != 0)
        card_cpu_play_end_side(side & 0xff);
    }
    side = side + 1;
  } while ((int)side < 2);
  return;
}

int discarded_code_take(int code)
{
  uint i;
  for (i = 0; i < 10; i++) { if (user_back_data.discarded_codes[i] == code) { user_back_data.discarded_codes[i] = 0; return 1; } }
  return 0;
}

void card_renew_times_reset(uint side)
{
  card_renew_times_clear(side & 0xff);
  return;
}

void card_side_reset(int side_in)
{
  int count;
  uchar *dst;
  uchar *dst_copy;
  int side;
  uint flags;
  card_record_rec *rec;

  side = (int)(char)side_in;
  dst = (uchar *)&card_records[side].header;
  dst_copy = (uchar *)&card_records[side].header2;
  count = 0x1b;
  do {
    *dst = 0;
    dst = dst + 1;
    *dst_copy = 0;
    dst_copy = dst_copy + 1;
    count = count + -1;
  } while (count != -1);
  dst = (uchar *)&card_records[side].body;
  count = 0x47;
  do {
    *dst = 0;
    dst = dst + 1;
    count = count + -1;
  } while (count != -1);
  card_format2_area_clear(side);
  dst = (uchar *)&card_images[side].header;
  count = 0x1b;
  do {
    *dst = 0;
    dst = dst + 1;
    count = count + -1;
  } while (count != -1);
  dst = (uchar *)&card_images[side].body;
  count = 0x47;
  do {
    *dst = 0;
    dst = dst + 1;
    count = count + -1;
  } while (count != -1);
  flags = card_flags[side];
  if (flags & 1) {
    if (flags & 8) {
      card_images[side].header.level = 0;
    }
    else {
      card_images[side].header.level = 0xff;
    }
  }
  else {
    card_images[side].header.level = 0xff;
  }
  rec = &card_records[side];
  rec->in_use = 0;
  flags = card_flags[side];
  if (flags & 1) {
    if (flags & 8) {
      rec->header.level = 0;
    }
    else {
      card_records[side].header.level = 0xff;
    }
  }
  else {
    card_records[side].header.level = 0xff;
  }
  card_body_name_clear(&card_records[side].body);
  card_body_name_clear(&card_images[side].body);
}

void rank_points_update(void)
{
  short s;
  int i;

  if (net_ready_latched() != 0) {
    for (i = 0; i < 2; i++) {
      if (card_syu_bits_get(i) == 2) {
        if (champion_side == i) {
          if (jisaku_flag_get() == 0) {
            card_image_rec *e = card_images;
            if (e[i].body.rank_pnt != 0xffff) {
              e[i].body.rank_pnt++;
            }
            if (net_card_place_matches(i & 0xff)) {
              card_image_rec *f = card_images;
              if (f[i].body.tenpo_pnt != 0xffff) {
                f[i].body.tenpo_pnt++;
              }
            }
          }
        }
        else {
          ushort *r = &card_images[i].body.rank_pnt;
          s = *r;
          if (s != 0) {
            *r = s + -1;
          }
          if (net_card_place_matches(i & 0xff)) {
            r = &card_images[i].body.tenpo_pnt;
            s = *r;
            if (s != 0) {
              *r = s + -1;
            }
          }
        }
      }
    }
  }
}

int jisaku_flag_get(void)
{
  return jisaku_flag;
}

void card_image_all_win_inc(int side)
{
  ushort count;

  count = card_images[side].header.all_win;
  if (count != 0xffff) {
    count = count + 1;
  }
  card_images[side].header.all_win = count;
  return;
}

void card_image_all_lose_inc(int side)
{
  ushort count;

  count = card_images[side].header.all_lose;
  if (count != 0xffff) {
    count = count + 1;
  }
  card_images[side].header.all_lose = count;
  return;
}

void card_image_rensho_inc(int side)
{
  byte count;
  
  count = card_body_rensho_get(&card_images[side].body);
  if (count < 0xf) {
    count = count + 1 & 0xff;
  }
  card_body_rensho_set(&card_images[side].body,count);
  card_body_renpai_set(&card_images[side].body,0);
  return;
}

void card_image_renpai_inc(int side)
{
  byte count;
  
  count = card_body_renpai_get(&card_images[side].body);
  if (count < 0xf) {
    count = count + 1 & 0xff;
  }
  card_body_rensho_set(&card_images[side].body,0);
  card_body_renpai_set(&card_images[side].body,count);
  return;
}

void card_image_point_inc(int side)
{
  char c;
  c = card_images[side].body.point;
  if (c != 0x7f) c = c + 1;
  card_images[side].body.point = c;
}

void card_image_point_dec(int side)
{
  char c;
  c = card_images[side].body.point;
  if (-0x7f < c) c = c + -1;
  card_images[side].body.point = c;
}

void card_all_lose_inc(int side)
{
  ushort s;

  s = card_records[side].header.all_lose;
  if (s != 0xffff) {
    s = s + 1;
  }
  card_records[side].header.all_lose = s;
}

void card_renpai_inc(int side)
{
  byte count;
  
  count = card_body_renpai_get(&card_records[side].body);
  if (0xf > count) {
    count = count + 1 & 0xff;
  }
  card_body_rensho_set(&card_records[side].body,0);
  card_body_renpai_set(&card_records[side].body,count);
  return;
}

void card_point_dec(int side)
{
  char c;
  c = card_records[side].body.point;
  if (c > -0x7f) c--;
  card_records[side].body.point = c;
}

int level_change_flags_compute(void)
{
  int i;
  int changed;
  level_change_flags[0] = 0;
  level_change_flags[1] = 0;
  if (play_style == 2) {
    for (i = 0; i < 2; i++) {
      if (user_back_data.card_use[i] != 0 && card_unit_ok(i)) {
        if (card_records[i].in_use != 0) {
          if (card_images[i].header.level < 0x13 || net_ready_latched() != 0) {
            level_change_flags_side(i & 0xff);
          }
        }
      }
    }
  }
  if (level_change_flags[0] != 0 || level_change_flags[1] != 0) changed = 1;
  else changed = 0;
  return changed;
}

void level_change_flags_side(uint side_in)
{
  uchar side = side_in;
  uchar other;
  uchar level;
  uchar other_level;
  ushort gain;
  uint total;
  ushort level_exp;
  uchar row;
  char point;
  uchar rensho;
  uchar renpai;
  uint result;

  other = (side & 1) ^ 1;
  { card_image_rec *e = card_images;
  level = e[side].header.level;
  other_level = e[other].header.level; }
  if (other_level > 237) other_level = 0;
  if (level > 237) level = 0;
  if (level < 10) {
    if (level > other_level) {
      gain = (win_streak + 1) * 1.5 + 5.0;
      if (gain > 100) gain = 100;
      total = (ushort)(gain + card_images[side].body.exp + card_images[side].header.level * 100);
      if (level < (uchar)(total / 100)) {
        level_change_flags[side] |= 1;
      }
      { uint sum = card_images[side].body.exp; level_exp = card_images[side].header.level * 100; sum += 2; total = sum + level_exp; }
    }
    else {
      gain = win_streak * 3 + 18;
      if (gain > 100) gain = 100;
      total = (ushort)(gain + card_images[side].body.exp + card_images[side].header.level * 100);
      if (level < (uchar)(total / 100)) {
        level_change_flags[side] |= 1;
      }
      { uint sum = card_images[side].body.exp; level_exp = card_images[side].header.level * 100; sum += 5; total = sum + level_exp; }
    }
    if (level < (uchar)(total / 100)) {
      level_change_flags[side] |= 2;
    }
  }
  else if (level > 19) {
    if (level == 30) {
      return;
    }
    if (level == 29) {
      if (other_level == 29) return;
      if (other_level != 28) return;
      row = level_row_index(29);
      { card_image_rec *e = card_images;
      point = e[side].body.point + 1;
      rensho = card_body_rensho_get(&card_images[side].body) + 1; }
      result = level_threshold_test(row, point, rensho, 0);
      if (!net_ready_latched()) return;
      if (card_syu_not_both_2()) return;
      if (result & 8) {
        level_change_flags[side] |= 1;
      }
      level_change_flags[side] |= 4;
      return;
    }
    if (level == 28) {
      if (other_level == 29) {
        if (card_syu_not_both_2()) return;
        if (!net_ready_latched()) return;
        level_change_flags[side] |= 1;
        row = level_row_index(28);
        { card_image_rec *e = card_images;
        point = e[side].body.point - 1;
        renpai = card_body_renpai_get(&card_images[side].body) + 1; }
        result = level_threshold_test(row, point, 0, renpai);
        if (result & 0x10) {
          level_change_flags[side] |= 4;
        }
        return;
      }
      if (other_level != 28) return;
    }
    else {
      if (level > 27) return;
      if (level != other_level) return;
    }
    row = level_row_index(level);
    { card_image_rec *e = card_images;
    point = e[side].body.point + 1;
    rensho = card_body_rensho_get(&card_images[side].body) + 1; }
    result = level_threshold_test(row, point, rensho, 0);
    if (!net_ready_latched()) return;
    if (card_syu_bits_get(side) == 2) {
      if (result & 8) {
        level_change_flags[side] |= 1;
      }
    }
    row = level_row_index(level);
    { card_image_rec *e = card_images;
    point = e[side].body.point - 1;
    renpai = card_body_renpai_get(&card_images[side].body) + 1; }
    result = level_threshold_test(row, point, 0, renpai);
    if (result & 0x10) {
      level_change_flags[side] |= 4;
    }
  }
  else {
    if (level != other_level) return;
    row = level_row_index(level);
    { card_image_rec *e = card_images;
    point = e[side].body.point + 1;
    rensho = card_body_rensho_get(&card_images[side].body) + 1; }
    result = level_threshold_test(row, point, rensho, 0);
    if (level == 19) {
      if (!net_ready_latched()) return;
      if (card_syu_bits_get(side) == 2) {
        if (result & 8) {
          level_change_flags[side] |= 1;
        }
      }
    }
    else if (result & 8) {
      level_change_flags[side] |= 1;
    }
    row = level_row_index(level);
    { card_image_rec *e = card_images;
    point = e[side].body.point - 1;
    renpai = card_body_renpai_get(&card_images[side].body) + 1; }
    result = level_threshold_test(row, point, 0, renpai);
    if (result & 0x10) {
      level_change_flags[side] |= 4;
    }
  }
}

void card_service_state_update(void)
{
  if (mode_get_main() == MODE_MAIN_ADVERTISE) {
    if (card_service_state.open == '\0') {
      card_service_state.mode = -1;
    }
  }
  else if ((card_service_state.mode == 1) && (card_service_state.open == '\0')) {
    if (play_style == '\x01') {
      card_service_state.mode = 3;
      card_close_phase_set(1);
    }
    else if (play_style == '\x02') {
      card_service_state.mode = 2;
    }
  }
  else if (play_style == '\x01' && card_service_state.open == '\0' &&
           card_service_state.mode != -1 && card_service_state.mode != 3) {
    card_service_state.mode = -1;
  }
  if (card_service_state.open != '\0') {
    card_service_state.mode = 1;
    card_close_phase_set(0);
  }
  if (card_service_state.mode == -1) {
    card_close_phase_set(0);
  }
}

uchar * card_service_state_get(void)
{
  return (uchar *)&card_service_state;
}

void card_block_e4ec_set(void *src)
{
  memcpy(card_block_e4ec,src,6);
  return;
}

card_record_rec *card_record_get(int side)
{
  return &card_records[side];
}

card_image_rec *card_image_get(int side)
{
  return &card_images[side];
}

void card_user_count_clear(void)
{
  card_user_count = 0;
  return;
}

int card_user_count_get(void)
{
  return card_user_count;
}

void card_user_count_inc(void)
{
  card_user_count = card_user_count + 1;
  return;
}

uchar level_change_flags_get(int side)
{
  return level_change_flags[side];
}

void card_record_release(int side)
{
  card_records[side].in_use = 0;
}

void level_change_rose_mark(uchar side)
{
  level_change_flags[side] |= 8;
  return;
}

void level_change_fell_mark(uchar side)
{
  level_change_flags[side] |= 0x10;
  return;
}

uint card_record_char_get(uint side)
{
  uint chara;
  
  chara = card_hdr_char_num_get(&card_records[side & 0xff].header);
  return chara;
}

uint card_image_char_get(uint side)
{
  uint chara;
  
  chara = card_hdr_char_num_get(&card_images[side & 0xff].header);
  return chara;
}

void card_record_char_set(unsigned char side, unsigned char chara)
{
  card_hdr_char_num_set(&card_records[side].header, chara);
}

void card_image_char_set(uchar side, uchar chara)
{
  card_hdr_char_num_set(&card_images[side].header,chara);
}

ushort card_syu_bits_get(byte side)
{
  return card_images[side].body.syu.f.user_class;
}

int card_syu_not_both_2(void)
{
  byte i;
  int n;

  n = 0;
  for (i = 0; i < 2; i++) {
    if (card_syu_bits_get(i) == 2) n++;
  }
  if (n == 2) return 0;
  return 1;
}

int card_misc1_bits_3_4(uchar side)
{
  int fouls;

  fouls = 0;
  if ((card_images[side].body.misc.f.foul_first != 0) &&
     (card_images[side].body.misc.f.foul_second != 0)) {
    fouls = 1;
  }
  return fouls;
}

void card_rank_pnt_dec(uchar side)
{
  short points;
  if (net_ready_latched() != 0 && card_syu_bits_get(side) == 2) {
    points = card_records[side].body.rank_pnt;
    if (points != 0) {
      card_records[side].body.rank_pnt = points - 1;
    }
  }
}

void card_tenpo_pnt_dec(uchar side)
{
  short points;
  if (net_ready_latched() != 0 && card_syu_bits_get(side) == 2) {
    points = card_records[side].body.tenpo_pnt;
    if (points != 0) {
      card_records[side].body.tenpo_pnt = points - 1;
    }
  }
}

void card_close_phase_set(int phase)
{
  card_close_phase = phase;
  return;
}

int card_close_phase_get(void)
{
  return card_close_phase;
}

int card_service_mode_get(void)
{
  return card_service_state.mode;
}

void card_dbg_points_copy(void)
{
  int i;
  
  card_dbg_secret = jisaku_flag;
  for (i = 0; i < 2; i++) {
    ((struct { char m[2]; } *)card_dbg_point)->m[i] = card_images[i].body.point;
    ((struct { char m[2]; } *)card_dbg_rensho)->m[i] = card_body_rensho_get(&card_images[i].body);
    ((struct { char m[2]; } *)card_dbg_renpai)->m[i] = card_body_renpai_get(&card_images[i].body);
  }
}

void card_make_open(void)
{
  return;
}

void card_make_close(void)
{
  card_make_flag = 0;
}

void dbg_card_init(void)
{
  uchar *p = dbg_card;
  memset(p,0,0x88);
  *(short *)(p + 0x34) = 500;
  p[9] = 3;
  p[4] = 3;
}

void empty_fn_0c0899e0(void)
{
  return;
}

void empty_fn_0c089a00(void)
{
  return;
}

void dbg_card_header_pack(card_header_rec *header, uchar *src)
{
  uint id;
  int i;
  uchar b;

  header->crc = 0;
  b = src[3] << 7 | src[4] << 6 | src[0] << 4 | src[2] << 3 | src[1] << 2 |
       src[5];
  header->flags = b;
  header->char_num = src[6];
  header->level = src[7];
  header->all_win = *(short *)(src + 8);
  header->all_lose = *(short *)(src + 10);
  for (i = 0; i < 8; i++) {
    header->item_have[i] = *(uchar *)(i + (int)src + 0xc);
  }
  header->use_count = *(ushort *)(src + 0x30) & 0x1ff;
  b = src[0x35] << 7 | src[0x34] << 5 | src[0x33] << 3 | src[0x32] << 1;
  header->ftf.b[0] = b;
  id = *(uint *)(src + 0x38);
  header->card_id[0] = id;
  header->card_id[1] = id >> 8;
  header->card_id[2] = id >> 0x10;
  header->nickname = *(short *)(src + 0x3c);
}

void dbg_card_save(void)
{
  typedef struct { short play_id;uchar rensho; uchar renpai; char exp; char win_lose; uchar color_1p[4]; uchar color_2p[4]; char equip_1p[4]; char equip_2p[4]; char curse; char emblem; char name[20]; char pad_0x2c; char other_col; char temp; char pad_0x2f; short my_tenpo_id; short money; uchar emblem_dan; char pad_0x35; short syuutyuu; short quest0; short quest1; char misc0; char misc1; char misc2; char pad_0x3f; short rank_pnt; short tenpo_pnt; } dbg_body_fields;
  typedef struct { char pad_0x0[7]; uchar level; char pad_0x8[56]; dbg_body_fields body; } dbg_card_fields;
  uchar target;
  uint side;
  card_image_rec *image;
  card_body_rec *body;
  dbg_card_fields *dbg;
  dbg_body_fields *src;
  int k;
  uint flags;

  target = dbg_card_taget;
  side = target;
  image = &card_images[side];
  body = &card_images[side].body;
  dbg = (dbg_card_fields *)&f_version;
  src = &dbg->body;
  flags = card_flags_get(side);
  if ((flags & 1) == 0) return;
  if ((flags & 0x80) == 0) return;
  dbg_card_header_pack(&image->header,(uchar *)dbg);
  body->crc = 0;
  body->play_id = src->play_id;
  if (dbg->level < 10) body->exp = dbg->body.exp;
  else body->exp = src->rensho << 4 | src->renpai;
  body->point = src->win_lose;
  body->color_1p.w = (uint)src->color_1p[3] << 15 | (uint)src->color_1p[2] << 10 | (uint)src->color_1p[1] << 5 | (uint)src->color_1p[0];
  body->color_2p.w = (uint)src->color_2p[3] << 15 | (uint)src->color_2p[2] << 10 | (uint)src->color_2p[1] << 5 | (uint)src->color_2p[0];
  for (k = 0; k < 4; k++) {
    body->equip_1p[k] = src->equip_1p[k];
    body->equip_2p[k] = src->equip_2p[k];
  }
  body->curse.b.curse = src->curse;
  body->curse.b.emblem = src->emblem;
  for (k = 0; k < 20; k++) body->name[k] = src->name[k];
  body->other_col = src->other_col;
  body->temp = src->temp;
  body->my_tenpo_id = src->my_tenpo_id;
  body->money = src->money;
  body->syu.b.emblem_dan = (uchar)(src->emblem_dan << 4);
  body->syu.b.syuutyuu = src->syuutyuu;
  body->quest.b.quest0 = src->quest0;
  body->quest.b.quest1 = src->quest1;
  body->misc.b[0] = src->misc0;
  body->misc.b[1] = src->misc1;
  body->misc.b[2] = src->misc2;
  body->rank_pnt = src->rank_pnt;
  body->tenpo_pnt = src->tenpo_pnt;
  card_body_prize_misses_set(body,0);
  card_body_syu_b11_set(body,0);
  card_body_syu_top3_set(body,0);
  champion_side = side;
  card_mode_start_result(side);
}

void card_make_update(void)
{
  uchar chara;
  int k, j, i, n, slot, byte_no;
  uint ids;
  uchar bits;
  chara = char_num;
  for (k = 0, n = 0; k < 7; k++) {
    ids = ((struct { char pad[24]; uint item_ids[7]; } *)&dbg_card)->item_ids[k];
    for (j = 3; j >= 0; j--) {
      bits = ids >> (j * 8);
      for (i = 0; i < 8; i++) {
        if ((bits >> i) & 1) {
          slot = item_bit_index(chara, (n + i + 1) & 0xff);
          if (slot >= 0) if (slot < 64) {
            ((struct { char pad[16]; char item_bits[8]; } *)&dbg_card)->item_bits[byte_no = slot / 8] = *(dbg_card_item_bits + byte_no) | 1 << slot % 8;
          }
        }
      }
      n += 8;
    }
  }
}

void dbg_sample_set(void)
{
  dbg_card_init();
  return;
}
