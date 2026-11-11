/*
 * vf4lib/libeffect/src/effect_sky.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float light_anim_speed; /* 0x0c2a2d1c */
extern int break_link_start_mask; /* 0x0c2aa694 */
extern int break_link_done_mask; /* 0x0c2aa698 */
extern uint sky_state; /* 0x0c458c68 */
extern sky_flash_rec sky_flash; /* 0x0c458c6c */
extern sky_flash_ambient_rgb sky_flash_ambient; /* 0x0c458c88 */

void sky_objs_init(void)
{
  sky_obj_rec *obj;
  sky_obj_def_rec *def;
  int i;
  
  obj = sky_objs;
  def = sky_obj_defs;
  for (i = 0; i < 10; i++, obj++, def++) {
    obj->flags = 1;
    if (i == 1 || (lcg_next15() & 1) != 0) {
      obj->flags |= 8;
    }
    else {
      obj->flags |= 0x10;
    }
    obj->layers = def->layers;
    obj->sounds = def->sounds;
    obj->light_pos = def->light_pos;
    obj->model = def->model;
    obj->model_done = def->model_done;
    obj->frame = 0;
    obj->frames = def->frames;
    obj->time = obj->time_prev = 0.0;
    obj->field_0x1c = def->field_0xc;
    obj->field_0x24 = def->field_0x10;
    obj->field_0x20 = 0.0;
    obj->mask = def->mask;
  }
  sky_state = 3;
  break_link_start_mask = 0;
  break_link_done_mask = 0;
}

void sky_objs_exec(void)
{
  typedef struct { unsigned int b0 : 1; unsigned int b1 : 1; unsigned int b2 : 1; unsigned int : 29; } sky_state_bits;
  uint *sound_state;
  int i;
  short *snd;
  short **pp, **t;
  sky_obj_rec *e;

  e = sky_objs;
  sound_state = sound_state_get();
  if ((sky_state & 1) == 0) return;
  if (mode_get_main() == 3 && mode_get_sub() == 8 && (sky_state & 4) != 0) {
    ((sky_state_bits *)&sky_state)->b1 = 1;
    ((sky_state_bits *)&sky_state)->b2 = 0;
  }
  for (i = 0; i < 10; i++, e++) {
    if ((e->flags & 1) && (e->flags & 2)) {
      e->frame++; if (e->frame == 0x1e) {
        break_link_start_mask = break_link_start_mask | e->mask;
      }
      if (e->frame == 0x1f) {
        e->flags &= 0xf7;
      }
      t = e->sounds;
      if (t != 0 && (*sound_state & 0x2000) != 0) {
        pp = t;
        while ((snd = *pp++) != 0) {
          if (*snd == e->frame) {
            sound_play_id(snd[1],0);
          }
        }
      }
      if ((e->flags & 0x20) && *++e->seq_a == -1) {
        e->flags &= 0xdf;
      }
      if ((e->flags & 0x40) && *++e->seq_b == -1) {
        e->flags &= 0xbf;
      }
      if ((break_link_done_mask & e->mask) != 0) {
        e->flags |= 0x10;
      }
      if (e->frame >= e->frames && (break_link_done_mask & e->mask) != 0) {
        e->flags = e->flags & 0xfd | 0x14;
        ((sky_state_bits *)&sky_state)->b2 = 1;
      }
    }
  }
}

void sky_rob_landed(void)
{
  return;
}

void sky_objs_draw(void)
{
  sky_obj_rec *obj;
  int i;
  
  if ((sky_state & 1) != 0) {
    obj = sky_objs;
    i = 0;
    do {
      if (!((obj->flags & 8) == 0)) {
        obj_draw(obj->model);
      }
      else {
        if ((obj->flags & 0x10) != 0) {
          obj_draw(obj->model_done);
        }
      }
      i = i + 1;
      obj = obj + 1;
    } while (i < 10);
  }
  return;
}

