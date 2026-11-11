/*
 * sdk/kamui/list_buffers.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint strip_header_words[16]; /* 0x0c32a770 */
extern uint setup_steps_done; /* 0x0c32a910 */
extern uint ta_direct_mode; /* 0x0c32a914 */
extern uint list_cursor_slot; /* 0x0c41df8c */
extern uint list_buffer_starts; /* 0x0c5d5cf4 */
extern render_setup_rec render_setup; /* 0x0c41df94 */
extern list_write_rec list_write_state; /* 0x0c41e000 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern list_start_set bank_list_starts; /* 0x0c5d5cb8 */
extern list_cursor_set list_cursors; /* 0x0c5d5ce0 */
extern ta_param_area_pair ta_param_area; /* 0x0c5d5d08 */
extern list_header_count_set list_header_counts; /* 0x0c5d5d10 */

static inline uint slot_addr(uint o, uint b)
{
  o = o * 4 + b;
  return o;
}

static inline uint round_up32(uint x)
{
  x = x + 0x1f & 0xffffffe0;
  return x;
}

int listbuf_lay_out_four(uint *desc, int size0, int size1, int size2, int size3)
{
  int ret;
  uint i;
  uint sum;
  int used_count;
  int sizes[4];
  int bits[4];
  uint banks;
  uint opb_word;
  uint total_bytes;
  
  ret = 0;
  banks = 2;
  bits[0] = 0;
  bits[1] = 8;
  bits[2] = 0x10;
  bits[3] = 0x20;
  render_setup.step_kind = 1;
  if ((gfx_state.flags & 0x40) != 0) {
    size0 = size0 + 0x20;
  }
  render_setup.list_desc = (uint)desc;
  render_setup.list0_size = size0;
  render_setup.list1_size = size1;
  render_setup.list2_size = size2;
  render_setup.list3_size = size3;
  if (setup_steps_read() == -0x80000000) {
    used_count = used_lists_mask_set(size0,size1,size2,size3,0);
    desc[4] = gfx_state.used_lists;
    ta_direct_mode = 0;
    for (i = 0; i < 4; i++) {
      (&list_header_counts.list0)[i] = 0;
    }
    opb_word = opb_size_word_build(size0,size1,size2,size3,0,0);
    objlist_bytes_compute(opb_word);
    sum = size0 + size1 + size2 + size3;
    total_bytes = sum + used_count * 8 + 0xa0;
    sizes[0] = size0;
    sizes[1] = size1;
    sizes[2] = size2;
    sizes[3] = size3;
    if (sizes[0] != 0) {
      sizes[0] = sizes[0] - 0x10;
    }
    for (i = 1; i < 4; i++) {
      if (sizes[i] != 0) {
        sizes[i] = sizes[i] - 0x10;
      }
    }
    render_setup.list0_size = sizes[0];
    render_setup.list1_size = sizes[1];
    render_setup.list2_size = sizes[2];
    render_setup.list3_size = sizes[3];
    size0 = sizes[0];
    size1 = sizes[1];
    size2 = sizes[2];
    size3 = sizes[3];
    if (gfx_state.single_bank == 1) {
      banks = 1;
    }
    ret = listbuf_lists_carve(size0,size1,size2,size3,0,total_bytes,banks);
    if (ret != 0) return ret;
    {
      fb_size_from_display_mode(0xff,render_setup.fb_mode,render_setup.fb0_height);
      if (sum < 0x10000) {
        sum = 0x10000;
      }
      render_setup.param_area_size = sum * 4 + render_setup.objlist_bytes;
      if (render_setup.param_area_size > 0x200000U - render_setup.fb_bytes) {
        render_setup.param_area_size = 0x200000U - render_setup.fb_bytes;
      }
      ret = vram32_buffer_pair_alloc(render_setup.param_area_size,(int *)&ta_param_area,(int *)&ta_param_area.second_half);
      if (ret != 0) return ret;
      {
        gfx_state.ta_buf_size = render_setup.param_area_size;
        ta_buffers_lay_out((uint)gfx_state.area_offsets[0].region >> 7,(uint)gfx_state.area_offsets[0].objlist >> 7,
                     (uint)gfx_state.area_offsets[0].isp >> 7,(uint)gfx_state.area_offsets[0].tsp >> 7,
                     ~render_setup.fb_mode & opb_word,0);
        list_cursors.list0 = bank_list_starts.list0;
        list_cursors.list1 = bank_list_starts.list1;
        list_cursors.list2 = bank_list_starts.list2;
        list_cursors.list3 = bank_list_starts.list3;
        for (i = 0; i < 4; i++) {
          (&list_buffer_starts)[i] = (&list_cursors.list0)[i];
        }
        list_write_state.list_type = 0;
        list_write_state.header_bytes = 0;
        list_write_state.block_kind = 0;
        list_write_state.bank_parity = 0;
        list_starts = (uint *)&bank_list_starts;
        list_write_ptrs = (uint *)&list_cursors;
        for (i = 0; i < banks; i++) {
          bank_list_start_ptrs[i] = &bank_list_starts + i;
        }
        list_dummy_headers_write(banks);
        for (i = 0; i < 4; i++) {
          list_write_ptrs[i] = (&list_header_counts.list0)[i] * 4 + list_write_ptrs[i];
        }
        desc[0] = (uint)&list_cursors;
        desc[1] = (uint)strip_header_words;
        desc[2] = (uint)&list_write_state;
        desc[3] = (uint)bank_list_start_ptrs;
      }
    }
  }
  else {
    if (setup_steps_mark(0x100) == 0xb00) {
      setup_steps_store(0x80000000);
      ret = render_setup_finish(render_setup.step_kind);
    }
  }
  return ret;
}

