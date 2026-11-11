/*
 * vf4lib/libmotion/src/mot_tools.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern mot_slot_rec mot_slots[4]; /* 0x0c2a0494 */
extern uchar rsmooth_bone_list[23]; /* 0x0c18be68 */
extern uint game_flags; /* 0x0c2925b8 */
extern char *mot_dir; /* 0x0c2a0388 */
extern int *mot_table2; /* 0x0c2a038c */
extern uchar **mothead_table; /* 0x0c2a0490 */
extern float rsmooth_frames; /* 0x0c2a04f4 */
extern char mirror_angle_flips[9][4]; /* 0x0c2a05c4 */
extern char mirror_angle_pairs[11][4]; /* 0x0c2a05e8 */
extern char mirror_pos_pairs[8][4]; /* 0x0c2a0614 */
extern static_heap_rec motcm_load_addr; /* 0x0c36000c */
extern static_heap_rec rob_load_heaps[2][4]; /* 0x0c360028 */
extern rob_rec *rob_eval_rob; /* 0x0c37f944 */
extern mot_channel *mot_channel_pool; /* 0x0c431c18 */
extern int *rob_bone_links_pool; /* 0x0c431c1c */
extern short mot_bind_frames; /* 0x0c431c20 */
extern float rob_yaw_delta_mtx[16]; /* 0x0c446df0 */
extern float rsmooth_root_mtx[2][2][16]; /* 0x0c446e30 */
extern bone_work_rec *rob_eval_bones; /* 0x0c446fd8 */
extern bone_def *rob_eval_defs; /* 0x0c446fdc */
extern float rob_base_mtx[2][16]; /* 0x0c446fe0 */
extern camera_state camera; /* 0x0c370ce0 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern vec3 rob_root_offset; /* 0x0c37f938 */
extern rob_eval_aim_vec rob_eval_aim; /* 0x0c446f8c */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

void rob_bones_init(int first_id, int count, bone_work_rec *bone, bone_def *defs)
{
  bone_work_rec *bones = bone;
  int i;
  int j;
  int nlinks;
  bone_work_rec **links;
  *rob_bone_links_count_ptr = 0;
  for (i = 0; i < count; i++, bone++, defs++) {
    bone->def = defs;
    bone->pos_ch[0] = 0;
    bone->pos_ch[1] = 0;
    bone->pos_ch[2] = 0;
    bone->rot_ch[0] = 0;
    bone->rot_ch[1] = 0;
    bone->rot_ch[2] = 0;
    bone->mtx = 0;
    bone->id = first_id + i;
    nlinks = defs->nchildren;
    if (nlinks > 0) {
      links = bone->children = (bone_work_rec **)rob_bone_links_alloc(nlinks);
      for (j = 0; j < nlinks; j++) {
        links[j] = bones + (defs->children[j] - first_id);
      }
    }
  }
}

void mot_bind(int first_id, int count, bone_work_rec *bones, int motion)
{
  short id = motion;
  int *dir;
  int base;

  dir = (int *)mot_dir_get((short)(id - 1));
  base = mot_bank_base(id);
  mot_bind_frames = *(short *)(*dir + base);
  mot_bind_bone_cursor = (char *)(base + dir[1]);
  if (mot_bind_frames < 0x100) {
    mot_bind_times16_cursor = 0;
    mot_bind_times8_cursor = (uchar *)(base + dir[3]);
  }
  else {
    mot_bind_times8_cursor = 0;
    mot_bind_times16_cursor = (short *)(base + dir[3]);
  }
  mot_bind_count8_cursor = (uchar *)(base + dir[2]);
  if (*mot_bind_count8_cursor != 0) {
    mot_bind_count16_cursor = 0;
    mot_bind_count8_cursor = mot_bind_count8_cursor + 1;
  }
  else {
    mot_bind_count16_cursor = (short *)(mot_bind_count8_cursor + 2);
    mot_bind_count8_cursor = 0;
  }
  mot_bind_codes_cursor = (uchar *)(base + dir[4]);
  mot_bind_values_cursor = base + dir[5];
  mot_bind_channels(first_id,count,bones);
}

void mot_bind_channels(int bone_id, int count, bone_work_rec *bone)
{
  uint flags;
  int i;
  mot_channel **ch;

  *mot_channel_count_ptr = 0;
  for (i = 0; i < count; i++, bone++, bone_id++) {
    ch = bone->pos_ch;
    if ((char)*mot_bind_bone_cursor == bone_id) {
      flags = (uint)(char)mot_bind_bone_cursor[1];
      mot_bind_bone_cursor = mot_bind_bone_cursor + 2;
      mot_channel_init(ch++,flags & 0x40);
      mot_channel_init(ch++,flags & 0x20);
      mot_channel_init(ch++,flags & 0x10);
      mot_channel_init(ch++,flags & 8);
      mot_channel_init(ch++,flags & 4);
      mot_channel_init(ch,flags & 2);
    }
    else {
      *ch++ = 0;
      *ch++ = 0;
      *ch++ = 0;
      *ch++ = 0;
      *ch++ = 0;
      *ch = 0;
    }
  }
}

void mot_channel_init(mot_channel **slot, int present)
{
  int last_key;
  mot_channel *ch;
  int code_bytes;
  int i;

  if (present == 0) {
    *slot = 0;
  }
  else {
    if (mot_bind_count16_cursor != 0) {
      last_key = *mot_bind_count16_cursor++;
    }
    else {
      last_key = *mot_bind_count8_cursor++;
    }
    ch = mot_channel_alloc(1);
    *slot = ch;
    ch->nkeys = last_key + 1;
    ch->cursor = 0;
    ch->cursor_values = 0;
    ch->last_frame = mot_bind_frames - 1;
    ch->times8 = mot_bind_times8_cursor;
    ch->times16 = mot_bind_times16_cursor;
    ch->codes = mot_bind_codes_cursor;
    ch->values = (ushort *)mot_bind_values_cursor;
    i = 0;
    code_bytes = ((ch->nkeys + 1) % 4) != 0;
    code_bytes += (ch->nkeys + 1) / 4;
    for (; i < code_bytes; i++) {
      int *values = (int *)&mot_bind_values_cursor;
      char *value_counts = mot_code_value_counts;
      *values += value_counts[*mot_bind_codes_cursor++] * 2;
    }
    ch->values_end = (ushort *)mot_bind_values_cursor;
    if (mot_bind_times16_cursor != 0) {
      mot_bind_times16_cursor += last_key;
    }
    else {
      mot_bind_times8_cursor += last_key;
    }
  }
}

mot_channel * mot_channel_alloc(int count)
{
  mot_channel *ch;

  ch = *mot_channel_count_ptr + mot_channel_pool;
  *mot_channel_count_ptr = count + *mot_channel_count_ptr;
  return ch;
}

int * rob_bone_links_alloc(int count)
{
  int *links;

  links = *rob_bone_links_count_ptr + rob_bone_links_pool;
  *rob_bone_links_count_ptr = count + *rob_bone_links_count_ptr;
  return links;
}

void mot_dir_set(char *dir)
{
  mot_dir = dir;
  return;
}

char * mot_dir_get(int index)
{
  short idx = (short)index;
  int entry;
  
  if (idx > 0x4000) {
    return mot_dir;
  }
  if (idx > 0x3fff) {
    entry = idx * 0x18;
    entry += (int)"*ADVC_STAGE";
  }
  else {
    entry = (int)mot_dir + idx * 0x18;
  }
  return (char *)entry;
}

void mot_table2_set(int *table)
{
  mot_table2 = table;
  return;
}

int mot_table2_get(int motion)
{
  int idx;
  int entry;

  idx = (short)motion;
  if (idx > 0x4000) {
    return *mot_table2;
  }
  if (idx > 0x3fff) {
    entry = (int)&stage_mot_table2 + idx * 4;
    return *(int *)(entry + 0xfffefffc);
  }
  return mot_table2[idx];
}

mot_channel * mot_channel_pool_set(mot_channel *pool)
{
  mot_channel *old;
  
  old = mot_channel_pool;
  mot_channel_pool = pool;
  return old;
}

int * rob_bone_links_pool_set(int *pool)
{
  int *old;
  
  old = rob_bone_links_pool;
  rob_bone_links_pool = pool;
  return old;
}

ushort mot_bind_frame_count(void)
{
  return mot_bind_frames;
}

void rob_work_buffers_assign(void)
{
  int i;
  int j;
  rob_rec *rob;
  bone_work_rec *bones;
  uchar *rot_mtx;
  uchar *blend_mtx;
  uchar *keep_mtx;
  int rsmooth_mtx;
  uchar *bone_ids;
  for (i = 0; i < 2; i++) {
    rob = rob_get(i & 0xff);
    rob->channel_pool = &rob_channel_buf + i * 0x2340;
    rob->link_pool = &rob_link_buf + i * 0xb8;
    rob->bones = bones = (bone_work_rec *)(&rob_bone_buf + i * 0x13d4);
    rob->face_channel_pool = &rob_face_channel_buf + i * 0x18c0;
    rob->face_link_pool = &rob_face_link_buf + i * 0x80;
    rob->face_bones = (bone_work_rec *)(&rob_face_bone_buf + i * 0xdec);
    rob->frame_parity = 0;
    rob->bone_mtx = (float *)(&rob_bone_mtx_buf + i * 0xf80);
    rob->bone_mtx_prev = (float *)(&rob_bone_mtx_prev_buf + i * 0xf80);
    rob->place_mtx = (float *)(&rob_place_mtx_buf + i * 0x40);
    rob->mtx = (float *)(&rob_slot_mtx_buf + i * 0x28c0);
    rob->face_mtx = (float *)(&rob_face_mtx_buf + i * 0x7c0);
    rot_mtx = &rsmooth_rot_mtx_buf + i * 0x5c0;
    keep_mtx = &rsmooth_keep_mtx_buf + i * 0x5c0;
    blend_mtx = &rsmooth_blend_mtx_buf + i * 0x5c0;
    rsmooth_mtx = (int)rob_load_heaps[i][2].addr;
    bone_ids = rsmooth_bone_list;
    {
      bone_work_rec *bone = bones;
      for (j = 0; j < 0x17; j++) {
        bone[*bone_ids].rot_mtx = (float *)rot_mtx;
        rot_mtx += 0x40;
        bone[*bone_ids].keep_mtx = (float *)keep_mtx;
        keep_mtx += 0x40;
        bone[*bone_ids].blend_mtx = (float *)blend_mtx;
        blend_mtx += 0x40;
        bone[*bone_ids].rsmooth_mtx = (float *)rsmooth_mtx;
        rsmooth_mtx += 0x40;
        bone_ids++;
      }
    }
    rob_bone_matrices_link(rob);
  }
  bytes_0c29265a_clear();
  stage_anim_frame_ptrs_init();
}

void rob_skeleton_init(rob_rec *rob)
{
  typedef struct { int w[16]; } blk;
  byte count;
  unsigned char i;
  blk *bone_mtx;
  blk *mtx;
  bone_def *defs;
  blk *mtx_prev;
  blk *face_mtx;
  blk *ident;

  bone_mtx = (blk *)rob->bone_mtx;
  mtx = (blk *)rob->mtx;
  mtx_prev = (blk *)rob->bone_mtx_prev;
  face_mtx = (blk *)rob->face_mtx;
  ident = (blk *)&skeleton_ident_mtx;
  mtx_push_identity();
  mtx_store_wordwise((float *)ident);
  for (i = 0; i < 0x1f; i++) {
    *bone_mtx = *ident;
    bone_mtx++;
  }
  for (i = 0; i < 0xa2; i++) {
    *mtx = *ident;
    *mtx_prev = *mtx;
    mtx++;
    mtx_prev++;
  }
  for (i = 0; i < 0x1f; i++) {
    *face_mtx = *ident;
    face_mtx++;
  }
  mtx_pop(1);
  face_rig_init(rob);
  defs = rob->rig->bones;
  count = *(byte *)&rob->rig->count;
  rob_pools_select(rob);
  rob_bones_init(0,(uint)count,rob->bones,defs);
  if ((mode_get_main() != MODE_MAIN_ADVERTISE) ||
     ((mode_get_sub() != 8 && (mode_get_sub() != 7)))) {
    rob_move_set(rob,(uint)*rob->move_table);
  }
}

void rob_bone_matrices_link(rob_rec *rob)
{
  bone_work_rec *bones;
  int mtx;
  byte *bone;
  int i;

  mtx = (int)rob->bone_mtx;
  bones = rob->bones;
  bone = bone_mtx_bone_ids;
  i = 0;
  do {
    if (2 < (byte)(*bone + 3)) {
      bones[*bone].mtx = (float *)mtx;
    }
    i = i + 1;
    bone = bone + 1;
    mtx = mtx + 0x40;
  } while (i < 0x1f);
  return;
}

