/*
 * vf4lib/libsprite/src/aet_play.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float aet_depth_levels[14]; /* 0x0c322668 */
extern int aet_obj_next_handle; /* 0x0c4ee0cc */
extern aet_draw_state_rec aet_draw_state; /* 0x0c4ee0d0 */
extern aet_obj_rec aet_objs[64]; /* 0x0c4ee188 */
extern aet_obj_rec *aet_cur_obj; /* 0x0c4eff88 */
extern int aet_still_next_handle; /* 0x0c4eff8c */
extern aet_still_rec aet_stills[64]; /* 0x0c4eff90 */
extern aet_still_rec *aet_cur_still; /* 0x0c4f1a90 */
extern aet_still_cache_rec aet_still_caches[64]; /* 0x0c4f1a94 */
extern aet_still_cache_rec *aet_cur_cache; /* 0x0c50e894 */

void ev_aet_task_start(void)
{
  aet_objs_init();
  aet_task_start_nop();
  aet_stills_init();
  return;
}

void ev_aet_task_frame(void)
{
  aet_objs_step();
  aet_objs_draw();
  aet_stills_draw();
  return;
}

void ev_aet_task_stop(void)
{
  aet_objs_stop_all();
  aet_task_stop_nop();
  aet_stills_init();
  return;
}

int aet_objs_init(void)
{
  int i;

  aet_obj_next_handle = 0;
  memset(&aet_draw_state,0,sizeof(aet_draw_state));
  memset(aet_objs,0,sizeof(aet_objs));
  i = 0;
  do {
    aet_obj_clear(i);
    i = i + 1;
  } while (i < 0x40);
  return 1;
}

int aet_objs_step(void)
{
  aet_obj_rec *obj;
  int i;

  if (pause_frame_held() != 0) return 0;
  obj = aet_objs_table();
  for (i = 0; i < 64; i++, obj++) {
    if (obj->in_use == 0) continue;
    if ((obj->param.flags & 8) != 0) continue;
    if ((obj->param.flags & 0x10) != 0) {
      obj->frame = obj->frame - 1;
    } else {
      obj->frame = obj->frame + 1;
    }
    obj->end_reached = 0;
    if (obj->frame >= obj->end_frame) {
      if (obj->param.loop_marker != 0) obj->start_frame = obj->loop_frame;
      if ((obj->param.flags & 4) != 0) {
        if (obj->end_frame != obj->first_end_frame) obj->frame = obj->end_frame;
        else obj->frame = obj->end_frame - 1;
        obj->end_reached = 1;
      } else if ((obj->param.flags & 2) != 0) {
        obj->frame = obj->start_frame;
      } else {
        aet_obj_clear(i);
      }
    } else {
      if (obj->frame >= obj->start_frame) continue;
      if (obj->param.loop_marker != 0) obj->start_frame = obj->loop_frame;
      if ((obj->param.flags & 4) != 0) {
        obj->frame = obj->start_frame;
        obj->end_reached = 1;
      } else if ((obj->param.flags & 2) != 0) {
        obj->frame = obj->end_frame - 1;
      } else {
        aet_obj_clear(i);
      }
    }
  }
  return 0;
}

int aet_objs_stop_all(void)
{
  aet_obj_rec *obj;
  int i;

  obj = aet_objs_table();
  i = 0;
  do {
    if (obj->in_use == '\x01') {
      aet_obj_clear(i);
    }
    i = i + 1;
    obj = obj + 1;
  } while (i < 0x40);
  return 1;
}

int aet_task_start_nop(void)
{
  return 1;
}

int aet_objs_draw(void)
{
  aet_obj_rec *obj;
  int i;

  hdr_or_fog(2);
  obj = aet_objs_table();
  for (i = 0; i < 0x40; i++, obj++) {
    aet_draw_state_rec *state = &aet_draw_state;
    state->last_skip = 0;
    state->last_blend = 0;
    state->last_alpha = 0.0f;
    state->field_0xac = 0xff;
    aet_draw_state.blend_override = 0;
    aet_draw_state.quads_drawn = 0;
    if (((obj->in_use != '\0') && ((obj->param.flags & 0x20) == 0)) &&
       ((obj->flags & 0x40) == 0)) {
      if (obj->param.filter != '\0') {
        hdr_or_filter(1);
      }
      aet_cur_obj_set(obj);
      obj->param.depth = obj->param.depth_top;
      aet_obj_draw(obj);
      if (obj->param.filter != '\0') {
        hdr_or_filter(0);
      }
    }
  }
  aet_blend_select(-1);
  hdr_or_fog(0);
  return 0;
}

int aet_task_stop_nop(void)
{
  return 1;
}

