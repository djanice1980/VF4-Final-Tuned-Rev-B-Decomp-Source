/*
 * vf4lib/libcard/src/card_item.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_sprite_put_flags_xyz
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar item_bit_table[19][200]; /* 0x0c18a870 */
extern uchar item_lottery_table_set[16]; /* 0x0c18b748 */
extern uchar item_lottery_table_alt[12]; /* 0x0c18b758 */
extern uchar *item_set_reward_lists[19]; /* 0x0c18b7a4 */
extern int item_award_rules[19][3][4]; /* 0x0c18b7f0 */
extern int item_award_thresholds[8][9]; /* 0x0c18bb80 */
extern int item_award_sprites[3][2]; /* 0x0c18bca0 */
extern int game_frame_count; /* 0x0c292540 */
extern uint game_flags; /* 0x0c2925b8 */
extern char rounds_to_win; /* 0x0c2925ce */
extern char round_winner; /* 0x0c2925d2 */
extern char round_number; /* 0x0c2925d8 */
extern int item_lottery_frame; /* 0x0c2a02ec */
extern short item_set_redeem_timer; /* 0x0c2a02f0 */
extern char item_set_redeem_on; /* 0x0c2a02f2 */
extern short item_award_mes_timer[2]; /* 0x0c2a02f6 */
extern float item_plate_icon_x; /* 0x0c2a0300 */
extern uchar item_set_icons; /* 0x0c2a0304 */
extern int item_award_kind[2]; /* 0x0c2a0358 */
extern uchar item_plate_icons; /* 0x0c2a0360 */
extern char costume_colors_changed[2][2]; /* 0x0c431be4 */
extern float item_pending_mark_phase; /* 0x0c431bfc */
extern int item_pending_mark_on; /* 0x0c431c00 */
extern char item_award_mes_on[2]; /* 0x0c2a02f4 */
extern char item_plate_hidden[2]; /* 0x0c2a02fc */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern item_lottery_rec item_lottery[2]; /* 0x0c431bbc */
extern item_pending_mark_rec item_pending_mark; /* 0x0c431c04 */

void item_select_state_clear(int side)
{
  int i;

  costume_colors_changed[side][0] = 0;
  costume_colors_changed[side][1] = 0;
  for (i = 0; i < 4; i++) {
    item_select_state_bytes[side][0][i] = 0;
    item_select_state_bytes[side][1][i] = 0;
  }
  item_select_state_byte[side] = 0;
  item_lottery_clear();
}

void item_lottery_kind1_set(int side)
{
  item_lottery[side].state = 1;
  return;
}

void item_lottery_kind2_set(int side)
{
  item_lottery[side].state = 2;
  return;
}

int item_bit_index(int chara, int item)
{
  uint bit;

  bit = item_bit_table[chara][item];
  if (bit == 0xff) {
    return -1;
  }
  return bit;
}

int item_has(uint side, int item)
{
  card_image_rec *image;
  int chara;
  int bit;

  image = card_image_get(side);
  chara = card_image_char_get(side & 0xff);
  bit = item_bit_index(chara,item);
  if (!(-1 < bit)) {
    return 0;
  }
  return card_hdr_item_test(&image->header,bit & 0xff);
}

int item_give(uint side, int item)
{
  card_image_rec *image = card_image_get(side);
  int bit = item_bit_index(card_image_char_get(side & 0xff), item);
  uint had;
  if (bit < 0) return -1;
  had = card_hdr_item_test(&image->header, bit);
  card_hdr_item_put(&image->header, bit & 0xff, 1);
  if (had != 0) return -1;
  return 0;
}

int item_has_carded(int side, int item)
{
  if (!(card_record_get(side)->in_use != 0)) {
    return 0;
  }
  return item_has(side,item);
}

int item_has_unchecked(int side, int item)
{
  int has;

  card_flags_get(side);
  has = item_has(side,item);
  return has;
}

int item_take(uint side, int item)
{
  card_image_rec *image;
  card_record_rec *record;
  int bit;
  uint had;

  image = card_image_get(side);
  record = card_record_get(side);
  bit = item_bit_index(card_image_char_get(side & 0xff),item);
  if (bit < 0) {
    return -1;
  }
  if (record->in_use == 0) {
    return -1;
  }
  had = card_hdr_item_test(&image->header,bit & 0xff);
  card_hdr_item_put(&image->header,bit & 0xff,0);
  return (had != 0) - 1;
}

int item_give_carded(uint side, int item)
{
  card_image_rec *image;
  card_record_rec *record;
  int bit;
  image = card_image_get(side);
  record = card_record_get(side);
  bit = item_bit_index(card_image_char_get(side & 0xff), item);
  if (bit < 0) return -1;
  if (record->in_use == 0) return -1;
  card_hdr_item_test(&image->header, bit & 0xff);
  card_hdr_item_put(&image->header, bit & 0xff, 1);
  return 0;
}

