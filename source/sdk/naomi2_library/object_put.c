/*
 * sdk/naomi2_library/object_put.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_texidx_bit_clear
#define NOPROTO_pal_group_unref
#define NOPROTO_obj_poly_texture_hook
#define NOPROTO_tex_slot_words_or_fallback
#define NOPROTO_pal_mark_blocks_used
#define NOPROTO_palette_shadow_write
#define NOPROTO_obj_mtx_push_proj_view
#define NOPROTO_obj_light_vecs_to_model
#define NOPROTO_obj_light_vecs_to_model_view
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float obj_light_scalar; /* 0x0c325888 */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_height; /* 0x0c32a700 */
extern float proj_rect_px_left; /* 0x0c32a704 */
extern float proj_rect_px_top; /* 0x0c32a708 */
extern uint obj_palette_table; /* 0x0c3dc328 */
extern float rect_x_proj_work[16]; /* 0x0c41c958 */
extern float rect_x_proj_matrix[16]; /* 0x0c41c9a0 */
extern float proj_x_factor_px; /* 0x0c41ca68 */
extern float proj_y_factor_px; /* 0x0c41caf0 */
extern obj_light_dir_vec obj_light_dir; /* 0x0c325890 */
extern obj_half_vector_vec obj_half_vector; /* 0x0c3258a0 */
extern obj_view_dir_vec obj_view_dir; /* 0x0c3258b0 */
extern unit_rgb obj_light_color; /* 0x0c3258c0 */
extern obj_send_header_rec obj_send_header; /* 0x0c417d18 */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */
extern mtx_stack_record_set mtx_stack_records; /* 0x0c41c860 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

void obj_tex_bases_clear(void)
{
  texidx_base = 0;
  obj_palette_table = 0;
  return;
}

void busy_delay_5000(void)
{
  volatile int i;

  i = 0;
  while (i++ < 5000) ;
  return;
}

uint obj_textures_load_step(int *obj, int texlist, uint *from, uint **next_out, int mapped)
{
  typedef struct { short block; char kind; char block_count; short uses; short group; } pal_group_ent;
  int loaded;
  uint result;
  uint *cur;
  uint *next;
  int tex_no;
  int texidx;
  int uses;
  tex_slot_rec *slot;
  texidx_map_entry *ent;
  uint fmt;
  int group_no;
  int pal;
  pal_group_ent *grp;
  int block;
  int pal_size;
  int *tex_ent;
  
  loaded = 0;
  result = 0;
  *next_out = 0;
  next = from;
  if (next == 0) {
    if (*obj < 0 || obj[6] == 0) return result;
    next = (uint *)(obj + 6);
  }
  {
  loop:
    cur = next;
    next = cur + 0x14;
    texidx = tex_no = cur[8];
    if (tex_no >= 0) {
      if (mapped == 1) {
        texidx = *(int *)(texlist + tex_no * 0x10 + 0xc);
      }
      texidx = texidx + texidx_base;
      { int map = (int)texidx_map; ent = (texidx_map_entry *)(texidx * 8 + map); }
      if (ent->use_count == 0) {
        ent->slot_no = texidx_bit_take();
      }
      texidx_map[texidx].use_count++;
      texidx = texidx_map[texidx].slot_no;
      slot = &tex_slot_table[texidx];
      uses = slot->in_use;
      if (uses == 0) {
        tex_ent = (int *)(texlist + tex_no * 0x10);
        result |= tex_slot_create_and_load((short *)tex_ent,texidx,tex_ent[2]);
        tex_slot_words_or_fallback(texidx);
        slot->slot_id = texidx;
        loaded = 1;
        uses = slot->in_use;
      }
      slot->in_use = uses + 1;
    }
    if (tex_no < 0) {
      cur[0] |= 1;
      cur[1] |= 0x400000;
    }
    cur[0] = cur[0] & 0xfffcffff | poly_header_bits16 << 0x10 | 0x8c0000;
    if (texidx >= 0) {
      tex_slot_bits_into_poly_hdr(cur,texidx);
    }
    else {
      tex_builtin_bits_into_poly_hdr(cur);
    }
    if (pal_raw_mode == '\0') {
      fmt = cur[3] >> 0x1b & 7;
      if (obj_palette_table != 0) {
        if (fmt - 5 < 2) {
          group_no = cur[0xf];
          if (group_no < graphics_work.pal_group_count) {
            grp = (pal_group_ent *)graphics_work.pal_group_table;
            pal = group_no * 0xc + obj_palette_table;
            grp += group_no;
            if (grp->uses != 0) {
              grp->uses++;
              block = grp->block;
            }
            else {
              grp->kind = *(char *)(pal + 2);
              grp->block_count = (char)((*(int *)(pal + 8) + 0xfU) >> 4);
              grp->uses = 1;
              grp->group = (short)group_no;
              pal_size = *(int *)(pal + 8);
              block = pal_find_free_blocks((int)*(short *)(pal + 2),pal_size);
              if (block >= 0) {
                grp->block = (short)block;
                pal_mark_blocks_used(block,pal_size);
                palette_shadow_write(block << 4,*(uint **)(pal + 4),pal_size);
              }
              else {
                block = 0;
              }
            }
            cur[3] = cur[3] & 0xf81fffff | block << 0x15;
          }
        }
      }
    }
    else if ((cur[3] >> 0x1b & 7) - 5 < 2) {
      { uint v = cur[0xf]; cur[3] = cur[3] & 0xf81fffff | v << 0x15; }
    }
    obj_poly_texture_hook(cur);
    next = (uint *)((int)next + cur[0x13]);
    if (*next == 0) {
      *next_out = 0;
      return result;
    }
  if (!loaded) goto loop;
  *next_out = next;
  }
  return result;
}

void obj_textures_load_from(int *obj, int texlist, uint *from, uint **next_out)
{
  obj_textures_load_step(obj,texlist,from,next_out,0);
  return;
}

uint obj_textures_load(int *obj, int texlist)
{
  uint bits;
  uint result;
  uint * resume [1];
  
  resume[0] = 0;
  result = 0;
  do {
    bits = obj_textures_load_step(obj,texlist,resume[0],resume,0);
    result = result | bits;
  } while (resume[0] != 0);
  return result;
}

void obj_textures_load_from_mapped(int *obj, int texlist, uint *from, uint **next_out)
{
  obj_textures_load_step(obj,texlist,from,next_out,1);
  return;
}

uint obj_textures_load_mapped(int *obj, int texlist)
{
  uint bits;
  uint result;
  uint * resume [1];
  
  resume[0] = 0;
  result = 0;
  do {
    bits = obj_textures_load_step(obj,texlist,resume[0],resume,1);
    result = result | bits;
  } while (resume[0] != 0);
  return result;
}

int obj_texture_bits_refresh(int *obj, int texlist, int mapped)
{
  int texidx;
  uint pal_bits;
  int *rec;
  int *next;
  int ret;

  ret = 0;
  if (*obj < 0 || obj[6] == 0) return ret;
  next = obj + 6;
  while (1) {
    rec = next;
    next = rec + 0x14;
    texidx = rec[8];
    if (texidx >= 0) {
      if (mapped != 0 && texlist != 0) {
        texidx = *(int *)(texidx * 0x10 + texlist + 0xc);
      }
      texidx = texidx + texidx_base;
      if (texidx_map[texidx].use_count == 0) {
        ret = 1;
        texidx = -1;
      }
      else {
        texidx = texidx_map[texidx].slot_no;
      }
    }
    pal_bits = rec[3] & 0x7e00000;
    if (texidx >= 0) {
      tex_slot_bits_into_poly_hdr((uint *)rec,texidx);
    }
    else {
      tex_builtin_bits_into_poly_hdr((uint *)rec);
    }
    rec[3] = rec[3] | pal_bits;
    next = (int *)((int)next + rec[0x13]);
    if (*next == 0) break;
  }
  return ret;
}

void obj_texture_bits_refresh_plain(int *obj)
{
  obj_texture_bits_refresh(obj,0,0);
  return;
}

void obj_texture_bits_refresh_mapped(int *obj, int texlist)
{
  obj_texture_bits_refresh(obj,texlist,1);
  return;
}

uint obj_textures_release_step(int *obj, int texlist, int *from, int **next_out, int mapped)
{
  int freed;
  uint ret;
  int *next;
  int *rec;
  int texidx;
  int slot_no;
  uint fmt;
  freed = 0;
  ret = 0;
  if (from == 0) {
    if (*obj < 0) goto out;
    if (obj[6] == 0) goto out;
    next = obj + 6;
  }
  else {
    next = from;
  }
top:
  rec = next;
  next = (int *)((char *)next + 0x50);
  texidx = rec[8];
  if (texidx >= 0) {
    if ((mapped != 0) & (texlist != 0)) texidx = *(int *)(texidx * 16 + texlist + 12);
    texidx += texidx_base;
    if (texidx_map[texidx].use_count != 0) {
      texidx_map[texidx].use_count--;
      if (texidx_map[texidx].use_count == 0) {
        slot_no = texidx_map[texidx].slot_no;
        if (tex_slot_table[slot_no].status == 0 && surface_memory_return_by_kind(&tex_slot_table[slot_no].surface) != 0) ret = 1;
        tex_slot_table[slot_no].in_use = 0;
        tex_slot_table[slot_no].slot_id = -1;
        texidx_bit_clear(slot_no);
        texidx_map[texidx].slot_no = -1;
        freed = 1;
      }
    }
  }
  if (pal_raw_mode == 0) {
    fmt = (uint)rec[3] >> 0x1b & 7;
    if (obj_palette_table != 0 && fmt - 5 < 2) pal_group_unref(rec[0xf]);
  }
  next = (int *)((char *)next + rec[0x13]);
  if (*next == 0) {
    *next_out = 0;
    return ret;
  }
  if (freed == 0) goto top;
  *next_out = next;
out:
  return ret;
}

void obj_textures_release_from(int *obj, int *from, int **next_out)
{
  obj_textures_release_step(obj,0,from,next_out,0);
  return;
}

void obj_textures_release_from_mapped(int *obj, int texlist, int *from, int **next_out)
{
  obj_textures_release_step(obj,texlist,from,next_out,1);
  return;
}

uint obj_textures_release(int *obj)
{
  uint bits;
  uint result;
  int * resume [1];
  
  resume[0] = 0;
  result = 0;
  do {
    bits = obj_textures_release_step(obj,0,resume[0],resume,0);
    result = result | bits;
  } while (resume[0] != 0);
  return result;
}

uint obj_textures_release_mapped(int *obj, int texlist)
{
  uint bits;
  uint result;
  int * resume [1];
  
  resume[0] = 0;
  result = 0;
  do {
    bits = obj_textures_release_step(obj,texlist,resume[0],resume,1);
    result = result | bits;
  } while (resume[0] != 0);
  return result;
}

void obj_palette_table_put(uint table)
{
  obj_palette_table = table;
  return;
}

void obj_palette_offset_apply(int *obj, int offset)
{
  int *rec;
  int *next;
  uint fmt;
  int pal;

  if (pal_raw_mode != 1) return;
  if (*obj < 0) return;
  if (obj[6] == 0) return;
  next = obj + 6;
  do {
    rec = next;
    next = rec + 0x14;
    fmt = (uint)rec[3] >> 0x1b & 7;
    if (fmt - 5 < 2) {
      pal = rec[0xf] + offset;
      rec[3] = (rec[3] & 0xf81fffff) | (pal << 0x15);
    }
    next = (int *)((char *)next + rec[0x13]);
  } while (*next != 0);
  return;
}

void modvol_shadow_set_up(uint shadow_scale)
{
  pvr_shadow_scale_write(shadow_scale);
  modvol_instruction_put(1);
  modvol_sign_put(1);
  return;
}

void modvol_shadow_scale_put(uint shadow_scale)
{
  pvr_shadow_scale_write(shadow_scale);
  return;
}

void modvol_instruction_put(int mode)
{
  if (mode != 2) {
    mode = 1;
  }
  modvol_instruction = mode << 0x1d;
  return;
}

int modvol_instruction_get(void)
{
  return (int)modvol_instruction >> 0x1d;
}

void modvol_sign_put(int v)
{
    if (v != -1)
        v = 1;
    modvol_sign = v;
}

int modvol_sign_get(void)
{
  return modvol_sign;
}

void obj_modvol_put(uint *obj)
{
  obj_mtx_push_proj();
  obj_modvol_send(obj,0);
  return;
}

void obj_modvol_put_alt(uint *obj)
{
  obj_mtx_push_proj();
  obj_modvol_send(obj,-1);
  return;
}

void obj_modvol_put_world(uint *obj)
{
  obj_mtx_push_proj_view();
  obj_modvol_send(obj,0);
  return;
}

void obj_modvol_put_world_alt(uint *obj)
{
  obj_mtx_push_proj_view();
  obj_modvol_send(obj,-1);
  return;
}

void obj_modvol_send(uint *obj, int cull_mode)
{
  uint *qacr = (uint *)0xff000038;
  uint first_word = obj[6];
  uint present = ~*obj >> 0x1f;
  uint has_records = first_word != 0;
  uint qacr_bits;

  if ((present & has_records) != 0) {
    qacr_bits = list_write_ptrs[1] >> 0x18;
    qacr[0] = qacr_bits;
    qacr[1] = qacr_bits;
    obj_send_header.head.words[0] =
         (poly_header_or_words.control_or | first_word) & 0x30000 | 0x818c0000;
    obj_send_header.head.words[1] = 0;
    obj_send_header.head.words[2] = modvol_instruction;
    modvol_strips_send(obj,&obj_send_header,cull_mode,modvol_sign);
  }
  mtx_pop(1);
}

void obj_poly_texture_hook(uint *rec)
{
  return;
}

char * object_put_build_date_get(void)
{
  return object_put_build_date_str;
}

void object_put_init(void)
{
  int i;
  int j;
  int k;
  float v;
  float f;
  obj_send_header.light_scalar = obj_light_scalar;
  obj_palette_table = 0;
  for (j = 0; j < 0x20; j++) {
    for (i = 0; i < 0x80; i++) {
      v = ((float)i * 0.292893) / 127.0 + 0.707107;
      f = 1.0;
      obj_spec_power_table[j][i] = 0.0;
      v = v * v;
      v = (v + v) - 1.0;
      v = v * v;
      if (j < 2) {
        obj_spec_power_table[j][i] = v;
      }
      else {
        for (k = 0; k < j; k++) {
          f = f * v;
        }
        obj_spec_power_table[j][i] = f;
      }
    }
  }
}

void obj_proj_view_capture(void)
{
  mtx_store(obj_saved_mtx);
  mtx_premul_by_pairs(rect_x_proj_matrix);
  mtx_store(rect_x_proj_work);
  mtx_set_current(obj_saved_mtx);
  return;
}

void obj_put_typed(int *obj)
{
  extern void obj_light_vecs_to_model(void); /* ORIGINAL SHAPE (corroborated 2026-11-10: the front end leaves no other way: a file-scope void declaration is a hard error against the value-returning definition below, and an undeclared call goes through r0): the game calls obj_light_vecs_to_model through r1 (void) here, before its definition, which the later callers see as returning a value */
  if ((obj[1] & 2) == 0) {
    obj_light_vecs_to_model();
  }
  mtx_push(0);
  mtx_premul_by_pairs(rect_x_proj_work);
  obj_typed_send(obj);
  return;
}

