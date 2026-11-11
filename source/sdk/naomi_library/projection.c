/*
 * sdk/naomi_library/projection.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint clip_stretch_step; /* 0x0c32a590 */
extern float proj_x_factor; /* 0x0c32a6e8 */
extern float proj_y_factor; /* 0x0c32a6ec */
extern float proj_factor_px_max; /* 0x0c32a6f0 */
extern float proj_pending_offset_x; /* 0x0c32a6f4 */
extern float proj_pending_offset_y; /* 0x0c32a6f8 */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_height; /* 0x0c32a700 */
extern float proj_rect_px_left; /* 0x0c32a704 */
extern float proj_rect_px_top; /* 0x0c32a708 */
extern int frame_pixel_format; /* 0x0c417ac8 */
extern float proj_rect_width; /* 0x0c41c950 */
extern float rect_x_proj_work[16]; /* 0x0c41c958 */
extern float proj_rect_left; /* 0x0c41c998 */
extern float rect_x_proj_matrix[16]; /* 0x0c41c9a0 */
extern float view_x_model_matrix[16]; /* 0x0c41c9e0 */
extern float proj_rect_height; /* 0x0c41ca20 */
extern float proj_rect_top; /* 0x0c41ca24 */
extern float proj_x_factor_px; /* 0x0c41ca68 */
extern float rect_from_clip_matrix[16]; /* 0x0c41ca6c */
extern float proj_y_factor_px; /* 0x0c41caf0 */
extern obj_send_header_rec obj_send_header; /* 0x0c417d18 */
extern display_state_rec display_state; /* 0x0c41bb38 */
extern mtx_stack_record_set mtx_stack_records; /* 0x0c41c860 */
extern view_side_planes_rec view_side_planes; /* 0x0c41cab0 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */

void proj_from_near_rect(float left, float right, float bottom, float top, float near_z, float far_z)
{
  float m[16];
  m[0] = (near_z + near_z) / (right - left);
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = (near_z + near_z) / (top - bottom);
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = (right + left) / (right - left);
  m[9] = (top + bottom) / (top - bottom);
  m[10] = -(far_z + near_z) / (far_z - near_z);
  m[11] = -1.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = (far_z * -2.0 * near_z) / (far_z - near_z);
  m[15] = 0.0;
  graphics_work.near_z = near_z;
  graphics_work.far_z = far_z;
  graphics_work.near_inv = 1.0 / near_z;
  obj_send_header.near_z = near_z;
  obj_send_header.near_limit = -near_z;
  obj_send_header.far_z = far_z;
  obj_send_header.near_inv = graphics_work.near_inv;
  mtx_mul_by(m);
  proj_rebuild_rect_proj();
}

void proj_from_near_rect_zflip(float left, float right, float bottom, float top, float near_z, float far_z)
{
  float m [16];
  float inv;

  m[0] = (near_z + near_z) / (right - left);
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = (near_z + near_z) / (top - bottom);
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = (right + left) / (right - left);
  m[9] = (top + bottom) / (top - bottom);
  m[10] = -(far_z + near_z) / (far_z - near_z);
  m[11] = 1.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = ((far_z + far_z) * near_z) / (far_z - near_z);
  m[15] = 0.0;
  graphics_work.near_z = near_z;
  graphics_work.far_z = far_z;
  graphics_work.near_inv = inv = 1.0 / near_z;
  obj_send_header.near_z = near_z;
  obj_send_header.near_limit = -near_z;
  obj_send_header.far_z = far_z;
  obj_send_header.near_inv = inv;
  mtx_mul_by(m);
  proj_rebuild_rect_proj();
}

void mtx_save_proj_x_current(float unused_a, float unused_b)
{
  mtx_push(mtx_stack_records.proj.top);
  mtx_times_last_pushed();
  mtx_store(proj_x_model_matrix);
  mtx_pop(1);
  return;
}

void mtx_save_proj_view_x_current(void)
{
  mtx_push(mtx_stack_records.proj.top);
  mtx_mul_by(mtx_stack_records.view.top);
  mtx_times_last_pushed();
  mtx_store(proj_x_model_matrix);
  mtx_pop(1);
}

void mtx_save_view_x_current(void)
{
  mtx_push(mtx_stack_records.view.top);
  mtx_times_last_pushed();
  mtx_store_wordwise(view_x_model_matrix);
  mtx_pop(1);
  return;
}

