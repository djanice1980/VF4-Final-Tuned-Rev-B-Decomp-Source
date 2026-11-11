/*
 * sdk/kamui/vram_alloc.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_packed_chunk_take_range_at
#define NOPROTO_vram_take_range_at
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

static inline void blk_set_owner(vram_node *node, int *owner, int slot, ushort kind)
{
  owner[slot] = node->start;
  node->owner = owner;
  node->flags = node->flags | kind;
}

int vram_lists_clear(void)
{
  vram_node *node;
  vram_node *next;
  int i;
  packed_chunk_rec *chunk;
  packed_chunk_rec *next_chunk;

  for (node = vram_halves[0].free_head; node != 0; node = next) {
    next = node->next;
    vram_node_return(node);
  }
  for (node = vram_halves[0].used_head; node != 0; node = next) {
    next = node->next;
    vram_node_return(node);
  }
  for (node = vram_halves[1].free_head; node != 0; node = next) {
    next = node->next;
    vram_node_return(node);
  }
  for (node = vram_halves[1].used_head; node != 0; node = next) {
    next = node->next;
    vram_node_return(node);
  }
  for (i = 0; i < 0x1000; i++) {
    vram_node_pool[i].flags = 0;
  }
  vram_halves[0].used_head = 0;
  vram_halves[0].used_tail = 0;
  vram_halves[0].free_head = 0;
  vram_halves[0].free_tail = 0;
  vram_halves[0].free_bytes = 0;
  vram_halves[1].used_head = 0;
  vram_halves[1].used_tail = 0;
  vram_halves[1].free_head = 0;
  vram_halves[1].free_tail = 0;
  vram_halves[1].free_bytes = 0;
  for (chunk = packed_chunk_list; chunk != 0; chunk = next_chunk) {
    next_chunk = chunk->next;
    packed_chunk_record_return(chunk);
  }
  packed_chunk_list = 0;
  return 0;
}

int vram_pool_setup(void)
{
  int enabled;
  vram_node *node;
  int half;

  if (vram_one_area_mode == 1) {
    enabled = cfg_setting_or_default("HARDWARE","EnableTexture16MB",0);
    if (enabled == 1) {
      int size = 0x1000000;
      gfx_state.vram_size = size;
      vram_half_size = size;
      vram_half0_start = 0;
      vram_half0_size = size;
      vram_half1_start = size;
      vram_half1_size = 0;
    }
    else {
      int size = 0x800000;
      gfx_state.vram_size = size;
      vram_half_size = size;
      vram_half0_start = 0;
      vram_half0_size = size;
      vram_half1_start = size;
      vram_half1_size = 0;
    }
    pvr_reg_write_both(0xe4,0);
    vram_lists_clear();
    vram_halves[0].free_bytes = vram_half0_size;
    vram_halves[1].free_bytes = vram_half1_size;
    node = vram_node_take();
    if (node == 0) {
      return 3;
    }
    node->start = vram_half0_start;
    node->size = vram_half0_size;
    node->next = vram_halves[0].free_head;
    vram_halves[0].free_head = node;
    node->prev = vram_halves[0].free_tail;
    vram_halves[0].free_tail = node;
  }
  else {
    enabled = cfg_setting_or_default("HARDWARE","EnableTexture16MB",0);
    if (enabled == 1) {
      gfx_state.vram_size = 0x1000000;
      half = 0x800000;
      vram_half_size = half;
      vram_half0_start = 0;
      vram_half0_size = half;
      vram_half1_start = half;
      vram_half1_size = half;
    }
    else {
      gfx_state.vram_size = 0x800000;
      half = 0x400000;
      vram_half_size = half;
      vram_half0_start = 0;
      vram_half0_size = half;
      vram_half1_start = half;
      vram_half1_size = half;
    }
    pvr_reg_write_both(0xe4,0);
    vram_lists_clear();
    vram_halves[0].free_bytes = vram_half0_size;
    vram_halves[1].free_bytes = vram_half1_size;
    node = vram_node_take();
    if (node == 0) {
      return 3;
    }
    node->start = vram_half0_start;
    node->size = vram_half0_size;
    node->next = vram_halves[0].free_head;
    vram_halves[0].free_head = node;
    node->prev = vram_halves[0].free_tail;
    vram_halves[0].free_tail = node;
    node = vram_node_take();
    if (node == 0) {
      return 3;
    }
    node->start = vram_half1_start;
    node->size = vram_half1_size;
    node->next = vram_halves[1].free_head;
    vram_halves[1].free_head = node;
    node->prev = vram_halves[1].free_tail;
    vram_halves[1].free_tail = node;
  }
  return 0;
}

int fb_surface_alloc_paired(int size, surface_rec *surface)
{
  int r;

  if ((size & 0x1f) != 0) {
    size = (size / 32) * 32 + 32;
  }
  if (fb_pair_spare_size != size) {
    if ((gfx_state.flags & 4) != 0 &&
        vram_halves[0].free_bytes < vram_halves[1].free_bytes) {
      r = vram_take_first_fit_buffer(1,size * 2,surface,8);
    }
    else {
      r = vram_take_first_fit_buffer(0,size * 2,surface,8);
    }
    if (r == 0) {
      fb_pair_spare_size = size;
      fb_pair_spare_block = surface->tex_address;
      { int block = surface->tex_address;
      surface->bytes = size;
      surface->tex_address = vram64_to_32_offset(block); }
    }
    return r;
  }
  surface->bytes = size;
  surface->tex_address = vram64_to_32_offset(fb_pair_spare_block + 4);
  fb_pair_spare_size = 0;
  fb_pair_spare_block = 0;
  return 0;
}

int fb_surfaces_alloc_together(uint size, surface_rec *surface_a, surface_rec *surface_b)
{
  int r;
  int block;

  if (size & 0x1f)
    size = ((int)size / 32) * 32 + 32;
  if ((gfx_state.flags & 4) && vram_halves[0].free_bytes <= vram_halves[1].free_bytes)
    r = vram_take_first_fit_buffer(1,size * 2,surface_a,8);
  else
    r = vram_take_first_fit_buffer(0,size * 2,surface_a,8);
  if (r == 0) {
    block = surface_a->tex_address;
    surface_a->bytes = size;
    surface_a->tex_address = vram64_to_32_offset(block);
    surface_b->bytes = size;
    surface_b->tex_address = vram64_to_32_offset(block + 4);
  }
  return r;
}

int vram32_buffer_pair_alloc(int size, int *buffer_a, int *buffer_b)
{
  int r;
  int block;
  if (size % 32 != 0) {
    size = (size / 32 + 1) * 32;
  }
  r = vram_take_first_fit_buffer(0,size * 2,buffer_a,0x2000);
  if (r == 0) {
    block = *buffer_a;
    *buffer_a = vram64_to_32_offset(block);
    *buffer_b = vram64_to_32_offset(block + 4);
  }
  return r;
}

int surface_vram_alloc(uint size, surface_rec *surface)
{
  int half;
  int half0_largest; int half1_largest;

  if ((gfx_state.flags & 4) != 0) {
    vram_largest_free_per_half(&half0_largest,&half1_largest);
    if (half0_largest >= half1_largest) {
      half = 0;
      goto out;
    }
  }
  half = 1;
out:
  return vram_take_page_aligned_fit(half,size,surface);
}

int surface_alloc_both_halves(uint size, surface_rec *surface_a, surface_rec *surface_b)
{
  int addr;
  int r;
  addr = vram_find_mirrored_offset_for_surface(size,surface_a);
  if (addr == -1) return 3;
  r = vram_take_range_at(0,addr,size,surface_a,1,1);
  if (r != 0) return r;
  r = vram_take_range_at(1,addr + vram_half0_size,size,surface_b,1,1);
  if (r != 0) {
    vram_free_at(surface_a->tex_address);
  }
  return r;
}

int surface_vram_take_at_addr(uint size, surface_rec *surface)
{
  int r;
  int addr = surface->tex_address;
  if (addr < vram_half1_start) {
    r = vram_take_range_at(0,addr,size,surface,0,1);
  }
  else {
    r = vram_take_range_at(1,addr,size,surface,0,1);
  }
  return r;
}

int surface_alloc_with_shadow(uint size, surface_rec *surface)
{
  vram_node *shadow;
  int addr;
  int r;
  surface_rec dummy;
  if ((size & 0x1f) != 0) {
    size = ((int)size / 32) * 32 + 32;
  }
  shadow = vram_find_shadow_for(0,size,surface);
  if (shadow == 0) {
    shadow = vram_find_shadow_for(1,size,surface);
  }
  if (shadow != 0) {
    surface->tex_address = shadow->start;
    shadow->owner = surface;
    shadow->flags = shadow->flags | 4;
    return 0;
  }
  addr = vram_find_mirrored_offset_for_surface(size,surface);
  if (addr == -1) return 3;
  r = vram_take_range_at(0,addr,size,surface,1,1);
  if (r != 0) return r;
  return vram_take_range_at(1,addr + vram_half0_size,size,&dummy,1,0);
}

int packed_tex_alloc(surface_rec *surface)
{
  packed_class_rec info;
  int half0_largest;
  int half1_largest;
  packed_chunk_rec *chunk;
  packed_chunk_rec *found;
  uint i;
  uint slot;
  int r;
  uint chunk_bytes;

  packed_tex_class(&info,surface);
  found = 0;
  chunk = packed_chunk_list;
  while (chunk != 0) {
    if ((chunk->flags & 0xf000) == info.class_mark) {
      for (i = 0; i < info.slot_count; i++) {
        if (chunk->slots[i] == 0) {
          found = chunk;
          slot = i;
          i = info.slot_count;
          chunk = 0;
        }
      }
    }
    if (found == 0) chunk = chunk->next;
  }
  if (found == 0) {
    found = packed_chunk_record_take();
    if (found == 0) return 3;
    for (i = 0; i < 16; i++) found->slots[i] = 0;
    slot = 0;
    chunk_list_push(&packed_chunk_list,found);
    found->flags |= info.class_mark;
    chunk_bytes = info.chunk_bytes;
    if ((gfx_state.flags & 4) == 0 || (vram_largest_free_per_half(&half0_largest,&half1_largest), half0_largest < half1_largest))
      r = packed_chunk_vram_take(1,chunk_bytes,found);
    else
      r = packed_chunk_vram_take(0,chunk_bytes,found);
    if (r != 0) {
      packed_chunk_record_return(found);
      return 3;
    }
  }
  surface->tex_address = found->base + slot * (info.stride >> 2) * 4;
  found->slots[slot] = surface;
  found->flags |= 2;
  return 0;
}

int vram_free_at(int addr)
{
  vram_node *node;
  vram_node *mirror;
  int mirror_addr;
  short half[2];

  node = vram_used_node_at(addr,half);
  if (node == 0) return 7;
  if (node->flags & 2) {
    if (half[0] == 0) mirror_addr = addr + vram_half0_size;
    else mirror_addr = addr - vram_half0_size;
    mirror = vram_used_node_at(mirror_addr,&half[1]);
    if (mirror == 0) {
      vram_node_release((int)half[0],node);
      return 0;
    }
    if (mirror->flags & 4) {
      node->flags = node->flags & 0xfffb;
      return 0;
    }
    vram_node_release((int)half[0],node);
    return vram_node_release((int)half[1],mirror);
  }
  return vram_node_release((int)half[0],node);
}

int vram32_pair_free(uint addr_a, uint addr_b)
{
  uint addr;
  int r;

  addr = vram32_to_64_offset(addr_a);
  r = vram_free_at(addr);
  if (r != 0) {
    addr = vram32_to_64_offset(addr_b);
    r = vram_free_at(addr);
  }
  return r;
}

int packed_tex_free(surface_rec *surface)
{
  packed_class_rec info;
  packed_chunk_rec *found;
  packed_chunk_rec *chunk;
  uint i;
  uint slot;
  int any;
  int r;
  packed_tex_class(&info, surface);
  chunk = packed_chunk_list;
  found = 0;
  while (chunk != 0) {
    if ((chunk->flags & 0xf000) == info.class_mark) {
      for (i = 0; i < info.slot_count; i++) {
        if (chunk->slots[i] != 0 && chunk->base + i * (info.stride >> 2) * 4 == surface->tex_address) {
          found = chunk;
          slot = i;
          i = info.slot_count;
          chunk = 0;
        }
      }
    }
    if (found == 0) {
      chunk = chunk->next;
    }
  }
  if (found != 0) {
  found->slots[slot] = 0;
  any = 0;
  for (i = 0; i < info.slot_count; i++) {
    if (found->slots[i] != 0) {
      any = 1;
    }
  }
  if (!any) {
    r = vram_free_at(found->base);
    if (r != 0) {
      return r;
    }
    chunk_list_remove(&packed_chunk_list, found);
    packed_chunk_record_return(found);
  }
  return 0;
  }
  return 7;
}

int vram_free_space_report(int *free_bytes0, int *free_bytes1, int *packed_free_bytes, int *largest)
{
  vram_node *node;
  packed_chunk_rec *chunk;
  int packed_free, largest0, largest1, free0, free1, reserved0, reserved1;
  int slot_bytes;
  int slot_count;
  int i;

  reserved1 = 0; reserved0 = 0; free1 = 0; free0 = 0; largest1 = 0; largest0 = 0; packed_free = 0;

  for (node = vram_halves[0].free_head; node != 0; node = node->next) {
    if (node->size > largest0) {
      largest0 = node->size;
    }
    free0 += node->size;
  }
  for (node = vram_halves[0].used_head; node != 0; node = node->next) {
    if ((node->flags & 4) == 0) {
      if (node->size > largest0) {
        largest0 = node->size;
      }
      reserved0 += node->size;
    }
  }
  for (node = vram_halves[1].free_head; node != 0; node = node->next) {
    if (node->size > largest1) {
      largest1 = node->size;
    }
    free1 += node->size;
  }
  for (node = vram_halves[1].used_head; node != 0; node = node->next) {
    if ((node->flags & 4) == 0) {
      if (node->size > largest1) {
        largest1 = node->size;
      }
      reserved1 += node->size;
    }
  }
  for (chunk = packed_chunk_list; chunk != 0; chunk = chunk->next) {
    switch (chunk->flags & 0xf000) {
    case 0x1000: slot_bytes = 0x90; slot_count = 16; break;
    case 0x2000: slot_bytes = 0xa0; slot_count = 16; break;
    case 0x3000: slot_bytes = 0xc0; slot_count = 16; break;
    case 0x4000: slot_bytes = 0xe0; slot_count = 16; break;
    case 0x5000: slot_bytes = 0x200; slot_count = 8; break;
    case 0x6000: slot_bytes = 0x360; slot_count = 4; break;
    case 0x7000: slot_bytes = 0x800; slot_count = 2; break;
    case 0x8000: slot_bytes = 0xd60; slot_count = 1; break;
    default: slot_bytes = 0; slot_count = 0; break;
    }
    for (i = 0; i < slot_count; i++) {
      if (chunk->slots[i] == 0) {
        packed_free += slot_bytes;
      }
    }
  }
  *free_bytes0 = free0 + reserved0;
  *free_bytes1 = free1 + reserved1;
  *packed_free_bytes = packed_free;
  *largest = (largest0 >= largest1) ? largest0 : largest1;
  return 0;
}

int vram_largest_free_per_half(int *largest0, int *largest1)
{
  vram_node *node;
  int largest_1;
  int largest_0;
  largest_1 = 0;
  largest_0 = 0;
  for (node = vram_halves[0].free_head; node != 0; node = node->next) {
    if (node->size > largest_0) largest_0 = node->size;
  }
  for (node = vram_halves[0].used_head; node != 0; node = node->next) {
    if ((node->flags & 4) == 0 && node->size > largest_0) largest_0 = node->size;
  }
  for (node = vram_halves[1].free_head; node != 0; node = node->next) {
    if (node->size > largest_1) largest_1 = node->size;
  }
  for (node = vram_halves[1].used_head; node != 0; node = node->next) {
    if ((node->flags & 4) == 0 && node->size > largest_1) largest_1 = node->size;
  }
  *largest0 = largest_0;
  *largest1 = largest_1;
  return 0;
}

int vram_lists_sort(void)
{
  node_list_sort_by_start(&vram_halves[0].used_head,&vram_halves[0].used_tail);
  node_list_sort_by_start(&vram_halves[1].used_head,&vram_halves[1].used_tail);
  node_list_sort_by_start(&vram_halves[0].free_head,&vram_halves[0].free_tail);
  node_list_sort_by_start(&vram_halves[1].free_head,&vram_halves[1].free_tail);
  return 0;
}

int vram_compact_halves(void)
{
  int r;

  r = vram_half_compact(&vram_halves[0].free_head, &vram_halves[0].free_tail, &vram_halves[0].used_head, &vram_halves[0].used_tail);
  if (r != 0) return r;
  return vram_half_compact(&vram_halves[1].free_head, &vram_halves[1].free_tail, &vram_halves[1].used_head, &vram_halves[1].used_tail);
}

uint vram_page_fit_start(vram_node *node, uint size, surface_rec *surface)
{
  uint page;
  uint end;
  uint lead;
  uint before;

  page = (node->start + 0x800) & 0xfffff800;
  end = node->start + node->size;
  if (size <= 0x2b0) {
    if (page >= end) return node->start;
    if (page - node->start >= size) return node->start;
    if (end - page >= size) return page;
    return 0xffffffff;
  }
  before = page - node->start;
  if (before > 0x2af) lead = before;
  else {
    page += 0x800;
    lead = page - node->start;
  }
  if (lead + size - 0x2b0 <= end) return page - 0x2b0;
  return 0xffffffff;
}

int vram_find_mirrored_offset_for_surface(uint size, surface_rec *surface)
{
  int rounded;
  vram_node *node;
  int last;
  int addr;
  int fallback;
  int mode;
  int page_addr;

  fallback = 0;
  mode = 0;
  rounded = size;
  if ((rounded & 0x1f) != 0) {
    rounded = rounded / 32 * 32 + 32;
  }
  for (node = vram_halves[0].free_head; node != 0; node = node->next) {
    if (node->size >= rounded) {
      last = (node->size - rounded) + node->start;
      if ((surface->format_flags & 0x29) == 1) {
        page_addr = vram_page_fit_start(node,rounded,surface);
        if (page_addr >= 0) {
          for (addr = page_addr; addr <= last; addr += 0x800) {
            if (vram_range_is_free(1,addr + vram_half0_size,rounded) == 0) return addr;
          }
        }
        mode = 2;
        if (fallback == 0) {
          for (addr = node->start; addr <= last; addr += 0x20) {
            if (vram_range_is_free(1,addr + vram_half0_size,rounded) == 0) {
              fallback = addr;
              addr = last;
            }
          }
        }
      }
      else {
        mode = 1;
        for (addr = node->start; addr <= last; addr += 0x20) {
          if (vram_range_is_free(1,addr + vram_half0_size,rounded) == 0) return addr;
        }
      }
    }
  }
  if (mode == 2) {
    if (fallback != 0) return fallback;
  }
  return -1;
}

int vram_range_is_free(int half, int addr, int size)
{
  short h = half;
  int end;
  vram_node *node;
  vram_node *found;
  int start;
  int stop;

  end = addr + size;
  node = vram_halves[h].free_head;
  found = 0;
  while (node != 0) {
    start = node->start;
    if (start <= addr) {
      stop = start + node->size;
      if (stop > addr) {
        if (start <= end) {
          if (stop >= end) {
            found = node;
            node = 0;
            continue;
          }
        }
      }
    }
    node = node->next;
  }
  if (found == 0) return 3;
  return 0;
}

int vram_take_range_at(half, addr, size, surface, linked, live) short half; int addr; uint size; surface_rec *surface; int linked; int live;
{
  int end;
  vram_node *node;
  vram_node *hold;
  vram_node *taken;
  vram_node *rest;
  int old_size;
  int head_size;

  end = addr + size;
  if ((size & 0x1f) != 0) size = (int)size / 32 * 32 + 32;
  hold = 0;
  node = vram_halves[half].free_head;
  while (node != 0) {
    if (node->start <= addr && node->start + node->size > addr && node->start <= end && node->start + node->size >= end) {
      hold = node;
      node = 0;
    }
    else {
      node = node->next;
    }
  }
  if (hold == 0) return 3;
  if (hold->size == size) {
    node_list_unlink(&vram_halves[half].free_head,&vram_halves[half].free_tail,hold);
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,hold);
    surface->tex_address = hold->start;
    hold->owner = surface;
    hold->flags = 1;
    if (half == 0) hold->flags |= 0x80;
    else hold->flags |= 0x100;
    if (linked != 0) hold->flags |= 2;
    if (live != 0) hold->flags |= 4;
    vram_halves[half].free_bytes -= hold->size;
    return 0;
  }
  if (hold->start == addr) {
    taken = vram_node_take();
    if (taken == 0) return 3;
    taken->start = hold->start;
    taken->size = size;
    head_size = hold->size - size;
    hold->size = head_size;
    hold->start += size;
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,taken);
  }
  else if (hold->start + hold->size == end) {
    taken = vram_node_take();
    if (taken == 0) return 3;
    head_size = hold->size - size;
    hold->size = head_size;
    taken->start = addr;
    taken->size = size;
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,taken);
  }
  else {
    taken = vram_node_take();
    if (taken == 0) return 3;
    rest = vram_node_take();
    if (rest == 0) {
      vram_node_return(taken);
      return 3;
    }
    old_size = hold->size;
    head_size = addr - hold->start;
    hold->size = head_size;
    taken->start = addr;
    taken->size = size;
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,taken);
    rest->start = addr + size;
    rest->size = old_size - (head_size + size);
    node_list_insert_after(&vram_halves[half].free_tail,rest,vram_halves[half].free_tail);
  }
  surface->tex_address = taken->start;
  taken->owner = surface;
  if (half == 0) taken->flags |= 0x80;
  else taken->flags |= 0x100;
  if (linked != 0) taken->flags |= 2;
  if (live != 0) taken->flags |= 4;
  vram_halves[half].free_bytes -= taken->size;
  return 0;
}

int vram_take_first_fit_buffer(short half,int size,void *owner,short kind)
{
  vram_node *node;
  vram_node *best;
  vram_node *taken;


  if ((size & 0x1f) != 0) {
    size = (size / 32) * 32 + 32;
  }
  best = 0;
  for (node = vram_halves[half].free_head; node != 0; ) {
    if (node->size >= size) {
      best = node;
      node = 0;
    }
    else {
      node = node->next;
    }
  }
  if (best == 0) return 3;
  if (best->size == size) {
    node_list_unlink(&vram_halves[half].free_head,&vram_halves[half].free_tail,best);
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,best);
    best->flags = 1;
    switch (kind) {
    case 8:
      blk_set_owner(best, owner, 7, 8);
      break;
    case 0x2000:
      blk_set_owner(best, owner, 0, 0x2000);
      break;
    default:
      return 3;
    }
    best->flags = best->flags | 4;
    vram_halves[half].free_bytes = vram_halves[half].free_bytes - best->size;
  }
  else {
    taken = vram_node_take();
    if (taken == 0) return 3;
    taken->start = best->start;
    taken->size = size;
    best->size = best->size - size;
    best->start = best->start + size;
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,taken);
    switch (kind) {
    case 8:
      blk_set_owner(taken, owner, 7, 8);
      break;
    case 0x2000:
      blk_set_owner(taken, owner, 0, 0x2000);
      break;
    default:
      return 3;
    }
    taken->flags = taken->flags | 4;
    vram_halves[half].free_bytes = vram_halves[half].free_bytes - taken->size;
  }
  return 0;
}

int vram_take_first_fit(int half_no, uint size, surface_rec *surface)
{
  int half = (short)half_no;
  vram_node *node;
  vram_node *best;
  vram_node *taken;

  if ((size & 0x1f) != 0) {
    size = ((int)size / 32) * 32 + 32;
  }
  node = vram_halves[half].free_head;
  best = 0;
  while (node != 0) {
    if (node->size >= (int)size) {
      best = node;
      node = 0;
    }
    else {
      node = node->next;
    }
  }
  if (best == 0) return 3;
  if (best->size == (int)size) {
    node_list_unlink(&vram_halves[(short)half].free_head, &vram_halves[(short)half].free_tail, best);
    node_list_push_front(&vram_halves[(short)half].used_head, &vram_halves[(short)half].used_tail, best);
    surface->tex_address = best->start;
    best->owner = surface;
    best->flags = 1;
    if (half == 0) {
      best->flags = 0x21;
    }
    else {
      best->flags |= 0x40;
    }
    best->flags |= 4;
    vram_halves[half].free_bytes -= best->size;
  }
  else {
    taken = vram_node_take();
    if (taken == 0) return 3;
    taken->start = best->start;
    taken->size = (int)size;
    best->size -= (int)size;
    best->start += (int)size;
    node_list_push_front(&vram_halves[(short)half].used_head, &vram_halves[(short)half].used_tail, taken);
    surface->tex_address = taken->start;
    taken->owner = surface;
    if (half == 0) {
      taken->flags |= 0x20;
    }
    else {
      taken->flags |= 0x40;
    }
    taken->flags |= 4;
    vram_halves[half].free_bytes -= taken->size;
  }
  return 0;
}

int vram_take_page_aligned_fit(int half_no, uint size, surface_rec *surface)
{
  int half = (short)half_no;
  vram_node *node;
  vram_node *best;
  vram_node *first;
  vram_node *taken;
  int mode;
  int aligned;
  int end;

  mode = 0;
  if ((size & 0x1f) != 0) {
    size = ((int)size / 32) * 32 + 32;
  }
  first = 0;
  node = vram_halves[half].free_head;
  best = 0;
  while (node != 0) {
    if (node->size >= (int)size) {
      if (first == 0) first = node;
      if ((node->start & 0x7ff) != 0) {
        aligned = (node->start + 0x800) & ~0x7ff;
        end = node->start + node->size;
        if (aligned >= end) {
          mode = 1;
          best = node;
          node = 0;
        }
        else if (aligned - node->start >= (int)size) {
          mode = 1;
          best = node;
          node = 0;
        }
        else if (end - aligned < (int)size) {
          node = node->next;
        }
        else {
          mode = 2;
          best = node;
          node = 0;
        }
      }
      else {
        aligned = node->start; /* redundant, but game code: aligned is not read after this arm (mode 1 never reaches the split), but the game performs this load (mov.l @(12,r3),r7). */
        mode = 1;
        best = node;
        node = 0;
      }
    }
    else {
      node = node->next;
    }
  }
  if (mode == 0 && first != 0) {
    mode = 1;
    best = first;
  }
  if (best == 0) return 3;
  if (mode == 2) return vram_take_range_at(half,aligned,size,surface,0,1);
  if (best->size == (int)size) {
    node_list_unlink(&vram_halves[(short)half].free_head, &vram_halves[(short)half].free_tail, best);
    node_list_push_front(&vram_halves[(short)half].used_head, &vram_halves[(short)half].used_tail, best);
    surface->tex_address = best->start;
    best->owner = surface;
    best->flags = 1;
    if (half == 0) {
      best->flags = 0x21;
    }
    else {
      best->flags |= 0x40;
    }
    best->flags |= 4;
    vram_halves[half].free_bytes -= best->size;
  }
  else {
    taken = vram_node_take();
    if (taken == 0) return 3;
    taken->start = best->start;
    taken->size = (int)size;
    best->size -= (int)size;
    best->start += (int)size;
    node_list_push_front(&vram_halves[(short)half].used_head, &vram_halves[(short)half].used_tail, taken);
    surface->tex_address = taken->start;
    taken->owner = surface;
    if (half == 0) {
      taken->flags |= 0x20;
    }
    else {
      taken->flags |= 0x40;
    }
    taken->flags |= 4;
    vram_halves[half].free_bytes -= taken->size;
  }
  return 0;
}

