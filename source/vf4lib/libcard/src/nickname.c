/*
 * vf4lib/libcard/src/nickname.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern void *nickname_point_funcs[21]; /* 0x0c18a7d4 */
extern int round_time; /* 0x0c292558 */
extern undefined4 match_round_time; /* 0x0c29255c */
extern char play_style; /* 0x0c2925cc */
extern char champion_side; /* 0x0c2925d4 */
extern int win_streak; /* 0x0c2925dc */
extern short *nickname_throw_moves[19]; /* 0x0c2a0258 */
extern char nickname_active; /* 0x0c4313b0 */
extern ushort nickname_rates[8]; /* 0x0c4313b2 */
extern nickname_point nickname_points[21][2]; /* 0x0c43151c */

void nickname_boot_init(void)
{
  nickname_record_rec *rec;
  int i;
  rec = nickname_record_get(0);
  for (i = 0; i < 2; i++, rec++) {
    nickname_record_clear(rec);
  }
  nickname_active = 0;
  for (i = 0; i < 8; i++) {
    nickname_rates[i] = 100;
  }
  idnm = 0;
  lv_chk_games = 0x14;
  disp_force = OFF_ON_OFF;
  debug_disp_0c37e4ac = OFF_ON_OFF;
}

void nickname_stage_init(void)
{
  int i;
  for (i = 0; i < 2; i++) nickname_side_stage_init(i);
  nickname_active = 0;
  if (nickname_both_players() != 0) {
    nickname_active = 1;
    nickname_points_clear();
  }
}

void nickname_round_init(void)
{
  if (nickname_active != '\0') {
    nickname_points_round_clear();
  }
  return;
}

void nickname_disp(void)
{
  int side;

  if (nickname_active != '\0') {
    nickname_points_update();
    side = 0;
    do {
      nickname_side_disp(side);
      side = side + 1;
    } while (side < 2);
  }
  return;
}

void nickname_judge(void)
{
  int side;

  if (nickname_active != '\0') {
    side = 0;
    do {
      nickname_side_judge(side);
      side = side + 1;
    } while (side < 2);
  }
  return;
}

int nickname_recv_data_set(int side, nickname_recv_rec *data)
{
  nickname_record_rec *rec;

  rec = nickname_record_get(side);
  if (!((rec->flags >> 1 & 1) != 0)) {
    return -1;
  }
  rec->games = data->games;
  rec->disp_type = data->disp_type;
  rec->disp_name = data->disp_name;
  memcpy(rec->level,data->level,8);
  memcpy(rec->pt,data->pt,0x30);
  if (data->god != '\0') {
    rec->flags = rec->flags | 0x10;
  }
  rec->flags = rec->flags | 1;
  nickname_card_marks_sync(rec);
  nickname_disp_name_fix(rec);
  rec->flags = rec->flags | 4;
  return 0;
}

int nickname_rates_set(void *rates)
{
  memcpy(nickname_rates,rates,0x10);
  return 0;
}

nickname_result_rec *nickname_result_get(int side)
{
  nickname_record_rec *rec;
  rec = nickname_record_get(side);
  if ((nickname_active == '\0') || ((rec->flags >> 2 & 1) == 0)) {
    return 0;
  }
  if (nickname_champion_cached(side,rec) != 0) return &rec->result[0]; return &rec->result[1];
}

int nickname_current_get(int side, uint *name_out, uint *level_out)
{
  nickname_record_rec *rec;
  uint name;

  rec = nickname_record_get(side);
  *level_out = 0xffffffff;
  *name_out = 0xffffffff;
  if ((rec->flags >> 2 & 1) == 0) return 0;
  if ((rec->disp_type == '\0') || (rec->disp_name == 0xff)) {
    return 0;
  }
  *name_out = (uint)rec->disp_name;
  name = (uint)rec->disp_name;
  if (name < 8) {
    *level_out = (uint)rec->level[name];
  }
  return 1;
}

int nickname_levelup_get(int side, uint *name_out, uint *level_out)
{
  nickname_record_rec *rec;
  uint i;
  uchar max;
  uint level;
  uint name;
  int t;

  rec = nickname_record_get(side);
  *level_out = 0xffffffff;
  *name_out = 0xffffffff;
  if (nickname_active == 0 || (rec->flags >> 2 & 1) == 0) return 0;
  if (rec->disp_type == 0) return 0;
  if (nickname_champion_cached(side,rec) == 0) return 0;
  if ((rec->flags >> 5 & 1) != 0) {
    *name_out = 8;
    goto ok;
  }
  if (rec->disp_type == 1) {
    max = 0;
    for (i = 0; (int)i < 7; i++) {
      if (rec->levelup[i] != 0 && (level = rec->level[i]) > max) {
        *level_out = level;
        max = level;
        *name_out = i;
      }
    }
    t = max == 0;
    return t == 0;
  }
  if (rec->disp_type != 2) goto bad;
  name = rec->disp_name;
  if (name > 6 || rec->levelup[name] == 0) goto fail;
  *level_out = rec->level[name];
  *name_out = name;
ok:
  return 1;
fail:
  return 0;
bad:
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/nickname.c",0x1d3,"disp_type illegal");
  return -1;
}