void proj_offset_xy(float x, float y)
{
  proj_pending_offset_x = __builtin_fabsf(x);
  proj_pending_offset_y = __builtin_fabsf(y);
  mtx_translate(x,y,0.0);
}

void proj_from_vfov(int fov, float aspect, float near_z, float far_z)
{
  float m[16];
  float scale;
  float x_secant;
  float y_secant;
  float edge_angle;
  struct { float m[16]; } *planes;
  int half_fov;
  scale = 1.0 / tan_ang16(fov >> 1);
  if (proj_pending_offset_x != 0.0) {
    x_secant = 1.0 / cos_ang16(atan_ang16(tan_ang16(atan_ang16(aspect / scale)) * (proj_pending_offset_x + 1.0)));
  }
  else {
    x_secant = 1.0 / cos_ang16(atan_ang16(aspect / scale));
  }
  if (proj_pending_offset_y != 0.0) {
    y_secant = 1.0 / cos_ang16(atan_ang16(tan_ang16(fov >> 1) * (proj_pending_offset_y + 1.0)));
  }
  else {
    y_secant = 1.0 / cos_ang16(fov >> 1);
  }
  proj_pending_offset_x = 0.0;
  proj_pending_offset_y = 0.0;
  proj_x_factor = scale / aspect * 0.5 * x_secant;
  proj_y_factor = scale * 0.5 * y_secant;
  proj_x_factor_px = proj_x_factor * proj_rect_px_width;
  proj_y_factor_px = proj_y_factor * proj_rect_px_height;
  if (proj_x_factor_px > proj_y_factor_px) {
    proj_factor_px_max = proj_x_factor_px;
  }
  else {
    proj_factor_px_max = proj_y_factor_px;
  }
  half_fov = fov >> 1;
  edge_angle = (int)atan_ang16(tan_ang16(half_fov) * aspect);
  planes = (void *)&view_side_planes;
  planes->m[1] = planes->m[5] = 0.0;
  planes->m[8] = planes->m[12] = 0.0;
  planes->m[0] = cos_ang16((int)edge_angle);
  planes->m[4] = -planes->m[0];
  planes->m[2] = planes->m[6] = sin_ang16((int)edge_angle);
  planes->m[9] = cos_ang16(half_fov);
  planes->m[13] = -planes->m[9];
  planes->m[10] = planes->m[14] = sin_ang16(half_fov);
  planes->m[3] = planes->m[7] = 0.0;
  planes->m[11] = planes->m[15] = 0.0;
  m[0] = scale / aspect;
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = scale;
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = 0.0;
  m[9] = 0.0;
  m[10] = (far_z + near_z) / (near_z - far_z);
  m[11] = -1.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = ((far_z + far_z) * near_z) / (near_z - far_z);
  m[15] = 0.0;
  graphics_work.near_z = near_z;
  graphics_work.far_z = far_z;
  graphics_work.near_inv = 1.0 / near_z;
  obj_send_header.near_z = near_z;
  obj_send_header.near_limit = -near_z;
  obj_send_header.far_z = far_z;
  obj_send_header.near_inv = graphics_work.near_inv;
  mtx_mul_by(m);
  proj_rebuild_rect_proj();
}