void obj_put_typed_world(int *obj)
{
  extern void obj_light_vecs_to_model_view(void); /* ORIGINAL SHAPE (corroborated 2026-11-10: the front end leaves no other way: a file-scope void declaration is a hard error against the value-returning definition below, and an undeclared call goes through r0): the game calls obj_light_vecs_to_model_view through r1 (void) here, before its definition, which the later callers see as returning a value */
  extern void obj_mtx_push_proj_view(void); /* ORIGINAL SHAPE (corroborated 2026-11-10: the front end leaves no other way: a file-scope void declaration is a hard error against the value-returning definition below, and an undeclared call goes through r0): the game calls obj_mtx_push_proj_view through r1 (void) here, before its definition, which the later callers see as returning a value */
  if ((obj[1] & 2) == 0) {
    obj_light_vecs_to_model_view();
  }
  obj_mtx_push_proj_view();
  obj_typed_send(obj);
  return;
}

void obj_put_typed_f(int *obj, float alpha)
{
  extern void obj_light_vecs_to_model(void); /* ORIGINAL SHAPE (corroborated 2026-11-10: the front end leaves no other way: a file-scope void declaration is a hard error against the value-returning definition below, and an undeclared call goes through r0): the game calls obj_light_vecs_to_model through r1 (void) here, before its definition, which the later callers see as returning a value */
  if ((obj[1] & 2) == 0) {
    obj_light_vecs_to_model();
  }
  mtx_push(0);
  mtx_premul_by_pairs(rect_x_proj_work);
  obj_typed_send_f(obj,alpha);
  return;
}

