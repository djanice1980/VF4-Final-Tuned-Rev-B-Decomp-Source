/*
 * sdk/naomi2_library/naomi2_library.c
 * One object (0x0C100A00-0x0C1019DF) of the library whose build string is
 * "NAOMI2 LIBRARY Ver 0.03" (the whole library is 0x0C0FBEE0-0x0C1041A0); the
 * object's file name is unknown, so this file keeps the library's name.
 * Built with the common flags.
 * Reconstructed C: every function here matches the game byte for byte
 * (python3 -I tools/match/match.py checks the whole tree).
 */
#define NOPROTO_modvol_shadow_set_up
#define NOPROTO_dummy_tri_link_on_lists
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_height; /* 0x0c32a700 */
extern float proj_rect_px_left; /* 0x0c32a704 */
extern float proj_rect_px_top; /* 0x0c32a708 */
extern model_error_rec model_error_record; /* 0x0c3dc158 */
extern elan_matrix_param_rec elan_matrix_params; /* 0x0c3dc188 */
extern graphics_work_rec graphics_work; /* 0x0c5d4800 */
extern tile_split_args_rec tile_split_args; /* 0x0c3dc1d4 */
extern render_config_block render_config; /* 0x0c417a60 */

static inline void sq_pref(void *a)
{
  __asm__ __volatile__ ("pref @%0" : : "r" (a));
}

void proj_cmd_queue(int list)
{
  float m[16];
  list_block_ref dl;
  float a;
  float b;
  uint addr;
  uint addr_hi;
  uint *p;
  elan_proj_cmd_rec *cmd;

  elan_matrix_params.near = graphics_work.near_z;
  elan_matrix_params.far = graphics_work.far_z;
  mtx_store_wordwise(m);
  a = -m[8];
  b = -m[9];
  addr = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = addr >> 0x18;
  *(volatile uint *)0xff00003c = addr >> 0x18;
  p = (uint *)(addr & 0x3ffffff | 0xe0000000);
  addr_hi = addr & 0xfc000000;
  dl.addr = (uint)p;
  dl.length = 0;
  cmd = (elan_proj_cmd_rec *)p;
  cmd->cmd = 0x08000300;
  cmd->field_0x4 = 0;
  cmd->scale_x = -proj_rect_px_width * m[0] * 0.5;
  cmd->offset_x = a * (proj_rect_px_width * 0.5) + proj_rect_px_width * 0.5 + proj_rect_px_left;
  cmd->scale_y = -proj_rect_px_height * m[5] * 0.5;
  cmd->offset_y = proj_rect_px_height * 0.5 - b * (proj_rect_px_height * 0.5) + proj_rect_px_top;
  cmd->field_0x18 = 0;
  cmd->field_0x1c = 0;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  elan_matrix_params.neg_inv_near = -1.0 / elan_matrix_params.near;
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  dl.length = (int)p - dl.addr;
  dl.addr = dl.addr & 0x3ffffff | addr_hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | addr_hi);
  elan_list_block_queue(list,&dl);
}

void proj_cmds_send(void)
{
  list_block_ref buf;
  uint addr;
  uint addr_hi;
  uint *p;
  uint *target;
  uint mode;
  uint hdr;
  int bits;
  int i;

  if (elan_list_enable_mask & 1) proj_cmd_queue(0);
  if (elan_list_enable_mask & 2) proj_cmd_queue(1);
  if (elan_list_enable_mask & 4) proj_cmd_queue(2);
  if (elan_list_enable_mask & 8) proj_cmd_queue(3);
  if (elan_list_enable_mask & 0x10) proj_cmd_queue(4);
  if (elan_revision == 0x12) return;
  if (last_link_cmd != 0) {
    target = (uint *)(last_link_cmd[4] & 0x1fffff8 | 0xaa000000);
    if ((*target >> 8 & 0xf) == 5) {
      mode = target[0x10] & 0x30000;
    }
    else {
      mode = *target & 0x30000;
    }
    mode ^= *last_link_cmd & 0x30000;
  }
  else {
    mode = 0;
  }
  dummy_tri_link_on_lists(mode);
  bits = list_end_extra_mask | elan_list_enable_mask;
  for (i = 0; i < 5; i++) {
    if (bits & 1) {
      addr = (uint)elan_cmd_write_ptr;
      *(uint *)0xff000038 = addr >> 0x18;
      *(uint *)0xff00003c = addr >> 0x18;
      p = (uint *)(addr & 0x3ffffff | 0xe0000000);
      addr_hi = addr & 0xfc000000;
      buf.addr = (uint)p;
      buf.length = 0;
      hdr = i << 0x18;
      if (i & 1) hdr |= mode | 0x81800040;
      else hdr |= mode | 0xa0800000;
      p[0] = hdr;
      p[1] = 0xe1000000;
      p[2] = 0x00800440;
      p[3] = 0;
      p[4] = 0x00ff0000;
      p[5] = 0x00ff0000;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x08000000;
      p[2] = 0x08000000;
      p[4] = 0x08000000;
      p[6] = 0x08000000;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      buf.length = (int)p - buf.addr;
      buf.addr = buf.addr & 0x3ffffff | addr_hi;
      elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | addr_hi);
      elan_list_block_queue(i,&buf);
    }
    bits >>= 1;
  }
}