int item_give_both(uint side, int item)
{
  card_image_rec *image;
  card_record_rec *record;
  int bit;
  unsigned char b;

  image = card_image_get(side);
  record = card_record_get(side);
  card_flags_get(side);
  bit = item_bit_index(card_image_char_get(side & 0xff),item);
  if (bit < 0) return -1;
  b = bit;
  card_hdr_item_test(&image->header,b);
  card_hdr_item_put(&image->header,b,1);
  card_hdr_item_put(&record->header,b,1);
  return 0;
}

uint item_slot_lock_test(int side, uint slot)
{
  card_image_rec *image;
  image = card_image_get(side);
  return image->body.curse.b.curse & 0xf & (1 << slot);
}

int item_slot_lock_match(int side, int costume, uint slot, int item)
{
  card_image_rec *image;
  int result;

  image = card_image_get(side);
  result = image->body.curse.b.curse & 0xf & (1 << slot);
  if (result != 0) {
    result = item_slot_get(side,costume,slot);
    if (result == item) return result;
    return 0;
  }
  return result;
}

void item_slot_unlock(int side, uint slot)
{
  card_image_rec *image;

  image = card_image_get(side);
  image->body.curse.f.slot_lock &= ~(1 << slot);
  return;
}

int item_has_88(int side)
{
  return item_has(side,0x88);
}

int item_has_79(int side)
{
  int has;

  has = item_has(side,0x79);
  return has;
}

int item_has_7a(int side)
{
  int has;

  has = item_has(side,0x7a);
  return has;
}

int item_has_7b(int side)
{
  int has;

  has = item_has(side,0x7b);
  return has;
}

int item_has_7c(int side)
{
  int has;

  has = item_has(side,0x7c);
  return has;
}

int item_has_7d(int side)
{
  int has;

  has = item_has(side,0x7d);
  return has;
}

int item_has_7e(int side)
{
  int has;

  has = item_has(side,0x7e);
  return has;
}

int item_has_7f(int side)
{
  int has;

  has = item_has(side,0x7f);
  return has;
}

int item_has_83(int side)
{
  return item_has(side,0x83);
}

uchar item_slot_get(int side, int costume, int slot)
{
  card_image_rec *image;
  uchar item;

  image = card_image_get(side);
  if (costume == 0) {
    item = image->body.equip_1p[slot];
  }
  else {
    item = image->body.equip_2p[slot];
  }
  return item;
}

void item_equip(int side, int costume, int slot, int item)
{
  card_image_rec *image;

  if ((item_has_carded(side,item) != 0) && (item_slot_lock_test(side,slot) == 0)) {
    image = card_image_get(side);
    if (costume == 0) {
      image->body.misc.f.equip_flag0 = 0;
    }
    else {
      image->body.misc.f.equip_flag1 = 0;
    }
    if (costume == 0) {
      image->body.equip_1p[slot] = (char)item;
    }
    else {
      image->body.equip_2p[slot] = (char)item;
    }
  }
  return;
}

void item_equip_unchecked(int side, int costume, int slot, int item)
{
  card_image_rec *image;
  if (item_has_unchecked(side,item) != 0 && item_slot_lock_test(side,slot) == 0) {
    image = card_image_get(side);
    if (costume == 0) {
      image->body.misc.f.equip_flag0 = 0;
    } else {
      image->body.misc.f.equip_flag1 = 0;
    }
    if (costume == 0) {
      image->body.equip_1p[slot] = (char)item;
    } else {
      image->body.equip_2p[slot] = (char)item;
    }
  }
}

void item_unequip(int side, int costume, int slot)
{
  card_image_rec *image;

  image = card_image_get(side);
  item_slot_unlock(side,slot);
  if (costume == 0) {
    image->body.equip_1p[slot] = 0;
  }
  else {
    image->body.equip_2p[slot] = 0;
  }
  if (costume == 0) {
    image->body.misc.f.equip_flag0 = 0;
  }
  else {
    image->body.misc.f.equip_flag1 = 0;
  }
}

void item_equip_if_flagged(int side, int costume, int slot, int item)
{
  card_image_rec *image;
  image = card_image_get(side);
  if ((costume != 0 || (image->body.misc.w & 1) != 0) && (costume != 1 || (image->body.misc.w & 2) != 0) && item_slot_lock_test(side,slot) == 0) {
    if (costume == 0) image->body.equip_1p[slot] = (uchar)item; else image->body.equip_2p[slot] = (uchar)item;
  }
}

int item_slot_of(int chara, int costume, int item)
{
  int slot;

  slot = chara_item_slot_get(chara,costume,item);
  if (3 < slot) {
    slot = -1;
  }
  return slot;
}

