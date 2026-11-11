/*
 * vf4lib/librobdisp/src/rob_item.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern item_test_names_1p_rec item_test_names_1p; /* 0x0c2b1470 */
extern item_test_names_2p_rec item_test_names_2p; /* 0x0c2b1480 */
extern item_test_counts_1p_rec item_test_counts_1p; /* 0x0c2b1490 */
extern item_test_counts_2p_rec item_test_counts_2p; /* 0x0c2b14a0 */
extern undefined format_02d; /* 0x0c21b3c8 */
extern item_anim_entry item_anim_defs[35]; /* 0x0c2b12cc */
extern undefined chara_item_table; /* 0x0c2dd3d8 */
extern char rob_item_flag8[2]; /* 0x0c2dd5bc */
extern char option_item_names[128]; /* 0x0c39be00 */
extern int chara3_item_values[2]; /* 0x0c495ef0 */
extern rob_item_slot rob_item_slots[2][4]; /* 0x0c495ef8 */
extern rob_item_model rob_item_models[2][4]; /* 0x0c495f58 */
extern rob_item_env rob_item_envelopes[2][4]; /* 0x0c495ff8 */
extern card_mark_rec card_marks[2]; /* 0x0c4960b8 */
extern item_anim_rec item_anims[2][4]; /* 0x0c4961f0 */
extern int item_anim_count; /* 0x0c4964f0 */
extern int item_anim_snap; /* 0x0c4964f4 */
extern int rob_card_item_lists[2][4]; /* 0x0c4964f8 */
extern card_mark_motion_rec card_mark_motion; /* 0x0c1a78c8 */
extern rob_body_vram_sides rob_body_vram_used; /* 0x0c292750 */
extern rob_items_vram_sides rob_items_vram_used; /* 0x0c292758 */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern int card_mark_models[15]; /* 0x0c1a78d8 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern int chara3_item_defaults[2]; /* 0x0c2b1120 */
extern char side_entry_flags[3]; /* 0x0c370c84 */

static inline int dbg_slot_get(void *table, int side, int slot) /* an inline getter, kept: written out by hand, rob_items_from_part_test loses two instructions and its loads reorder, not just a register swap, so the original most likely had a helper like this here */
{
  return ((int *)table)[side * 4 + slot];
}

void rob_item_slots_init(void)
{
  rob_rec *rob;
  int side;
  
  rob = rob_get(0);
  side = 0;
  do {
    rob->items = (void *)rob_item_slots[side];
    rob_items_clear(rob);
    side = side + 1;
    rob = rob + 1;
  } while (side < 2);
  return;
}

void rob_items_clear(rob_rec *rob)
{
  rob_item_slot *slot;
  char *flag8;
  int i;
  
  slot = (rob_item_slot *)rob->items;
  for (i = 0; i < 4; i++, slot++) {
    slot->flags = 0;
    slot->card_kind = slot->kind = 0;
    slot->data = 0;
    slot->card_data = 0;
  }
  flag8 = rob_item_flag8;
  flag8[rob->side] = 0;
  card_mark_reset(&card_marks[rob->side]);
  for (i = 0; i < 4; i++) {
    item_anim_reset(&item_anims[rob->side][i]);
  }
  return;
}

