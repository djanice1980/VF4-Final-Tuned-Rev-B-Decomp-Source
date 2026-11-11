/*
 * vf4lib/librobdisp/src/osage.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_vec3_set_length
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern short demo_flags; /* 0x0c28c7a2 */
extern float mode_timer_rate; /* 0x0c292678 */
extern rob_rec *osage_rob; /* 0x0c2b100c */
extern osage_coli_rec *osage_coli_list; /* 0x0c2ff5cc */
extern uint shun_tint_color; /* 0x0c2ff66c */
extern short shun_tint_mats_0[32]; /* 0x0c2ff670 */
extern short shun_tint_mats_1[24]; /* 0x0c2ff6b0 */
extern int shun_tint_scale; /* 0x0c2ff6e0 */
extern float cloth_rest_pull; /* 0x0c2ff6e4 */
extern int cloth_reset_steps; /* 0x0c2ff6e8 */
extern int cloth_asm_flags; /* 0x0c2ff6f0 */
extern int bone_chains_enabled; /* 0x0c2ff6f8 */
extern int bone_chain_bone_map[10][2]; /* 0x0c2ff700 */
extern osage_node *osage_node_next; /* 0x0c3a8268 */
extern bone_chain_node *bone_chain_nodes_cur; /* 0x0c3a8278 */
extern float cloth_damping; /* 0x0c3a8284 */
extern float cloth_strength; /* 0x0c3a9a88 */
extern float cloth_side_add; /* 0x0c3a9a8c */
extern int bone_chain_slots[2][16][7]; /* 0x0c3a9a98 */
extern char bone_chain_nodes[2][6656]; /* 0x0c3a9e18 */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern int obj_set_bank[241]; /* 0x0c3b6fc4 */
extern cloth_slot_rec cloth_slots[2][8]; /* 0x0c4dd4a8 */
extern osage_wind_vec osage_wind; /* 0x0c2ff5bc */
extern osage_root_offset_vec osage_root_offset; /* 0x0c2ff5e0 */
extern wind_rec bone_chain_wind; /* 0x0c2ff5f0 */
extern rob_usec_pair rob_chain_usec; /* 0x0c371b18 */

static inline float fsqrt_inline(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}

static inline float fsrra_asm(float x)
{
  __asm__ ("fsrra %0" : "=f" (x) : "0" (x));
  return x;
}

void rob_cloth_add(rob_rec *rob, uint model_id, cloth_param_rec *params)
{
  obj_node *obj;
  uint side;
  int *list;
  cloth_param_rec *p;

  obj = obj_node_get(model_id);
  side = rob->side;
  if (obj == 0) return;
  list = (int *)obj->list_8c;
  if (list == 0) return;
  if (params == 0) params = cloth_params_default;
  while (*list != 0) {
    if (params == 0) p = 0;
    else {
      p = params;
      params++;
    }
    cloth_slot_start(&cloth_slots[side][cloth_slot_count],(obj_node *)*list++,p);
    cloth_slot_count++;
  }
  return;
}

void rob_cloth_init(rob_rec *rob)
{
  cloth_vertex_buf_a = &cloth_vertex_blocks_a + (uint)rob->side * 0x6000;
  cloth_vertex_buf_b = &cloth_vertex_blocks_b + (uint)rob->side * 0x6000;
  cloth_slot_count = 0;
  rob_cloth_clear(rob);
  switch(rob->chara) {
  case 2:
    if (rob->costume == 0) {
      rob_cloth_add(rob,rob->body_model,cloth_params_chara2_0);
    }
    else {
      rob_cloth_add(rob,rob->body_model,cloth_params_chara2_1);
    }
    break;
  case 0xb:
    if (rob->costume == 0) {
      rob_cloth_add(rob,rob->body_model,cloth_params_chara11_0);
    }
    else {
      rob_cloth_add(rob,rob->body_model,cloth_params_chara11_1);
    }
    break;
  case 3:
    if (rob->costume == 0) {
      rob_cloth_add(rob,rob->body_model,cloth_params_chara3_0);
    }
    else {
      rob_cloth_add(rob,rob->body_model,cloth_params_chara3_1);
    }
    break;
  case 5:
  case 7:
    if (rob->costume != 0) break;
  case 0:
  case 1:
    rob_cloth_add(rob,rob->body_model,(cloth_param_rec *)0x0);
    break;
  case 0xf:
    rob_cloth_add(rob,rob->body_model,cloth_params_chara15);
  case 0xe:
    rob_cloth_add(rob,rob->body_model,cloth_params_chara14);
    break;
  }
  rob_cloth_items_init(rob);
}

void rob_cloth_items_init(rob_rec *rob)
{
  int model_id;
  uint slot;
  int i;

  slot = (uint)rob->items;
  i = 0;
  do {
    model_id = rob_item_value_get(rob,i,1,0);
    if (-1 < model_id) {
      rob_cloth_add(rob,model_id,*(cloth_param_rec **)(*(int *)(slot + 4) + 0x10));
    }
    model_id = rob_item_value_get(rob,i,1,1);
    if (-1 < model_id) {
      rob_cloth_add(rob,model_id,*(cloth_param_rec **)(*(int *)(slot + 8) + 0x10));
    }
    i = i + 1;
    slot = slot + 0xc;
  } while (i < 4);
  return;
}

void rob_cloth_clear(rob_rec *rob)
{
  int i;
  int side = rob->side;
  i = 0;
  for (; i < 8; i++)
    cloth_slots[side][i].on = 0;
}

void cloth_clear_all(void)
{
  rob_rec *rob;
  uint side;

  side = 0;
  do {
    rob = rob_get(side & 0xff);
    rob_cloth_clear(rob);
    side = side + 1;
  } while ((int)side < 2);
  return;
}

void cloth_calc_all(void)
{
  rob_rec *rob;
  int n;
  int i;
  obj_node *obj;
  float t;
  float *m;
  cloth_slot_rec *e;

  rob = rob_get(0);
  if ((demo_flags & 2) != 0 && mode_get_main() == MODE_MAIN_ADVERTISE && mode_get_sub() == 8) return;
  if (pause_frame_held() != 0 && deb_sw == 0) return;
  mtx_push(0);
  for (n = 0; n < 2; n++, rob++) {
    t = (float)(int)stopwatch_usec();
    for (i = 0; i < 8; i++) {
      if (cloth_slots[n][i].on != 0) {
        e = &cloth_slots[n][i];
        obj = e->obj;
        osage_rob = rob;
        m = rob->mtx;
        mtx_set_current_wordwise(m + (e->bone << 4));
        if ((obj->type == 9) | (obj->type == 0xf)) {
          if ((rob->flags & 0x100000) != 0) {
            cloth_pose_reset(rob, e);
          }
          cloth_step(rob, &cloth_slots[n][i]);
          perf_color_mark(0x3f3f);
          cloth_mesh_update(&cloth_slots[n][i]);
          perf_color_mark(0);
        }
      }
    }
    rob_cloth_usec[n] = (float)(int)stopwatch_usec() - t;
    rob->flags &= ~0x100000;
  }
  mtx_pop(1);
}

void rob_cloth_draw(rob_rec *rob)
{
  uint side;
  int i;

  side = rob->side;
  for (i = 0; i < 8; i++) {
    if (cloth_slots[side][i].on != 0) {
      if (cloth_grid_show != 0) {
        cloth_grid_lines_draw(&cloth_slots[side][i]);
      }
      else {
        cloth_draw(&cloth_slots[side][i]);
      }
    }
  }
}