int item_give_and_equip(uint side, int item)
{
  int slot;
  int chara;
  int r;

  card_image_get(side);
  if (item_give_carded(side,item) != 0) {
    r = -1;
  }
  else {
    chara = card_image_char_get(side & 0xff);
    slot = item_slot_of(chara,0,item);
    if (-1 < slot) {
      item_equip(side,0,slot,item);
    }
    slot = item_slot_of(chara,1,item);
    if (-1 < slot) {
      item_equip(side,1,slot,item);
    }
    r = 0;
  }
  return r;
}

int item_take_and_unequip(int side, uint item)
{
  card_image_rec *image;
  int had;
  int slot;

  image = card_image_get(side);
  had = item_take(side,item);
  slot = 0;
  do {
    if (image->body.equip_1p[slot] == item) {
      item_unequip(side,0,slot);
    }
    if (image->body.equip_2p[slot] == item) {
      item_unequip(side,1,slot);
    }
    slot = slot + 1;
  } while (slot < 4);
  return had;
}

int item_card_take(uint side, int item)
{
  card_record_rec *record;
  int chara;
  int bit;
  int had;
  int r;

  record = card_record_get(side);
  chara = card_record_char_get(side & 0xff);
  bit = item_bit_index(chara,item);
  if (bit < 0) {
    r = -1;
  }
  else {
    had = card_hdr_item_test(&record->header,bit & 0xff);
    card_hdr_item_put(&record->header,bit & 0xff,0);
    r = (had != 0) - 1;
  }
  return r;
}

int item_from_bit(uint side, uchar bit)
{
  int found;
  int chara;
  int item;

  found = 0;
  chara = card_image_char_get(side & 0xff);
  item = 0;
  do {
    if (item_bit_table[chara][item] == bit) {
      found = 1;
      break;
    }
    item++;
  } while (item < 200);
  if (!found) item = -1;
  return item;
}

void costume_colors_set(int side, int costume, uint color0, int color1, int color2, int color3)
{
  card_image_rec *image;
  uint word;

  image = card_image_get(side);
  word = color0 | color1 << 5 | color2 << 10 | color3 << 0xf;
  if (costume == 0) image->body.color_1p.w = word;
  else image->body.color_2p.w = word;
  costume_colors_changed[side][costume] = 1;
  if (costume == 0) image->body.misc.f.color_flag0 = 0;
  else image->body.misc.f.color_flag1 = 0;
}

void costume_colors_apply(int side, int costume, uint color0, int color1, int color2, int color3, int hair, uint eye)
{
  card_image_rec *image;
  uint word;

  image = card_image_get(side);
  if ((costume != 0) || ((image->body.misc.w & 0xc) == 0xc)) {
    word = color0 | color1 << 5 | color2 << 10 | color3 << 0xf;
    if (costume == 0) {
      image->body.color_1p.w = word;
      image->body.other_col = costume_hair_eye_pack(0,(uchar)image->body.other_col,hair,eye);
    }
    else {
      image->body.color_2p.w = word;
      image->body.other_col = costume_hair_eye_pack(1,(uchar)image->body.other_col,hair,eye);
    }
  }
  return;
}

void card_costume_colors_set(int side, int costume, uint color0, int color1, int color2, int color3)
{
  card_record_rec *record;
  uint word;

  record = card_record_get(side);
  word = color0 | color1 << 5 | color2 << 10 | color3 << 0xf;
  if (costume == 0) {
    record->body.color_1p.w = word;
  }
  else {
    record->body.color_2p.w = word;
  }
  return;
}

uint costume_hair_eye_pack(int costume, uint old, int hair, uint eye)
{
  uchar b = old;
  uchar nibble;
  int bad = (hair > 2) | ((int)eye > 3);
  if (bad) return 0;
  nibble = hair << 2 | eye;
  if (costume == 0) {
    int low = b & 0xf; return (uchar)(nibble << 4 | low);
  }
  return b & 0xf0 | nibble;
}

uint costume_eye_color_get(int side, int costume)
{
  card_image_rec *image;
  uint bits;

  image = card_image_get(side);
  if (costume == 0) {
    bits = (uint)(int)image->body.other_col >> 4;
  }
  else {
    bits = (uint)image->body.other_col;
  }
  return bits & 3;
}

uint costume_hair_color_get(int side, int costume)
{
  card_image_rec *image;
  uint bits;

  image = card_image_get(side);
  if (costume == 0) {
    bits = (uint)(int)image->body.other_col >> 4;
  }
  else {
    bits = (uint)image->body.other_col;
  }
  return (bits & 0xc) >> 2;
}