void rob_items_setup(int side_arg, int chara_arg, int costume_arg)
{
  int side = (char)side_arg;
  int chara = (char)chara_arg;
  int costume = (char)costume_arg;
  rob_rec *rob;
  rob_item_slot *slot;
  card_record_rec *record;
  card_image_rec *image;
  chara_item_row *row;
  char *own;
  byte *equip;
  char *entries;
  int i;
  int no_face_item, have_item_50_64;
  chara_item_entry *entry;
  uint card_in_use;
  int card_on;
  char item_flags;
  int entry_flag;
  int skip;
  uint index;

  rob = rob_get((uchar)side);
  slot = (rob_item_slot *)rob->items;
  record = card_record_get(side);
  image = card_image_get(side);
  rob_items_clear(rob);
  if ((rob_item_flags & 1) == 0) return;
  row = &((chara_item_row *)&chara_item_table)[chara];
  own = row->own_items[costume];
  if (own != 0) {
    for (i = 0; i < 4; i++, slot++, own += 8) {
      if (*own != -1) {
        slot->flags |= 1;
        slot->kind = *own;
        slot->data = *(int *)(own + 4);
      }
    }
    slot = (rob_item_slot *)rob->items;
  }
  if ((rob_item_flags & 2) != 0) card_in_use = 1;
  else card_in_use = (uchar)record->in_use;
  {
    char *t = (char *)side_entry_flags;
    entry_flag = (t[side] >> 2) & 1;
    card_on = (entry_flag != 0) & (card_in_use != 0);
  }
  if (card_on) {
    if ((rob_item_flags & 2) != 0) {
      if (side == 0) equip = &item_1p_0;
      else equip = &item_2p_0;
    } else {
      if (chara_costume_swap[chara] != costume) equip = image->body.equip_2p;
      else equip = image->body.equip_1p;
    }
    entries = row->entries;
    if (entries != 0) {
      no_face_item = 0;
      have_item_50_64 = 0;
      for (i = 0; i < 4; i++, slot++, equip++) {
        skip = i == 1;
        skip &= no_face_item;
        if (skip) continue;
        if (*equip == 0) continue;
        if ((rob_item_flags & 2) != 0 && *equip == 0xff) {
          slot->flags &= 0xfe;
          continue;
        }
        index = (byte)(*equip - 1);
        if ((int)index >= row->count) continue;
        entry = (chara_item_entry *)(entries + index * 12);
        if (entry->kind[costume] == -1) continue;
        if (entry->data[costume] == 0) continue;
        if ((rob_item_flags & 4) != 0 && entry->slot != i) continue;
        if (item_id_is_50_or_64(*equip)) {
          if (have_item_50_64) continue;
          have_item_50_64 = 1;
        }
        slot->flags |= 2;
        slot->card_item = *equip;
        slot->card_kind = entry->kind[costume];
        slot->card_data = entry->data[costume];
        item_flags = 0;
        if (slot->card_kind == 0) goto rd;
        if (slot->card_kind == 1) {
        rd:
          item_flags = *(char *)slot->card_data;
        }
        if ((item_flags & 1) == 0) slot->flags &= 0xfe;
        if (i == 0 && (item_flags & 4) != 0) no_face_item = 1;
        if ((item_flags & 8) != 0) {
          char *t = rob_item_flag8;
          t[side] = 1;
        }
      }
    }
    slot = (rob_item_slot *)rob->items;
  }
  if (chara == 3) {
    slot = slot + 2;
    if ((*(int *)slot & 0xff0002) == 0x20002) {
      chara3_item_values[costume] = *(int *)(slot->card_data + 4);
    } else {
      chara3_item_values[costume] = chara3_item_defaults[costume];
    }
  }
  item_anims_setup(rob, chara, costume);
}

void rob_items_update(rob_rec *rob)
{
  typedef struct { unsigned char own : 1; unsigned char card : 1; unsigned char : 6; unsigned char kind; unsigned char card_kind; unsigned char pad; int data; int card_data; } slot_bits;
  slot_bits *slot;
  int i;
  int value;

  slot = (slot_bits *)rob->items;
  mtx_push(0);
  for (i = 0; i < 4; i++, slot++) {
    if (*(unsigned char *)slot & 1) {
      if (slot->kind == 1) {
        value = slot->data; value = *(int *)(value + 4);
        if (value >= 0) rob_model_envelope_calc(rob, value);
      }
    }
    if (slot->card && slot->card_kind == 1) {
      value = slot->card_data; value = *(int *)(value + 4);
      if (value >= 0) rob_model_envelope_calc(rob, value);
    }
  }
  card_mark_update(rob,&card_marks[rob->side]);
  mtx_pop(1);
}

void rob_items_draw(rob_rec *rob)
{
  int shown;
  rob_item_slot *slot;
  int i;
  
  slot = (rob_item_slot *)rob->items;
  if (((&disp_sw)[rob->side] & 2) == 0) {
    mtx_push(0);
    if ((rob->flags & 0x40000000) == 0) {
      i = 0;
      do {
        if ((slot->flags & 1) != 0) {
          rob_item_draw(rob,(int)(char)slot->kind,slot->data);
        }
        if (((slot->flags & 2) != 0) && (shown = rob_item_shown(rob,(uint)slot->card_item), shown != 0)) {
          rob_item_draw(rob,(int)slot->card_kind,slot->card_data);
        }
        i = i + 1;
        slot = slot + 1;
      } while (i < 4);
    }
    card_mark_draw(&card_marks[rob->side]);
    mtx_pop(1);
  }
  return;
}