char * naomi2_lib_version_get(void)
{
  return naomi2_lib_version_string;
}

void naomi2_lib_start(uint mode, uint flags, uint count)
{
  if ((*(uint *)0xa05f74b0 & 0xf0) == 0xb0) {
    short_line_timing = 1;
  }
  elan_revision = *(volatile uint *)0xa8800004;
  model_error_record.error_bits = 0;
  graphics_bring_up_entry(mode,flags,count);
  render_config.flags = render_config.flags | 1;
  elan_probe();
  lights_elan_state_init();
  ((int (*)())normal_block_scale_reset)();
  elan_cmd_write_ptr = 0;
  elan_ram_top = 0;
  elan_cmd_half_size = 0x100000;
  elan_lists_started = 0;
  tile_split_args.kind = 0;
  tile_split_args.paired = 1;
  tile_split_args.y1 = 0;
  tile_split_args.x1 = 0;
  tile_split_args.y0 = 0;
  tile_split_args.x0 = 0;
  elan_list_enable_mask = 0x1f;
  last_link_cmd = 0;
  list_end_extra_mask = 0;
  modvol_two_volume = 0;
  obj_flag_overrides_reset();
  return;
}

void tile_split_args_keep(uint kind, int paired, uint x0, uint y0, uint x1, uint y1)
{
  tile_split_args.kind = kind;
  tile_split_args.paired = paired;
  tile_split_args.x0 = x0;
  tile_split_args.y0 = y0;
  tile_split_args.x1 = x1;
  tile_split_args.y1 = y1;
  if (elan_lists_started != '\0') {
    tile_split_select(kind,paired,x0,y0,x1,y1);
  }
  return;
}

int render_config_from_list_sizes(int list0_size, int list1_size, int list2_size, int list3_size, int list4_size)
{
  render_config_block *cfg;
  int total;
  int r;

  cfg = &render_config;
  cfg->bypass_list = 0;
  if (render_frames_in_flight == 3) {
    total = (list0_size + list1_size + list2_size + list3_size + list4_size) * 2;
  }
  else {
    total = list0_size + list1_size + list2_size + list3_size + list4_size;
  }
  cfg->vertex_area_size = total;
  if (render_frames_in_flight == 3) {
    total = total / 2;
    cfg->list_shares[0] = (float)list0_size * 100.0 / (float)total;
    cfg->list_shares[1] = (float)list1_size * 100.0 / (float)total;
    cfg->list_shares[2] = (float)list2_size * 100.0 / (float)total;
    cfg->list_shares[3] = (float)list3_size * 100.0 / (float)total;
    cfg->list_shares[4] = (float)list4_size * 100.0 / (float)total;
  }
  else {
    cfg->list_shares[0] = (float)list0_size * 100.0 / (float)total;
    cfg->list_shares[1] = (float)list1_size * 100.0 / (float)total;
    cfg->list_shares[2] = (float)list2_size * 100.0 / (float)total;
    cfg->list_shares[3] = (float)list3_size * 100.0 / (float)total;
    cfg->list_shares[4] = (float)list4_size * 100.0 / (float)total;
  }
  r = render_buffers_lay_out(&render_config);
  if (r != 0) {
    __asm__ volatile ("trapa #0x41");
  }
  *(uint *)vertex_buffer_desc.header = 0x00800000;
  if (builtin_texture_loaded == 0) {
    tex_builtin_8x8_upload();
    builtin_texture_loaded = 1;
  }
  return r;
}