int item_set_steal_pick(int taker, int victim)
{
  int taker_any;
  int victim_any;
  int i;
  int found;
  int other;

  card_image_get(taker);
  taker_any = 0;
  for (i = 0x6f; i <= 0x75; i++) {
    if (item_has(taker, i)) { taker_any = 1; break; }
  }
  victim_any = 0;
  for (i = 0x6f; i <= 0x75; i++) {
    if (item_has(victim, i)) { victim_any = 1; break; }
  }
  if ((taker_any == 0) | (victim_any == 0)) return 0;
  found = 0;
  for (i = 0x6f; i <= 0x75; i++) {
    if (item_has(victim, i) && !item_has(taker, i)) { found = i; break; }
  }
  if (found == 0) return 0;
  other = 0;
  for (i = 0x6f; i <= 0x75; i++) {
    if (!item_has(victim, i)) { other = i; break; }
  }
  if (other == 0) return found;
  other = 0;
  for (i = 0x6f; i <= 0x75; i++) {
    if (item_has(taker, i) && !item_has(victim, i)) { other = i; break; }
  }
  if (other == 0) return 0;
  return found;
}

void item_set_steal(int winner, int loser)
{
  int item;
  if (card_record_get(winner)->in_use != 0) {
    item = item_set_steal_pick(winner,loser);
    if (item != 0 && item_take_and_unequip(loser,item) == 0) {
      item_give_and_equip(winner,item);
      card_item_log_reassign((int)(char)loser,(int)(char)item);
    }
  }
}

void item_set_steal_logged(int side, int other)
{
  uint flags;
  int item;

  card_record_get(side);
  flags = card_flags_get(other);
  if (((flags & 0x80) != 0) && (item = item_set_steal_pick(other,side), item != 0)) {
    item_card_take(side,item);
    card_item_log_add((int)(char)side,(int)(char)item);
  }
  return;
}

bool item_lottery_hit_test(int key, int other_key, uint bits, int count, int wins)
{
  int mask;
  mask = (1 << bits) - 1;
  return ((key + wins * 0x9759 + count & mask) + other_key & mask) == 0;
}

byte item_lottery_table_pick(int side, int key, int other_key, int table)
{
  byte *entry;
  int value;
  int mask = 0xff;

  card_image_get(side);
  entry = (byte *)0x0;
  switch (table) {
  case 0:
    entry = item_lottery_table_set;
    break;
  case 1:
    entry = item_lottery_table_alt;
    break;
  }
  if (entry == (byte *)0x0) return 0;
  value = ((key >> 8) + key + other_key + (other_key >> 8)) & mask;
  while (1) {
    if (entry[1] == 0) return 0;
    if (value <= *entry) break;
    entry += 2;
  }
  return entry[1];
}

void empty_fn_0c090280(void)
{
  return;
}

void item_pending_save_mark(void)
{
  user_back_mark_dirty();
  return;
}

int item_pending_time_now(void)
{
  tm t;
  int year, month;
  get_now_tm(&t);
  year = t.tm_year - 100;
  month = t.tm_mon + 1;
  return date_to_minutes(year, month, t.tm_mday, t.tm_hour, t.tm_min);
}

void item_pending_expire(void)
{
  int now;
  int i;
  item_pending_table_rec *table = &user_back_data.item_pending_table;
  item_pending_entry_rec *entries = table->entries;

  now = item_pending_time_now();
  for (i = 0; i < table->count; i++) {
    if (now - entries[i].time > 0xb3f) {
      entries[i].time = 0;
      entries[i].card_id = 0;
      entries[i].count = 0;
    }
  }
  table->count = item_pending_sort(entries);
  if (table->count == 0) {
    table->last_card_id = 0xffffffff;
    table->last_card_hits = 0;
  }
  item_pending_save_mark();
}

int item_pending_count(void)
{
  return user_back_data.item_pending_table.count;
}

int item_pending_redeem(int side)
{
  item_pending_entry_rec *entries;
  item_pending_table_rec *table;
  int item;
  uint id;
  table = &user_back_data.item_pending_table;
  entries = table->entries;
  if (table->count == 0)
  {
    table->last_card_id = -1;
    table->last_card_hits = 0;
    return 0;
  }
  if (card_record_get(side)->in_use == 0)
  {
    table->last_card_id = -1;
    table->last_card_hits = 0;
    return 0;
  }
  id = card_hdr_card_id_get(&card_image_get(side)->header);
  if (table->last_card_id != id)
  {
    table->last_card_id = id;
    table->last_card_hits = 0;
    return 0;
  }
  if ((++table->last_card_hits) < 10)
  {
    return 0;
  }
  item = item_set_first_missing(side);
  if (item == 0)
  {
    table->last_card_hits = 10;
    return 0;
  }
  item_give(side, item);
  entries[table->count - 1].time = 0;
  entries[table->count - 1].card_id = 0;
  entries[table->count - 1].count = 0;
  table->count = item_pending_sort(entries);
  table->last_card_id = -1;
  table->last_card_hits = 0;
  return item;
}