void rob_item_draw(rob_rec *rob, int kind_arg, int data)
{
  float *mtx;
  char kind = kind_arg;
  
  mtx = rob->mtx;
  switch (kind) {
  case 1:
    mtx_peek_pushed(0);
    if (-1 < *(int *)(data + 4)) {
      rob_model_draw_bones(rob,*(int *)(data + 4));
    }
    break;
  case 0:
    mtx_peek_pushed(0);
    mtx_mul_by(mtx + *(int *)(data + 8) * 0x10);
    obj_draw(*(int *)(data + 4));
    if (*(int *)(data + 0xc) != -1) {
      mtx_peek_pushed(0);
      mtx_mul_by(mtx + *(int *)(data + 0xc) * 0x10);
      mtx_scale_xyz(1.0,1.0,-1.0);
      hdr_or_top_bit(1);
      obj_draw(*(int *)(data + 4));
      hdr_or_top_bit(0);
    }
    break;
  }
}

void rob_item_hook_stub(rob_rec *rob)
{
  return;
}

int rob_item_value_get(rob_rec *rob, int slot_no, int kind_arg, int which)
{
  rob_item_slot *slot;
  char kind;
  int data;
  int n;

  slot = (rob_item_slot *)rob->items;
  slot += slot_no;
  switch (which) {
  case 0:
    if ((slot->flags & 1) == 0) {
      return -1;
    }
    kind = slot->kind;
    data = slot->data;
    break;
  case 1:
    if ((slot->flags & 2) == 0) {
      return -1;
    }
    kind = slot->card_kind;
    data = slot->card_data;
    break;
  default:
    return -1;
  }
  n = kind_arg;
  if ((n != kind) & (n >= 0)) {
    return -1;
  }
  switch (kind) {
  case 0:
    return *(int *)(data + 4);
  case 1:
    return *(int *)(data + 4);
  case 2:
    return *(int *)(data + 4);
  }
  return -1;
}

int * rob_card_item_obj_list(char side, int chara, int costume, int slot_no)
{
  rob_rec *rob;
  int value;
  int *list;
  
  rob = rob_get((byte)side);
  list = rob_card_item_lists[side];
  value = rob_item_value_get(rob,slot_no,-1,1);
  if (value != -1) {
    *list++ = value;
  }
  *list = -1;
  return rob_card_item_lists[side];
}

void rob_item101_head_set(rob_rec *rob)
{
  rob_item_slot *slot;
  chara_item_entry *entry;

  slot = (rob_item_slot *)rob->items;
  entry = (chara_item_entry *)((chara_item_row *)&chara_item_table)[rob->chara].entries;
  if (entry == 0) return;
  entry += 100;
  if (entry->kind[rob->costume] != -1 && entry->data[rob->costume] != 0) {
    slot->flags = (slot->flags & 0xfe) | 2;
    slot->card_item = 101;
    slot->card_kind = entry->kind[rob->costume];
    slot->card_data = entry->data[rob->costume];
  }
}

bool item_id_is_50_or_64(uchar item)
{
  int t = (item == 'P') | (item == 'd');
  int u = t == 0;
  return u == 0;
}

int rob_card_item_is_50_or_64(rob_rec *rob, int slot_no)
{
  rob_item_slot *slot;
  slot = (rob_item_slot *)rob->items;
  slot += slot_no;
  if (((slot->flags & 2) != 0) && (item_id_is_50_or_64(slot->card_item) != 0)) {
    return 1;
  }
  return 0;
}

int chara3_item_value_get(int costume)
{
  return chara3_item_values[costume];
}

int chara_item_slot_get(int chara, uint costume, int item)
{
  char *entry;
  
  if (item <= 0) return -1;
  costume ^= chara_costume_swap[chara];
  entry = ((chara_item_row *)&chara_item_table)[chara].entries;
  if (entry == 0) return -1;
  entry = entry + item * 0xc + -0xc;
  if (entry[costume + 1] < 0 || *(int *)(entry + costume * 4 + 4) == 0) return -1;
  return *entry;
}