void obj_put_typed_world_f(int *obj, float alpha)
{
  extern void obj_light_vecs_to_model_view(void); /* ORIGINAL SHAPE (corroborated 2026-11-10: the front end leaves no other way: a file-scope void declaration is a hard error against the value-returning definition below, and an undeclared call goes through r0): the game calls obj_light_vecs_to_model_view through r1 (void) here, before its definition, which the later callers see as returning a value */
  extern void obj_mtx_push_proj_view(void); /* ORIGINAL SHAPE (corroborated 2026-11-10: the front end leaves no other way: a file-scope void declaration is a hard error against the value-returning definition below, and an undeclared call goes through r0): the game calls obj_mtx_push_proj_view through r1 (void) here, before its definition, which the later callers see as returning a value */
  if ((obj[1] & 2) == 0) {
    obj_light_vecs_to_model_view();
  }
  obj_mtx_push_proj_view();
  obj_typed_send_f(obj,alpha);
  return;
}

void obj_light_scalar_set(float value)
{
  obj_light_scalar = value;
  obj_send_header.light_scalar = value;
  return;
}

void obj_view_dir_put(float x, float y, float z)
{
  obj_half_vector.x = obj_light_dir.x + x;
  obj_half_vector.y = obj_light_dir.y + y;
  obj_half_vector.z = obj_light_dir.z + z;
  obj_view_dir.x = x;
  obj_view_dir.y = y;
  obj_view_dir.z = z;
  return;
}