int item_set_first_missing(int side)
{
  int item;
  int missing = 0;

  for (item = 0x6f; item < 0x76; item++) {
    if (item_has(side,item) == 0) {
      missing = item;
      break;
    }
  }
  return missing;
}

void item_pending_add(int card_id)
{
  item_pending_table_rec *table = &user_back_data.item_pending_table;
  item_pending_entry_rec *entries = table->entries;
  if (table->count > 9) table->count--;
  entries[table->count].time = item_pending_time_now();
  entries[table->count].card_id = card_id;
  entries[table->count].count = 1;
  table->count = item_pending_sort(entries);
  if (table->last_card_id != card_id) {
    table->last_card_id = card_id;
    table->last_card_hits = 0;
  }
  item_pending_save_mark();
}

int item_pending_sort(item_pending_entry_rec *e)
{
  item_pending_entry_rec t;
  int i;
  int j;
  int n;

  for (i = 1; i < 10; i++) {
    for (j = i - 1; j >= 0; j--) {
      if (e[j].time >= e[j + 1].time) break;
      t = e[j];
      e[j] = e[j + 1];
      e[j + 1] = t;
    }
  }
  n = 0;
  for (i = 0; i < 10; i++) {
    int empty = e[i].time == 0;
    n += empty == 0;
  }
  return n;
}

void item_lottery_stage_roll(void)
{
  int i;
  item_lottery_rec *p;

  item_lottery_frame = game_frame_count;
  p = item_lottery;
  for (i = 0; i < 2; i++, p++) {
    switch (p->state) {
    case 0:
      p->roll = item_lottery_roll_if_card(8,i);
      break;
    case 1:
    case 2:
      p->roll = item_lottery_roll_if_card(9,i);
      break;
    default:
      break;
    }
  }
}

void item_lottery_judge(int side)
{
  item_lottery_rec *lottery = &item_lottery[side];
  card_image_rec *image = card_image_get(side);
  int item;

  switch (lottery->state) {
  case 0:
    item = item_lottery_roll_if_card(0,side);
    lottery->result = item;
    if (item == 0) return;
    item_pending_add(card_hdr_card_id_get(&image->header));
    break;
  case 1:
  case 2:
    item = item_lottery_roll_if_card(1,side);
    lottery->result = item;
    if (item == 0) return;
    if (item_give(side,item) == 0) return;
    break;
  default:
    return;
  }
  if (image->body.misc.b[2] == 0xff) {
    image->body.misc.b[2] = 0xff;
  }
  else {
    image->body.misc.b[2] = image->body.misc.b[2] + 1;
  }
}

int item_lottery_roll_if_card(int kind, int side)
{
  card_record_rec *record;
  int item;

  record = card_record_get(side);
  if (record->in_use == 0) return 0; item = item_lottery_roll(kind,side,0); if (item == 0) return 0; return item;
}

int item_lottery_roll(uint kind, int side, int table)
{
  int other;
  card_image_rec *images[2];
  card_record_rec *records[2];
  uint bits;
  int count;
  int key;
  int other_key;
  int item;
  uint wins;
  card_image_rec *image;

  other = side == 0;
  images[side] = card_image_get(side);
  images[other] = card_image_get(other);
  records[side] = card_record_get(side);
  records[other] = card_record_get(other);
  kind &= 7;
  if (kind == 0) {
    if (records[other]->in_use == 0) return 0;
  }
  item = 0;
  if (kind == 0) {
    bits = net_rare_value_get(0);
    if (bits - 8 > 0x10) return 0;
  } else if (kind == 1) {
    bits = net_rare_value_get(1);
    if (bits - 2 > 0x16) return 0;
  } else {
    return 0;
  }
  count = net_rare_value_get(2);
  if (count - 1U >= 0xffff) return 0;
  image = images[side];
  wins = image->body.misc.b[2];
  key = image->access_key;
  if (kind == 1) other_key = item_lottery_frame;
  else other_key = images[other]->access_key;
  if (item_lottery_hit_test(key, other_key, bits, count, wins) == 0) return 0;
  item = item_lottery_table_pick(side, key, other_key, table);
  if (item == 0) return 0;
  return item;
}

int item_lottery_result_get(int side)
{
  int item;
  int state;
  item_lottery_rec *lottery;

  lottery = &item_lottery[side];
  item = 0;
  state = lottery->state;
  if (state != 0) {
    if (state >= 0) {
      if (state <= 2) {
        if (lottery->result != 0) item = lottery->result;
        return item;
      }
    }
  }
  return 0;
}

void item_lottery_clear(void)
{
  item_lottery_rec *p;
  uint i;

  p = item_lottery;
  i = 0;
  do {
    p->state = 0;
    p->roll = 0;
    p->field_0x8 = 0;
    p->result = 0;
    p->field_0x10 = 0;
    i = i + 1;
    p++;
  } while (i < 2);
  return;
}