void rob_card_mark_set(rob_rec *rob, int mark)
{
  byte side;
  
  side = rob->side;
  card_mark_reset(&card_marks[side]);
  if (mark < 0xf) {
    card_marks[side].mark = (char)mark;
  }
  return;
}

void card_mark_reset(card_mark_rec *mark)
{
  mark->mark = 0xff;
  mark->pos[0] = 0.0;
  mark->pos[1] = 100.0;
  mark->pos[2] = 0.0;
  mark->circle_phase = lcg_next15();
  mark->bob_phase = lcg_next15();
  mark->sway_phase = lcg_next15();
  mark->field_0x8 = lcg_next15();
  mark->spin = lcg_next15();
  mark->spin_phase = lcg_next15();
}

void card_mark_update(rob_rec *rob, card_mark_rec *mark)
{
  card_mark_motion_rec *motion = &card_mark_motion;
  float *head_mtx;
  float target[3];
  if (mark->mark >= 0) {
    mark->circle_phase += motion->circle_step;
    mark->bob_phase += motion->bob_step;
    head_mtx = rob->mtx + 0x30;
    target[0] = head_mtx[12] + motion->amp * cos_s16(mark->circle_phase);
    target[1] = head_mtx[13] + motion->lift + motion->amp * sin_s16(mark->bob_phase);
    target[2] = head_mtx[14] + motion->amp * sin_s16(mark->circle_phase);
    mark->pos[0] = (target[0] - mark->pos[0]) * motion->rate + mark->pos[0];
    mark->pos[2] = (target[2] - mark->pos[2]) * motion->rate + mark->pos[2];
    if (mark->pos[1] < target[1]) mark->pos[1] = target[1];
    else mark->pos[1] = (target[1] - mark->pos[1]) * motion->rate + mark->pos[1];
    mark->sway_phase += 0xc0;
    {
      short s = (short)(int)(sin_s16(mark->sway_phase) * 2048.0);
      short c = (short)(int)(cos_s16(mark->sway_phase) * 2048.0);
      mtx_load_identity();
      mtx_rot_x_s16(c);
      mtx_rot_z_s16(s);
    }
    mtx_store_wordwise(mark->sway_mtx);
    mark->spin_phase += 0x30;
    mark->spin = (short)(int)(sin_s16(mark->spin_phase) * 768.0) + mark->spin;
  }
}

void card_mark_draw(card_mark_rec *mark)
{
  if (-1 < mark->mark) {
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(mark->pos);
    mtx_mul_by(mark->sway_mtx);
    mtx_rot_y_s16(mark->spin);
    obj_draw(0x10000);
    mtx_peek_pushed(0);
    mtx_translate_by_vec3(mark->pos);
    mtx_reset_3x3();
    mtx_rot_y_s16(0x8000);
    obj_draw(card_mark_models[mark->mark]);
  }
  return;
}

void item_anim_snap_start(void)
{
  item_anim_snap = 8;
  return;
}

void item_anim_reset(item_anim_rec *anim)
{
  memset(anim,0,0x60);
  anim->pos[1] = 50.0f;
  item_anim_snap_start();
}

void item_anim_count_clear(void)
{
  item_anim_count = 0;
  return;
}

void item_anim_slot_add(rob_rec *rob, int chara_arg, int costume_arg, int slot_no)
{
  char chara;
  char costume;
  byte *item_p;
  uchar costume_bit;
  int i;
  item_anim_rec *anim;
  rob_item_slot *slot;
  
  chara = (char)chara_arg;
  costume = (char)costume_arg;
  slot = (rob_item_slot *)rob->items;
  slot = slot + slot_no;
  i = 0;
  costume_bit = 1 << costume;
loop:
  {
    if (item_anim_defs[i].chara == chara && (costume_bit & item_anim_defs[i].costume_mask) != 0 &&
        (slot->flags & 2) != 0) {
      item_p = &slot->card_item;
      if (*item_p == item_anim_defs[i].item && item_anim_count < 4) {
        anim = &item_anims[rob->side][item_anim_count];
        item_anim_reset(anim);
        anim->def = item_anim_defs[i].def;
        anim->item = *item_p;
        item_anim_count = item_anim_count + 1;
      }
    }
    i++;
  }
  if (i < 0x23) goto loop;
}