void listbuf_bank_select(uint bank)
{
  uint i;
  
  if (gfx_state.single_bank != 1) {
    list_starts = (uint *)(&bank_list_starts + bank);
  }
  else {
    list_starts = (uint *)&bank_list_starts;
  }
  list_write_ptrs[0] = list_starts[0];
  list_write_ptrs[1] = list_starts[1];
  list_write_ptrs[2] = list_starts[2];
  list_write_ptrs[3] = list_starts[3];
  list_write_ptrs[4] = list_starts[4];
  for (i = 0; i < 5; i++) {
    (&list_buffer_starts)[i] = list_write_ptrs[i];
  }
  list_write_state.bank_parity = bank & 1;
  for (i = 0; i < 5; i++) {
    list_write_ptrs[i] += (&list_header_counts.list0)[i] * 4;
  }
  return;
}

int listbuf_list_transfer(int list)
{
  int bytes = list_write_ptrs[list] - (&list_buffer_starts)[list];

  gfx_state.list_dma_ticket = cmd_area_write_ticketed((&list_buffer_starts)[list], bytes & 0xfffffffc);
  list_write_ptrs[list] = (&list_header_counts.list0)[list] * 4 + (&list_buffer_starts)[list];
  return 0;
}

int listbuf_transfer_done(void)
{
  int done;
  
  done = ticket_done_poll(gfx_state.list_dma_ticket);
  if (done != 1) {
    return 0xd;
  }
  return 0;
}