int packed_chunk_vram_take(int half_no, uint size, packed_chunk_rec *chunk)
{
  int half = (short)half_no;
  vram_node *node;
  vram_node *best;
  vram_node *first;
  vram_node *taken;
  int mode;
  int aligned;
  int end;

  mode = 0;
  if ((size & 0x1f) != 0) {
    size = ((int)size / 32) * 32 + 32;
  }
  first = 0;
  node = vram_halves[half].free_head;
  best = 0;
  while (node != 0) {
    if (node->size >= (int)size) {
      if (first == 0) first = node;
      if ((node->start & 0x7ff) != 0) {
        aligned = (node->start + 0x800) & ~0x7ff;
        end = node->start + node->size;
        if (aligned >= end) {
          mode = 1;
          best = node;
          node = 0;
        }
        else if (aligned - node->start >= (int)size) {
          mode = 1;
          best = node;
          node = 0;
        }
        else if (end - aligned < (int)size) {
          node = node->next;
        }
        else {
          mode = 2;
          best = node;
          node = 0;
        }
      }
      else {
        aligned = node->start; /* redundant, but game code: aligned is not read after this arm (mode 1 never reaches the split), but the game performs this load (mov.l @(12,r3),r7). */
        mode = 1;
        best = node;
        node = 0;
      }
    }
    else {
      node = node->next;
    }
  }
  if (mode == 0 && first != 0) {
    mode = 1;
    best = first;
  }
  if (best == 0) return 3;
  if (mode == 2) return packed_chunk_take_range_at(half,aligned,size,chunk,0,1);
  if (best->size == (int)size) {
    node_list_unlink(&vram_halves[(short)half].free_head, &vram_halves[(short)half].free_tail, best);
    node_list_push_front(&vram_halves[(short)half].used_head, &vram_halves[(short)half].used_tail, best);
    chunk->base = best->start;
    best->owner = chunk;
    best->flags = 1;
    if (half == 0) {
      best->flags = 0x201;
    }
    else {
      best->flags |= 0x400;
    }
    best->flags |= 4;
    vram_halves[half].free_bytes -= best->size;
  }
  else {
    taken = vram_node_take();
    if (taken == 0) return 3;
    taken->start = best->start;
    taken->size = (int)size;
    best->size -= (int)size;
    best->start += (int)size;
    node_list_push_front(&vram_halves[(short)half].used_head, &vram_halves[(short)half].used_tail, taken);
    chunk->base = taken->start;
    taken->owner = chunk;
    if (half == 0) {
      taken->flags |= 0x200;
    }
    else {
      taken->flags |= 0x400;
    }
    taken->flags |= 4;
    vram_halves[half].free_bytes -= taken->size;
  }
  return 0;
}