void rob_mot_bind(rob_rec *rob)
{
  byte count;
  int t0;
  int t;
  int base;
  int off;
  count = *(byte *)&rob->rig->count;
  t0 = stopwatch_usec();
  mot_bind(0,(uint)count,rob->bones,(int)(short)rob->move_no);
  t = stopwatch_usec() - t0;
  mot_bind_usec = t;
  if (t > mot_bind_usec_max) {
    mot_bind_usec_max = t;
    mot_bind_usec_max_move = rob->move_no;
  }
  base = mot_bank_base((int)(short)rob->move_no);
  off = mot_table2_get((int)(short)rob->move_no);
  rob->motion = (uchar *)(off + base);
  rob_base_mtx_update(rob,0.033333335);
}

void rob_motion_set(rob_rec *rob)
{
  if (((user_back_data.option_bits & 0x4000) != 0) || (motion_blend_switch == '\0') ||
      (rob->field_0x3b4 > 0) ||
      ((*(uint *)&rob->frame_parity & 0x820000) != 0) ||
      ((rob->state & 0x10200) != 0) ||
      (rob->action.part.type == 7) || (rob->action.part.type == 8) ||
      (mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() == 8) ||
      (mode_get_main() == MODE_MAIN_GAME && (mode_get_sub() == 7 || mode_get_sub() == 0xf)) ||
      (mode_get_main() == MODE_MAIN_OVER && mode_get_sub() == 6)) {
    rob->motion_flags = rob->motion_flags & 0xfffc;
    rob->blend_weight = 0.0;
  }
  else {
    if ((((game_flags & 0x2000) == 0) || ((rob->side_flags & 0x20) != 0)) &&
       (mot_blend_start(rob), (game_flags & 0x2000) != 0)) {
      rob->field_0x578 = rob->field_0x578 | 1;
    }
  }
  rob_pools_select(rob);
  rob_mot_bind(rob);
  if (rob->face_motion != 0) {
    face_mot_bind(rob);
  }
  if ((game_flags & 0x2000) == 0) {
    rob->side_flags = rob->side_flags | 0x60;
  }
  if ((game_flags & 0x2000) == 0) {
    rob->root_offset_y = 0.0;
  }
  return;
}

void rob_pose_eval(rob_rec *rob, bone_work_rec *bones, float time)
{
  short flags;

  rob_eval_rob = rob;
  rob_eval(bones,bone_defs,time);
  flags = rob->motion_flags;
  if ((flags & 4) != 0) {
    rob->motion_flags = flags & 0xfffb;
  }
  if (rob->face_motion != 0) {
    face_rig_eval(rob->face_bones,time);
  }
  return;
}

void rob_skeleton_update(rob_rec *rob)
{
  typedef struct { float x; float y; float z; } v3;
  typedef struct { int w[16]; } blk;
  short heading;
  short smooth_set;
  int smoothing;
  float frame_time;
  float value;
  int smooth_mtx;
  bone_work_rec *bone_tbl;
  bone_work_rec *bone;
  byte *bone_ids;
  int n;
  short yaw;
  vec3 vec;
  v3 dir;
  float sin_y;
  float cos_y;
  int root;
  float *mtx;
  short *slot_ix;
  int i;
  int slot_mtx;

  {
    short *prev = &rob->field_0x3b0;
    short *cur = &rob->heading;
    short last = *prev;
    short now = *cur;
    rob->yaw_delta = last - now;
    *prev = now;
  }
  mtx_push_identity();
  frame_time = rob->frame_pos * 0.033333335;
  if (pause_frame_held() == 0) {
    if ((rob->motion_flags & 1) != 0) {
      int ok = 0;
      if ((game_flags & 0x2000) == 0 || (rob->field_0x578 & 1) != 0) {
        ok = 1;
      }
      if (ok) {
        float *pr = &rob->blend_weight;
        *pr = 1.0 / rob->blend_length + *pr;
        if (*pr > 1.0) {
          rob->motion_flags &= 0xfffe;
          *pr = 1.0;
        }
      }
    }
    smoothing = 0;
    smooth_set = 0;
    if ((user_back_data.option_bits & 0x8000) == 0 && motion_blend_switch != 0 &&
        rob->followup_word != -1 && (rob->motion_flags & 0x100) == 0 &&
        (mode_get_main() != MODE_MAIN_ADVERTISE || mode_get_sub() != 8) &&
        (mode_get_main() != MODE_MAIN_GAME || (mode_get_sub() != 7 && mode_get_sub() != 15)) &&
        (mode_get_main() != MODE_MAIN_OVER || mode_get_sub() != 6) &&
        (float)rob->move_length >= rsmooth_frames + rob->blend_length &&
        (float)rob->move_length < (float)rob->frame + rsmooth_frames) {
      if ((game_flags & 0x2000) != 0) {
        rob_end_stance_load(rob);
        rob->move_stance &= 0xbf;
      }
      smoothing = 1;
      if (rob->stance != 0 && rob->chara == 12) {
        switch (rob->move_stance) {
        case 3:
          if ((rob->state & 0x800000) != 0) smooth_set = 10;
          else smooth_set = 8;
          break;
        case 0:
          smooth_set = 12;
          break;
        case 0x80:
          if (rob->chara == 12) {
            smooth_set = 14;
            break;
          }
        default:
          smoothing = 0;
          break;
        }
      }
      else {
        switch (rob->move_stance) {
        case 0:
          smooth_set = 0;
          break;
        case 0x80:
          smooth_set = 2;
          break;
        case 1:
          smooth_set = 4;
          break;
        case 0x81:
          smooth_set = 6;
          break;
        case 3:
          if ((rob->state & 0x800000) != 0) smooth_set = 10;
          else smooth_set = 8;
          break;
        default:
          smoothing = 0;
          break;
        }
      }
    }
    if (smoothing) {
      rob->motion_flags |= 8;
      if (rob->move_length < rob->frame) value = 1.0;
      else value = 1.0 - (float)(rob->move_length - rob->frame) / rsmooth_frames;
      rob->rsmooth_weight = value;
    }
    else {
      rob->motion_flags &= 0xfff7;
      value = 0.0;
      rob->rsmooth_weight = value;
    }
    if ((rob->motion_flags & 8) != 0) {
      smooth_mtx = (int)rob_load_heaps[rob->side][2].addr;
      bone_tbl = rob->bones;
      bone_ids = rsmooth_bone_list;
      if ((rob->motion_flags & 0x10) != 0) smooth_set++;
      smooth_mtx = smooth_mtx + smooth_set * 0x5c8;
      for (n = 0; n < 23; n++) {
        bone = bone_tbl + *bone_ids++;
        bone->rsmooth_mtx = (float *)(n * 64 + smooth_mtx);
      }
      bone = bone_tbl + 13;
      bone->rsmooth_pos[0] = *(float *)(smooth_mtx + 0x5c0);
      bone = bone_tbl + 21;
      bone->rsmooth_pos[0] = *(float *)(smooth_mtx + 0x5c4);
    }
  }
  rob_bone_matrices_link(rob);
  rob_pose_eval(rob, rob->bones, frame_time);
  heading = rob->heading;
  yaw = -heading;
  vec = rob->place_pos;
  *(short *)&rob->skel_heading = heading;
  if ((rob->flags & 8) != 0) {
    fsca_sincos16(rob->heading, &sin_y, &cos_y);
    {
      int root = (int)rob->bone_mtx;
      float gx = *(float *)(root + 0x70);
      float gz = *(float *)(root + 0x78);
      vec.y = vec.y - *(float *)(root + 0x74);
      vec.x = vec.x - (gx * cos_y - gz * sin_y);
      vec.z = vec.z - (gx * sin_y + gz * cos_y);
    }
  }
  mtx_translate_by_vec3(&vec.x);
  mtx_rot_y_s16(yaw);
  mtx_store_wordwise(rob->place_mtx);
  mtx = rob->bone_mtx;
  slot_mtx = (int)rob->mtx;
  slot_ix = bone_mtx_slots;
  mtx_push(0);
  for (i = 0; i < 31; i++, mtx += 16, slot_ix++) {
    mtx_peek_pushed(0);
    mtx_mul_by(mtx);
    mtx_store_wordwise(mtx);
    if (i < 24 && *slot_ix >= 0) {
      *(blk *)(*slot_ix * 0x40 + slot_mtx) = *(blk *)mtx;
    }
  }
  mtx_pop(1);
  mtx_pop(1);
  root = (int)rob->bone_mtx;
  value = *(float *)(root + 0x70);
  rob->pos.x = value;
  rob->ground_pos.x = value;
  rob->pos.y = *(float *)(root + 0x74);
  rob->ground_pos.y = rob->place_pos.y;
  value = *(float *)(root + 0x78);
  rob->pos.z = value;
  rob->ground_pos.z = value;
  if ((game_flags & 0x2000) == 0) {
    rob->skel_place_pos = rob->place_pos;
    rob->flags_copy = rob->flags;
  }
  vec.x = 1.0;
  vec.y = 0.0;
  vec.z = 0.0;
  dir.x = 0.0;
  dir.y = 0.0;
  dir.z = 1.0;
  mtx_push(0);
  mtx_set_current_wordwise(rob->bone_mtx + 16);
  mtx_rot_y_rad(rob->bone_mtx[17]);
  mtx_store_wordwise(rob->body_mtx);
  mtx_xform_dir(&dir.x, &dir.x);
  mtx_xform_dir(&vec.x, &vec.x);
  rob->body_pitch = asin_ang16(-dir.y);
  rob->body_yaw = atan2_s16(vec.z, vec.x);
  mtx_pop(1);
  eyes_look_at(rob);
}

void rob_pools_select(rob_rec *rob)
{
  mot_channel_pool_set((mot_channel *)rob->channel_pool);
  rob_bone_links_pool_set((int *)rob->link_pool);
  return;
}

void mot_bank_load(uchar slot, uchar bank)
{
  mot_bank *mb;
  mot_slot_rec *e;

  if (bank <= 20) {
    mb = &mot_banks[bank];
    e = &mot_slots[slot];
    perf_color_mark(0xff7f007f);
    load_file(mb->motdat, (void *)e->mot_heap->addr, e->mot_heap->size);
    if (mb->smooth != 0) {
      if (e->smooth_heap != 0) {
        load_file(mb->smooth, (void *)e->smooth_heap->addr, e->smooth_heap->size);
      }
      else {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libmotion/src/mot_tools.c", 0x3eb, "assertion `%s\' failed.\n", "0");
      }
    }
    perf_color_mark(0);
    e->first = mb->first;
    e->end = mb->end;
    e->bank = bank;
  }
}

void mot_bank_load_req(uchar slot, uchar bank)
{
  mot_slot_rec *e;
  mot_bank *m;
  static_heap_rec *q;
  
  if (bank <= 20) {
    m = &mot_banks[bank];
    e = &mot_slots[slot];
    load_req(m->motdat,(void *)e->mot_heap->addr,e->mot_heap->size);
    if (m->smooth != 0) {
      q = e->smooth_heap;
      if (q != 0) {
        load_req(m->smooth,(void *)q->addr,q->size);
      }
      else {
        fatal_at("/p71/prj71p/vf4e2/vf4lib/libmotion/src/mot_tools.c",0x416,
                 "assertion `%s\' failed.\n","0");
      }
    }
    e->first = m->first;
    e->end = m->end;
    e->bank = bank;
  }
}

void mothead_load(uchar slot, uchar bank)
{
  mot_bank *b;
  mot_slot_rec *s;

  if (bank > 20) return;
  b = &mot_banks[bank];
  s = &mot_slots[slot];
  if (bank >= 20) return;
  s->mothead_pending = 1;
  load_file(b->mothead,(void *)s->mothead_heap->addr,s->mothead_heap->size);
  mothead_relocate(slot,bank);
}

void mothead_load_req(unsigned char slot, unsigned char bank)
{
  mot_bank *b;
  mot_slot_rec *e;

  if (bank <= 20) {
    b = &mot_banks[bank];
    e = &mot_slots[slot];
    if (bank < 20) {
      e->mothead_pending = 1;
      load_req(b->mothead,(void *)e->mothead_heap->addr,e->mothead_heap->size);
    }
  }
}

int mot_bank_base(int motion)
{
  short id = motion;
  mot_slot_rec *p = mot_slots;
  uint i;

  if (id > 0x4000) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libmotion/src/mot_tools.c",0x462,"assertion `%s\' failed.\n","0");
    return (int)p->mot_heap->addr;
  }
  if (id >= 0x4000) return 0;
  for (i = 0; i < 4; i++, p++) {
    if (p->first <= id && id < p->end) return (int)p->mot_heap->addr;
  }
  fatal_at("/p71/prj71p/vf4e2/vf4lib/libmotion/src/mot_tools.c",0x46c,"assertion `%s\' failed.\n","0");
  return 0;
}