int aet_obj_start(aet_obj_param *param)
{
  aet_obj_rec *obj;
  int idx;
  int i;
  aet_scene *scene;
  aet_comp *comp;
  int frame;
  aet_layer *layer;

  idx = -1;
  obj = aet_objs_table();
  for (i = 0; i < 0x40; i++, obj++) {
    if (obj->in_use != 1) {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    return 0xffffffff;
  }
  aet_obj_clear(idx);
  obj->param = *param;
  scene = aet_scene_get(obj->param.scene_id);
  if (scene == 0) {
    return 0xffffffff;
  }
  obj->scene = scene;
  comp = aet_main_comp_get(scene);
  obj->comp = comp;
  if (obj->param.layer_name != 0) {
    obj->layer = aet_comp_layer_find(comp,obj->param.layer_name);
  }
  layer = obj->layer;
  if (layer != 0) {
    obj->start_frame = (int)layer->start_frame;
    obj->end_frame = (int)layer->end_frame;
  }
  else {
    obj->start_frame = (int)obj->comp->start_frame;
    obj->end_frame = (int)obj->comp->end_frame;
  }
  obj->field_0x68 = obj->start_frame;
  obj->first_end_frame = obj->end_frame;
  if (obj->param.start_marker != 0) {
    frame = aet_obj_marker_frame(obj->handle,obj->param.start_marker);
    if (frame != -1) {
      obj->start_frame = frame;
    }
  }
  if (obj->param.end_marker != 0) {
    frame = aet_obj_marker_frame(obj->handle,obj->param.end_marker);
    if (frame != -1) {
      obj->end_frame = frame;
    }
  }
  if (obj->param.loop_marker != 0) {
    frame = aet_obj_marker_frame(obj->handle,obj->param.loop_marker);
    if (frame != -1) {
      obj->loop_frame = frame;
    }
  }
  if ((obj->param.flags & 0x10) != 0) {
    obj->frame = obj->end_frame - 1;
  }
  else {
    obj->frame = obj->start_frame;
  }
  obj->in_use = 1;
  obj->handle = aet_obj_next_handle;
  if (param->tex_table == 0) {
    aet_param_tex_table_set(&obj->param,aet_scene_tex_table_get(param->scene_id));
  }
  aet_obj_next_handle = aet_obj_next_handle + 1;
  if (aet_obj_next_handle == -1) {
    aet_obj_next_handle = 0;
  }
  return obj->handle;
}

int aet_obj_stop(int handle)
{
  aet_obj_rec *obj = aet_obj_get(handle);
  if (obj == 0) return 0;
  if (obj->in_use == 0) return 1;
  aet_obj_clear(aet_obj_index(handle));
  return 1;
}

int aet_objs_stop_by_scene(int scene_id)
{
  aet_obj_rec *obj;
  int i;
  int n;

  obj = aet_objs_table();
  n = 0;
  i = 0;
  do {
    if ((obj->in_use != '\0') && (obj->param.scene_id == scene_id)) {
      aet_obj_clear(i);
      n = n + 1;
    }
    i = i + 1;
    obj = obj + 1;
  } while (i < 0x40);
  return n;
}

void aet_param_init(aet_obj_param *param)
{
  if (param != 0) {
    memset(param,0,sizeof(aet_obj_param));
    param->scene_id = 0xffffffff;
    param->tex_table = 0;
    param->flags = 1;
    param->field_0xc = 0;
    param->x = 0.0;
    param->y = 0.0;
    param->rotation = 0.0;
    param->depth = 0.15;
    param->color = 0xffffff;
    param->alpha = 0xff;
    param->start_marker = 0;
    param->end_marker = 0;
    param->loop_marker = 0;
    param->layer_name = 0;
    param->filter = 1;
  }
  return;
}

void aet_param_scene_set(aet_obj_param *param, uint scene_id)
{
  if (param != 0) {
    param->scene_id = scene_id;
  }
  return;
}

void aet_param_pos_set(aet_obj_param *param, float x, float y)
{
  if (param != 0) {
    param->x = x;
    param->y = y;
  }
  return;
}

void aet_param_depth_range_set(aet_obj_param *param, float top, float bottom)
{
  aet_obj_param *p = param;
  float *level;
  uchar i;

  if (p != 0) {
    if (__builtin_fabsf(top - bottom) < 1.1920929e-07) {
      p->depth_bottom = p->depth = bottom + 0.05;
      for (i = 0; i < 13; i++) {
        level = aet_depth_levels + i;
        if (__builtin_fabsf(level[0] - top) < 1.1920929e-07) {
          p->depth_top = level[1] + 0.05;
          p->depth_step = (p->depth_top - p->depth_bottom) / 1000.0;
          return;
        }
      }
    }
    else {
      p->depth_bottom = p->depth = bottom + 0.05;
      p->depth_top = top + 0.05;
      p->depth_step = (p->depth_top - p->depth_bottom) / 1000.0;
    }
  }
}

void aet_param_color_set(aet_obj_param *param, uint color)
{
  if (param != 0) {
    param->color = color;
  }
  return;
}

void aet_param_filter_set(aet_obj_param *param, byte filter)
{
  if (param != 0) {
    param->filter = filter;
  }
  return;
}

void aet_param_alpha_set(aet_obj_param *param, float alpha)
{
  if (param != 0) {
    param->alpha = (int)(alpha * 255.0);
  }
  return;
}

void aet_param_tex_table_set(aet_obj_param *param, int *tex_table)
{
  if (param != 0) {
    param->tex_table = tex_table;
  }
  return;
}

void aet_param_rotation_set(aet_obj_param *param, float rotation)
{
  if (param != 0) {
    param->rotation = rotation;
  }
  return;
}

void aet_param_flags_set(aet_obj_param *param, int flags)
{
  if (param != 0) {
    param->flags = flags;
  }
  return;
}

void aet_param_layer_set(aet_obj_param *param, char *layer_name)
{
  if (param != 0) {
    param->layer_name = layer_name;
  }
  return;
}

void aet_param_start_marker_set(aet_obj_param *param, char *marker_name)
{
  if (param != 0) {
    param->start_marker = marker_name;
  }
  return;
}

void aet_param_end_marker_set(aet_obj_param *param, char *marker_name)
{
  if (param != 0) {
    param->end_marker = marker_name;
  }
  return;
}

void aet_param_loop_marker_set(aet_obj_param *param, char *marker_name)
{
  if (param != 0) {
    param->loop_marker = marker_name;
  }
  return;
}

int aet_obj_frame_get(int handle)
{
  aet_obj_rec *obj;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    return obj->frame;
  }
  return 0;
}

int aet_obj_start_frame_get(int handle)
{
  aet_obj_rec *obj;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    return obj->start_frame;
  }
  return 0;
}

int aet_obj_end_frame_get(int handle)
{
  aet_obj_rec *obj;

  obj = aet_obj_get(handle);
  if (0 != obj) {
    return obj->end_frame;
  }
  return 0;
}

int aet_obj_end_reached(int handle)
{
  aet_obj_rec *obj;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    return obj->end_reached;
  }
  return 0;
}

void aet_obj_bounds_measure(int handle, aet_box *box)
{
  aet_obj_rec *obj;
  aet_draw_state_rec *state;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    aet_cur_obj_set(obj);
    state = &aet_draw_state;
    state->measuring = 1;
    state->box.min.x = 2024.0;
    state->box.min.y = 2024.0;
    state->box.max.x = 0.0;
    state->box.max.y = 0.0;
    aet_obj_draw(obj);
    state->measuring = 0;
    *box = state->box;
  }
  return;
}

void aet_obj_markers_set(int handle, char *start_marker, char *end_marker)
{
  bool changed;
  aet_obj_rec *obj;
  float frame;

  changed = false;
  obj = aet_obj_get(handle);
  if (obj != 0) {
    if (start_marker != 0) {
      frame = aet_obj_marker_frame(handle,start_marker);
      if ((int)frame != -1) {
        obj->param.start_marker = start_marker;
        obj->start_frame = (int)frame;
        changed = true;
      }
    }
    if (end_marker != 0) {
      frame = aet_obj_marker_frame(handle,end_marker);
      if ((int)frame != -1) {
        obj->param.end_marker = end_marker;
        obj->end_frame = (int)frame;
        changed = true;
      }
    }
    if (changed) {
      if ((obj->param.flags & 0x10) != 0) {
        obj->frame = obj->end_frame;
      }
      else {
        obj->frame = obj->start_frame;
      }
    }
    obj->end_reached = 0;
  }
  return;
}

void aet_obj_depth_set(int handle, float depth)
{
  aet_obj_rec *obj;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    obj->param.depth = depth + 0.05;
  }
  return;
}

void aet_obj_flags_clear(int handle)
{
  aet_obj_rec *obj;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    obj->param.flags = 0;
  }
  return;
}

void aet_obj_flag_put(int handle, int flag, char on)
{
  aet_obj_rec *obj;
  obj = aet_obj_get(handle);
  if (obj != 0) {
    obj->param.flags &= ~flag;
    if (on != 0) {
      obj->param.flags |= flag;
    }
  }
}