vram_node * vram_find_shadow_for(int half_no, int size, surface_rec *surface)
{
  short half = half_no;
  vram_node *node;
  int limit = vram_half_size;
  vram_node *best = 0;
  vram_node *mirror;
  int mirror_addr;
  short mirror_half[2];
  for (node = vram_halves[half].used_head; node != 0; node = node->next) {
    if (((*(uint *)&node->flags & 6) == 2) && ((*(uint *)&node->flags & 0x180) != 0) && (node->size >= size) && (node->size < limit)) {
      if (half == 0) {
        mirror_addr = node->start + vram_half0_size;
      }
      else {
        mirror_addr = node->start - vram_half0_size;
      }
      mirror = vram_used_node_at(mirror_addr,mirror_half);
      if ((mirror != 0) && (((surface_rec *)mirror->owner)->format_flags == surface->format_flags)) {
        best = node;
        limit = node->size;
      }
    }
  }
  return best;
}

vram_node * vram_used_node_at(int addr, short *half)
{
  vram_node *node;
  vram_node *found;

  if (addr < vram_half1_start) *half = 0; else *half = 1;
  found = 0;
  node = vram_halves[(short)*half].used_head;
  while (node != 0) {
    if (node->start == addr) {
      found = node;
      node = 0;
    }
    else {
      node = node->next;
    }
  }
  return found;
}