void proj_from_hfov(int fov, float aspect, float near_z, float far_z)
{
  float m[16];
  float scale;
  float x_secant;
  float y_secant;
  float edge_angle;
  struct { float m[16]; } *planes;
  int half_fov;
  scale = 1.0 / tan_ang16(fov >> 1);
  if (proj_pending_offset_x != 0.0) {
    x_secant = 1.0 / cos_ang16(atan_ang16(tan_ang16(fov >> 1) * (proj_pending_offset_x + 1.0)));
  }
  else {
    x_secant = 1.0 / cos_ang16(fov >> 1);
  }
  if (proj_pending_offset_y != 0.0) {
    y_secant = 1.0 / cos_ang16(atan_ang16(tan_ang16(atan_ang16(1.0 / (aspect * scale))) * (proj_pending_offset_y + 1.0)));
  }
  else {
    y_secant = 1.0 / cos_ang16(atan_ang16(1.0 / (aspect * scale)));
  }
  proj_pending_offset_x = 0.0;
  proj_pending_offset_y = 0.0;
  proj_x_factor = scale * 0.5 * x_secant;
  proj_y_factor = aspect * scale * 0.5 * y_secant;
  proj_x_factor_px = proj_x_factor * proj_rect_px_width;
  proj_y_factor_px = proj_y_factor * proj_rect_px_height;
  if (proj_x_factor_px > proj_y_factor_px) {
    proj_factor_px_max = proj_x_factor_px;
  }
  else {
    proj_factor_px_max = proj_y_factor_px;
  }
  half_fov = fov >> 1;
  edge_angle = (int)atan_ang16(tan_ang16(half_fov) / aspect);
  planes = (void *)&view_side_planes;
  planes->m[1] = planes->m[5] = 0.0;
  planes->m[8] = planes->m[12] = 0.0;
  planes->m[0] = cos_ang16(half_fov);
  planes->m[4] = -planes->m[0];
  planes->m[2] = planes->m[6] = sin_ang16(half_fov);
  planes->m[9] = cos_ang16((int)edge_angle);
  planes->m[13] = -planes->m[9];
  planes->m[10] = planes->m[14] = sin_ang16((int)edge_angle);
  planes->m[3] = planes->m[7] = 0.0;
  planes->m[11] = planes->m[15] = 0.0;
  m[0] = scale;
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = aspect * scale;
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = 0.0;
  m[9] = 0.0;
  m[10] = (far_z + near_z) / (near_z - far_z);
  m[11] = -1.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = ((far_z + far_z) * near_z) / (near_z - far_z);
  m[15] = 0.0;
  graphics_work.near_z = near_z;
  graphics_work.far_z = far_z;
  graphics_work.near_inv = 1.0 / near_z;
  obj_send_header.near_z = near_z;
  obj_send_header.near_limit = -near_z;
  obj_send_header.far_z = far_z;
  obj_send_header.near_inv = graphics_work.near_inv;
  mtx_mul_by(m);
  proj_rebuild_rect_proj();
}

void proj_from_vfov_zflip(int fov, float aspect, float near_z, float far_z)
{
  float m[16];
  float scale;
  float x_secant;
  float y_secant;
  uint angle;
  scale = 1.0 / tan_ang16(fov >> 1);
  if (proj_pending_offset_x != 0.0) {
    angle = atan_ang16(aspect / scale);
    angle = atan_ang16(tan_ang16(angle) * (proj_pending_offset_x + 1.0));
    x_secant = 1.0 / cos_ang16(angle);
  }
  else {
    angle = atan_ang16(aspect / scale);
    x_secant = 1.0 / cos_ang16(angle);
  }
  if (proj_pending_offset_y != 0.0) {
    y_secant = 1.0 / cos_ang16(atan_ang16(tan_ang16(fov >> 1) * (proj_pending_offset_y + 1.0)));
  }
  else {
    y_secant = 1.0 / cos_ang16(fov >> 1);
  }
  proj_pending_offset_x = 0.0;
  proj_pending_offset_y = 0.0;
  proj_x_factor = scale / aspect * 0.5 * x_secant;
  proj_y_factor = scale * 0.5 * y_secant;
  proj_x_factor_px = proj_x_factor * proj_rect_px_width;
  proj_y_factor_px = proj_y_factor * proj_rect_px_height;
  if (proj_x_factor_px > proj_y_factor_px) proj_factor_px_max = proj_x_factor_px;
  else proj_factor_px_max = proj_y_factor_px;
  m[0] = scale / aspect;
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = scale;
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = 0.0;
  m[9] = 0.0;
  m[10] = (far_z + near_z) / (near_z - far_z);
  m[11] = 1.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = (-2.0 * far_z * near_z) / (near_z - far_z);
  m[15] = 0.0;
  graphics_work.near_z = near_z;
  graphics_work.far_z = far_z;
  graphics_work.near_inv = 1.0 / near_z;
  obj_send_header.near_z = near_z;
  obj_send_header.near_limit = -near_z;
  obj_send_header.far_z = far_z;
  obj_send_header.near_inv = 1.0 / near_z;
  mtx_mul_by(m);
  proj_rebuild_rect_proj();
}