void item_anims_setup(rob_rec *rob, int chara_arg, int costume_arg)
{
  char chara = chara_arg;
  char costume = costume_arg;
  int slot_no;
  
  item_anim_count_clear();
  slot_no = 0;
  do {
    item_anim_slot_add(rob,chara,costume,slot_no);
    slot_no = slot_no + 1;
  } while (slot_no < 4);
  return;
}

void item_dir_to_pitch_yaw(float *dir, short *pitch, short *yaw)
{
  int angle;
  float length;
  float sum;
  float v[3];
  
  v[0] = dir[0];
  v[1] = dir[1];
  v[2] = dir[2];
  if (vec3_normalize(v) < 1.1920929e-07f) {
    *yaw = 0;
    *pitch = 0;
  }
  sum = v[0] * v[0] + v[2] * v[2];
  __asm__ ("fsqrt %0" : "=f"(length) : "0"(sum));
  if (length > 1.1920929e-07f) {
    angle = asin_ang16(v[0] / length);
  }
  else {
    angle = 0;
  }
  *yaw = angle;
  if (v[2] < 0.0f) {
    *yaw = -0x8000 - angle;
  }
  *pitch = -asin_ang16(v[1]);
}

void item_anim_aim(item_anim_rec *anim, item_anim_def *def, float *dir, uchar snap)
{
  short turn[2];
  
  item_dir_to_pitch_yaw(dir,&turn[0],&turn[1]);
  if (snap != 0) {
    anim->aim_pitch = turn[0];
    anim->aim_yaw = turn[1];
  }
  else {
    turn[0] = turn[0] - anim->aim_pitch;
    turn[1] = turn[1] - anim->aim_yaw;
    if (abs(turn[0]) > def->aim_step) {
      turn[0] = turn[0] < 0 ? -def->aim_step : def->aim_step;
    }
    if (abs(turn[1]) > def->aim_step) {
      turn[1] = turn[1] < 0 ? -def->aim_step : def->aim_step;
    }
    anim->aim_pitch = anim->aim_pitch + turn[0];
    anim->aim_yaw = anim->aim_yaw + turn[1];
  }
}

void item_anim_aim_ahead(rob_rec *rob, item_anim_rec *anim, item_anim_def *def, uchar snap)
{
  float mtx_rows[12];
  float point[4];
  float ahead[3];
  float *dir;
  
  ahead[0] = 1.0;
  ahead[1] = 0.0;
  ahead[2] = 0.0;
  mtx_push(rob->mtx + 0x30);
  mtx_translate_by_vec3(ahead);
  mtx_store_wordwise(mtx_rows);
  dir = point;
  mtx_pop(1);
  vec3_sub_from(dir,anim->pos);
  item_anim_aim(anim,def,dir,snap);
}

void item_anim_aim_opponent(rob_rec *rob, item_anim_rec *anim, item_anim_def *def, uchar snap)
{
  float *head_mtx;
  float dir[3];
  head_mtx = rob->opp->mtx + 0x30;
  dir[0] = head_mtx[12];
  dir[1] = head_mtx[13];
  dir[2] = head_mtx[14];
  vec3_sub_from(dir,anim->pos);
  item_anim_aim(anim,def,dir,snap);
}

void item_anim_sway(rob_rec *rob, item_anim_rec *anim)
{
  short *phase;
  short *yaw;
  item_anim_def *def;
  float *mtx;
  float amp;
  float s;

  def = anim->def;
  mtx = rob->mtx;
  phase = &anim->yaw_phase;
  *phase = *phase + 0x30;
  yaw = &anim->yaw;
  amp = (float)def->yaw_amp;
  s = sin_s16((int)*phase);
  *yaw = (short)(int)(s * amp + (float)def->yaw_bias) + *yaw;
  mtx_push(mtx + def->base_slot * 0x10);
  mtx_rot_y_s16(*yaw);
  mtx_store_wordwise(mtx + def->mtx_slot * 0x10);
  mtx_pop(1);
}