void obj_view_dir_put_v(float *dir)
{
  obj_view_dir_put(*dir,dir[1],dir[2]);
  return;
}

void obj_light_dir_put_v(float *dir)
{
  obj_light_dir_put(*dir,dir[1],dir[2]);
  return;
}

void obj_light_dir_put(float x, float y, float z)
{
  obj_light_dir.x = x;
  obj_light_dir.y = y;
  obj_light_dir.z = z;
  obj_light_dir.field_0xc = z + 1.0;
  obj_half_vector.x = x;
  obj_half_vector.y = y;
  obj_half_vector.z = z + 1.0;
  obj_view_dir.x = 0.0;
  obj_view_dir.y = 0.0;
  obj_view_dir.z = 1.0;
  return;
}

void obj_light_color_set(float r, float g, float b)
{
  obj_light_color.r = r;
  obj_light_color.g = g;
  obj_light_color.b = b;
  *(unit_rgb *)obj_light_colour2 = obj_light_color;
  return;
}

void obj_light_colour_put(float red, float green, float blue)
{
  obj_light_color.r = red;
  obj_light_color.g = green;
  obj_light_color.b = blue;
  return;
}

void obj_light_colour2_set(float r, float g, float b)
{
  obj_light_colour2[0] = r;
  obj_light_colour2[1] = g;
  obj_light_colour2[2] = b;
  return;
}