void aet_obj_frame_add(int handle, int delta)
{
  aet_obj_rec *obj;
  int frame;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    frame = obj->frame + delta;
    obj->frame = frame;
    if (frame < obj->start_frame) {
      obj->frame = obj->start_frame;
    }
    if (obj->frame >= obj->end_frame) {
      obj->frame = obj->end_frame + -1;
    }
  }
  return;
}

void aet_obj_pos_set(int handle, float x, float y)
{
  aet_obj_rec *obj;

  obj = aet_obj_get(handle);
  if (obj != 0) {
    obj->param.x = x;
    obj->param.y = y;
  }
  return;
}

void aet_obj_hide_set(int handle, unsigned char hide)
{
  aet_obj_rec *obj;
  obj = aet_obj_get(handle);
  if (obj != 0) {
    if (hide != 0) {
      obj->flags = obj->flags | 0x40;
    }
    else {
      obj->flags = obj->flags & 0xffffffbf;
    }
  }
}

void aet_obj_markers_print(int column, int row, int handle)
{
  aet_comp *comp;
  aet_comp *child;
  aet_layer *layer;
  aet_layer *child_layer;
  aet_marker *marker;
  int line;
  int i;
  int j;
  int k;
  int j2;
  aet_marker *child_marker;
  int y;
  int rows;
  aet_obj_rec *obj;
  int y2;
  int y3;
  int y4;

  obj = aet_obj_get(handle);
  if (obj == 0 || obj->comp == 0) return;
  comp = obj->comp;
  rows = 0x3c;
  line = 0;
  for (i = 0; i < comp->layer_count; i++) {
    layer = &comp->layers[i];
    y = row + line;
    if (y >= 0) {
      if (y < rows) {
        text_color_set(0xff00ffff);
        text_cursor_to_cell(column,y);
        text_draw_formatted("%s",layer->name);
        text_color_set(0xffffffff);
      }
    }
    line++;
    for (j = 0; j < layer->marker_count; j++) {
      marker = &layer->markers[j];
      y2 = row + line;
      if (y2 >= 0) {
        if (y2 < rows) {
          text_cursor_to_cell(column,y2);
          text_draw_formatted("%s %3.2f\n",marker->name,marker->frame);
        }
      }
      line++;
    }
    if (layer->type == 1) {
      child = layer->item.comp;
      for (k = 0; k < child->layer_count; k++) {
        child_layer = &child->layers[k];
        y3 = row + line;
        if (y3 >= 0) {
          if (y3 < rows) {
            text_cursor_to_cell(column,y3);
            text_draw_formatted("  %s\n",child_layer->name);
          }
        }
        line++;
        for (j2 = 0; j2 < child_layer->marker_count; j2++) {
          child_marker = &child_layer->markers[j2];
          y4 = row + line;
          if (y4 >= 0) {
            if (y4 < rows) {
              text_color_set(0xff00ffff);
              text_cursor_to_cell(column,y4);
              text_draw_formatted("   %s, %3.2f\n",child_marker->name,child_marker->frame);
              text_color_set(0xffffffff);
            }
          }
          line++;
        }
      }
    }
  }
  text_color_set(0xffffffff);
  return;
}

aet_marker * aet_layer_marker_find(aet_layer *layer, char *name)
{
  aet_marker *marker;
  int i;

  if (layer != 0 && name != 0) {
    if (layer->type == '\x01') {
      marker = aet_comp_marker_find(layer->item.comp,name);
      if (marker != 0) {
        return marker;
      }
    }
    for (i = 0; i < layer->marker_count; i++) {
      char *marker_name = layer->markers[i].name;
      if (strcmp(name,marker_name) == 0) {
        return &layer->markers[i];
      }
    }
  }
  return 0;
}

float aet_obj_marker_frame(int handle, char *name)
{
  aet_obj_rec *obj;
  aet_marker *marker;

  if (name == 0) return -1.0;
  obj = aet_obj_get(handle);
  if (obj == 0) return -1.0;
  if (obj->comp == 0) return -1.0;
  if (obj->layer != 0) {
    marker = aet_layer_marker_find(obj->layer,name);
  }
  else {
    marker = aet_comp_marker_find(obj->comp,name);
  }
  if (marker == 0) return -1.0;
  return marker->frame;
}

int aet_obj_is_playing(int handle)
{
  aet_obj_rec *obj = aet_obj_get(handle);
  if (obj == 0) return 0;
  if (obj->in_use == 0) return 0;
  return obj->handle == handle;
}

int aet_objs_count_by_set(uint set_id)
{
  aet_obj_rec *obj;
  int i;
  int n;
  obj = aet_objs_table();
  n = 0;
  for (i = 0; i < 0x40; i++, obj++) {
    if (obj->in_use != '\0') {
      if (set_id == ((obj->param.scene_id >> 16) & 0xffff)) n++;
    }
  }
  return n;
}

int aet_objs_count_by_scene(int scene_id)
{
  aet_obj_rec *obj;
  int i;
  int n;

  obj = aet_objs_table();
  n = 0;
  i = 0;
  do {
    if (obj->in_use != '\0') {
      n = n + (uint)(scene_id == obj->param.scene_id);
    }
    i = i + 1;
    obj = obj + 1;
  } while (i < 0x40);
  return n;
}

int aet_objs_stop_by_set(uint set_id)
{
  aet_obj_rec *obj;
  int i;
  int n;
  obj = aet_objs_table();
  n = 0;
  for (i = 0; i < 0x40; i++, obj++) {
    if ((obj->in_use != '\0') && (set_id == (obj->param.scene_id >> 0x10 & 0xffff))) {
      aet_obj_clear(i);
      n++;
    }
  }
  return n;
}

int aet_obj_scene_get(int handle)
{
  aet_obj_rec *obj = aet_obj_get(handle);
  if (obj == 0) return 0;
  if (obj->in_use == 0) return -1;
  return obj->param.scene_id;
}

int aet_obj_index(int handle)
{
  aet_obj_rec *obj;
  int i;

  obj = aet_objs_table();
  i = 0;
  do {
    if (obj->handle == handle) {
      return i;
    }
    i = i + 1;
    obj = obj + 1;
  } while (i < 0x40);
  return -1;
}

aet_obj_rec * aet_objs_table(void)
{
  return aet_objs;
}

aet_obj_rec * aet_obj_get(int handle)
{
  int index;

  index = aet_obj_index(handle);
  if (!(index != -1)) {
    return 0;
  }
  return &aet_objs[index];
}

aet_obj_rec * aet_cur_obj_get(void)
{
  return aet_cur_obj;
}

void aet_cur_obj_set(aet_obj_rec *obj)
{
  aet_cur_obj = obj;
  return;
}

void aet_obj_clear(int index)
{
  aet_obj_rec *obj;

  if (index != -1) {
    obj = aet_objs_table() + index;
    memset(obj,0,sizeof(aet_obj_rec));
    obj->in_use = 0;
    obj->handle = 0xffffffff;
    obj->scene = 0;
    obj->comp = 0;
    obj->layer = 0;
    obj->frame = 0;
    memset(&obj->param,0,sizeof(aet_obj_param));
    obj->param.color = 0xffffff;
    obj->param.alpha = 0xff;
  }
  return;
}