void elan_lists_start(int list0_size, int list1_size, int list2_size, int list3_size, int list4_size)
{
  int block;
  int total;
  
  total = list0_size + list1_size + list2_size + list3_size + list4_size;
  if (list0_size < 0x10000) {
    list0_size = 0x10000;
    total = list0_size + list1_size + list2_size + list3_size + list4_size;
  }
  elan_ram_region_take();
  elan_list0_size = list0_size;
  elan_list1_size = list1_size;
  elan_list2_size = list2_size;
  elan_list3_size = list3_size;
  elan_list4_size = list4_size;
  if (render_frames_in_flight == 2) {
    block = elan_ram_alloc_top(total * 4);
    ta_param_base_put(block);
  }
  else {
    block = elan_ram_alloc_top(total * 8);
    ta_param_base_put(block);
  }
  block = elan_ram_alloc_top(0x80);
  dummy_tri_never = dummy_tri_list_build((list_block_ref *)block,0);
  block = elan_ram_alloc_top(0x80);
  dummy_tri_inside = dummy_tri_list_build((list_block_ref *)block,1);
  block = elan_ram_alloc_top(0x80);
  dummy_tri_outside = dummy_tri_list_build((list_block_ref *)block,2);
  elan_ram_top_after_lists = elan_ram_top;
  elan_ram_top_after_lists2 = elan_ram_top;
  elan_cmd_half = 0;
  render_config_from_list_sizes(list0_size,list1_size,list2_size,list3_size,list4_size);
  elan_lists_started = 1;
  elan_reg_d0_value_put(6);
  tile_split_select(tile_split_args.kind,tile_split_args.paired,
               tile_split_args.x0,tile_split_args.y0,
               tile_split_args.x1,tile_split_args.y1);
  frame_lists_send_and_rearm();
}

void elan_frame_finish(void)
{
    elan_lists_end();
    elan_cmd_buffer_flip();
    render_with_busy_flag();
    last_link_cmd = 0;
    list_end_extra_mask = 0;
}

void elan_lists_end_with_clip(uint clip_x0, uint clip_y0, uint clip_x1, uint clip_y1, int extra_lists)
{
  uint *target;
  uint mode;
  uint addr;
  uint addr_hi;
  uint hdr;
  uint list_bits;
  uint *p;
  elan_tile_clip_cmd_rec *clip;
  int i;
  list_block_ref buf;

  if (last_link_cmd != 0) {
    target = (uint *)(last_link_cmd[4] & 0x1fffff8 | 0xaa000000);
    if ((*target >> 8 & 0xf) == 5) {
      mode = target[0x10] & 0x30000;
    }
    else {
      mode = *target & 0x30000;
    }
    mode = mode ^ *last_link_cmd & 0x30000;
  }
  else {
    mode = 0;
  }
  list_end_extra_mask = extra_lists;
  extra_lists = extra_lists | elan_list_enable_mask;
  for (i = 0; i < 5; i++) {
    if ((extra_lists & 1) != 0) {
      addr = (uint)elan_cmd_write_ptr;
      *(uint *)0xff000038 = addr >> 0x18;
      *(uint *)0xff00003c = addr >> 0x18;
      p = (uint *)(addr & 0x3ffffff | 0xe0000000);
      addr_hi = addr & 0xfc000000;
      buf.addr = (uint)p;
      buf.length = 0;
      list_bits = i << 0x18;
      if ((i & 1) != 0) {
        hdr = list_bits | mode | 0x81800040;
      }
      else {
        hdr = list_bits | mode | 0xa0800000;
      }
      p[0] = hdr;
      p[1] = 0xe1000000;
      p[2] = 0x800440;
      p[3] = 0;
      p[4] = 0xff0000;
      p[5] = 0xff0000;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      clip = (elan_tile_clip_cmd_rec *)p;
      clip->cmd = 0x20000000;
      clip->field_0x4 = 0;
      clip->field_0x8 = 0;
      clip->field_0xc = 0;
      clip->x0 = clip_x0;
      clip->y0 = clip_y0;
      clip->x1 = clip_x1;
      clip->y1 = clip_y1;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      p[0] = 0x8000000;
      p[2] = 0x8000000;
      p[4] = 0x8000000;
      p[6] = 0x8000000;
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
      p = p + 8;
      buf.length = (int)p - buf.addr;
      buf.addr = buf.addr & 0x3ffffff | addr_hi;
      elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | addr_hi);
      elan_list_block_queue(i,&buf);
    }
    extra_lists = extra_lists >> 1;
  }
  last_link_cmd = 0;
}