void blend_pair_bits_put_inverted(uint src_factor, uint dst_factor)
{
  blend_pair_bits = ((~src_factor & 7) << 5) + ((~dst_factor & 7) << 2);
}

void obj_vertex_hook_select(int kind)
{
  if (kind == 0) {
    obj_vertex_hook = 0;
  }
  else if (kind == 1) {
    obj_vertex_hook = (uint)argb_strips_send_from_hook;
  }
  else if (kind == 2) {
    obj_vertex_hook = (uint)argb_offset_strips_send_from_hook;
  }
  return;
}

void list_write_ptrs_mark(void)
{
  gfx_counters_saved[0] = list_write_ptrs[0];
  gfx_counters_saved[1] = list_write_ptrs[1];
  gfx_counters_saved[2] = list_write_ptrs[2];
  gfx_counters_saved[3] = list_write_ptrs[3];
  return;
}

void list_write_ptrs_since_mark(int *list0, int *list1, int *list2, int *list3)
{
  *list0 = list_write_ptrs[0] - gfx_counters_saved[0];
  *list1 = list_write_ptrs[1] - gfx_counters_saved[1];
  *list2 = list_write_ptrs[2] - gfx_counters_saved[2];
  *list3 = list_write_ptrs[3] - gfx_counters_saved[3];
  return;
}