void aet_last_quad_keep(aet_quad *quad)
{
  aet_draw_state.last_quad = *quad;
}

void aet_last_skip_put(int skip)
{
  aet_draw_state.last_skip = skip;
  return;
}

void aet_last_blend_put(int blend)
{
  aet_draw_state.last_blend = blend;
  return;
}

void aet_last_alpha_put(float alpha)
{
  aet_draw_state.last_alpha = alpha;
  return;
}

void aet_rect_corners_xform(vec3 *corners, int width, int height)
{
  int i;
  float rect[4][3];
  rect[3][0] = 0.0;
  rect[0][0] = 0.0;
  rect[1][1] = 0.0;
  rect[0][1] = 0.0;
  rect[2][0] = (float)width;
  rect[1][0] = (float)width;
  rect[3][1] = (float)height;
  rect[2][1] = (float)height;
  rect[3][2] = 0.0;
  rect[2][2] = 0.0;
  rect[1][2] = 0.0;
  rect[0][2] = 0.0;
  for (i = 0; i < 4; i++) {
    mtx_xform_point(rect[i],&corners[i].x);
  }
}

void aet_quad_build(aet_quad *quad, aet_picture *picture, float width, float height, float alpha)
{
  float rect[4][3];
  float xformed[4][3];
  uint color;
  int i;
  aet_quad_vertex *vtx;
  float *pt;

  if (quad->alpha != 0) {
    color = 0xffffff;
    if (quad->color != 0xffffff) color = quad->color;
    rect[0][0] = rect[2][0] = 0.0f;
    rect[0][1] = rect[1][1] = 0.0f;
    rect[1][0] = rect[3][0] = width;
    rect[2][1] = rect[3][1] = height;
    rect[0][2] = rect[1][2] = rect[2][2] = rect[3][2] = 0.0f;
    if (picture->u0 > picture->u1) {
      quad->v[0].u = quad->v[1].u = picture->u0;
      quad->v[2].u = quad->v[3].u = picture->u1;
      quad->v[1].v = quad->v[3].v = 1.0f - picture->v1;
      quad->v[0].v = quad->v[2].v = 1.0f - picture->v0;
    } else {
      quad->v[0].u = quad->v[2].u = picture->u0;
      quad->v[1].u = quad->v[3].u = picture->u1;
      quad->v[0].v = quad->v[1].v = 1.0f - picture->v0;
      quad->v[2].v = quad->v[3].v = 1.0f - picture->v1;
    }
    for (i = 0; i < 4; i++) {
      mtx_xform_point(rect[i], xformed[i]);
      vtx = (aet_quad_vertex *)(i * 0x1c + (int)quad);
      pt = xformed[i];
      vtx->x = *pt++;
      vtx->y = *pt;
      vtx->z = quad->z;
      quad->v[i].color = (quad->alpha << 24) | color;
      quad->v[i].field_0x18 = 0;
    }
  }
}

void aet_blend_select(char blend)
{
  switch(blend) {
  case -1:
    blend_factor_pair_keep(4,5);
    break;
  case 0:
    blend_factor_pair_keep(4,5);
    break;
  case 4:
    blend_factor_pair_keep(4,1);
    break;
  case 12:
    blend_factor_pair_keep(4,5);
    break;
  default:
    blend_factor_pair_keep(4,5);
    break;
  }
  return;
}

void aet_corners_bounds(vec3 *corners, vec3 *min, vec3 *max)
{
  int i;

  *max = *corners;
  *min = *max;
  for (i = 1; i < 4; i++) {
    if (corners[i].x > max->x) {
      max->x = corners[i].x;
    }
    if (corners[i].y > max->y) {
      max->y = corners[i].y;
    }
    if (corners[i].x < min->x) {
      min->x = corners[i].x;
    }
    if (corners[i].y < min->y) {
      min->y = corners[i].y;
    }
  }
}

void aet_bounds_layer_add(aet_layer *layer, aet_image *image, float alpha)
{
  vec3 corners[4];
  vec3 mn;
  vec3 mx;
  aet_draw_state_rec *state;

  if ((layer == 0) | (image == 0)) {
    return;
  }
  aet_rect_corners_xform(corners, image->width, image->height);
  state = &aet_draw_state;
  if (state->measuring != 1) {
    return;
  }
  aet_corners_bounds(corners, &mn, &mx);
  if (mn.x < state->box.min.x) {
    state->box.min.x = mn.x;
  }
  if (mn.y < aet_draw_state.box.min.y) {
    aet_draw_state.box.min.y = mn.y;
  }
  if (mx.x > aet_draw_state.box.max.x) {
    aet_draw_state.box.max.x = mx.x;
  }
  if (mx.y > aet_draw_state.box.max.y) {
    aet_draw_state.box.max.y = mx.y;
  }
}

void aet_image_layer_draw(aet_layer *layer, aet_image *image, float frame, float alpha)
{
  aet_quad quad;
  aet_obj_rec *obj;
  aet_picture *picture;
  int bad;
  char blend;

  bad = (layer == 0) | (image == 0);
  if (bad) return;
  obj = aet_cur_obj_get();
  if (obj == 0) return;
  aet_image_picture_set(image,frame);
  if (image->pictures == 0) return;
  picture = &image->pictures[image->picture];
  if (picture == 0) return;
  if (picture->tex_index < 0) return;
  quad.tex = obj->param.tex_table[picture->tex_index];
  quad.field_0x7c = obj->param.field_0xc;
  quad.z = obj->param.depth;
  quad.color = 0xffffff;
  if (obj->param.color != 0xffffff) {
    quad.color = obj->param.color;
  }
  quad.alpha = (int)(alpha * 255.0);
  if (obj->param.alpha != 0xff) {
    quad.alpha = obj->param.alpha;
  }
  aet_quad_build(&quad,picture,(float)image->width,(float)image->height,alpha);
  if (layer->no_send == '\0') {
    if (aet_draw_state.blend_override != 0) {
      blend = -1;
    }
    else {
      blend = layer->blend;
    }
    aet_blend_select(blend);
    aet_strip_send(&quad);
    aet_depth_step();
  }
  aet_blend_select(-1);
  aet_last_skip_put(layer->no_send);
  aet_last_blend_put(layer->blend);
  aet_last_alpha_put(alpha);
  aet_last_quad_keep(&quad);
  aet_draw_state.quads_drawn = aet_draw_state.quads_drawn + 1;
}

