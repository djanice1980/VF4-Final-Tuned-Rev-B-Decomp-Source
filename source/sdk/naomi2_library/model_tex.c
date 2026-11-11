/*
 * sdk/naomi2_library/model_tex.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_texidx_bit_clear
#define NOPROTO_pal_group_unref
#define NOPROTO_tex_slot_words_or_fallback
#define NOPROTO_pal_mark_blocks_used
#define NOPROTO_palette_shadow_write
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int vertex_bytes_lo[32]; /* 0x0c211618 */
extern int vertex_bytes_hi[32]; /* 0x0c211698 */
extern int list_to_model_part[5]; /* 0x0c211718 */
extern uint obj_palette_table; /* 0x0c3dc328 */
extern model_error_rec model_error_record; /* 0x0c3dc158 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

int vertex_bytes_from_format(uint format)
{
  int bytes;
  bytes = vertex_bytes_lo[format & 0x1f];
  format >>= 5;
  bytes += vertex_bytes_hi[format & 0x1f];
  return bytes;
}

uint model_tex_load_walk(compiled_model_rec *model, int tex_list, uint resume, uint *next, int by_entry_ids)
{
  int tex_ref[2];
  int pal_base[2];
  uint *end;
  uint list;
  compiled_model_rec *base;
  uint *tex_words;
  uint *obj;
  int loaded;
  uint result;
  int pal_size;
  model_part_span_rec *part;
  uint *p;
  uint *poly;
  int i;
  int entry;
  int *ref;
  tex_slot_rec *tex_slot;
  texidx_map_entry *map_ent;
  short *group;
  int c;
  uint tex_format;
  uchar kind;
  uint tex_word;
  int n;
  int group_no;
  int *pal_src;
  int *entry_rec;

  base = model;
  loaded = 0;
  result = 0;
  *next = 0;
  list = resume >> 28;
  part = &model->parts[list_to_model_part[list]];
  while (part->offset == 0) {
    list += 2;
    if (list > 4) goto none;
    part = &base->parts[list_to_model_part[list]];
  }
next_bucket:
  end = (uint *)(part->offset + (int)part);
  end = (uint *)((int)end + part->size);
  if ((resume & 0xfffffff) != 0) {
    p = (uint *)((char *)model + (resume & 0xfffffff));
  }
  else {
    p = (uint *)((int)part + part->offset);
    if ((*p >> 8 & 0xf) != 5) {
      *next = 0;
      return result;
    }
  }
  if (p > end) {
    model_error_record.error_bits |= 0x1000000;
    model_error_record.field_0x4 = (uint)p;
    model_error_record.field_0x8 = (uint)end;
    model_error_record.field_0xc = resume;
    model_error_record.field_0x10 = (uint)model;
    model_error_record.field_0x14 = list;
    model_error_record.field_0x18 = 0;
    model_error_record.field_0x1c = 0;
    __asm__ __volatile__ ("trapa #0x38");
  }
  obj = p;
  p = p + 8;
next_group:
  tex_words = p;
  p = p + 8;
  poly = p;
  i = 0;
  tex_ref[0] = tex_words[1];
  pal_base[0] = -1;
slot:
    if (tex_ref[i] >= 0) {
      entry = tex_ref[i];
      if (by_entry_ids == 1) {
        tex_ref[i] = *(int *)(tex_list + entry * 0x10 + 0xc);
      }
      tex_ref[i] = tex_ref[i] + texidx_base;
      c = (int)texidx_map; map_ent = (texidx_map_entry *)(tex_ref[i] * 8 + c);
      if (map_ent->use_count == 0) {
        map_ent->slot_no = texidx_bit_take();
      }
      ref = &tex_ref[i];
      texidx_map[*ref].use_count = texidx_map[*ref].use_count + 1;
      *ref = texidx_map[*ref].slot_no;
      tex_slot = tex_slot_table + *ref;
      c = tex_slot->in_use;
      if (c == 0) {
        entry_rec = (int *)(tex_list + entry * 0x10);
        result |= tex_slot_create_and_load((short *)entry_rec, *ref, entry_rec[2]);
        tex_slot_words_or_fallback(*ref);
        tex_slot->slot_id = *ref;
        loaded = 1;
        c = tex_slot->in_use;
      }
      tex_slot->in_use = c + 1;
    }
    if (i == 0) {
      tex_word = poly[3];
    }
    else {
      tex_word = poly[5];
    }
    tex_format = tex_word >> 0x1b & 7;
    if (pal_raw_mode == 0) {
      if (obj_palette_table != 0 && tex_format - 5 < 2) {
        group_no = tex_words[i * 4 + 3];
        if (group_no < graphics_work.pal_group_count) {
          group = (short *)graphics_work.pal_group_table;
          pal_src = (int *)(group_no * 0xc + obj_palette_table);
          group = group + group_no * 4;
          if (group[2] != 0) {
            group[2] = group[2] + 1;
            n = *group;
          }
          else {
            *((char *)group + 2) = *((char *)pal_src + 2);
            *((char *)group + 3) = (char)((uint)(pal_src[2] + 0xf) >> 4);
            group[2] = 1;
            group[3] = (short)group_no;
            pal_size = pal_src[2];
            n = pal_find_free_blocks(*(short *)((char *)pal_src + 2), pal_size);
            if (n >= 0) {
              *group = (short)n;
              pal_mark_blocks_used(n, pal_size);
              palette_shadow_write(n << 4, (uint *)pal_src[1], pal_size);
            }
            else {
              n = 0;
            }
          }
          pal_base[i] = n;
        }
      }
    }
    else if (tex_format - 5 < 2) {
      pal_base[i] = tex_words[i * 4 + 3];
    }
    if (i == 0) {
      i = 1;
      tex_ref[1] = tex_words[5];
      pal_base[1] = -1;
      goto slot;
    }
  obj[2] = obj[2] & obj_flag_keep_mask;
  obj[2] = obj[2] | obj_flag_force_bits;
  i = obj_flag_force_bits & ~obj_flag_keep_mask;
  if (i & 0x200) obj[4] = 0;
  if (i & 0x400) obj[6] = 0;
  model_obj_load_hook(obj);
  do {
    kind = (*p & 0xf00) >> 8;
    if (kind == 0) {
      p += 2;
    }
    else if (kind == 5) {
      break;
    }
    else if (kind == 7) {
      poly = p;
      n = vertex_bytes_from_format(p[6]);
      p = p + (n * p[7]) / 4 + 8;
      *poly = *poly & 0xfffcffff | poly_header_bits16 << 16 | 0x8c0000;
      if (*poly & 0x80) {
        if (modvol_two_volume == 0) *poly = *poly & ~0x40;
        else *poly = *poly | 0x40;
      }
      if (tex_ref[0] >= 0) {
        tex_slot_bits_into_poly_hdr(poly, tex_ref[0]);
        if (pal_base[0] >= 0) poly[3] = poly[3] & 0xf81fffff | pal_base[0] << 21;
      }
      else {
        tex_builtin_bits_into_poly_hdr(poly);
      }
      if (tex_ref[1] >= 0) {
        tex_slot_bits_into_poly_hdr(poly + 2, tex_ref[1]);
        if (pal_base[1] >= 0) poly[5] = poly[5] & 0xf81fffff | pal_base[1] << 21;
      }
      else {
        tex_builtin_bits_into_poly_hdr(poly + 2);
      }
      model_poly_load_hook(poly);
    }
    else {
      model_error_record.error_bits |= 0x2000000;
      model_error_record.field_0x4 = (uint)p;
      model_error_record.field_0x8 = (uint)end;
      model_error_record.field_0xc = resume;
      model_error_record.field_0x10 = (uint)model;
      model_error_record.field_0x14 = list;
      model_error_record.field_0x18 = 0;
      model_error_record.field_0x1c = 0;
      __asm__ __volatile__ ("trapa #0x39");
    }
  } while (p < end);
  if (p < end) goto more;
  do {
    list += 2;
    if (list > 4) goto none3;
    part = &base->parts[list_to_model_part[list]];
  } while (part->offset == 0);
  if (loaded == 0) {
    resume = 0;
    goto next_bucket;
  }
  *next = list << 28;
  return result;
none:
  *next = 0;
  return result;
none3:
  *next = 0;
  return result;
more:
  if (loaded == 0) {
    obj = p;
    p = p + 8;
    goto next_group;
  }
  *next = list << 28 | (int)p - (int)model;
  return result;
}

uint model_tex_load(compiled_model_rec *model, int tex_list)
{
  uint result;
  int cursor;

  cursor = 0;
  result = 0;
  if (model->format != 0x100) {
    return obj_textures_load((int *)model,tex_list);
  }
  do {
    result |= model_tex_load_walk(model,tex_list,cursor,&cursor,0);
  } while (cursor != 0);
  return result;
}

uint model_tex_load_by_entry_ids(compiled_model_rec *model, int tex_list)
{
  uint result;
  int cursor;

  cursor = 0;
  result = 0;
  if (model->format != 0x100)
    return obj_textures_load_mapped((int *)model,tex_list);
  do {
    result |= model_tex_load_walk(model,tex_list,cursor,(uint *)&cursor,1);
  } while (cursor != 0);
  return result;
}

void model_tex_load_step(compiled_model_rec *model, int tex_list, uint resume, uint *next)
{
  if (model->format != 0x100) {
    ((int (*)())obj_textures_load_from)(model,tex_list,resume,next);
  }
  else {
    model_tex_load_walk(model,tex_list,resume,next,0);
    return;
  }
}

int model_tex_load_step_by_entry_ids(compiled_model_rec *model, int tex_list, uint resume, uint *next)
{
  if (model->format != 0x100) {
    return ((int (*)())obj_textures_load_from_mapped)(model,tex_list,resume,next);
  }
  return model_tex_load_walk(model,tex_list,resume,next,1);
}

uint model_tex_unload_walk(compiled_model_rec *model, int tex_list, uint resume, uint *next, int by_entry_ids)
{
  uint *end;
  compiled_model_rec *base;
  uint *tex_words;
  uint *poly;
  int freed;
  uint result;
  uint list;
  model_part_span_rec *part;
  uint *obj;
  int i;
  int tex_ref;
  int slot_no;
  uint tex_word;
  uchar kind;
  int vertex_bytes;
  uint tex_format;

  base = model;
  freed = 0;
  result = 0;
  *next = 0;
  list = resume >> 28;
  part = &model->parts[list_to_model_part[list]];
  while (part->offset == 0) {
    list += 2;
    if (list > 4) goto fail;
    part = &base->parts[list_to_model_part[list]];
  }
section:
  end = (uint *)((char *)part + part->offset);
  end = (uint *)((char *)end + part->size);
  if ((resume & 0xfffffff) != 0) {
    obj = (uint *)((char *)model + (resume & 0xfffffff));
  }
  else {
    obj = (uint *)((char *)part + part->offset);
    if ((*obj >> 8 & 0xf) != 5) {
      *next = 0;
      return result;
    }
  }
  if (obj > end) {
    model_error_record.error_bits = model_error_record.error_bits | 0x1000000;
    model_error_record.field_0x4 = (uint)obj;
    model_error_record.field_0x8 = (uint)end;
    model_error_record.field_0xc = resume;
    model_error_record.field_0x10 = (uint)model;
    model_error_record.field_0x14 = list;
    model_error_record.field_0x18 = 0;
    model_error_record.field_0x1c = 0;
    __asm__ volatile ("trapa #0x38");
  }
  obj += 8;
object:
  tex_words = obj;
  obj += 8;
  poly = obj;
  i = 0;
  tex_ref = tex_words[1];
slot:
    if (tex_ref >= 0) {
      if ((by_entry_ids != 0) & (tex_list != 0)) {
        tex_ref = *(int *)(tex_ref * 16 + tex_list + 12);
      }
      tex_ref += texidx_base;
      if (texidx_map[tex_ref].use_count != 0) {
        texidx_map[tex_ref].use_count = texidx_map[tex_ref].use_count + -1;
        if (texidx_map[tex_ref].use_count == 0) {
          slot_no = texidx_map[tex_ref].slot_no;
          if (tex_slot_table[slot_no].status == 0) {
            result |= surface_memory_return_by_kind(&tex_slot_table[slot_no].surface);
          }
          tex_slot_table[slot_no].in_use = 0;
          tex_slot_table[slot_no].slot_id = -1;
          texidx_bit_clear(slot_no);
          texidx_map[tex_ref].slot_no = 0xffffffff;
          freed = 1;
        }
      }
    }
    if (i == 0) {
      tex_word = poly[3];
    }
    else {
      tex_word = poly[5];
    }
    tex_format = tex_word >> 0x1b & 7;
    if (pal_raw_mode == '\0' && obj_palette_table != 0 && tex_format - 5 < 2) {
      pal_group_unref(tex_words[i * 4 + 3]);
    }
    if (i == 0) {
      i = 1;
      tex_ref = tex_words[5];
      goto slot;
    }
  do {
    kind = (*obj & 0xf00) >> 8;
    if (kind == 0) {
      obj += 2;
    }
    else if (kind == 5) {
      break;
    }
    else if (kind == 7) {
      vertex_bytes = vertex_bytes_from_format(obj[6]);
      obj = obj + (vertex_bytes * obj[7]) / 4 + 8;
    }
    else {
      model_error_record.error_bits = model_error_record.error_bits | 0x2000000;
      model_error_record.field_0x4 = (uint)obj;
      model_error_record.field_0x8 = (uint)end;
      model_error_record.field_0xc = resume;
      model_error_record.field_0x10 = (uint)model;
      model_error_record.field_0x14 = list;
      model_error_record.field_0x18 = 0;
      model_error_record.field_0x1c = 0;
      __asm__ volatile ("trapa #0x39");
    }
  } while (obj < end);
rec5:
  if (obj < end) goto more;
  do {
    list += 2;
    if (list > 4) goto none;
    part = &base->parts[list_to_model_part[list]];
  } while (part->offset == 0);
  if (freed == 0) {
    resume = 0;
    goto section;
  }
  *next = list << 28;
  return result;
fail:
  *next = 0;
  return result;
none:
  *next = 0;
  return result;
more:
  if (freed == 0) {
    obj += 8;
    goto object;
  }
  *next = list << 28 | (int)obj - (int)model;
  return result;
}

uint model_tex_unload(compiled_model_rec *model)
{
  uint step;
  uint result;
  int cursor [1];
  
  cursor[0] = 0;
  result = 0;
  do {
    step = model_tex_unload_walk(model,0,cursor[0],cursor,0);
    result = result | step;
  } while (cursor[0] != 0);
  return result;
}

uint model_tex_unload_by_entry_ids(compiled_model_rec *model, int tex_list)
{
  uint step;
  uint result;
  int cursor [1];
  
  cursor[0] = 0;
  result = 0;
  do {
    step = model_tex_unload_walk(model,tex_list,cursor[0],cursor,1);
    result = result | step;
  } while (cursor[0] != 0);
  return result;
}

void model_tex_unload_step(compiled_model_rec *model, uint resume, uint *next)
{
  model_tex_unload_walk(model,0,resume,next,0);
  return;
}

void model_tex_unload_step_by_entry_ids(compiled_model_rec *model, int tex_list, uint resume, uint *next)
{
  model_tex_unload_walk(model,tex_list,resume,next,1);
  return;
}

int model_tex_rebind_core(compiled_model_rec *model, int tex_list, int by_entry_ids)
{
  int tex_slot[2];
  uint pal_bits;
  uint i;
  compiled_model_rec *base;
  int missing;
  int j;
  model_part_span_rec *part;
  uint *p;
  uint *end;
  uint *tex_words;
  uint kind;
  int vertex_bytes;
  
  base = model;
  missing = 0;
  if (model->format != 0x100 || model->size == 0) return missing;
  for (i = 0; i < 5; i += 2) {
    part = &base->parts[list_to_model_part[i]];
    if (part->offset != 0) {
      p = (uint *)((char *)part + part->offset);
      end = (uint *)((char *)p + part->size);
      if ((*p >> 8 & 0xf) == 5) {
        if (p > end) {
          model_error_record.error_bits = model_error_record.error_bits | 0x1000000;
          model_error_record.field_0x4 = (uint)p;
          model_error_record.field_0x8 = (uint)end;
          model_error_record.field_0xc = 0;
          model_error_record.field_0x10 = (uint)model;
          model_error_record.field_0x14 = i;
          model_error_record.field_0x18 = tex_list;
          model_error_record.field_0x1c = 0;
          __asm__ volatile ("trapa #0x38");
        }
      block:
        p = p + 8;
        tex_words = p;
        p = p + 8;
        for (j = 0; j < 2; j++) {
          tex_slot[j] = tex_words[j * 4 + 1];
          if (tex_slot[j] < 0) continue;
          if ((by_entry_ids != 0) & (tex_list != 0)) {
            tex_slot[j] = *(int *)(tex_slot[j] * 16 + tex_list + 12);
          }
          tex_slot[j] = tex_slot[j] + texidx_base;
          if (texidx_map[tex_slot[j]].use_count == 0) {
            missing = 1;
            tex_slot[j] = -1;
          }
          else {
            tex_slot[j] = texidx_map[tex_slot[j]].slot_no;
          }
        }
        do {
          kind = *p >> 8 & 0xf;
          if (kind == 0) {
            p = p + 2;
          }
          else if (kind == 5) {
            break;
          }
          else if (kind == 7) {
            uint *poly = p;
            vertex_bytes = vertex_bytes_from_format(p[6]);
            p = p + (vertex_bytes * p[7]) / 4 + 8;
            pal_bits = poly[3] & 0x7e00000;
            if (tex_slot[0] >= 0) {
              tex_slot_bits_into_poly_hdr(poly,tex_slot[0]);
            }
            else {
              tex_builtin_bits_into_poly_hdr(poly);
            }
            poly[3] = poly[3] | pal_bits;
            pal_bits = poly[5] & 0x7e00000;
            if (tex_slot[1] >= 0) {
              tex_slot_bits_into_poly_hdr(poly + 2,tex_slot[1]);
            }
            else {
              tex_builtin_bits_into_poly_hdr(poly + 2);
            }
            poly[5] = poly[5] | pal_bits;
          }
          else {
            model_error_record.error_bits = model_error_record.error_bits | 0x2000000;
            model_error_record.field_0x4 = (uint)p;
            model_error_record.field_0x8 = (uint)end;
            model_error_record.field_0xc = 0;
            model_error_record.field_0x10 = (uint)model;
            model_error_record.field_0x14 = i;
            model_error_record.field_0x18 = tex_list;
            model_error_record.field_0x1c = 0;
            __asm__ volatile ("trapa #0x39");
          }
        } while (p < end);
        if (p < end) goto block;
      }
    }
  }
  return missing;
}

void model_tex_rebind(compiled_model_rec *model)
{
  model_tex_rebind_core(model,0,0);
  return;
}

void model_tex_rebind_by_entry_ids(compiled_model_rec *model, int tex_list)
{
  model_tex_rebind_core(model,tex_list,1);
  return;
}

void obj_flag_override_set(uint field, uint value)
{
  uint *f;

  if (field < 4) {
    f = obj_flag_fields[field];
    obj_flag_keep_mask &= ~(f[0] << f[1]);
    obj_flag_force_bits |= value << f[1];
  }
  return;
}

void obj_flag_override_clear(uint field)
{
  uint *f;

  if (field < 4) {
    f = obj_flag_fields[field];
    obj_flag_keep_mask |= f[0] << f[1];
    obj_flag_force_bits &= ~(f[0] << f[1]);
  }
  return;
}

void obj_flag_overrides_reset(void)
{
    obj_flag_keep_mask = -1;
    obj_flag_force_bits = 0;
}

void model_obj_load_hook(uint *obj)
{
  return;
}

void model_poly_load_hook(uint *poly)
{
  return;
}
