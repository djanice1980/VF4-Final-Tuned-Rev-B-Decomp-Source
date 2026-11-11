/*
 * vf4lib/libeffect/src/effect_break.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float mode_timer_rate; /* 0x0c292678 */
extern int break_link_start_mask; /* 0x0c2aa694 */
extern int break_link_done_mask; /* 0x0c2aa698 */
extern stage_rec *stage_cur; /* 0x0c306ce8 */
extern int stage_cur_no; /* 0x0c306cf0 */
extern uchar *oneshot_anim_pool; /* 0x0c387104 */
extern uchar *break_buf; /* 0x0c4548f0 */
extern rome_break_source rome_break_sources; /* 0x0c2a2aac */
extern stage_anim_rec stage_anim_objs[2]; /* 0x0c382438 */
extern stage_anim_rec stage_anim_obj2[2]; /* 0x0c382498 */

void stage_anim_buffers_set(stage_anim_rec *obj)
{
  mot_channel_pool_set(obj->channels);
  rob_bone_links_pool_set(obj->links);
  return;
}

void stage_anim_obj_bind(stage_anim_rec *obj)
{
  int start;
  int end;

  stage_anim_buffers_set(obj);
  rob_bones_init(0,0x21,obj->bones,bone_defs + 0x263);
  stage_anim_bones_link((int *)obj);
  start = stopwatch_usec();
  mot_bind(0,0x21,obj->bones,(int)obj->anim_id);
  end = stopwatch_usec();
  stage_anim_bind_usec = end - start;
  if (stage_anim_bind_usec > stage_anim_bind_usec_max) {
    stage_anim_bind_usec_max = stage_anim_bind_usec;
    stage_anim_bind_max_motion = obj->anim_id;
  }
  obj->anim_frames = mot_bind_frame_count();
}

void stage_anim_frame_ptrs_init(void)
{
  int i;
  for (i = 0; i < 4; i++) {
    (stage_anim_objs + i)->mtx = stage_anim_mtx[i];
  }
}

void stage_anim_buffers_carve(uchar *buf)
{
  stage_anim_rec *obj;
  int i;

  obj = stage_anim_objs;
  for (i = 0; i < 4; i++, obj++) {
    obj->bones = (bone_work_rec *)buf;
    buf += 0xdec;
    obj->channels = (mot_channel *)buf;
    buf += 0x1800;
    obj->links = (int *)buf;
    buf += 0x80;
  }
}

void stage_anim_objs_step(uint first, uint end)
{
  uint i;
  uint n;
  stage_anim_rec *p;
  short flags;
  short id;
  float t;
  float *pt;
  short *pf;
  short *hide;

  first = (uchar)first;
  n = (uchar)end;
  p = stage_anim_objs + first;
  i = first;
  if (i >= n) return;
top:
  {
    flags = p->flags;
    id = p->anim_id;
    if ((id == 0x1672) | (id == 0x1670)) goto next;
    if ((flags & 1) == 0) goto next;
    if ((flags & 4) != 0) goto next;
    t = p->frame_time;
    if (id == 0x166f) hide = anim_skip_nodes_166f;
    else if (p->anim_id == 0x1671) hide = anim_skip_nodes_1671;
    else hide = 0;
    anim_skip_list = hide;
    mot_children_eval(p->bones,t * 0.033333335);
    if ((float)p->anim_frames < t) {
      p->flags = p->flags | 4;
    }
    if ((flags & 1) != 0) {
      pt = &p->frame_time;
      *pt = mode_timer_rate + t;
      p->frame_count = p->frame_count + 1;
      pf = &p->flags;
      if (((*pf & 0x40) != 0) && ((lcg_next15() & 3) == 0) &&
          ((float)p->anim_frames * 0.6 > *pt)) {
        *pf = *pf | 0x20;
      }
    }
  }
next:
  i = (uchar)(i + 1);
  p++;
  if (i < n) goto top;
}

void stage_anim_reset(void)
{
  uchar *p;
  int i;
  int n;

  switch (stage_cur_no) {
  case 0:
    n = 2;
    break;
  case 4:
    n = 4;
    break_fall_list_clear();
    break;
  default:
    return;
  }
  p = oneshot_anim_pool;
  for (i = 0; i < 0x14; i++, p += 0x30) {
    *(uint *)(p + 0x18) = 0;
  }
  for (i = 0; i < n; i++) {
    stage_anim_objs[i].flags = 0;
  }
}

void stage_anim_buffer_free(void)
{
  effect_free(break_buf);
  break_buf = 0;
  return;
}

void yam_break_objs_init(void)
{
  stage_anim_rec *p;
  int i;
  short *motion;
  short tab[4];
  memcpy(tab,yam_break_motions,8);
  motion = tab;
  p = stage_anim_objs;
  break_buf = effect_malloc(0x99b0);
  stage_anim_buffers_carve(break_buf);
  for (i = 0; i < 4; i++, p++, motion++) {
    if (p->anim_id == 0x166f) {
      p->flags = 0;
    }
    else {
      p->flags = 8;
    }
    p->frame_time = 1.0;
    p->frame_count = 0;
    if (i < 4) {
      p->anim_id = *motion;
      stage_anim_obj_bind(p);
    }
  }
}