int ta_direct_lay_out(int desc, uint size)
{
  int r;
  uint list_bits;
  uint used[4];
  uint opb_word;
  int z = 0;
  render_setup_rec *setup = &render_setup;

  setup->step_kind = 2;
  setup->list_desc = desc;
  setup->param_area_size = size;
  if (setup_steps_read() == 0x80000000) {
    ta_direct_mode = 1;
    gfx_state.ta_slot = gfx_state.frame_idx;
    gfx_state.slot_states[gfx_state.frame_idx] = 2;
    gfx_state.slot_split_words[gfx_state.frame_idx] = tile_split_word;
    gfx_state.ta_busy = z;
    gfx_state.frame_pending = 1;
    list_bits = *(uint *)(desc + 0x10);
    used[0] = list_bits & 1;
    used[1] = list_bits & 2;
    used[2] = list_bits & 4;
    used[3] = list_bits & 8;
    z = used_lists_mask_set(used[0],used[1],used[2],used[3],z); /* WORKAROUND (kept as possibly original; no game instruction of its own): z is not read after this call. Without 'z =' the call is unchanged, but GCC keeps the constant 1 in r11 and reuses it where the game loads it twice. Kept as plausible original code: a result kept and then not used in this build. */
    opb_word = opb_size_word_build(used[0],used[1],used[2],used[3],0,1);
    objlist_bytes_compute(opb_word);
    setup->param_area_size = setup->param_area_size >> 1;
    fb_size_from_display_mode(0xff,setup->fb_mode,setup->fb0_height);
    if (setup->param_area_size > 0x200000U - setup->fb_bytes) {
      setup->param_area_size = 0x200000U - setup->fb_bytes;
    }
    r = vram32_buffer_pair_alloc(render_setup.param_area_size,(int *)&ta_param_area,(int *)&ta_param_area.second_half);
    if (r != 0) return r;
    gfx_state.ta_buf_size = render_setup.param_area_size;
    ta_buffers_lay_out(gfx_state.area_offsets[0].region >> 7,gfx_state.area_offsets[0].objlist >> 7,gfx_state.area_offsets[0].isp >> 7,gfx_state.area_offsets[0].tsp >> 7,~render_setup.fb_mode & opb_word,0);
    gfx_state.ta_busy = 1;
  }
  else {
    if (setup_steps_mark(0x100) == 0xb00) {
      setup_steps_store(0x80000000);
      r = render_setup_finish(render_setup.step_kind);
    }
  }
  return r;
}

int ta_param_area_release(int unused)
{
  int r;
  int i;
  r = vram32_pair_free((*(uint *)((char *)&ta_param_area + 0)),ta_param_area.second_half);
  if (r != 0) return r;
  for (i = 0; i < 5; i++) (&list_header_counts.list0)[i] = 0;
  return 0;
}

int render_pipeline_choose(int single_bank, uint list_mode)
{
  gfx_state.vblank_ticks = 0;
  while (gfx_state.ta_busy != 0) {
    (*(void (*)(uint))gfx_state.hooks[7].fn)(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks > 10) break;
  }
  gfx_state.single_bank = single_bank;
  gfx_state.kept_list = list_mode;
  gfx_state.direct_list = list_mode & 0xf;
  gfx_state.slot_states[0] = 0;
  gfx_state.slot_states[1] = 0xffffffff;
  gfx_state.slot_states[2] = 0xffffffff;
  gfx_state.slot_states[3] = 0xffffffff;
  gfx_state.frame_idx = 0;
  gfx_state.render_slot = 0;
  gfx_state.ta_slot = 0;
  return 0;
}

int listbuf_list_used_words(int list)
{
  return ((int **)list_write_ptrs)[list] - ((int **)&list_buffer_starts)[list];
}

int listbuf_list_seek(int list, int words)
{
  list_write_ptrs[list] = (&list_buffer_starts)[list] + words * 4;
  return 0;
}

int listbuf_list_put_word(int list, uint word, int count)
{
  *(int **)&list_cursor_slot = (int *)(list_write_ptrs + list);
  copy_by_pairs_or_words(**(uint ***)&list_cursor_slot,&word,4);
  **(int **)&list_cursor_slot = count * 4 + **(int **)&list_cursor_slot;
  return 0;
}