int nickname_side_stage_init(int side)
{
  nickname_record_rec *rec;
  card_record_rec *card;
  char f; char *q;

  rec = nickname_record_get(side);
  nickname_record_clear(rec);
  q = (char *)side_entry_flags; f = q[side];
  if (((f >> 2) & 1) == 0) return -1;
  card = card_record_get(side);
  if (card->in_use == 0) return -1;
  rec->flags = rec->flags | 2;
  nickname_card_marks_load(side,rec);
  return 0;
}

int nickname_side_disp(int side)
{
  nickname_record_rec *rec;

  rec = nickname_record_get(side);
  if ((rec->flags >> 1 & 1) == 0) return -1;
  if ((rec->flags >> 2 & 1) == 0) return -1;
  nickname_debug_draw(side);
  return (rec->flags >> 3 & 1 ^ 1) - 1;
}

int nickname_side_judge(int side)
{
  nickname_record_rec *rec;
  int champion;
  int i;

  rec = nickname_record_get(side);
  if ((rec->flags >> 2 & 1) == 0) {
    return -1;
  }
  if ((rec->flags >> 3 & 1) != 0) {
    int *q = (int *)rec->result;
    q[0] = 0;
    q[1] = 0;
    q[2] = 0;
    q[3] = 0;
    q[4] = 0;
    q[5] = 0;
    q[6] = 0;
    q[7] = 0;
    *(char *)(q + 8) = 0;
    if ((rec->flags >> 4 & 1) != 0) {
      rec->result[0].god = 1;
    }
    for (i = 0; i < 8; i++) {
      rec->result[0].level[i] = rec->level[i];
    }
    memcpy(&rec->result[1],&rec->result[0],0x21);
    nickname_card_marks_save(side,rec);
  }
  else {
    nickname_pt_diff_store(side,rec);
    nickname_level_judge(side,rec);
    champion = nickname_champion_cached(side,rec);
    if (champion != 0) {
      nickname_card_marks_save(side,rec);
    }
  }
  return 0;
}

int nickname_disp_name_fix(nickname_record_rec *rec)
{
  uint max;
  int i;
  uint name;
  uchar *q;

  if (rec->disp_type == '\0') {
    rec->disp_name = 0xff;
    return 0;
  }
  if (rec->disp_type == '\x01') {
    max = 0;
    if ((rec->flags >> 4 & 1) != 0) {
      rec->disp_name = 8;
      return 0;
    }
    for (i = 0; i < 8; i++) {
      if (rec->level[i] > max) {
        max = rec->level[i];
        rec->disp_name = (char)i;
      }
    }
    if (max == 0) {
      rec->disp_name = 0xff;
    }
    return 0;
  }
  if (rec->disp_type == '\x02') {
    q = &rec->disp_name;
    name = *q;
    if (name < 8) {
      if (rec->level[name] == '\0') {
        *q = 0xff;
      }
      return 0;
    }
    if (name == 8) {
      if ((rec->flags >> 4 & 1) == 0) {
        rec->disp_name = 0xff;
      }
    }
    return 0;
  }
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/nickname.c",0x273,"disp_type illegal");
  return -1;
}

int nickname_card_marks_sync(nickname_record_rec *rec)
{
  uchar mark;
  uchar bits;
  uchar *level;
  int i;
  for (i = 0; i < 7; i++) {
    level = &rec->level[i];
    bits = *level & 3;
    mark = rec->card_mark[i];
    if (bits != mark) {
      if ((bits + 1 & 3) == mark) {
        rec->flags = rec->flags | 8;
        if (++*level > 7) {
          *level = 7;
        }
      }
      else {
        rec->card_mark[i] = bits;
      }
    }
  }
  return 0;
}