void osage_chain_build(rob_rec *rob, osage_chain_slot *chain, int *cmds, osage_param *params)
{
  typedef struct { int w[3]; } vec;
  int more;
  int base;
  osage_joint_desc *joint;
  osage_node *node;
  int *c;
  vec v;
  vec zero;

  more = 1;
  zero.w[0] = 0;
  zero.w[1] = 0;
  zero.w[2] = 0;
  base = (int)rob->mtx;
  chain->on = 1;
  chain->cmds = cmds;
  chain->params = params;
  node = osage_node_next;
  chain->nodes = node;
  c = cmds;
  mtx_push(0);
  do {
    joint = (osage_joint_desc *)c[1];
    switch (*c) {
    case 1:
      node->mtx = 0;
      osage_root_node_calc((chain_root_desc *)c[1],node);
      v = *(vec *)node->pos;
      chain->root_slot = *(int *)c[1];
      node->link = 0;
      node->num = 0xffffffff;
      node = node + 1;
      break;
    case 3:
      node->mtx = (float *)(joint->slot * 0x40 + base);
      *(vec *)node->vel = zero;
      osage_node_init(joint,node,params);
      node->link = 0;
      node->num = joint->param;
      node = node + 1;
      break;
    case 0:
    case 4:
      node->mtx = (float *)(joint->slot * 0x40 + base);
      *(vec *)node->vel = zero;
      osage_node_init(joint,node,params);
      node->link = 0;
      node->num = joint->param;
      node = node + 1;
      more = 0;
      break;
    }
    c = c + 2;
  } while (more);
  mtx_pop(1);
  osage_node_next = node;
  return;
}

void osage_node_init(osage_joint_desc *joint, osage_node *node, osage_param *params)
{
  osage_param *prm;
  float buf[3];
  float out[3];
  float v[3];

  v[0] = 1.0;
  v[1] = 0.0;
  v[2] = 0.0;
  prm = &params[joint->param];
  mtx_push(0);
  mtx_rot_z_s16(prm->rest_rz);
  mtx_rot_y_s16(prm->rest_ry);
  mtx_xform_dir(v,out);
  mtx_pop(1);
  out[1] = out[1] - (world_gravity + prm->gravity);
  vec3_set_length(out,out,joint->length);
  vec3_add(node->pos,node[-1].pos,out);
  osage_coli_list = (osage_coli_rec *)prm->coli_list;
  osage_node_collide(node->pos);
  node->vel[0] = node->vel[1] = node->vel[2] = 0.0;
  vec_to_local_into(node->pos,buf);
  osage_node_rotate((short)prm->rot_type,buf,prm->limit);
  mtx_store_wordwise(node->mtx);
  mtx_translate(joint->length,0.0,0.0);
  mtx_store_translation(node->pos);
}

void osage_chain_reset(osage_chain_slot *chain)
{
  int flag;
  int *p;
  osage_node *d;
  osage_param *params;
  osage_joint_desc *joint;
  int i;

  flag = 1;
  params = chain->params;
  chain->on = 1;
  d = chain->nodes;
  p = chain->cmds;
  mtx_push(0);
  do {
    joint = (osage_joint_desc *)p[1];
    switch (*p) {
    case 1:
      osage_root_node_calc((chain_root_desc *)p[1],d++);
      break;
    case 2:
      osage_coli_list_set(p[1]);
      break;
    case 3:
      osage_node_init(joint,d++,params);
      break;
    case 0:
    case 4:
      osage_node_init(joint,d++,params);
      flag = 0;
      break;
    }
    p += 2;
  } while (flag);
  mtx_pop(1);
  for (i = 0; i < osage_settle_steps; i++) {
    osage_chain_settle_step(chain);
  }
}

void osage_chain_settle_step(osage_chain_slot *chain)
{
  typedef struct { int x, y, z; } v3;
  int more = 1;
  osage_param *params = chain->params;
  v3 zero = {0, 0, 0};
  osage_node *node;
  int *e;
  chain->on = 1;
  node = chain->nodes;
  e = chain->cmds;
  mtx_push(0);
  do {
    osage_joint_desc *joint = (osage_joint_desc *)e[1];
    switch(*e) {
    case 1:
      osage_root_node_calc((chain_root_desc *)e[1],node);
      *(v3 *)node->vel = zero;
      node = node + 1;
      break;
    case 2:
      osage_coli_list_set(e[1]);
      break;
    case 3:
      osage_node_calc(joint,node,params,0);
      *(v3 *)node->vel = zero;
      node = node + 1;
      break;
    case 0:
    case 4:
      osage_node_calc(joint,node,params,1);
      *(v3 *)node->vel = zero;
      node = node + 1;
      more = 0;
      break;
    }
    e = e + 2;
  } while (more);
  mtx_pop(1);
}

void osage_chain_calc(osage_chain_slot *chain)
{
  int go;
  int *p;
  osage_node *v;
  osage_param *params;

  go = 1;
  params = chain->params;
  osage_coli_list = 0;
  v = chain->nodes;
  p = chain->cmds;
  mtx_push(0);
  do {
    switch(*p) {
    case 1:
      osage_root_node_calc((chain_root_desc *)p[1],v);
      v++;
      break;
    case 2:
      osage_coli_list_set(p[1]);
      break;
    case 3:
      osage_node_calc((osage_joint_desc *)p[1],v++,params,0);
      break;
    case 0:
      osage_node_calc((osage_joint_desc *)p[1],v++,params,1);
      go = 0;
      break;
    }
    p = p + 2;
  } while (go);
  mtx_pop(1);
}

void osage_root_node_calc(chain_root_desc *root, osage_node *node)
{
  uint *v = (uint *)node->pos;
  v[0] = osage_root_offset.x;
  v[1] = osage_root_offset.y;
  v[2] = osage_root_offset.z;
  vec3_add_to((float *)v,root->offset);
  mtx_xform_point((float *)v,(float *)v);
  mtx_overwrite_translation((float *)v);
  if ((chain_table_flags & 2) == 0) {
    mtx_rot_z_s16(root->rot_z);
    mtx_rot_y_s16(root->rot_y);
    mtx_rot_x_s16(root->rot_x);
  }
  else {
    struct { short x, y, z; } *a = (void *)&chain_angle_table;
    mtx_rot_z_s16(a[chain_table_index].z);
    mtx_rot_y_s16(a[chain_table_index].y);
    mtx_rot_x_s16(a[chain_table_index].x);
    chain_table_index = chain_table_index + 3;
  }
}

void osage_node_calc(osage_joint_desc *joint, osage_node *node, osage_param *params, int last)
{
  uint angles;
  float *pos;
  float *cur;
  float *vel;
  float *prev;
  osage_param *prm;
  float damp;
  float t;
  float m[3];
  int v[3];
  float r[3];
  float a[3];

  prm = &params[joint->param];
  pos = node->pos;
  v[0] = ((int *)pos)[0];
  v[1] = ((int *)pos)[1];
  v[2] = ((int *)pos)[2];
  damp = 1.0 / (prm->damping + 1.0 + air_reg);
  vec3_add_to(pos,(float *)&osage_wind);
  vec3_add_to(pos,node->vel);
  if (last == 0) {
    osage_link_constrain_asm(pos,(float *)v,node[1].pos,joint->length,world_gravity + osage_gravity_add);
  }
  pos = node->pos;
  prev = node[-1].pos;
  osage_link_constrain_asm(pos,(float *)v,prev,joint->length,world_gravity + osage_gravity_add);
  a[0] = prm->stiffness;
  a[2] = 0.0;
  a[1] = 0.0;
  mtx_xform_dir(a,r);
  vec3_add_to(pos,r);
  point_dist_clamp_asm(pos,prev,joint->length);
  if (node->link != 0) {
    point_dist_clamp_asm(pos,((osage_node *)node->link)->pos,node->link_len);
  }
  osage_coli_list = (osage_coli_rec *)prm->coli_list;
  if ((osage_coli_off == 0) && (osage_coli_list != 0)) {
    osage_node_collide(node->pos);
  }
  cur = node->pos;
  vec_to_local_into(cur,m);
  angles = osage_node_rotate((short)prm->rot_type,m,(int)prm->limit);
  mtx_store_wordwise(node->mtx);
  mtx_translate(joint->length,0.0,0.0);
  mtx_store_translation(cur);
  node->ang[0] = (short)(angles >> 0x10);
  node->ang[1] = (short)angles;
  vel = node->vel;
  vec3_sub(vel,cur,(float *)v);
  *vel = *vel * damp;
  t = damp * node->vel[1];
  node->vel[1] = t;
  node->vel[2] = node->vel[2] * damp;
  node->vel[1] = t - (world_gravity + prm->gravity);
}