int item_lottery_pending(int side)
{
  item_lottery_rec *lottery;
  int pending;

  lottery = item_lottery + side;
  pending = 0;
  if ((lottery->roll != 0) || (lottery->field_0x8 != 0)) {
    pending = 1;
  }
  return pending;
}

uchar * item_set_reward_list(uint chara)
{
  uchar *list;

  list = (uchar *)0x0;
  if (chara < 0x13) {
    list = item_set_reward_lists[chara];
  }
  return list;
}

int item_set_complete(int side)
{
  int item;
  int has;

  if (!(card_record_get(side)->in_use != 0)) {
    return 0;
  }
  item = 0x6f;
  do {
    has = item_has(side,item);
    if (has == 0) {
      return 0;
    }
    item = item + 1;
  } while (item < 0x76);
  return 1;
}

int item_set_redeem(uint side)
{
  byte *list;
  uint item;
  int i;

  list = (byte *)item_set_reward_list(card_image_char_get(side & 0xff));
  if (list == 0) return -1;
  for (;;) {
    item = *list++;
    if (item == 0) return -1;
    if (item_has(side,item) == 0 && item_give_carded(side,item) == 0) break;
  }
  for (i = 0x6f; i < 0x76; i++)
    item_take_and_unequip(side,i);
  return 0;
}

void item_award_rules_check(int side, int costume)
{
  card_record_rec *record;

  record = card_record_get(side);
  if (record->in_use != 0) {
    item_award_rule_check(side,costume,0);
    item_award_rule_check(side,costume,1);
    item_award_rule_check(side,costume,2);
  }
  return;
}

int item_award_rule_check(uint side, uint costume, int rule_no)
{
  typedef struct { int item; int slot; int mode; int kind; } award_rule;
  typedef struct { int min_level; int unowned_limit[4]; int owned_limit[4]; } award_limits;
  card_image_rec *image;
  award_rule *rule;
  award_limits *limits;
  int item;
  int mode;
  int owned;
  int limit;
  int score;
  int equipped;

  image = card_image_get(side);
  rule = &((award_rule (*)[3])&item_award_rules)[card_image_char_get(side & 0xff)][rule_no];
  limits = (award_limits *)&item_award_thresholds + rule->kind;
  item = rule->item;
  mode = rule->mode;
  owned = 0;
  if (item == -1) return 0;
  if (item_has(side,item) != 0) {
    equipped = item_slot_lock_match(side,costume,rule->slot,item);
    if (equipped == item) owned = 1;
    else return 0;
  }
  limit = rounds_to_win - 2;
  if (limit < 0) limit = 0;
  if (limit > 3) limit = 3;
  score = vs_stat_get(side,rule->kind,owned);
  if (owned == 0) limit = limits->unowned_limit[limit];
  else limit = limits->owned_limit[limit];
  if (score < limit) return 0;
  if (owned == 0) {
    if (round_winner == side) return 0;
    if (image->header.level < limits->min_level) return 0;
    if (((rule->kind == 0) | (rule->kind == 2)) && score <= round_number) return 0;
    if (rule_no == 0) {
      equipped = item_slot_get(side,costume,rule->slot);
      if (item_slot_lock_test(side,rule->slot) != 0 && equipped != 0) item_take_and_unequip(side,equipped);
    }
    else {
      if (item_slot_lock_test(side,rule->slot) != 0) return 0;
    }
    if (mode == 3) {
      item_unequip(side,costume,1);
      item_unequip(side,costume ^ 1,1);
    }
    item_give_and_equip(side,item);
    image->body.curse.f.slot_lock |= mode;
    return 1;
  }
  if (round_winner != side) return 0;
  item_take_and_unequip(side,item);
  if (rule_no != 0) item_give(side,item);
  if (mode == 3) {
    item_unequip(side,costume,1);
    item_unequip(side,costume ^ 1,1);
  }
  return 2;
}

void item_set_take_all(void)
{
  int item;

  item = 0x6f;
  do {
    item_take_and_unequip(0,item);
    item_take_and_unequip(1,item);
    item = item + 1;
  } while (item < 0x76);
  return;
}

void item_set_rewards_give_all(void)
{
  int item;
  int side;

  side = 0;
  do {
    item = 0x6f;
    do {
      item_give_and_equip(side,item);
      item = item + 1;
    } while (item < 0x76);
    item = item_set_redeem(side);
  } while ((item != -1) || (side = side + 1, side < 2));
  return;
}

void item_plate_hidden_clear(void)
{
  item_plate_hidden[0] = 0;
  item_plate_hidden[1] = 0;
  return;
}

void item_plate_hidden_set(int side, int item)
{
  item_plate_hidden[side] = (char)item;
  return;
}

void item_award_mes_reset(void)
{
  item_award_mes_on[0] = 0;
  item_award_mes_on[1] = 0;
  item_set_redeem_on = 0;
  return;
}