void mothead_relocate(uint slot_no, uint bank_no)
{
  uchar slot = slot_no;
  uchar bank = bank_no;
  char *pending;
  int base;
  int *dst;
  int *offsets;
  int *e;
  int *list;
  int *group;
  int *entry;
  int addr;
  
  pending = &mot_slots[slot].mothead_pending;
  if (*pending == 0) return;
  *pending = 0;
  dst = (int *)mothead_table;
  if (bank == 0) {
    offsets = (int *)motcm_load_addr.addr;
  }
  else {
    offsets = (int *)mot_slots[slot].mothead_heap->addr;
    dst = dst + mot_banks[bank].first - 1;
  }
  base = (int)offsets;
  for (; *offsets != -1; offsets++, dst++) {
    addr = *offsets;
    if (addr != 0) {
      addr += base;
      *dst = addr;
      e = (int *)addr;
      if (e[2] != 0) {
        addr = e[2]; addr += base;
        e[2] = addr;
        for (list = (int *)addr; *list != 0; list += 2) {
          addr = *list; addr += base;
          *list = addr;
        }
      }
      if (e[3] != 0) {
        addr = e[3]; addr += base;
        e[3] = addr;
        for (list = (int *)addr; *list != 0; list += 2) {
          addr = *list; addr += base;
          *list = addr;
        }
      }
      if (e[4] != 0) {
        addr = e[4]; addr += base;
        e[4] = addr;
        for (group = (int *)addr; *group != 0; group += 2) {
          addr = *group; addr += base;
          *group = addr;
          entry = (int *)addr;
          addr = entry[3]; addr += base;
          entry[3] = addr;
          for (list = (int *)addr; *list != 0; list++) {
            addr = *list; addr += base;
            *list = addr;
          }
        }
      }
    }
  }
}

void rob_end_stance_load(rob_rec *rob)
{
  short s = rob->move_no;
  if (s != 0) {
    rob->move_stance = ((char **)mothead_table)[(short)(s - 1)][5];
  }
}

void eyes_look_at(rob_rec *rob)
{
  vec3 a;
  vec3 b;
  float mid[3];
  vec3 t;
  rob_eye_rec *eye;
  int *q;
  vec3 *pb;
  float *pm;
  vec3 *pt;
  obj_node *r;
  int yaw;
  short pitch;
  short s;
  float d;
  float x;
  float y;
  float z;

  if ((rob->eye_look & 3) != 0) {
    eye = eye_table[rob->chara][rob->costume];
    r = obj_node_get(eye->model_id);
    q = (int *)eye->offset;
    pb = &b;
    pm = mid;
    pt = &t;
    a = *(vec3 *)q;
    if (r != 0) {
      vec3_add_to((float *)&a,r->pos);
    }
    eye++;
    q = (int *)eye->offset;
    *pb = *(vec3 *)q;
    r = obj_node_get(eye->model_id);
    if (r != 0) {
      vec3_add_to((float *)pb,r->pos);
    }
    vec3_weighted_sum((float *)&a,(float *)pb,0.5,pm,0.5);
    if ((rob->eye_look & 2) != 0) {
      if ((rob->opp->flags & 1) == 0) {
        return;
      }
      mtx_push(0);
      mtx_set_current_wordwise(rob->opp->mtx + 48);
      mtx_store_translation(&pt->x);
      mtx_pop(1);
    }
    else {
      q = (int *)&camera.eye.x;
      *pt = *(vec3 *)q;
    }
    mtx_push(0);
    mtx_set_current_wordwise(rob->mtx + 48);
    mtx_translate_by_vec3(pm);
    vec_to_local((float *)pt);
    mtx_pop(1);
    x = pt->x;
    y = pt->y;
    z = pt->z;
    yaw = atan2_s16(z,x);
    pitch = atan2_s16(fsqrt_asm(x * x + z * z),y); pitch -= 0x4000;
    if (pitch > 0x1c00) {
      pitch = 0x1c00;
    }
    else if (pitch < -0x1c00) {
      pitch = -0x1c00;
    }
    if (yaw > 0x1000) {
      yaw = 0x1000;
    }
    else if (yaw < -0x1200) {
      yaw = -0x1200;
    }
    if (pitch < 0) {
      d = (float)pitch / -7168.0;
    }
    else {
      d = (float)pitch / 7168.0;
    }
    yaw = (short)(int)((float)yaw * (1.0 - d));
    rob->eye_angles[1] = -yaw;
    rob->eye_angles[0] = -yaw;
    rob->eye_angles[3] = -pitch;
    rob->eye_angles[2] = -pitch;
  }
}

short mot_id_skip_unloaded(short motion, short prev)
{
  mot_slot_rec *e;
  uint i;
  short result;
  
  e = mot_slots;
  for (i = 0; i < 4; i++, e++) {
    if (e->first <= motion) {
      if (e->end > motion) return motion;
    }
  }
  e = mot_slots;
  if (prev > motion) {
    result = 0;
    if (motion == 0x4000) motion = 0x164c;
    for (i = 0; i < 4; i++, e++) {
      if (motion > e->end - 1) {
        if (result < e->end - 1) result = e->end - 1;
      }
    }
  }
  else {
    result = 0x167c;
    for (i = 0; i < 4; i++, e++) {
      if (e->first > motion) {
        if (e->first < result) result = e->first;
      }
    }
    if (result > 0x164b) result = prev;
  }
  return result;
}

uchar * move_record(int index)
{
  int id;

  id = (short)index;
  if (id > 0x4000) return (uchar *)mothead_table;
  if (id >= 0x4000) return stage_mothead_table[id - 0x4000];
  return mothead_table[id];
}

void mothead_table_set(uchar **table)
{
  mothead_table = table;
  return;
}

int mot_slot_get(int slot)
{
  return (int)&mot_slots[slot];
}

mot_slot_rec * mot_slot_find(int motion)
{
  short id = motion;
  mot_slot_rec *p = mot_slots;
  uint i;

  for (i = 0; i < 4; i++, p++) {
    if (p->first <= id && id < p->end) return p;
  }
  return 0;
}

void rob_bone_pos_blend(bone_work_rec *bones)
{
  rob_yaw_delta_mtx_set();
  bone_pos_blend_walk(bones);
  return;
}

void rob_yaw_delta_mtx_set(void)
{
  rob_rec *rob;
  short s;
  
  rob = rob_eval_rob;
  mtx_push_identity();
  s = rob->yaw_delta;
  if (s != 0) {
    mtx_rot_y_s16(s);
  }
  mtx_store(rob_yaw_delta_mtx);
  mtx_pop(1);
}

void bone_pos_blend_walk(bone_work_rec *bone)
{
  bone_work_rec **children;
  int count;

  if (bone->def->flags & 0x2000) {
    if (rob_eval_rob->motion_flags & 4)
      bone_pos_blend_capture(bone);
    if (rob_eval_rob->motion_flags & 1)
      bone_pos_blend_prev(bone, rob_eval_rob->blend_weight);
    else if (rob_eval_rob->motion_flags & 8)
      bone_pos_rsmooth_blend(bone, rob_eval_rob->rsmooth_weight);
    bone_pos_keep(bone);
  }
  children = bone->children;
  count = bone->def->nchildren;
  while (--count != -1)
    bone_pos_blend_walk(*children++);
}

void bone_pos_keep(bone_work_rec *bone)
{
  typedef struct { int w[3]; } blk;
  blk *src = (blk *)bone->pos;
  blk *dst = (blk *)bone->keep_pos;
  *dst = *src;
  mtx_push_identity();
  if ((rob_eval_rob->state & 0x10) == 0) {
    mtx_translate_by_vec3(&rob_root_offset.x);
  }
  vec_to_local((float *)dst);
  mtx_pop(1);
  return;
}

void bone_pos_blend_capture(bone_work_rec *bone)
{
  uint *src;
  uint *dst;

  src = (uint *)bone->keep_pos;
  dst = (uint *)bone->blend_pos;
  dst[0] = src[0];
  dst[1] = src[1];
  dst[2] = src[2];
  mtx_push(0);
  mtx_set_current_wordwise(rob_yaw_delta_mtx);
  mtx_translate(-rob_eval_rob->root_offset.x,-rob_eval_rob->root_offset_y,
                -rob_eval_rob->root_offset.z);
  vec_to_local((float *)dst);
  mtx_pop(1);
  return;
}

void bone_pos_blend_prev(bone_work_rec *bone, float weight)
{
  float *pos = bone->pos;
  float *from = bone->blend_pos;
  float inv = 1.0 - weight;
  *pos = weight * *pos + *from * inv;
  from++; pos++;
  *pos = weight * *pos + *from * inv;
  from++; pos++;
  *pos = weight * *pos + inv * *from;
}

void rsmooth_targets_set(bone_work_rec *bones_addr)
{
  typedef struct { int w[16]; } mtx_t;
  float v[3];
  bone_work_rec *base;
  bone_work_rec *e;
  float *pos;
  uchar *bone_ids;
  int i;
  mtx_t *root;
  mtx_t *src;
  
  base = bones_addr;
  bone_ids = rsmooth_bone_list;
  mtx_push(0);
  if (rob_eval_rob->motion_flags & 0x5040) {
    mtx_reset_3x3();
  }
  mtx_store_translation(v);
  v[1] = 0.0;
  if ((rob_eval_rob->state & 0x40000400) == 0x40000000) {
    v[1] = v[1] - 0.6;
  }
  mtx_overwrite_translation(v);
  if (rob_eval_rob->motion_flags & 0x5040) {
    if (rob_eval_rob->side_flags & 0x10) {
      mtx_rot_y_s16(rob_eval_rob->facing_offset);
    }
    else {
      mtx_rot_y_s16(-rob_eval_rob->facing_offset);
    }
    if (rob_eval_rob->move_stance == 3) {
      if (rob_eval_rob->state & 0x800000) {
        mtx_rot_x_s16(-0x4000);
      }
      else {
        mtx_rot_x_s16(0x4000);
      }
    }
  }
  else if (rob_eval_rob->motion_flags & 0x8000) {
    if (rob_eval_rob->side_flags & 0x10) {
      mtx_rot_y_s16(0x4000);
    }
    else {
      mtx_rot_y_s16(0xc000);
    }
  }
  else if (rob_eval_rob->motion_flags & 0x400) {
    mtx_rot_y_s16(0x8000);
  }
  mtx_push(0);
  for (i = 0; i < 23; i++) {
    e = &base[*bone_ids++];
    pos = e->rsmooth_pos;
    mtx_set_current_wordwise(e->rsmooth_mtx);
    mtx_store_translation(v);
    mtx_peek_pushed(0);
    mtx_xform_point(v,pos);
  }
  e = &base[2];
  src = (mtx_t *)e->rsmooth_mtx;
  root = &((mtx_t (*)[2])&rsmooth_root_mtx)[rob_eval_rob->side][0];
  *root = *src;
  if (rob_eval_rob->motion_flags & 0x5040) {
    float *w = rsmooth_work_mtx; /* ORIGINAL SHAPE (corroborated 2026-11-10: the bytes need one pseudo for the address in each of the three branches, which local-alloc gets from a block-local pointer; one function-level pointer does not match): block-local pointer to the work matrix (rsmooth_work_mtx is used only here); with the address at every use the diff is 237 lines */
    mtx_set_current_wordwise(base[1].rot_mtx);
    mtx_negate_x_axis();
    mtx_store(w);
    mtx_load_identity();
    if (rob_eval_rob->side_flags & 0x10) {
      mtx_rot_y_s16(-rob_eval_rob->facing_offset);
    }
    else {
      mtx_rot_y_s16(rob_eval_rob->facing_offset);
    }
    mtx_rot_y_s16(0x8000);
    if (rob_eval_rob->move_stance == 3) {
      if (rob_eval_rob->state & 0x800000) {
        mtx_rot_x_s16(-0x4000);
      }
      else {
        mtx_rot_x_s16(0x4000);
      }
    }
    mtx_mul_by(w);
    mtx_store(w);
    mtx_set_current_wordwise(w);
    mtx_mul_by((float *)root);
    mtx_store((float *)root);
    e->rsmooth_mtx = (float *)root;
    e = &base[25];
    src = (mtx_t *)e->rsmooth_mtx;
    root = &((mtx_t (*)[2])&rsmooth_root_mtx)[rob_eval_rob->side][1];
    *root = *src;
    mtx_set_current_wordwise(w);
    mtx_mul_by((float *)root);
    mtx_store((float *)root);
    e->rsmooth_mtx = (float *)root;
  }
  else if (rob_eval_rob->motion_flags & 0x8000) {
    float *w = rsmooth_work_mtx; /* ORIGINAL SHAPE (corroborated 2026-11-10: the bytes need one pseudo for the address in each of the three branches, which local-alloc gets from a block-local pointer; one function-level pointer does not match): block-local pointer to the work matrix (rsmooth_work_mtx is used only here); with the address at every use the diff is 237 lines */
    mtx_load_identity();
    if (rob_eval_rob->motion_flags & 0x8000) {
      if (rob_eval_rob->side_flags & 0x10) {
        mtx_rot_y_s16(0x4000);
      }
      else {
        mtx_rot_y_s16(0xc000);
      }
    }
    else {
      mtx_rot_y_s16(rsmooth_alt_yaw);
    }
    mtx_store(w);
    mtx_set_current_wordwise(w);
    mtx_mul_by((float *)root);
    mtx_store((float *)root);
    e->rsmooth_mtx = (float *)root;
    e = &base[25];
    src = (mtx_t *)e->rsmooth_mtx;
    root = &((mtx_t (*)[2])&rsmooth_root_mtx)[rob_eval_rob->side][1];
    *root = *src;
    mtx_set_current_wordwise(w);
    mtx_mul_by((float *)root);
    mtx_store((float *)root);
    e->rsmooth_mtx = (float *)root;
  }
  else if (rob_eval_rob->motion_flags & 0x400) {
    float *w = rsmooth_work_mtx; /* ORIGINAL SHAPE (corroborated 2026-11-10: the bytes need one pseudo for the address in each of the three branches, which local-alloc gets from a block-local pointer; one function-level pointer does not match): block-local pointer to the work matrix (rsmooth_work_mtx is used only here); with the address at every use the diff is 237 lines */
    mtx_load_identity();
    mtx_rot_y_s16(0x8000);
    mtx_store(w);
    mtx_set_current_wordwise(w);
    mtx_mul_by((float *)root);
    mtx_store((float *)root);
    e->rsmooth_mtx = (float *)root;
    e = &base[25];
    src = (mtx_t *)e->rsmooth_mtx;
    root = &((mtx_t (*)[2])&rsmooth_root_mtx)[rob_eval_rob->side][1];
    *root = *src;
    mtx_set_current_wordwise(w);
    mtx_mul_by((float *)root);
    mtx_store((float *)root);
    e->rsmooth_mtx = (float *)root;
  }
  mtx_pop(2);
}

