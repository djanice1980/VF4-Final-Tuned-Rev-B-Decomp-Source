/*
 * sdk/naomi2_library/elan_light_cmds.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

static inline void dl_pref(void *a)
{
  __asm__ __volatile__ ("pref @%0" : : "r" (a));
}

static inline void dl_end(uint *p)
{
  p[0] = 0x08000000; p[2] = 0x08000000; p[4] = 0x08000000; p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  dl_pref(p);
}

void elan_light_cmd_build(elan_light_rec *light, uint *cmd)
{
  elan_light_params_rec args;
  uint color;

  color = (uchar)(int)(light->field_0x4 * 127.0);
  color = color << 8 | (uchar)(int)(light->color[0] * 127.0);
  color = color << 8 | (uchar)(int)(light->color[1] * 127.0);
  color = color << 8 | (uchar)(int)(light->color[2] * 127.0);
  if (light->type == 2) {
    elan_light_pack_directional(color,(int)(light->dir[0] * 65535.0),
                 (int)(light->dir[1] * 65535.0),
                 (int)(light->dir[2] * 65535.0),light->diffuse,
                 light->specular,light->routing,cmd);
    cmd[7] |= light->spec_fade << 20 & 0x100000;
  }
  else {
    args.color = color;
    args.dir_fixed[0] = (int)(light->dir[0] * 65535.0);
    args.dir_fixed[1] = (int)(light->dir[1] * 65535.0);
    args.dir_fixed[2] = (int)(light->dir[2] * 65535.0);
    args.pos[0] = light->pos[0];
    args.pos[1] = light->pos[1];
    args.pos[2] = light->pos[2];
    args.mode[0] = light->diffuse;
    args.mode[1] = light->specular;
    args.mode[2] = light->routing;
    args.field_0x3c = light->spec_fade;
    args.on = light->field_0x30;
    args.near_dist = light->dist_min;
    args.far_dist = light->dist_max;
    if (light->type == 3) {
      args.cone_angle[0] = 0.0;
      args.cone_angle[1] = 0.0;
      elan_light_pack_point(&args,cmd);
    }
    else {
      args.cone_angle[0] = (float)(light->angle_min & 0xffff) * 6.2831855 * 1.5258789e-05;
      args.cone_angle[1] = (float)(light->angle_max & 0xffff) * 6.2831855 * 1.5258789e-05;
      elan_light_pack_spot(&args,cmd);
    }
  }
}

void elan_light_queue(elan_light_rec *light)
{
  uint cmd[16];
  list_block_ref block;
  int i;
  int list_bits;
  uint v;
  uint hi;
  uint *p;
  uint *src;
  uint *dst;
  volatile uint *qacr1;
  elan_light_cmd_build(light, cmd);
  list_bits = elan_list_enable_mask;
  for (i = 0; i < 5; i += 2)
  {
    if ((list_bits & 1) != 0)
    {
      v = (uint) elan_cmd_write_ptr;
      qacr1 = (volatile uint *) 0xff00003c;
      *((volatile uint *) 0xff000038) = v >> 0x18;
      *qacr1 = v >> 0x18;
      p = (uint *) ((v & 0x3ffffff) | 0xe0000000);
      hi = v & 0xfc000000;
      block.addr = (uint) p;
      block.length = 0;
      p = p + 8;
      src = cmd;
      dst = p;
      *(--dst) = *(src++);
      *(--dst) = *(src++);
      *(--dst) = *(src++);
      *(--dst) = *(src++);
      *(--dst) = *(src++);
      *(--dst) = *(src++);
      *(--dst) = (light->id & 0xf) | (*(src++));
      *(--dst) = *src;
      list_end_block_count = (list_end_block_count + 1) & 3;
      asm __volatile__("pref @%0" : : "r"(dst));
      p[0] = 0x08000000;
      p[2] = 0x08000000;
      p[4] = 0x08000000;
      p[6] = 0x08000000;
      list_end_block_count = (list_end_block_count + 1) & 3;
      asm __volatile__("pref @%0" : : "r"(p));
      p = p + 8;
      block.length = ((int) p) - block.addr;
      block.addr = (block.addr & 0x3ffffff) | hi;
      elan_cmd_write_ptr = (uint *)((((uint) p) & 0x3ffffff) | hi);
      elan_list_block_queue(i,&block);
    }
    list_bits = list_bits >> 1;
  }

}

void elan_lights_queue(int count, elan_light_rec *lights)
{
  uint cmd[16][16];
  list_block_ref block;
  int first;
  int i;
  int k;
  int list_bits;
  uint v;
  uint hi;
  uint *p;
  uint *src;
  uint *dst;
  volatile uint *qacr1;

  first = 0; /* ORIGINAL SHAPE (assumed; revisit if a matched sibling shows it): both light loops start from a start index that is always 0; set outside the queue loop, its zero is not the one dl.length is cleared with (a plain i = 0 ties them: 2 lines) */
  for (i = first; i < count; i++) {
    elan_light_cmd_build(&lights[i],cmd[i]);
  }
  list_bits = elan_list_enable_mask;
  for (k = 0; k < 5; k += 2)
  {
    if ((list_bits & 1) != 0)
    {
      i = first;
      v = (uint) elan_cmd_write_ptr;
      qacr1 = (volatile uint *) 0xff00003c;
      *((volatile uint *) 0xff000038) = v >> 0x18;
      *qacr1 = v >> 0x18;
      p = (uint *) ((v & 0x3ffffff) | 0xe0000000);
      hi = v & 0xfc000000;
      block.addr = (uint) p;
      block.length = 0;
      while (i < count) {
        p = p + 8;
        src = cmd[i];
        dst = p;
        *(--dst) = *(src++);
        *(--dst) = *(src++);
        *(--dst) = *(src++);
        *(--dst) = *(src++);
        *(--dst) = *(src++);
        *(--dst) = *(src++);
        *(--dst) = (((elan_light_rec *)lights)[i].id & 0xf) | (*(src++));
        *(--dst) = *src;
        list_end_block_count = (list_end_block_count + 1) & 3;
        dl_pref(dst);
        i++;
      }
      dl_end(p);
      p = p + 8;
      block.length = ((int) p) - block.addr;
      block.addr = (block.addr & 0x3ffffff) | hi;
      elan_cmd_write_ptr = (uint *)((((uint) p) & 0x3ffffff) | hi);
      elan_list_block_queue(k,&block);
    }
    list_bits = list_bits >> 1;
  }
}