int render_buffers_lay_out(render_config_block *cfg)
{
  float pct[5];
  uint size[5];
  uint bits[5];
  int addr;
  int len;
  uint fb_mode;
  uint fb_clear;
  uint flag20;
  uint banks;
  float sum;
  uint i;
  uint total;
  int max;
  uint v;
  uint room;
  uint mask;
  int r;
  int desc;

  sum = 0.0;
  banks = 2;
  i = 0;
  vram_pool_setup();
  render_drain();
  render_setup.step_kind = 0;
  v = cfg->flags;
  fb_clear = v & 1;
  if (v & 0x20) {
    flag20 = i;
  }
  else {
    flag20 = 1;
  }
  if (v & 2) {
    fb_mode = 1;
  }
  else {
    fb_mode = 0;
  }
  if (v & 0x100000) {
    gfx_state.upload_wait = i;
  }
  else {
    gfx_state.upload_wait = 1;
  }
  fb_size_from_display_mode(0xff,fb_mode,cfg->field_0x14);
  if ((cfg->flags & 0x10000) == 0) {
    ta_direct_mode = 0;
    if (cfg->flags & 0x10) {
      gfx_state.single_bank = 1;
      gfx_state.upload_wait = 1;
    }
    else {
      gfx_state.single_bank = 0;
    }
    render_pipeline_choose(gfx_state.single_bank,cfg->bypass_list);
    room = cfg->vertex_area_size;
    gfx_state.vertex_buf_base = cfg->vertex_area_base;
    if ((gfx_state.vertex_buf_base & 0x1f) != 0) {
      room = room + (gfx_state.vertex_buf_base & 0x1f) - 0x20;
      gfx_state.vertex_buf_base = gfx_state.vertex_buf_base + 0x1f & 0xffffffe0;
    }
    if (gfx_state.single_bank == 0) {
      room = room >> 1;
    }
    for (i = 0; i < 5; i++) {
      size[i] = 0;
    }
    for (i = 0; i < 5; i++) {
      sum = sum + cfg->list_shares[i];
    }
    if (sum > 100.0) {
      for (i = 0; i < 5; i++) {
        pct[i] = cfg->list_shares[i] * 100.0 / sum;
      }
    }
    else {
      for (i = 0; i < 5; i++) {
        pct[i] = cfg->list_shares[i];
      }
    }
    for (i = 0; i < 5; i++) {
      size[i] = __fixunssfsi((float)room * pct[i] / 100.0) & 0xfffffff8;
    }
    if ((gfx_state.flags & 0x40) != 0 && size[0] < 0x38) {
      size[0] = size[0] + 0x38;
    }
    {
      uint idx;

      total = 0;
      max = -1;
      for (i = 0; i < 5; i++) {
        total = total + size[i];
        if ((int)size[i] > max) {
          idx = i;
          max = size[i];
        }
      }
      if (total > room) {
        total = total - room;
        size[idx] = size[idx] - total & 0xffffffe0;
      }
    }
    if (size[0] != 0) {
      size[0] = size[0] - 0x10;
    }
    for (i = 1; i < 5; i++) {
      if (size[i] != 0) {
        size[i] = size[i] - 0x10;
      }
    }
    render_setup.list_desc = cfg->vertex_buf_desc;
    render_setup.list0_size = size[0];
    render_setup.list1_size = size[1];
    render_setup.list2_size = size[2];
    render_setup.list3_size = size[3];
    render_setup.list4_size = size[4];
    used_lists_mask_set(size[0],size[1],size[2],size[3],size[4]);
    ((uint *)cfg->vertex_buf_desc)[4] = gfx_state.used_lists;
    for (i = 0; i < 5; i++) {
      (&list_header_counts.list0)[i] = 0;
    }
    mask = opb_size_word_build(size[0],size[1],size[2],size[3],size[4],0);
    objlist_bytes_compute(mask);
    if (gfx_state.single_bank == 1) {
      banks = 1;
    }
    listbuf_lists_carve(size[0],size[1],size[2],size[3],size[4],0,banks);
    *(list_start_set *)&list_cursors = bank_list_starts;
    for (i = 0; i < 5; i++) {
      (&list_buffer_starts)[i] = (&list_cursors.list0)[i];
    }
    list_write_state.list_type = 0;
    list_write_state.header_bytes = 0;
    list_write_state.block_kind = 0;
    list_write_state.bank_parity = 0;
    list_starts = (uint *)&bank_list_starts;
    list_write_ptrs = (uint *)&list_cursors;
    for (i = 0; i < banks; i++) {
      bank_list_start_ptrs[i] = &bank_list_starts + i;
    }
    list_dummy_headers_write(banks);
    for (i = 0; i < 5; i++) {
      list_write_ptrs[i] = list_write_ptrs[i] + (&list_header_counts.list0)[i] * 4;
    }
  }
  else {
    gfx_state.upload_wait = 1;
    ta_direct_mode = 1;
    gfx_state.ta_slot = gfx_state.frame_idx;
    gfx_state.slot_states[gfx_state.frame_idx] = 2;
    gfx_state.slot_split_words[gfx_state.frame_idx] = tile_split_word;
    gfx_state.ta_busy = 0;
    gfx_state.frame_pending = 1;
    desc = cfg->vertex_buf_desc;
    bits[0] = *(uint *)(desc + 0x10) & 1;
    bits[1] = *(uint *)(desc + 0x10) & 2;
    bits[2] = *(uint *)(desc + 0x10) & 4;
    bits[3] = *(uint *)(desc + 0x10) & 8;
    bits[4] = *(uint *)(desc + 0x10) & 0x10;
    used_lists_mask_set(bits[0],bits[1],bits[2],bits[3],bits[4]);
    mask = opb_size_word_build(bits[0],bits[1],bits[2],bits[3],bits[4],1);
    objlist_bytes_compute(mask);
  }
  render_setup.field_0x5c = cfg->texmem_size;
  if ((render_setup.field_0x5c & 7) != 0) {
    render_setup.field_0x5c = render_setup.field_0x5c + 7 & 0xfffffff8;
  }
  render_setup.param_area_size = gfx_state.vram_size - (render_setup.field_0x5c * 2 + render_setup.fb_bytes) * 2 >> 1;
  if (cfg_setting_or_default("HARDWARE","EnableTexture16MB",0) == 1) {
    if (render_setup.param_area_size > 0x400000) {
      render_setup.param_area_size = 0x400000;
    }
  }
  else if (render_setup.param_area_size > 0x200000) {
    render_setup.param_area_size = 0x200000;
  }
  if (render_setup.param_area_size < render_setup.objlist_bytes) {
    return 3;
  }
  r = vram32_buffer_pair_alloc(render_setup.param_area_size,&addr,(uint *)&len);
  if (r != 0) {
    return r;
  }
  gfx_state.ta_buf_size = render_setup.param_area_size;
  ta_buffers_lay_out(gfx_state.area_offsets[0].region >> 7,gfx_state.area_offsets[0].objlist >> 7,gfx_state.area_offsets[0].isp >> 7,gfx_state.area_offsets[0].tsp >> 7,~fb_mode & mask,0);
  if ((cfg->flags & 0x10000) != 0) {
    gfx_state.ta_busy = 1;
  }
  ((uint *)cfg->vertex_buf_desc)[0] = (uint)&list_cursors;
  ((uint *)cfg->vertex_buf_desc)[1] = (uint)strip_header_words;
  ((uint *)cfg->vertex_buf_desc)[2] = (uint)&list_write_state;
  ((uint *)cfg->vertex_buf_desc)[3] = (uint)bank_list_start_ptrs;
  render_setup.fb_mode = fb_mode;
  gfx_state.fb_sof_hold = fb_mode;
  render_setup.fb0_surface = ((uint *)cfg->frame_surfaces)[0];
  render_setup.field_0x20 = cfg->field_0x10;
  render_setup.fb0_height = cfg->field_0x14;
  render_setup.fb0_clear = fb_clear;
  render_setup.fb1_surface = ((uint *)cfg->frame_surfaces)[1];
  render_setup.field_0x30 = cfg->field_0x10;
  render_setup.fb1_height = cfg->field_0x14;
  render_setup.fb1_clear = fb_clear;
  render_setup.front_surface = ((uint *)cfg->frame_surfaces)[0];
  render_setup.back_surface = ((uint *)cfg->frame_surfaces)[1];
  render_setup.field_0x44 = flag20;
  setup_steps_store(0x80000000);
  return render_setup_finish(render_setup.step_kind);
}