void elan_lists_end(void)
{
  list_block_ref buf;
  uint mode;
  uint word;
  int bits;
  int i;
  uint addr;
  uint addr_hi;
  uint *p;
  uint hdr;
  uint addr_lo;
  uint *link;
  elan_tile_clip_cmd_rec *clip;

  link = last_link_cmd;
  if (link != 0) {
    word = link[4] & 0x1fffff8 | 0xaa000000;
    if ((*(uint *)word >> 8 & 0xf) == 5) {
      word = ((uint *)word)[16];
    }
    else {
      word = *(uint *)word;
    }
    mode = (word & 0x30000) ^ (*last_link_cmd >> 16 & 3) << 16;
  }
  else {
    mode = 0;
  }
  bits = list_end_extra_mask | elan_list_enable_mask;
  for (i = 0; i < 5; i++) {
    if (bits & 1) {
      addr = (uint)elan_cmd_write_ptr;
      *(uint *)0xff000038 = addr >> 0x18;
      *(uint *)0xff00003c = addr >> 0x18;
      addr_lo = addr & 0x3ffffff;
      p = (uint *)(addr_lo | 0xe0000000);
      addr_hi = addr & 0xfc000000;
      buf.addr = (uint)p;
      buf.length = 0; /* redundant, but game code: buf.length is set again before elan_list_block_queue reads buf, but the game performs this store (mov.l r1,@(4,r14)). */
      hdr = i << 24;
      if (i & 1) hdr = hdr | mode | 0x81800040;
      else hdr = hdr | mode | 0xa0800000;
      p[0] = hdr;
      p[1] = 0xe1000000;
      p[2] = 0x00800440;
      p[3] = 0;
      p[4] = 0x00ff0000;
      p[5] = 0x00ff0000;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      p[0] = 0xf0000000;
      p[1] = 0xbf800000;
      p[2] = 0xbf800000;
      p[3] = 0x3dcccccd;
      p[4] = 0xbf800000;
      p[5] = 0x3f800000;
      p[6] = 0x3dcccccd;
      p[7] = 0xbdcccccd;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      p[0] = 0x3f800000;
      p[1] = 0x3dcccccd;
      p[2] = 0xbdcccccd;
      p[3] = 0xbf800000;
      p[4] = 0;
      p[5] = 0;
      p[6] = 0;
      p[7] = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      clip = (elan_tile_clip_cmd_rec *)p;
      clip->cmd = 0x20000000;
      clip->field_0x4 = 0;
      clip->field_0x8 = 0;
      clip->field_0xc = 0;
      clip->x0 = 0;
      clip->y0 = 0;
      clip->x1 = 0;
      clip->y1 = 0;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      p[0] = 0x08000000;
      p[2] = 0x08000000;
      p[4] = 0x08000000;
      p[6] = 0x08000000;
      list_end_block_count = list_end_block_count + 1 & 3;
      sq_pref(p);
      p = p + 8;
      buf.length = (int)p - buf.addr;
      buf.addr = buf.addr & 0x3ffffff | addr_hi;
      elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | addr_hi);
      elan_list_block_queue(i,&buf);
    }
    bits >>= 1;
  }
}

void elan_list_enable_mask_put(uint mask)
{
  elan_list_enable_mask = mask;
  return;
}

void modvol_mode_cheap_shadow(uint shadow_scale)
{
  modvol_two_volume = 0;
  modvol_shadow_set_up(shadow_scale);
  return;
}

void modvol_mode_two_volume(void)
{
  modvol_two_volume = 1;
  modvol_shadow_set_up(0xffffffff);
  return;
}

void pt_alpha_ref_write_both(uint alpha_ref)
{
  *(volatile uint *)0xa05f811c = alpha_ref;
  *(volatile uint *)0xa25f811c = alpha_ref;
  return;
}

void elan_regs_quiesce(void)
{
  (*(volatile uint *)0xa880000c) = 0;
  (*(volatile uint *)0xa8800010) = 0;
  (*(volatile uint *)0xa8800014) = 0x2029;
  (*(volatile uint *)0xa8800018) = 0x1f;
  (*(volatile uint *)0xa880001c) = 0x87320961;
  (*(volatile uint *)0xa8800030) = 0;
  (*(volatile uint *)0xa8800074) = 0x30;
  (*(volatile uint *)0xa8800078) = 0xc;
  (*(volatile uint *)0xa880007c) = 0;
  (*(volatile uint *)0xa8800080) = 4;
  (*(volatile uint *)0xa8800084) = 8;
  (*(volatile uint *)0xa8800088) = 0;
  (*(volatile uint *)0xa8800090) = 0x13e;
  (*(volatile uint *)0xa8800094) = 0x32000000;
  (*(volatile uint *)0xa88000a8) = 0;
  (*(volatile uint *)0xa25f6808) = 0;
  (*(volatile uint *)0xa25f6820) = 0;
  (*(volatile uint *)0xa25f6910) = 0;
  (*(volatile uint *)0xa25f6914) = 0;
  (*(volatile uint *)0xa25f6918) = 0;
  (*(volatile uint *)0xa25f6920) = 0;
  (*(volatile uint *)0xa25f6924) = 0;
  (*(volatile uint *)0xa25f6928) = 0;
  (*(volatile uint *)0xa25f6930) = 0;
  (*(volatile uint *)0xa25f6934) = 0;
  (*(volatile uint *)0xa25f6938) = 0;
  (*(volatile uint *)0xa25f6940) = 0;
  (*(volatile uint *)0xa25f6944) = 0;
  (*(volatile uint *)0xa25f6950) = 0;
  (*(volatile uint *)0xa25f6954) = 0;
  return;
}