int item_award_mes_start(int side, int kind)
{
  if (item_set_redeem_on == 1) return 0;
  if (item_award_mes_on[side] == 1) return 0;
  item_award_kind_set(kind, side);
  item_award_mes_timer[side] = 0;
  item_award_mes_on[side] = 1;
  return 1;
}

bool item_award_mes_active(int side)
{
  return item_award_mes_on[side] != '\0';
}

void item_award_mes_draw(int side)
{
  typedef struct { int sprite; int kind; } award_sprite;
  int t;
  int i;
  award_sprite *e;
  float alpha;
  float scale;

  if (item_award_mes_on[side] == '\0') {
    return;
  }
  t = item_award_mes_timer[side];
  if (t < 120) {
    scale = (float)t * 2.0f / 120.0f;
  }
  else if (t < 156) {
    scale = (float)(t - 120) * 3.2f / 36.0f + 0.8f;
  }
  else if (t < 180) {
    scale = (float)(t - 156) * 8.0f / 24.0f + 4.0f;
  }
  else if (t < 192) {
    scale = (1.0f - (float)(t - 192) / 24.0f) * 12.0f;
  }
  else {
    scale = 0.0f;
  }
  if (t <= 139) alpha = 0.9f;
  else if (t <= 179) {
    if ((t & 4) != 0) alpha = 0.99f;
    else alpha = 0.7f;
  }
  else alpha = 0.9f;
  sprite_next_blend_set(0);
  sprite_put_xyz_scaled_alpha_flags(0x970,(float)(side * 0x168 + 0x8c),410.0f,0.02f,scale,scale,alpha,0x100a);
  sprite_next_blend_normal();
  if (t > 125) {
    alpha = (float)(t - 126) / 18.0f;
    if (alpha > 1.0f) {
      alpha = 1.0f;
    }
    for (i = 0; ; i++) {
      e = &((award_sprite *)item_award_sprites)[i];
      if (e->sprite == -1) break;
      if (item_award_kind_get(side) == e->kind) {
        sprite_put_xyz_alpha(e->sprite,(float)(side * 0x168 + 0x6c),394.0f,0.03f,alpha);
        break;
      }
    }
  }
  if (pause_frame_held() == 0) {
    item_award_mes_timer[side] = item_award_mes_timer[side] + 1;
  }
  return;
}

int item_set_redeem_start(int side)
{
  item_set_redeem_on = 0;
  if (item_set_complete(side) == 0) return 0;
  if (item_set_redeem(side) != 0) return 0;
  item_set_redeem_timer = 0;
  item_set_redeem_on = 1;
  return 1;
}

bool item_set_redeem_active(void)
{
  return item_set_redeem_on != '\0';
}

void item_set_redeem_draw(void)
{
  typedef struct { int sprite; float x; float y; } set_icon;
  typedef struct { int sprite; int kind; } award_sprite;
  int t;
  char i;
  int k;
  float alpha;
  float scale;
  float flash;
  set_icon *icon;
  award_sprite *award;

  if (item_set_redeem_on == 0) return;
  t = item_set_redeem_timer;
  if (t < 0x84) {
    if (t < 0x3c) {
      for (i = 0; i < 7; i++) {
        if (t - i * 6 > 0) {
          alpha = ((float)t - (float)(i * 6)) / 12.0f;
          if (alpha > 1.0f) alpha = 1.0f;
          scale = (((float)t - (float)(i * 6)) / 10.0f) * 10.0f + 2.0f;
          set_icon *icons = (set_icon *)&item_set_icons;
          icon = &icons[i];
          sprite_put_xyz_alpha_flags(icon->sprite, icon->x, icon->y, 0.03f, alpha, 10);
          if (alpha < 1.0f) {
            sprite_next_blend_set(0);
            sprite_put_xyz_scaled_alpha_flags(0x970, icon->x, icon->y, 0.02f, scale, scale, 1.0f - alpha, 0x100a);
            sprite_next_blend_normal();
          }
        }
      }
    }
    else {
      for (i = 0; i < 7; i++) {
        item_set_icon_fly_draw(t, (short)(i * 8 + 0x3c), i);
      }
    }
  }
  if (t < 0x78) {
    float ft = (float)t;
    scale = (ft + ft) / 120.0f;
  }
  else if (t < 0x9c) {
    scale = ((float)(t - 0x78) * 3.2f) / 36.0f + 0.8f;
  }
  else if (t < 0xb4) {
    scale = ((float)(t - 0x9c) * 8.0f) / 24.0f + 4.0f;
  }
  else if (t < 0xc0) {
    scale = (1.0f - (float)(t - 0xc0) / 24.0f) * 12.0f;
  }
  else {
    scale = 0.0f;
  }
  if (t <= 0x8b) flash = 0.9f;
  else if (t < 0xb4) {
    if (t & 4) flash = 0.99f;
    else flash = 0.7f;
  }
  else flash = 0.9f;
  sprite_next_blend_set(0);
  sprite_put_xyz_scaled_alpha_flags(0x970, 320.0f, 416.0f, 0.02f, scale, scale, flash, 0x100a);
  sprite_next_blend_normal();
  if (t > 0x7d) {
    alpha = (float)(t - 0x7e) / 18.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    item_award_kind_set(0, 0);
    item_award_kind_set(0, 1);
    for (k = 0; ; k++) {
      award = &((award_sprite *)item_award_sprites)[k];
      if (award->sprite == -1) break;
      if (item_award_kind_get(0) == award->kind) {
        sprite_put_xyz_alpha(award->sprite, 288.0f, 400.0f, 0.03f, alpha);
        break;
      }
    }
  }
  if (pause_frame_held() == 0) item_set_redeem_timer = item_set_redeem_timer + 1;
}