void item_anim_follow(rob_rec *rob, item_anim_rec *anim)
{
  item_anim_def *def;
  float *mtx;
  uchar snap;
  float height;
  float min_height;
  float target[3];
  float *tp = target; /* ORIGINAL SHAPE (corroborated 2026-11-10: lens_flare_draw has the same idiom, a local vector reached through a pointer to it, as field.c's coli_segment_hit_any writes float pt[3]; float *o = pt;): the target vector reached through a pointer to it; without it 27 lines */
  int mode_main;
  int mode_sub;
  short tilt_z;
  short tilt_x;

  def = anim->def;
  snap = item_anim_snap > 0;
  mtx_push(0);
  mtx = rob->mtx + def->base_slot * 0x10;
  height = mtx[13] + def->height;
  min_height = stage_cur->center.y + def->floor_height;
  if (height < min_height) {
    height = min_height;
  }
  min_height = (mtx[13] + def->height) - def->bob_height * 0.5;
  if (height < min_height) {
    height = min_height;
  }
  if (snap != 0) {
    anim->pos[0] = mtx[12];
    anim->pos[2] = mtx[14];
    anim->pos[1] = height;
  }
  else {
    anim->circle_phase += def->circle_step;
    anim->bob_phase += def->bob_step;
    tp[0] = mtx[12] + def->circle_radius * cos_s16(anim->circle_phase);
    tp[2] = mtx[14] + def->circle_radius * sin_s16(anim->circle_phase);
    tp[1] = height + def->bob_height * sin_s16(anim->bob_phase);
    anim->pos[0] += (tp[0] - anim->pos[0]) * def->rate_xz;
    anim->pos[2] += (tp[2] - anim->pos[2]) * def->rate_xz;
    anim->pos[1] += (tp[1] - anim->pos[1]) * def->rate_y;
  }
  if (def->aim_step != 0) {
    mode_main = mode_get_main();
    mode_sub = mode_get_sub();
    if ((mode_main == 3 && (mode_sub == 7 || mode_sub == 15)) || (mode_main == 5 && (mode_sub == 1 || mode_sub == 3)) || mode_main == 6) {
      item_anim_aim_ahead(rob,anim,def,snap);
    }
    else {
      item_anim_aim_opponent(rob,anim,def,snap);
    }
  }
  anim->tilt_phase += 0xc0;
  tilt_z = (float)def->tilt_z_amp * sin_s16(anim->tilt_phase) + (float)def->tilt_z_bias;
  tilt_x = (float)def->tilt_x_amp * cos_s16(anim->tilt_phase) + (float)def->tilt_x_bias;
  mtx_load_identity();
  mtx_rot_x_s16(tilt_x);
  mtx_rot_z_s16(tilt_z);
  mtx_store_wordwise(anim->tilt_mtx);
  if (*(int *)&def->yaw_amp == 0) {
    anim->yaw = anim->aim_yaw + 0x8000;
  }
  else {
    anim->yaw_phase += 0x30;
    anim->yaw += (short)((float)def->yaw_amp * sin_s16(anim->yaw_phase) + (float)def->yaw_bias);
  }
  mtx = rob->mtx;
  mtx_load_identity();
  mtx_translate_by_vec3(anim->pos);
  mtx_rot_y_s16(anim->yaw);
  mtx_mul_by(anim->tilt_mtx);
  mtx_scale_xyz(def->scale,def->scale,def->scale);
  mtx_store_wordwise(mtx + def->mtx_slot * 0x10);
  mtx_pop(1);
}

void rob_item_anims_update(rob_rec *rob, item_anim_rec *anim)
{
  int i;
  
  if (card_marks[rob->side].mark < '\0') {
    i = 0;
    do {
      if (anim->def != (item_anim_def *)0x0) {
        if (!((anim->def->flags & 1) == 0)) {
          item_anim_sway(rob,anim);
        }
        else {
          item_anim_follow(rob,anim);
        }
      }
      i = i + 1;
      anim = anim + 1;
    } while (i < 4);
  }
  return;
}

void item_anims_update(void)
{
  rob_rec *rob;
  uint paused;
  int side;
  
  rob = rob_get(0);
  paused = pause_frame_held();
  if (paused == 0) {
    if (0 < item_anim_snap) {
      item_anim_snap = item_anim_snap + -1;
    }
    side = 0;
    do {
      rob_item_anims_update(rob,item_anims[rob->side]);
      side = side + 1;
      rob = rob + 1;
    } while (side < 2);
  }
  return;
}

