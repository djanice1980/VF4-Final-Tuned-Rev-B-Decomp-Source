/*
 * sdk/kamui/pvr_irq.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_render_hang_recover
#define NOPROTO_xfer_job_next
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

void pvrirq_check_or_halt(int ok)
{
  if (ok == 0) {
    *(volatile int *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
}

void pvrirq_sq_save(uint *save)
{
  unsigned int lvl;
  uint q;
  uint v;
  
  {
    unsigned int sr;
    __asm__ volatile ("stc sr,%0" : "=r" (sr));
    lvl = 0xf0;
    lvl &= sr;
    lvl >>= 4;
    sr |= 0xf0;
    __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  }
  q = sq_save_top & 0x03ffffff | 0xe0000000;
  pvrirq_check_or_halt((q & 0x3f) == 0);
  save[0] = *(volatile uint *)0xff000038;
  save[1] = *(volatile uint *)0xff00003c;
  save[2] = sq_save_top;
  sq_save_top = sq_save_top + 0x40;
  v = sq_save_top >> 0x18 & 0x1c;
  *(volatile uint *)0xff000038 = v;
  *(volatile uint *)0xff00003c = v;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  q += 0x20;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  *(volatile uint *)0xff000038 = save[0];
  *(volatile uint *)0xff00003c = save[1];
  {
    unsigned int t;
    __asm__ volatile ("stc sr,%0" : "=r" (t));
    t = (t & 0xffffff0f) | (lvl << 4);
    __asm__ volatile ("ldc %0,sr" : : "r" (t));
  }
}

void pvrirq_sq_restore(uint *save)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  uint *sq;
  uint *s;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  sq = (uint *)0xe0000000;
  s = (uint *)(save[2] & 0x1fffffff | 0xa0000000);
  sq_save_top = sq_save_top - 0x40;
  pvrirq_check_or_halt(save[2] == sq_save_top);
  *(volatile uint *)0xff000038 = save[0];
  *(volatile uint *)0xff00003c = save[1];
  sq[0] = s[0];
  sq[1] = s[1];
  sq[2] = s[2];
  sq[3] = s[3];
  sq[4] = s[4];
  sq[5] = s[5];
  sq[6] = s[6];
  sq[7] = s[7];
  sq[8] = s[8];
  sq[9] = s[9];
  sq[10] = s[10];
  sq[11] = s[11];
  sq[12] = s[12];
  sq[13] = s[13];
  sq[14] = s[14];
  sq[15] = s[15];
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void pvr_irq_entry_and_masks_set(void)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int vbr;
  unsigned int t;
  uint i;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  irq_private_stack_handler = (uint)pvr_irq_service;
  __asm__ volatile ("stc vbr,%0" : "=r" (vbr));
  *(void **)(vbr + 0x224) = (void *)irq_entry_on_private_stack;
  *(volatile uint *)0xa85f6930 = 0;
  *(volatile uint *)0xa85f6938 = 0;
  *(volatile uint *)0xa85f6900 = 0xffffffff;
  *(volatile uint *)0xa85f6908 = 0xffffffff;
  *(volatile uint *)0xa05f6930 = 0x6c;
  *(volatile uint *)0xa25f6930 = 0x44;
  *(volatile uint *)0xa85f6938 = 0x1f;
  elan_irq_regs_init();
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  gfx_state.field_0x1ec = 0;
  gfx_state.pending_events = 0;
  gfx_state.tex_render_on = 0;
  gfx_state.stall_scan_hold = 0;
  render_stats.render_stall_ticks = 0;
  render_stats.vertex_stall_ticks = 0;
  render_stats.render_stalls = 0;
  render_stats.vertex_stalls = 0;
  render_stats.ol_shrink_tries = 0;
  render_stats.param_grow_tries = 0;
  render_stats.tex_render_ticks = 0;
  render_stats.ol_overflows = 0;
  render_stats.param_overflows = 0;
  render_stall_resets = 0;
  vblank_recheck_request = 0;
  for (i = 0; i < 5; i++) {
    render_stats.list_high_water[i] = 0;
  }
}

int hblank_irq(int regs)
{
  *(volatile uint *)(regs + 0x900) = 0x20;
  gfx_state.pending_events = gfx_state.pending_events | 4;
  return 0;
}

int yuv_done_irq(int regs)
{
  *(volatile int *)(regs + 0x900) = 0x40;
  gfx_state.pending_events = gfx_state.pending_events | 0x40;
  return 0;
}

int vblank_skip_check(uint regs)
{
  uint skip;
  if ((gfx_state.flags & 2) != 0) {
    display_field = display_status_reg >> 10 & 1;
  }
  else {
    soft_field_parity = soft_field_parity + 1 & 1;
    display_field = soft_field_parity;
  }
  if ((gfx_state.display_mode & 4) != 0 && *(volatile uint *)&display_field == 1) return 1;
  if (--gfx_state.vsync_countdown >= 0) {
    skip = 1;
    if (vblank_recheck_request != 0) {
      skip = 0;
      vblank_recheck_request = 0;
    }
  }
  else {
    gfx_state.vsync_countdown = gfx_state.vsync_count;
    skip = 0;
  }
  return skip;
}

void render_stall_check(uint *ticks, int slot)
{
  uint v;

  if (gfx_state.slot_states[slot] == 4) {
    v = *ticks + 1;
    *ticks = v;
    if (v > gfx_state.render_timeout) {
      render_stats.render_stalls = render_stats.render_stalls + 1;
      ta_timeout_hook();
      *(volatile uint *)0xa85f8044 = gfx_state.read_ctrl_word & 0xfffffffe;
      *(volatile uint *)0xa85f8008 = 2;
      *(volatile uint *)0xa85f8008 = 0;
      *(volatile uint *)0xa85f8044 = gfx_state.read_ctrl_word;
      tile_regions_write(gfx_state.tile_cols,gfx_state.tile_rows,0,1,0,~gfx_state.list_ctrl & 1);
      tile_regions_write(gfx_state.tile_cols,gfx_state.tile_rows,0,1,1,~gfx_state.list_ctrl & 1);
      gfx_state.slot_states[slot] = 5;
      gfx_state.render_busy = 0;
      render_stall_resets = render_stall_resets + 1;
      *ticks = 0;
    }
  }
}

void vertex_stall_check(uint *ticks, int slot)
{
  if (gfx_state.slot_states[slot] == 2) {
    *ticks = *ticks + 1;
    if (*ticks > gfx_state.vertex_timeout) {
      render_stats.vertex_stalls = render_stats.vertex_stalls + 1;
      if (elan_pvr_reset_retry_frame() == 0) {
        gfx_state.slot_states[slot] = -1;
        gfx_state.ta_busy = 0;
      }
      *ticks = 0;
    }
  }
  return;
}

uint frame_slots_stall_scan(void)
{
  uint busy = 0;
  int i;

  if (gfx_state.stall_scan_hold != 0) return 1;
  if (gfx_state.tex_render_on != 0) {
    render_stats.tex_render_ticks++;
    if (render_stats.tex_render_ticks > 0x10) {
      *(volatile uint *)0xa85f8048 = gfx_state.write_ctrl_word;
      *(volatile uint *)0xa85f804c = gfx_state.line_stride_word;
      *(volatile uint *)0xa85f8068 = gfx_state.x_clip_word;
      *(volatile uint *)0xa85f806c = gfx_state.y_clip_word;
      gfx_state.render_busy = 0;
      render_stats.tex_render_ticks = 0;
    }
    return 1;
  }
  for (i = 0; i < 4; i++) {
    if ((int)gfx_state.slot_states[i] >= 0 && (gfx_state.slot_states[i] & 1) == 0) {
      if (gfx_state.render_timeout == 0) {
        if (gfx_state.slot_states[i] == 4) {
          gfx_state.slot_states[i] = 5;
          if (gfx_state.render_busy != 0) render_stats.render_stalls++;
          gfx_state.render_busy = 0;
          *(volatile uint *)0xa85f8008 = 2;
          *(volatile uint *)0xa85f8008 = 0;
          continue;
        }
        vblank_recheck_request = 1;
        busy = 1;
        vertex_stall_check(&render_stats.vertex_stall_ticks, i);
      } else {
        vblank_recheck_request = 1;
        busy = 1;
        render_stall_check(&render_stats.render_stall_ticks, i);
        vertex_stall_check(&render_stats.vertex_stall_ticks, i);
      }
    }
  }
  return busy | gfx_state.render_busy;
}

void display_flip_rendered(void)
{
  int i;

  for (i = 0; i < 4; i++) {
    if (gfx_state.slot_states[i] == 5 || gfx_state.slot_states[i] == -3) {
      gfx_state.slot_states[i] = 7;
      if ((gfx_state.display_mode & 1) == 0 && (gfx_state.display_mode & 0x2c) == 0x20) {
        both_fb_read_start1_reg = gfx_state.fb_addrs[0] + gfx_state.field_read_offsets[0];
        both_fb_read_start2_reg = gfx_state.fb_addrs[1] + gfx_state.field_read_offsets[1];
      }
      else if (gfx_state.fb_sof_hold != 1) {
        both_fb_read_start1_reg = gfx_state.fb_addrs[i & 1] + gfx_state.field_read_offsets[0];
        both_fb_read_start2_reg = gfx_state.fb_addrs[i & 1] + gfx_state.field_read_offsets[1];
      }
      if (gfx_state.slot_states[i] != -3 && gfx_state.display_regs_pending == 1) {
        both_video_out_ctrl_reg = gfx_state.vo_ctrl_word;
        both_fb_read_ctrl_reg = gfx_state.read_ctrl_word;
        gfx_state.display_regs_pending = 0;
      }
      return;
    }
  }
}

int ta_start_queued_frame(void)
{
  int i;
  int j;

  for (i = 0; i < 4; i++) {
    if (gfx_state.slot_states[i] == 1 && gfx_state.ta_busy == 0) {
      gfx_state.ta_busy = 1;
      gfx_state.slot_states[i] = 2;
      gfx_state.slot_split_words[i] = tile_split_word;
      gfx_state.ta_slot = i;
      j = i & 1;
      render_stats.ta_start_time = tmu0_count_reg;
      *(volatile uint *)0xa85f8008 = 1;
      *(volatile uint *)0xa85f8008 = 0;
      *(volatile uint *)0xa85f8124 = gfx_state.ta_bufs[j].ol_base;
      *(volatile uint *)0xa85f812c = gfx_state.ta_bufs[j].ol_limit;
      *(volatile uint *)0xa85f8128 = gfx_state.ta_bufs[j].isp_base;
      *(volatile uint *)0xa85f8130 = gfx_state.ta_bufs[j].isp_limit;
      *(volatile uint *)0xa85f8164 = gfx_state.ta_bufs[j].opb_start;
      *(volatile uint *)0xa85f8144 = 0x80000000;
      *(volatile uint *)0xa05f8144;
      gfx_state.send_mask = 0;
      gfx_state.field_0x364 = 0;
      render_stats.vertex_stall_ticks = 0;
      if (gfx_state.slot_list_bytes[0][i] != 0) {
        gfx_state.send_mask |= 0x21;
        list_seg_append_padded(0, gfx_state.slot_list_start[0][i], gfx_state.slot_list_bytes[0][i]);
        gfx_state.sent_list = 0;
      }
      if (gfx_state.slot_list_bytes[1][i] != 0) {
        gfx_state.send_mask |= 0x42;
        if (gfx_state.mv_hdr_on == 1) {
          *gfx_state.mv_hdr = *gfx_state.mv_hdr & 0xf0ffffff | 0x1000000;
          list_seg_append_padded(1, (uint)gfx_state.mv_hdr, gfx_state.mv_hdr_bytes);
        }
        list_seg_append_padded(1, gfx_state.slot_list_start[1][i], gfx_state.slot_list_bytes[1][i]);
        gfx_state.sent_list = 1;
      }
      if (gfx_state.slot_list_bytes[2][i] != 0) {
        gfx_state.send_mask |= 0x84;
        list_seg_append_padded(2, gfx_state.slot_list_start[2][i], gfx_state.slot_list_bytes[2][i]);
        gfx_state.sent_list = 2;
      }
      if (gfx_state.slot_list_bytes[3][i] != 0) {
        gfx_state.send_mask |= 0x108;
        if (gfx_state.mv_hdr_on == 1) {
          *gfx_state.mv_hdr = *gfx_state.mv_hdr & 0xf0ffffff | 0x3000000;
          list_seg_append_padded(3, (uint)gfx_state.mv_hdr, gfx_state.mv_hdr_bytes);
        }
        list_seg_append_padded(3, gfx_state.slot_list_start[3][i], gfx_state.slot_list_bytes[3][i]);
        gfx_state.sent_list = 3;
      }
      if (gfx_state.slot_list_bytes[4][i] != 0) {
        gfx_state.send_mask |= 0x210;
        gfx_state.sent_list = 4;
        list_seg_append_padded(4, gfx_state.slot_list_start[4][i], gfx_state.slot_list_bytes[4][i]);
      }
      bank_close_and_submit(gfx_state.send_mask, 0);
      return 1;
    }
  }
  return 1;
}

void render_start_ready_frame(void)
{
  int i;
  uint j;
  uint k;
  int ch;
  uint t;
  uint f;

  for (i = 0; i < 4; i++) {
    if (gfx_state.slot_states[i] == 3 && gfx_state.render_busy == 0) {
      j = i & 1;
      if (gfx_state.slot_split_words[i] != gfx_state.ta_bufs[j].split_word) {
        ch = 1;
        gfx_state.ta_bufs[j].split_word = gfx_state.slot_split_words[i];
      }
      else {
        ch = 0;
      }
      if (ch != 0 || ((j + 1) & gfx_state.region_rebuild) != 0) {
        tile_regions_write(gfx_state.tile_cols,gfx_state.tile_rows,0,1,j,~gfx_state.list_ctrl & 1);
        gfx_state.region_rebuild = ~(j + 1) & gfx_state.region_rebuild & 3;
      }
      else if (gfx_state.ta_bufs[j].ends_pending != 0) {
        gfx_state.ta_bufs[j].ends_pending = 0;
        vram32_write_chip_a(gfx_state.ta_bufs[j].end_a,(uint *)&gfx_state.ta_bufs[j].last_a,4);
        vram32_write_chip_b(gfx_state.ta_bufs[j].end_b,(uint *)&gfx_state.ta_bufs[j].last_b,4);
      }
      tile_split_bitmap_update(j);
      gfx_state.render_busy = 3;
      gfx_state.slot_states[i] = 4;
      if ((gfx_state.flags & 0x10) != 0) {
        *(volatile uint *)0xa85f8008 = 2;
        *(volatile uint *)0xa85f8008 = 0;
      }
      *(volatile uint *)0xa85f802c = gfx_state.ta_bufs[j].region_base;
      *(volatile uint *)0xa85f8020 = gfx_state.ta_bufs[j].isp_base;
      if (gfx_state.fb_sof_hold != 1) {
        if ((gfx_state.display_mode & 1) == 0 && (gfx_state.display_mode & 0x2c) == 0x20) {
          *(volatile uint *)0xa85f8060 = gfx_state.fb_addrs[display_field];
        }
        else {
          *(volatile uint *)0xa85f8060 = gfx_state.fb_addrs[j];
        }
      }
      *(volatile uint *)0xa85f808c = gfx_state.bg_tag;
      *(volatile uint *)0xa85f8088 = gfx_state.bg_depth & 0xfffffff0;
      *(volatile uint *)0xa85f8068 = gfx_state.x_clip_word;
      *(volatile uint *)0xa85f806c = gfx_state.y_clip_word;
      *(volatile uint *)0xa85f804c = gfx_state.line_stride_word;
      *(volatile uint *)0xa85f8048 = gfx_state.write_ctrl_word;
      render_stats.render_stall_ticks = 0;
      if ((gfx_state.display_mode & 0xffffffed) == 0x20) {
        if (display_field == 0) {
          *(volatile uint *)0xa85f80f4 = gfx_state.scaler_word | 0x20000;
        }
        else {
          *(volatile uint *)0xa85f80f4 = gfx_state.scaler_word | 0x60000;
        }
      }
      *(volatile uint *)0xa85f8044 = gfx_state.read_ctrl_word;
      t = *(volatile uint *)0xa05f80e8 & 8;
      f = gfx_state.vo_ctrl_word;
      *(volatile uint *)0xa05f80e8 = f | t;
      t = *(volatile uint *)0xa25f80e8 & 8;
      f = gfx_state.vo_ctrl_word;
      *(volatile uint *)0xa25f80e8 = f | t;
      render_stats.render_start_time = *(volatile uint *)0xffd8000c;
      gfx_state.render_slot = i;
      render_just_started = 1;
      *(volatile uint *)0xa85f8014 = 0xffffffff;
      return;
    }
  }
}

int vblank_irq(int regs)
{
  int r;
  uint busy;
  uint sq_save[3];
  
  *(volatile int *)(regs + 0x900) = 8;
  pvrirq_sq_save(sq_save);
  gfx_state.pending_events = gfx_state.pending_events | 2;
  gfx_state.vblank_ticks = gfx_state.vblank_ticks + 1;
  ((volatile struct { char pad[0x1ac]; uint f; } *)&both_isp_base_reg)->f = 0xf0f0f0f0;
  r = vblank_skip_check(regs);
  if ((r == 0) && (busy = frame_slots_stall_scan(), busy == 0)) {
    display_flip_rendered();
    render_start_ready_frame();
    r = ta_start_queued_frame();
    if (r == 0) {
      pvrirq_sq_restore(sq_save);
      return 1;
    }
    gfx_state.frame_pending = 0;
  }
  pvrirq_sq_restore(sq_save);
  return 0;
}

void ta_input_finish(void)
{
  uint m;
  uint m1;
  uint m2;
  uint n;
  uint i;

  if (gfx_state.slot_states[gfx_state.ta_slot] != -1) {
    if (gfx_state.tex_render_on == 0) {
      gfx_state.pending_events |= 0x20;
      gfx_state.slot_states[gfx_state.ta_slot] = 3;
    }
    m1 = *(volatile uint *)0xa05f8138;
    n = *(volatile uint *)0xa25f8138;
    if (m1 > n) {
      m = m1;
    }
    else {
      m = n;
    }
    if ((gfx_state.flags & 1) != 0) {
      m2 = m;
    }
    else {
      m2 = gfx_state.ta_bufs[gfx_state.ta_slot & 1].isp_limit;
    }
    render_stats.param_used[gfx_state.ta_slot & 1] = m;
    gfx_state.bg_tag = gfx_state.bg_tag_bits & 0xff000000 | (m2 - gfx_state.ta_bufs[gfx_state.ta_slot & 1].isp_base) * 2 & 0xfffff8;
    {
      uint *s = (uint *)&gfx_state.bg_plane[0];
      m2 += 0xa5000000;
      for (i = 0; i < 0x30; i++) {
        ((uint *)m2)[i] = s[i];
      }
    }
    *(volatile uint *)0xa85f8008 = 1;
    i = *(volatile uint *)0xa05f8138;
    i = *(volatile uint *)0xa25f8138;
    *(volatile uint *)0xa85f8008 = 0;
  }
  gfx_state.ta_busy = 0;
  render_stats.ta_time = render_stats.ta_start_time - *(volatile int *)0xffd8000c;
}

int ta_area_split_shift(int grow_objlist)
{
  int i;

  switch (grow_objlist) {
  case 0:
    i = 0;
    do {
      if (gfx_state.ta_bufs[i].isp_limit + 0x2000 < gfx_state.ta_bufs[i].ol_base - 0xc0) {
        gfx_state.ta_bufs[i].isp_limit = gfx_state.ta_bufs[i].isp_limit + 0x2000;
        gfx_state.ta_bufs[i].ol_limit = gfx_state.ta_bufs[i].isp_limit + 0xc0;
        render_stats.param_grow_tries = 0;
      }
      else if (render_stats.param_grow_tries != 0) {
        error_hook_run(3);
      }
      i++;
    } while (i < 2);
    break;
  case 1:
    i = 0;
    do {
      if (gfx_state.ta_bufs[i].ol_limit - 0x800 > gfx_state.ta_bufs[i].isp_base + 0x140) {
        if (render_stats.param_used[i] < gfx_state.ta_bufs[i].ol_limit - 0x800) {
          gfx_state.ta_bufs[i].ol_limit = gfx_state.ta_bufs[i].ol_limit - 0x800;
          gfx_state.ta_bufs[i].isp_limit = gfx_state.ta_bufs[i].ol_limit - 0xc0;
        }
        render_stats.ol_shrink_tries = 0;
      }
      else if (render_stats.ol_shrink_tries > 10) {
        error_hook_run(3);
      }
      i++;
    } while (i < 2);
  }
  return 0;
}

int param_overflow_irq(int regs)
{
  *(volatile uint *)(regs + 0x908) = 4;
  render_stats.param_overflows = render_stats.param_overflows + 1;
  gfx_state.slot_states[gfx_state.ta_slot] = -3;
  if (gfx_state.opb_downward == 1) {
    render_stats.param_grow_tries = render_stats.param_grow_tries + 1;
    ta_area_split_shift(0);
  }
  return 0;
}

int objlist_overflow_irq(int regs)
{
  *(volatile int *)(regs + 0x908) = 8;
  render_stats.ol_overflows = render_stats.ol_overflows + 1;
  if (gfx_state.opb_downward == 1) {
    render_stats.ol_shrink_tries = render_stats.ol_shrink_tries + 1;
    ta_area_split_shift(1);
  }
  return 0;
}

int ta_bad_input_irq(int regs)
{
  *(volatile int *)(regs + 0x908) = 0x10;
  elan_pvr_reset_drop_frame();
  return 0;
}

int isp_cache_irq_ack(int regs)
{
  *(volatile uint *)(regs + 0x908) = 1;
  return 0;
}

int strip_hazard_irq(int regs)
{
  *(volatile int *)(regs + 0x908) = 2;
  if (render_just_started == 0) {
    gfx_state.pending_events |= 0x10;
  }
  render_just_started = 0;
  return 0;
}

int return_zero_0c149d80(void)
{
  return 0;
}

int pvr_irq_service(void)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int m;
  unsigned int t;
  int loop;
  uint st;
  uint kind;
  int r;
  int i;
  uint sq_save[3];

  loop = 1;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  m = sr & 0xf0;
  lvl = m >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  pvrirq_sq_save(sq_save);
  if (bank_render_index > 3) {
    pvrirq_check_or_halt(0);
  }
  while (loop) {
    loop = 0;
    st = *(volatile uint *)0xa05f6900;
    kind = elan_status_service();
    if (gfx_state.vertex_timeout == 0 && kind == 1 && (st & 8) != 0 &&
        list_kind_sent != -1 && xfer_dma_queued <= xfer_dma_finished) {
      list_bank_rec *banks = list_banks;
      if (*(volatile int *)&banks[*(volatile uint *)&bank_render_index].state == 1 ||
          *(volatile int *)&banks[*(volatile uint *)&bank_render_index].state == 2) {
        render_hang_recoveries = render_hang_recoveries + 1;
        render_hang_recover();
        kind = 4;
        loop = 1;
      }
    }
    switch (kind) {
    case 4:
      *(volatile uint *)0xa85f6900 = 0x200780;
      ta_input_finish();
      xfer_job_next();
      break;
    case 1:
    case 6:
      if ((st & 4) != 0) {
        r = render_done_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 8) != 0) {
        r = vblank_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 0x20) != 0) {
        r = hblank_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 0x40) != 0) {
        r = yuv_done_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      st = *(volatile uint *)0xa05f6908;
      if ((st & 4) != 0) {
        r = param_overflow_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 8) != 0) {
        r = objlist_overflow_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 0x10) != 0) {
        r = ta_bad_input_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 2) != 0) {
        r = strip_hazard_irq(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 1) != 0) {
        r = isp_cache_irq_ack(0xa05f6000);
        if (r != 0) error_hook_run(r);
      }
      if (kind == 1) break;
    case 2:
      st = *(volatile uint *)0xa25f6900;
      if ((st & 4) != 0) {
        r = render_done_irq(0xa25f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 0x40) != 0) {
        r = yuv_done_irq(0xa25f6000);
        if (r != 0) error_hook_run(r);
      }
      st = *(volatile uint *)0xa25f6908;
      if ((st & 4) != 0) {
        r = param_overflow_irq(0xa25f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 8) != 0) {
        r = objlist_overflow_irq(0xa25f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 0x10) != 0) {
        r = ta_bad_input_irq(0xa25f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 2) != 0) {
        r = strip_hazard_irq(0xa25f6000);
        if (r != 0) error_hook_run(r);
      }
      if ((st & 1) != 0) {
        r = isp_cache_irq_ack(0xa25f6000);
        if (r != 0) error_hook_run(r);
      }
      break;
    case 3:
    case 5:
      break;
    case 0:
      goto done;
    }
  }
  for (i = 0; i < 9; i++) {
    if ((gfx_state.pending_events & (1 << i)) != 0) {
      gfx_state.pending_events &= ~(1 << i);
      gfx_state.hooks[i].fn(
        gfx_state.hooks[i].arg);
    }
  }
done:
  pvrirq_sq_restore(sq_save);
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return 0;
}

int render_done_irq(int regs)
{
  *(volatile int *)(regs + 0x900) = 7;
  gfx_state.render_busy &= regs == 0xa05f6000 ? ~1 : ~2;
  if (gfx_state.render_busy == 0) {
    render_stats.render_stall_ticks = 0;
    render_stats.render_time = render_stats.render_start_time - *(volatile int *)0xffd8000c;
    if (gfx_state.tex_render_on != 0) {
      gfx_state.tex_render_on = 0;
      render_stats.tex_render_ticks = 0;
      *(volatile uint *)0xa85f8048 = gfx_state.write_ctrl_word;
      *(volatile uint *)0xa85f804c = gfx_state.line_stride_word;
      *(volatile uint *)0xa85f8068 = gfx_state.x_clip_word;
      *(volatile uint *)0xa85f806c = gfx_state.y_clip_word;
    }
    else {
      gfx_state.slot_states[gfx_state.render_slot] = 5;
    }
    gfx_state.pending_events |= 1;
    render_stall_resets = 0;
  }
  return 0;
}