int nickname_pt_diff_store(int side, nickname_record_rec *rec)
{
  uint pt;
  int counter;
  int name;

  name = 0;
  do {
    counter = 0;
    do {
      pt = nickname_point_get(side,name,counter);
      if (name == 0 && counter == 1) {
        pt = pt / 10 & 0xffff;
      }
      if (0xff < pt) {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/nickname.c",0x2b5,"pt_diff overflow");
        pt = 0xff;
      }
      rec->pt_diff[name][counter] = (char)pt;
      counter = counter + 1;
    } while (counter < 3);
    name = name + 1;
  } while (name < 7);
  return 0;
}

int nickname_level_judge(int side, nickname_record_rec *rec)
{
  int i;
  int j;
  int r;

  if ((rec->flags >> 4 & 1) != 0) {
    rec->result[0].god = 1;
  }
  else {
    rec->result[0].god = 0;
  }
  for (i = 0; i < 8; i++) {
    rec->result[0].level[i] = rec->level[i];
    for (j = 0; j < 3; j++) {
      rec->result[0].pt_diff[i][j] = rec->pt_diff[i][j];
    }
  }
  memcpy(&rec->result[1],&rec->result[0],0x21);
  if ((rec->flags >> 4 & 1) != 0) return 0;
  if (rec->games >= lv_chk_games) {
    r = nickname_god_check(side,rec);
    if (r != 0) {
      rec->flags = rec->flags | 0x30;
      rec->result[0].god = 1;
    }
    else {
      r = nickname_levelup_candidates(rec);
      if (r != 0) {
        nickname_level_up(rec);
      }
    }
  }
  return 0;
}

int nickname_levelup_candidates(nickname_record_rec *rec)
{
  int i;
  int j;
  uint level;
  uint rate;
  uint ok;
  uint value;
  ushort *threshold;
  int r;

  r = 0;
  for (i = 0; i < 7; i++) {
    level = rec->level[i];
    rate = nickname_rates[i];
    if (level < 7) {
      if (rate == 0) {
        rate = 100;
      }
      threshold = nickname_threshold_get(i,level);
      ok = 1;
      for (j = 0; j < 3; j++) {
        value = ((uint)rec->pt[i][j] +
             (uint)rec->pt_diff[i][j]) * 100 / rate;
        if (value < threshold[j]) ok = 0;
      }
      if (ok) {
        rec->levelup[i] = 1;
        r = 1;
      }
      else {
        rec->levelup[i] = 0;
      }
    }
  }
  return r;
}

int nickname_level_up(nickname_record_rec *rec)
{
  uint sel;
  uint name;
  uint min;
  int i;
  int j;
  char *q;

  name = rec->disp_name;
  if (((rec->disp_type == 2) & (name <= 6)) && rec->levelup[name] != 0) {
    sel = name;
  }
  else {
    min = 7;
    for (i = 0; i < 7; i++) {
      if (rec->levelup[i] != 0 && rec->level[i] < min) {
        min = rec->level[i];
      }
    }
    if (min == 7) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/nickname.c",0x33f,"no level up");
      return 0;
    }
    sel = nickname_random() % 7;
    while (rec->levelup[sel] == 0 || rec->level[sel] != min) {
      if (++sel == 7) sel = 0;
    }
  }
  for (i = 0; i < 7; i++) {
    if (i == sel) {
      rec->level[i]++;
      rec->result[0].level[i]++;
      for (j = 0; j < 3; j++) {
        rec->result[0].pt_diff[i][j] = 0;
        q = (char *)(i * 3 + (int)rec) + j + 0x90;
        q[2] = 0x7f;
      }
    }
    else {
      rec->levelup[i] = 0;
    }
  }
  return 0;
}

int nickname_god_check(int side, nickname_record_rec *rec)
{
int i; int r; for (i = 0; i < 7; i++) { if (rec->level[i] < 7) return 0; }
if (nickname_god_streak_check(side) == 0) return 0; return 1;
}

ushort *nickname_threshold_get(int name, int level)
{
  if (6 < name || 7 < level) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/nickname.c",0x387,"name or level illegal");
    level = 0;
    name = 0;
  }
  return (ushort *)nickname_thresholds[name][level];
}

nickname_record_rec *nickname_record_get(int side)
{
  if (1 < side) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/nickname.c",0x393,"idnm illegal");
    side = 0;
  }
  return (nickname_record_rec *)nickname_records[side];
}

void nickname_record_clear(nickname_record_rec *rec)
{
  memset(rec,0,0xac);
  return;
}

int nickname_both_players(void)
{
  int i;
  char *p; for (i = 0; i < 2; i++) { p = (char *)side_entry_flags; if ((p[i] >> 2 & 1) == 0) return 0; }
  return 1;
}