uint osage_node_rotate(int rot_type,float *dir,int limit)
{
  int y;
  int x;
  float d;

  y = 0;
  x = 0;
  switch (rot_type) {
  case 0:
    y = atan2_s16(-dir[2],dir[0]);
    d = fsqrt_asm(dir[0] * dir[0] + dir[2] * dir[2]);
    x = atan2_s16(dir[1],d);
    if (limit != 0) {
      if (y > limit) y = (short)limit;
      if (y < -limit) y = (short)-limit;
      if (x > limit) x = (short)limit;
      if (x < -limit) x = (short)-limit;
    }
    mtx_rot_y_s16(y);
    mtx_rot_z_s16(x);
    break;
  case 1:
    x = atan2_s16(dir[1],dir[0]);
    d = fsqrt_asm(dir[1] * dir[1] + dir[0] * dir[0]);
    y = atan2_s16(-dir[2],d);
    if (limit != 0) {
      if (x > limit) x = (short)limit;
      if (x < -limit) x = (short)-limit;
      if (y > limit) y = (short)limit;
      if (y < -limit) y = (short)-limit;
    }
    mtx_rot_z_s16(x);
    mtx_rot_y_s16(y);
    break;
  case 2:
    y = atan2_s16(-dir[2],dir[0]);
    if (limit != 0) {
      if (y > limit) y = (short)limit;
      if (y < -limit) y = (short)-limit;
    }
    mtx_rot_y_s16(y);
    break;
  case 3:
    x = atan2_s16(dir[1],dir[0]);
    if (limit != 0) {
      if (x > limit) x = (short)limit;
      if (x < -limit) x = (short)-limit;
    }
    mtx_rot_z_s16(x);
    break;
  }
  return y << 16 | x;
}

void osage_node_sag_apply(float *point, float *old, float *neighbour, float length)
{
  float a[3];
  float b[3];
  float t;

  vec3_sub(a,neighbour,old);
  t = (-a[1] * (world_gravity + osage_gravity_add)) / length;
  vec3_scale(b,a,t);
  vec3_add(point,point,b);
  point_dist_clamp_asm(point,neighbour,length);
}

void osage_chain_models_draw(osage_chain_slot *chain)
{
  int *p;
  osage_node *q;
  q = chain->nodes;
  q++;
  for (p = chain->cmds; ; p += 2) {
    switch (*p) {
    case 3:
      osage_node_model_draw((osage_joint_desc *)p[1], q++);
      break;
    case 0:
    case 4:
      osage_node_model_draw((osage_joint_desc *)p[1], q);
      return;
    }
  }
}

void osage_node_model_draw(osage_joint_desc *joint, osage_node *node)
{
  if (0 < joint->model_id) {
    mtx_push(0);
    mtx_mul_by(node->mtx);
    obj_draw(joint->model_id);
    mtx_pop(1);
  }
}

void osage_chain_relocate(int *cmds, int delta)
{
  int op;
  for (;;) {
    op = *cmds;
    switch (op) { case 0: case 1: case 3: cmds[1] += delta; }
    if (op == 0) { cmds += 2; break; }
    cmds += 2;
  }
}

void chain_slot_build_straight(osage_chain_slot *chain, int *cmds)
{
  int loop = 1;
  bone_chain_node *e;
  int *p;
  vec3 pos;
  vec3 rot = {0, 0, 0};

  chain->on = 1;
  chain->cmds = cmds;
  e = bone_chain_nodes_cur;
  chain->nodes = e;
  p = cmds;
  do {
    switch (*p) {
    case 1:
      e->mtx = e->m;
      chain_root_node_calc((chain_root_desc *)p[1], e);
      pos = e->pos;
      chain->root_slot = *(int *)p[1];
      e->link = 0;
      e++;
      break;
    case 3:
      e->mtx = e->m;
      mtx_load_identity();
      mtx_translate_by_vec3(&pos.x);
      mtx_store_wordwise(e->mtx);
      e->pos = pos;
      e->vel = rot;
      pos.y -= ((chain_joint_desc *)p[1])->length;
      e->link = 0;
      e++;
      break;
    case 0:
    case 4:
      e->mtx = e->m;
      mtx_load_identity();
      mtx_translate_by_vec3(&pos.x);
      mtx_store_wordwise(e->mtx);
      e->pos = pos;
      e->vel = rot;
      e->link = 0;
      e++;
      loop = 0;
      break;
    }
    p += 2;
  } while (loop);
  bone_chain_nodes_cur = e;
}

void chain_slot_build(rob_rec *rob, osage_chain_slot *chain, int *cmds)
{
  typedef struct { int w[3]; } v3;
  int more;
  v3 tmp;
  v3 zero;
  int base;
  bone_chain_node *node;
  chain_joint_desc *joint;
  int *p;

  more = 1;
  zero.w[0] = 0;
  zero.w[1] = 0;
  zero.w[2] = 0;
  base = (int)rob->mtx;
  chain->on = 1;
  chain->cmds = cmds;
  node = bone_chain_nodes_cur;
  chain->nodes = node;
  p = cmds;
  mtx_push(0);
  do {
    joint = (chain_joint_desc *)p[1];
    switch(*p) {
    case 1:
      node->mtx = (float *)(joint->slot * 0x40 + base);
      chain_root_node_calc((chain_root_desc *)p[1],node);
      tmp = *(v3 *)&node->pos;
      chain->root_slot = *(int *)p[1];
      node->link = 0;
      node = node + 1;
      break;
    case 3:
      node->mtx = (float *)(joint->slot * 0x40 + base);
      *(v3 *)&node->vel = zero;
      chain_node_init(joint,node);
      node->link = 0;
      node = node + 1;
      break;
    case 0:
    case 4:
      node->mtx = (float *)(joint->slot * 0x40 + base);
      *(v3 *)&node->vel = zero;
      chain_node_init(joint,node);
      node->link = 0;
      node = node + 1;
      more = 0;
      break;
    }
    p = p + 2;
  } while (more);
  mtx_pop(1);
  bone_chain_nodes_cur = node;
  return;
}

void chain_node_init(chain_joint_desc *joint, bone_chain_node *node)
{
  float *p;
  float m[3];
  float out[3];
  float v[3] = { 0.0, 0.0, 0.0 };
  node->vel.x = node->vel.y = node->vel.z = 0.0;
  mtx_push(0);
  v[0] = joint->stiffness;
  mtx_rot_z_s16(joint->rot_z);
  mtx_rot_y_s16(joint->rot_y);
  mtx_xform_dir(v,out);
  mtx_pop(1);
  out[1] -= world_gravity * chain_weight;
  vec3_set_length(out,out,joint->length);
  p = &node->pos.x;
  vec3_add(p,&node[-1].pos.x,out);
  point_dist_clamp(p,&node[-1].pos.x,joint->length);
  vec_to_local_into(p,m);
  chain_node_rotate(chain_rot_type,m);
  mtx_store_wordwise(node->mtx);
  mtx_translate(joint->length,0.0,0.0);
  mtx_store_translation(p);
  osage_node_collide(p);
}