int used_lists_mask_set(int size0, int size1, int size2, int size3, int size4)
{
  int used_count;
  uint i;
  int direct[5];
  used_count = 0;
  for (i = 0; i < 5; i++) direct[i] = 0;
  if (gfx_state.single_bank == 1) {
    if (gfx_state.kept_list == 0x20) direct[0] = 1;
    if (gfx_state.kept_list == 0x21) direct[1] = 1;
    if (gfx_state.kept_list == 0x22) direct[2] = 1;
    if (gfx_state.kept_list == 0x23) direct[3] = 1;
    if (gfx_state.kept_list == 0x24) direct[4] = 1;
  }
  size0 += direct[0];
  size1 += direct[1];
  size2 += direct[2];
  size3 += direct[3];
  size4 += direct[4];
  gfx_state.used_lists = 0;
  if (size0 != 0) { gfx_state.used_lists |= 1; used_count++; }
  if (size1 != 0) { gfx_state.used_lists |= 2; used_count++; }
  if (size2 != 0) { gfx_state.used_lists |= 4; used_count++; }
  if (size3 != 0) { gfx_state.used_lists |= 8; used_count++; }
  if (size4 != 0) { gfx_state.used_lists |= 0x10; used_count++; }
  return used_count;
}

uint opb_size_word_build(uint size0, int size1, uint size2, int size3, int size4, int direct)
{
  int tbl[4] = {0, 8, 16, 32};
  uint r = gfx_state.list_ctrl & 0xff;
  int i5 = 0;
  int u4 = 0;
  int i2 = 0;
  int u3 = 0;
  int i1 = 0;

  if (size1 != 0) u3 = 1;
  if (size3 != 0) u4 = 1;
  if (size4 != 0) i5 = 2;
  if (direct == 1) {
    if (size0 != 0) i1 = 2;
    if (size2 != 0) i2 = 2;
  }
  else {
    if (size0 != 0) i1 = 3;
    if (size2 != 0) i2 = 3;
    if ((size0 != 0) & (size2 != 0)) {
      if (size0 < size2) i1 = 2;
      else i2 = 2;
    }
  }
  if (gfx_state.single_bank == 1) {
    if (gfx_state.kept_list == 0x20) i1 = 3;
    if (gfx_state.kept_list == 0x21) u3 = 2;
    if (gfx_state.kept_list == 0x22) i2 = 3;
    if (gfx_state.kept_list == 0x23) u4 = 2;
    if (gfx_state.kept_list == 0x24) i5 = 3;
  }
  r |= i1 << 17 | u3 << 19 | i2 << 21 | u4 << 23 | i5 << 25;
  r |= 0x10000;
  if ((gfx_state.flags & 0x20) != 0) {
    gfx_state.double_regions = ((u3 != 0) & (i2 != 0)) && i5 != 0;
  }
  return r;
}