int nickname_card_marks_load(int side, nickname_record_rec *rec)
{
  ushort word;
  int i;

  word = card_hdr_nickname_get(&card_image_get(side)->header);
  for (i = 0; i < 7; i++, word >>= 2) {
    rec->card_mark[i] = word & 3;
  }
  if ((word & 1) != 0) {
    rec->flags = rec->flags | 0x10;
  }
  return 0;
}

int nickname_card_marks_save(int side, nickname_record_rec *rec)
{
  int i;
  ushort word;

  word = 0;
  for (i = 0; i < 7; i++) {
    word |= (rec->level[i] & 3) << (i * 2);
  }
  if ((rec->flags >> 4 & 1) != 0) {
    word |= 0x4000;
  }
  card_hdr_nickname_set(&card_image_get(side)->header,word);
  return 0;
}

int nickname_champion_cached(int side, nickname_record_rec *rec)
{
  typedef struct { char pad[100]; unsigned int : 6; unsigned int a : 1; unsigned int b : 1; } bf;
  if (((bf *)rec)->a) return 1;
  if (((bf *)rec)->b) return 0;
  if (nickname_champion_check(side) != 0) {
    ((bf *)rec)->a = 1;
    return 1;
  }
  else {
    ((bf *)rec)->b = 1;
    return 0;
  }
}

bool nickname_champion_check(int side)
{
  int t;
  if (side != champion_side) return 0;
  if (level_change_flags_get(side) != 0) return 0;
  if ((int)mission_active_get() >= 0) t = 1; else t = 0;
  return !t;
}

int nickname_god_streak_check(int side)
{
  int r;

  if (!((side != (champion_side)) || (r = 1, win_streak < 0x13))) return r;
  r = 0;
  return r;
}

int nickname_random(void)
{
  int r;

  r = lcg_next15();
  return r;
}

void set_recv_data(void)
{
  if (1 < idnm) {
    idnm = 0;
  }
  nickname_recv_data_set((uint)idnm,(nickname_recv_rec *)&games);
  return;
}

void nickname_debug_draw(int side)
{
  ushort v[3];
  int col[2];
  nickname_record_rec *rec;
  int i;
  short *q;

  col[0] = 10;
  col[1] = 0x37;
  rec = nickname_record_get(side);
  if (debug_disp_0c37e4ac != OFF_ON_OFF && (player_input_get(side)->now & 0x8000) != 0) {
    text_cursor_to_cell(col[side],0xf);
    text_draw_formatted("GAMES: %d\n",rec->games);
    for (i = 0; i < 7; i++) {
      q = (short *)(i * 6);
      q = (short *)((int)q + (int)rec);
      q += 0x1a;
      v[0] = nickname_point_get(side,i,0) + *q;
      if (i == 0) {
        v[1] = rec->pt[0][1] + nickname_point_get(side,0,1) / 10;
      }
      else {
        q = (short *)(i * 6);
        q = (short *)((int)q + (int)rec);
        q += 0x1b;
        v[1] = nickname_point_get(side,i,1) + *q;
      }
      q = (short *)(i * 6);
      q = (short *)((int)q + (int)rec);
      q += 0x1c;
      v[2] = nickname_point_get(side,i,2) + *q;
      text_draw_formatted("%s:\n %4d %4d %4d\n",e_disp_name_names[i],v[0],v[1],v[2]);
    }
  }
}

