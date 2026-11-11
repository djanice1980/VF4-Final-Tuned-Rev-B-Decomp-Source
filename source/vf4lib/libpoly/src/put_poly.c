/*
 * vf4lib/libpoly/src/put_poly.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define NOPROTO_line3d_quads_opaque
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float normal_line_length; /* 0x0c322720 */
extern int envelope_on; /* 0x0c322724 */
extern int obj_volumes_on; /* 0x0c322728 */
extern int envelope_skin_off; /* 0x0c32272c */
extern int envelope_skin_sq_on; /* 0x0c322730 */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern int obj_set_bank[241]; /* 0x0c3b6fc4 */
extern float envelope_slot_mtx[162][16]; /* 0x0c3b7790 */


compiled_model_rec * obj_model_get(uint obj_id)
{
  int i;
  objbank *e;
  int k;

  i = obj_set_bank[(short)(obj_id >> 0x10)];
  if (i >= 0) {
    e = &objbanks[i];
    if (e->state == 1) {
      k = obj_id & 0xffff;
      if (k < e->count) {
        return e->nodes[k]->model;
      }
    }
  }
  return 0;
}

obj_node * obj_node_get(uint obj_id)
{
  int bank;
  objbank *e;

  bank = obj_set_bank[(short)(obj_id >> 0x10)];
  if (bank >= 0) {
    e = &objbanks[bank];
    if (e->state == 1 && (int)(obj_id & 0xffff) < e->count) {
      return e->nodes[obj_id & 0xffff];
    }
  }
  return 0;
}

void obj_draw(uint obj_id)
{
  compiled_model_rec *model;

  model = obj_model_get(obj_id);
  if (model != 0) {
    model_draw(model);
  }
  return;
}

void obj_draw_culled(uint obj_id)
{
  compiled_model_rec *model;
  int visible;

  model = obj_model_get(obj_id);
  if ((model != 0) && (visible = obj_sphere_visible(model), visible != 0)) {
    model_draw(model);
  }
  return;
}

void obj_draw_translucent(uint obj_id)
{
  compiled_model_rec *model;

  model = obj_model_get(obj_id);
  if (model != 0) {
    model_draw_all_as_translucent(model);
  }
  return;
}

void obj_volume_draw(uint obj_id)
{
  compiled_model_rec *model;

  model = obj_model_get(obj_id);
  if (model != 0) {
    if (!(obj_volumes_on == 0)) model_volumes_draw(model); else model_draw(model);
  }
  return;
}

void obj_volume_draw_trans(uint obj_id)
{
  compiled_model_rec *model;

  model = obj_model_get(obj_id);
  if (model != 0) {
    if (!(obj_volumes_on == 0)) model_volumes_draw_trans_mod(model); else model_draw(model);
  }
  return;
}

void obj_draw_buffered(uint obj_id)
{
  obj_node *node;
  compiled_model_rec *model;

  node = obj_node_get(obj_id);
  if (node != 0) {
    if (!((node->type < 1) || (frame_parity == 0))) {
      model = node->model2;
    }
    else {
      model = node->model;
    }
    model_draw(model);
  }
  return;
}

void obj_volume_draw_buffered(uint obj_id)
{
  obj_node *node;
  compiled_model_rec *model;

  node = obj_node_get(obj_id);
  if (node != 0) {
    if (!((node->type < 1) || (frame_parity == 0))) {
      model = node->model2;
    }
    else {
      model = node->model;
    }
    if (!(obj_volumes_on == 0)) model_volumes_draw(model); else model_draw(model);
  }
  return;
}

void node_draw(obj_node *node)
{
  if (node != 0) {
    model_draw(node->model);
  }
  return;
}

void node_draw_culled(obj_node *node)
{
  int visible;

  if ((node != 0) && (visible = obj_sphere_visible(node->model), visible != 0)) {
    model_draw(node->model);
  }
  return;
}

void node_draw_translucent(obj_node *node)
{
  if (node != 0) {
    model_draw_all_as_translucent(node->model);
  }
  return;
}

void node_volume_draw(obj_node *node)
{
  void (*draw)(compiled_model_rec *);
  compiled_model_rec *model;

  if (node != 0) {
    if (!(obj_volumes_on == 0)) {
      model = node->model;
      draw = model_volumes_draw;
    }
    else {
      model = node->model;
      draw = model_draw;
    }
    (*draw)(model);
  }
  return;
}

