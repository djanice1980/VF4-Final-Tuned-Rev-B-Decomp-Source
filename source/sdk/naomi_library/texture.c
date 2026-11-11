/*
 * sdk/naomi_library/texture.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int texidx_bitmap_words; /* 0x0c32a584 */
extern tex_load_hook_fn tex_load_hook; /* 0x0c32a588 */
extern int tex_slot_count; /* 0x0c418418 */
extern tex_builtin_desc tex_builtin_8x8_desc; /* 0x0c211e9c */

int tex_slot_create_and_load(short *header, int slot_no, int data)
{
  int result;
  tex_slot_rec *slot;

  slot = tex_slot_table + slot_no;
  tex_load_count = tex_load_count + 1;
  result = surface_set_up_and_place(&slot->surface,(int)*header,(int)header[1],*(uint *)(header + 2));
  slot->status = result;
  if (result == 0) {
    if ((tex_load_hook != 0) && (result = tex_load_hook(header), result != 0)) {
      data = result;
    }
    surface_data_to_vram(&tex_slot_table[slot_no].surface,(void *)data,0,0);
    return 0;
  }
  return 1;
}

void texidx_tables_place(int work, int slot_count, int index_count)
{
  int i;
  slot_count = slot_count + 1;
  index_count = index_count + 1;
  tex_slot_table_bind((tex_slot_rec *)work,slot_count);
  work = work + slot_count * 0x3c + 8;
  texidx_map = (texidx_map_entry *)work;
  work = work + index_count * 8;
  texidx_bitmap = (uint *)work;
  texidx_bitmap_words = (index_count + 31) / 32;
  texidx_count = index_count - 1;
  for (i = -1; i < texidx_count; i++) {
    texidx_map[i].use_count = 0;
    texidx_map[i].slot_no = -1;
  }
  for (i = 0; i < texidx_bitmap_words; i++) {
    texidx_bitmap[i] = 0;
  }
  texidx_base = 0;
}

void texidx_tables_wipe(void)
{
  int i;
  tex_slot_table_wipe();
  if (texidx_count != 0) {
    for (i = -1; i < texidx_count; i++) {
      texidx_map[i].use_count = 0;
      texidx_map[i].slot_no = -1;
    }
    for (i = 0; i < texidx_bitmap_words; i++) {
      texidx_bitmap[i] = 0;
    }
  }
}

void texidx_base_put(int base)
{
  texidx_base = base;
  return;
}

bool texidx_slot_overwrite(int index, int data)
{
  int result;

  if (-1 < index) {
    index = index + texidx_base;
  }
  if (texidx_map[index].use_count == 0) return 0;
  index = texidx_map[index].slot_no;
  if (tex_slot_table[index].status != 0) return 0;
  result = surface_data_to_vram(&tex_slot_table[index].surface,(void *)data,0,0);
  return result == 0;
}

void texidx_slot_overwrite_from_list(int index, int list, int entry)
{
  texidx_slot_overwrite(index,((int (*)[4])list)[entry][2]);
  return;
}

bool texidx_vram_words_read(int index, uint *out)
{
  uint *words;
  int count;
  int i;

  if (-1 < index) {
    index = index + texidx_base;
  }
  if (texidx_map[index].use_count == 0) return 0;
  index = texidx_map[index].slot_no;
  if (tex_slot_table[index].status != 0) return 0;
  words = (uint *)(tex_slot_table[index].surface.tex_address + 0xa4000000);
  count = tex_slot_table[index].surface.height * tex_slot_table[index].surface.width;
  for (i = 0; i < count; i += 2) {
    *out++ = *words++;
  }
  return 1;
}

uint tex_vram_free_total(void)
{
  int free_bytes;
  uint largest;

  vram_pool_free_and_largest(&free_bytes,&largest);
  return free_bytes;
}

uint tex_vram_free_second(void)
{
  int free_bytes;
  uint largest;

  vram_pool_free_and_largest(&free_bytes,&largest);
  return largest;
}

int texidx_bit_take(void)
{
  int i;
  int n;
  uint m;
  uint w;

  for (i = 0; i < texidx_bitmap_words; i++) {
    w = texidx_bitmap[i];
    if (w != 0xffffffff) {
      n = 0;
      for (m = 1; ; m <<= 1, n++) {
        if ((w & m) == 0) {
          texidx_bitmap[i] |= m;
          return i * 0x20 + n;
        }
      }
    }
  }
  return -1;
}

void texidx_bit_clear(uint index)
{
  uint m;
  int i = index;
  if (i >= 0) {
    m = 1 << (i & 0x1f);
    texidx_bitmap[i >> 5] &= ~m;
  }
}