void nickname_points_clear(void)
{
  int n;
  nickname_point *slot;

  slot = &nickname_points[0][0];
  n = 0x29;
  do {
    slot->count = 0;
    slot->state = 0;
    slot->work[0] = 0;
    slot->work[1] = 0;
    slot->work[2] = 0;
    slot = slot + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void nickname_points_round_clear(void)
{
  int n;
  nickname_point *slot;

  slot = &nickname_points[0][0];
  n = 0x29;
  do {
    slot->state = 0;
    slot->work[0] = 0;
    slot->work[1] = 0;
    slot->work[2] = 0;
    slot = slot + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void nickname_points_update(void)
{
  int side;
  code *func;
  nickname_point *slot;
  int counter;
  int name;

  slot = &nickname_points[0][0];
  if (play_style == '\x02') {
    name = 0;
    do {
      counter = 0;
      for (; counter < 3; slot = slot + 2, counter = counter + 1) {
        func = (code *)nickname_point_funcs[name * 3 + counter];
        if (func != (code *)0x0) {
          for (side = 0; side < 2; side = side + 1) {
            (*func)(side,slot + side);
          }
        }
      }
      name = name + 1;
    } while (name < 7);
  }
  return;
}

uint nickname_point_get(int side, int name, int counter)
{
  int r;
  int *tally;
  uint ret = 0;

  if ((side < 2 & name < 7) && (counter < 3)) {
    if (name == 3) {
      tally = msnm_side_tally_get((int)(char)side);
      r = 0;
      switch (counter) {
      case 0:
        r = nickname_points[name * 3 + counter][side].count;
        break;
      case 1:
        r = tally[5];
        break;
      case 2:
        r = tally[6];
        break;
      }
      if (r > 0xffff) r = 0xffff;
      ret = (ushort)r;
    }
    else ret = nickname_points[name * 3 + counter][side].count;
  }
  return ret;
}

void nickname_pt_combo0(uint side, nickname_point *slot)
{
  int r;

  r = nickname_attack_hit(side);
  if ((r != 0) && (r = nickname_in_combo(side ^ 1), r != 0)) {
    slot->count = slot->count + 1;
  }
  return;
}

void nickname_pt_combo1(uint side, nickname_point *slot)
{
  rob_rec *rob;
  int r;

  rob = rob_get(side & 0xff);
  r = nickname_attack_hit(side);
  if ((r != 0) && (r = nickname_in_combo(side ^ 1), r != 0)) {
    slot->count = slot->count + rob->last_damage_2;
  }
  return;
}

void nickname_pt_combo2(uint side, nickname_point *slot)
{
  int r;

  switch ((ushort)slot->state) {
  case 0:
    r = nickname_combo_damage30(side ^ 1);
    if (r != 0) {
      slot->state = slot->state + 1;
      slot->count = slot->count + 1;
    }
    break;
  case 1:
    r = nickname_in_combo(side ^ 1);
    if (r == 0) {
      slot->state = 0;
    }
    break;
  }
  return;
}

void nickname_pt_chicken0(uint side, nickname_point *slot)
{
  int r;

  r = nickname_chicken_test(side);
  if (r != 0) {
    slot->count = slot->count + 1;
  }
  return;
}

void nickname_pt_chicken1(uint side, nickname_point *slot)
{
  int r;

  r = nickname_chicken_test(side);
  if ((r != 0) && (r = nickname_round_opening(), r != 0)) {
    slot->count = slot->count + 1;
  }
  return;
}

void nickname_pt_throw0(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_rob28_bit29(side) == 0) return;
    if (nickname_rob318_bit5(side) == 0) return;
    if (nickname_rob28_bit20(side) != 0) return;
    if (nickname_rob584_bit20(side) != 0) return;
    slot->state = slot->state + 1;
    break;
  case 1:
    if (nickname_rob28_bit27(side ^ 1) != 0 || nickname_rob_out(side) != 0) {
      slot->state = 0;
    }
    else if (nickname_rob318_bit5(side) != 0 || nickname_rob28_bit26(side ^ 1) == 0) {
      slot->count = slot->count + 1;
      slot->state = 0;
    }
    break;
  }
}

void nickname_pt_throw1(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_frame_advantage8(side) != 0) {
      slot->state = slot->state + 1;
    }
    break;
  case 1:
    if (nickname_rob28_bit29(side) != 0 && nickname_rob318_bit5(side) != 0) {
      slot->state = slot->state + 1;
    }
    side ^= 1;
    if (nickname_rob_neutral(side) != 0 || nickname_attack_hit(side) != 0 ||
        (nickname_attack_flag2(side) != 0 && nickname_rob318_bit5(side) != 0)) {
      slot->state = 0;
    }
    break;
  case 2:
    if (nickname_rob28_bit27(side ^ 1) != 0 || nickname_rob_out(side) != 0) {
      slot->state = 0;
    }
    else if (nickname_rob318_bit5(side) != 0 || nickname_rob28_bit26(side ^ 1) == 0) {
      slot->count = slot->count + 1;
      slot->state = 0;
    }
    break;
  }
  return;
}

void nickname_pt_throw2(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_throw_listed(side) != 0 && nickname_rob318_bit5(side) != 0) {
      slot->state = slot->state + 1;
    }
    break;
  case 1:
    if (nickname_rob28_bit27(side ^ 1) != 0 || nickname_rob_out(side) != 0) {
      slot->state = 0;
    }
    else if (nickname_rob318_bit5(side) != 0 || nickname_rob28_bit26(side ^ 1) == 0) {
      slot->count = slot->count + 1;
      slot->state = 0;
    }
    break;
  }
  return;
}