void list_dummy_headers_write(uint banks)
{
  uint i;
  uint buf[8];
  
  buf[0] = 0x80000000;
  buf[1] = 0;
  buf[2] = 0x800400;
  buf[3] = 0;
  buf[4] = 0xffffffff;
  buf[5] = 0xffffffff;
  buf[6] = 0xffffffff;
  buf[7] = 0xffffffff;
  if ((gfx_state.used_lists & 1) != 0) {
    if (gfx_state.single_bank == 0 || gfx_state.kept_list != 0x20) {
      for (i = 0; i < banks; i++) {
        list_start_set *tab = &bank_list_starts;
        copy_words((uint *)tab[i].list0,buf,0x20);
      }
      list_header_counts.list0 = 8;
    }
  }
  if ((gfx_state.used_lists & 2) != 0) {
    buf[0] = 0x81000000;
    if (gfx_state.single_bank == 0 || gfx_state.kept_list != 0x21) {
      for (i = 0; i < banks; i++) {
        list_start_set *tab = &bank_list_starts;
        copy_words((uint *)tab[i].list1,buf,0x20);
      }
      list_header_counts.list1 = 8;
    }
  }
  if ((gfx_state.used_lists & 4) != 0) {
    buf[0] = 0x82000000;
    if (gfx_state.single_bank == 0 || gfx_state.kept_list != 0x22) {
      for (i = 0; i < banks; i++) {
        list_start_set *tab = &bank_list_starts;
        copy_words((uint *)tab[i].list2,buf,0x20);
      }
      list_header_counts.list2 = 8;
    }
  }
  if ((gfx_state.used_lists & 8) != 0) {
    buf[0] = 0x83000000;
    if (gfx_state.single_bank == 0 || gfx_state.kept_list != 0x23) {
      for (i = 0; i < banks; i++) {
        list_start_set *tab = &bank_list_starts;
        copy_words((uint *)tab[i].list3,buf,0x20);
      }
      list_header_counts.list3 = 8;
    }
  }
  if ((gfx_state.used_lists & 16) != 0) {
    buf[0] = 0x84000000;
    if (gfx_state.single_bank == 0 || gfx_state.kept_list != 0x24) {
      for (i = 0; i < banks; i++) {
        list_start_set *tab = &bank_list_starts;
        copy_words((uint *)tab[i].list4,buf,0x20);
      }
      list_header_counts.list4 = 8;
    }
  }
  return;
}