bool obj_sphere_visible(compiled_model_rec *model)
{
  float radius;
  float x_limit;
  float y_limit;
  float v[3];
  
  radius = model->sphere[3] * graphics_work.coord_scale;
  mtx_push(0);
  mtx_premul_by_pairs(rect_x_proj_work);
  mtx_xform_point(model->sphere,v);
  mtx_pop(1);
  if (obj_send_header.near_z > radius + v[0]) return 0;
  v[1] = v[1] - proj_rect_px_left * v[0];
  x_limit = radius * proj_x_factor_px;
  if (-x_limit >= v[1]) return 0;
  v[2] = v[2] - proj_rect_px_top * v[0];
  y_limit = radius * proj_y_factor_px;
  if (-y_limit >= v[2]) return 0;
  if (proj_rect_px_width * v[0] + x_limit <= v[1]) return 0;
  return proj_rect_px_height * v[0] + y_limit > v[2];
}

bool obj_sphere_visible_near_far(compiled_model_rec *model)
{
  float radius;
  float v[3];
  float x_limit;
  float y_limit;
  
  radius = model->sphere[3] * graphics_work.coord_scale;
  mtx_push(0);
  mtx_premul_by_pairs(rect_x_proj_work);
  mtx_xform_point(model->sphere,v);
  mtx_pop(1);
  if (radius + v[0] < obj_send_header.near_z) return 0;
  if (v[0] - radius > obj_send_header.far_z) return 0;
  x_limit = radius * proj_x_factor_px;
  if (v[1] <= -x_limit) return 0;
  y_limit = radius * proj_y_factor_px;
  if (v[2] <= -y_limit) return 0;
  if (proj_rect_px_width * v[0] + x_limit <= v[1]) return 0;
  return v[2] < proj_rect_px_height * v[0] + y_limit;
}

int obj_trailer_word_get(int *obj)
{
  int *rec;
  int *next;

  if (*obj < 0 || obj[6] == 0) return 0;
  next = obj + 6;
  do {
    rec = next;
    next = rec + 0x14;
    next = (int *)((int)next + rec[0x13]);
  } while (*next != 0);
  return next[1];
}

int obj_light_vecs_to_model(void)
{
  mtx_transpose();
  mtx_xform_dir(&obj_light_dir.x,obj_send_header.light_dir);
  mtx_xform_dir(&obj_half_vector.x,obj_send_header.head.half_vec);
  vec3_set_length(436.5,obj_send_header.head.half_vec,obj_send_header.head.half_vec);
  mtx_transpose();
  return;
}

int obj_light_vecs_to_model_view(void)
{
  float *d;
  float *s;

  mtx_stack_select(2);
  mtx_transpose();
  d = obj_send_header.head.half_vec;
  mtx_xform_dir((float *)&obj_view_dir, d);
  mtx_transpose();
  mtx_stack_select(1);
  mtx_transpose();
  s = (float *)&obj_light_dir;
  mtx_xform_dir(s, d + 4);
  d[0] = d[0] + s[0];
  d[1] = d[1] + s[1];
  d[2] = d[2] + s[2];
  mtx_xform_dir(d, d);
  vec3_set_length(436.5, d, d);
  mtx_transpose();
}