void nickname_pt_return0(uint side, nickname_point *slot)
{
  int r;

  r = nickname_rob28_bit27(side);
  if ((r != 0) && (r = nickname_rob318_bit5(side), r != 0)) {
    slot->count = slot->count + 1;
  }
  return;
}

void nickname_pt_return1(uint side, nickname_point *slot)
{
  return;
}

void nickname_pt_return2(uint side, nickname_point *slot)
{
  return;
}

void nickname_pt_response0(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_attack_guarded(side ^ 1) != 0) {
      slot->state = slot->state + 1;
    }
    break;
  case 1:
    if ((nickname_attack_flag2(side) != 0) || (nickname_rob28_bit30(side) != 0)) {
      slot->count = slot->count + 1;
      slot->state = 0;
    }
    if (((nickname_rob_neutral(side) != 0) && (nickname_rob28_600(side) == 0)) ||
        (nickname_attack_hit(side ^ 1) != 0)) {
      slot->state = 0;
    }
    break;
  }
  return;
}

void nickname_pt_response1(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_attack_guarded(side ^ 1) != 0) {
      slot->state = slot->state + 1;
    }
    break;
  case 1:
    if ((nickname_attack_hit(side) != 0) || (nickname_rob28_bit29(side) != 0)) {
      slot->count = slot->count + 1;
      slot->state = 0;
    }
    if (((nickname_rob_neutral(side) != 0) && (nickname_rob28_600(side) == 0)) ||
        (nickname_attack_open(side) != 0) || (nickname_attack_hit(side ^ 1) != 0)) {
      slot->state = 0;
    }
    break;
  }
}

void nickname_pt_response2(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_attack_guarded(side ^ 1) && nickname_low6_weak_attack(side ^ 1)) {
      slot->state++;
    }
    break;
  case 1:
    if (nickname_attack_hit(side) || nickname_rob28_bit29(side)) {
      slot->count++;
      slot->state = 0;
    }
    if ((nickname_rob_neutral(side) && !nickname_rob28_600(side)) || nickname_attack_open(side) ||
        nickname_attack_hit(side ^ 1)) {
      slot->state = 0;
    }
    break;
  }
}

void nickname_pt_guard0(uint side, nickname_point *slot)
{
  int r;

  r = nickname_attack_guarded(side ^ 1);
  if (r != 0) {
    slot->count = slot->count + 1;
  }
  return;
}

void nickname_pt_guard1(uint side, nickname_point *slot)
{
  int r;

  r = nickname_attack_guarded(side ^ 1);
  if ((r != 0) && (r = nickname_low_attack_no6(side ^ 1), r != 0)) {
    slot->count = slot->count + 1;
  }
  return;
}

void nickname_pt_rowdy0(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_frame_disadvantage(side) && nickname_rob_quiet(side) && nickname_rob_quiet(side ^ 1) &&
        nickname_attack_no_flag2(side)) {
      slot->state = slot->state + 1;
    }
    break;
  case 1:
    if (nickname_attack_flag2(side) != 0 && nickname_rob28_bit30(side) == 0) {
      slot->count = slot->count + 1;
      slot->state = slot->state + 1;
      break;
    }
    if (nickname_rob_neutral(side) != 0 && nickname_rob28_600(side) == 0) {
      slot->state = 0;
    }
    if (nickname_attack_hit(side ^ 1) != 0) {
      slot->state = 0;
    }
    break;
  case 2:
    if (nickname_rob_neutral(side) != 0 || nickname_attack_no_flag2(side) != 0) {
      slot->state = 0;
      break;
    }
    if (nickname_attack_hit(side ^ 1) != 0) {
      slot->state = 0;
    }
    break;
  }
}

void nickname_pt_rowdy1(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if ((nickname_frame_disadvantage(side) != 0) && (nickname_rob_quiet(side) != 0) &&
        (nickname_rob_quiet(side ^ 1) != 0) && (nickname_attack_no_flag2(side) != 0)) {
      slot->state = slot->state + 1;
    }
    break;
  case 1:
    if ((nickname_attack_flag2(side) != 0) && (nickname_rob28_bit30(side) == 0)) {
      slot->state = slot->state + 1;
    }
    if ((nickname_rob_neutral(side) != 0) && (nickname_rob28_600(side) == 0)) {
      slot->state = 0;
    }
    if (nickname_attack_hit(side ^ 1) != 0) {
      slot->state = 0;
    }
    break;
  case 2:
    if (nickname_attack_hit(side) != 0) {
      slot->count = slot->count + 1;
      slot->state = 0;
    }
    if ((nickname_attack_open(side) != 0) || (nickname_rob_neutral(side) != 0) ||
        (nickname_attack_hit(side ^ 1) != 0)) {
      slot->state = 0;
    }
    break;
  }
}