int vram_node_release(int half_no, vram_node *node)
{
  vram_node *merged = node;
  int half = (short)half_no;
  vram_node *scan;
  vram_node *neighbor;

  merged->flags &= ~4;
  node_list_unlink(&vram_halves[(short)half].used_head, &vram_halves[(short)half].used_tail, merged);
  vram_halves[half].free_bytes += merged->size;
  neighbor = 0;
  scan = vram_halves[half].free_head;
  while (scan != 0) {
    if (scan->start == merged->start + merged->size || merged->start == scan->size + scan->start) {
      neighbor = scan;
      scan = 0;
    }
    else {
      scan = scan->next;
    }
  }
  while (neighbor != 0) {
    if (neighbor->start == merged->start + merged->size) {
      neighbor->start = merged->start;
      neighbor->size = neighbor->size + merged->size;
    }
    else {
      neighbor->size = neighbor->size + merged->size;
    }
    vram_node_return(merged);
    {
      vram_node **heads = &vram_halves->free_head;
      node_list_unlink(&heads[(short)half * 5], &vram_halves[(short)half].free_tail, neighbor);
    }
    merged = neighbor;
    neighbor = 0;
    scan = vram_halves[half].free_head;
    while (scan != 0) {
      if (scan->start == merged->start + merged->size || merged->start == scan->size + scan->start) {
        neighbor = scan;
        scan = 0;
      }
      else {
        scan = scan->next;
      }
    }
  }
  node_list_push_front(&vram_halves[(short)half].free_head, &vram_halves[(short)half].free_tail, merged);
  return 0;
}

