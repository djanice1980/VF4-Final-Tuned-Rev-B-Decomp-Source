/*
 * sdk/kamui/lib_state.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint lib_state_ptr; /* 0x0c32ab88 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

void lib_state_set_up_from_cfg(void)
{
  typedef struct { int a; int b; int c; int d; } ta_bufs;
  volatile gfx_state_rec *g;
  lib_info_rec *hdr;
  uint region;
  uint objlist;
  uint isp;
  uint tsp;
  int region_off;
  int objlist_off;
  int isp_off;
  int tsp_off;
  int i;

  if (*(volatile uint *)&lib_state_ptr == 0) {
    *(volatile uint *)&lib_state_ptr = (uint)&gfx_state;
    memset((void *)*(volatile uint *)&lib_state_ptr,0,sizeof(gfx_state_rec));
  }
  hdr = &lib_info;
  hdr->info_words[0] = 4;
  hdr->info_words[1] = 1;
  hdr->info_words[2] = 4;
  hdr->info_words[3] = 0;
  hdr->credits_text = (uint)nec_copyright;
  g = &gfx_state;
  g->info_ptr = (uint)hdr;
  g->fb_sof_hold = 0;
  g->top_skip_mode = 0;
  g->top_skip_lines = 0;
  g->vo_offset_x = 0;
  g->vo_offset_y = 0;
  hook_slots_defaults();
  g->render_busy = 0;
  g->ta_busy = 0;
  g->vsync_countdown = g->vsync_count = cfg_setting_or_default("RENDER","VSYNCCount",1) - 1;
  g->field_0x14 = 0x10000;
  g->vram_size = 0x800000;
  g->field_0x1c = 0x800000;
  g->tile_limit_on = 0;
  g->presort_flag = 0;
  g->region_rebuild = 0;
  g->strip_len_code = 3;
  g->display_regs_pending = 1;
  bg_plane_defaults_set();
  region = cfg_setting_or_default("RENDER","RegionSize",0x6000);
  objlist = cfg_setting_or_default("RENDER","ObjListSize",0x7a000);
  isp = cfg_setting_or_default("RENDER","ISPParamSize",0x180000);
  tsp = cfg_setting_or_default("RENDER","TSPParamSize",0x180000);
  region = (region >> 11) << 11;
  objlist = (objlist >> 11) << 11;
  isp = (isp >> 11) << 11;
  tsp = (tsp >> 11) << 11;
  g->isp_param_size = isp;
  g->tsp_param_size = tsp;
  region_off = 0;
  objlist_off = region;
  isp_off = objlist_off + objlist;
  tsp_off = isp_off + isp;
  for (i = 0; i < 2; i++) {
    gfx_state.area_offsets[i].region = region_off + (i << 22);
    gfx_state.area_offsets[i].objlist = objlist_off + (i << 22);
    gfx_state.area_offsets[i].isp = isp_off + (i << 22);
    gfx_state.area_offsets[i].tsp = tsp_off + (i << 22);
    gfx_state.area_ends[i] = 0x7d0000 - (i << 22);
  }
  gfx_state.single_bank = 0;
  gfx_state.kept_list = 0;
  gfx_state.slot_states[0] = 0;
  gfx_state.slot_split_words[0] = tile_split_word;
  gfx_state.slot_states[1] = 0xffffffff;
  gfx_state.slot_states[2] = 0xffffffff;
  gfx_state.slot_states[3] = 0xffffffff;
  gfx_state.frame_idx = 0;
  gfx_state.render_slot = 0;
  gfx_state.ta_slot = 0;
  gfx_state.upload_ticket = 0;
  gfx_state.mv_hdr_on = 0;
  gfx_state.upload_wait = 1;
  gfx_state.vertex_buf_base = cfg_setting_or_default("VertexBuffer","BufferBase",0xacc00000);
  gfx_state.vertex_buf_size = cfg_setting_or_default("VertexBuffer","BufferSize",0x400000);
  gfx_state.pool_ta_status = cfg_setting_or_default("RENDER","PoolingTAStatus",0);
  gfx_state.render_timeout = cfg_setting_or_default("RENDER","RenderTimeOut",10);
  gfx_state.vertex_timeout = cfg_setting_or_default("RENDER","VertexTimeOut",10);
  gfx_state.vsync_timeout = cfg_setting_or_default("RENDER","VSYNCTimeOut",10);
  gfx_state.modvol_merge = 0;
  gfx_state.modvol_merge_mode = 1;
  gfx_state.list_ctrl = 0xf;
  gfx_state.list_ctrl = gfx_state.list_ctrl;
  gfx_state.field_read_offsets[0] = 0;
  gfx_state.field_read_offsets[1] = 0;
  render_setup_empty_step();
}

void state_release(void)
{
  if (*(volatile uint *)&lib_state_ptr != 0) {
    teardown_empty_step();
    lib_state_ptr = 0;
  }
}

void chips_pool_irq_set_up(void)
{
  device_bringup_noop();
  pvr_regs_defaults_and_vram_clear();
  ta_memory_defaults_set();
  vram_pool_setup();
  pvr_irq_entry_and_masks_set();
  fog_regs_defaults();
  gfx_state.ta_buf_size = 0x100000;
  gfx_state.tile_cols = 0;
  gfx_state.tile_rows = 0;
  gfx_state.double_regions = 0;
  ((void (*)(uint,uint,uint,uint,uint,int))ta_buffers_lay_out)(gfx_state.area_offsets[0].region >> 7,gfx_state.area_offsets[0].objlist >> 7,
               gfx_state.area_offsets[0].isp >> 7,gfx_state.area_offsets[0].tsp >> 7,
               gfx_state.list_ctrl, 0);
}

void busy_wait_scaled(float units)
{
  volatile uint i;
  for (i = 0; i < (uint)(int)(units * 2000.0f); i++) {
  }
}

void error_hook_run(int code)
{
  ((void (*)())gfx_state.hooks[8].fn)();
}

int hook_request_ignored(uint routine, uint arg)
{
  return 0;
}