void nickname_pt_rowdy2(uint side, nickname_point *slot)
{
  switch ((ushort)slot->state) {
  case 0:
    if (nickname_frame_disadvantage(side) && nickname_rob_quiet(side) && nickname_rob_quiet(side ^ 1) &&
        nickname_attack_no_flag2(side)) {
      slot->state = slot->state + 1;
    }
    break;
  case 1:
    if (nickname_attack_flag2(side) && !nickname_rob28_bit30(side)) {
      slot->state = slot->state + 1;
    }
    if (nickname_rob_neutral(side) && !nickname_rob28_600(side)) {
      slot->state = 0;
    }
    if (nickname_attack_hit(side ^ 1)) {
      slot->state = 0;
    }
    break;
  case 2:
    if (nickname_attack_hit(side)) {
      slot->state = slot->state + 1;
    }
    if (nickname_attack_open(side) || nickname_rob_neutral(side) || nickname_attack_hit(side ^ 1)) {
      slot->state = 0;
    }
    break;
  case 3:
    if (nickname_move_bit10_or_7(side ^ 1)) {
      slot->count = slot->count + 1;
    }
    slot->state = 0;
    break;
  }
  return;
}

bool nickname_round_opening(void)
{
  int b = round_time; int a = match_round_time; int r = 0; if (a - b <= 0x78) r = 1; return r;
}

bool nickname_attack_move(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->move_flags & 0x4000) != 0) boolflag_r = 1;
  return boolflag_r;
}

bool nickname_attack_guarded(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if (rob->coli->field_0x94 == '\n') boolflag_r = 1;
  return boolflag_r;
}

bool nickname_attack_hit(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->coli->field_0x3c[0] & 0x281) != 0) boolflag_r = 1;
  return boolflag_r;
}

int nickname_attack_flag2(uint side)
{
  rob_rec *rob;
  int r;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  r = nickname_attack_move(side);
  if ((r != 0) && ((rob->field_0x30 & 2) != 0)) {
    flag = 1;
  }
  return flag;
}

int nickname_attack_open(uint side)
{
  rob_rec *rob;
  int r;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  if ((((rob->field_0x30 & 2) == 0) && (rob->field_0x115 == '\0')) &&
     (r = nickname_attack_move(side), r != 0)) {
    flag = 1;
  }
  if ((((rob->side_flags & 0x20) != 0) &&
      ((rob->old_cur->state & 0x4000) != 0)) &&
     (rob->field_0x114 == '\0')) {
    flag = 1;
  }
  return flag;
}

int nickname_attack_no_flag2(uint side)
{
  int flag;

  rob_rec *rob = rob_get(side & 0xff);
  flag = 0;
  if (((rob->field_0x30 & 2) == 0) && nickname_attack_move(side)) {
    flag = 1;
  }
  return flag;
}

int nickname_rob_neutral(uint side)
{
  rob_rec *rob;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  if ((((rob->state & 1) == 0) &&
      ((rob->old_cur->state & 1) == 0)) && (rob->stun == 0)) {
    flag = 1;
  }
  return flag;
}

int nickname_chicken_test(uint side)
{
  rob_rec *rob;
  int flag;
  uint f;
  uint side_copy;
  int a;
  int b;
  int want_frame;
  int s;
  side_copy = side;
  rob = rob_get(0xff & side_copy);
  flag = 0;
  a = 0x4200;
  b = 0x4400;
  want_frame = 10;
  f = rob->flags28;
  if (((((f & a) == a) || ((f & b) == b)) && ((s, (rob->frame) == want_frame))) && (((rob->side_flags) & 0x40) != 0))
  {
    flag = 1;
  }
  return flag;
}

int nickname_rob28_600(uint side)
{
  rob_rec *rob;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  if (((rob->flags28 & 0x600) != 0) && ((rob->side_flags & 0x20) != 0)) {
    flag = 1;
  }
  return flag;
}

int nickname_combo_damage30(uint side)
{
  rob_rec *rob;
  int flag;
  int lim;
  short a, b;
  rob = rob_get(side & 0xff);
  flag = 0;
  lim = 30;
  a = rob->combo[3].damage;
  b = rob->combo[1].damage;
  if (a < lim) {
    if (b < lim) goto out;
  }
  flag = 1;
out:
  return flag;
}

int nickname_in_combo(uint side)
{
  rob_rec *rob;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  if ((rob->combo[3].hits != 0) || (rob->combo[1].hits != 0)) {
    flag = 1;
  }
  return flag;
}