vram_node * vram_node_take(void)
{
  int i;
  vram_node *node;

  for (i = 0; i < 0x1000; i++) {
    vram_node *pool = vram_node_pool;
    node = &pool[i];
    if ((node->flags & 1) == 0) {
      node->flags = 1;
      return node;
    }
  }
  return 0;
}

void vram_node_return(vram_node *node)
{
  int i;
  vram_node *entry;
  for (i = 0; i < 0x1000; i++) {
    entry = &vram_node_pool[i];
    if (entry == node) {
      entry->flags = entry->flags & 0xfffe;
    }
  }
}

void node_list_push_front(vram_node **head, vram_node **tail, vram_node *node)
{
  if (*head == 0) {
    node->prev = 0;
    node->next = 0;
    *head = node;
    *tail = node;
  }
  else {
    vram_node *first = *head;
    node->prev = first->prev;
    node->next = *head;
    first->prev = node;
    *head = node;
  }
}

void node_list_insert_after(vram_node **tail, vram_node *node, vram_node *after)
{
  vram_node *next;

  next = after->next;
  if (next == 0) {
    *tail = node;
  }
  else {
    /* ORIGINAL SHAPE (corroborated 2026-11-10: node_list_push_front above reads *head a second time in the same place, and there the second read is forced): the else branch reads the link again; delay-slot filling deletes that load as redundant and moves the branch to a new label, which is why the game leaves this label unaligned (a label that survives machine reorg always gets the 32-byte alignment) */
    next = after->next;
    next->prev = node;
  }
  node->prev = after;
  node->next = next;
  after->next = node;
  return;
}