void texidx_header_words_put(uint *header, int index)
{
  tex_slot_rec *slot;
  int slot_no;
  uint size_word;
  uint control;

  slot_no = texidx_map[index].slot_no;
  size_word = header[2] & 0xffffffc0;
  slot = &tex_slot_table[slot_no];
  size_word |= slot->size_word;
  control = slot->tex_control_word;
  header[2] = size_word;
  header[3] = control;
}

void tex_slots_rebuild_after_vram_pass(void)
{
  tex_slot_rec *slot;
  int i;

  ((int (*)())vram_pool_sort_and_compact)();
  slot = tex_slot_table;
  for (i = 0; i < tex_slot_count; i++, slot++) {
    if (slot->status == 0) {
      tex_slot_build_hw_words(slot);
    }
  }
}

void tex_load_hook_set(tex_load_hook_fn hook)
{
  tex_load_hook = hook;
  return;
}

uint tex_size_code(int size)
{
  uint code;

  if (size == 8) {
    code = 0;
  }
  else if (size == 0x10) {
    code = 1;
  }
  else if (size == 0x20) {
    code = 2;
  }
  else if (size == 0x40) {
    code = 3;
  }
  else if (size == 0x80) {
    code = 4;
  }
  else if (size == 0x100) {
    code = 5;
  }
  else if (size == 0x200) {
    code = 6;
  }
  else if (size == 0x400) {
    code = 7;
  }
  return code;
}

void tex_slot_bits_into_poly_hdr(uint *hdr, int slot_no)
{
  tex_slot_rec *slot;
  uint m;
  uint u;

  m = hdr[2] & 0xffffffc0;
  slot = tex_slot_table + slot_no;
  m |= slot->size_word;
  u = slot->tex_control_word;
  hdr[2] = m;
  hdr[3] = u;
}

void tex_slot_table_bind(tex_slot_rec *table, int count)
{
  int i;

  tex_slot_table = table;
  tex_slot_count = count;
  tex_load_count = 0;
  texidx_base = 0;
  for (i = 0; i < count; i++) {
    table->status = -1;
    table->in_use = 0;
    table->slot_id = -1;
    table++;
  }
}

void tex_slot_build_hw_words(tex_slot_rec *slot)
{
  uint size_bits;
  uint flags;

  size_bits = tex_size_code(slot->surface.width);
  size_bits = size_bits << 3;
  size_bits |= tex_size_code(slot->surface.height);
  slot->size_word = size_bits;
  flags = slot->surface.format_flags;
  slot->tex_control_word =
       flags << 0x1f | (flags & 8) << 0x1b | slot->surface.pixel_format & 0x38000000 |
       (~flags & 4) << 0x18 | (flags & 0x10) << 0x15 | slot->surface.tex_address >> 3;
  return;
}

void tex_slot_words_or_fallback(int slot_no)
{
  tex_slot_rec *slot = tex_slot_table + slot_no;
  if (slot->status == 0) {
    tex_slot_build_hw_words(slot);
  }
  else {
    slot->size_word = tex_fallback_slot.size_word;
    slot->tex_control_word = tex_fallback_slot.tex_control_word;
  }
}

void tex_slot_table_wipe(void)
{
  tex_slot_rec *slot;
  int i;
  for (slot = tex_slot_table, i = 0; i < tex_slot_count; i++, slot++) {
    if ((slot->in_use != 0) && (slot->status == 0)) {
      surface_memory_return_by_kind(&slot->surface);
    }
  }
  for (slot = tex_slot_table, i = 0; i < tex_slot_count; i++, slot++) {
    slot->status = -1;
    slot->in_use = 0;
    slot->slot_id = -1;
  }
  texidx_base = 0;
  tex_load_count = 0;
}

void tex_builtin_8x8_upload(void)
{
  typedef struct { short width; short height; int format; uchar *data; } desc;
  desc *d = (desc *)&tex_builtin_8x8_desc;
  surface_set_up_and_place(&tex_fallback_slot.surface,d->width,d->height,d->format);
  surface_data_to_vram(&tex_fallback_slot.surface,d->data,0,0);
  tex_slot_build_hw_words(&tex_fallback_slot);
  return;
}

void tex_builtin_bits_into_poly_hdr(uint *hdr)
{
  uint a;
  uint b;
  a = hdr[2];
  b = hdr[3];
  a &= 0xffffffc0;
  b &= 0xf8000000;
  a |= tex_fallback_slot.size_word;
  b |= tex_fallback_slot.tex_control_word;
  hdr[2] = a;
  hdr[3] = b;
}
