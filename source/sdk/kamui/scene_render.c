/*
 * sdk/kamui/scene_render.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint list_end_word_reset; /* 0x0c21690c */
extern uint far_triangle_block[32]; /* 0x0c216914 */
extern uint list_end_word; /* 0x0c216994 */
extern uint fb_pair_index; /* 0x0c32a75c */
extern fb_write_rec fb_write_block; /* 0x0c41df70 */
extern render_setup_rec render_setup; /* 0x0c41df94 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

void opaque_list_far_triangle(void)
{
  uint dst;
  uint i;
  uchar *src;
  
  dst = list_write_ptrs[0];
  src = (uchar *)far_triangle_block;
  ((volatile uint *)0xff000038)[0] = dst >> 24;
  ((volatile uint *)0xff000038)[1] = dst >> 24;
  dst = dst & 0x3ffffff | 0xe0000000;
  for (i = 0; i < 4; i++) {
    copy_by_pairs_or_words((uint *)dst,(uint *)src,0x20);
    __asm__ __volatile__ ("pref @%0" : : "r" (dst));
    dst += 0x20;
    src += 0x20;
  }
  list_write_ptrs[0] += 0x80;
}

int fb_pair_install(surface_rec *front, surface_rec *back, int mode, uint pair_word)
{
  int ret = 0;
  
  if ((front == 0) | (back == 0)) {
    return 0xe;
  }
  render_setup.front_surface = (uint)front;
  render_setup.back_surface = (uint)back;
  render_setup.field_0x44 = pair_word;
  render_setup.fb_mode = mode;
  gfx_state.fb_sof_hold = mode;
  if (setup_steps_read() == 0x80000000) {
    if (front->format_flags != back->format_flags) {
      return 0xf;
    }
    if (front->bytes != back->bytes) {
      return 0x10;
    }
    gfx_state.fb_addrs[0] = front->tex_address;
    gfx_state.field_0x334 = 0;
    gfx_state.fb_addrs[1] = back->tex_address;
    gfx_state.field_0x338 = 0;
    pvr_reg_write_both(0x50,gfx_state.fb_r_sof1);
    pvr_reg_write_both(0x54,gfx_state.fb_r_sof1);
    pvr_reg_write_both(0x60,gfx_state.fb_addrs[0]);
    pvr_reg_write_both(100,gfx_state.fb_addrs[1]);
    gfx_state.fb_r_sof1 = gfx_state.fb_addrs[0];
    gfx_state.fb_r_sof2 = gfx_state.fb_addrs[1];
    if (mode == 1) {
      gfx_state.field_read_offsets[0] = 0;
      gfx_state.field_read_offsets[1] = 0;
      pvr_reg_write_both(0x50,gfx_state.fb_r_sof1);
      pvr_reg_write_both(0x54,gfx_state.fb_r_sof2);
    }
    else {
      pvr_reg_write_both(0x50,gfx_state.fb_r_sof1);
      pvr_reg_write_both(0x54,gfx_state.fb_r_sof1);
    }
    gfx_state.display_regs_pending = 1;
    gfx_state.fb_pair_word = pair_word;
  }
  else if (setup_steps_mark(0x800) == 0xb00) {
    setup_steps_store(0x80000000);
    ret = render_setup_finish(render_setup.step_kind);
  }
  return ret;
}

void lists_send_to_ta(uint skip_list)
{
  uint list;
  int *bytes;
  uint *start;
  int f;
  
  f = gfx_state.frame_idx;
  start = (uint *)&gfx_state.slot_list_start[0][0] + f;
  bytes = (int *)&gfx_state.slot_list_bytes[0][0] + f;
  list = 0;
  do {
    if (*bytes != 0 && skip_list != list) {
      if ((gfx_state.mv_hdr_on == 1) && (list == 1 || list == 3)) {
        *gfx_state.mv_hdr = *gfx_state.mv_hdr & 0xf0ffffff | list << 0x18;
        list_seg_append_padded(list, (uint)gfx_state.mv_hdr, gfx_state.mv_hdr_bytes);
      }
      list_seg_append_padded(list,*start,*bytes);
    }
    bytes = bytes + 4;
    list = list + 1;
    start = start + 4;
  } while (list < 5);
  bank_close_and_submit(gfx_state.send_mask,skip_list);
}

int frame_lists_send_and_rearm(void)
{
  uint i;
  int timed_out;
  int *p;
  int *q;
  
  direct_send_hold(1);
  if (list_send_skip == 0) {
    if (gfx_state.single_bank == 0) {
      gfx_state.slot_states[gfx_state.frame_idx] = 1;
      gfx_state.frame_pending = 1;
    }
    else {
      render_stats.ta_start_time = tmu0_count_reg;
      i = gfx_state.frame_idx;
      gfx_state.ta_slot = i;
      gfx_state.ta_busy = 1;
      gfx_state.field_0x364 = 0;
      gfx_state.send_mask = gfx_state.used_lists;
      gfx_state.frame_pending = 1;
      render_stats.vertex_stall_ticks = 0;
      gfx_state.slot_states[i] = 2;
      gfx_state.slot_split_words[i] = tile_split_word;
      if ((gfx_state.kept_list & 0x20) != 0) {
        lists_send_to_ta(gfx_state.direct_list);
      }
      else {
        p = (int *)&gfx_state.slot_list_start[0][0] + gfx_state.frame_idx;
        q = (int *)&gfx_state.slot_list_bytes[0][0] + gfx_state.frame_idx;
        q += gfx_state.direct_list * 4;
        p += gfx_state.direct_list * 4;
        if (*q != 0) {
          list_seg_append_padded(gfx_state.direct_list,*p,*q);
        }
        lists_send_to_ta(gfx_state.direct_list);
      }
    }
  }
  gfx_state.vblank_ticks = 0;
  timed_out = 0;
  while (gfx_state.frame_pending != 0) {
    gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > gfx_state.vertex_timeout + gfx_state.render_timeout + 1) {
      timed_out = 1;
      break;
    }
  }
  if (timed_out != 0) {
    gfx_state.render_busy = 0;
    gfx_state.vblank_ticks = 0;
    timed_out = 0;
    while (gfx_state.frame_pending != 0) {
      gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
      if (gfx_state.vblank_ticks > 0x14) {
        timed_out = 1;
        break;
      }
    }
    if (timed_out != 0) {
      error_hook_run(4);
    }
  }
  gfx_state.frame_idx = gfx_state.frame_idx + 1;
  gfx_state.frame_idx = gfx_state.frame_idx & 3;
  gfx_state.slot_states[gfx_state.frame_idx] = 0;
  gfx_state.slot_split_words[gfx_state.frame_idx] = tile_split_word;
  if (gfx_state.single_bank == 1 || list_send_skip == 1) {
    i = gfx_state.frame_idx & 1;
    xfer_wait_idle();
    pvr_reg_write_both(8,1);
    pvr_reg_write_both(8,0);
    pvr_reg_write_both(0x124,gfx_state.ta_bufs[i].ol_base);
    pvr_reg_write_both(300,gfx_state.ta_bufs[i].ol_limit);
    pvr_reg_write_both(0x128,gfx_state.ta_bufs[i].isp_base);
    pvr_reg_write_both(0x130,gfx_state.ta_bufs[i].isp_limit);
    pvr_reg_write_both(0x164,gfx_state.ta_bufs[i].opb_start);
    pvr_reg_write_both(0x144,0x80000000);
    pvr_reg_read(0x144);
    gfx_state.field_0x364 = 0;
    gfx_state.send_mask = gfx_state.used_lists;
    if (list_send_skip != 0) {
      gfx_state.ta_slot = gfx_state.frame_idx;
      gfx_state.slot_states[gfx_state.frame_idx] = 2;
      gfx_state.slot_split_words[gfx_state.frame_idx] = tile_split_word;
      gfx_state.ta_busy = 1;
      gfx_state.stall_scan_hold = 1;
    }
  }
  direct_send_hold(0);
  return 0;
}

void fb_pair_index_toggle(void)
{
  fb_pair_index = fb_pair_index + 1 & 1;
  return;
}

void pvr_render_pass_start(fb_write_rec *wr)
{
  volatile gfx_state_rec *p;
  volatile gfx_state_rec *p2;
  uint idx;
  int abort;
  
  gfx_state.render_busy = 3;
  idx = gfx_state.ta_slot & 1;
  if ((gfx_state.flags & 0x10) != 0) {
    pvr_reg_write_both(8,2);
    pvr_reg_write_both(8,0);
  }
  p = &gfx_state;
  pvr_reg_write_both(0x2c,p->ta_bufs[idx].region_base);
  pvr_reg_write_both(0x20,p->ta_bufs[idx].isp_base);
  pvr_reg_write_both(0x60,wr->write_start);
  pvr_reg_write_both(0x48,wr->write_ctrl);
  pvr_reg_write_both(0x4c,wr->line_stride);
  pvr_reg_write_both(0x68,wr->clip_x);
  pvr_reg_write_both(0x6c,wr->clip_y);
  p->vblank_ticks = 0;
  abort = 0;
  if (p->ta_busy != 0) {
    do {
      gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
      if (gfx_state.vblank_ticks > gfx_state.vertex_timeout) {
        abort = 1;
        break;
      }
    } while (gfx_state.ta_busy != 0);
  }
  if (abort != 0) {
    pvr_reg_write_both(0x48,gfx_state.write_ctrl_word);
    pvr_reg_write_both(0x4c,gfx_state.line_stride_word);
    pvr_reg_write_both(0x68,gfx_state.x_clip_word);
    pvr_reg_write_both(0x6c,gfx_state.y_clip_word);
    gfx_state.ta_busy = 0;
    gfx_state.tex_render_on = 0;
    listbuf_bank_select(fb_pair_index);
  }
  else {
    p2 = &gfx_state;
    if (p2->slot_split_words[gfx_state.frame_idx] != p2->ta_bufs[idx].split_word) {
      p2->ta_bufs[idx].split_word = p2->slot_split_words[gfx_state.frame_idx];
      tile_regions_write(p2->tile_cols,p2->tile_rows,0,1,idx,~p2->list_ctrl & 1);
      p2->ta_bufs[idx].split_word = -1;
    }
    pvr_reg_write_both(0x8c,gfx_state.bg_tag);
    pvr_reg_write_both(0x88,gfx_state.bg_depth & 0xfffffff0);
    { volatile uint *hw = (volatile uint *)0xa85f81a4; hw[12] = 0xf0f0f0f0; }
    pvr_reg_write_both(0x14,0xffffffff);
  }
  return;
}

int fb_pack_mode_from_texture(surface_rec *surf)
{
  uint mode;

  if ((surf->format_flags & 4) != 0) return 4;
  if (surf->texel_size_class != 1) return 4;
  switch (surf->pixel_format & 0x38000000) {
  case 0:
    mode = 3;
    break;
  case 0x08000000:
    mode = 1;
    break;
  case 0x10000000:
    mode = 2;
    break;
  }
  fb_write_block.write_ctrl = gfx_state.write_ctrl_word & 0xfffffff8 | mode;
  return 0;
}

void render_setup_empty_step(void)
{
  return;
}

int scene_lists_close(void)
{
  int *starts[5];
  uint *bytes[5];
  uint hdr[8];
  int idx;
  uint zero = 0;
  int i;
  int j;
  uint p;
  volatile int *q;

  hdr[0] = 0x80000000;
  hdr[1] = zero;
  hdr[2] = 0x800400;
  hdr[3] = zero;
  hdr[4] = 0xffffffff;
  hdr[5] = 0xffffffff;
  hdr[6] = 0xffffffff;
  hdr[7] = 0xffffffff;
  if ((gfx_state.flags & 0x40) != 0 && gfx_state.opb_codes[0] != 0 && gfx_state.opb_codes[2] != 0 && gfx_state.presort_flag != 0) {
    opaque_list_far_triangle();
  }
  idx = gfx_state.frame_idx;
  starts[0] = (int *)&gfx_state.slot_list_start[0][0] + idx;
  starts[1] = (int *)&gfx_state.slot_list_start[1][0] + idx;
  starts[2] = (int *)&gfx_state.slot_list_start[2][0] + idx;
  starts[3] = (int *)&gfx_state.slot_list_start[3][0] + idx;
  starts[4] = (int *)&gfx_state.slot_list_start[4][0] + idx;
  bytes[0] = (uint *)&gfx_state.slot_list_bytes[0][0] + idx;
  bytes[1] = (uint *)&gfx_state.slot_list_bytes[1][0] + idx;
  bytes[2] = (uint *)&gfx_state.slot_list_bytes[2][0] + idx;
  bytes[3] = (uint *)&gfx_state.slot_list_bytes[3][0] + idx;
  bytes[4] = (uint *)&gfx_state.slot_list_bytes[4][0] + idx;
  *starts[0] = list_starts[0];
  *starts[1] = list_starts[1];
  *starts[2] = list_starts[2];
  *starts[3] = list_starts[3];
  *starts[4] = list_starts[4];
  if (gfx_state.single_bank == 0) {
    for (i = 0; i < 5; i++) {
      if (gfx_state.used_lists & (1 << i)) {
        *bytes[i] = (((int *)list_write_ptrs)[i] - *starts[i]) >> 2 << 2;
      }
    }
    for (i = 0; i < 5; i++) {
      if ((gfx_state.used_lists & (1 << i)) == 0) {
        *bytes[i] = 0;
      }
    }
    for (i = 0; i < 5; i++) {
      if ((1 << i) & gfx_state.used_lists) {
        if (*bytes[i] > render_stats.list_high_water[i]) {
          render_stats.list_high_water[i] = *bytes[i];
        }
        if (*bytes[i] > gfx_state.list_limits[i]) {
          for (j = 0; j < 5; j++) {
            if ((1 << j) & gfx_state.used_lists) {
              hdr[0] = hdr[0] & 0xf0ffffff | j << 24;
              copy_words((uint *)*starts[j], hdr, 32);
              copy_words((uint *)(*starts[j] + 32), &list_end_word_reset, 4);
              *bytes[j] = 32;
            }
          }
          error_hook_run(2);
          break;
        }
      }
    }
  }
  else if ((gfx_state.kept_list & 0x20) != 0) {
    for (i = 0, q = (volatile int *)((char *)&gfx_state + 0x3fc); i < 5; i++) {
      if (i != q[13]) {
        if ((1 << i) & gfx_state.used_lists) {
          *bytes[i] = (((int *)list_write_ptrs)[i] - *starts[i]) >> 2 << 2;
          if (*bytes[i] > render_stats.list_high_water[i]) {
            render_stats.list_high_water[i] = *bytes[i];
          }
          if (*bytes[i] > gfx_state.list_limits[i]) {
            for (j = 0; j < 5; j++) {
              if (j != gfx_state.direct_list && ((1 << j) & gfx_state.used_lists)) {
                hdr[0] = hdr[0] & 0xf0ffffff | j << 24;
                copy_words((uint *)*starts[j], hdr, 32);
                copy_words((uint *)(*starts[j] + 32), &list_end_word_reset, 4);
                *bytes[j] = 32;
              }
              else {
                *bytes[j] = 0;
              }
            }
            error_hook_run(2);
            break;
          }
        }
        else {
          *bytes[i] = 0;
        }
      }
    }
    if (list_write_ptrs[gfx_state.direct_list] == *starts[gfx_state.direct_list]) {
      *(volatile uint *)0xff000038 = 9;
      *(volatile uint *)0xff00003c = 9;
      hdr[0] = hdr[0] & 0xf0ffffff | (gfx_state.direct_list & 7) << 24;
      p = list_write_ptrs[gfx_state.direct_list] & 0x3ffffff | 0xe0000000;
      copy_words((uint *)p, hdr, 32);
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
    }
  }
  else {
    listbuf_transfer_done();
    for (i = 0; i < 5; i++) {
      *bytes[i] = (((int *)list_write_ptrs)[i] - *starts[i]) >> 2 << 2;
      if (((1 << i) & gfx_state.used_lists) == 0) {
        *bytes[i] = 0;
      }
      if (gfx_state.used_lists & (1 << i)) {
        if (*bytes[i] > render_stats.list_high_water[i]) {
          render_stats.list_high_water[i] = *bytes[i];
        }
        if (*bytes[i] > gfx_state.list_limits[i]) {
          for (j = 0; j < 5; j++) {
            if ((1 << j) & gfx_state.used_lists) {
              hdr[0] = hdr[0] & 0xf0ffffff | j << 24;
              copy_words((uint *)*starts[j], hdr, 32);
              copy_words((uint *)(*starts[j] + 32), &list_end_word_reset, 4);
              *bytes[j] = 32;
            }
            else {
              *bytes[j] = 0;
            }
          }
          error_hook_run(2);
          break;
        }
      }
    }
  }
  list_send_skip = 0;
  fb_pair_index_toggle();
  listbuf_bank_select(fb_pair_index);
  return 0;
}

void scene_frame_pending_mark(void)
{
  gfx_state.frame_pending = 1;
  gfx_state.stall_scan_hold = 0;
  list_send_skip = 1;
  return;
}

int close_lists_and_draw_to_surface(surface_rec *surf)
{
  char work[32];
  int ret;
  uint saved_27c;
  uint half;
  int timeout;
  uint buf;
  volatile uint *src;
  volatile uint *dst;
  uint i;
  uint bg_tag_saved;
  uint clip_x;
  uint clip_y;
  uint w;
  uint h;
  uint sel;
  uint d;

  ret = fb_pack_mode_from_texture(surf);
  if (ret != 0) {
    listbuf_bank_select(fb_pair_index);
    return ret;
  }
  fb_write_block.line_stride = (surf->width << 4) >> 6;
  fb_write_block.write_start = surf->tex_address | 0x1000000;
  buf = gfx_state.frame_idx;
  half = buf & 1;
  src = &gfx_state.slot_list_start[0][buf];
  dst = &gfx_state.slot_list_bytes[0][buf];
  if (gfx_state.single_bank == 0) {
    gfx_state.vblank_ticks = 0;
    timeout = 0;
    while (gfx_state.ta_busy != 0) {
      gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
      if (gfx_state.vblank_ticks > gfx_state.vertex_timeout) {
        timeout = 1;
        break;
      }
    }
    if (timeout) {
      gfx_state.tex_render_on = 0;
      gfx_state.ta_busy = 0;
      listbuf_bank_select(fb_pair_index);
      return -1;
    }
  }
  gfx_state.slot_list_start[0][buf] = list_starts[0];
  gfx_state.slot_list_start[1][buf] = list_starts[1];
  gfx_state.slot_list_start[2][buf] = list_starts[2];
  gfx_state.slot_list_start[3][buf] = list_starts[3];
  gfx_state.slot_list_start[4][buf] = list_starts[4];
  for (i = 0; i < 5; i++) {
    d = list_write_ptrs[i];
    d -= *src;
    d &= ~3;
    *dst = d;
    if (gfx_state.single_bank == 0 || i != gfx_state.direct_list) {
      if (((1 << i) & (d = gfx_state.used_lists)) != 0) {
        copy_words(((uint **)list_write_ptrs)[i], &list_end_word, 4);
        list_write_ptrs[i] += 32;
        if (*dst > gfx_state.list_limits[i]) {
          error_hook_run(2);
        }
      }
    }
    dst += 4;
    src += 4;
  }
  bg_tag_saved = gfx_state.bg_tag;
  gfx_state.vblank_ticks = 0;
  timeout = 0;
  while (gfx_state.render_busy != 0) {
    gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > gfx_state.render_timeout) {
      timeout = 1;
      break;
    }
  }
  if (timeout) {
    gfx_state.ta_busy = 0;
    listbuf_bank_select(fb_pair_index);
    return -1;
  }
  gfx_state.tex_render_on = 1;
  gfx_state.ta_busy = 1;
  if (gfx_state.single_bank == 0) {
    xfer_wait_idle();
    pvr_reg_write_both(8, 1);
    pvr_reg_write_both(8, 0);
    pvr_reg_write_both(0x124, gfx_state.ta_bufs[half].ol_base);
    pvr_reg_write_both(300, gfx_state.ta_bufs[half].ol_limit);
    pvr_reg_write_both(0x128, gfx_state.ta_bufs[half].isp_base);
    pvr_reg_write_both(0x130, gfx_state.ta_bufs[half].isp_limit);
    pvr_reg_write_both(0x164, gfx_state.ta_bufs[half].opb_start);
    pvr_reg_write_both(0x144, 0x80000000);
    pvr_reg_read(0x144);
  }
  gfx_state.vblank_ticks = 0;
  timeout = 0;
  while (gfx_state.render_busy != 0) {
    gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > gfx_state.render_timeout) {
      timeout = 1;
      break;
    }
  }
  if (timeout) {
    gfx_state.tex_render_on = 0;
    listbuf_bank_select(fb_pair_index);
    return -2;
  }
  gfx_state.ta_slot = buf;
  gfx_state.send_mask = gfx_state.used_lists;
  gfx_state.field_0x364 = 0;
  if (gfx_state.single_bank == 0) {
    lists_send_to_ta(0xff);
  } else if (gfx_state.kept_list == 0) {
    lists_send_to_ta(0xff);
  } else {
    lists_send_to_ta(gfx_state.direct_list);
  }
  clip_x = gfx_state.x_clip_word;
  clip_y = gfx_state.y_clip_word;
  if (surf->width > gfx_state.fb_width) {
    w = gfx_state.fb_width;
  } else {
    w = surf->width - 1;
  }
  if (surf->height > gfx_state.fb_height) {
    h = gfx_state.fb_height;
  } else {
    h = surf->height - 1;
  }
  fb_clip_rect_store(0, 0, w, h);
  fb_write_block.clip_x = gfx_state.x_clip_word;
  fb_write_block.clip_y = gfx_state.y_clip_word;
  gfx_state.x_clip_word = clip_x;
  gfx_state.y_clip_word = clip_y;
  pvr_render_pass_start(&fb_write_block);
  gfx_state.bg_tag = bg_tag_saved;
  listbuf_bank_select(fb_pair_index);
  if (gfx_state.single_bank == 1) {
    gfx_state.vblank_ticks = 0;
    timeout = 0;
    while (gfx_state.render_busy != 0) {
      gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
      if (gfx_state.vblank_ticks > gfx_state.render_timeout) {
        timeout = 1;
        break;
      }
    }
    if (timeout) {
      gfx_state.tex_render_on = 0;
      return -3;
    }
    xfer_wait_idle();
    pvr_reg_write_both(8, 1);
    pvr_reg_write_both(8, 0);
    pvr_reg_write_both(0x124, gfx_state.ta_bufs[half].ol_base);
    pvr_reg_write_both(300, gfx_state.ta_bufs[half].ol_limit);
    pvr_reg_write_both(0x128, gfx_state.ta_bufs[half].isp_base);
    pvr_reg_write_both(0x130, gfx_state.ta_bufs[half].isp_limit);
    pvr_reg_write_both(0x164, gfx_state.ta_bufs[half].opb_start);
    pvr_reg_write_both(0x144, 0x80000000);
    pvr_reg_read(0x144);
  }
  list_send_skip = 0;
  gfx_state.field_0x27c = saved_27c;
  return 0;
}

int redraw_lists_to_surface(surface_rec *surf)
{
  int r;
  int timed_out;
  uint i;
  uint w;
  uint h;
  uint clip_x;
  uint clip_y;

  r = fb_pack_mode_from_texture(surf);
  if (r != 0) {
    listbuf_bank_select(fb_pair_index);
    return r;
  }
  fb_write_block.line_stride = (uint)(surf->width << 4) >> 6;
  fb_write_block.write_start = surf->tex_address | 0x1000000;
  i = gfx_state.frame_idx;
  gfx_state.vblank_ticks = 0;
  timed_out = 0;
  while (gfx_state.ta_busy != 0) {
    gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > gfx_state.vertex_timeout) {
      timed_out = 1;
      break;
    }
  }
  if (timed_out != 0) return;
  gfx_state.vblank_ticks = 0;
  timed_out = 0;
  while (gfx_state.render_busy != 0) {
    gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > gfx_state.render_timeout) {
      timed_out = 1;
      break;
    }
  }
  if (timed_out != 0) {
    gfx_state.render_busy = 0;
    return;
  }
  gfx_state.tex_render_on = 1;
  clip_x = gfx_state.x_clip_word;
  clip_y = gfx_state.y_clip_word;
  w = (surf->width > gfx_state.fb_width) ? gfx_state.fb_width : surf->width - 1;
  h = (surf->height > gfx_state.fb_height) ? gfx_state.fb_height : surf->height - 1;
  fb_clip_rect_store(0,0,w,h);
  fb_write_block.clip_x = gfx_state.x_clip_word;
  fb_write_block.clip_y = gfx_state.y_clip_word;
  gfx_state.x_clip_word = clip_x;
  gfx_state.y_clip_word = clip_y;
  pvr_render_pass_start(&fb_write_block);
  gfx_state.vblank_ticks = 0;
  timed_out = 0;
  while (gfx_state.render_busy != 0) {
    gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > gfx_state.render_timeout) {
      timed_out = 1;
      break;
    }
  }
  if (timed_out != 0) {
    gfx_state.tex_render_on = 0;
    return;
  }
  xfer_wait_idle();
  i = i & 1;
  pvr_reg_write_both(8,1);
  pvr_reg_write_both(8,0);
  pvr_reg_write_both(0x124,gfx_state.ta_bufs[i].ol_base);
  pvr_reg_write_both(300,gfx_state.ta_bufs[i].ol_limit);
  pvr_reg_write_both(0x128,gfx_state.ta_bufs[i].isp_base);
  pvr_reg_write_both(0x130,gfx_state.ta_bufs[i].isp_limit);
  pvr_reg_write_both(0x164,gfx_state.ta_bufs[i].opb_start);
  pvr_reg_write_both(0x144,0x80000000);
  pvr_reg_read(0x144);
  gfx_state.field_0x364 = 0;
  gfx_state.send_mask = gfx_state.used_lists;
  gfx_state.ta_slot = gfx_state.frame_idx;
  gfx_state.ta_busy = 1;
  gfx_state.stall_scan_hold = 1;
  gfx_state.slot_states[gfx_state.frame_idx] = 2;
  gfx_state.slot_split_words[gfx_state.frame_idx] = tile_split_word;
  return 0;
}

int fb_front_as_surface_desc(surface_rec *desc)
{
  int height;
  int width;

  if (gfx_state.video_out == 1) {
    width = 0x280;
    height = 0x1e0;
  }
  else {
    width = (gfx_state.display_mode & 0x10) ? 0x280 : 0x140;
    height = (gfx_state.display_mode & 0x20) ? 0x1e0 : 0xf0;
  }
  switch(gfx_state.fb_format) {
  case 0:
    desc->texel_size_class = 1;
    desc->pixel_format = 0x08000000;
    break;
  case 1:
  case 3:
    desc->texel_size_class = 1;
    desc->pixel_format = 0;
    break;
  case 2:
  case 4:
  case 5:
  default:
    return 4;
  }
  desc->kind = 2;
  desc->width = 0x400;
  desc->height = 0x400;
  desc->bytes = width * height * 2;
  desc->format_flags = 0x10;
  desc->tex_address = gfx_state.fb_addrs[fb_pair_index];
  return 0;
}

void ta_lists_rearm(void)
{
  uint i;

  if (gfx_state.single_bank == 1) {
    xfer_wait_idle();
    i = gfx_state.frame_idx & 1;
    pvr_reg_write_both(8,1);
    pvr_reg_write_both(8,0);
    pvr_reg_write_both(0x124,gfx_state.ta_bufs[i].ol_base);
    pvr_reg_write_both(300,gfx_state.ta_bufs[i].ol_limit);
    pvr_reg_write_both(0x128,gfx_state.ta_bufs[i].isp_base);
    pvr_reg_write_both(0x130,gfx_state.ta_bufs[i].isp_limit);
    pvr_reg_write_both(0x164,gfx_state.ta_bufs[i].opb_start);
    pvr_reg_write_both(0x144,0x80000000);
    pvr_reg_read(0x144);
    gfx_state.field_0x364 = 0;
    gfx_state.send_mask = gfx_state.used_lists;
  }
  listbuf_bank_select(fb_pair_index);
}