void proj_from_hfov_zflip(int fov, float aspect, float near_z, float far_z)
{
  float m[16];
  float scale;
  float x_secant;
  float y_secant;
  uint angle;
  scale = 1.0 / tan_ang16(fov >> 1);
  if (proj_pending_offset_x != 0.0) {
    x_secant = 1.0 / cos_ang16(atan_ang16(tan_ang16(fov >> 1) * (proj_pending_offset_x + 1.0)));
  }
  else {
    x_secant = 1.0 / cos_ang16(fov >> 1);
  }
  if (proj_pending_offset_y != 0.0) {
    angle = atan_ang16(1.0 / (aspect * scale));
    angle = atan_ang16(tan_ang16(angle) * (proj_pending_offset_y + 1.0));
    y_secant = 1.0 / cos_ang16(angle);
  }
  else {
    angle = atan_ang16(1.0 / (aspect * scale));
    y_secant = 1.0 / cos_ang16(angle);
  }
  proj_pending_offset_x = 0.0;
  proj_pending_offset_y = 0.0;
  proj_x_factor = scale * 0.5 * x_secant;
  proj_y_factor = aspect * scale * 0.5 * y_secant;
  proj_x_factor_px = proj_x_factor * proj_rect_px_width;
  proj_y_factor_px = proj_y_factor * proj_rect_px_height;
  if (proj_x_factor_px > proj_y_factor_px) proj_factor_px_max = proj_x_factor_px;
  else proj_factor_px_max = proj_y_factor_px;
  m[0] = scale;
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = aspect * scale;
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = 0.0;
  m[9] = 0.0;
  m[10] = (far_z + near_z) / (near_z - far_z);
  m[11] = 1.0;
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = (-2.0 * far_z * near_z) / (near_z - far_z);
  m[15] = 0.0;
  graphics_work.near_z = near_z;
  graphics_work.far_z = far_z;
  graphics_work.near_inv = 1.0 / near_z;
  obj_send_header.near_z = near_z;
  obj_send_header.near_limit = -near_z;
  obj_send_header.far_z = far_z;
  obj_send_header.near_inv = 1.0 / near_z;
  mtx_mul_by(m);
  proj_rebuild_rect_proj();
}

void proj_point_to_pixel_xy(float *point, float *out)
{
  float v[4];
  float inv_w;
  mtx_push(proj_x_model_matrix);
  mtx_xform_point_xyzw(point,v);
  inv_w = 1.0 / v[3];
  v[0] = inv_w * v[0];
  v[1] = inv_w * v[1];
  *out = (v[0] + 1.0) * proj_rect_px_width * 0.5 + proj_rect_px_left;
  out[1] = (1.0 - v[1]) * proj_rect_px_height * 0.5 + proj_rect_px_top;
  mtx_pop(1);
  return;
}

void mtx_xform_point_by_saved_view(float *point, float *out)
{
  mtx_push(view_x_model_matrix);
  mtx_xform_point(point,out);
  mtx_pop(1);
  return;
}

void proj_build_rect_matrix(float *m)
{
  *m = proj_rect_px_width * 0.5;
  m[1] = 0.0;
  m[2] = 0.0;
  m[3] = 0.0;
  m[4] = 0.0;
  m[5] = -proj_rect_px_height * 0.5;
  m[6] = 0.0;
  m[7] = 0.0;
  m[8] = 0.0;
  m[9] = 0.0;
  m[10] = -0.5;
  m[0xb] = 0.0;
  m[0xc] = proj_rect_px_width * 0.5 + proj_rect_px_left;
  m[0xd] = proj_rect_px_height * 0.5 + proj_rect_px_top;
  m[0xe] = 0.5;
  m[0xf] = 1.0;
  return;
}

void proj_map_to_rect(float left, float top, float width, float height)
{
  float px_top;
  float px_height;
  
  px_top = top;
  px_height = height;
  if ((display_state.anti_alias != 0) && (display_state.screen_height * 2 <= 0x1e0)) {
    px_top = top + top;
    px_height = height + height;
  }
  proj_rect_left = left;
  proj_rect_top = top;
  proj_rect_width = width;
  proj_rect_height = height;
  proj_rect_px_top = px_top;
  proj_rect_px_height = px_height;
  if (display_state.anti_alias != 0) {
    proj_rect_px_left = left + left;
    proj_rect_px_width = width + width;
  }
  else {
    proj_rect_px_left = left;
    proj_rect_px_width = width;
  }
  proj_x_factor_px = proj_x_factor * proj_rect_px_width;
  proj_y_factor_px = proj_y_factor * proj_rect_px_height;
  if (proj_x_factor_px > proj_y_factor_px) {
    proj_factor_px_max = proj_x_factor_px;
  }
  else {
    proj_factor_px_max = proj_y_factor_px;
  }
  proj_build_rect_matrix(rect_from_clip_matrix);
  render_clip_from_rect(left,top,width,height);
  return;
}