void objlist_bytes_compute(uint opb_word)
{
  int tiles;
  uint a;
  uint b;
  uint c;
  uint d;
  uint e;
  int words[4];

  words[0] = 0;
  words[1] = 8;
  words[2] = 0x10;
  words[3] = 0x20;
  a = opb_word >> 0x11 & 3;
  b = opb_word >> 0x13 & 3;
  c = opb_word >> 0x15 & 3;
  d = opb_word >> 0x17 & 3;
  e = opb_word >> 0x19 & 3;
  tiles = gfx_state.tile_cols * gfx_state.tile_rows;
  if (gfx_state.flags & 0x20) {
    if (gfx_state.double_regions != 0) {
      tiles = tiles * 4;
    }
    else {
      tiles = tiles * 2;
    }
  }
  render_setup.objlist_bytes = ((words[a] + words[b] + words[c] + words[d] + words[e]) * 4 + 0x18) * tiles;
}

int listbuf_lists_carve(int size0, int size1, int size2, int size3, int size4, uint unused, uint banks)
{
  uint i;
  uint off;
  uint base[2];
  uint end;
  off = 0;
  gfx_state.list_limits[0] = size0 << 2;
  gfx_state.list_limits[1] = size1 << 2;
  gfx_state.list_limits[2] = size2 << 2;
  gfx_state.list_limits[3] = size3 << 2;
  gfx_state.list_limits[4] = size4 << 2;
  base[0] = gfx_state.vertex_buf_base;
  for (i = 0; i < banks; i++) {
    if ((base[i] & 0x1f) != 0) base[i] = round_up32(base[i]);
    if (gfx_state.single_bank == 1 && gfx_state.kept_list == 0x20) {
      (&bank_list_starts)[i].list0 = 0x09000000;
    }
    else if (size0 != 0) {
      (&bank_list_starts)[i].list0 = base[i] + off * 4;
      off = off + size0 + 0x17 & 0xfffffff8;
    }
    else {
      (&bank_list_starts)[i].list0 = 0;
    }
    if (gfx_state.single_bank == 1 && gfx_state.kept_list == 0x21) {
      (&bank_list_starts)[i].list1 = 0x09000000;
    }
    else if (size1 != 0) {
      (&bank_list_starts)[i].list1 = base[i] + off * 4;
      off = off + size1 + 0x17 & 0xfffffff8;
    }
    else {
      (&bank_list_starts)[i].list1 = 0;
    }
    if (gfx_state.single_bank == 1 && gfx_state.kept_list == 0x22) {
      (&bank_list_starts)[i].list2 = 0x09000000;
    }
    else if (size2 != 0) {
      (&bank_list_starts)[i].list2 = base[i] + off * 4;
      off = off + size2 + 0x17 & 0xfffffff8;
    }
    else {
      (&bank_list_starts)[i].list2 = 0;
    }
    if (gfx_state.single_bank == 1 && gfx_state.kept_list == 0x23) {
      (&bank_list_starts)[i].list3 = 0x09000000;
    }
    else if (size3 != 0) {
      (&bank_list_starts)[i].list3 = base[i] + off * 4;
      off = off + size3 + 0x17 & 0xfffffff8;
    }
    else {
      (&bank_list_starts)[i].list3 = 0;
    }
    if (gfx_state.single_bank == 1 && gfx_state.kept_list == 0x24) {
      (&bank_list_starts)[i].list4 = 0x09000000;
    }
    else if (size4 != 0) {
      (&bank_list_starts)[i].list4 = base[i] + off * 4;
      off = off + size4 + 0x17 & 0xfffffff8;
    }
    else {
      (&bank_list_starts)[i].list4 = 0;
    }
    end = slot_addr(off, base[0]);
    base[1] = end;
    off = 0;
  }
  return 0;
}