void obj_typed_send(int *obj)
{
  typed_send_by_slot(obj);
}

void obj_typed_send_f(int *obj, float alpha)
{
  typed_send_by_slot_alpha(obj,alpha);
  return;
}

uint obj_mtx_push_proj(void)
{
  mtx_push(0);
  mtx_premul_by_pairs(rect_x_proj_work);
}

int obj_mtx_push_proj_view(void)
{
  mtx_push(rect_x_proj_work);
  mtx_mul_by(mtx_stack_records.view.top);
  mtx_times_last_pushed();
  return;
}

void obj_put_lit(int *obj)
{
  if ((obj[1] & 0x10) != 0) {
    obj_light_vecs_to_model();
  }
  lights_by_inverse_matrix();
  obj_mtx_push_proj();
  obj_lit_send(obj);
  return;
}

void obj_put_lit_world(int *obj)
{
  if ((obj[1] & 0x10) != 0) {
    obj_light_vecs_to_model_view();
  }
  lights_by_inverse_matrix();
  obj_mtx_push_proj_view();
  obj_lit_send(obj);
  return;
}

void obj_put_lit_f(int *obj, float alpha)
{
  if ((obj[1] & 0x10) != 0) {
    obj_light_vecs_to_model();
  }
  lights_by_inverse_matrix();
  obj_mtx_push_proj();
  obj_lit_send_f(obj,alpha);
  return;
}

void obj_put_lit_world_f(int *obj, float alpha)
{
  if ((obj[1] & 0x10) != 0) {
    obj_light_vecs_to_model_view();
  }
  lights_by_inverse_matrix();
  obj_mtx_push_proj_view();
  obj_lit_send_f(obj,alpha);
  return;
}

void obj_alt_lighting_on(void)
{
    obj_alt_lighting = 1;
}

void obj_alt_lighting_off(void)
{
  obj_alt_lighting = 0;
  return;
}

void obj_lit_send(int *obj)
{
  if (-1 < *obj) {
    if (obj_alt_lighting == 0) {
      lit_groups_send(obj,&obj_send_header);
    }
    else {
      lit_offset_groups_send(obj,&obj_send_header);
    }
  }
  mtx_pop(1);
  return;
}

void obj_lit_send_f(int *obj, float alpha)
{
  uint *qacr = (uint *)0xff000038;
  
  qacr[0] = 0xc;
  qacr[1] = 0xc;
  if (-1 < *obj) {
    if (obj_alt_lighting == 0) {
      lit_groups_send_alpha(obj,&obj_send_header,alpha);
    }
    else {
      lit_offset_groups_send_alpha(obj,&obj_send_header,alpha);
    }
  }
  mtx_pop(1);
}

void obj_put_second_path(int *obj)
{
  if (-1 < *obj) {
    if ((obj[1] & 2U) == 0) {
      obj_light_vecs_to_model();
    }
    mtx_push(0);
    mtx_premul_by_pairs(rect_x_proj_work);
    float_colour_send(obj,&obj_send_header);
    mtx_pop(1);
  }
}

void obj_put_second_path_float(int *obj, float alpha)
{
  if (-1 < *obj) {
    if ((obj[1] & 2U) == 0) {
      obj_light_vecs_to_model();
    }
    mtx_push(0);
    mtx_premul_by_pairs(rect_x_proj_work);
    float_colour_send_alpha(obj,&obj_send_header,alpha);
    mtx_pop(1);
  }
  return;
}

void single_light_color_set(float r, float g, float b)
{
  single_light_color.red = r;
  single_light_color.green = g;
  single_light_color.blue = b;
  return;
}

void second_light_color_set(float r, float g, float b)
{
  second_light_color.red = r;
  second_light_color.green = g;
  second_light_color.blue = b;
  return;
}
