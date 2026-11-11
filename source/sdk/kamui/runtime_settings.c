/*
 * sdk/kamui/runtime_settings.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

int vertex_area_base_size_put(uint base, uint size)
{
  gfx_state.vertex_buf_base = base;
  gfx_state.vertex_buf_size = size;
  return 0;
}

int render_stats_read(frame_stats_rec *stats)
{
  typedef struct { int w[6]; } out6;
  int i;

  for (i = 0; i < 2; i++) {
    ((out6 *)stats)[1 - i].w[0] = gfx_state.ta_bufs[i].isp_base;
    ((out6 *)stats)[1 - i].w[1] = gfx_state.ta_bufs[i].ol_base;
    ((out6 *)stats)[1 - i].w[2] = gfx_state.ta_bufs[i].region_base;
    ((out6 *)stats)[1 - i].w[3] = gfx_state.ta_bufs[i].isp_limit;
    ((out6 *)stats)[1 - i].w[4] = gfx_state.ta_bufs[i].ol_limit;
    ((out6 *)stats)[1 - i].w[5] = render_stats.param_used[i];
    *(int *)((1 - i) * 4 + (int)stats + 0x38) = gfx_state.field_read_offsets[i];
  }
  if (gfx_state.single_bank == 0) {
    stats->shown_buffer_addr = gfx_state.fb_addrs[gfx_state.frame_idx & 1];
    stats->hidden_buffer_addr = gfx_state.fb_addrs[(gfx_state.frame_idx & 1) ^ 1];
  } else {
    stats->hidden_buffer_addr = gfx_state.fb_addrs[gfx_state.frame_idx & 1];
    stats->shown_buffer_addr = gfx_state.fb_addrs[(gfx_state.frame_idx & 1) ^ 1];
  }
  *(int *)((int)stats + 0x40) = gfx_state.x_clip_word;
  *(int *)((int)stats + 0x44) = gfx_state.y_clip_word;
  stats->render_stalls = render_stats.render_stalls;
  stats->vertex_stalls = render_stats.vertex_stalls;
  *(int *)((int)stats + 0x50) = render_stats.ol_overflows;
  *(int *)((int)stats + 0x54) = render_stats.param_overflows;
  for (i = 0; i < 5; i++) {
    /* the (int *) drops gfx_state's volatile: the game reads these two with indexed loads,
       which a volatile read does not give; scene_lists_close and frame_lists_send_and_rearm
       cast the same table the same way */
    *(int *)((int)stats + i * 4 + 0x58) = *(int *)&gfx_state.slot_list_start[i][0];
    *(int *)((int)stats + i * 4 + 0x6c) = *(int *)&gfx_state.slot_list_start[i][1];
    *(int *)((int)stats + i * 4 + 0x80) = gfx_state.list_limits[i];
    *(int *)((int)stats + i * 4 + 0x94) = render_stats.list_high_water[i];
  }
  stats->vertex_input_ticks = render_stats.ta_time;
  stats->drawing_ticks = render_stats.render_time;
  return 0;
}

int frame_stall_limits_put(uint vertex_limit, int render_limit)
{
  gfx_state.vertex_timeout = vertex_limit;
  gfx_state.render_timeout = render_limit;
  if (render_limit == 0) {
    gfx_state.flags = gfx_state.flags | 0x10;
  }
  else {
    switch (chip_revision_reg) {
    case 4:
    case 5:
    case 6:
    case 8:
      gfx_state.flags = gfx_state.flags | 0x10;
      break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 7:
    case 9:
    default:
      gfx_state.flags = gfx_state.flags & 0xffffffef;
      break;
    }
  }
  return 0;
}

int ta_yuv_regs_write(surface_rec *surface, int format, int mode, int width, int height)
{
  uint ctrl;
  
  ctrl = 0;
  if (format == 3) {
    ctrl = 0x1000000;
  }
  if (mode == 0) {
    ctrl = ctrl | (height - 1U & 0x3f) << 8 | width - 1U & 0x3f;
  }
  else {
    ctrl = ctrl | (height - 1U & 0x3f) << 8 | width - 1U & 0x3f | 0x10000;
  }
  pvr_reg_write_both(0x14c,ctrl);
  pvr_reg_write_both(0x148,surface->tex_address);
  return 0;
}

int mv_hdr_buffer_set(uint *buffer, uint bytes)
{
  if (bytes == 0) {
    gfx_state.mv_hdr_on = 0;
    return 0;
  }
  gfx_state.mv_hdr_on = 1;
  if (((uint)buffer & 0x1f) != 0 || (bytes & 0x1f) != 0) {
    gfx_state.mv_hdr_on = 0;
    return 7;
  }
  gfx_state.mv_hdr = buffer;
  gfx_state.mv_hdr_bytes = bytes;
  return 0;
}