void player_plate_item_icons_draw(int side)
{
  typedef struct { short item; short sprite; } plate_icon;
  float x;
  float step;
  float y;
  float z;
  int flags;
  char i;
  plate_icon *icon;

  if (side == 1) {
    x = item_plate_icon_x + 320.0;
    step = -1.0;
    flags = 7;
  } else {
    x = 320.0 - item_plate_icon_x;
    step = 1.0;
    flags = 5;
  }
  y = 12.0;
  z = 0.05;
  for (i = 0; ((plate_icon *)&item_plate_icons)[i].item != -1; i++) {
    if ((mode_get_sub() != 0xd || (game_flags & 0x400) == 0 || (icon = &((plate_icon *)&item_plate_icons)[i], icon->item != item_plate_hidden[side]))
        && item_has_carded(side, (icon = &((plate_icon *)&item_plate_icons)[i])->item) != 0) {
      sprite_put_flags_xyz(icon->sprite, x, flags, y + 4.0, i * 0.001 + z);
      x -= step * 16.0;
    }
  }
}

int item_award_kind_get(int side)
{
  return item_award_kind[side];
}

void item_award_kind_set(int kind, int side)
{
  item_award_kind[side] = kind;
  return;
}

void item_set_icon_fly_draw(int frame, int start, int icon_no)
{
  typedef struct { int sprite; float x; float y; } set_icon;
  short now = frame;
  short begin = start;
  int n = (short)icon_no;
  set_icon *icon;
  float off;
  float alpha;
  float dir;
  int i;
  char *ofs;

  if ((float)now < (float)begin + 16.0) {
    if (now < begin) {
      off = 0.0;
      alpha = 1.0;
      sprite_put_xyz_alpha_flags(((set_icon *)&item_set_icons)[n].sprite, ((set_icon *)&item_set_icons)[n].x + off,
                   ((set_icon *)&item_set_icons)[n].y, 0.03, alpha, 10);
    }
    else {
      ofs = (char *)(n * 12);
      off = (320.0 - *(float *)(ofs + (int)&((set_icon *)&item_set_icons)->x)) * ((float)(now - begin) * 0.0625);
      alpha = 0.7;
      if (320.0 > *(float *)(ofs + (int)&((set_icon *)&item_set_icons)->x)) dir = -1.0;
      else dir = 1.0;
      for (i = 0; i < 4; i++) {
        icon = &((set_icon *)&item_set_icons)[n];
        sprite_put_xyz_alpha_flags(icon->sprite, icon->x + off + 4.0 * dir * (float)i, icon->y, (float)i * 0.0001 + 0.03, alpha / (float)(i + 1), 10);
      }
    }
  }
}

void item_pending_mark_init(uint sprite, float x, float y)
{
  int count;

  item_pending_mark_phase = 0.0;
  item_pending_mark_on = 0;
  count = item_pending_count();
  if (count != 0) {
    item_pending_mark_on = 1;
  }
  item_pending_mark.sprite = sprite;
  item_pending_mark.x = x;
  item_pending_mark.y = y;
  return;
}

void item_pending_mark_update(void)
{
  float step;
  step = 1.0;
  if (item_pending_mark_on != 0) {

    if (8 < user_back_data.item_pending_table.last_card_hits) {
      step = 2.5;
    }
    item_pending_mark_phase = step + item_pending_mark_phase;
    if (480.0 <= item_pending_mark_phase) {
      item_pending_mark_phase = 0.0;
    }
  }
}

void item_pending_mark_draw(void)
{
  float dx;
  float dy;
  float angle;
  angle = (item_pending_mark_phase / 480.0) * 6.2831855;
  if (item_pending_mark_on != 0) {
    dx = sin(angle) * 20.0;
    dy = sin(angle + angle) * 10.0;
    sprite_put_flags_xyz(item_pending_mark.sprite,dx + item_pending_mark.x,0x100a,dy + item_pending_mark.y,0.09);
  }
}