void rome_break_init(void)
{
  int i;
  stage_anim_rec *obj;
  rome_break_source *src;

  obj = stage_anim_obj2;
  src = &rome_break_sources;
  break_buf = effect_malloc(0x99b0);
  stage_anim_buffers_carve(break_buf);
  for (i = 2; i < 4; i++, obj++) {
    obj->flags = 8;
    obj->frame_time = 1.0;
    obj->frame_count = 0;
    obj->anim_id = 0;
    obj->pos[0] = obj->pos[1] = obj->pos[2] = 0.0;
    obj->turn = 0;
  }
  for (i = 0; i < 0xb; i++, src++) {
    src->flags = 0;
  }
  return;
}

void rome_break_obj_start(uint slot, uint source)
{
  uchar a = slot;
  uchar b = source;
  stage_anim_rec *e = stage_anim_objs + a;
  rome_break_src_rec *s = (rome_break_src_rec *)&rome_break_sources + b;
  e->source = b;
  e->anim_id = s->anim_id;
  e->models = s->models;
  e->flags = 0x43;
  e->frame_count = 0;
  e->frame_time = 1.0;
  e->pos[0] = s->pos[0];
  e->pos[1] = s->pos[1];
  e->pos[2] = s->pos[2];
  e->turn = 0;
  stage_anim_obj_bind(e);
  s->flags |= 1;
}

void rome_break_update(void)
{
  uint next_slot;
  rome_break_src_rec *src;
  uint source;
  uint slot;

  src = (rome_break_src_rec *)&rome_break_sources;
  slot = 2;
  source = 0;
  do {
    if (((src->flags & 1) == 0) && ((break_link_start_mask & src->done_mask) != 0)) {
      next_slot = slot & 0xff;
      slot = slot + 1;
      rome_break_obj_start(next_slot,source & 0xff);
    }
    source = source + 1;
    src = src + 1;
  } while ((int)source < 0xb && (int)slot < 4);
  stage_anim_objs_step(2,4);
  return;
}

void rome_break_draw(void)
{
  stage_anim_rec *p;
  uchar i;
  float *m;
  int *model;
  int chance;
  short flags;
  short cleared;
  float v[3];
  p = stage_anim_obj2;
  mtx_push(0);
  for (i = 2; i < 4; i++, p++) {
    if ((p->flags & 2) == 0) continue;
    p->frame_count++;
    m = p->mtx;
    model = p->models;
    mtx_push(0);
    mtx_translate_by_vec3(p->pos);
    mtx_rot_y_s16(p->turn);
    mtx_push(0);
    while (m += 16, *model != -1) {
      mtx_peek_pushed(0);
      mtx_mul_by(m);
      if ((uint)(p->source - 3) <= 1) chance = 20; else chance = 10;
      if ((p->flags & 0x20) != 0) {
        if ((int)lcg_next15() % chance == 0) {
          mtx_push_identity();
          mtx_translate_by_vec3(p->pos);
          mtx_rot_y_s16(p->turn);
          mtx_mul_by(m);
          mtx_store_translation(v);
          vec3_sub_from(v, &stage_cur->center.x);
          if ((uint)(p->source - 3) <= 3) {
            oneshot_anim_spawn(rome_break_oneshot_type3_6, (uint *)v, 0);
          }
          else {
            oneshot_anim_spawn(rome_break_oneshot_other, (uint *)v, 0);
          }
          mtx_pop(1);
        }
      }
      obj_draw(*model++);
    }
    mtx_pop(2);
    flags = p->flags;
    cleared = flags & ~0x20;
    p->flags = cleared;
    if ((flags & 4) != 0) {
      if ((flags & 8) != 0) {
        p->flags = cleared & ~1;
      }
      else {
        p->flags &= ~3;
      }
      { rome_break_src_rec *tb = ((rome_break_src_rec *)&rome_break_sources); break_link_done_mask |= tb[p->source].done_mask; }
    }
  }
  mtx_pop(1);
}

void rome_break_end(void)
{
  oneshot_anim_pool_free();
  stage_anim_buffer_free();
  return;
}

void rome_break_reset(void)
{
  rome_break_source *src;
  stage_anim_rec *obj;
  int i;

  if ((stage_cur_no == 0) && (break_buf != 0)) {
    stage_anim_buffers_carve(break_buf);
    obj = stage_anim_obj2;
    i = 2;
    do {
      obj->flags = 8;
      obj->frame_time = 1.0;
      obj->frame_count = 0;
      obj->anim_id = 0;
      obj->pos[0] = 0.0;
      obj->pos[1] = 0.0;
      obj->pos[2] = 0.0;
      obj->turn = 0;
      i = i + 1;
      obj = obj + 1;
    } while (i < 4);
    src = &rome_break_sources;
    i = 0;
    do {
      src->flags = 0;
      i = i + 1;
      src = src + 1;
    } while (i < 0xb);
  }
  return;
}