void aet_layer_transform(aet_layer *layer, float frame)
{
  float dx;
  float dy;
  float rx;
  float ry;

  aet_cur_obj_get();
  if (layer != 0) {
    aet_layer_transform(layer->parent,frame);
    aet_layer_tracks_eval(layer,frame);
    dx = layer->pos_x - layer->anchor_x;
    dy = layer->pos_y - layer->anchor_y;
    rx = (float)(int)((dx < 0.0 ? -0.5 : 0.5) + dx);
    ry = (float)(int)((dy < 0.0 ? -0.5 : 0.5) + dy);
    mtx_translate(rx,ry,0.0);
    mtx_translate(layer->anchor_x,layer->anchor_y,0.0);
    mtx_rot_z_s16((int)(((float)(int)layer->rotation * 65536.0) / 360.0 + 0.5) & 0xffff);
    mtx_scale_xyz(layer->scale_x * 0.01,layer->scale_y * 0.01,1.0);
    mtx_translate(-layer->anchor_x,-layer->anchor_y,0.0);
  }
  return;
}

void aet_layer_draw(aet_layer *layer, float frame, float alpha)
{
  aet_obj_rec *obj;
  float item_frame;
  float item_alpha;

  if (((layer != 0) && (obj = aet_cur_obj_get(), obj != 0)) && (layer->item.image != 0)) {
    mtx_push(0);
    aet_layer_transform(layer,frame);
    if (layer->visible != '\0') {
      item_frame = (frame - layer->start_frame) * layer->time_scale + layer->frame_offset;
      item_alpha = alpha * (layer->opacity * 0.01);
      if (layer->type == '\x01') {
        aet_comp_draw(layer->item.comp,item_frame,item_alpha);
      }
      else if (layer->type == '\0') {
        aet_image_layer_draw(layer,layer->item.image,item_frame,item_alpha);
      }
      else if (layer->type == '\x02') {
        aet_bounds_layer_add(layer,layer->item.image,item_alpha);
      }
    }
    mtx_pop(1);
  }
  return;
}

void aet_comp_draw(aet_comp *comp, float frame, float alpha)
{
  int i;

  if (comp != 0) {
    aet_comp_visible_update(comp,frame);
    if (comp->visible != '\0') {
      for (i = comp->layer_count - 1; i >= 0; i--) {
        aet_layer_draw(&comp->layers[i],frame,alpha);
      }
    }
  }
  return;
}

void aet_obj_draw(aet_obj_rec *obj)
{
  if ((obj != 0) && (obj->comp != 0)) {
    mtx_push(0);
    mtx_load_identity();
    mtx_translate(obj->param.x,obj->param.y,0.0);
    mtx_rot_z_s16((int)(((float)(int)obj->param.rotation * 65536.0) / 360.0 + 0.5) & 0xffff);
    if (obj->layer != 0) {
      aet_layer_draw(obj->layer,(float)obj->frame,1.0);
    }
    else {
      aet_comp_draw(obj->comp,(float)obj->frame,1.0);
    }
    mtx_pop(1);
  }
}

void aet_strip_send(aet_quad *quad)
{
  if (quad->alpha != '\0') {
    strip_send_textured_blended(4,(uint *)quad,quad->tex);
  }
  return;
}

void aet_depth_step(void)
{
  aet_obj_rec *obj;
  float *depth;
  float next;

  obj = aet_cur_obj_get();
  depth = &obj->param.depth;
  next = *depth - obj->param.depth_step;
  *depth = next;
  if (next < obj->param.depth_bottom) {
    *depth = obj->param.depth_bottom;
  }
  return;
}

void aet_stills_init(void)
{
  int i;
  aet_still_next_handle = 0;
  memset(aet_stills,0,sizeof(aet_stills));
  memset(aet_still_caches,0,sizeof(aet_still_caches));
  aet_cur_still = 0;
  aet_cur_cache = 0;
  for (i = 0; i < 0x40; i++) {
    aet_still_cache_clear(i);
    aet_still_clear(i);
  }
}

void aet_still_param_init(aet_still_param *param)
{
  if (param != 0) {
    memset(param,0,sizeof(aet_still_param));
    param->scene_id = 0;
    param->tex_table = 0;
    param->field_0x8 = 0;
    param->x = 0.0f;
    param->y = 0.0f;
    param->field_0x14 = 0;
    param->layer_name = 0;
    param->rotation = -1.0f;
    param->scale[0] = 1.0f;
    param->scale[1] = 1.0f;
    param->scale[2] = 1.0f;
    param->depth = 0.15f;
    param->color = 0xffffff;
    param->alpha = 0xff;
    param->filter = 1;
  }
  return;
}

int aet_still_start(aet_still_param *param)
{
  aet_still_rec *still;
  aet_still_cache_rec *cache;
  int i;
  int idx;
  aet_scene *scene;
  aet_comp *comp;

  idx = -1;
  still = aet_stills_table();
  cache = aet_still_caches_table();
  for (i = 0; i < 0x40; i++, still++, cache++) {
    if (still->in_use != 1) {
      idx = i;
      break;
    }
  }
  if (idx == -1) return 0xffffffff;
  aet_still_cache_clear(idx);
  aet_still_clear(idx);
  still->param = *param;
  scene = aet_scene_get(still->param.scene_id);
  if (scene == 0) return 0xffffffff;
  still->scene = scene;
  comp = aet_main_comp_get(scene);
  still->comp = comp;
  if (still->param.layer_name != 0) {
    still->layer = aet_comp_layer_find(comp, still->param.layer_name);
  }
  still->in_use = 1;
  still->handle = aet_still_next_handle;
  cache->in_use = 1;
  cache->handle = aet_still_next_handle;
  if (param->tex_table == 0) {
    aet_still_param_tex_table_set(&still->param, aet_scene_tex_table_get(param->scene_id));
  }
  if ((int)still->param.rotation != -1) {
    still->flags |= 4;
  }
  if (((int)still->param.scale[0] != 1) || ((int)still->param.scale[1] != 1)) {
    still->flags |= 8;
  }
  aet_still_next_handle = aet_still_next_handle + 1;
  if (aet_still_next_handle == -1) {
    aet_still_next_handle = 0;
  }
  return still->handle;
}

void aet_still_clear(int index)
{
  aet_still_rec *still;

  if (index != -1) {
    still = aet_stills_table() + index;
    memset(still,0,sizeof(aet_still_rec));
    still->in_use = 0;
    still->handle = 0xffffffff;
    still->layer = 0;
    still->scene = 0;
    still->comp = 0;
    still->flags = still->flags | 1;
    memset(&still->param,0,sizeof(aet_still_param));
    still->param.rotation = -1.0;
    still->last_quad = 0xffffffff;
    still->quad_slot = 0xffffffff;
    still->param.color = 0xffffff;
    still->param.alpha = 0xff;
  }
  return;
}

void aet_still_cache_clear(int index)
{
  aet_still_cache_rec *cache;

  if (index != -1) {
    cache = aet_still_caches_table() + index;
    memset(cache,0,sizeof(aet_still_cache_rec));
    cache->in_use = 0;
  }
  return;
}

void aet_still_param_scene_set(aet_still_param *param, uint scene_id)
{
  if (param != 0) {
    param->scene_id = scene_id;
  }
  return;
}

