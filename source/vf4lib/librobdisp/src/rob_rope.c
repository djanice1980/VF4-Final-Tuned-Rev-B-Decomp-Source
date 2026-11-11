/*
 * vf4lib/librobdisp/src/rob_rope.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float rope_link_length; /* 0x0c2b1024 */
extern float rope_link_scale[3]; /* 0x0c2b1028 */
extern int rope_link_model; /* 0x0c2b1034 */
extern int rope_lines_show; /* 0x0c2b1038 */
extern int rope_frame; /* 0x0c39bdfc */
extern rope_link rope_links[128]; /* 0x0c3985fc */

void rope_init(void)
{
  rope_link *e;
  int i;

  rope_frame = 0;
  rope_span_length = rope_link_length * 128.0;
  mtx_push(0);
  mtx_load_identity();
  for (i = 0; i < 0x80; i++) {
    rope_link *t = rope_links;
    t[i].state = 1;
    t[i].len_prev = rope_link_length;
    t[i].len_next = rope_link_length;
    t[i].pos.x = t[i].old.x = (float)i * 0.3;
    t[i].pos.y = t[i].old.y = 5.5;
    t[i].pos.z = t[i].old.z = 0.0;
    t[i].vel.x = 0.0;
    t[i].vel.y = 0.0;
    t[i].vel.z = 0.0;
    mtx_store_wordwise(rope_links[i].m);
  }
  mtx_pop(1);
  e = rope_links;
  e[0].state = 2;
  e[127].state = 2;
}

void rope_tension_add(float *pos, float *from, float *to, float length)
{
  float v[3];
  float m[3];

  vec3_sub(v,to,from);
  vec3_set_length((-v[1] * rope_cloth_gravity) / length,m,v);
  vec3_add(pos,pos,m);
  return;
}

void rope_grips_update(void)
{
  rob_rec *rob0;
  rob_rec *rob1;
  int a;
  int b;

  rob0 = rob_get(0);
  rob1 = rob_get(1);
  a = 0;
  b = 0x7f;
  switch (rob0->grip_mode) {
  case 2:
    rob0->grip_link = rope_grip_link_index(rob0,(int)rob0->grip_bone);
  case 1:
    a = (int)*(char *)&rob0->grip_link;
  }
  switch (rob1->grip_mode) {
  case 2:
    rob1->grip_link = rope_grip_link_index(rob1,(int)rob1->grip_bone);
  case 1:
    b = (int)*(char *)&rob1->grip_link;
  }
  rope_span_length = (float)(b - a) * rope_link_length;
}

void rope_link_step(rope_link *link, rope_link *neighbour)
{
  int *q;

  if (link->state == 1) {
    vec3_add_to(&link->pos.x,&link->vel.x);
    q = &link[1].state;
    if (*q != 0) {
      rope_tension_add(&link->pos.x,&link->old.x,&link[1].old.x,link->len_next);
    }
    if (link[-1].state != 0) {
      rope_tension_add(&link->pos.x,&link->old.x,&link[-1].old.x,link->len_prev);
    }
    if (neighbour != 0 && neighbour->state != 0) {
      point_dist_clamp(&link->pos.x,&neighbour->pos.x,link->len_prev);
    }
    vec3_sub(&link->vel.x,&link->pos.x,&link->old.x);
    if (link->pos.y < 4.01) {
      link->pos.y = 4.01;
      link->vel.z = 0;
      link->vel.x = 0;
      link->vel.y = fabsf(link->vel.y) * 0.9;
    }
    else {
      link->vel.y = link->vel.y - rope_cloth_gravity;
    }
  }
}

void rope_update(void)
{
  int origin[3];
  float buf[3];
  rob_rec *rob0;
  rob_rec *rob1;
  int i;
  rope_link *e;
  if (rope_stage_on == 0) return;
  rope_grips_update();
  { rope_link *t; for (i = 126, t = rope_links; i != 0; i--) t[i].state = 1; }
  rob0 = rob_get(0);
  rob1 = rob_get(1);
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = 0;
  mtx_push(0);
  mtx_set_current_wordwise(rob0->bone_mtx + rope_anchor_bone * 0x10);
  mtx_xform_point((float *)origin, &rope_links[0].pos.x);
  mtx_set_current_wordwise(rob1->bone_mtx + rope_anchor_bone * 0x10);
  mtx_xform_point((float *)origin, &rope_links[127].pos.x);
  if (rob0->grip_mode != 0) {
    mtx_set_current_wordwise(rob0->bone_mtx + rob0->grip_bone * 0x10);
    mtx_xform_point((float *)origin, &rope_links[rob0->grip_link].pos.x);
    { rope_link *t = rope_links; t[rob0->grip_link].state = 2; }
  }
  if (rob1->grip_mode != 0) {
    mtx_set_current_wordwise(rob1->bone_mtx + rob1->grip_bone * 0x10);
    mtx_xform_point((float *)origin, &rope_links[rob1->grip_link].pos.x);
    { rope_link *t = rope_links; t[rob1->grip_link].state = 2; }
  }
  mtx_pop(1);
  if (rope_frame & 1) {
    for (i = 1; i < 128; i++) rope_link_step(&rope_links[i], &rope_links[i - 1]);
  } else {
    for (i = 126; i >= 0; i--) rope_link_step(&rope_links[i], &rope_links[i + 1]);
  }
  e = rope_links;
  for (i = 0; i < 128; i++, e++) e->old = e->pos;
  mtx_push(0);
  mtx_set_current_wordwise(rob0->bone_mtx + rope_anchor_bone * 0x10);
  e = rope_links;
  for (i = 0; i < 127; i++, e++) {
    vec_to_local_into(&e[1].pos.x, buf);
    chain_node_rotate(0, buf);
    mtx_store_wordwise(e->m);
    mtx_translate(e->len_next, 0.0, 0.0);
  }
  mtx_pop(1);
  rope_frame++;
}

void rope_draw(void)
{
  int i;
  if (rope_stage_on != 0) {
    mtx_push(0);
    for (i = 0; i < 0x7f; i++) {
      rope_link *e = rope_links;
      rope_link *p = &e[i];
      if (p->state != 0 && p[1].state != 0) {
        char *from = (char *)&rope_links[i].pos;
        char *to = (char *)&rope_links[i + 1].pos;
        mtx_peek_pushed(0);
        if (rope_lines_show != 0) {
          line3d_quads_opaque((float *)from, (float *)to, 0xff00);
        }
        mtx_mul_by(rope_links[i].m);
        rope_link_scale[0] = rope_link_length;
        mtx_scale_by_vec3(rope_link_scale);
        obj_draw(rope_link_model);
      }
    }
    mtx_pop(1);
  }
}

int rope_link_at_distance(rob_rec *rob, float distance)
{
  int c;
  c = (int)(distance / rope_link_length);
  if (rob->side != '\0') {
    c = 0x7f - c;
  }
  return (char)c;
}

int rope_grip_link_index(rob_rec *rob, int bone)
{
  float f = vec3_distance((float *)((int)rob->bone_mtx + (((short)(bone << 8)) >> 2) + 0x30), (float *)((int)rob->bone_mtx + rope_anchor_bone * 0x40 + 0x30));
  return rope_link_at_distance(rob, f);
}