void node_list_unlink(vram_node **head, vram_node **tail, vram_node *node)
{
  vram_node *prev = node->prev;
  vram_node *next = node->next;
  if (prev == 0) {
    *head = next;
  }
  else {
    prev->next = node->next;
  }
  if (node->next == 0) {
    *tail = node->prev;
  }
  else {
    next->prev = node->prev;
  }
}

void vram_block_copy(int dst, int src, uint n)
{
  int i;
  char *line;
  char buf [2048];

  if (n > 0x800) {
    do {
      vram64_read((uint *)buf,src,0x800);
      for (line = buf, i = 0; i < 0x40; i++, line += 0x20) {
        ocbwb_line((int)line);
      }
      vram64_write(dst,(uint)buf,0x800);
      src += 0x800;
      dst += 0x800;
      n -= 0x800;
    } while (n > 0x800);
  }
  vram64_read((uint *)buf,src,n);
  for (line = buf, i = 0; i < 0x40; i++, line += 0x20) {
    ocbwb_line((int)line);
  }
  vram64_write(dst,(uint)buf,n);
}

void node_list_sort_by_start(vram_node **head, vram_node **tail)
{
  vram_node *node, *best, *first;
  int count, i, j, k;

  first = *head;
  count = 0;
  if (first == 0) return;
  best = first;
  node = first;
  while (node != 0) {
    if (node->start > best->start) best = node;
    node = node->next;
    count++;
  }
  if (count < 2) return;
  if (*tail != best) {
    node_list_unlink(head,tail,best);
    node_list_insert_after(tail,best,*tail);
  }
  count -= 2;
  for (i = 0, j = count; i < count; i++, j--) {
    best = node = *head;
    for (k = 0; k < j; k++) {
      node = node->next;
      if (node->start > best->start) best = node;
    }
    if (node != best) {
      node_list_unlink(head,tail,best);
      node_list_insert_after(tail,best,node);
    }
  }
}