void bone_pos_rsmooth_blend(bone_work_rec *bone, float weight)
{
  float *dst = bone->pos;
  float *src = bone->rsmooth_pos;
  float inv = 1.0f - weight;
  *dst = weight * *src + *dst * inv;
  src++; dst++;
  *dst = weight * *src + *dst * inv;
  src++; dst++;
  *dst = weight * *src + inv * *dst;
}

void bone_rot_blend_capture(bone_work_rec *bone)
{
  typedef struct { int w[16]; } blk;
  float *src;
  float *dst;
  uint def_flags;
  
  def_flags = bone->def->flags;
  src = bone->keep_mtx;
  dst = bone->blend_mtx;
  if (((def_flags & 0x8000) != 0) &&
     ((rob_eval_rob->state & 0x420000) == 0x400000)) {
    mtx_push(0);
    mtx_set_current_wordwise(src);
    mtx_rot_y_s16(0x8000);
    mtx_store(dst);
    mtx_pop(1);
  }
  else {
    *(blk *)dst = *(blk *)src;
  }
  return;
}

void bone_rot_keep(bone_work_rec *bone)
{
  typedef struct { int w[16]; } blk;
  uint def_flags = bone->def->flags;
  float *dst = bone->keep_mtx;
  float *src = bone->rot_mtx;

  if ((def_flags & 0x40000000) != 0) {
    if ((rob_eval_rob->motion_flags & 0x40) != 0) {
      mtx_push_identity();
      if (rob_eval_rob->facing_offset != 0) {
        mtx_rot_y_s16(-rob_eval_rob->facing_offset);
      }
      if ((rob_eval_rob->state & 0x400) != 0) {
        if ((rob_eval_rob->state & 0x800000) != 0) {
          mtx_rot_z_s16(0xffffc000);
          mtx_rot_y_s16(0x8000);
        }
        else {
          mtx_rot_x_s16(0x4000);
        }
      }
      mtx_mul_by(rob_eval_rob->bones[1].rot_mtx);
      mtx_mul_by(src);
      mtx_store(dst);
      mtx_pop(1);
      return;
    }
    if ((rob_eval_rob->motion_flags & 0x400) != 0) {
      mtx_push_identity();
      mtx_rot_y_s16(0x8000);
      mtx_mul_by(src);
      mtx_store(dst);
      mtx_pop(1);
      return;
    }
  }
  *(blk *)dst = *(blk *)src;
}

void mtx_rot_lerp(float *out,float *mtx_a,float *mtx_b,float weight)
{
  float a[3];
  float b[3];
  float c[3];
  float d[3];
  float *pa = a;
  float *pb = b;
  float *pd = d;
  float *pc = c;
  float inv;
  float l;
  float r1;
  float r2;
  float r3;

  inv = 1.0 - weight;
  pa[0] = out[0] = weight * mtx_a[0] + mtx_b[0] * inv;
  pa[1] = out[4] = weight * mtx_a[4] + mtx_b[4] * inv;
  pa[2] = out[8] = weight * mtx_a[8] + mtx_b[8] * inv;
  l = fsqrt_asm(pa[0] * pa[0] + pa[1] * pa[1] + pa[2] * pa[2]);
  if (l > 1.1920929e-07) {
    r1 = 1.0 / l;
    out[0] = r1 * pa[0];
    out[4] = r1 * pa[1];
    out[8] = r1 * pa[2];
  }
  pc[0] = weight * mtx_a[1] + inv * mtx_b[1];
  pc[1] = weight * mtx_a[5] + inv * mtx_b[5];
  pc[2] = weight * mtx_a[9] + inv * mtx_b[9];
  pd[0] = pa[1] * pc[2] - pa[2] * pc[1];
  pd[1] = pa[2] * pc[0] - pa[0] * pc[2];
  pd[2] = pa[0] * pc[1] - pa[1] * pc[0];
  pb[0] = pd[1] * pa[2] - pd[2] * pa[1];
  pb[1] = pd[2] * pa[0] - pd[0] * pa[2];
  pb[2] = pd[0] * pa[1] - pd[1] * pa[0];
  l = fsqrt_asm(pb[0] * pb[0] + pb[1] * pb[1] + pb[2] * pb[2]);
  if (l > 1.1920929e-07) {
    r2 = 1.0 / l;
    out[1] = r2 * pb[0];
    out[5] = r2 * pb[1];
    out[9] = r2 * pb[2];
  }
  l = fsqrt_asm(pd[0] * pd[0] + pd[1] * pd[1] + pd[2] * pd[2]);
  if (l > 1.1920929e-07) {
    r3 = 1.0 / l;
    out[2] = r3 * pd[0];
    out[6] = r3 * pd[1];
    out[10] = r3 * pd[2];
  }
}

void bytes_0c29265a_clear(void)
{
  mot_flag_a = 0;
  mot_flag_b = 0;
  return;
}

void mot_blend_start(rob_rec *rob)
{
  float length;
  if ((game_flags & 0x2000) != 0) {
    rob_move_byte4_load(rob);
  }
  if ((rob->flags & 0x100) != 0) {
    length = 10.0f;
  }
  else if (((rob->move_flags & 0x4000) != 0) ||
       ((rob->flags & 0x4000000) != 0)) {
    length = 5.0f;
  }
  else if (rob->move_no == rob->old_cur->move_no) {
    length = 4.0f;
  }
  else {
    length = 7.0f;
  }
  rob->blend_length = length;
  rob->blend_weight = 0;
  rob->motion_flags = rob->motion_flags | 5;
}

void root_mtx_blend_capture(bone_work_rec *bone)
{
  rob_rec *rob;
  
  rob = rob_eval_rob;
  mtx_push_identity();
  mtx_translate(rob->root_offset.x,rob->root_offset_y,rob->root_offset.z);
  if (rob->yaw_delta != 0) {
    mtx_rot_y_s16(-rob->yaw_delta);
  }
  mtx_mul_by(bone->keep_mtx);
  mtx_store(bone->blend_mtx);
  mtx_pop(1);
  return;
}

void root_mtx_keep(bone_work_rec *bone)
{
  float *mtx;
  if ((rob_eval_rob->motion_flags & 0x400) != 0) {
    mtx_push(0);
    mtx_rot_y_s16(0x8000);
    mtx_store(bone->keep_mtx);
    mtx_pop(1);
  }
  else {
    mtx_store(bone->keep_mtx);
  }
  if ((rob_eval_rob->state & 0x10) == 0) {
    mtx = bone->keep_mtx;
    mtx[12] = mtx[14] = 0;
  }
}

void root_mtx_blend_prev(bone_work_rec *bone, float weight)
{
  typedef struct { int w[16]; } movstr_blk16;
  float *cur;
  float *from;
  float *out;
  float inv;
  
  if (!((rob_eval_rob->motion_flags & 0x820) == 0)) {
    *(movstr_blk16 *)bone->mtx = *(movstr_blk16 *)bone->rot_mtx;
  }
  else {
    mtx_rot_lerp(bone->mtx,bone->rot_mtx,
                 bone->blend_mtx,weight);
  }
  out = bone->mtx;
  cur = bone->rot_mtx;
  from = bone->blend_mtx;
  inv = 1.0 - weight;
  out[12] = weight * cur[12] + from[12] * inv;
  out[13] = weight * cur[13] + from[13] * inv;
  out[14] = weight * cur[14] + inv * from[14];
  return;
}

void root_pos_rsmooth_blend(bone_work_rec *bone, float weight)
{
  float inv;
  float v[3];
  float *src;
  float *dst;
  src = bone->rsmooth_pos;
  inv = 1.0 - weight;
  mtx_store_translation(v);
  dst = v;
  *dst = weight * *src + *dst * inv;
  src++; dst++;
  *dst = weight * *src + *dst * inv;
  src++; dst++;
  *dst = weight * *src + *dst * inv;
  mtx_overwrite_translation(v);
}

void rsmooth_data_get_stub(void)
{
  return;
}

void rob_move_byte4_load(rob_rec *rob)
{
  short move = rob->move_no;
  uchar *rec;
  if (move != 0) {
    rec = move_record(--move);
    rob->field_0x284 = rec[4];
  }
}

void rob_eval_empty_a(void)
{
  return;
}

void face_mot_work_select(rob_rec *rob)
{
  mot_channel_pool_set((mot_channel *)rob->face_channel_pool);
  rob_bone_links_pool_set((int *)rob->face_link_pool);
  return;
}

void face_mot_bind(rob_rec *rob)
{
  int start, elapsed;
  
  start = stopwatch_usec();
  face_mot_work_select(rob);
  mot_bind(0,0x21,rob->face_bones,(int)rob->face_motion);
  elapsed = stopwatch_usec() - start;
  face_mot_bind_usec = elapsed;
  if (elapsed > face_mot_bind_usec_max) {
    face_mot_bind_usec_max = elapsed;
    face_mot_bind_usec_max_motion = rob->face_motion;
  }
}

void face_rig_init(rob_rec *rob)
{
  bone_def *defs;
  
  defs = *(bone_def **)(*(int *)(rob->chara_data + 0x48) + 0x88);
  if (defs == (bone_def *)0x0) {
    defs = bone_defs + 0x285;
  }
  face_mot_work_select(rob);
  rob_bones_init(0,0x21,rob->face_bones,defs);
  face_rig_outputs();
  return;
}

void face_rig_outputs(void)
{
  bone_work_rec *bone;
  float *mtx;
  uint j;
  uint i;
  for (i = 0; i < 2; i++) {
    bone = (rob_get(i & 0xff))->face_bones;
    mtx = (float *)(&rob_face_mtx_buf + i * 0x7c0);
    for (j = 0; j < 0x21; j++, bone++, mtx += 0x10) {
      bone->mtx = mtx;
    }
  }
}

void rob_eval_empty_b(void)
{
  return;
}

void face_rig_offset(rob_rec *rob)
{
  uint i;
  float *mtx;

  mtx = rob->face_mtx;
  mtx_push(0);
  ((void (*)(void))mtx_load_identity)();
  mtx_translate_by_vec3(face_rig_offset_pos);
  mtx_translate_by_vec3((float *)(*(int *)(rob->chara_data + 0x48) + 0x8c));
  mtx_push(0);
  i = 0;
  do {
    mtx_peek_pushed(0);
    mtx_mul_by(mtx);
    mtx_store_wordwise(mtx);
    i = i + 1;
    mtx = mtx + 0x10;
  } while (i < 0x1f);
  mtx_pop(2);
  return;
}

float half_to_float(uint bits)
{
  union { uint i; float f; } u;
  ushort h = (ushort)bits;
  if (h != 0) {
    u.i = (uint)(h & 0x8000) << 16 | (uint)(ushort)(h * 2) * 0x1000 + 0x38000000;
    return u.f;
  }
  return 0.0f;
}