int render_setup_finish(int step_kind)
{
  int r;
  uint i;
  uint banks;
  uint bank;

  banks = 2;
  if (step_kind == 1) {
    r = listbuf_lay_out_four((uint *)render_setup.list_desc,render_setup.list0_size,
                     render_setup.list1_size,render_setup.list2_size,
                     render_setup.list3_size);
    if (r != 0) return r;
  }
  if (step_kind == 2) {
    r = ta_direct_lay_out(render_setup.list_desc,render_setup.param_area_size);
    if (r != 0) return r;
  }
  if (render_setup.fb0_surface == render_setup.fb1_surface) {
    banks = 1;
  }
  for (i = 0; i < banks; i++) {
    int *b = (int *)&render_setup;
    r = fb_vram_alloc((surface_rec *)(b + i * 4)[7],(b + i * 4)[8],(b + i * 4)[9],b[6],(b + i * 4)[10]);
    if (r != 0) return r;
  }
  r = fb_pair_install((surface_rec *)render_setup.front_surface,(surface_rec *)render_setup.back_surface,
                   render_setup.fb_mode,render_setup.field_0x44);
  if (r != 0) return r;
  if ((gfx_state.single_bank == 1 || ta_direct_mode == 1)) {
    gfx_state.ta_slot = gfx_state.frame_idx;
    gfx_state.slot_states[gfx_state.frame_idx] = 0;
    gfx_state.slot_split_words[gfx_state.frame_idx] = tile_split_word;
    gfx_state.frame_pending = 1;
    xfer_wait_idle();
    bank = gfx_state.frame_idx & 1;
    pvr_reg_write_both(8,1);
    pvr_reg_write_both(8,0);
    pvr_reg_write_both(0x124,gfx_state.ta_bufs[bank].ol_base);
    pvr_reg_write_both(300,gfx_state.ta_bufs[bank].ol_limit);
    pvr_reg_write_both(0x128,gfx_state.ta_bufs[bank].isp_base);
    pvr_reg_write_both(0x130,gfx_state.ta_bufs[bank].isp_limit);
    pvr_reg_write_both(0x164,gfx_state.ta_bufs[bank].opb_start);
    pvr_reg_write_both(0x144,0x80000000);
    pvr_reg_read(0x144);
    gfx_state.field_0x364 = 0;
    gfx_state.send_mask = gfx_state.used_lists;
  }
  return r;
}

uint setup_steps_mark(uint bits)
{
  setup_steps_done = bits | setup_steps_done;
  return setup_steps_done;
}

uint setup_steps_store(uint value)
{
  setup_steps_done = value;
  return value;
}

uint setup_steps_read(void)
{
  return setup_steps_done;
}

void render_drain(void)
{
  int i;

  gfx_state.vblank_ticks = 0;
  while (gfx_state.render_busy != 0) {
    gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
    if (gfx_state.vblank_ticks >= 11) break;
  }
  if (gfx_state.single_bank == 0) {
    gfx_state.vblank_ticks = 0;
    while (gfx_state.ta_busy != 0) {
      gfx_state.hooks[7].fn(gfx_state.hooks[7].arg);
      if (gfx_state.vblank_ticks >= 11) break;
    }
  }
  gfx_state.ta_busy = 0;
  gfx_state.render_busy = 0;
  for (i = 0; i < 4; i++) {
    if (i == gfx_state.frame_idx) {
      gfx_state.slot_states[i] = 0;
      gfx_state.slot_split_words[i] = tile_split_word;
    }
    else {
      gfx_state.slot_states[i] = -1;
    }
  }
  gfx_state.render_slot = 0;
  gfx_state.ta_slot = 0;
}