int rob_item_shown(rob_rec *rob, uint item_arg)
{
  item_anim_rec *anim;
  int i;
  byte item;

  item = (byte)item_arg;
  if (card_marks[rob->side].mark < 0) return 1;
  anim = item_anims[rob->side];
  for (i = 0; i < 4; i++, anim++) {
    if (anim->def != 0 && anim->item == item) return 0;
  }
  return 1;
}

void rob_items_from_part_test(char side,char chara,char costume)
{
  typedef struct { char flags; char pad; char card_kind; char pad2; char pad3[4]; void *card_data; } item_slot;
  rob_rec *rob;
  item_slot *slot;
  int i;
  int high;
  uint value;
  int table;
  rob_item_env *env;
  rob_item_model *model;

  rob = rob_get((byte)side);
  slot = (item_slot *)rob->items;
  rob_items_clear(rob);
  for (i = 0; i < 4; i++, slot++) {
    if (deb_sw_0c2b104c == 0) {
      high = *(int *)(costume * 0x20 + (int)chara_data_get(chara)[0x12] + 4);
    }
    else {
      table = (int)chara_data_get(chara)[0x12];
      high = *(int *)((costume * 4 + i) * 4 + table + 0x9c);
    }
    value = (high << 16) | dbg_slot_get(&item, side, i);
    if ((int)value >= 0) {
      slot->flags = 2;
      if (dbg_slot_get(&part, side, i) == ENVELOPE) {
        env = &rob_item_envelopes[side][i];
        slot->card_kind = 1;
        slot->card_data = env;
        env->val = value;
        env->x = 0;
        env->y = 0;
        env->z = 0;
        env->r = env->g = env->b = 0xff;
      }
      else {
        model = &rob_item_models[side][i];
        slot->card_kind = 0;
        slot->card_data = model;
        model->val = value;
        model->part = dbg_slot_get(&part, side, i);
        model->m = -1;
        model->r = model->g = model->b = -1;
      }
    }
  }
}

void rob_debug_reload(rob_rec *rob)
{
  char item_chara;
  char item_costume;
  char item_side;
  int free_before;
  int free_body;
  int free_parts;
  void (*items_setup)();
  uint side;
  
  side = (uint)rob->side;
  rob_disp_release(rob);
  objbank_free(side + 5);
  objbank_free(side + 9);
  objbank_free(side + 0xd);
  objbank_free(side + 0xf);
  objbank_free(side + 0x11);
  objbank_free(side + 0x13);
  rob_hand_models_clear(side);
  if (!((rob_item_flags & 2) == 0)) {
    item_side = rob->side;
    item_chara = rob->chara;
    item_costume = rob->costume;
    items_setup = rob_items_setup;
  }
  else {
    item_side = rob->side;
    item_chara = rob->chara;
    item_costume = rob->costume;
    items_setup = rob_items_from_part_test;
  }
  (*items_setup)((int)(char)item_side,(int)(char)item_chara,(int)(char)item_costume);
  rob_color_build_custom((uint)rob->side,(uint)rob->chara,(int)(char)rob->costume);
  free_before = tex_vram_free_total();
  chara_body_load(rob);
  free_body = tex_vram_free_total();
  chara_parts_load(rob);
  free_parts = tex_vram_free_total();
  (&rob_body_vram_used.side0)[side] = free_before - free_body;
  (&rob_items_vram_used.side0)[side] = free_body - free_parts;
  rob_disp_setup(rob);
  return;
}

void rob_pol_send(void)
{
  rob_rec *rob;
  
  rob = rob_get(0);
  rob_debug_reload(rob);
  return;
}

void rob_pol_send2(void)
{
  rob_rec *rob;
  
  rob = rob_get(1);
  rob_debug_reload(rob);
  return;
}