void render_clip_from_rect(float left, float top, float width, float height)
{
  float stretch;
  int x0;
  int y0;
  int x1;
  int y1;

  x0 = (int)left;
  y0 = (int)top;
  x1 = (int)(left + width);
  y1 = (int)(top + height);
  if (x0 < 0) x0 = 0;
  else if (x0 >= display_state.screen_width) x0 = display_state.screen_width;
  if (x1 < 0) x1 = 0;
  else if (x1 >= display_state.screen_width) x1 = display_state.screen_width;
  if (y0 < 0) y0 = 0;
  else if (y0 >= display_state.screen_height) y0 = display_state.screen_height;
  if (y1 < 0) y1 = 0;
  else if (y1 >= display_state.screen_height) y1 = display_state.screen_height;
  stretch = 1.0;
  if (clip_stretch_on == 1) {
    switch(clip_stretch_step) {
    case 0:
      stretch = 1.033;
      break;
    case 1:
      stretch = 1.066;
      break;
    case 2:
      stretch = 1.1;
      break;
    case 3:
      stretch = 1.133;
      break;
    case 4:
      stretch = 1.166;
    }
    if (frame_pixel_format == 4) {
      fb_clip_rect_store(x0,(int)((float)y0 * stretch),x1,(int)((float)y1 * stretch));
    }
    else {
      fb_clip_rect_store(x0,(int)((float)y0 * stretch),x1 - 1,(int)((float)(y1 - 1) * stretch));
    }
  }
  else {
    if (frame_pixel_format != 4) {
      x1 = x1 + -1;
      y1 = y1 + -1;
    }
    fb_clip_rect_store(x0,y0,x1,y1);
  }
  return;
}

void proj_point_to_pixel_xyw(float x, float y, float z, float *out_x, float *out_y, float *out_w)
{
  float in[3];
  float out[4];
  float w;
  
  mtx_push(rect_from_clip_matrix);
  mtx_mul_by(proj_x_model_matrix);
  in[0] = x;
  in[1] = y;
  in[2] = z;
  mtx_xform_point_xyzw(in,out);
  w = 1.0 / out[3];
  *out_x = out[0] * w;
  *out_y = out[1] * w;
  *out_w = w;
  mtx_pop(1);
}

void mtx_set_rect_x_saved_proj(void)
{
  mtx_set_current_wordwise(rect_from_clip_matrix);
  mtx_mul_by(proj_x_model_matrix);
}

void mtx_xform_point_div_w(float *point, float *out)
{
  float v[4];
  float w;
  
  mtx_xform_point_xyzw(point,v);
  w = 1.0 / v[3];
  *out = v[0] * w;
  out[1] = v[1] * w;
  out[2] = w;
  return;
}

void mtx_premul_rect_proj(float unused_a, float unused_b)
{
  mtx_premul_two(mtx_stack_records.proj.top,rect_from_clip_matrix);
  return;
}

void mtx_premul_rect_proj_view(void)
{
  mtx_push(rect_from_clip_matrix);
  mtx_mul_by(mtx_stack_records.proj.top);
  mtx_mul_by(mtx_stack_records.view.top);
  mtx_times_last_pushed();
  ((int (*)(int))mtx_stack_drop)(1);
}

void coord_scale_store(float scale)
{
  graphics_work.coord_scale = scale;
  return;
}

void mtx_set_w_first_wrap(float *mtx)
{
  mtx_set_w_first(mtx);
  return;
}

void proj_rebuild_rect_proj(void)
{
  mtx_store(proj_saved_matrix);
  mtx_set_w_first_wrap(rect_from_clip_matrix);
  mtx_mul_by_pairs(proj_saved_matrix);
  mtx_store(rect_x_proj_work);
  mtx_store(rect_x_proj_matrix);
  mtx_set_current(proj_saved_matrix);
  return;
}