void sky_objs_draw_layers(void)
{
  sky_obj_rec *p;
  sky_layer_rec *e;
  sky_layer_rec **pp;
  int i;
  int id;
  int t;
  int v;

  if ((sky_state & 1) != 0) {
    p = sky_objs;
    for (i = 0; i < 10; i++, p++) {
      if ((p->flags & 2) != 0) break;
    }
    if (i == 10) return;
    mtx_push(0);
    pp = p->layers;
    while ((e = *pp++) != 0) {
      id = e->model;
      if ((id == 0x1b0036) | (id == 0x1b00b6)) continue;
      if (id == 0x1b0140) continue;
      if (id == 0x1b04a1) continue;
      if (id == 0x1b04e6) continue;
      if (id == 0x1b054b) continue;
      t = (short)(p->frame - e->start);
      if (t < 0) continue;
      if (!(e->frames > t)) continue;
      mtx_peek_pushed(0);
      mtx_translate_by_vec3(e->pos);
      if (e->flags & 2) {
        mtx_rot_zyx_s16(e->rot[0],e->rot[1],e->rot[2]);
      }
      else if (e->flags & 4) {
        mtx_reset_3x3();
      }
      if (e->flags & 1) {
        mtx_scale_xyz(e->scale[0],e->scale[1],e->scale[2]);
      }
      v = t + e->model;
      obj_draw(v);
      if (v == 0x1b05b9) {
        if ((p->flags & 0x20) == 0) {
          p->flags = p->flags | 0x20;
          p->seq_a = sky_model_seq;
          mtx_store_wordwise(sky_seq_mtx_a);
        }
        else if ((p->flags & 0x40) == 0) {
          p->flags = p->flags | 0x40;
          p->seq_b = sky_model_seq + 1;
          mtx_store_wordwise(sky_seq_mtx_b);
        }
      }
    }
    if ((p->flags & 0x20) != 0) {
      mtx_set_current_wordwise(sky_seq_mtx_a);
      obj_draw(*p->seq_a);
    }
    if ((p->flags & 0x40) != 0) {
      mtx_set_current_wordwise(sky_seq_mtx_b);
      obj_draw(*p->seq_b);
    }
    mtx_pop(1);
  }
}

void sky_flash_init(void)
{
  sky_flash_rec *p = &sky_flash;
  p->flags = 3;
  p->pieces = 0;
  p->frame = 0;
  p->time = p->time_prev = 0.0;
  p->model = 0x1b05c0;
  p->delay = 0;
  p->frames = (int)lcg_next15() % 60 + 60;
  p->variant = (uint)&sky_flash_variants[4];
}

void sky_flash_exec(void)
{
  sky_flash_rec *p = &sky_flash;
  uint *sound_state = sound_state_get();
  sky_flash_variant_rec *e;
  if ((p->flags & 1) != 0) {
    if ((p->flags & 2) == 0) {
      e = &sky_flash_variants[(char)((int)lcg_next15() % 8)];
      p->flags |= 2;
      p->pieces = 0;
      p->frame = 0;
      p->frames = (int)lcg_next15() % 60 + 60;
      p->time = p->time_prev = 0.0;
      p->variant = (uint)e;
    }
    if ((p->flags & 2) != 0) {
      e = (sky_flash_variant_rec *)p->variant;
      if (p->frame == 1) {
        p->model = e->model;
      }
      else if (p->frame == 4) {
        p->model = 0x1b05c0;
      }
      if (e->light == 0) {
        if (p->frame == 2) {
          p->flags |= 8;
          p->delay = sky_flash_sound_delay;
        }
        else if (p->frame == 5) {
          p->flags &= 0xf7;
        }
      }
      if ((p->flags & 0x10) != 0) {
        p->list++;
        if (*p->list == -1) {
          p->flags &= 0xef;
          p->pieces = 0;
        }
      }
      if (p->delay > 0) {
        if (--p->delay == 0) {
          if ((*sound_state & 0x2000) != 0) sound_play_id(0x38d,0);
        }
      }
      p->frame++;
      if ((short)p->frame >= p->frames || p->time_prev > p->time) {
        p->flags &= 0xfd;
        p->model = 0x1b05c0;
      }
    }
  }
}

