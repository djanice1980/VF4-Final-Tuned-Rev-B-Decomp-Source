/*
 * sdk/kamui/texture_surfaces.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_fb_pixel_addrs_by_chip
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 lib_state_ptr; /* 0x0c32ab88 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

int surface_set_up_and_place(surface_rec *surface, int width, int height, uint format)
{
  int r;
  r = surface_desc_from_format(surface,width,height,format);
  if (r != 0) return r;
  if ((surface->kind & 0xffff) == 3) {
    r = packed_tex_alloc(surface);
  }
  else {
    r = surface_vram_alloc(surface->bytes,surface);
  }
  if (r == 3) {
    hook_slot_run(3);
  }
  return r;
}

int surface_pair_set_up_and_place(surface_rec *surface_a, surface_rec *surface_b, int width, int height, uint format)
{
  int r;

  r = surface_desc_from_format(surface_a,width,height,format);
  if (r != 0) return r;
  surface_b->kind = surface_a->kind;
  surface_b->texel_size_class = surface_a->texel_size_class;
  surface_b->pixel_format = surface_a->pixel_format;
  surface_b->width = surface_a->width;
  surface_b->height = surface_a->height;
  surface_b->bytes = surface_a->bytes;
  surface_b->format_flags = surface_a->format_flags;
  if ((surface_a->kind & 0xffff) == 3) {
    r = packed_tex_alloc(surface_a);
    if ((r ^ 3) == 0) {
      hook_slot_run(3);
      return r;
    }
    r = packed_tex_alloc(surface_b);
    if ((r ^ 3) == 0) {
      packed_tex_free(surface_a);
      hook_slot_run(3);
      return r;
    }
    return r;
  }
  r = surface_vram_alloc(surface_a->bytes,surface_a);
  if ((r ^ 3) == 0) {
    hook_slot_run(3);
    return r;
  }
  r = surface_vram_alloc(surface_b->bytes,surface_b);
  if (r == 3) {
    vram_free_at(surface_a->tex_address);
    hook_slot_run(3);
  }
  return r;
}

int surface_array_one_block(surface_rec *surfaces, int count, uint width, int height, uint format)
{
  int r;
  int i;
  int j;

  r = surface_desc_from_format(surfaces,width,height,format);
  if (r != 0) return r;
  if ((surfaces->kind & 0xffff) == 3) return 4;
  if (count > 1) {
    for (i = 1; i < count; i++) {
      surfaces[i].kind = surfaces[0].kind;
      surfaces[i].texel_size_class = surfaces[0].texel_size_class;
      surfaces[i].pixel_format = surfaces[0].pixel_format;
      surfaces[i].width = surfaces[0].width;
      surfaces[i].height = surfaces[0].height;
      surfaces[i].bytes = surfaces[0].bytes;
      surfaces[i].format_flags = surfaces[0].format_flags;
    }
  }
  r = surface_vram_alloc(count * surfaces->bytes,surfaces);
  if ((r ^ 3) == 0) {
    hook_slot_run(3);
    return r;
  }
  width = surfaces->tex_address;
  vram_free_at(width);
  for (i = 0; i < count; i++) {
    surfaces[i].tex_address = width;
    r = surface_vram_take_at_addr(surfaces[i].bytes,&surfaces[i]);
    width += (surfaces[i].bytes >> 2) * 4;
    if (r == 3) {
      for (j = 0; j < i; j++) {
        vram_free_at(surfaces[i].tex_address);
      }
      hook_slot_run(3);
      return r;
    }
  }
  return r;
}

int surface_data_to_vram(surface_rec *surface, void *data, int flag_a, int flag_b)
{
  int bad;

  bad = (flag_a == 1) | (flag_b == 1);
  if (bad) return 4;
  if ((surface->kind & 0xffff) == 3) {
    small_vq_to_vram(surface->tex_address,(uint)data,surface->width,surface->format_flags);
    return 0;
  }
  if ((surface->format_flags & 0x2d) == 5) {
    mip16_chain_to_vram(surface->tex_address,(uint)data,surface->width);
    return 0;
  }
  vram64_write(surface->tex_address,(uint)data,surface->bytes);
  return 0;
}

int surface_indexed_piece_to_vram(surface_rec *surface, void *data, int piece, int units)
{
  uint skip;
  uint n;
  uint off;
  uint len;

  if ((surface->kind & 0xffff) == 3) return 4;
  skip = ((surface->format_flags & 0x2d) == 5) * 4;
  n = units * 0x20;
  off = piece * n + skip;
  len = surface->bytes - off;
  if (off >= surface->bytes) return 0xc;
  if (len > n) len = n;
  vram64_write(off + surface->tex_address,(uint)data,len);
  return 0;
}

int surface_bytes_at_offset_to_vram(surface_rec *surface, void *data, uint offset, uint bytes)
{
  uint off;
  uint len;
  uint skip;
  off = offset & 0xfffffffc;
  bytes = bytes & 0xfffffffc;
  if (bytes == 0) return 0;
  if ((surface->kind & 0xffff) == 3) return 4;
  skip = (surface->format_flags & 0x2d) == 5;
  off = off + skip * 4;
  len = surface->bytes - off;
  if (off >= surface->bytes) return 7;
  if (len > bytes) len = bytes;
  vram64_write(off + surface->tex_address,(uint)data,len);
  return 0;
}

int surface_rect_rows_to_vram(surface_rec *surface, uint *src, int width, uint rows, int x, int y, int src_x, int src_y, int src_width)
{
  int pitch;
  int addr;
  uint row_bytes;
  uint stride;
  uint i;
  uint src_pitch;
  uint *p;

  if ((surface->format_flags & 0x2d) != 0) return 4;
  pitch = surface->width * 2;
  row_bytes = width * 2;
  addr = pitch * y + x * 2 + surface->tex_address;

  if (src_width == 0) {
    p = src;
    stride = row_bytes;
  }
  else {
    src_pitch = src_width * 2;
    p = src + ((src_pitch * src_y + src_x * 2) >> 2);
    stride = src_pitch;
  }
  stride >>= 2;
  for (i = 0; i < rows; i++) {
    vram64_write(addr,(uint)p,row_bytes);
    addr = addr + pitch;
    p = p + stride;
  }
  return 0;
}

int yuv_convert_into_surface(surface_rec *surface, uint data, uint across, uint down, int format, int wait)
{
  int yuv422;
  int multi;
  uint cols;
  uint rows;
  int blocks;
  int size;
  int ticket;

  if ((data & 0x1f) != 0) return 7;
  if ((surface->format_flags & 0x2d) != 0) return 4;
  if ((surface->pixel_format & 0x18000000) == 0) return 4;
  switch (format) {
  case 6: yuv422 = 0; break;
  case 3: yuv422 = 1; break;
  default: return 4;
  }
  if (across - 1 > 0x3f) return 4;
  if (down - 1 > 0x3f) return 4;
  blocks = across * down;
  if (blocks > 1) {
    multi = 1;
    cols = across - 1;
    rows = down - 1;
  } else {
    multi = 0;
    cols = (surface->width >> 4) - 1;
    rows = (surface->height >> 4) - 1;
  }
  if (yuv422 == 0) {
    if (multi == 0) size = (cols + 1) * 0x180 * (rows + 1);
    else size = blocks * 0x180;
  } else {
    if (multi == 0) size = (cols + 1) * 0x200 * (rows + 1);
    else size = blocks * 0x200;
  }
  pvr_reg_write_both(0x14c, yuv422 << 24 | multi << 16 | (rows & 0x3f) << 8 | cols & 0x3f);
  yuv422 = surface->tex_address & 0xfffff8;
  pvr_reg_write_both(0x148, yuv422);
  ticket = block_write_ticketed(data, 0x10800000, size);
  if (wait == 1) ticket_wait(ticket);
  return 0;
}

int surface_vq_entries_to_vram(surface_rec *surface, void *data)
{
  int r = 0;
  int level;
  uint bytes;
  if ((surface->kind & 0xffff) == 3) {
    level = log2_of_smaller_side(surface->width,surface->width);
    if ((surface->format_flags & 1) != 0) bytes = small_vq_mip_codebook_bytes[level];
    else bytes = small_vq_codebook_bytes[level];
    vram64_write(surface->tex_address,(uint)data,bytes);
  }
  else if ((surface->format_flags & 8) != 0) {
    vram64_write(surface->tex_address,(uint)data,0x800);
  }
  else {
    r = 4;
  }
  return r;
}

int surface_memory_return_by_kind(surface_rec *surface)
{
  if ((surface->kind & 0xffff) == 3) return packed_tex_free(surface);
  return vram_free_at(surface->tex_address);
}

int vram_pool_free_and_largest(int *free_bytes, int *largest)
{
  int r;
  int free0;
  int free1;
  int packed_free;
  int largest_block [1];

  r = vram_free_space_report(&free0,&free1,&packed_free,largest_block);
  *free_bytes = free0 + free1 + packed_free;
  *largest = largest_block[0];
  return r;
}

int mipmap_levels_overwrite(surface_rec *surface, uchar *data, int side)
{
  if (surface->pixel_format == 0x28000000 && (surface->format_flags & 1) != 0)
    return mip_level_to_vram_pal4(surface,data,side);
  if (surface->pixel_format == 0x30000000 && (surface->format_flags & 1) != 0)
    return mip_level_to_vram_pal8(surface,data,side);
  if ((surface->kind & 0xffff) == 3 || (surface->format_flags & 8) != 0)
    return mip_level_to_vram_vq(surface,data,side);
  if ((surface->format_flags & 5) == 5)
    return mip_level_to_vram_16bit(surface,data,side);
  return 4;
}

int surface_read_back(uint dest, surface_rec *surface)
{
  ushort pos[2];
  uint words[8];
  uint addr;
  uint cols;
  uint rows;
  int stride;
  uint y;
  uint x;
  uint k;
  int aligned;
  uint misalign;
  uint n;
  uint count;
  uint line;
  uint *out;

  if ((surface->kind & 3) == 1) {
    cols = gfx_state.tile_cols;
    rows = gfx_state.tile_rows;
    (void)gfx_state.field_read_offsets[1];
    stride = gfx_state.field_read_offsets[1] >> 2;
    misalign = dest & 0x1f;
    aligned = misalign == 0;
    for (y = 0; y < rows; y++) {
      pos[1] = y << 5;
      for (x = 0; x < cols; x++) {
        pos[0] = x << 5;
        fb_pixel_addrs_by_chip(pos,&addr,1);
        addr = addr >> 2;
        for (k = 0; k < 32; k++) {
          line = addr & 0xff4fffff;
          for (n = 0; n < 8; n++) {
            words[n] = (addr + n) << 2;
          }
          count = elan_vram_read_batch(words,8);
          if ((aligned != 0) & (count != 0)) {
            uint a;
            uint *p;
            uint *q;
            a = (uint)out + 28;
            *(volatile uint *)0xff000038 = (a >> 24) & 0x1c;
            *(volatile uint *)0xff00003c = (a >> 24) & 0x1c;
            p = (uint *)(a & 0x3ffffff | 0xe0000000);
            *p = words[7];
            q = p - 1;
            *q = words[6];
            *--q = words[5];
            *--q = words[4];
            *--q = words[3];
            *--q = words[2];
            *--q = words[1];
            *--q = words[0];
            __asm__ volatile ("pref @%0" : : "r" (q));
          }
          else {
            for (n = 0; n < count; n++) {
              ((volatile uint *)out)[n] = words[n];
            }
          }
          for (n = 0; n < 8; n++) {
            words[n] = (addr + n + 8) << 2;
          }
          elan_vram_read_batch(words,8);
          if (aligned != 0) {
            uint a;
            uint *p;
            uint *q;
            a = line * 4 + dest + 28;
            *(volatile uint *)0xff000038 = (a >> 24) & 0x1c;
            *(volatile uint *)0xff00003c = (a >> 24) & 0x1c;
            p = (uint *)(a & 0x3ffffff | 0xe0000000);
            *p = words[7];
            q = p - 1;
            *q = words[6];
            *--q = words[5];
            *--q = words[4];
            *--q = words[3];
            *--q = words[2];
            *--q = words[1];
            *--q = words[0];
            __asm__ volatile ("pref @%0" : : "r" (q));
          }
          else {
            for (n = 0; n < 8; n++) {
              ((volatile uint *)dest)[line + n] = words[n];
            }
          }
          out = (uint *)(dest + line * 4 + 0x20);
          addr = addr + stride;
        }
      }
    }
    count = elan_vram_read_batch(words,0);
    if ((aligned != 0) & (count != 0)) {
      uint a;
      uint *p;
      uint *q;
      a = (uint)out + 28;
      *(volatile uint *)0xff000038 = (a >> 24) & 0x1c;
      *(volatile uint *)0xff00003c = (a >> 24) & 0x1c;
      p = (uint *)(a & 0x3ffffff | 0xe0000000);
      *p = words[7];
      q = p - 1;
      *q = words[6];
      *--q = words[5];
      *--q = words[4];
      *--q = words[3];
      *--q = words[2];
      *--q = words[1];
      *--q = words[0];
      __asm__ volatile ("pref @%0" : : "r" (q));
    }
    else {
      for (n = 0; n < count; n++) {
        ((volatile uint *)out)[n] = words[n];
      }
    }
  }
  else if ((surface->kind & 0xffff) == 3) {
    small_vq_from_vram(dest,surface->tex_address,surface->width,surface->format_flags);
  }
  else if ((surface->format_flags & 0x2d) == 5) {
    mip16_chain_from_vram(dest,surface->tex_address,surface->width);
  }
  else {
    vram64_read((uint *)dest,surface->tex_address,surface->bytes);
  }
  return 0;
}

void vram_pool_sort_and_compact(void)
{
  vram_lists_sort();
  ((int (*)())vram_compact_halves)();
  return;
}

int surface_desc_from_format(surface_rec *surface, int width, int height, uint format)
{
  int r;
  uint type;
  uint pixel;
  r = 0;
  type = format & 0xff00;
  pixel = format & 0xff;
  if ((type == 0x1000) | (type == 0x1100)) surface->kind = 3;
  else surface->kind = 2;
  if ((type == 0x500) | (type == 0x600)) {
    if (*(int *)(lib_state_ptr + 0x9c) == 3) surface->texel_size_class = 3;
    else surface->texel_size_class = 1;
    surface->pixel_format = 0x28000000;
  }
  else if ((type == 0x700) | (type == 0x800)) {
    if (*(int *)(lib_state_ptr + 0x9c) == 3) surface->texel_size_class = 3;
    else surface->texel_size_class = 1;
    surface->pixel_format = 0x30000000;
  }
  else {
    surface->texel_size_class = 1;
    switch (pixel) {
    case 0:
      surface->pixel_format = 0;
      break;
    case 1:
      surface->pixel_format = 0x08000000;
      break;
    case 2:
      surface->pixel_format = 0x10000000;
      break;
    case 3:
      surface->pixel_format = 0x18000000;
      break;
    case 4:
      surface->pixel_format = 0x20000000;
      break;
    default:
      r = 4;
    }
  }
  surface->width = width;
  surface->height = height;
  switch (width) {
  case 8: case 0x10: case 0x20: case 0x40: case 0x80: case 0x100: case 0x200: case 0x400:
    break;
  default:
    r = 4;
  }
  switch (height) {
  case 8: case 0x10: case 0x20: case 0x40: case 0x80: case 0x100: case 0x200: case 0x400:
    break;
  default:
    r = 4;
  }
  if ((type != 0x900) & (type != 0xb00) && (type != 0xd00) & (type != 0x500) && (type != 0x700) & (width != height)) {
    r = 4;
  }
  surface->format_flags = 0;
  if ((type == 0xf00) | (type == 0x200) || (type == 0x400) | (type == 0x1100) || (type == 0x600) | (type == 0x800)) {
    surface->format_flags |= 1;
  }
  if ((type == 0xe00) | (type == 0xf00) || (type == 0x100) | (type == 0x200) || type == 0xd00) {
    surface->format_flags |= 4;
  }
  if ((type == 0x300) | (type == 0x400) || (type == 0x1000) | (type == 0x1100)) {
    surface->format_flags |= 0xc;
  }
  if (type == 0xb00) {
    surface->format_flags |= 0x10;
  }
  if ((type == 0x500) | (type == 0x600) || (type == 0x700) | (type == 0x800)) {
    surface->format_flags |= 0x24;
  }
  surface_desc_fill_bytes(surface);
  return r;
}

int log2_of_smaller_side(int side0, int side1)
{
  int r;
  if (side1 > side0) side1 = side0;
  switch (side1) {
  case 1: r = 0; break;
  case 2: r = 1; break;
  case 4: r = 2; break;
  case 8: r = 3; break;
  case 0x10: r = 4; break;
  case 0x20: r = 5; break;
  case 0x40: r = 6; break;
  case 0x80: r = 7; break;
  case 0x100: r = 8; break;
  case 0x200: r = 9; break;
  case 0x400: r = 10; break;
  default: r = 3; break;
  }
  return r;
}

void surface_desc_fill_bytes(surface_rec *surface)
{
  if ((surface->kind & 0xffff) == 3) surface->bytes = small_vq_bytes_needed(surface);
  else if (surface->format_flags & 8) surface->bytes = vq_bytes_needed(surface);
  else if (surface->format_flags & 0x20) surface->bytes = palettized_bytes_needed(surface);
  else surface->bytes = direct_bytes_needed(surface);
}

int direct_bytes_needed(surface_rec *surface)
{
  uint w;
  uint h;
  uint smaller;
  int size;

  w = surface->width;
  h = surface->height;
  size = w * h;
  if ((surface->format_flags & 1) != 0) {
    w = w >> 1;
    h = h >> 1;
    smaller = (w < h) ? w : h;
    for (; 1 < smaller; smaller = smaller >> 1) {
      size = size + w * h;
      w = w >> 1;
      h = h >> 1;
    }
    if (w == h) {
      size = size + 4;
    }
    else {
      size = size + w * h;
    }
  }
  if (surface->texel_size_class == 1) {
    size = size * 2;
  }
  else {
    size = size << 2;
  }
  return size;
}

int vq_bytes_needed(surface_rec *surface)
{
  uint w;
  uint h;
  uint smaller;
  int size;

  w = surface->width >> 1;
  h = surface->height >> 1;
  size = w * h;
  if (surface->format_flags & 1) {
    w = surface->width >> 2;
    h = surface->height >> 2;
    smaller = w < h ? w : h;
    for (; smaller > 1; smaller >>= 1) {
      size += w * h;
      w >>= 1;
      h >>= 1;
    }
    if (w == h) {
      size += 2;
    }
    else {
      size += w * h;
    }
    size += 10;
  }
  else if (surface->width == 8 && surface->height == 8) {
    size += 0x10;
  }
  size += 0x800;
  return size;
}

uint palettized_bytes_needed(surface_rec *surface)
{
  uint n;
  uint w;
  uint h;
  uint smaller;

  w = surface->width;
  h = surface->height;
  n = w * h;
  if ((surface->format_flags & 1) != 0) {
    w >>= 1;
    h >>= 1;
    smaller = (w < h) ? w : h;
    for (; smaller > 1; smaller >>= 1) {
      n += w * h;
      w >>= 1;
      h >>= 1;
    }
    if (w == h) n += 1;
    else n += w * h;
    if (surface->pixel_format == 0x28000000) n += 0x2b;
    else n += 0xb;
  }
  if (surface->pixel_format == 0x28000000) {
    if (n != 1) n >>= 1;
  }
  return n;
}

int small_vq_bytes_needed(surface_rec *surface)
{
  uint height;
  uint width;
  uint w;
  uint h;
  uint smaller;
  int size;
  int level;

  width = surface->width;
  w = width >> 1;
  height = surface->height;
  h = height >> 1;
  size = w * h;
  level = log2_of_smaller_side(width,height);
  if (surface->format_flags & 1) {
    w = width >> 2;
    h = height >> 2;
    smaller = w < h ? w : h;
    for (; smaller > 1; smaller >>= 1) {
      size += w * h;
      w >>= 1;
      h >>= 1;
    }
    if (w == h) size += 2;
    else size += w * h;
    size += 10;
    size += small_vq_mip_codebook_bytes[level];
  }
  else size += small_vq_codebook_bytes[level];
  return size;
}

void mip16_chain_to_vram(int dst, uint src, int side)
{
  vram64_write(dst + 4,src,mip16_chain_tail_bytes[log2_of_smaller_side(side,side)] + 4);
  return;
}

void mip16_chain_from_vram(uint dst, int src, int side)
{
  int from = src + 4;
  uint bytes = mip16_chain_tail_bytes[log2_of_smaller_side(side,side)];
  vram64_read((uint *)dst,from,bytes);
}

void vq_image_to_vram(uint dst, uint src, int side, uint flags)
{
  uint from = src;
  uint to = dst;
  int level;
  int bytes;

  level = log2_of_smaller_side(side,side);
  if ((flags & 1) != 0) {
    bytes = vq_mip_index_bytes[level] + 0x80a;
  }
  else {
    bytes = vq_index_bytes[level] + 0x800;
    if (side == 8) {
      bytes += 0x10;
    }
  }
  vram64_write(to,from,bytes);
  return;
}

void vq_image_from_vram(uint dst, int src, int side, uint flags)
{
  int from = src;
  uint to = dst;
  int level;
  int bytes;

  level = log2_of_smaller_side(side,side);
  if (flags & 1) {
    bytes = vq_mip_index_bytes[level] + 0x80a;
  }
  else {
    bytes = vq_index_bytes[level] + 0x800;
    if (side == 8) {
      bytes += 0x10;
    }
  }
  vram64_read((uint *)to,from,bytes);
  return;
}

void small_vq_to_vram(int dst, int src, int side, uint flags)
{
  uint fl = flags;
  int from = src;
  int to = dst;
  int level;
  int codebook_bytes;
  int index_bytes;

  level = log2_of_smaller_side(side,side);
  if (fl & 1) {
    codebook_bytes = small_vq_mip_codebook_bytes[level];
    index_bytes = vq_mip_index_bytes[level] + 10;
  }
  else {
    codebook_bytes = small_vq_codebook_bytes[level];
    index_bytes = vq_index_bytes[level];
  }
  vram64_write(to,from,codebook_bytes);
  from += codebook_bytes;
  to += 0x800;
  vram64_write(to,from,index_bytes);
}

void small_vq_from_vram(int dst, int src, int side, uint flags)
{
  uint fl = flags;
  int from = src;
  int to = dst;
  int level;
  int codebook_bytes;
  int index_bytes;

  level = log2_of_smaller_side(side,side);
  if ((fl & 1) != 0) {
    codebook_bytes = small_vq_mip_codebook_bytes[level];
    index_bytes = vq_mip_index_bytes[level] + 10;
  }
  else {
    codebook_bytes = small_vq_codebook_bytes[level];
    index_bytes = vq_index_bytes[level];
  }
  vram64_read((uint *)to,from,codebook_bytes);
  from += 0x800;
  to += codebook_bytes;
  vram64_read((uint *)to,from,index_bytes);
}

void empty_fn_0c143740(void)
{
  return;
}

void empty_fn_0c143760(void)
{
  return;
}

int mip_level_to_vram_pal4(surface_rec *surface, uchar *src, int side)
{
  uchar buf[16];
  int level;
  int i;
  int dst;
  int off;
  int base;
  uchar *s;
  int *offsets;

  offsets = pal4_level_offsets;
  level = log2_of_smaller_side(side,side);
  if (side < 5) {
    vram64_read((uint *)buf,surface->tex_address,0x10);
    if (side == 1) {
      buf[1] = *src;
    }
    else {
      s = src;
      if (side == 2) {
        for (i = 0; i < 2; i++) buf[i + 2] = *s++;
      }
      else {
        for (i = 0; i < 8; i++) buf[i + 4] = *s++;
      }
    }
    dst = surface->tex_address;
  }
  else {
    base = surface->tex_address;
    vram64_read((uint *)buf,base + pal4_level_offsets[level] + -0xc,0x10);
    s = src;
    for (i = 0; i < 4; i++) buf[i + 12] = *s++;
    dst = surface->tex_address;
    off = pal4_level_offsets[level];
    dst = dst + off + -0xc;
    ocbwb_line((int)buf);
    ocbwb_line((int)buf + 0x20);
    vram64_write(dst,(uint)buf,0x10);
    if (pal4_level_bytes[level] != 0x10) {
      dst = surface->tex_address;
      dst = dst + off + 4;
      vram64_write(dst,(uint)(src + 4),
                   pal4_level_bytes[level] + -0x10);
    }
    base = surface->tex_address;
    vram64_read((uint *)buf,base + offsets[level + 1] + -0xc,0x10);
    s = src + pal4_level_bytes[level] + -0xc;
    for (i = 0; i < 12; i++) buf[i] = *s++;
    dst = surface->tex_address;
    dst = dst + offsets[level + 1] + -0xc;
  }
  ocbwb_line((int)buf);
  ocbwb_line((int)buf + 0x20);
  vram64_write(dst,(uint)buf,0x10);
  return 0;
}

int mip_level_to_vram_pal8(surface_rec *surface, uchar *src, int side)
{
  int level;
  int i;
  int dst;
  uint n;
  uchar *s;
  uchar buf[8];

  level = log2_of_smaller_side(side,side);
  if (side < 3) {
    vram64_read((uint *)buf,surface->tex_address,8);
    if (side == 1) {
      buf[3] = *src;
    }
    else {
      for (i = 0, s = src; i < 4; i++) {
        buf[i + 4] = *s++;
      }
    }
    dst = surface->tex_address;
    ocbwb_line((int)buf);
    ocbwb_line((int)(buf + 0x20));
    s = buf;
    n = 8;
    vram64_write(dst,(uint)s,n);
  }
  else {
    n = pal8_level_bytes[level];
    s = src;
    dst = surface->tex_address + pal8_level_offsets[level];
    vram64_write(dst,(uint)s,n);
  }
  return 0;
}

int mip_level_to_vram_16bit(surface_rec *surface, uchar *src, int side)
{
  uchar buf[16];
  int level;
  int i;
  uint dst;
  uint len;
  uchar *s;

  level = log2_of_smaller_side(side,side);
  if (side < 3) {
    vram64_read((uint *)buf,surface->tex_address,0x10);
    if (side == 1) {
      s = src;
      for (i = 0; i < 2; i++) {
        buf[i + 6] = *s++;
      }
    }
    else {
      s = src;
      for (i = 0; i < 8; i++) {
        buf[i + 8] = *s++;
      }
    }
    dst = surface->tex_address;
    ocbwb_line((int)buf);
    ocbwb_line((int)(buf + 0x20));
    vram64_write(dst,(uint)buf,0x10);
  }
  else {
    len = mip16_level_bytes[level];
    s = src;
    dst = surface->tex_address + mip16_level_offsets[level];
    vram64_write(dst,(uint)s,len);
  }
  return 0;
}

int mip_level_to_vram_vq(surface_rec *surface, uchar *src, int side)
{
  char buf[16];
  int level;
  int i;
  int off;
  int dst;
  int base;
  uchar *p;
  int *offsets = vq_mip_index_bytes - 1;
  level = log2_of_smaller_side(side,side);
  if (side == 1) {
  }
  else if (side <= 4) {
    vram64_read((uint *)buf,surface->tex_address + 0x800,0x10);
    if (side == 2) {
      buf[1] = *src;
    }
    else {
      p = src;
      for (i = 0; i < 4; i++) buf[i + 2] = *p++;
    }
    dst = surface->tex_address;
    dst += 0x800;
    ocbwb_line((int)buf);
    ocbwb_line((int)(buf + 0x20));
    vram64_write(dst,(uint)buf,0x10);
  }
  else {
    base = surface->tex_address;
    vram64_read((uint *)buf,base + offsets[level] + 0x7fa,0x10);
    p = src;
    for (i = 0; i < 10; i++) buf[i + 6] = *p++;
    dst = surface->tex_address;
    off = offsets[level];
    dst += off;
    dst += 0x7fa;
    ocbwb_line((int)buf);
    ocbwb_line((int)(buf + 0x20));
    vram64_write(dst,(uint)buf,0x10);
    if (vq_index_bytes[level] != 0x10) {
      dst = surface->tex_address;
      dst += off;
      dst += 0x80a;
      vram64_write(dst,(uint)(src + 10),vq_index_bytes[level] - 0x10);
    }
    base = surface->tex_address;
    vram64_read((uint *)buf,base + (vq_mip_index_bytes - 1)[level + 1] + 0x7fa,0x10);
    p = src + vq_index_bytes[level] - 6;
    for (i = 0; i < 6; i++) buf[i] = *p++;
    dst = surface->tex_address;
    dst += (vq_mip_index_bytes - 1)[level + 1];
    dst += 0x7fa;
    ocbwb_line((int)buf);
    ocbwb_line((int)(buf + 0x20));
    vram64_write(dst,(uint)buf,0x10);
  }
  return 0;
}

uint quantize_byte_to_3bit(uint value)
{
  uchar c = value;
  uchar level;
  level = c < 0x80 ? 3 : 4;
  if (c < 0x70) {
    level = (c + 0x10) >> 5;
  }
  if (0x8f < c) {
    level = (c - 0xf) >> 5;
  }
  return level;
}

int quant_error_6bit(uint c)
{
  return c - (c & 0xfc | 2);
}

int quant_error_5bit(uint c)
{
  return c - (c & 0xf8 | 4);
}

int quant_error_4bit(uint c)
{
  return c - (c & 0xf0 | c >> 4 & 8 | 4);
}

int quant_error_3bit(uint c)
{
  int kept;

  kept = quantize_byte_to_3bit(c & 0xff) << 5;
  if (kept & 0x80) return c - kept - 0x1f;
  return c - kept;
}

int quant_error_2bit(uint c)
{
  uint kept;

  kept = ((quantize_byte_to_3bit(c & 0xff) >> 1) & 0xff) << 6;
  if (kept & 0x80) return c - kept - 0x3f;
  return c - kept;
}

void dither_argb8888_in_place(uchar *pixels, int width, int height, uint format)
{
  int err_right[4];
  int err_diag[4];
  int err_below[4096];
  int *below;
  int n;
  int rows;
  int c3, c2, c1, c0;
  int share;
  uchar *src;

  if (width != 1) {
    if (height != 1) {
      below = err_below;
      for (n = width; n != 0; n--) {
        below[0] = 0;
        below[1] = 0;
        below[2] = 0;
        below[3] = 0;
        below += 4;
      }
      err_right[0] = 0;
      err_right[1] = 0;
      err_right[2] = 0;
      err_right[3] = 0;
      err_diag[0] = 0;
      err_diag[1] = 0;
      err_diag[2] = 0;
      err_diag[3] = 0;
      src = pixels;
      for (rows = height; rows != 0; rows--) {
        below = err_below;
        for (n = width; n != 0; n--) {
          c3 = src[3] + below[0] + err_right[0];
          c2 = src[2] + below[1] + err_right[1];
          c1 = src[1] + below[2] + err_right[2];
          c0 = src[0];
          c3 = c3 < 0 ? 0 : (c3 > 255 ? 255 : c3);
          c2 = c2 < 0 ? 0 : (c2 > 255 ? 255 : c2);
          c1 = c1 < 0 ? 0 : (c1 > 255 ? 255 : c1);
          c0 = c0 < 0 ? 0 : (c0 > 255 ? 255 : c0);
          src[3] = c3;
          src[2] = c2;
          src[1] = c1;
          src[0] = c0;
          if (format == 0x08000000) {
            c3 = quant_error_5bit(c3);
            c2 = quant_error_6bit(c2);
            c1 = quant_error_5bit(c1);
            c0 = 0;
          } else if (format == 0x10000000) {
            c3 = quant_error_4bit(c3);
            c2 = quant_error_4bit(c2);
            c1 = quant_error_4bit(c1);
            c0 = quant_error_4bit(c0);
          } else if (format == 0) {
            c3 = quant_error_5bit(c3);
            c2 = quant_error_5bit(c2);
            c1 = quant_error_5bit(c1);
          }
          share = (c3 * 3 + 4) >> 3;
          err_right[0] = share;
          below[0] = err_diag[0] + share;
          err_diag[0] = c3 - share * 2;
          share = (c2 * 3 + 4) >> 3;
          err_right[1] = share;
          below[1] = err_diag[1] + share;
          err_diag[1] = c2 - share * 2;
          share = (c1 * 3 + 4) >> 3;
          err_right[2] = share;
          below[2] = err_diag[2] + share;
          err_diag[2] = c1 - share * 2;
          share = (c1 * 3 + 4) >> 3;
          err_right[3] = share;
          below[3] = err_diag[3] + share;
          err_diag[3] = c0 - share * 2;
          src += 4;
          below += 4;
        }
      }
    }
  }
}

void pixels_to_twiddled16(ushort *dest, uchar *pixels, int side, uint format)
{
  uint row_bits;
  uint column_bits;
  int rows;
  int n;
  uchar *row;
  uchar *src;
  uint pixel;

  row = pixels;
  row_bits = 0;
  for (rows = side; rows != 0; rows--) {
    column_bits = 0;
    src = row;
    for (n = side; n != 0; n--) {
      if (format == 0x08000000) {
        pixel = (src[3] & 0xf8) << 8;
        pixel |= (src[2] & 0xfc) << 3;
        pixel |= src[1] >> 3;
      }
      else if (format == 0x10000000) {
        pixel = (src[0] & 0xf0) << 8;
        pixel |= ((src[3] & 0xf0) >> 4) << 8;
        pixel |= src[2] & 0xf0;
        pixel |= src[1] >> 4;
      }
      else if (format == 0) {
        pixel = (src[0] & 0x80) << 8;
        pixel |= (src[3] & 0xf8) << 7;
        pixel |= (src[2] & 0xf8) << 2;
        pixel |= src[1] >> 3;
      }
      dest[column_bits | row_bits] = pixel;
      src += 4;
      column_bits = (column_bits + 0x55555556) & 0xaaaaaaaa;
    }
    row += side * 4;
    row_bits = (row_bits + 0xaaaaaaab) & 0x55555555;
  }
}

int return_zero_0c1441e0(void)
{
  return 0;
}