float mot_channel_sample(mot_channel *ch, float time)
{
  float value;
  if (ch == 0) return 0.0;
  if (ch->times8 != 0)
    value = mot_sample_times8(ch,time);
  else
    value = mot_sample_times16(ch,time);
  return value;
}

float mot_sample_times8(mot_channel *ch, float time)
{
  typedef struct { int data; float t; char type; } key;
  key k[2];
  int i;
  int data;
  int mode;

  if (ch == (mot_channel *)0x0) return 0.0f;
  data = (int)ch->values;
  k[1].t = 0.0f;
  k[1].data = data;
  i = 0;
  k[1].type = (ch->codes[i >> 2] >> (6 - (i % 4) * 2)) & 3;
  i = 1;
  data += k[1].type * 2;
  for (; i < ch->nkeys; i++, data += k[1].type * 2) {
    k[0] = k[1];
    k[1].type = (ch->codes[i >> 2] >> (6 - (i % 4) * 2)) & 3;
    k[1].data = data;
    k[1].t = (float)ch->times8[i - 1] * 0.033333335f;
    if (k[1].t > time) break;
  }
  if (i == ch->nkeys) {
    k[0] = k[1];
    k[1].t = (float)ch->last_frame * 0.033333335f;
    k[1].data = data;
    k[1].type = (ch->codes[i >> 2] >> (6 - (i % 4) * 2)) & 3;
  }
  if (__builtin_fabsf(k[1].t - time) < 1.1920929e-07f) mode = 0;
  else if (k[1].t > time) mode = 2;
  else {
    mode = 3;
    k[0] = k[1];
  }
  return mot_key_eval((int)k, mode, time);
}

float mot_sample_times16(mot_channel *ch, float time)
{
  typedef struct { int data; float t; char type; } key;
  key k[2];
  int i;
  int data;
  int mode;

  if (ch == (mot_channel *)0x0) return 0.0f;
  if (ch->cursor == 0) i = 0;
  else i = ch->cursor - 1;
  k[0].t = (float)ch->times16[i] * 0.033333335f;
  if (k[0].t > time) {
    ch->cursor_values = 0;
    ch->cursor = 0;
  }
  if (ch->cursor_values != 0) {
    data = (int)ch->cursor_values;
    i = ch->cursor;
  }
  else {
    data = (int)ch->values;
    i = 0;
  }
  if (ch->nkeys == i) k[1].t = (float)ch->last_frame * 0.033333335f;
  else if (i == 0) k[1].t = 0.0f;
  else k[1].t = (float)ch->times16[i - 1] * 0.033333335f;
  k[1].data = data;
  k[1].type = (ch->codes[i >> 2] >> (6 - (i % 4) * 2)) & 3;
  i++;
  data += k[1].type * 2;
  for (; i < ch->nkeys; i++, data += k[1].type * 2) {
    k[0] = k[1];
    k[1].type = (ch->codes[i >> 2] >> (6 - (i % 4) * 2)) & 3;
    k[1].data = data;
    k[1].t = (float)ch->times16[i - 1] * 0.033333335f;
    if (k[1].t > time) break;
    ch->cursor_values = (unsigned short *)data;
    ch->cursor = i;
  }
  if (i == ch->nkeys) {
    k[0] = k[1];
    k[1].t = (float)ch->last_frame * 0.033333335f;
    k[1].data = data;
    k[1].type = (ch->codes[i >> 2] >> (6 - (i % 4) * 2)) & 3;
  }
  if (__builtin_fabsf(k[1].t - time) < 1.1920929e-07f) mode = 0;
  else if (k[1].t > time) mode = 2;
  else {
    mode = 3;
    k[0] = k[1];
  }
  return mot_key_eval((int)k, mode, time);
}

float mot_key_eval(int keys, uint mode, float time)
{
  typedef struct { int w[2]; ushort h; short pad; } key;
  key k[2];
  float r;
  int at = keys;

  r = 0.0;
  switch(mode) {
  case 2:
    mot_key_unpack((int *)keys,(int *)&k[0]);
    mot_key_unpack((int *)(keys + 0xc),(int *)&k[1]);
    r = mot_hermite((float *)&k[0],(float *)&k[1],time);
    break;
  case 0:
  case 1:
    at = at + 0xc;
  case 3:
    mot_key_unpack((int *)at,(int *)&k[0]);
    r = half_to_float((uint)k[0].h);
    break;
  }
  return r;
}

void mot_key_unpack(int *packed, int *out)
{
  typedef struct { short *data; float t; char type; } src_t;
  typedef struct { float t; short a; short b; short c; } key_t;
  src_t *s = (src_t *)packed;
  key_t *d = (key_t *)out;
  switch (s->type) {
  case 0:
    d->a = d->b = d->c = 0;
    break;
  case 1:
    d->c = *s->data;
    d->a = d->b = 0;
    break;
  case 2:
    d->a = d->b = *s->data;
    d->c = s->data[1];
    break;
  case 3:
    d->a = s->data[0];
    d->b = s->data[1];
    d->c = s->data[2];
    break;
  }
  d->t = s->t;
}

float mot_hermite(float *key0, float *key1, float time)
{
  float t1;
  float tan1;
  float v1;
  float t0;
  float v0;
  float tan0;
  float dt;
  float s;
  float s2;
  float s3;
  float h;

  t1 = *key1;
  tan1 = (float)half_to_float((uint)*(ushort *)(key1 + 1));
  v1 = (float)half_to_float((uint)*(ushort *)(key1 + 2));
  t0 = *key0;
  v0 = (float)half_to_float((uint)*(ushort *)(key0 + 2));
  tan0 = (float)half_to_float((uint)*(ushort *)((int)key0 + 6));
  dt = t1 - t0;
  s = (time - t0) / dt;
  s2 = s * s;
  s3 = s * s2;
  {
    float one = 1.0;
    float two = 2.0;
    float three = 3.0;
    h = s3 * two - s2 * three;
    return ((one + h) * v0 - h * v1) + (((s3 - s2 * two) + s) * tan0 + (s3 - s2) * tan1) * dt;
  }
}

float curve_half_sample(short *keys, int count, float time)
{
  float r;
  int i;
  int mode;
  float z;
  float one, two, three;
  float k, t1, t0, a, b, c, d, dt, s, s2, s3, h;

  z = 0.0;
  if (keys == 0) { r = z; } else {
  mode = 3;
  for (i = 0; i < count; i++, keys += 4) {
    k = keys[0] * 0.016666668f;
    if (__builtin_fabsf(k - time) < 1.1920929e-07f) {
      mode = 0;
      break;
    }
    if (k > time) {
      { int y = i == 0; mode = y == 0; mode++; }
      break;
    }
  }
  switch (mode) {
  case 3:
    keys -= 4;
  case 0:
  case 1:
    r = half_to_float((ushort)keys[3]);
    break;
  case 2:
    t1 = keys[0] * 0.016666668f;
    a = half_to_float((ushort)keys[1]);
    b = half_to_float((ushort)keys[3]);
    keys -= 4;
    t0 = keys[0] * 0.016666668f;
    c = half_to_float((ushort)keys[3]);
    d = half_to_float((ushort)keys[2]);
    dt = t1 - t0;
    s = (time - t0) / dt;
    s2 = s * s;
    s3 = s * s2;
    one = 1.0f;
    two = 2.0f;
    three = 3.0f;
    h = s3 * two - s2 * three;
    k = (h + one) * c - h * b;
    r = k + (d * ((s3 - s2 * two) + s) + (s3 - s2) * a) * dt;
    break;
  default:
    r = z;
  }
  }
  return r;
}

float curve_sample(float *keys, int count, float time)
{
  int i;
  int mode;
  int first;

  if (keys == 0) {
    return 0.0;
  }
  mode = 3;
  for (i = 0; i < count; i++, keys += 4) {
    if (__builtin_fabsf(*keys - time) < 1.1920929e-07) {
      mode = 0;
      break;
    }
    if (time < *keys) {
      first = i == 0;
      mode = !first;
      mode++;
      break;
    }
  }
  return curve_key_eval((int)keys,mode,time);
}

float curve_key_eval(int key, uint mode, float time)
{
  float r;

  r = 0;
  switch(mode) {
  case 3:
    key = key + -0x10;
  case 0:
  case 1:
    r = *(float *)(key + 0xc);
    break;
  case 2:
    r = curve_hermite((float *)key,time);
    break;
  }
  return r;
}

float curve_hermite(float *key, float time)
{
  float t0 = key[0];
  float v1 = key[1];
  float v3 = key[3];
  float pt, pv, pm;
  float d, t, t2, t3, h;

  key -= 4;
  pt = key[0];
  pv = key[3];
  pm = key[2];

  d = t0 - pt;
  t = (time - pt) / d;
  t2 = t * t;
  t3 = t * t2;
  {
    float one = 1.0f;
    float two = 2.0f;
    float three = 3.0f;
    h = t3 * two - t2 * three;
    return ((one + h) * pv - h * v3) + ((t3 - t2 * two + t) * pm + (t3 - t2) * v1) * d;
  }
}

void rob_eval(bone_work_rec *bones, bone_def *defs, float time)
{
  rob_eval_bones = bones;
  rob_eval_defs = defs;
  time = time - 0.033333335;
  if (time < 0.0) {
    time = 0.0;
  }
  rob_attack_reach(rob_eval_rob);
  mtx_push_identity();
  rob_eval_ik(bones,defs,time);
  if ((rob_eval_rob->side_flags & 0x10) != 0) {
    rob_pose_mirror(bones);
  }
  rob_eval_matrix(bones);
  rob_bone_pos_blend(bones);
  rob_eval_output(bones);
  mtx_pop(1);
}

void rob_eval_ik(bone_work_rec *bone, bone_def *def, float time)
{
  float c1[2];
  float c2[2];
  float c3[2];
  uint flags;
  float *v;
  bone_work_rec **child;
  uchar *child_ids;
  int count;
  float *tgt;
  float *work_mtx;

  tgt = ik_target;
  work_mtx = rob_eval_work_mtx;
  flags = bone->def->flags;
  mtx_push(0);
  if ((flags & 0x80) != 0) {
    if ((flags & 0x80000) != 0) {
      bone_mot_pos_sample(*bone->children,tgt,time);
      vec_to_local(tgt);
      mtx_push_identity();
      mtx_aim_x(tgt);
      mtx_to_euler(bone->rot);
      mtx_store(work_mtx);
      mtx_pop(1);
      mtx_mul_by(work_mtx);
    }
    if ((flags & 0x100000) != 0) {
      uint b;
      bone_mot_pos_sample(*(*bone->children)->children,tgt,time);
      vec_to_local(tgt);
      mtx_push_identity();
      mtx_aim_x(tgt);
      b = *rob_eval_defs[*def->children].children;
      ik_cos_law(rob_eval_defs[*def->children].pos[0],vec3_length(tgt),rob_eval_defs[b].pos[0],def->bend,c1);
      mtx_rot_z_sc(c1[0],c1[1]);
      mtx_to_euler(bone->rot);
      mtx_store(work_mtx);
      mtx_pop(1);
      mtx_mul_by(work_mtx);
    }
    if ((flags & 0x1000000) != 0) {
      bone_mot_pos_sample(*bone->children,tgt,time);
      mtx_push(0);
      mtx_set_current_wordwise(rob_eval_bones->mtx);
      mtx_xform_point(tgt,tgt);
      mtx_pop(1);
      vec_to_local(tgt);
      mtx_push_identity();
      mtx_aim_x(tgt);
      mtx_to_euler(bone->rot);
      mtx_store(work_mtx);
      mtx_pop(1);
      mtx_mul_by(work_mtx);
    }
    if ((flags & 0x800000) != 0) {
      uint b;
      bone_mot_pos_sample(*(*bone->children)->children,tgt,time);
      mtx_push(0);
      mtx_set_current_wordwise(rob_eval_bones->mtx);
      mtx_xform_point(tgt,tgt);
      mtx_pop(1);
      vec_to_local(tgt);
      mtx_push_identity();
      mtx_aim_x(tgt);
      b = *rob_eval_defs[*def->children].children;
      ik_cos_law(rob_eval_defs[*def->children].pos[0],vec3_length(tgt),rob_eval_defs[b].pos[0],def->bend,c2);
      mtx_rot_z_sc(c2[0],c2[1]);
      mtx_to_euler(bone->rot);
      mtx_store(work_mtx);
      mtx_pop(1);
      mtx_mul_by(work_mtx);
    }
    if ((flags & 0x200000) != 0) {
      float *v;
      v = def->pos;
      mtx_translate_by_vec3(v);
      mtx_push_identity();
      ik_cos_law(*v,rob_eval_defs[*def->children].pos[0],vec3_length(tgt),def->bend,c3);
      mtx_rot_z_sc(c3[0],-c3[1]);
      mtx_to_euler(bone->rot);
      mtx_pop(1);
      mtx_rot_z_sc(c3[0],-c3[1]);
    }
  }
  else {
    if ((flags & 0x100) != 0) {
      mtx_translate_by_vec3(def->pos);
    }
    if ((flags & 0x200) != 0) {
      mtx_rot_zyx(def->rot);
    }
    if ((flags & 0x400) != 0) {
      v = bone->pos;
      bone_mot_pos_sample(bone,v,time);
      mtx_translate_by_vec3(v);
    }
    if ((flags & 0x800) != 0) {
      bone_mot_rot_sample(bone,time);
      mtx_rot_zyx_s16(bone->rot[0],bone->rot[1],bone->rot[2]);
    }
    if ((flags & 0x10000000) != 0) {
      v = bone->pos;
      *v = mot_channel_sample(bone->pos_ch[0],time);
    }
    if ((flags & 0x2000000) != 0) {
      mtx_store(bone->mtx);
    }
    if ((flags & 0x8000) != 0) {
      mtx_store(bone->mtx);
    }
  }
  child = bone->children;
  child_ids = def->children;
  count = bone->def->nchildren;
  while (count--) {
    rob_eval_ik(*child++,rob_eval_defs + *child_ids++,time);
  }
  mtx_pop(1);
}