void aet_stills_draw(void)
{
  aet_still_rec *still;
  aet_still_cache_rec *cache;
  int i;

  hdr_or_fog(2);
  still = aet_stills_table();
  cache = aet_still_caches_table();
  for (i = 0; i < 0x40; i++, still++, cache++) {
    if ((still->in_use != '\0') && ((still->flags & 0x40) == 0)) {
      aet_cur_still_set(still);
      aet_cur_cache_set(cache);
      if (still->param.filter != '\0') {
        hdr_or_filter(1);
      }
      if ((still->flags & 1) != 0) {
        aet_still_build(still);
      }
      else if (cache->in_use != '\0') {
        aet_still_update_draw(still);
      }
      if (still->param.filter != '\0') {
        hdr_or_filter(0);
      }
    }
  }
  aet_blend_select(-1);
  hdr_or_fog(0);
}

aet_still_rec * aet_still_get(int handle)
{
  aet_still_rec *still;
  unsigned char i;
  int found;

  found = -1;
  still = aet_stills_table();
  for (i = 0; i < 0x40; i++, still++) {
    if (still->handle == handle) {
      found = i;
      break;
    }
  }
  if (found == -1) return 0;
  return &aet_stills[found];
}

aet_still_rec * aet_stills_table(void)
{
  return aet_stills;
}

void aet_cur_still_set(aet_still_rec *still)
{
  aet_cur_still = still;
  return;
}

aet_still_rec * aet_cur_still_get(void)
{
  return aet_cur_still;
}

aet_still_cache_rec * aet_still_cache_get(int handle)
{
  int idx = -1;
  aet_still_cache_rec *cache;
  uchar i;

  cache = aet_still_caches_table();
  for (i = 0; i < 0x40; i++, cache++) {
    if (cache->handle == handle) {
      idx = i;
      break;
    }
  }
  if (idx == -1) return 0;
  return &aet_still_caches[idx];
}

aet_still_cache_rec * aet_still_caches_table(void)
{
  return aet_still_caches;
}

void aet_cur_cache_set(aet_still_cache_rec *cache)
{
  aet_cur_cache = cache;
  return;
}

aet_still_cache_rec * aet_cur_cache_get(void)
{
  return aet_cur_cache;
}

int aet_still_cache_index(int handle)
{
  aet_still_cache_rec *cache;
  int i;

  cache = aet_still_caches_table();
  i = 0;
  do {
    if (cache->handle == handle) {
      return i;
    }
    i = i + 1;
    cache = cache + 1;
  } while (i < 0x40);
  return -1;
}

void aet_still_build(aet_still_rec *still)
{
  aet_layer *layer;
  aet_comp *comp;

  if ((still != 0) && (still->comp != 0)) {
    mtx_push(0);
    mtx_load_identity();
    mtx_translate(still->param.x,still->param.y,0.0);
    layer = still->layer;
    if ((layer != 0) && (layer->type == '\x01')) {
      comp = layer->item.comp;
      still->quad_slot = comp->layer_count + -1;
      still->last_quad = comp->layer_count + -1;
      still->flags = still->flags | 0x20;
    }
    if (still->layer != 0)
      aet_still_layer_draw(still->layer);
    else
      aet_still_comp_draw(still->comp);
    mtx_pop(1);
  }
  return;
}

void aet_still_layer_draw(aet_layer *layer)
{
  aet_still_rec *still;
  float alpha;

  alpha = 1.0;
  if (((layer != 0) && (still = aet_cur_still_get(), still != 0)) && (layer->item.image != 0)) {
    mtx_push(0);
    if ((still->flags & 0x20) != 0) {
      aet_layer_transform(layer,0.0);
      alpha = layer->opacity * 0.01;
    }
    if (layer->type == '\x01') {
      aet_still_comp_draw(layer->item.comp);
    }
    else if (layer->type == '\0') {
      aet_still_image_draw(layer,layer->item.image,alpha);
    }
    mtx_pop(1);
  }
  return;
}

void aet_still_comp_draw(aet_comp *comp)
{
  aet_still_rec *still;
  int i;

  still = aet_cur_still_get();
  if (still == 0) return;
  if (comp == 0) return;
  aet_comp_visible_update(comp,0.0);
  for (i = still->last_quad; 0 <= i; i--) {
    aet_still_layer_draw(&comp->layers[i]);
  }
}

void aet_still_image_draw(aet_layer *layer, aet_image *image, float alpha)
{
  aet_quad quad;
  aet_still_rec *still;
  aet_picture *picture;
  int bad;
  char blend;

  bad = (layer == 0) | (image == 0);
  if (bad) return;
  still = aet_cur_still_get();
  if (still == 0) return;
  aet_image_picture_set(image, 0.0);
  picture = image->pictures;
  if (picture == 0) return;
  picture = &picture[image->picture];
  if (picture == 0) return;
  if (picture->tex_index < 0) return;
  quad.tex = still->param.tex_table[picture->tex_index];
  quad.field_0x7c = still->param.field_0x14;
  quad.z = still->param.depth;
  quad.color = 0xffffff;
  if (still->param.color != 0xffffff) {
    quad.color = still->param.color;
  }
  quad.alpha = (int)(alpha * 255.0);
  if (still->param.alpha != 0xff) {
    quad.alpha = still->param.alpha;
  }
  aet_quad_build(&quad, picture, (float)image->width, (float)image->height, alpha);
  aet_still_quad_cache_store(&quad, still);
  still->flags &= ~1;
  if (layer->no_send == 0) {
    if (aet_draw_state.blend_override != 0) blend = -1;
    else blend = layer->blend;
    aet_blend_select(blend);
    aet_strip_send(&quad);
    aet_still_depth_step();
  }
  aet_blend_select(-1);
}

void aet_still_param_tex_table_set(aet_still_param *param, int *tex_table)
{
  if (param != 0) {
    param->tex_table = tex_table;
  }
  return;
}

void aet_still_param_pos_set(aet_still_param *param, float x, float y)
{
  if (param != 0) {
    param->x = x;
    param->y = y;
  }
  return;
}

void aet_still_param_rotation_set(aet_still_param *param, float rotation)
{
  if (param != 0) {
    param->rotation = rotation;
  }
  return;
}

void aet_still_param_scale_set(aet_still_param *param, int scale_x_bits, int scale_y_bits)
{
  union { int i; float f; } scale_x, scale_y;

  scale_x.i = scale_x_bits;
  scale_y.i = scale_y_bits;
  if (param != 0) {
    param->scale[0] = scale_x.f;
    param->scale[1] = scale_y.f;
  }
  return;
}

void aet_still_param_depth_range_set(aet_still_param *param, float top, float bottom)
{
  aet_still_param *p = param;
  float *level;
  uchar i;

  if (p != 0) {
    if (__builtin_fabsf(top - bottom) < 1.1920929e-07) {
      p->depth_bottom = p->depth = bottom + 0.05;
      for (i = 0; i < 13; i++) {
        level = aet_depth_levels + i;
        if (__builtin_fabsf(level[0] - top) < 1.1920929e-07) {
          p->depth_top = level[1] + 0.05;
          p->depth_step = (p->depth_top - p->depth_bottom) / 1000.0;
          return;
        }
      }
    }
    else {
      p->depth_bottom = p->depth = bottom + 0.05;
      p->depth_top = top + 0.05;
      p->depth_step = (p->depth_top - p->depth_bottom) / 1000.0;
    }
  }
}