void sky_flash_draw(void)
{
  volatile int flag = 0;
  sky_flash_rec *g = &sky_flash;
  sky_flash_variant_rec *variant = (sky_flash_variant_rec *)g->variant;
  int i;
  sky_flash_piece_rec *p;

  if ((*(char *)&g->flags & 1) != 0) {
    mtx_push(0);
    sky_flash_ambient.r = light_set_ambient[0].color[0];
    sky_flash_ambient.g = light_set_ambient[0].color[1];
    sky_flash_ambient.b = light_set_ambient[0].color[2];
    light_set_send(sky_flash_ambient_color,0);
    obj_draw(sky_flash.model);
    light_set_send(sky_flash_ambient_color,3);
    if ((*(char *)&g->flags & 0x10) != 0) {
      for (i = 0; i < *(char *)&g->pieces; i++) {
        mtx_set_current_wordwise(sky_flash_piece_mtx[i]);
        obj_draw(*g->list);
      }
    }
    if ((*(uint *)g & 0xffff0010) == 0x40000) {
      flag = 1;
      *(char *)&g->flags |= 0x10;
      g->list = sky_model_seq;
    }
    if ((*(char *)&g->flags & 8) != 0) {
      p = variant->pieces;
      for (i = 0; i < variant->count; i++, p++) {
        mtx_peek_pushed(0);
        mtx_translate_by_vec3(p->pos);
        mtx_rot_zyx_s16(p->rot[0], p->rot[1], p->rot[2]);
        mtx_scale_xyz(p->scale[0], p->scale[1], p->scale[2]);
        obj_draw(p->model);
        if (flag) {
          mtx_store_wordwise(sky_flash_piece_mtx[i]);
          *(char *)&g->pieces += 1;
        }
      }
    }
    mtx_pop(1);
  }
}

void sky_flash_lights_set(void)
{
  int on[1];
  sky_flash_variant_rec *variant;
  float *pos;
  int light;
  int j;
  sky_obj_rec *obj;
  int k;
  int i;

  on[0] = 1;
  variant = (sky_flash_variant_rec *)sky_flash.variant;
  obj = sky_objs;
  k = 0;
  light = 2;
  if ((sky_flash.flags & 2) != 0) {
    j = 0;
    sky_flash.time_prev = sky_flash.time;
    sky_flash.time = light_anim_play(sky_flash.time,5);
    for (; j < variant->count; light++, j++) {
      light_src_rec *dst = &light_sets[0][light];
      *dst = light_anim_lights[variant->light];
      dst->pos[0] = variant->light_pos[j * 3];
      dst->pos[1] = variant->light_pos[j * 3 + 1];
      dst->pos[2] = variant->light_pos[j * 3 + 2];
    }
  }
  for (i = 0; i < 10; i++, obj++) {
    if ((obj->flags & 2) != 0) break;
  }
  if ((i == 10) || (obj->time_prev > obj->time)) {
    on[0] = 0;
  }
  if (on[0]) {
    j = 0;
    obj->time_prev = obj->time;
    obj->time = light_anim_play(obj->time,7);
    do {
      light_src_rec *dst = &light_sets[0][light];
      *dst = light_anim_lights[j];
      dst->pos[0] = obj->light_pos[j * 3];
      pos = obj->light_pos + j * 3;
      dst->pos[1] = pos[1];
      dst->pos[2] = pos[2];
      light++;
      j++;
    } while (j < 4);
  }
  for (; k < 0x10; k++) {
    light_sets[3][k].type = 0;
  }
  for (; light < 0x10; light++) {
    light_sets[0][light].type = 0;
  }
}

void sky_init(void)
{
  light_anim_speed = 1.0;
  sky_objs_init();
  return;
}

void sky_exec(void)
{
  return;
}

void sky_draw(void)
{
  sky_objs_draw();
  return;
}