int vram_half_compact(vram_node **free_head, vram_node **free_tail, vram_node **used_head, vram_node **used_tail)
{
  vram_node *gap;
  vram_node *node;
  vram_node *split;
  vram_node *next_gap;
  surface_rec *packed;
  int mode;
  uint flags;
  int i;
  uint moved;
  vram_node *prev;

  gap = *free_head;
  node = *used_head;
  if (gap == 0) {
    return 0;
  }
  if (node == 0) {
    return 0;
  }
  mode = 0;
  goto next;
fail:
  return 3;
next:
  flags = *(uint *)&node->flags;
  for (;;) {
    if ((flags & 0x60) != 0 || (flags & 0x82) == 0x80 || (flags & 0x102) == 0x100) {
      mode = 1;
      flags = *(uint *)&node->flags;
    }
    if ((flags & 0x600) != 0 || (flags & 0x802) == 0x800 || (flags & 0x1002) == 0x1000) {
      mode = 2;
    }
    if (mode != 0) {
      if (gap->start + gap->size == node->start) {
        if ((gap->start & 0x1f) != 0) {
          split = vram_node_take();
          if (split == 0) {
            goto fail;
          }
          split->start = gap->start;
          gap->start = (gap->start + 0x1f) & 0xffffffe0;
          split->size = gap->start - split->start;
          gap->size = gap->size - split->size;
          prev = gap->prev;
          if (prev == 0) {
            node_list_push_front(free_head, free_tail, split);
          }
          else {
            node_list_insert_after(free_tail, split, prev);
          }
        }
        vram_block_copy(gap->start, node->start, node->size);
        moved = node->start - gap->start;
        node->start = gap->start;
        gap->start = gap->start + node->size;
        if (mode == 1) {
          ((surface_rec *)node->owner)->tex_address = node->start;
        }
        else {
          ((packed_chunk_rec *)node->owner)->base = node->start;
          for (i = 0; i < 16; i++) {
            packed = ((packed_chunk_rec *)node->owner)->slots[i];
            if (packed != 0) {
              packed->tex_address = packed->tex_address - ((moved >> 2) << 2);
            }
          }
        }
        next_gap = gap->next;
        if (next_gap != 0 && gap->start + gap->size == next_gap->start) {
          gap->size = next_gap->size + gap->size;
          node_list_unlink(free_head, free_tail, next_gap);
          vram_node_return(next_gap);
        }
      }
      if (node == 0) {
        goto out;
      }
      while (gap->start > node->start) {
        node = node->next;
        if (node == 0) {
          goto out;
        }
      }
      if (node == 0) {
        goto out;
      }
    }
    else {
      if (gap == 0) {
        goto out;
      }
      while (gap->start < node->start) {
        gap = gap->next;
        if (gap == 0) {
          goto out;
        }
      }
      if (gap == 0) {
        goto out;
      }
      if (node == 0) {
        goto out;
      }
      while (gap->start > node->start) {
        node = node->next;
        if (node == 0) {
          goto out;
        }
      }
    }
    if (node == 0) {
      goto out;
    }
    if (gap == 0) {
      goto out;
    }
    mode = 0;
    flags = *(uint *)&node->flags;
  }
out:
  return 0;
}

int packed_chunk_take_range_at(half, addr, size, chunk, linked, live) short half; int addr; uint size; packed_chunk_rec *chunk; int linked; int live;
{
  int end;
  vram_node *node;
  vram_node *hold;
  vram_node *taken;
  vram_node *rest;
  int old_size;
  int head_size;

  end = addr + size;
  if ((size & 0x1f) != 0) size = (int)size / 32 * 32 + 32;
  hold = 0;
  node = vram_halves[half].free_head;
  while (node != 0) {
    if (node->start <= addr && node->start + node->size > addr && node->start <= end && node->start + node->size >= end) {
      hold = node;
      node = 0;
    }
    else {
      node = node->next;
    }
  }
  if (hold == 0) return 3;
  if (hold->size == size) {
    node_list_unlink(&vram_halves[half].free_head,&vram_halves[half].free_tail,hold);
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,hold);
    chunk->base = hold->start;
    hold->owner = chunk;
    hold->flags = 1;
    if (half == 0) hold->flags |= 0x800;
    else hold->flags |= 0x1000;
    if (linked != 0) hold->flags |= 2;
    if (live != 0) hold->flags |= 4;
    vram_halves[half].free_bytes -= hold->size;
    return 0;
  }
  if (hold->start == addr) {
    taken = vram_node_take();
    if (taken == 0) return 3;
    taken->start = hold->start;
    taken->size = size;
    head_size = hold->size - size;
    hold->size = head_size;
    hold->start += size;
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,taken);
  }
  else if (hold->start + hold->size == end) {
    taken = vram_node_take();
    if (taken == 0) return 3;
    head_size = hold->size - size;
    hold->size = head_size;
    taken->start = addr;
    taken->size = size;
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,taken);
  }
  else {
    taken = vram_node_take();
    if (taken == 0) return 3;
    rest = vram_node_take();
    if (rest == 0) {
      vram_node_return(taken);
      return 3;
    }
    old_size = hold->size;
    head_size = addr - hold->start;
    hold->size = head_size;
    taken->start = addr;
    taken->size = size;
    node_list_push_front(&vram_halves[half].used_head,&vram_halves[half].used_tail,taken);
    rest->start = addr + size;
    rest->size = old_size - (head_size + size);
    node_list_insert_after(&vram_halves[half].free_tail,rest,vram_halves[half].free_tail);
  }
  chunk->base = taken->start;
  taken->owner = chunk;
  if (half == 0) taken->flags |= 0x800;
  else taken->flags |= 0x1000;
  if (linked != 0) taken->flags |= 2;
  if (live != 0) taken->flags |= 4;
  vram_halves[half].free_bytes -= taken->size;
  return 0;
}