void item_test_draw(void)
{
  if (deb_sw_0c2b104c != 0) {
    item_test_names_1p.slot0 = (uint)objbanks[13].names;
    item_test_names_1p.slot1 = (uint)objbanks[15].names;
    item_test_names_1p.slot2 = (uint)objbanks[17].names;
    item_test_names_1p.slot3 = (uint)objbanks[19].names;
    item_test_names_2p.slot0 = (uint)objbanks[14].names;
    item_test_names_2p.slot1 = (uint)objbanks[16].names;
    item_test_names_2p.slot2 = (uint)objbanks[18].names;
    item_test_names_2p.slot3 = (uint)objbanks[20].names;
    item_test_counts_1p.slot0 = objbanks[13].count;
    item_test_counts_1p.slot1 = objbanks[15].count;
    item_test_counts_1p.slot2 = objbanks[17].count;
    item_test_counts_1p.slot3 = objbanks[19].count;
    item_test_counts_2p.slot0 = objbanks[14].count;
    item_test_counts_2p.slot1 = objbanks[16].count;
    item_test_counts_2p.slot2 = objbanks[18].count;
    item_test_counts_2p.slot3 = objbanks[20].count;
  }
  else {
    item_test_names_1p.slot0 = (uint)objbanks[5].names;
    item_test_names_1p.slot1 = (uint)objbanks[5].names;
    item_test_names_1p.slot2 = (uint)objbanks[5].names;
    item_test_names_1p.slot3 = (uint)objbanks[5].names;
    item_test_names_2p.slot0 = (uint)objbanks[6].names;
    item_test_names_2p.slot1 = (uint)objbanks[6].names;
    item_test_names_2p.slot2 = (uint)objbanks[6].names;
    item_test_names_2p.slot3 = (uint)objbanks[6].names;
    item_test_counts_1p.slot0 = objbanks[5].count;
    item_test_counts_1p.slot1 = objbanks[5].count;
    item_test_counts_1p.slot2 = objbanks[5].count;
    item_test_counts_1p.slot3 = objbanks[5].count;
    item_test_counts_2p.slot0 = objbanks[6].count;
    item_test_counts_2p.slot1 = objbanks[6].count;
    item_test_counts_2p.slot2 = objbanks[6].count;
    item_test_counts_2p.slot3 = objbanks[6].count;
  }
}

void option_menu_names_clear(void)
{
  memset(option_item_names,0,0x80);
  return;
}

void item_name_format(char *out, int chara, int costume, int slot, uint number)
{
  char number_text [32];
  
  if (costume > 0x20 || slot > 3) return;
  strcpy(out,item_chara_names[chara * 2 + costume]);
  strcat(out,"_");
  strcat(out,item_slot_names[slot]);
  strcat(out,"_");
  sprintf(number_text,&format_02d,number);
  strcat(out,number_text);
}

void option_menu_draw(void)
{
  rob_rec *rob;
  uchar *equip;
  uint number;
  uint chara;
  uint costume;
  int item;
  char *entry;
    int slot;
  uint side;
  
  option_menu_names_clear();
  for (side = 0; (int)side < 2; side++) {
    for (slot = 0; slot < 4; slot++) {
      rob = rob_get(side & 0xff);
      chara = (uint)rob->chara;
      costume = (uint)rob->costume;
      entry = ((chara_item_row *)&chara_item_table)[chara].entries;
      if (entry != 0) {
        if (side == 0) equip = &item_1p_0;
        else equip = &item_2p_0;
        item = (byte)equip[slot];
        if (item > 0 && item - 1 < ((chara_item_row *)&chara_item_table)[chara].count) {
          entry += item * 0xc; entry -= 0xc;
          if (*entry == slot) {
            switch (entry[costume + 1]) {
            case 0:
              number = *(byte *)(*(int *)(entry + costume * 4 + 4) + 0x10);
              break;
            case 1:
              number = *(byte *)(*(int *)(entry + costume * 4 + 4) + 0x14);
              break;
            default:
              continue;
            }
            item_name_format(option_item_names + (side * 4 + slot) * 0x10,chara,costume,slot,number);
          }
        }
      }
    }
  }
}

void option_menu_open(void)
{
  rob_item_flags = rob_item_flags | 7;
  rob_color_flags = rob_color_flags | 3;
  return;
}

void opttst_start(void)
{
  itmtst_start();
  return;
}

void opttst_all_start(void)
{
  window_trap();
  itmtst_start();
  return;
}

void option_menu_update(void)
{
  itmtst_step();
  return;
}