void chain_reset(osage_chain_slot *chain)
{
  int go;
  bone_chain_node *p;
  int *e;
  int desc;
  int i;
  float buf[16];
  go = 1;
  chain->on = 1;
  p = chain->nodes;
  e = chain->cmds;
  mtx_push(0);
  do {
    desc = e[1];
    switch (*(uint *)e) {
    case 1:
      chain_root_node_calc((chain_root_desc *)e[1],p++);
      break;
    case 3:
      chain_node_init((chain_joint_desc *)desc,p++);
      break;
    case 2:
      osage_coli_list_set(e[1]);
      break;
    case 0:
    case 4:
      chain_node_init((chain_joint_desc *)desc,p++);
      go = 0;
      break;
    }
    e += 2;
  } while (go);
  mtx_pop(1);
  bone_chain_nodes_cur = p;
  mtx_store_wordwise(buf);
  for (i = 0; i < osage_settle_steps; i++) {
    mtx_set_current_wordwise(buf);
    chain_settle_step(chain);
  }
}

void chain_settle_step(osage_chain_slot *chain)
{
  typedef struct { int w[3]; } v3;
  int more = 1;
  v3 z = {0, 0, 0};
  int *e;
  bone_chain_node *node;
  chain_joint_desc *joint;

  chain->on = 1;
  node = chain->nodes;
  e = chain->cmds;
  do {
    joint = (chain_joint_desc *)e[1];
    switch(*e) {
    case 1:
      chain_root_node_calc((chain_root_desc *)e[1],node);
      node = node + 1;
      break;
    case 3:
      chain_node_calc(joint,node,0);
      *(v3 *)&node->vel = z;
      node = node + 1;
      break;
    case 2:
      osage_coli_list_set(e[1]);
      break;
    case 0:
    case 4:
      chain_node_calc(joint,node,1);
      *(v3 *)&node->vel = z;
      node = node + 1;
      more = 0;
      break;
    }
    e = e + 2;
  } while (more);
  bone_chain_nodes_cur = node;
}

void chain_calc(osage_chain_slot *chain)
{
  int done;
  bone_chain_node *q;
  int *p;
  done = 1;
  osage_coli_list = 0;
  q = chain->nodes;
  p = chain->cmds;
  do {
    switch (*p) {
    case 1:
      chain_root_node_calc((chain_root_desc *)p[1], q);
      q++;
      break;
    case 2:
      osage_coli_list_set(p[1]);
      break;
    case 3:
      chain_node_calc((chain_joint_desc *)p[1], q++, 0);
      break;
    case 0:
      chain_node_calc((chain_joint_desc *)p[1], q++, 1);
      done = 0;
      break;
    }
    p += 2;
  } while (done);
}

void chain_root_node_calc(chain_root_desc *root, bone_chain_node *node)
{
  typedef struct { short x; short y; short z; } R;
  int *v;

  v = (int *)&node->pos;
  v[0] = osage_root_offset.x;
  v[1] = osage_root_offset.y;
  v[2] = osage_root_offset.z;
  vec3_add_to((float *)v,root->offset);
  mtx_xform_point((float *)v,(float *)v);
  chain_rot_type = (int)root->rot_type;
  chain_limit = (int)root->limit;
  chain_root_1a = (int)root->field_0x1a;
  chain_weight = root->weight;
  mtx_overwrite_translation((float *)v);
  if ((chain_table_flags & 2) == 0) {
    mtx_rot_z_s16((int)root->rot_z);
    mtx_rot_y_s16(root->rot_y);
    mtx_rot_x_s16(root->rot_x);
  }
  else {
    R *r = (R *)&chain_angle_table;
    mtx_rot_z_s16((int)r[chain_table_index].z);
    mtx_rot_y_s16(r[chain_table_index].y);
    mtx_rot_x_s16(r[chain_table_index].x);
    chain_table_index = chain_table_index + 3;
  }
  return;
}

void chain_node_calc(chain_joint_desc *joint, bone_chain_node *node, int last)
{
  typedef struct { float x, y, z; } vec;
  typedef struct { short z; short y; } ang;
  float *pos;
  float *b;
  float vy;
  float damp;
  float local[3];
  vec v;
  vec d;
  vec l;

  pos = &node->pos.x;
  v = *(vec *)&node->pos;
  vec3_add_to(pos,(float *)&osage_wind);
  vec3_add_to(pos,&node->vel.x);
  if (last == 0) {
    chain_node_sag_apply(pos,&v.x,&node[1].pos.x,joint->length);
  }
  chain_node_sag_apply(&node->pos.x,&v.x,&node[-1].pos.x,joint->length);
  mtx_push(0);
  if ((chain_table_flags & 1) == 0) {
    l.x = joint->stiffness;
    mtx_rot_z_s16((int)joint->rot_z);
    mtx_rot_y_s16(joint->rot_y);
  }
  else {
    l.x = chain_stiffness_table[chain_table_index] * 0.1;
    mtx_rot_z_s16((int)((ang *)&chain_angle_table)[chain_table_index].z);
    mtx_rot_y_s16(((ang *)&chain_angle_table)[chain_table_index].y);
    chain_table_index = chain_table_index + 1;
  }
  l.z = 0.0;
  l.y = 0.0;
  mtx_xform_dir(&l.x,&d.x);
  mtx_pop(1);
  b = &node->pos.x;
  vec3_add_to(b,&d.x);
  point_dist_clamp(b,&node[-1].pos.x,joint->length);
  if (node->link != 0) {
    point_dist_clamp(b,&node->link->pos.x,node->link_len);
  }
  vec_to_local_into(&node->pos.x,local);
  chain_node_rotate(chain_rot_type,local);
  mtx_store_wordwise(node->mtx);
  mtx_translate(joint->length,0.0,0.0);
  mtx_store_translation(&node->pos.x);
  if (coli_flag != 0) {
    osage_coli_list = coli_flag_list;
  }
  osage_node_collide(&node->pos.x);
  vec3_sub(&node->vel.x,&node->pos.x,&v.x);
  damp = 1.0 / (air_reg + 1.0);
  node->vel.x = node->vel.x * damp;
  vy = node->vel.y;
  node->vel.y = damp * vy;
  node->vel.z = damp * node->vel.z;
  node->vel.y = damp * vy - (world_gravity * chain_weight + osage_gravity_add);
  return;
}

void chain_node_sag_apply(float *point, float *old, float *neighbour, float length)
{
  float a[3];
  float b[3];

  vec3_sub(a,neighbour,old);
  vec3_set_length((-a[1] * (world_gravity * chain_weight + osage_gravity_add)) / length,b,a);
  vec3_add(point,point,b);
  point_dist_clamp(point,neighbour,length);
  return;
}

void chain_node_rotate(int rot_type,float *dir)
{
  int rx;
  int ry;
  int rz;
  float len;
  switch (rot_type) {
  case 0:
    ry = atan2_s16(-dir[2],dir[0]);
    len = fsqrt_inline(dir[0] * dir[0] + dir[2] * dir[2]);
    rz = atan2_s16(dir[1],len);
    if (chain_limit != 0) {
      if (ry > chain_limit) ry = (short)chain_limit;
      if (ry < -chain_limit) ry = (short)-chain_limit;
      if (rz > chain_limit) rz = (short)chain_limit;
      if (rz < -chain_limit) rz = (short)-chain_limit;
    }
    mtx_rot_y_s16(ry);
    mtx_rot_z_s16(rz);
    break;
  case 1:
    rz = atan2_s16(dir[1],dir[0]);
    len = fsqrt_inline(dir[1] * dir[1] + dir[0] * dir[0]);
    ry = atan2_s16(-dir[2],len);
    if (chain_limit != 0) {
      if (rz > chain_limit) rz = (short)chain_limit;
      if (rz < -chain_limit) rz = (short)-chain_limit;
      if (ry > chain_limit) ry = (short)chain_limit;
      if (ry < -chain_limit) ry = (short)-chain_limit;
    }
    mtx_rot_z_s16(rz);
    mtx_rot_y_s16(ry);
    break;
  case 4:
    rx = atan2_s16(dir[2],dir[1]);
    len = fsqrt_inline(dir[1] * dir[1] + dir[2] * dir[2]);
    rz = atan2_s16(dir[0],len);
    mtx_rot_x_s16(rx);
    mtx_rot_z_s16(0x4000 - rz);
    break;
  case 5:
    rx = atan2_s16(dir[1],-dir[2]);
    len = fsqrt_inline(dir[1] * dir[1] + dir[2] * dir[2]);
    ry = atan2_s16(dir[0],len);
    mtx_rot_x_s16(rx);
    mtx_rot_y_s16(0x4000 - ry);
    break;
  case 2:
    ry = atan2_s16(-dir[2],dir[0]);
    if (chain_limit != 0) {
      if (ry > chain_limit) ry = (short)chain_limit;
      if (ry < -chain_limit) ry = (short)-chain_limit;
    }
    mtx_rot_y_s16(ry);
    break;
  case 3:
    rz = atan2_s16(dir[1],dir[0]);
    if (chain_limit != 0) {
      if (rz > chain_limit) rz = (short)chain_limit;
      if (rz < -chain_limit) rz = (short)-chain_limit;
    }
    mtx_rot_z_s16(rz);
    break;
  }
}