void elan_light_masks_queue(elan_light_masks_rec *state)
{
  uint colors[4];
  list_block_ref block;
  float *f;
  int i;
  int list_bits;
  uint v;
  uint color;
  uint hi;
  uint *p;
  uint *dst;
  uint *q;

  f = state->ambient0;
  for (i = 0; i < 4; i++) {
    q = &colors[i];
    color = (int)(*f++ * 255.0) & 0xff;
    color <<= 8;
    color |= (int)(*f++ * 255.0) & 0xff;
    color <<= 8;
    color |= (int)(*f++ * 255.0) & 0xff;
    color <<= 8;
    color |= (int)(*f++ * 255.0) & 0xff;
    *q = color;
  }
  list_bits = elan_list_enable_mask;
  for (i = 0; i < 5; i += 2)
  {
    if ((list_bits & 1) != 0)
    {
      uint *qacr = (uint *) 0xff000038;
      v = (uint) elan_cmd_write_ptr;
      *qacr = v >> 0x18;
      *((uint *) 0xff00003c) = v >> 0x18;
      p = (uint *) ((v & 0x3ffffff) | 0xe0000000);
      hi = v & 0xfc000000;
      block.addr = (uint) p;
      block.length = 0;
      p = p + 8;
      dst = p;
      *(--dst) = colors[3];
      *(--dst) = colors[2];
      *(--dst) = (ushort)state->masks[1] | ((ushort)state->masks[3] << 16) & 0xffff0000;
      *(--dst) = colors[1];
      *(--dst) = colors[0];
      *(--dst) = (ushort)state->masks[0] | ((ushort)state->masks[2] << 16) & 0xffff0000;
      *(--dst) = (state->field_0x4 << 12) & 0xf000 | state->flags | (state->field_0x5 & 0xf) << 16 | 0x10;
      *(--dst) = 0x08000400;
      list_end_block_count = (list_end_block_count + 1) & 3;
      dl_pref(dst);
      dl_end(p);
      p = p + 8;
      block.length = ((int) p) - block.addr;
      block.addr = (block.addr & 0x3ffffff) | hi;
      elan_cmd_write_ptr = (uint *)((((uint) p) & 0x3ffffff) | hi);
      elan_list_block_queue(i,&block);
    }
    list_bits = list_bits >> 1;
  }
}

int model_cmd_iter_init(compiled_model_rec *model, model_cmd_iter_rec *iter)
{
  model_part_span_rec *part;
  int bad;
  bad = (model == (compiled_model_rec *)0x0) | (iter == (model_cmd_iter_rec *)0x0);
  if (bad) return 1;
  if (model->format != 0x100) return 1;
  iter->model = model;
  iter->list = 0;
  iter->vertices_left = 0;
  iter->vertex_size = 0;
  part = model->parts;
  if (part->offset == 0) {
    iter->end = 0;
    iter->cur = 0;
  }
  else {
    iter->cur = part->offset + (int)part;
    iter->end = part->offset + (int)part + part->size;
  }
  return 0;
}

int model_cmd_iter_next(model_cmd_iter_rec *iter, uint *kind, uint *format)
{
  int cur;
  model_part_span_rec *part;
  uint v;
  uint w;

  cur = iter->cur;
  if (iter->vertices_left > 0) {
    *kind = 3;
    iter->vertices_left = iter->vertices_left - 1;
    iter->cur = (int)((int *)iter->cur + iter->vertex_size / 4);
    return cur;
  }
  if ((uint)iter->cur >= (uint)iter->end) {
    if (iter->list + 2 > 4) {
      v = 0x80000000;
    }
    else {
      iter->list = iter->list + 2;
      part = &iter->model->parts[model_list_slot_table[iter->list]];
      if (part->offset == 0) {
        iter->end = 0;
        iter->cur = 0;
      }
      else {
        iter->cur = part->offset + (int)part;
        iter->end = part->offset + (int)part + part->size;
      }
      v = 0x40000000;
    }
    *kind = v;
    return 0;
  }
  w = (*(uint *)iter->cur >> 8) & 0xf;
  switch (w) {
  case 7:
    *kind = 2;
    iter->cur = iter->cur + 0x20;
    *format = *(uint *)(cur + 0x18);
    iter->vertex_size = vertex_bytes_from_format(*(uint *)(cur + 0x18));
    iter->vertices_left = *(int *)(cur + 0x1c);
    break;
  case 5:
    *kind = 1;
    iter->cur = iter->cur + 0x40;
    break;
  case 0:
    *kind = 0;
    iter->cur = iter->cur + 8;
    break;
  }
  return cur;
}