void aet_still_param_color_set(aet_still_param *param, uint color)
{
  if (param != 0) {
    param->color = color;
  }
  return;
}

void aet_still_param_filter_set(aet_still_param *param, byte filter)
{
  if (param != 0) {
    param->filter = filter;
  }
  return;
}

void aet_still_param_alpha_set(aet_still_param *param, float alpha)
{
  if (param != 0) {
    param->alpha = (int)(alpha * 255.0);
  }
  return;
}

void aet_still_move_to(float x, float y, int handle)
{
  aet_still_rec *still;
  still = aet_still_get(handle);
  if (still != 0) {
    still->param.move_x = x - still->param.x;
    still->param.move_y = y - still->param.y;
    still->param.x += still->param.move_x;
    still->param.y += still->param.move_y;
    still->flags |= 2;
  }
}

void aet_still_rotation_set(int handle, float rotation)
{
  aet_still_rec *still;

  still = aet_still_get(handle);
  if ((still != 0) && ((int)still->param.rotation != (int)rotation)) {
    still->param.rotation = rotation;
    still->flags = still->flags | 4;
  }
  return;
}

void aet_still_scale_set(int handle, int scale_x_bits, int scale_y_bits)
{
  aet_still_rec *still;
  float k;
  union { int i; float f; } scale_x, scale_y;

  scale_x.i = scale_x_bits;
  scale_y.i = scale_y_bits;
  k = 100.0f;
  still = aet_still_get(handle);
  if ((still != 0) &&
     (((int)(still->param.scale[0] * k) != (int)(scale_x.f * k) ||
      ((int)(still->param.scale[1] * k) != (int)(scale_y.f * k))))) {
    still->param.scale[0] = scale_x.f;
    still->param.scale[1] = scale_y.f;
    still->flags = still->flags | 8;
  }
  return;
}

void aet_still_depth_range_set(int handle, float top, float bottom)
{
  aet_still_rec *still;
  still = aet_still_get(handle);
  if (still != 0) {
    still->param.depth_top = top + 0.05;
    still->param.depth_bottom = still->param.depth = bottom + 0.05;
    still->param.depth_step = ((top + 0.05) - (bottom + 0.05)) / 1000.0;
  }
}

void aet_still_color_set(int handle, uint color)
{
  aet_still_rec *still;
  aet_still_cache_rec *cache;

  still = aet_still_get(handle);
  if ((still != 0) && (cache = aet_still_cache_get(handle), cache->in_use != '\0')) {
    still->param.color = cache->quads[0].color << 0x18 | color;
    still->flags = still->flags | 0x10;
  }
  return;
}

void aet_still_hide_set(int handle, unsigned char hide)
{
  aet_still_rec *still;

  still = aet_still_get(handle);
  if (still != 0) {
    if (hide) {
      still->flags = still->flags | 0x40;
    }
    else {
      still->flags = still->flags & 0xffffffbf;
    }
  }
}

void aet_still_param_layer_set(aet_still_param *param, char *layer_name)
{
  if (param != 0) {
    param->layer_name = layer_name;
  }
  return;
}

void aet_still_quad_cache_store(aet_quad *quad, aet_still_rec *still)
{
  aet_still_cache_rec *cache;
  uchar i;
  if (still->layer != 0)
  {
    cache = aet_cur_cache_get();
    if (((still->flags & 0x20) != 0) && (still->quad_slot >= 0))
    {
      for (i = 0; i < 4; i++)
      {
        cache->corners[still->quad_slot][i].x = (cache->quads[still->quad_slot].v[i].x = quad->v[i].x);
        cache->corners[still->quad_slot][i].y = (cache->quads[still->quad_slot].v[i].y = quad->v[i].y);
        cache->corners[still->quad_slot][i].z = (cache->quads[still->quad_slot].v[i].z = quad->v[i].z);
        cache->quads[still->quad_slot].v[i].u = quad->v[i].u;
        cache->quads[still->quad_slot].v[i].v = quad->v[i].v;
        cache->quads[still->quad_slot].v[i].color = quad->v[i].color;
        cache->quads[still->quad_slot].v[i].field_0x18 = quad->v[i].field_0x18;
      }

      cache->quads[still->quad_slot].tex = quad->tex;
      cache->quads[still->quad_slot].alpha = -1;
      cache->quads[still->quad_slot].z = quad->z;
      still->quad_slot = still->quad_slot - 1;
    }
    else
    {
      for (i = 0; i < 4; i++)
      {
        cache->corners[0][i].x = (cache->quads[0].v[i].x = quad->v[i].x);
        cache->corners[0][i].y = (cache->quads[0].v[i].y = quad->v[i].y);
        cache->corners[0][i].z = (cache->quads[0].v[i].z = quad->v[i].z);
        cache->quads[0].v[i].u = quad->v[i].u;
        cache->quads[0].v[i].v = quad->v[i].v;
        cache->quads[0].v[i].color = quad->v[i].color;
        cache->quads[0].v[i].field_0x18 = quad->v[i].field_0x18;
      }

      if (still)
      {
        cache->quads[0].tex = quad->tex;
        cache->quads[0].alpha = -1;
        cache->quads[0].z = quad->z;
      }
      else
      {
        cache->quads[0].tex = quad->tex;
        cache->quads[0].alpha = -1;
        cache->quads[0].z = quad->z;
      }
    }
  }
}

void aet_still_rotate_matrix(aet_still_rec *still)
{
  mtx_translate(still->param.x,still->param.y,0.0);
  mtx_rot_z_s16((int)(((float)(int)still->param.rotation * 65536.0) / 360.0 + 0.5) & 0xffff);
  still->flags = still->flags & 0xfffffffb;
  return;
}

void aet_still_move_apply(aet_still_rec *still)
{
  aet_still_cache_rec *cache;
  unsigned char j;
  aet_layer *layer;
  int i;

  layer = still->layer;
  cache = aet_cur_cache_get();
  if ((layer != 0) && (cache->in_use != '\0')) {
    if (layer->type == '\x01') {
      for (i = still->last_quad; i >= 0; i--) {
        for (j = 0; j < 4; j++) {
          cache->quads[i].v[j].x = cache->quads[i].v[j].x + still->param.move_x;
          cache->quads[i].v[j].y = cache->quads[i].v[j].y + still->param.move_y;
        }
      }
    }
    else {
      for (j = 0; j < 4; j++) {
        cache->quads[0].v[j].x = cache->quads[0].v[j].x + still->param.move_x;
        cache->quads[0].v[j].y = cache->quads[0].v[j].y + still->param.move_y;
      }
    }
    still->flags = still->flags & 0xfffffffd;
  }
}