void osage_coli_list_set(int list)
{
  osage_coli_list = (osage_coli_rec *)list;
  return;
}

int osage_node_collide(float *point)
{
  int hits;
  float *pf;
  float floor;
  
  hits = 0;
  if (osage_coli_list != 0) {
    mtx_push(0);
    if (osage_coli_debug_on == 0) {
      hits = coli_list_push_point(point,(uint)osage_rob->mtx,(uint)osage_coli_list);
    }
    else {
      hits = coli_list_push_point(point,(uint)osage_rob->mtx,(uint)&type_0c2fedbc);
    }
    mtx_pop(1);
    pf = point + 1;
    floor = osage_rob->floor_y + 0.01;
    if (*pf < floor) {
      *pf = floor;
      hits = hits + 1;
    }
  }
  return hits;
}

void chain_models_draw(osage_chain_slot *chain)
{
  bone_chain_node *q;
  int *p;

  q = chain->nodes;
  q++;
  for (p = chain->cmds; ; p += 2) {
    switch (*p) {
    case 3:
      bone_chain_part_draw((chain_joint_desc *)p[1], q++);
      break;
    case 0:
    case 4:
      bone_chain_part_draw((chain_joint_desc *)p[1], q);
      return;
    }
  }
}

void bone_chain_part_draw(chain_joint_desc *joint, bone_chain_node *node)
{
  if (0 < joint->model_id) {
    mtx_push(0);
    mtx_mul_by(node->mtx);
    obj_draw(joint->model_id);
    mtx_pop(1);
  }
  return;
}

void bone_chain_wind_update(void)
{
  wind_rec *w;
  float v[3];
  float a;
  int i;

  ((int *)v)[0] = 0;
  ((int *)v)[1] = 0;
  v[2] = bone_chain_wind.bias;
  for (i = 0; i < 8; i++) {
    wind_rec *s = &bone_chain_wind;
    float *ph = &s->phase[i];
    float *am;
    a = s->cycle[i] * 0.104719736 + *ph;
    am = &s->ratio[i];
    v[2] += (sin_s16((short)((a * 65536.0) / 6.2831855 + 0.5)) + 1.0) * *am;
    *ph = a;
  }
  mtx_push_identity();
  w = &bone_chain_wind;
  mtx_rot_y_s16(w->yang);
  mtx_rot_x_s16(w->xang);
  mtx_xform_dir(v,&w->vec.x);
  mtx_pop(1);
}

uint argb_mul(uint a, uint b)
{
  uint b3 = b >> 24;
  uint b2 = (b & 0xff0000) >> 16;
  uint b1 = (b & 0xff00) >> 8;
  uint b0 = b & 0xff;
  uint a3 = a >> 24;
  uint a2 = (a & 0xff0000) >> 16;
  uint a1 = (a & 0xff00) >> 8;
  uint a0 = a & 0xff;
  a3 *= b3;
  a2 *= b2;
  a1 *= b1;
  a0 *= b0;
  return (a3 & 0xff00) << 16 | (a2 & 0xff00) << 8 | (a1 & 0xff00) | (a0 & 0xff00) >> 8;
}

void model_material_tint(obj_node *node, compiled_model_rec *model, uint material, uint color)
{
  uint *p;
  uint ofs;
  uint base_color;
  char *d;
  uint c;
  if (model != 0) {
    p = (uint *)(node->tex_refs_offset + (int)node->file);
    while (*p != 0) {
      if (material == p[3]) {
        ofs = *p;
        base_color = p[1];
        c = argb_mul(base_color,color);
        d = (char *)model + (ofs >> 2) * 4;
        *(uint *)(d - 0x54) = c;
        c = argb_mul(base_color,color);
        *(uint *)(d - 0x4c) = c;
      }
      p = p + 4;
    }
  }
}

void obj_set_materials_tint(int set, short *materials, uint color)
{
  uint c = color;
  short *s;
  obj_node *p;
  obj_node **q;
  short *l = materials;
  if (s || p)
  {
    q = objbanks[set].nodes;
  }
  else
  {
    q = objbanks[set].nodes;
  }
  while ((p = *(q++)) != 0)
  {
    for (s = l; (*s) != (-1); s += 8)
    {
      model_material_tint(p, p->model, *s, c);
      model_material_tint(p, p->model2, *s, c);
    }

  }

}

void obj_model_copies_tint(uint obj_id, short *materials, compiled_model_rec **models, uint color)
{
  obj_node *o = obj_node_get(obj_id);
  int i;
  short *p;
  short *s = materials;
  if (o != 0) {
    for (i = 0; i < 0x20; i++) {
      for (p = s; *p != -1; p += 8) model_material_tint(o, *models, *p, color);
      models++;
    }
  }
}

void shun_drink_tint_apply(rob_rec *rob)
{
  int country;
  int set;
  int v;

  set = obj_set_bank[(short)(rob->body_model >> 0x10)];
  country = system_param_country_get();
  if (country == 0) {
    v = (int)((0x28 - rob->attack_rate) * shun_tint_scale) / 0x28 - shun_tint_scale + 0xff;
    if (v > 0xff) {
      v = 0xff;
    }
    shun_tint_color = v << 8 | v | 0xff0000;
    if (rob->costume == 0) {
      obj_set_materials_tint(set,shun_tint_mats_0,shun_tint_color);
      obj_model_copies_tint(0x580014,shun_tint_mats_0,rob_hand_models[rob->side],shun_tint_color);
    }
    else {
      obj_set_materials_tint(set,shun_tint_mats_1,shun_tint_color);
      obj_model_copies_tint(0x590014,shun_tint_mats_1,rob_hand_models[rob->side],shun_tint_color);
    }
  }
}

void cloth_slot_start_by_id(cloth_slot_rec *slot, cloth_source_rec *src, int map_order)
{
  typedef struct { float v[3]; } v3;
  obj_node *r;
  int bad;
  int header;
  float *d;
  float *s;

  if (src->obj_id == 0) return;
  r = obj_node_get(src->obj_id);
  bad = (r == 0) | (r == (obj_node *)-0x28);
  if (bad) return;
  if (cloth_obj->type == 9)
    cloth_header_ofs = (int *)&cloth_obj->unknown_40;
  else if (cloth_obj->type == 0xf)
    cloth_header_ofs = (int *)&cloth_obj->list_80;
  else
    return;
  if (*cloth_header_ofs != 0) {
    header = *cloth_header_ofs + (int)cloth_obj->file;
    slot->header = (cloth_header_rec *)header;
    slot->on = 1;
    slot->obj_id = src->obj_id;
    slot->obj = cloth_obj;
    slot->bone = src->bone;
    d = &slot->strength;
    s = &src->strength;
    *d = *s; d++; s++;
    *d = *s; d++; s++;
    *d = *s;
    slot->source = src;
    slot->map_order = map_order;
    slot->coli = src->coli;
    cloth_nodes_place(r,(cloth_header_rec *)header);
  }
}