void rob_eval_matrix(bone_work_rec *bone)
{
  bone_work_rec *child;
  int count;
  uint def_flags;
  bone_work_rec **children;

  def_flags = bone->def->flags;
  mtx_push(0);
  if (((def_flags & 0x8000) != 0) && ((rob_eval_rob->motion_flags & 4) != 0)) {
    root_mtx_blend_capture(bone);
  }
  if ((def_flags & 0x1980000) != 0) {
    mtx_rot_zyx_s16((int)bone->rot[0],(int)bone->rot[1],
                    (int)bone->rot[2]);
  }
  if ((def_flags & 0x200000) != 0) {
    mtx_translate_by_vec3(bone->def->pos);
    mtx_rot_zyx_s16((int)bone->rot[0],(int)bone->rot[1],
                    (int)bone->rot[2]);
  }
  if ((def_flags & 0x100) != 0) {
    mtx_translate_by_vec3(bone->def->pos);
  }
  if ((def_flags & 0x200) != 0) {
    mtx_rot_zyx(bone->def->rot);
  }
  if ((def_flags & 0x400) != 0) {
    mtx_translate_by_vec3(bone->pos);
  }
  if ((def_flags & 0x800) != 0) {
    mtx_rot_zyx_s16((int)bone->rot[0],(int)bone->rot[1],
                    (int)bone->rot[2]);
  }
  if ((def_flags & 0x2000) != 0) {
    mtx_store_translation(bone->pos);
  }
  if ((def_flags & 0x4000) != 0) {
    float z = 0.0f;
    rob_reach_rec *q = &rob_eval_rob->reach;
    if ((q->flags & 4) != 0) {
      mtx_translate(z,*(float *)(rob_eval_rob->chara_data + 0x24),z);
    }
    mtx_translate(0.0f,q->current[0] * q->out,0.0f);
    if ((rob_eval_rob->state & 0x10) != 0) {
      rob_root_offset.x = rob_root_offset.y = rob_root_offset.z = z;
    }
    else {
      mtx_store_translation(&rob_root_offset.x);
      rob_root_offset.y = 0.0f;
    }
  }
  if ((def_flags & 0x8000) != 0) {
    if ((rob_eval_rob->motion_flags & 1) != 0) {
      mtx_store(bone->rot_mtx);
      root_mtx_blend_prev(bone,rob_eval_rob->blend_weight);
      mtx_push(0);
      mtx_set_current_wordwise(bone->mtx);
      root_mtx_keep(bone);
      mtx_pop(1);
    }
    else {
      if ((rob_eval_rob->motion_flags & 8) != 0) {
        rsmooth_targets_set(rob_eval_bones);
        root_pos_rsmooth_blend(bone,rob_eval_rob->rsmooth_weight);
        mtx_store(bone->rot_mtx);
        mtx_store(bone->mtx);
      }
      else {
        mtx_store(bone->mtx);
      }
      root_mtx_keep(bone);
    }
  }
  children = bone->children;
  count = bone->def->nchildren;
  while (count = count + -1, count != -1) {
    child = *children;
    children = children + 1;
    rob_eval_matrix(child);
  }
  mtx_pop(1);
  return;
}

void rob_eval_output(bone_work_rec *bone)
{
  typedef struct { int w[16]; } mtx_blk;
  float sc1[2];
  float sc2[2];
  float sc3[2];
  uint def_flags;
  bone_work_rec **children;
  int count;
  rob_eval_aim_vec *aim;
  float *work_mtx;

  aim = &rob_eval_aim;
  work_mtx = rob_eval_work_mtx;
  def_flags = bone->def->flags;
  mtx_push(0);
  if ((def_flags & 0x2004000) != 0) goto done;
  if ((def_flags & 0x8000) != 0) {
    mtx_mul_by(bone->mtx);
    goto done;
  }
  if ((def_flags & 0x80) != 0) {
    if ((def_flags & 0x80000) != 0) {
      *aim = *(rob_eval_aim_vec *)(*bone->children)->pos;
      vec_to_local((float *)aim);
      mtx_push_identity();
      mtx_aim_x((float *)aim);
      mtx_to_euler(bone->rot);
      mtx_store(work_mtx);
      mtx_pop(1);
      mtx_mul_by(work_mtx);
    }
    if ((def_flags & 0x100000) != 0) {
      float *pl;
      float len;
      *aim = *(rob_eval_aim_vec *)(*(*bone->children)->children)->pos;
      vec_to_local((float *)aim);
      mtx_push_identity();
      mtx_aim_x((float *)aim);
      pl = (*bone->children)->def->pos;
      len = vec3_length((float *)aim);
      ik_cos_law(*pl,len,
                 (*(*bone->children)->children)->def->pos[0],bone->def->bend,sc1);
      mtx_rot_z_sc(sc1[0],sc1[1]);
      mtx_to_euler(bone->rot);
      mtx_store(work_mtx);
      mtx_pop(1);
      mtx_mul_by(work_mtx);
    }
    if ((def_flags & 0x1000000) != 0) {
      bone_work_rec *c = *bone->children;
      rob_reach_rec *q = &rob_eval_rob->reach;
      float *pv;
      *aim = *(rob_eval_aim_vec *)c->pos;
      if (bone->id == 5) {
        pv = &q->look_point.x;
        vec_to_local((float *)aim);
        mtx_push_identity();
        mtx_aim_x((float *)aim);
        mtx_store(look_aim_mtx);
        mtx_pop(1);
        look_target_clamp(pv);
        vec_to_local(pv);
        mtx_push_identity();
        mtx_store(look_blend_mtx);
        mtx_aim_x(pv);
        mtx_store(look_target_mtx);
        mtx_pop(1);
        mtx_rot_lerp(look_blend_mtx,look_target_mtx,look_aim_mtx,
                     q->look_weight);
        mtx_mul_by(look_blend_mtx);
      }
      else {
        vec_to_local((float *)aim);
        mtx_push_identity();
        mtx_aim_x((float *)aim);
        mtx_to_euler(bone->rot);
        mtx_store(work_mtx);
        mtx_pop(1);
        mtx_mul_by(work_mtx);
      }
      if (bone->id == 3) {
        mtx_store(look_chest_mtx);
      }
    }
    if ((def_flags & 0x800000) != 0) {
      float *pl;
      float len;
      int i;
      *aim = *(rob_eval_aim_vec *)(*(*bone->children)->children)->pos;
      if (((((rob_eval_rob->reach.flags & 4) != 0) && (bone->id == 0x1b || bone->id == 0x22)) &&
          (*(float *)(rob_eval_rob->chara_data + 0x24) < 0.0)) && (rob_eval_byte_flag == '\0')) {
        aim->y =
             aim->y - *(float *)(rob_eval_rob->chara_data + 0x24);
        if (rob_eval_rob->chara == '\x03') {
          aim->y = aim->y - 0.02;
        }
        else if (rob_eval_rob->chara == '\n') {
          aim->y = aim->y + 0.025;
        }
      }
      {
        rob_reach_rec *g = &rob_eval_rob->reach;
        float *arr = g->current;
        float h;
        switch(bone->id) {
        case 0x22:
          h = arr[3];
          break;
        case 0x1b:
          h = arr[4];
          break;
        case 0x12:
          h = arr[1];
          break;
        case 10:
          h = arr[2];
          break;
        default:
          h = 0.0;
        }
        aim->y = h * g->out + aim->y;
      }
      switch(bone->id) {
      case 0x22:
        i = 3;
        break;
      case 0x1b:
        i = 2;
        break;
      case 0x12:
        i = 1;
        break;
      case 10:
        i = 0;
        break;
      default:
        i = -1;
      }
      if (-1 < i) {
        rob_limb_rec *e = &rob_eval_rob->reach.limbs[i];
        if (e->bone != '\0') {
          float t = e->weight;
          float u = 1.0 - t;
          aim->x = u * aim->x + e->target_local.x * t;
          aim->y = u * aim->y + e->target_local.y * t;
          aim->z = u * aim->z + t * e->target_local.z;
        }
      }
      vec_to_local((float *)aim);
      mtx_push_identity();
      mtx_aim_x((float *)aim);
      pl = (*bone->children)->def->pos;
      len = vec3_length((float *)aim);
      ik_cos_law(*pl,len,
                 (*(*bone->children)->children)->def->pos[0],bone->def->bend,sc2);
      mtx_rot_z_sc(sc2[0],sc2[1]);
      mtx_to_euler(bone->rot);
      mtx_store(work_mtx);
      mtx_pop(1);
      mtx_mul_by(work_mtx);
    }
    if ((def_flags & 0x200000) != 0) {
      float *pa;
      float *pb;
      float len;
      mtx_translate_by_vec3(bone->def->pos);
      mtx_push_identity();
      pa = bone->def->pos;
      pb = (*bone->children)->def->pos;
      len = vec3_length((float *)aim);
      ik_cos_law(*pa,*pb,len,
                 bone->def->bend,sc3);
      mtx_rot_z_sc(sc3[0],-sc3[1]);
      mtx_to_euler(bone->rot);
      mtx_pop(1);
      mtx_rot_z_sc(sc3[0],-sc3[1]);
    }
  }
  else {
    if ((def_flags & 0x100) != 0) {
      mtx_translate_by_vec3(bone->def->pos);
    }
    if ((def_flags & 0x200) != 0) {
      mtx_rot_zyx(bone->def->rot);
    }
    if ((def_flags & 0x400) != 0) {
      mtx_translate_by_vec3(bone->pos);
    }
    if ((def_flags & 0x800) != 0) {
      short a = bone->rot[0];
      if ((def_flags & 0xc000000) != 0) {
        float *dst;
        if ((def_flags & 0x8000000) != 0) {
          dst = rob_eval_rob->bone_mtx + 0x90;
        }
        else {
          dst = rob_eval_rob->bone_mtx + 0xe0;
        }
        mtx_store(dst);
      }
      if ((def_flags & 0x2008000) == 0) {
        if ((rob_eval_rob->motion_flags & 4) != 0) {
          bone_rot_blend_capture(bone);
        }
        mtx_push_identity();
        mtx_rot_zyx_s16((int)a,(int)bone->rot[1],(int)bone->rot[2]);
        mtx_store(work_mtx);
        if ((rob_eval_rob->motion_flags & 1) != 0) {
          if (((rob_eval_rob->motion_flags & 0x800) != 0) && ((def_flags & 0x40000000) != 0))
          goto copy;
          mtx_rot_lerp(bone->rot_mtx,work_mtx,bone->blend_mtx,
                       rob_eval_rob->blend_weight);
        }
        else {
          if (((rob_eval_rob->motion_flags & 8) == 0) ||
             (((rob_eval_rob->motion_flags & 0x1000) != 0 && ((def_flags & 0x40000000) != 0))))
          goto copy;
          mtx_rot_lerp(bone->rot_mtx,bone->rsmooth_mtx,work_mtx,
                       rob_eval_rob->rsmooth_weight);
        }
        goto copied;
copy:
        *(mtx_blk *)bone->rot_mtx = *(mtx_blk *)work_mtx;
copied:
        bone_rot_keep(bone);
        if ((def_flags & 0xc000000) != 0) {
          vec3 m[3] = {{0.0,0.0,1.0},{0.0,1.0,0.0},{1.0,0.0,0.0}};
          float *base;
          short r;
          if ((def_flags & 0x8000000) != 0) {
            base = rob_eval_rob->bone_mtx + 0x90;
          }
          else {
            base = rob_eval_rob->bone_mtx + 0xe0;
          }
          mtx_xform_dir((float *)&m[0],(float *)&m[0]);
          mtx_xform_dir((float *)&m[1],(float *)&m[1]);
          mtx_xform_dir((float *)&m[2],(float *)&m[2]);
          if (m[0].z < 0.0) {
            m[0].z = m[0].z * -1.0;
          }
          r = atan2_s16(m[0].z,m[0].y) - 0x4000;
          mtx_push(0);
          mtx_set_current_wordwise(base);
          mtx_rot_x_s16((int)r);
          mtx_store(base);
          mtx_pop(1);
        }
        mtx_pop(1);
        mtx_mul_by(bone->rot_mtx);
      }
    }
    if ((def_flags & 0x10000000) != 0) {
      int i;
      float v;
      i = 0;
      if (bone->id == 0x15) i = 1;
      if ((rob_eval_rob->hands[0].flags & 1) != 0) {
        v = *bone->pos;
      }
      else if (bone->id == 0xd) {
        v = rob_eval_rob->hands[0].current * 0.1;
      }
      else {
        v = rob_eval_rob->hands[1].current * 0.1;
      }
      if ((rob_eval_rob->motion_flags & 4) != 0) {
        *bone->blend_pos = *bone->keep_pos;
      }
      if ((rob_eval_rob->motion_flags & 1) != 0) {
        float *s = bone->blend_pos;
        float count = rob_eval_rob->blend_weight;
        v = v * count + (1.0 - count) * *s;
      }
      else if ((rob_eval_rob->motion_flags & 8) != 0) {
        float *s = bone->rsmooth_pos;
        float count = rob_eval_rob->rsmooth_weight;
        v = v * (1.0 - count) + count * *s;
        rob_eval_rob->hands[i].smooth_target = *s * 10.0;
      }
      rob_eval_rob->hands[i].field_0x0 = v;
      *bone->keep_pos = *bone->pos = v;
    }
  }
  if ((def_flags & 0x1000) != 0) {
    mtx_store(bone->mtx);
  }
done:
  children = bone->children;
  count = bone->def->nchildren;
  while (count = count + -1, count != -1) {
    rob_eval_output(*children++);
  }
  mtx_pop(1);
}