void node_draw_buffered(obj_node *node)
{
  compiled_model_rec *model;

  if (node != 0) {
    if (!((node->type < 1) || (frame_parity == 0))) {
      model = node->model2;
    }
    else {
      model = node->model;
    }
    model_draw(model);
  }
  return;
}

void objbank_draw_all(int bank)
{
  obj_node *node;
  obj_node **p;
  compiled_model_rec *model;

  p = objbanks[bank].nodes;
  while ((node = *p++) != 0) {
    model = node->model;
    if (model->format == 0x100) {
      model_draw(model);
    }
    else {
      obj_put_typed((int *)model);
    }
  }
  return;
}

void obj_polys_float_1c_set(int *obj, float value)
{
  int size;

  obj += 6;
  while (*obj != 0) {
    *(float *)(obj + 7) = value;
    obj += 0x13;
    size = *obj++;
    obj = (int *)((char *)obj + size / 4 * 4);
  }
  return;
}

void obj_material_color_set(uint obj_id, uint color)
{
  compiled_model_rec *model;
  int cmd;
  int kind_format[2];
  model_cmd_iter_rec *iter;

  model = obj_model_get(obj_id);
  if (model != 0) {
    obj_material_cmd_count = 0;
    iter = &obj_material_cmd_iter;
    model_cmd_iter_init(model,iter);
    if (iter->cur == 0) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libpoly/src/put_poly.c",0x135,"assertion `%s\' failed.\n",
               "0");
    }
    while( true ) {
      obj_material_cmd_count = obj_material_cmd_count + 1;
      cmd = model_cmd_iter_next(&obj_material_cmd_iter,(uint *)&kind_format[0],(uint *)&kind_format[1]);
      if ((kind_format[0] == -0x80000000) | (kind_format[0] == 0x40000000)) break;
      if (kind_format[0] == 1) {
        *(uint *)(cmd + 0xc) = color;
        *(uint *)(cmd + 0x14) = color;
      }
    }
  }
  return;
}

void obj_morph_set(uint obj_id, int key, float frac)
{
  obj_node *node;
  uint normal;
  int file;
  int *block;
  int *list;
  uint *dst;
  float *from;
  float *to;
  compiled_model_rec *model;
  float t;
  float v[6];
  int k;
  int j;
  int n;
  int cnt;
  int o;
  uint f;

  node = obj_node_get(obj_id);
  if (node == 0) return;
  file = (int)node->file;
  model = node->model;
  if (model == 0) return;
  if (node->type == 6) block = (int *)&node->unknown_40;
  else {
    if (node->type != 0xe) return;
    block = (int *)0x88;
    block = (int *)((int)block + (int)node);
  }
  list = (int *)(file + *block);
  if (key >= block[1]) return;
  from = (float *)(file + block[key + 2]);
  to = (float *)(file + block[key + 3]);
  while (1) {
    cnt = *list++;
    if (cnt == 0) break;
    for (k = 0; k < cnt; k++) {
      n = *list++;
      t = 1.0 - frac;
      v[0] = t * from[0] + to[0] * frac;
      v[1] = t * from[1] + to[1] * frac;
      v[2] = t * from[2] + to[2] * frac;
      v[3] = t * from[3] + to[3] * frac;
      v[4] = t * from[4] + to[4] * frac;
      v[5] = t * from[5] + to[5] * frac;
      vec3_normalize(&v[3]);
      normal = normal_pack_bytes(&v[3]);
      for (j = 0; j < n; j++) {
        o = *list++;
        f = *list++;
        dst = (uint *)((char *)model + o);
        *dst = normal | f;
        dst[1] = ((uint *)v)[0];
        dst[2] = ((uint *)v)[1];
        dst[3] = ((uint *)v)[2];
      }
    }
    from = from + 6;
    to = to + 6;
  }
}

void envelope_update(obj_node **bones, float *mtx)
{
  obj_node *node;
  compiled_model_rec *v;
  int groups;
  int bind;
  int i;
  int t;

  node = *bones;
  if (envelope_on == 0 || node == 0) return;
  t = (int)node->model;
  if (t == 0) return;
  if (node->type > 0) {
    if (frame_parity == 0) v = (compiled_model_rec *)t;
    else v = node->model2;
  }
  else {
    v = node->model;
  }
  if (v == 0) return;
  if (((node->type == 0x12) | (node->type == 0x10)) == 0) return;
  groups = (int)node->list_80 + (int)node->file;
  bind = node->bones + (int)node->file;
  mtx_mul_into(mtx + 0x10,(float *)(bind + 0x40),envelope_slot_mtx[1]);
  bones += 2;
  for (i = 2; i < 0xa2; i++, bones++) {
    if (*bones != 0) {
      mtx_mul_into(mtx + i * 0x10,(float *)(bind + i * 0x40),envelope_slot_mtx[i]);
    }
  }
  if (envelope_skin_off == 0) {
    if (node->type == 0x12) {
      if (envelope_skin_sq_on == 1) {
        envelope_skin_sq((float *)groups,(int)&envelope_slot_mtx,v);
      }
      else {
        envelope_skin((float *)groups,(int)&envelope_slot_mtx,v);
      }
    }
    else if (node->type == 0x10) {
      envelope_skin_nouv((float *)groups,(int)&envelope_slot_mtx,v);
    }
  }
}

float * envelope_skin(float *src, int slot_mtx, compiled_model_rec *dst)
{
  typedef struct { int w[3]; } skin_vec;
  typedef struct { uint flags; skin_vec pos; int a; int b; } skin_ent;
  int out = (int)dst;
  int base = slot_mtx;
  float acc[16];
  float m[16];
  skin_vec pos;
  float q[3];
  int n;
  int i;
  int k;
  int j;
  int cnt;
  float w;
  uint flags;
  int a;
  int b;
  int off;
  uint val;
  int addr;

  n = *(int *)src++;
  while (n != 0) {
    src++;
    if (n > 1) {
      for (i = 0; i < 16; i++) {
        acc[i] = 0.0;
      }
      for (k = 0; k < n; k++) {
        w = *src++;
        src++;
        addr = *(int *)src++;
        src++;
        mtx_set_current_wordwise((float *)(base + ((uint)addr >> 6 << 6)));
        mtx_store_wordwise(m);
        for (i = 0; i < 16; i++) {
          acc[i] = w * m[i] + acc[i];
        }
      }
      mtx_set_current_wordwise(acc);
    }
    else {
      src += 2;
      addr = *(int *)src++;
      src++;
      mtx_set_current_wordwise((float *)(base + (addr & 0xffffffc0)));
    }
    while (1) {
      cnt = *(int *)src++;
      src++;
      if (cnt == 0) break;
      mtx_xform_point(src + 3,(float *)&pos);
      mtx_xform_dir(src,q);
      vec3_normalize(q);
      flags = normal_pack_bytes(q);
      a = ((int *)src)[6];
      b = ((int *)src)[7];
      src += 8;
      for (j = 0; j < cnt; j++) {
        off = *(int *)src++;
        val = *(int *)src++;
        ((uint *)out)[off / 4] = val | flags;
        ((skin_ent *)&((uint *)out)[off / 4])->pos = pos;
        ((skin_ent *)&((uint *)out)[off / 4])->a = a;
        ((skin_ent *)&((uint *)out)[off / 4])->b = b;
      }
    }
    n = *(int *)src++;
  }
  return src;
}

void mtx_lerp(float *a, float *b, float *out, float t)
{
  int i;

  for (i = 0; i < 0x10; i++) {
    out[i] = (1.0 - t) * a[i] + b[i] * t;
  }
  return;
}

void mtx_lerp_normalized(float *a, float *b, float *out, float t)
{
  int i;

  for (i = 0; i < 0x10; i++) {
    out[i] = (1.0 - t) * a[i] + b[i] * t;
  }
  vec3_normalize(out);
  vec3_normalize(out + 4);
  vec3_normalize(out + 8);
  return;
}

void normal_line_draw(float *point, float *dir)
{
  uint v[3];

  v[0] = *(uint *)dir;
  v[1] = ((uint *)dir)[1];
  v[2] = ((uint *)dir)[2];
  vec3_scale((float *)v,dir,normal_line_length);
  vec3_add_to((float *)v,point);
  line3d_quads_opaque(point,v,0xff00);
  return;
}

void poly_modvol_bit_off(void)
{
  hdr_or_volume(0);
  return;
}

void poly_modvol_bit_on(void)
{
  hdr_or_volume(2);
  return;
}

void obj_tex_refs_walk(uint obj_id)
{
  obj_node *node;
  int *p;
  node = obj_node_get(obj_id);
  for (p = (int *)(node->tex_refs_offset + (int)node->file); *p != 0; p += 4) ;
}