void aet_still_scale_matrix(aet_still_rec *still, int quad_index)
{
  aet_still_cache_rec *cache;
  aet_layer *layer;
  uchar j;
  int i;
  float a[4][3];
  float b[4][3];

  cache = aet_cur_cache_get();
  layer = still->layer;
  if (layer != 0 && cache->in_use != '\0') {
    for (i = 0; i < 10; i++) {
      for (j = 0; j < 4; j++) {
        a[j][0] = a[j][1] = a[j][2] = 0.0;
        b[j][0] = b[j][1] = b[j][2] = 0.0;
      }
    }
    for (j = 0; j < 4; j++) {
      a[j][0] = cache->corners[quad_index][j].x;
      a[j][1] = cache->corners[quad_index][j].y;
      a[j][2] = 0.0;
    }
    mtx_scale_by_vec3(still->param.scale);
    still->flags = still->flags & 0xfffffff7;
  }
}

void aet_still_quad_retransform(aet_still_rec *still, int quad_index)
{
  float d[2];
  float v[4][3];
  float o[4][3];
  float c[4][3];
  float s1[3];
  float a1[3];
  float e[4][3];
  float s2[3];
  float a2[3];
  aet_still_cache_rec *cache;
  aet_image *image;
  uchar i;

  cache = aet_cur_cache_get();
  image = still->layer->item.image;
  if (image != 0 && cache->in_use != '\0') {
    s1[0] = s1[1] = s1[2] = 0.0f;
    a1[0] = a1[1] = a1[2] = 0.0f;
    s2[0] = s2[1] = s2[2] = 0.0f;
    a2[0] = a2[1] = a2[2] = 0.0f;
    for (i = 0; i < 4; i++) {
      c[i][0] = c[i][1] = c[i][2] = 0.0f;
      e[i][0] = e[i][1] = e[i][2] = 0.0f;
    }
    v[0][0] = v[2][0] = 0.0f;
    v[0][1] = v[1][1] = 0.0f;
    v[1][0] = v[3][0] = (float)image->width;
    v[2][1] = v[3][1] = (float)image->height;
    v[0][2] = v[1][2] = v[2][2] = v[3][2] = 0.0f;
    for (i = 0; i < 4; i++) {
      mtx_xform_point(v[i], o[i]);
      cache->quads[quad_index].v[i].x = o[i][0];
      cache->quads[quad_index].v[i].y = o[i][1];
      cache->quads[quad_index].v[i].z = cache->quads[quad_index].z;
    }
    for (i = 0; i < 4; i++) {
      c[i][0] = cache->corners[quad_index][i].x;
      c[i][1] = cache->corners[quad_index][i].y;
      c[i][2] = 0.0f;
      e[i][0] = cache->quads[quad_index].v[i].x;
      e[i][1] = cache->quads[quad_index].v[i].y;
      e[i][2] = 0.0f;
    }
    for (i = 0; i < 4; i++) {
      vec3_add_to(a1, c[i]);
      vec3_add_to(a2, e[i]);
    }
    vec3_scale(s1, a1, 0.25);
    vec3_scale(s2, a2, 0.25);
    d[0] = s1[0] - s2[0];
    d[1] = s1[1] - s2[1];
    for (i = 0; i < 4; i++) {
      cache->quads[quad_index].v[i].x += d[0];
      cache->quads[quad_index].v[i].y += d[1];
    }
  }
}

void aet_still_color_apply(aet_still_rec *still, int quad_index)
{
  aet_still_cache_rec *cache;
  byte i;
  cache = aet_cur_cache_get();
  if (cache->in_use != '\0') {
    for (i = 0; i < 4; i++) {
      cache->quads[quad_index].v[i].color = still->param.alpha << 0x18 | still->param.color;
    }
    still->flags = still->flags & 0xffffffef;
  }
}

void aet_still_update_draw(aet_still_rec *still)
{
  int last;
  int i;
  aet_still_cache_rec *cache;
  aet_layer *layer;
  int last_quad;

  layer = still->layer;
  if (layer == 0) return;
  cache = aet_cur_cache_get();
  if (cache->in_use == '\0') return;
  if (layer->type == '\x01') {
    last = still->last_quad;
  }
  else {
    last = 0;
  }
  if ((still->flags & 2) != 0) {
    aet_still_move_apply(still);
  }
  if ((still->flags & 4) != 0) {
    for (i = last; i >= 0; i--) {
      mtx_push(0);
      mtx_load_identity();
      aet_still_rotate_matrix(still);
      aet_still_quad_retransform(still,i);
      mtx_pop(1);
    }
  }
  if ((still->flags & 8) != 0) {
    for (i = last; i >= 0; i--) {
      mtx_push(0);
      mtx_load_identity();
      aet_still_scale_matrix(still,i);
      aet_still_quad_retransform(still,i);
      mtx_pop(1);
    }
  }
  if ((still->flags & 0x10) != 0) {
    for (i = last; i >= 0; i--) {
      aet_still_color_apply(still,i);
    }
  }
  if ((layer->type == '\x01') && (last_quad = still->last_quad, last_quad != 0)) {
    for (i = last_quad; i >= 0; i--) {
      aet_strip_send(&cache->quads[i]);
    }
  }
  else {
    aet_strip_send(&cache->quads[0]);
  }
}

int aet_still_index(int handle)
{
  aet_still_rec *still;
  int i;

  still = aet_stills_table();
  i = 0;
  do {
    if (still->handle == handle) {
      return i;
    }
    i = i + 1;
    still = still + 1;
  } while (i < 0x40);
  return -1;
}

int aet_still_stop(int handle)
{
  aet_still_rec *still;
  still = aet_still_get(handle);
  if (still == 0) return 0;
  if (still->in_use == '\0') return 1;
  aet_still_cache_clear(aet_still_cache_index(handle));
  aet_still_clear(aet_still_index(handle));
  return 1;
}

int aet_stills_stop_by_scene(int scene_id)
{
  aet_still_rec *still;
  int i;
  int n;

  still = aet_stills_table();
  n = 0;
  i = 0;
  do {
    if ((still->in_use != '\0') && (still->param.scene_id == scene_id)) {
      aet_still_cache_clear(i);
      aet_still_clear(i);
      n = n + 1;
    }
    i = i + 1;
    still = still + 1;
  } while (i < 0x40);
  return n;
}

void aet_still_depth_step(void)
{
  aet_still_rec *still;
  float *depth;
  float next;

  still = aet_cur_still_get();
  depth = &still->param.depth;
  next = *depth - still->param.depth_step;
  *depth = next;
  if (next < still->param.depth_bottom) {
    *depth = still->param.depth_bottom;
  }
  return;
}

void aet_blend_pair_put(uint src_factor, uint dst_factor)
{
  blend_pair_bits_put_inverted(src_factor,dst_factor);
  return;
}

void aet_blend_normal(void)
{
  aet_blend_pair_put(4,5);
  return;
}

void aet_blend_additive(void)
{
  aet_blend_pair_put(4,1);
  return;
}