void mtx_aim_x(float *dir)
{
  float x = dir[0];
  float y = dir[1];
  float z = dir[2];
  float s = x * x + y * y;
  float r = s;
  __asm__ ("fsrra %0" : "+f"(r));
  mtx_rot_z_sc(y * r, x * r);
  r = r * s;
  s = z * z + s;
  __asm__ ("fsrra %0" : "+f"(s));
  mtx_rot_y_sc(-z * s, r * s);
}

void ik_cos_law(float len_a, float len_b, float len_c, int flip, float *out)
{
  float one = 1.0;
  float c;
  c = ((len_a * len_a + len_b * len_b) - len_c * len_c) /
          (len_a * len_b + len_a * len_b);
  if (c > one) c = one;
  else if (c < -one) c = -one;
  if (flip == 0) {
    *out++ = -fsqrt_asm(one - c * c);
  }
  else {
    *out++ = fsqrt_asm(one - c * c);
  }
  *out = c;
}

void mtx_to_euler(short *angles)
{
  float *m;
  float s;
  short *p = angles;

  m = rob_eval_work_mtx;
  mtx_store(m);
  s = m[2];
  if (!(__builtin_fabsf(s) < 0.9999999)) {
    p[0] = 0;
    p[1] = asin_ang16(-s);
    if (s < 0.0) {
      p[2] = atan2_s16(m[9],m[5]);
    }
    else {
      p[2] = -atan2_s16(m[9],m[5]);
    }
  }
  else {
    p[0] = atan2_s16(m[6],m[10]);
    p[1] = asin_ang16(-s);
    p[2] = atan2_s16(m[1],m[0]);
  }
}

void bone_mot_pos_sample(bone_work_rec *bone, float *out, float time)
{
  typedef struct { float x; float y; float z; } v3;
  mot_channel **chans = bone->pos_ch;
  v3 *dst = (v3 *)out;
  dst->x = mot_channel_sample(*chans++,time);
  dst->y = mot_channel_sample(*chans++,time);
  dst->z = mot_channel_sample(*chans,time);
}

void bone_mot_rot_sample(bone_work_rec *bone, float time)
{
  mot_channel **chans = bone->rot_ch;
  short *angles = bone->rot;
  
  *angles++ = (short)(int)((float)mot_channel_sample(*chans++,time) * 10430.378);
  *angles++ = (short)(int)((float)mot_channel_sample(*chans++,time) * 10430.378);
  *angles = (short)(int)((float)mot_channel_sample(*chans,time) * 10430.378);
  return;
}

void rob_base_mtx_eval(rob_rec *rob, float *out, int want_pos, float time)
{
  float *pos_out = out;
  char pos_only = (char)want_pos;
  bone_work_rec *root;
  bone_work_rec *bone;
  float *bone_pos;

  root = rob->bones;
  bone = root;
  bone_pos = bone->pos;
  time = time - 0.033333335;
  if (time < 0.0) time = 0.0;
  mtx_push_identity();
  bone_mot_pos_sample(bone,bone_pos,time);
  if ((rob->side_flags & 0x10) != 0) bone_pos_pair_swap(root,&mirror_pos_pairs[0][0]);
  mtx_translate_by_vec3(bone_pos);
  bone_mot_rot_sample(bone,time);
  if ((rob->side_flags & 0x10) != 0) bone_angle_mirror(root,&mirror_angle_flips[0][0]);
  mtx_rot_zyx_s16(bone->rot[0],bone->rot[1],bone->rot[2]);
  bone = *bone->children;
  bone_pos = bone->pos;
  bone_mot_pos_sample(bone,bone_pos,time);
  if ((rob->side_flags & 0x10) != 0) bone_pos_pair_swap(root,&mirror_pos_pairs[1][0]);
  mtx_translate_by_vec3(bone_pos);
  bone = *bone->children;
  bone_mot_rot_sample(bone,time);
  if ((rob->side_flags & 0x10) != 0) bone_angle_mirror(root,&mirror_angle_flips[2][0]);
  mtx_rot_zyx_s16(bone->rot[0],bone->rot[1],bone->rot[2]);
  if (pos_only == '\0') mtx_store(rob_base_mtx[(uint)rob->side]);
  else mtx_store_translation(pos_out);
  mtx_pop(1);
}

void rob_base_mtx_update(rob_rec *rob, float time)
{
  rob_base_mtx_eval(rob,0,0,time);
  return;
}

void rob_base_pos_get(rob_rec *rob, float time, float *out)
{
  rob_base_mtx_eval(rob,out,1,time);
  return;
}

void rob_attack_reach(rob_rec *rob)
{
  typedef struct { int w[5]; } reach_pose;
  typedef struct { char pad[0xf0]; float x; float y; float z; } joint_pos;
  rob_reach_rec *reach;
  byte flags;
  uint move_flags;
  float rate;
  float step;
  float a;
  float b;
  float diff;
  int open;
  float v[3];
  float t[3];
  int yaw;
  short pitch;
  float k;
  float k1;
  rob_limb_rec *limb;
  char i;
  float *opp_mtx;
  rob_rec *look_opp;
  rob_rec *opp;
  float *pt;
  vec3 *pa;
  vec3 *pb;
  float dx;
  float dz;
  float h;

  reach = &rob->reach;
  flags = reach->flags;
  move_flags = rob->move_flags;
  if (*(float *)(rob->chara_data + 0x24) < 0.0) {
    if (mode_get_main() == MODE_MAIN_GAME && (mode_get_sub() == 7 || mode_get_sub() == 0xf)) {
      flags &= 0xfb;
    }
    else if (mode_get_main() == MODE_MAIN_OVER && (mode_get_sub() == 1 || mode_get_sub() == 3)) {
      flags &= 0xfb;
    }
    else if ((rob->flags24 & 0x4000000) != 0 && (rob->flags & 0x100) == 0) {
      flags &= 0xfb;
    }
    else {
      flags |= 4;
    }
  }
  else if (*(float *)(rob->chara_data + 0x24) > 0.001) {
    flags |= 4;
  }
  else {
    flags &= 0xfb;
  }
  if ((rob->state & 0x800) != 0 ||
      ((rob->state & 0x300400) == 0x400 && (rob->action.part.lever & 1) == 0) ||
      ((rob->state & 0x200400) == 0x200400 && rob->field_0x2f2 <= rob->frame) ||
      (rob->state & 0x18000000) != 0 || (rob->flags24 & 0x40) != 0 ||
      (rob->flags2c & 0x10) != 0) {
    flags &= 0xfb;
  }
  rate = reach->rate;
  open = 0;
  switch (rob->attack_level) {
  case 1:
  case 2:
  case 4:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
    open = 1;
  }
  a = *(float *)(rob->chara_data + 0x20);
  b = *(float *)(rob->opp->chara_data + 0x20);
  if (a > b && (diff = a - b, __builtin_fabsf(diff) > 0.01)) {
    reach->dist = diff * 0.75;
    if ((move_flags & 0x4000) && open) {
      float x = rob->frame_pos;
      float l0 = rob->active_start;
      float l1 = rob->active_end;
      float l2 = rob->recovery_end;
      if (l0 > x) {
        if ((rob->opp->state & 0x200040) != 0x40) {
          rate = rate + (1.0 - rate) / (l0 - x);
        }
        flags |= 1;
      }
      else if (l1 > x) {
        if ((rob->opp->state & 0x200040) != 0x40) {
          rate = 1.0;
        }
      }
      else if (l2 > x) {
        rate = rate - rate / (l2 - x);
      }
      else {
        rate = 0.0;
        flags &= 0xfe;
      }
    }
    else if ((move_flags & 0x200) != 0) {
      flags &= 0xfe;
      rate = 0.0;
    }
    else if ((flags & 1) != 0) {
      rate = rate * 0.85;
      if (rate < 0.01) {
        rate = 0.0;
        flags &= 0xfe;
      }
    }
    if ((flags & 2) != 0) {
      flags &= 0xfd;
      *(reach_pose *)reach->target = ((reach_pose *)reach_poses)[reach_limb_poses[rob->attack_limb]];
    }
    if ((flags & 1) != 0) {
      {
        float *tg = reach->target;
        float *cu = reach->current;
        float half = 0.5;
        cu[0] = (tg[0] - cu[0]) * half + cu[0];
        cu[1] = (tg[1] - cu[1]) * half + cu[1];
        cu[2] = (tg[2] - cu[2]) * half + cu[2];
        cu[3] = (tg[3] - cu[3]) * half + cu[3];
        cu[4] = (tg[4] - cu[4]) * half + cu[4];
      }
    }
    else {
      reach->current[0] = 0.0;
      reach->current[1] = 0.0;
      reach->current[2] = 0.0;
      reach->current[3] = 0.0;
      reach->current[4] = 0.0;
    }
  }
  else {
    reach->dist = 0.0;
    rate = 0.0;
    flags &= 0xfe;
  }
  step = 0.1;
  if (mode_get_main() == MODE_MAIN_GAME && (mode_get_sub() == 7 || mode_get_sub() == 0xf)) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if (mode_get_main() == MODE_MAIN_OVER && (mode_get_sub() == 1 || mode_get_sub() == 3)) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if (mode_get_main() == MODE_MAIN_ADVERTISE && (mode_get_sub() == 8 || mode_get_sub() == 7)) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if ((user_back_data.option_bits & 0x40) != 0) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if ((rob->opp->flags & 1) == 0) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if ((rob->state & 0xa00) != 0) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if ((rob->state & 0x400) != 0 && (rob->state & 0x40000040) != 0) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if (rob->field_0x3b4 > 0 &&
           (mode_get_main() != MODE_MAIN_GAME || (mode_get_sub() != 9 && mode_get_sub() != 8))) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if ((rob->side_flags & 0x800) != 0) {
    flags |= 8;
  }
  else if ((rob->state & 0x10000401) != 0) {
    flags &= 0xf7;
    step = 0.2;
  }
  else if ((rob->flags24 & 0x40) != 0 || (rob->flags & 0x4000008) != 0) {
    flags &= 0xf7;
    step = 0.2;
  }
  else if ((rob->opp->flags & 1) == 0) {
    flags &= 0xf7;
    reach->look_weight = 0.0;
  }
  else if ((rob->side_flags & 0x200) != 0) {
    flags &= 0xf7;
    step = 0.1;
  }
  else if ((rob->opp->side_flags & 0x400) != 0) {
    flags &= 0xf7;
    step = 0.1;
  }
  else {
    flags |= 8;
  }
  if ((flags & 8) != 0) {
    mtx_push(0);
    mtx_set_current_wordwise(rob->mtx + 0x10);
    mtx_rot_y_s16(0x4000);
    mtx_rot_x_s16(0x4000);
    look_opp = rob->opp;
    v[0] = ((joint_pos *)look_opp->mtx)->x;
    v[1] = ((joint_pos *)look_opp->mtx)->y;
    v[2] = ((joint_pos *)look_opp->mtx)->z;
    vec_to_local(v);
    yaw = atan2_s16(v[0], v[2]);
    pitch = 0x4000 - atan2_s16(fsqrt_asm(v[0] * v[0] + v[2] * v[2]), v[1]);
    mtx_pop(1);
    if (pitch < -0x2800 || pitch > 0x3800 || yaw > 0x5000 || yaw < -0x5000) {
      flags &= 0xf7;
      step = 0.06;
    }
    else if ((flags & 0x10) == 0 && (pitch < -0x2000 || pitch > 0x3000 || yaw > 0x4000 || yaw < -0x4000)) {
      flags &= 0xf7;
      step = 0.06;
    }
  }
  if ((flags & 8) != 0) {
    reach->look_weight = reach->look_weight + 0.093;
  }
  else {
    reach->look_weight = reach->look_weight - step;
    if (reach->look_weight < 0.0) {
      reach->look_weight = 0.0;
    }
  }
  if (reach->look_weight > 1.0) {
    reach->look_weight = 1.0;
  }
  pt = t;
  pa = &reach->look_point;
  pb = &reach->look_target;
  opp = rob->opp;
  pt[0] = ((joint_pos *)opp->mtx)->x;
  pt[1] = ((joint_pos *)opp->mtx)->y;
  pt[2] = ((joint_pos *)opp->mtx)->z;
  h = *(float *)(rob->chara_data + 0x2c);
  dx = ((joint_pos *)rob->mtx)->x - pt[0];
  dz = ((joint_pos *)rob->mtx)->z - pt[2];
  pt[1] = h * fsqrt_asm(dx * dx + dz * dz) + pt[1];
  if ((flags & 0x10) != 0) {
    k = 0.3;
    k1 = 1.0 - k;
    pt[0] = pt[0] * k + pb->x * k1;
    pt[1] = pt[1] * k + pb->y * k1;
    pt[2] = pt[2] * k + pb->z * k1;
  }
  mtx_push_identity();
  mtx_translate_by_vec3(&rob->place_pos.x);
  mtx_rot_y_s16(-rob->heading);
  *pa = *(vec3 *)pt;
  *pb = *pa;
  vec_to_local(&pa->x);
  mtx_pop(1);
  if ((flags & 8) != 0) {
    flags |= 0x10;
  }
  else {
    flags &= 0xef;
  }
  reach->rate = rate;
  reach->out = reach->dist * rate;
  reach->flags = flags;
  limb = rob->reach.limbs;
  for (i = 0; i < 4; i++, limb++) {
    if (limb->bone != 0) {
      limb->weight = limb->weight + limb->speed;
      if (limb->weight > 1.0) {
        limb->weight = 1.0;
      }
      else if (limb->weight < 0.0) {
        limb->bone = 0;
        limb->speed = 0.0;
        limb->weight = 0.0;
        continue;
      }
      opp_mtx = rob->opp->mtx;
      mtx_push(0);
      mtx_set_current_wordwise(opp_mtx + limb->bone * 0x10 - 0x10);
      mtx_xform_point(&limb->target.x, &limb->target_local.x);
      mtx_set_current_wordwise(rob->place_mtx);
      vec_to_local(&limb->target_local.x);
      mtx_pop(1);
    }
  }
}