void cloth_slot_start(cloth_slot_rec *slot, obj_node *obj, cloth_param_rec *params)
{
  int header;
  float *d;
  volatile float *s;

  if ((obj == (obj_node *)0x0) | (obj == (obj_node *)0xffffffd8)) return;
  cloth_obj = obj;
  if (obj->type == 9) {
    cloth_header_ofs = (int *)&obj->unknown_40;
  }
  else {
    if (cloth_obj->type != 0xf) return;
    cloth_header_ofs = (int *)&cloth_obj->list_80;
  }
  if (*cloth_header_ofs != 0) {
    header = *cloth_header_ofs + (int)cloth_obj->file;
    slot->header = (cloth_header_rec *)header;
    slot->on = 1;
    slot->obj_id = 0;
    slot->obj = cloth_obj;
    slot->bone = 0;
    d = &slot->strength;
    s = (volatile float *)&params->strength;
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    slot->map_order = params->map_order;
    slot->coli = params->coli;
    cloth_nodes_place(obj,(cloth_header_rec *)header);
  }
}

void cloth_nodes_place(obj_node *obj, cloth_header_rec *header)
{
  typedef struct { float x, y, z; } vec;
  cloth_anchor_rec *anchor;
  cloth_node_rec *node;
  int i;
  int j;
  float *f;

  cloth_obj = obj;
  anchor = (cloth_anchor_rec *)((int)obj->file + header->anchors);
  node = (cloth_node_rec *)((int)obj->file + header->nodes);
  for (i = 0; i < header->cols; i++) {
    mtx_xform_point(anchor->pos,node->pos);
    mtx_xform_dir(anchor->dir,node->normal);
    anchor++;
    node++;
  }
  for (j = 1; j < header->rows; j++) {
    for (i = 0; i < header->cols; i++) {
      *(vec *)node->pos = *(vec *)node->rest;
      f = node->normal;
      *f++ = 0.0;
      *f++ = 0.0;
      *f++ = 1.0;
      *f++ = 0.0;
      *f++ = 0.0;
      *f = 0.0;
      node++;
    }
  }
}

void cloth_pose_reset(rob_rec *rob, cloth_slot_rec *slot)
{
  typedef struct { float x, y, z; } vec3f;
  float pos[3];
  float acc[16];
  float tmp[16];
  cloth_node_rec *cur;
  cloth_header_rec *header;
  int n;
  int bind_base;
  int file_base;
  int anchor;
  cloth_node_rec *base;
  int i, j, k;
  float *mtx_base;

  if (slot->on != 1) return;
  header = slot->header;
  file_base = (int)slot->obj->file;
  anchor = file_base + header->anchors;
  base = (cloth_node_rec *)(file_base + header->nodes);
  bind_base = file_base + header->bind_mtx;
  n = header->cols;
  cloth_strength = slot->strength;
  if (deb_sw != 0) {
    cloth_strength = slot->strength + strength;
  }
  cloth_side_add = slot->side_add + yoko;
  cur = base;
  mtx_push_identity();
  for (i = 0; i < n; i++) {
    mtx_base = rob->mtx;
    for (j = 0; j < 16; j++) {
      acc[j] = 0.0;
    }
    for (k = 0; k < 4; k++) {
      int *e = (int *)(anchor + k * 16);
      int bone = *e++;
      float weight = *(float *)e++;
      int bind = *e;
      if (bone < 1) break;
      mtx_set_current_wordwise(mtx_base + bone * 0x10);
      mtx_mul_by((float *)(bind_base + bind * 0x40));
      mtx_store_wordwise(tmp);
      for (j = 0; j < 16; j++) {
        acc[j] = weight * tmp[j] + acc[j];
      }
    }
    mtx_set_current_wordwise(acc);
    mtx_xform_point(((cloth_anchor_rec *)anchor)->pos, pos);
    vec3_sub(cur->vel, pos, cur->pos);
    *(vec3f *)cur->pos = *(vec3f *)pos;
    mtx_xform_dir(((cloth_anchor_rec *)anchor)->dir, cur->normal);
    cur++;
    anchor = anchor + 0x98;
  }
  for (k = 1; k < header->rows; k++) {
    for (i = 0; i < n; i++) {
      float *p = base[k * n + i].pos;
      float f;
      float *py;
      mtx_xform_point(base[k * n + i].rest, p);
      if (slot->coli != 0) {
        coli_list_push_point(p, (uint)osage_rob->mtx, (uint)slot->coli);
      }
      py = &p[1];
      f = osage_rob->floor_y + 0.01;
      if (*py < f) *py = f;
      base[k * n + i].vel[0] = 0.0;
      base[k * n + i].vel[1] = 0.0;
      base[k * n + i].vel[2] = 0.0;
    }
  }
  mtx_pop(1);
  for (k = 0; k < cloth_reset_steps; k++) {
    cloth_step(rob, slot);
  }
}

void cloth_step_unskinned(cloth_slot_rec *slot)
{
  typedef struct { float x; float y; float z; } V;
  float one = 1.0;
  int base;
  int anchor;
  int node;
  int n;
  int i;
  int j;
  cloth_header_rec *header;
  V tmp;

  if (slot->on == 1) {
    header = slot->header;
    anchor = (int)slot->obj->file + header->anchors;
    base = (int)slot->obj->file + header->nodes;
    n = header->cols;
    cloth_damping = one / (slot->air + one + air_reg);
    cloth_strength = slot->strength;
    if (deb_sw != 0) {
      cloth_strength = slot->strength + strength;
    }
    cloth_side_add = slot->side_add + yoko;
    node = base;
    for (i = 0; i < n; i++) {
      mtx_xform_point(((cloth_anchor_rec *)anchor)->pos,(float *)&tmp);
      vec3_sub(((cloth_node_rec *)node)->vel,(float *)&tmp,((cloth_node_rec *)node)->pos);
      *(V *)((cloth_node_rec *)node)->pos = tmp;
      mtx_xform_dir(((cloth_anchor_rec *)anchor)->dir,((cloth_node_rec *)node)->normal);
      node = node + 0x50;
      anchor = anchor + 0x98;
    }
    for (j = 1; j < header->rows; j++) {
      for (i = 0; i < n; i++) {
        cloth_node_move(slot,(cloth_node_rec *)(base + (j * n + i) * 0x50),cloth_points[i].pos);
      }
      for (i = 0; i < n; i++) {
        int o = (j * n + i) * 0x50; cloth_point *t = cloth_points; *(V *)((cloth_node_rec *)(o + base))->pos = *(V *)t[i].pos;
      }
    }
  }
}