int nickname_frame_disadvantage(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if (0 < rob->frame_disadvantage) boolflag_r = 1;
  return boolflag_r;
}

int nickname_frame_advantage8(uint side)
{
  rob_rec *rob;
  rob_rec *opp;
  int r;
  int d;

  rob = rob_get(side & 0xff);
  opp = rob_get((side ^ 1) & 0xff);
  r = 0;
  d = 0;
  if ((rob->state & 1) != 0 && rob->stun != 0) {
    d = rob->frame_disadvantage;
  }
  else if ((opp->state & 1) != 0 &&
       ((((opp->move_flags & 0x4000) != 0 && ((opp->state & 4) != 0)) ||
        ((opp->flags2c & 0x20) != 0)))) {
    d = (int)opp->frame - (int)opp->recovery_end;
  }
  if (d <= -8 && (rob->state & 0x1000200) == 0 &&
     ((rob->field_0x30 | rob->opp->field_0x30) & 2) == 0) {
    r = 1;
  }
  return r;
}

bool nickname_rob28_bit29(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->flags28 & 0x20000000) != 0) boolflag_r = 1;
  return boolflag_r;
}

bool nickname_rob28_bit20(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->flags28 & 0x100000) != 0) boolflag_r = 1;
  return boolflag_r;
}

bool nickname_rob28_bit30(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->flags28 & 0x40000000) != 0) boolflag_r = 1;
  return boolflag_r;
}

int nickname_throw_listed(uint side)
{
  rob_rec *rob;
  short *move;
  short move_no;
  int r;

  rob = rob_get(side & 0xff);
  r = 0;
  if ((rob->flags28 & 0x20000000) != 0) {
    move = (short *)nickname_throw_moves[rob->chara];
    move_no = rob->move_no;
    while (*move != 0) {
      if (*move++ == move_no) {
        r = 1;
        break;
      }
    }
  }
  return r;
}

int nickname_rob584_bit20(uint side)
{
  rob_rec *rob;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  if (((rob->old_cur->state & 0x100000) != 0) &&
     ((rob->old_cur->flags28 & 0x8000000) == 0)) {
    flag = 1;
  }
  return flag;
}

int nickname_rob28_bit27(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->flags28 & 0x8000000) != 0) boolflag_r = 1;
  return boolflag_r;
}

int nickname_rob28_bit26(uint side)
{
  rob_rec *rob;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  if (((rob->flags28 & 0x4000000) != 0) &&
     ((rob->flags & 0x30000000) != 0)) {
    flag = 1;
  }
  return flag;
}

int nickname_low6_weak_attack(uint side)
{
  rob_rec *rob;
  int flag;

  rob = rob_get(side & 0xff);
  flag = 0;
  if ((((rob->attack_level == '\x06') && (rob->attack_damage < 0xb)) &&
      ((rob->move_flags & 0x2000) != 0)) && ((rob->attack_mask & 2) != 0)) {
    flag = 1;
  }
  return flag;
}

int nickname_low_attack_no6(uint side)
{
  char c;
  int r;
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  r = 0;
  c = rob->attack_level;
  if (c == 5) { r = 1; }
  else if (c <= 5) { if (c == 3) r = 1; }
  else { if (c == 13) r = 1; }
  return r;
}

int nickname_rob_quiet(uint side)
{
  rob_rec *rob;
  char *ctrl;
  int flag;

  rob = rob_get(side & 0xff);
  ctrl = (char *)control_object_get(side);
  flag = 0;
  if ((((*(int *)(ctrl + 0x48) == 0) && (*(int *)(ctrl + 0x60) == 0)) &&
      (*(int *)(ctrl + 0x6c) == 0)) &&
     (((rob->flags & 0x8000) == 0 && (rob->followup_word == 0)))) {
    flag = 1;
  }
  return flag;
}

int nickname_move_bit10_or_7(uint side)
{
  rob_rec *rob;
  int found;
  int r;
  rob = rob_get(side & 0xff);
  found = move_cmd_arg_find((int)(short)rob->move_no,1,7);
  r = 0;
  if ((rob->move_flags >> 10 & 1) | (found != 0)) r = 1;
  return r;
}

bool nickname_rob318_bit5(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->side_flags & 0x20) != 0) boolflag_r = 1;
  return boolflag_r;
}

bool nickname_rob_out(uint side)
{
  rob_rec *rob;

  rob = rob_get(side & 0xff);
  int boolflag_r = 0;
  if ((rob->flags & 0x100) != 0) boolflag_r = 1;
  return boolflag_r;
}