int packed_chunk_alloc_with_shadow(uint size, packed_chunk_rec *chunk)
{
  vram_node *shadow;
  int r;
  int addr;
  packed_chunk_rec dummy;

  if ((size & 0x1f) != 0) {
    size = (int)size / 32 * 32 + 32;
  }
  shadow = packed_chunk_find_shadow(0,size,chunk);
  if (shadow == 0) {
    shadow = packed_chunk_find_shadow(1,size,chunk);
  }
  if (shadow != 0) {
    chunk->base = shadow->start;
    shadow->owner = chunk;
    shadow->flags = shadow->flags | 4;
    return 0;
  }
  addr = vram_find_mirrored_offset(size);
  if (addr == -1) return 3;
  r = packed_chunk_take_range_at(0,addr,size,chunk,1,1);
  if (r != 0) return r;
  return packed_chunk_take_range_at(1,addr + vram_half0_size,size,&dummy,1,0);
}

int packed_tex_class(packed_class_rec *info, surface_rec *surface)
{
  switch (surface->width) {
  case 8:
    if ((surface->format_flags & 1) != 0) {
      info->class_mark = 0x2000;
      info->stride = 0x80;
      info->field_0x8 = 0x20;
      info->chunk_bytes = 4000;
    }
    else {
      info->class_mark = 0x1000;
      info->stride = 0x80;
      info->field_0x8 = 0x20;
      info->chunk_bytes = 0xf90;
    }
    break;
  case 0x10:
    if ((surface->format_flags & 1) != 0) {
      info->class_mark = 0x4000;
      info->stride = 0x80;
      info->field_0x8 = 0x60;
      info->chunk_bytes = 0xfe0;
    }
    else {
      info->class_mark = 0x3000;
      info->stride = 0x80;
      info->field_0x8 = 0x40;
      info->chunk_bytes = 0xfc0;
    }
    break;
  case 0x20:
    if ((surface->format_flags & 1) != 0) {
      info->class_mark = 0x6000;
      info->stride = 0x200;
      info->field_0x8 = 0x160;
      info->chunk_bytes = 0xf60;
    }
    else {
      info->class_mark = 0x5000;
      info->stride = 0x100;
      info->field_0x8 = 0x100;
      info->chunk_bytes = 0x1000;
    }
    break;
  case 0x40:
    if ((surface->format_flags & 1) != 0) {
      info->class_mark = 0x8000;
      info->stride = 0x800;
      info->field_0x8 = 0x560;
      info->chunk_bytes = 0xd60;
    }
    else {
      info->class_mark = 0x7000;
      info->stride = 0x400;
      info->field_0x8 = 0x400;
      info->chunk_bytes = 0x1000;
    }
    break;
  default:
    return 0;
  }
  info->slot_count = (uint)0x800 / info->stride;
  return 1;
}

packed_chunk_rec * packed_chunk_record_take(void)
{
  int i;
  int pool;
  packed_chunk_rec *chunk;
  i = 0;
  pool = (int)packed_chunk_pool;
  for (; i < 0x400; i++) {
    chunk = (packed_chunk_rec *)(i * 0x4c + pool);
    if ((chunk->flags & 1) == 0) {
      chunk->flags = 1;
      return chunk;
    }
  }
  return 0;
}

void packed_chunk_record_return(packed_chunk_rec *chunk)
{
  packed_chunk_rec *entry;
  int i;
  for (i = 0; i < 0x400; i++) {
    entry = &packed_chunk_pool[i];
    if (entry == chunk) {
      entry->flags = entry->flags & 0xfffe;
    }
  }
}

void chunk_list_push(packed_chunk_rec **head, packed_chunk_rec *chunk)
{
  if (*head == 0) chunk->next = 0;
  else chunk->next = *head;
  *head = chunk;
}

void chunk_list_remove(packed_chunk_rec **head, packed_chunk_rec *chunk)
{
  packed_chunk_rec *node;
  packed_chunk_rec *prev;

  prev = 0;
  for (node = *head; node != 0; ) {
    if (node->next == chunk) {
      prev = node;
      node = 0;
    }
    else {
      node = node->next;
    }
  }
  if (*head == chunk) {
    *head = chunk->next;
  }
  else {
    prev->next = chunk->next;
  }
  return;
}

vram_node * packed_chunk_find_shadow(int half_no, int size, packed_chunk_rec *chunk)
{
  short half = (short)half_no;
  vram_node *mirror;
  int mirror_addr;
  vram_node *node;
  vram_node *best;
  int limit;
  short mirror_half[2];
  limit = vram_half_size;
  best = 0;
  for (node = vram_halves[half].used_head; node != 0; node = node->next) {
    if (((*(uint *)&node->flags & 6) == 2) && ((*(uint *)&node->flags & 0x1800) != 0) && (node->size >= size) && (node->size < limit)) {
      if (half == 0) mirror_addr = node->start + vram_half0_size;
      else mirror_addr = node->start - vram_half0_size;
      mirror = vram_used_node_at(mirror_addr,mirror_half);
      if ((mirror != 0) && ((((packed_chunk_rec *)mirror->owner)->flags & 0xf000) == (chunk->flags & 0xf000))) {
        best = node;
        limit = node->size;
      }
    }
  }
  return best;
}

int vram_find_mirrored_offset(int size)
{
  vram_node *node;
  int addr;
  int last;
  if ((size & 0x1f) != 0) {
    size = (size / 32) * 32 + 32;
  }
  for (node = vram_halves[0].free_head; node != 0; node = node->next) {
    if (node->size >= size) {
      int start;
      int room = node->size - size;
      start = node->start;
      last = room + start;
      for (addr = start; addr <= last; addr += 0x20) {
        if (vram_range_is_free(1, addr + vram_half0_size, size) == 0)
          return addr;
      }
    }
  }
  return -1;
}

int vram64_to_32_offset(uint addr)
{
  int offset;

  offset = ((addr & 0xfffffff8) >> 1) + (addr & 3);
  if ((addr & 4) != 0) {
    offset = offset + 0x400000;
  }
  if (0x7fffff < addr) {
    offset = offset + 0x400000;
  }
  return offset;
}

uint vram32_to_64_offset(uint addr32)
{
  uint a = addr32;
  int bank = 0;
  uint addr;

  if (a < 0x800000) {
    if (a > 0x3fffff) {
      bank = 4;
      a -= 0x400000;
    }
    addr = ((a & ~3) << 1) + (a & 3) + bank;
  }
  else {
    if (a > 0xbfffff) {
      bank = 4;
      a -= 0x400000;
    }
    addr = ((a & ~3) << 1) + (a & 3) + bank;
    addr -= 0x800000;
  }
  return addr;
}