void cloth_step(rob_rec *rob, cloth_slot_rec *slot)
{
  float pos[3];
  float acc[16];
  float tmp[16];
  float one;
  int base;
  int cur;
  cloth_header_rec *header;
  int file_base;
  int anchor;
  int bind_base;
  int n;
  int i, j, k;
  float *mtx_base;
  float *v;
  typedef struct { float x, y, z; } vec3f;

  one = 1.0;
  if (slot->on == 1) {
    header = slot->header;
    if (slot->obj != 0) {
      file_base = (int)slot->obj->file;
      anchor = file_base + header->anchors;
      base = header->nodes + file_base;
      bind_base = file_base + header->bind_mtx;
      n = header->cols;
      cloth_damping = one / (slot->air + one + air_reg);
      cloth_strength = slot->strength;
      if (deb_sw != 0) {
        cloth_strength = slot->strength + strength;
      }
      cloth_side_add = slot->side_add + yoko;
      cur = base;
      mtx_push_identity();
      for (i = 0; i < n; i++) {
        mtx_base = rob->mtx;
        for (j = 0; j < 16; j++) {
          acc[j] = 0.0;
        }
        for (k = 0; k < 4; k++) {
          int *e = (int *)(anchor + k * 16);
          int bone = *e++;
          float weight = *(float *)e++;
          int bind = *e;
          if (bone < 1) break;
          mtx_set_current_wordwise(mtx_base + bone * 0x10);
          mtx_mul_by((float *)(bind_base + bind * 0x40));
          mtx_store_wordwise(tmp);
          for (j = 0; j < 16; j++) {
            acc[j] = weight * tmp[j] + acc[j];
          }
        }
        mtx_set_current_wordwise(acc);
        mtx_xform_point(((cloth_anchor_rec *)anchor)->pos, pos);
        vec3_sub(((cloth_node_rec *)cur)->vel, pos, ((cloth_node_rec *)cur)->pos);
        *(vec3f *)((cloth_node_rec *)cur)->pos = *(vec3f *)pos;
        mtx_xform_dir(((cloth_anchor_rec *)anchor)->dir, ((cloth_node_rec *)cur)->normal);
        cur = cur + 0x50;
        anchor = anchor + 0x98;
      }
      for (k = 1; k < header->rows; k++) {
        for (i = 0; i < n; i++) {
          cloth_node_move(slot, (cloth_node_rec *)(base + (k * n + i) * 0x50), (float *)cloth_points + i * 6);
        }
        for (i = 0; i < n; i++) {
          int d = (k * n + i) * 0x50 + base;
          float *s = (float *)cloth_points;
          *(vec3f *)((cloth_node_rec *)d)->pos = *(vec3f *)(s + i * 6);
        }
      }
      mtx_pop(1);
    }
  }
}

void cloth_node_move(cloth_slot_rec *slot, cloth_node_rec *node, float *out)
{
  typedef struct { float v[3]; } v3;
  int cols = slot->header->cols;
  osage_coli_rec *coli;
  float floor;
  float *y;
  v3 cur;
  v3 old;
  float t[3];

  cur = old = *(v3 *)node->pos;
  if ((cloth_asm_flags & 1) != 0) {
    cloth_node_integrate_asm((uint *)node,cur.v,(float *)&osage_wind,cols,cloth_strength);
  }
  else {
    cloth_node_integrate(node,cur.v,(float *)&osage_wind,cols,cloth_strength);
  }
  if (0.0 < cloth_rest_pull) {
    mtx_xform_point(node->rest,t);
    point_dist_clamp_asm(cur.v,t,cloth_rest_pull);
  }
  if (coli_flag_0c2ff6ec != 0) coli = (osage_coli_rec *)&type_0c2fedbc;
  else coli = slot->coli;
  if (coli != (osage_coli_rec *)0x0) {
    if ((cloth_asm_flags & 2) != 0) {
      coli_list_push_point(cur.v,(uint)osage_rob->mtx,(uint)coli);
    }
    else {
      osage_coli_push_out(cur.v,osage_rob->mtx,coli);
    }
  }
  y = &cur.v[1];
  floor = osage_rob->floor_y + 0.01;
  if (*y < floor) {
    *y = floor;
  }
  node->vel[0] = (cur.v[0] - old.v[0]) * cloth_damping;
  node->vel[1] = (cur.v[1] - old.v[1]) * cloth_damping - world_gravity * mode_timer_rate;
  node->vel[2] = (cur.v[2] - old.v[2]) * cloth_damping;
  *(v3 *)out = cur;
}

void cloth_node_integrate(cloth_node_rec *node, float *out, float *force, int cols, float scale)
{
  typedef struct { int w[3]; } v3;
  v3 v;
  float m[3];
  v = *(v3 *)node->pos;
  *(v3 *)out = v;
  mtx_xform_dir(node->force_dir, m);
  vec3_scale(m, m, scale);
  vec3_add_to(out, m);
  vec3_add_to(out, node->vel);
  vec3_add_to(out, force);
  if ((node->flags & 2) == 0) cloth_link_constrain(out, (float *)&v, node[cols].pos, node->len_down);
  if ((node->flags & 4) == 0) cloth_link_constrain(out, (float *)&v, node[-1].pos, node->len_left + cloth_side_add);
  if ((node->flags & 8) == 0) cloth_link_constrain(out, (float *)&v, node[1].pos, node->len_right + cloth_side_add);
  cloth_link_constrain(out, (float *)&v, (node - cols)->pos, node->len_up);
}

void cloth_link_constrain(float *point, float *old, float *neighbour, float length)
{
  float a[3];
  float b[3];
  float t;
  vec3_sub(a,neighbour,old);
  t = (-a[1] * rope_cloth_gravity) / length;
  vec3_scale(b,a,t);
  vec3_add(point,point,b);
  point_dist_clamp(point,neighbour,length);
}

void point_dist_clamp(float *point,float *center,float radius)
{
  float *pa, *pb;
  float a, b, c, x, y, z, dx, dy, dz, d, s;
  a = point[0]; x = center[0]; dx = a - x;
  b = point[1]; y = center[1]; dy = b - y;
  pa = point + 2; pb = center + 2; c = *pa; z = *pb; dz = c - z;
  d = dx * dx + dy * dy + dz * dz;
  if (radius * radius < d) {
    s = radius * fsrra_asm(d);
    point[0] = s * dx + x;
    point[1] = s * dy + y;
    *pa = s * dz + z;
  }
}

void cloth_mesh_update(cloth_slot_rec *slot)
{
  typedef struct { int w[3]; } v3;
  cloth_header_rec *header;
  int i;
  int j;
  cloth_node_rec *e;
  v3 *d;

  if ((slot->on == 1) && (slot->obj != 0)) {
    header = slot->header;
    d = (v3 *)cloth_points;
    e = (cloth_node_rec *)(header->nodes + (int)slot->obj->file);
    for (j = 0; j < header->cols; j++) {
      *d++ = *(v3 *)e->pos;
      *d++ = *(v3 *)e->normal;
      e++;
    }
    for (i = 0; i < header->rows; i++) {
      for (j = 0; j < header->cols; j++) {
        *d = *(v3 *)e->pos;
        e++;
        d += 2;
      }
    }
    cloth_model_write(slot->obj,cloth_points,slot->map_order);
  }
}

void cloth_obj_draw(cloth_slot_rec *slot)
{
  node_draw_buffered(slot->obj);
  return;
}