void mot_children_eval(bone_work_rec *bone, float time)
{
  int i;
  short *skip;
  int count;
  bone_work_rec **children;
  
  children = bone->children;
  count = bone->def->nchildren;
  time = time - 0.033333335;
  if (time < 0.0) {
    time = 0.0;
  }
  mtx_push_identity();
  if (anim_skip_list == 0) {
    if (count != 0) {
      for (i = 0; i < count; i++) {
        bone_mot_eval_flat(children[i],time);
      }
    }
  }
  else {
    skip = anim_skip_list;
    if (count != 0) {
      for (i = 0; i < count; i++) {
        if (i == *skip) {
          if (0 < i) {
            skip = skip + 1;
          }
        }
        else {
          bone_mot_eval_flat(children[i],time);
        }
      }
    }
  }
  mtx_pop(1);
}

void bone_mot_eval_flat(bone_work_rec *bone, float time)
{
  float *pos;
  uint def_flags;
  
  def_flags = bone->def->flags;
  mtx_push(0);
  if ((def_flags & 0x400) != 0) {
    pos = bone->pos;
    bone_mot_pos_sample(bone,pos,time);
    *pos = *pos * 0.1;
    bone->pos[1] = bone->pos[1] * 0.1;
    bone->pos[2] = bone->pos[2] * 0.1;
    mtx_translate_by_vec3(pos);
  }
  if ((def_flags & 0x800) != 0) {
    bone_mot_rot_sample(bone,time);
    mtx_rot_zyx_s16(bone->rot[0],bone->rot[1],bone->rot[2]);
  }
  if ((def_flags & 0x1000) != 0) {
    mtx_store(bone->mtx);
  }
  mtx_pop(1);
  return;
}

void face_rig_eval(bone_work_rec *bones, float time)
{
  time -= 0.033333335;
  if (time < 0.0) {
    time = 0.0;
  }
  face_rig_scale_index = 0;
  mtx_push_identity();
  face_rig_walk(bones,time);
  mtx_pop(1);
}

void face_rig_walk(bone_work_rec *bone, float time)
{
  int count;
  bone_work_rec **children;
  uint def_flags;
  float v[3];

  def_flags = bone->def->flags;
  mtx_push(0);
  if ((def_flags & 0x20000000) != 0) {
    mtx_scale_by_vec3(((float (*)[3])face_rig_scales)[face_rig_scale_index++]);
  }
  if ((def_flags & 0x100) != 0) {
    mtx_translate_by_vec3(bone->def->pos);
  }
  if ((def_flags & 0x200) != 0) {
    mtx_rot_zyx(bone->def->rot);
  }
  if ((def_flags & 0x400) != 0) {
    float *s = bone->pos;
    bone_mot_pos_sample(bone,s,time);
    {
      float k = 0.1;
      v[0] = s[0] * k;
      v[1] = s[1] * k;
      v[2] = s[2] * k;
    }
    mtx_translate_by_vec3(v);
  }
  if ((def_flags & 0x800) != 0) {
    bone_mot_rot_sample(bone,time);
    mtx_rot_zyx_s16((int)bone->rot[0],(int)bone->rot[1],(int)bone->rot[2]);
    if (bone->id == 0x1f) {
      if ((rob_eval_rob->side_flags & 0x10) != 0) {
        rob_eval_rob->eye_angles[5] = -bone->rot[0];
        rob_eval_rob->eye_angles[1] = bone->rot[1];
        rob_eval_rob->eye_angles[3] = -bone->rot[2];
      }
      else {
        rob_eval_rob->eye_angles[4] = bone->rot[0];
        rob_eval_rob->eye_angles[0] = bone->rot[1];
        rob_eval_rob->eye_angles[2] = bone->rot[2];
      }
    }
    else if (bone->id == 0x20) {
      if ((rob_eval_rob->side_flags & 0x10) != 0) {
        rob_eval_rob->eye_angles[4] = -bone->rot[0];
        rob_eval_rob->eye_angles[0] = bone->rot[1];
        rob_eval_rob->eye_angles[2] = -bone->rot[2];
      }
      else {
        rob_eval_rob->eye_angles[5] = bone->rot[0];
        rob_eval_rob->eye_angles[1] = bone->rot[1];
        rob_eval_rob->eye_angles[3] = bone->rot[2];
      }
    }
  }
  if ((def_flags & 0x1000) != 0) {
    mtx_store(bone->mtx);
  }
  children = bone->children;
  for (count = bone->def->nchildren; count-- != 0; ) {
    face_rig_walk(*children++,time);
  }
  mtx_pop(1);
}

void look_target_clamp(float *point)
{
  float x;
  float y;
  float z;
  float dist;
  int yaw;
  int pitch;

  mtx_push(0);
  mtx_set_current_wordwise(look_chest_mtx);
  mtx_rot_y_s16(0x4000);
  mtx_rot_x_s16(0x4000);
  vec_to_local(point);
  x = point[0];
  y = point[1];
  z = point[2];
  yaw = atan2_s16(x,z);
  if (yaw > 0x3800) {
    float v[3];
    v[0] = x;
    v[1] = 0.0;
    v[2] = z;
    mtx_push_identity();
    mtx_rot_y_s16(0x3800 - yaw);
    mtx_xform_dir(v,v);
    x = v[0];
    z = v[2];
    mtx_pop(1);
  }
  else if (yaw < -0x3800) {
    float v[3];
    v[0] = x;
    v[1] = 0.0;
    v[2] = z;
    mtx_push_identity();
    mtx_rot_y_s16(-0x3800 - yaw);
    mtx_xform_dir(v,v);
    x = v[0];
    z = v[2];
    mtx_pop(1);
  }
  dist = x * x + z * z;
  __asm__ ("fsqrt %0" : "=f" (dist) : "0" (dist));
  pitch = (short)(0x4000 - atan2_s16(dist,y));
  if (pitch > 0x3000) {
    float v[3];
    v[0] = x;
    v[1] = y;
    v[2] = z;
    mtx_push_identity();
    mtx_rot_x_s16(pitch - 0x3000);
    mtx_xform_dir(v,v);
    x = v[0];
    z = v[2];
    y = v[1];
    mtx_pop(1);
  }
  else if (pitch < -0x800) {
    float v[3];
    v[0] = x;
    v[1] = y;
    v[2] = z;
    mtx_push_identity();
    mtx_rot_x_s16(pitch + 0x800);
    mtx_xform_dir(v,v);
    x = v[0];
    z = v[2];
    y = v[1];
    mtx_pop(1);
  }
  point[0] = x;
  point[1] = y;
  point[2] = z;
  mtx_translate_by_vec3(point);
  mtx_store_translation(point);
  mtx_pop(1);
}

float * rob_base_mtx_get(uint side)
{
  return rob_base_mtx[side & 0xff];
}

void rob_pose_mirror(bone_work_rec *bones)
{
  char *pair;
  
  for (pair = &mirror_pos_pairs[0][0]; *pair >= 0; pair += 4) {
    bone_pos_pair_swap(bones,pair);
  }
  for (pair = &mirror_angle_flips[0][0]; *pair >= 0; pair += 4) {
    bone_angle_mirror(bones,pair);
  }
  for (pair = &mirror_angle_pairs[0][0]; *pair >= 0; pair += 4) {
    bone_angle_pair_swap(bones,pair);
  }
  return;
}

void bone_pos_pair_swap(bone_work_rec *bones, char *pair)
{
  float *a;
  float *b;
  short flips;
  float t;
  float u;

  a = bones[pair[0]].pos;
  b = bones[pair[1]].pos;
  flips = *(short *)(pair + 2);
  t = *a;
  if (flips & 1) t = -t;
  u = *b;
  if (flips & 1) u = -u;
  *a++ = u;
  *b++ = t;
  t = *a;
  if (flips & 2) t = -t;
  u = *b;
  if (flips & 2) u = -u;
  *a++ = u;
  *b++ = t;
  t = *a;
  if (flips & 4) t = -t;
  u = *b;
  if (flips & 4) u = -u;
  *a = u;
  *b = t;
}

void bone_angle_pair_swap(bone_work_rec *bones, char *pair)
{
  short *a;
  short *b;
  short flips;
  short t1;
  short t2;

  a = bones[pair[0]].rot;
  b = bones[pair[1]].rot;
  flips = *(short *)(pair + 2);
  if (flips & 1) t1 = -*a; else t1 = *a;
  if (flips & 1) t2 = -*b; else t2 = *b;
  *a = t2; a++;
  *b = t1; b++;
  if (flips & 2) t1 = -*a; else t1 = *a;
  if (flips & 2) t2 = -*b; else t2 = *b;
  *a = t2; a++;
  *b = t1; b++;
  if (flips & 4) t1 = -*a; else t1 = *a;
  if (flips & 4) t2 = -*b; else t2 = *b;
  *a = t2;
  *b = t1;
}

void bone_angle_mirror(bone_work_rec *bones, char *flips)
{
  short value;
  int i;
  short *angle;
  
  angle = bones[*flips++].rot;
  for (i = 0; i < 3; i++, angle++, flips++) {

    switch((unsigned char)*flips) {
    case 0:
      value = *angle;
      break;
    case 1:
      value = -*angle;
      break;
    case 2:
    case 3:
      value = -0x8000 - *angle; break;
    default: value = 0;
    }
    *angle = value;
  }
}