void bone_chains_setup(rob_rec *rob)
{
  int ofs;
  int root_ofs;
  uint side;

  side = (uint)rob->side;
  bone_chain_nodes_cur = (bone_chain_node *)bone_chain_nodes[side];
  bone_chains_clear(rob);
  if (bone_chains_enabled != 0) {
    root_ofs = side * 0x20;
    bone_chain_a_roots[side].offset[0] = rob->rig->bones[8].pos[0] + rob->rig->bones[9].pos[0];
    bone_chain_a_roots[side].offset[2] = rob->rig->bones[8].pos[2] + rob->rig->bones[9].pos[2];
    bone_chain_a_joint1[side].length = rob->rig->bones[11].pos[0];
    bone_chain_a_joint2[side].length = rob->rig->bones[12].pos[0];
    bone_chain_b_roots[side].offset[0] = rob->rig->bones[16].pos[0] + rob->rig->bones[17].pos[0];
    bone_chain_b_roots[side].offset[2] = rob->rig->bones[16].pos[2] + rob->rig->bones[17].pos[2];
    bone_chain_b_joint1[side].length = rob->rig->bones[19].pos[0];
    bone_chain_b_joint2[side].length = rob->rig->bones[20].pos[0];
    {
      chain_root_desc *t = bone_chain_c_roots + side;
      *(vec3 *)t->offset = *(vec3 *)rob->rig->bones[5].pos;
    }
    if (side == 0) {
      chain_slot_build_straight((osage_chain_slot *)&bone_chain_slots[0][0][0],bone_chain_d_cmds_0);
      chain_slot_build_straight((osage_chain_slot *)((char *)&bone_chain_slots[0][0][0] + 0x1c),bone_chain_a_cmds_0);
      chain_slot_build_straight((osage_chain_slot *)((char *)&bone_chain_slots[0][0][0] + 0x38),bone_chain_b_cmds_0);
      chain_slot_build_straight((osage_chain_slot *)((char *)&bone_chain_slots[0][0][0] + 0x54),bone_chain_c_cmds_0);
    }
    else {
      ofs = side * 0x1c0;
      chain_slot_build_straight((osage_chain_slot *)((char *)&bone_chain_slots[0][0][0] + ofs),bone_chain_d_cmds_1);
      chain_slot_build_straight((osage_chain_slot *)((char *)&bone_chain_slots[0][0][0] + 0x1c + ofs),bone_chain_a_cmds_1);
      chain_slot_build_straight((osage_chain_slot *)((char *)&bone_chain_slots[0][0][0] + 0x38 + ofs),bone_chain_b_cmds_1);
      chain_slot_build_straight((osage_chain_slot *)((char *)&bone_chain_slots[0][0][0] + 0x54 + ofs),bone_chain_c_cmds_1);
    }
  }
  return;
}

void bone_chains_clear(rob_rec *rob)
{
  byte side;
  int i;
  int *p;
  side = rob->side;
  i = 0;
  p = &bone_chain_slots[0][0][0];
  for (; i < 0x10; i++) {
    p[i * 7 + side * 0x70] = 0;
  }
  return;
}

void bone_chains_init(void)
{
  int i;
  int side;

  rob_get(0);
  side = 0;
  do {
    i = 0;
    do {
      (&bone_chain_slots[0][0][0])[side * 0x70 + i * 7] = 0;
      i = i + 1;
    } while (i < 0x10);
    mtx_push_identity();
    i = 0;
    do {
      mtx_store_wordwise(((bone_chain_node *)bone_chain_nodes[side])[i].m);
      i = i + 1;
    } while (i < 0x40);
    mtx_pop(1);
    side = side + 1;
  } while (side < 2);
  return;
}

void bone_chains_update(void)
{
  rob_rec *rob;
  int i;
  int j;
  int t0;
  float *mdl;
  rob = rob_get(0);
  if ((((demo_flags & 2U) == 0) || (mode_get_main() != MODE_MAIN_ADVERTISE)) || (mode_get_sub() != 8))
  {
    bone_chain_wind_update();
    chain_table_index = 0;
    mtx_push(0);
    for (i = 0; i < 2; i++, rob++)
    {
      osage_rob = rob;
      mdl = rob->mtx;
      for (j = 0; j < 0x10; j++)
      {
        t0 = stopwatch_usec();
        if ((pause_frame_held() == 0) || (deb_sw != 0))
        {
          char *p0 = (char *) (&bone_chain_slots[0][0][0]);
          int *p = (int *) (p0 + ((i * 0x1c0) + (j * 28)));
          if ((*p) != 0)
          {
            mtx_set_current_wordwise(mdl + (((osage_chain_slot *) (p0 + ((i * 0x1c0) + (j * 28))))->root_slot * 0x10));
            chain_calc((osage_chain_slot *) (p0 + ((i * 0x1c0) + (j * 28))));
            if (bone_chains_enabled != 0)
            {
              bone_chain_to_rob_bones((osage_chain_slot *) (p0 + ((i * 0x1c0) + (j * 28))), rob);
            }
          }
        }
      }

      (&rob_chain_usec.rob0)[i] += (float) (((int) stopwatch_usec()) - t0);
    }

    mtx_pop(1);
  }
}

void bone_chain_to_rob_bones(osage_chain_slot *chain, rob_rec *rob)
{
  char *nodes;
  typedef struct
  {
    int w[16];
  } blk;
  struct ent
  {
    int key;
    int idx;
  } *e;
  uint side;
  int op;
  int *p;
  int m;
  chain_joint_desc *joint;
  int i;
  m = (int) rob->mtx;
  side = rob->side;
  p = chain->cmds;
  for (;; p = p + 2)
  {
    op = p[0];
    joint = (chain_joint_desc *) p[1];
    if ((op == 3) || (op == 0))
    {
      for (i = 0; i < 10; i++)
      {
        e = &((struct ent *) (&bone_chain_bone_map[0][0]))[i];
        nodes = (char *) (&bone_chain_nodes[0][0]);
        if (joint->slot == e->key)
        {
          *((blk *) ((e->idx * 0x40) + m)) = *((blk *) ((((joint->slot * 0x68) + (side * 0x1a00)) + nodes) + 0x20));
        }
        if (((struct ent *) (&bone_chain_bone_map[0][0]))[i].idx == 1)
        {
          mtx_set_current_wordwise((float *)(m + 0x40));
          mtx_translate(bone_chain_mtx2_x, 0.0, 0.0);
          mtx_store_wordwise((float *)(m + 0x80));
          mtx_translate_by_vec3(rob->rig->bones[8].pos);
          mtx_store_wordwise((float *)(m + 0x100));
          mtx_set_current_wordwise((float *)(m + 0x80));
          mtx_translate_by_vec3(rob->rig->bones[16].pos);
          mtx_store_wordwise((float *)(m + 0x240));
        }
      }

    }
    if (op == 0)
    {
      break;
    }
  }

  return;
}

void bone_chains_draw(rob_rec *rob)
{
  byte side;
  int i;
  int t;
  side = rob->side;
  i = 0;
  t = (int)&bone_chain_slots[0][0][0];
  for (; i < 0x10; i++) {
    if (*(int *)((i * 0x1c + (uint)side * 0x1c0) + t) != 0) {
      chain_models_draw((osage_chain_slot *)((i * 0x1c + (uint)side * 0x1c0) + t));
    }
  }
}

void osage_setup_alt_tables(rob_rec *rob)
{
  uint v;

  v = rob->body_model;
  if (obj_node_get(v) == 0) return;
  switch (rob->chara) {
  case 2:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_2_0, osage_alt_coli_2_0);
    else osage_chains_add_params(rob, v, osage_alt_params_2_1, osage_alt_coli_2_1);
    break;
  case 5:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_5_0, osage_alt_coli_5_0);
    else osage_chains_add_params(rob, v, osage_alt_params_5_1, osage_alt_coli_5_1);
    break;
  case 11:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_11_0, osage_alt_coli_11_0);
    else osage_chains_add_params(rob, v, osage_alt_params_11_1, osage_alt_coli_11_1);
    break;
  case 9:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_9_0, osage_alt_coli_9_0);
    else osage_chains_add_params(rob, v, osage_alt_params_9_1, osage_alt_coli_9_1);
    break;
  case 1:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_1_0, osage_alt_coli_1_0);
    else osage_chains_add_params(rob, v, osage_alt_params_1_1, osage_alt_coli_1_1);
    break;
  case 10:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_10_0, osage_alt_coli_10_0);
    else osage_chains_add_params(rob, v, osage_alt_params_10_1, osage_alt_coli_10_1);
    break;
  case 3:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_3_0, osage_alt_coli_3_0);
    else osage_chains_add_params(rob, v, osage_alt_params_3_1, osage_alt_coli_3_1);
    break;
  case 12:
    if (rob->costume == 0) osage_chains_add_params(rob, v, osage_alt_params_12_0, osage_alt_coli_12_0);
    else osage_chains_add_params(rob, v, osage_alt_params_12_1, osage_alt_coli_12_1);
    break;
  case 0:
  case 0xd:
  default:
    return;
  }
}
