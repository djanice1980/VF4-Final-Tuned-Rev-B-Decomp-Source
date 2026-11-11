/*
 * sdk/naomi2_library/model_compile.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern model_error_rec model_error_record; /* 0x0c3dc158 */
extern elan_matrix_param_rec elan_matrix_params; /* 0x0c3dc188 */
extern light_ambient_color_rec light_ambient_color; /* 0x0c41c64c */

static inline void sq_pref(void *a)
{
  __asm__ volatile ("pref @%0" : : "r" (a));
}

void elan_ram_top_mark_set(void)
{
  elan_ram_top_after_lists = elan_ram_top;
  return;
}

void elan_ram_top_mark_restore(void)
{
  elan_ram_top = elan_ram_top_after_lists;
  return;
}

void elan_ram_top_reset(void)
{
  elan_ram_top_after_lists = elan_ram_top_after_lists2;
  elan_ram_top = elan_ram_top_after_lists2;
  return;
}

void elan_cmd_half_size_set(int bytes)
{
  elan_cmd_half_size = (bytes + 0x1f) & ~0x1f;
  return;
}

uint model_heap_top(void)
{
  return elan_ram_top;
}

int frame_list_write_end(void)
{
  if ((render_frames_in_flight == 3) && (elan_cmd_half == 0)) {
    return (int)elan_cmd_write_ptr + elan_cmd_half_size;
  }
  return (int)elan_cmd_write_ptr;
}

void model_header_sq_copy(uint *model, uint dst)
{
  uint *d;
  int i;
  uint buf_size;
  volatile uint *q = (volatile uint *)0xff000038;
  uint h = dst >> 24;
  q[0] = h;
  q[1] = h;
  d = (uint *)(dst & 0x3ffffff | 0xe0000000);
  buf_size = *(uint *)(dst + 0x3c);
  for (i = 3; i > 0; i--) {
    __asm__ volatile ("pref @%0" : : "r" (model + 16));
    *d++ = *model++;
    *d++ = *model++;
    *d++ = *model++;
    *d++ = *model++;
    *d++ = *model++;
    *d++ = *model++;
    *d++ = *model++;
    *d++ = *model++;
    __asm__ volatile ("pref @%0" : : "r" (d - 8));
  }
  *(uint *)(dst + 0x3c) = buf_size;
}

void model_body_sq_copy(uint *model, uint dst)
{
  uint *d;
  uint *s;
  int i;
  int n;
  int t;
  uint *q = (uint *)0xff000038;
  uint v;
  uint h;
  s = model + 0x18;
  v = dst + 0x60;
  h = v >> 24;
  q[0] = h;
  q[1] = h;
  d = (uint *)(v & 0x3ffffff | 0xe0000000);
  t = model[6] - 0x41;
  n = (uint)t >> 5;
  if (n > 0) for (i = n; i > 0; i--) {
    __asm__ volatile ("pref @%0" : : "r" (s + 16));
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    __asm__ volatile ("pref @%0" : : "r" (d - 8));
  }
}

int model_size(int *model)
{
  int a;
  int b;
  int c;
  int n;
  
  if (model == (int *)0x0) return -1;
  if (*model == -1) return -1;
  if (*model == 0x100) {
    n = model[6];
  }
  else {
    a = model_list_compiled_size(model,0);
    b = model_list_compiled_size(model,2);
    c = model_list_compiled_size(model,4);
    n = a + b + c;
    if (n > 0) n += 0x60;
  }
  return n;
}

int model_modvol_size(int *model)
{
  int size;
  int size1;

  if (model == (int *)0x0) return -1;
  if (*model == -1) return -1;
  if (*model == 0x100) return model[6];
  size1 = model_list_compiled_size(model,1);
  size = model_list_compiled_size(model,3);
  size = size + size1;
  if (0 < size) {
    size = size + 0x60;
  }
  return size;
}

uint * model_buf_alloc(int *model)
{
  int size;
  uint *buf;

  buf = 0;
  size = model_size(model);
  if (0 < size) {
    size = (size + 0x1f) & ~0x1f;
    buf = (uint *)elan_ram_alloc_top(size);
    buf[15] = size;
  }
  return buf;
}

uint * model_modvol_buf_alloc(int *model)
{
  int size;
  uint *buf;
  buf = 0;
  size = model_modvol_size(model);
  if (0 < size) {
    size = size + 0x1fU & 0xffffffe0;
    buf = (uint *)elan_ram_alloc_top(size);
    buf[15] = size;
  }
  return buf;
}

uint * model_compile_to(int *model, uint *dst)
{
  uint end;

  if ((model == (int *)0x0) | (dst == (uint *)0x0)) return (uint *)0x0;
  if (*model == -1) return (uint *)0x0;
  if (*model == 0x100) {
    model_header_sq_copy(model,(uint)dst);
    model_body_sq_copy(model,(uint)dst);
  }
  else {
    end = model_list_compile(model,dst + 0x10,(uint)(dst + 0x18),0,-1);
    end = model_list_compile(model,dst + 0x12,end,2,-1);
    end = model_list_compile(model,dst + 0x14,end,4,-1);
    *dst = 0x100;
    dst[6] = end - (int)dst;
    dst[2] = model[2];
    dst[3] = model[3];
    dst[4] = model[4];
    dst[5] = model[5];
    dst[7] = 0;
  }
  return dst;
}

uint * model_modvol_compile_to(int *model, uint *dst)
{
  uint end;

  if (model == (int *)0x0 || dst == (uint *)0x0) return 0;
  if (*model == -1) return 0;
  if (*model == 0x100) {
    model_header_sq_copy(model,(uint)dst);
    model_body_sq_copy(model,(uint)dst);
  }
  else {
    end = model_list_compile(model,dst + 0x10,(uint)(dst + 0x18),1,-1);
    end = model_list_compile(model,dst + 0x12,end,3,-1);
    dst[0x14] = 0;
    dst[0x15] = 0;
    *dst = 0x100;
    dst[6] = end - (int)dst;
    dst[2] = model[2];
    dst[3] = model[3];
    dst[4] = model[4];
    dst[5] = model[5];
    dst[7] = 0;
  }
  return dst;
}

uint * model_compile_alloc(int *model)
{
  uint *buf;
  uint *compiled;

  buf = model_buf_alloc(model);
  compiled = model_compile_to(model,buf);
  if (compiled != 0) return compiled;
  model_buf_free(buf);
  compiled = 0;
  return compiled;
}

uint * model_modvol_compile_alloc(int *model)
{
  uint *buf;
  uint *compiled;

  buf = model_modvol_buf_alloc(model);
  compiled = model_modvol_compile_to(model,buf);
  if (compiled != 0) return compiled;
  model_buf_free(buf);
  compiled = 0;
  return compiled;
}

void model_buf_free(uint *buf)
{
    if (buf != 0)
        elan_ram_top_release_to((int)buf, buf[15]);
}

uint * model_compile_to_frame_list(int *model)
{
  uint *h;
  uint size;

  h = elan_cmd_write_ptr;
  if (model == (int *)0x0) return 0;
  if (*model == -1) return 0;
  if (*model == 0x100) {
    size = model[6] + 0x1fU & 0xffffffe0;
    elan_cmd_write_ptr = (uint *)((char *)elan_cmd_write_ptr + size);
    h[15] = size;
    model_header_sq_copy(model,(uint)h);
    model_body_sq_copy(model,(uint)h);
  }
  else {
    model_compile_to_frame = 1;
    elan_cmd_write_ptr = (uint *)model_list_compile(model,h + 0x10,(uint)(h + 0x18),0,-1);
    elan_cmd_write_ptr = (uint *)model_list_compile(model,h + 0x12,(uint)elan_cmd_write_ptr,2,-1);
    elan_cmd_write_ptr = (uint *)model_list_compile(model,h + 0x14,(uint)elan_cmd_write_ptr,4,-1);
    model_compile_to_frame = 0;
    h[0] = 0x100;
    h[6] = (int)elan_cmd_write_ptr - (int)h;
    h[2] = model[2];
    h[3] = model[3];
    h[4] = model[4];
    h[5] = model[5];
    h[7] = 0;
  }
  return h;
}

uint * model_modvol_compile_to_frame_list(int *model)
{
  uint *h;
  uint size;

  h = elan_cmd_write_ptr;
  if (model == (int *)0x0) return 0;
  if (*model == -1) return 0;
  if (*model == 0x100) {
    size = model[6] + 0x1fU & 0xffffffe0;
    elan_cmd_write_ptr = (uint *)((char *)elan_cmd_write_ptr + size);
    h[15] = size;
    model_header_sq_copy(model,(uint)h);
    model_body_sq_copy(model,(uint)h);
  }
  else {
    model_compile_to_frame = 1;
    elan_cmd_write_ptr = (uint *)model_list_compile(model,h + 0x10,(uint)(h + 0x18),1,-1);
    elan_cmd_write_ptr = (uint *)model_list_compile(model,h + 0x12,(uint)elan_cmd_write_ptr,3,-1);
    h[0x14] = 0;
    h[0x15] = 0;
    model_compile_to_frame = 0;
    h[0] = 0x100;
    h[6] = (int)elan_cmd_write_ptr - (int)h;
    h[2] = model[2];
    h[3] = model[3];
    h[4] = model[4];
    h[5] = model[5];
    h[7] = 0;
  }
  return h;
}

uint strip_vertex_code_1(uint n)
{
    if (n >= 3)
        return 1;
    if (n == 2)
        return 3;
    return 0;
}

uint strip_vertex_code_2(uint n)
{
    if (n >= 3)
        return 2;
    if (n == 2)
        return 3;
    return 0;
}

uint argb_pack_clamped(float alpha, float red, float green, float blue)
{
  uint argb;
  int c;
  c = (int)(alpha * 255.0);
  if (c < 0) c = 0; else if (c > 255) c = 255;
  argb = c << 24;
  c = (int)(red * 255.0);
  if (c < 0) c = 0; else if (c > 255) c = 255;
  argb |= c << 16;
  c = (int)(green * 255.0);
  if (c < 0) c = 0; else if (c > 255) c = 255;
  argb |= c << 8;
  c = (int)(blue * 255.0);
  if (c < 0) c = 0; else if (c > 255) c = 255;
  argb |= c;
  return argb;
}

uint model_list_compiled_size(int *model, uint list)
{
  uint *cur;
  int *rec;
  uint *strips;
  uint *va;
  uint *vb;
  uint flags;
  uint word;
  uint rec_list;
  int type;
  int n;
  int i;
  int size;
  int extra;

  size = 0;
  extra = 0;
  if (list == 0 && model_list_enabled == 0) return 0;
  if (list == 1 && model_list1_enabled == 0) return 0;
  if (list == 2 && model_list2_enabled == 0) return 0;
  if (list == 3 && model_list3_enabled == 0) return 0;
  if (list == 4 && model_list4_enabled == 0) return 0;
  if (*model < 0 || model[6] == 0) return 0;
  cur = (uint *)(model + 6);
  do {
    rec = (int *)cur;
    rec_list = *cur >> 24 & 7;
    type = rec[9];
    cur += 20;
    strips = cur;
    if (rec_list == list || ((rec_list == list - 1) & (type != -2))) {
      if (rec_list == list - 1) {
        extra += 32;
        flags = *cur;
      }
      else {
        extra += 96;
        flags = *cur;
      }
      for (;;) {
        n = cur[1];
        if (type != -2) va = cur + 2;
        else vb = cur + 2;
        if (flags >> 3 & 1) n *= 3;
        if (rec_list == list - 1) size += n << 4;
        else if (type >= -1) size += n * 24;
        else if (type == -2) size += n * 40;
        else if (modvol_two_volume == 0) size += n << 5;
        else size += n * 40;
        if (type != -2) {
          for (i = 0; i < n; i++) {
            __asm__ __volatile__ ("pref @%0" : : "r" (va + 8));
            if (*va & 1) va += 8;
            else va += 2;
          }
        }
        else {
          for (i = 0; i < n; i++) vb += 14;
        }
        if (type != -2) cur = va;
        else cur = vb;
        word = *cur;
        if ((int)word <= 0) break;
        flags = word;
        if ((word >> 7 & 1) == 0) extra += 32;
      }
    }
    cur = (uint *)((char *)strips + rec[19]);
  } while (*cur != 0);
  return (size + extra + 31) & ~31;
}

uint float_pair_pack16(float a, float b)
{
  uint high = *(uint *)&a;
  uint low = *(uint *)&b;

  return (high & 0xffff0000) | ((low & 0xffff0000) >> 16);
}

uint model_list_compile(int *model,int *span,uint dst,uint list,int expected_size)
{
  uint start = dst;
  uint *strips;
  uint *rec;
  float *vtx_m2;
  uint ctrl;
  uint addr_hi;
  uint *strip_hdr;
  uint rec_list;
  int count;
  uint init_count;
  uint flags;
  int n;
  uint *va;
  uint *vb;
  int i;
  uint offset_col;
  int env_flag;
  int tri_left;
  uint *p;
  uint *cur;
  float *vtx;
  int type;
  uint base_col;
  int tri_vtx;
  uint skip;
  uint code;
  float f;
  float tex_u;
  float tex_v;
  int pad0[8];
  struct hdr_ctx { uint a[36]; uint w0; uint w1; uint w2; uint w3; uint b[8]; } hdr;
  struct hdr_ctx *h;
  float env_dir[3];
  struct { uint start; uint size; } info;

  h = &hdr;
  span[0] = 0;
  span[1] = 0;
  if (list == 0 && model_list_enabled == 0) return dst;
  if (list == 1 && model_list1_enabled == 0) return dst;
  if (list == 2 && model_list2_enabled == 0) return dst;
  if (list == 3 && model_list3_enabled == 0) return dst;
  if (list == 4 && model_list4_enabled == 0) return dst;
  {
  *(volatile uint *)0xff000038 = dst >> 24;
  *(volatile uint *)0xff00003c = dst >> 24;
  p = (uint *)(dst & 0x3ffffff | 0xe0000000);
  addr_hi = dst & 0xfc000000;
  if (*model < 0 || model[6] == 0) return dst;
  cur = (uint *)(model + 6);
  {
  do {
    rec = cur;
    rec_list = *rec >> 24 & 7;
    type = rec[9];
    cur += 20;
    strips = cur;
    if (rec_list == list || (rec_list == list - 1 && type != -2)) {
      count = 0;
      offset_col = 0;
      env_flag = 0;
      if (model_opaque_as_translucent != 0) f = 0.5;
      else f = ((float *)rec)[11];
      base_col = argb_pack_clamped(f, ((float *)rec)[12], ((float *)rec)[13], ((float *)rec)[14]);
      ((float *)rec)[10] = 1.0;
      flags = *cur;
      if ((flags >> 8 & 1) != 0 && model_compile_to_frame == 0) env_flag = 1;
      if (rec_list != list - 1) {
        if (type > 0) {
          offset_col = argb_pack_clamped(((float *)rec)[15], ((float *)rec)[16], ((float *)rec)[17], ((float *)rec)[18]);
          {
            uint *q;
            p += 8;
            q = p - 1;
            *--q = offset_col;
            *--q = base_col;
            *--q = offset_col;
            *--q = base_col;
            *--q = env_flag << 11 | env_flag << 12 | 0xff;
            *--q = material_param_byte_table[type] & 0xff | (material_param_byte_table[type] << 8) & 0xff00;
            *--q = 0x08000500;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
        else if ((type == 0) | (type == -2)) {
          if (type == -2) base_col &= model_type_m2_color_mask;
          {
            uint *q;
            p += 8;
            q = p - 1;
            *--q = offset_col;
            *--q = base_col;
            *--q = offset_col;
            *--q = base_col;
            *--q = env_flag << 11 | env_flag << 12 | 0xff;
            *--q = 0xff;
            *--q = 0x08000500;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
        else if (type == -1) {
          {
            uint *q;
            p += 8;
            q = p - 1;
            *--q = offset_col;
            *--q = base_col;
            *--q = offset_col;
            *--q = base_col;
            *--q = env_flag << 11 | env_flag << 12 | 0x6ff;
            *--q = 0xff;
            *--q = 0x08000500;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
        else if (type == -3) {
          {
            uint *q;
            p += 8;
            q = p - 1;
            *--q = 0;
            *--q = 0;
            *--q = 0;
            *--q = 0;
            *--q = env_flag << 11 | env_flag << 12 | 0x200;
            *--q = 0xff;
            *--q = 0x08000500;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
      }
      if (rec_list != list - 1) {
        p[0] = 0x08000000;
        p[1] = rec[8];
        p[2] = 0x08000000;
        p[3] = rec[15];
        p[4] = 0x08000000;
        p[5] = rec[8];
        p[6] = 0x08000000;
        p[7] = rec[15];
        list_end_block_count = list_end_block_count + 1 & 3;
        sq_pref(p);
        p += 8;
      }
      h->w0 = rec[0];
      h->w1 = rec[1];
      h->w2 = rec[2];
      h->w3 = rec[3];
      if (rec_list == list - 1) {
        { uint t = h->w0 & 0xf8ffffff; h->w0 = list << 24 | t; }
        h->w0 &= ~8;
        h->w0 &= ~0xc0;
        h->w1 = h->w1 & 0xe7ffffff | 0x08000000;
        h->w1 &= 0x1fffffff;
      }
      else {
        if ((h->w0 & 0xc0) != 0) {
          h->w0 &= ~0xc0;
          if (modvol_two_volume == 0) h->w0 |= 0x80;
          else h->w0 |= 0xc0;
        }
        { uint t = h->w1 & 0xe7ffffff; h->w1 = (flags & 3) << 27 | t; }
      }
      if (rec_list == 0 && model_opaque_as_translucent != 0) {
        h->w0 = h->w0 & 0xf8ffffff | 0x2000000;
        { uint t = h->w2 & 0x1fffffff; h->w2 = model_forced_blend_src << 29 | t; }
        { uint t = h->w2 & 0xe3ffffff; h->w2 = model_forced_blend_dst << 26 | t; }
        h->w2 |= 0x1000c0;
      }
      else if (rec_list == 2 && model_translucent_as_opaque != 0) {
        h->w0 &= 0xf8ffffff;
        h->w2 = h->w2 & 0x1fffffff | 0x20000000;
        h->w2 &= 0xe3ffffff;
      }
      if (type == -2) {
        if (model_type_m2_blend_mode == 0) {
          h->w2 = h->w2 & 0x1fffffff | 0xa0000000;
          h->w2 = h->w2 & 0xe3ffffff | 0x10000000;
        }
        else if (model_type_m2_blend_mode == 1) {
          h->w2 = h->w2 & 0x1fffffff | 0x80000000;
          h->w2 &= 0xe3ffffff;
        }
        else if (model_type_m2_blend_mode == 2) {
          { uint t = h->w2 & 0x1fffffff; h->w2 = model_type_m2_blend_src << 29 | t; }
          { uint t = h->w2 & 0xe3ffffff; h->w2 = model_type_m2_blend_dst << 26 | t; }
        }
      }
      { uint t = h->w0 & ~2; h->w0 = (flags >> 6 & 1) << 1 | t; }
      init_count = 0;
      strip_hdr = p;
      if (rec_list == list - 1) {
        {
          uint *q;
          p += 8;
          q = p;
          *--q = init_count;
          *--q = 2;
          *--q = 0;
          *--q = 0;
          *--q = h->w3;
          *--q = h->w2;
          *--q = h->w1;
          *--q = h->w0 | 0x08000700;
          list_end_block_count = list_end_block_count + 1 & 3;
          sq_pref(q);
        }
      }
      else if (type >= -1) {
        if (modvol_two_volume == 0) {
          {
            uint *q;
            p += 8;
            q = p;
            *--q = init_count;
            *--q = 10;
            *--q = 0;
            *--q = 0;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w1;
            *--q = h->w0 | 0x08000700;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
        else {
          {
            uint *q;
            p += 8;
            q = p;
            *--q = init_count;
            *--q = 0x12;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w1;
            *--q = h->w0 | 0x08000700;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
      }
      else if (type == -2) {
        if (modvol_two_volume == 0) {
          {
            uint *q;
            p += 8;
            q = p;
            *--q = init_count;
            *--q = 0x10a;
            *--q = 0;
            *--q = 0;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w1;
            *--q = h->w0 | 0x08000700;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
        else {
          {
            uint *q;
            p += 8;
            q = p;
            *--q = init_count;
            *--q = 0x10a;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w1;
            *--q = h->w0 | 0x08000700;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
      }
      else {
        if (modvol_two_volume == 0) {
          {
            uint *q;
            p += 8;
            q = p;
            *--q = init_count;
            *--q = 0x4a;
            *--q = 0;
            *--q = 0;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w1;
            *--q = h->w0 | 0x08000700;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
        else {
          {
            uint *q;
            p += 8;
            q = p;
            *--q = init_count;
            *--q = 0xd2;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w3;
            *--q = h->w2;
            *--q = h->w1;
            *--q = h->w0 | 0x08000700;
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
          }
        }
      }
      for (;;) {
        n = cur[1];
        if (type != -2) va = cur + 2;
        else vb = cur + 2;
        if ((flags >> 3 & 1) != 0) {
          n = n * 3;
          tri_left = 3;
        }
        count += n;
        for (i = 0; i < n; i++) {
          if (type != -2) {
            sq_pref(va + 8);
            if ((*va & 1) != 0) {
              vtx = (float *)va;
              va += 8;
            }
            else {
              skip = va[1];
              va += 2;
              vtx = (float *)((char *)va + skip);
            }
          }
          else {
            sq_pref(vb + 8);
            vtx_m2 = (float *)vb;
            vb += 14;
          }
          if ((flags >> 3 & 1) == 0) {
            if ((flags >> 4 & 1) == 0) code = strip_vertex_code_2(i);
            else code = strip_vertex_code_1(i);
            if (rec_list == list - 1) {
              uint code_bits = code << 29 & 0x60000000;
              if (i == n - 1) ctrl = code_bits | 0x80000000;
              else ctrl = code_bits;
            }
            else if (type >= -1) {
              uint code_bits = code << 29 & 0x60000000;
              uint normal_bytes = (uchar)(int)(vtx[3] * 127.0) | (uchar)(int)(vtx[4] * 127.0) << 8 | (uchar)(int)(vtx[5] * 127.0) << 16;
              if (i == n - 1) ctrl = code_bits | normal_bytes | 0x80000000;
              else ctrl = code_bits | normal_bytes;
            }
            else if (type == -2) {
              uint code_bits = code << 29 & 0x60000000;
              uint normal_bytes = (uchar)(int)(vtx_m2[3] * 127.0) | (uchar)(int)(vtx_m2[4] * 127.0) << 8 | (uchar)(int)(vtx_m2[5] * 127.0) << 16;
              if (i == n - 1) ctrl = code_bits | normal_bytes | 0x80000000;
              else ctrl = code_bits | normal_bytes;
            }
            else if (type == -3) {
              uchar *col_bytes = (uchar *)vtx + 12;
              uint code_bits = code << 29 & 0x60000000;
              uint normal_bytes = col_bytes[2] | col_bytes[1] << 8 | col_bytes[0] << 16;
              if (i == n - 1) ctrl = code_bits | normal_bytes | 0x80000000;
              else ctrl = code_bits | normal_bytes;
            }
          }
          else {
            tri_vtx = 3 - tri_left;
            code = strip_vertex_code_1(tri_vtx);
            if (rec_list == list - 1) {
              uint code_bits = code << 29 & 0x60000000;
              if (tri_vtx == 2) ctrl = code_bits | 0x80000000;
              else ctrl = code_bits;
            }
            else if (type >= -1) {
              uint code_bits = code << 29 & 0x60000000;
              uint normal_bytes = (uchar)(int)(vtx[3] * 127.0) | (uchar)(int)(vtx[4] * 127.0) << 8 | (uchar)(int)(vtx[5] * 127.0) << 16;
              if (tri_vtx == 2) ctrl = code_bits | normal_bytes | 0x80000000;
              else ctrl = code_bits | normal_bytes;
            }
            else if (type == -2) {
              uint code_bits = strip_vertex_code_1(tri_vtx) << 29 & 0x60000000;
              uint normal_bytes = (uchar)(int)(vtx_m2[3] * 127.0) | (uchar)(int)(vtx_m2[4] * 127.0) << 8 | (uchar)(int)(vtx_m2[5] * 127.0) << 16;
              if (tri_vtx == 2) ctrl = code_bits | normal_bytes | 0x80000000;
              else ctrl = code_bits | normal_bytes;
            }
            else if (type == -3) {
              uchar *col_bytes = (uchar *)vtx + 12;
              uint code_bits = strip_vertex_code_1(tri_vtx) << 29 & 0x60000000;
              uint normal_bytes = col_bytes[2] | col_bytes[1] << 8 | col_bytes[0] << 16;
              if (tri_vtx == 2) ctrl = code_bits | normal_bytes | 0x80000000;
              else ctrl = code_bits | normal_bytes;
            }
            if (--tri_left == 0) tri_left = 3;
          }
          if (rec_list == list - 1) {
            float *fp = (float *)(p + 4);
            *--fp = vtx[2];
            *--fp = vtx[1];
            *--fp = vtx[0];
            p[0] = ctrl;
            if (((uint)p & 0x1f) == 0x10) {
              list_end_block_count = list_end_block_count + 1 & 3;
              sq_pref(p);
            }
            p += 4;
          }
          else if (type >= -1) {
            if ((flags >> 8 & 1) != 0 && model_compile_to_frame != 0) {
              mtx_xform_dir(vtx + 3, env_dir);
              tex_u = (env_dir[0] + 1.0) * 0.5 + elan_matrix_params.env_u_offset;
              tex_v = (env_dir[1] + 1.0) * 0.5 + elan_matrix_params.env_v_offset;
            }
            else {
              tex_u = vtx[6];
              tex_v = vtx[7];
            }
            if (modvol_two_volume == 0) {
              float *fp = (float *)(p + 6);
              *--fp = tex_v;
              *--fp = tex_u;
              *--fp = vtx[2];
              *--fp = vtx[1];
              *--fp = vtx[0];
              p[0] = ctrl;
            }
            else {
              float *fp = (float *)(p + 6);
              *--fp = tex_v;
              *--fp = tex_u;
              *--fp = vtx[2];
              *--fp = vtx[1];
              *--fp = vtx[0];
              p[0] = ctrl;
            }
            if (((uint)p & 0x1f) != 0) {
              list_end_block_count = list_end_block_count + 1 & 3;
              sq_pref(p);
            }
            p += 6;
          }
          else if (type == -2) {
            uint *q;
            f = light_ambient_color.field_0x0 * 255.0;
            if (modvol_two_volume == 0) {
              p[8] = (uchar)(int)(vtx_m2[9] * 127.0) | (uchar)(int)(vtx_m2[10] * 127.0) << 8 | (uchar)(int)(vtx_m2[11] * 127.0) << 16;
              p[7] = (uchar)(int)(vtx_m2[6] * 127.0) | (uchar)(int)(vtx_m2[7] * 127.0) << 8 | (uchar)(int)(vtx_m2[8] * 127.0) << 16;
              p[6] = (uchar)(int)(((float *)rec)[15] * 255.0) + ((uchar)(int)f << 8);
              q = p + 6;
              *(float *)--q = vtx_m2[13];
              *(float *)--q = vtx_m2[12];
              *(float *)--q = vtx_m2[2];
              *(float *)--q = vtx_m2[1];
              if (((uint)p & 0x1f) == 0x18) {
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
              *(float *)--q = vtx_m2[0];
              *--q = ctrl;
            }
            else {
              p[8] = (uchar)(int)(vtx_m2[9] * 127.0) | (uchar)(int)(vtx_m2[10] * 127.0) << 8 | (uchar)(int)(vtx_m2[11] * 127.0) << 16;
              p[7] = (uchar)(int)(vtx_m2[6] * 127.0) | (uchar)(int)(vtx_m2[7] * 127.0) << 8 | (uchar)(int)(vtx_m2[8] * 127.0) << 16;
              p[6] = (uchar)(int)(((float *)rec)[15] * 255.0) + ((uchar)(int)f << 8);
              q = p + 6;
              *(float *)--q = vtx_m2[13];
              *(float *)--q = vtx_m2[12];
              *(float *)--q = vtx_m2[2];
              *(float *)--q = vtx_m2[1];
              if (((uint)p & 0x1f) == 0x18) {
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
              *(float *)--q = vtx_m2[0];
              *--q = ctrl;
            }
            list_end_block_count = list_end_block_count + 1 & 3;
            sq_pref(q);
            p += 10;
          }
          else if (type == -3) {
            if (modvol_two_volume == 0) {
              uint *q;
              p += 8;
              q = p;
              *--q = ((uint *)vtx)[5];
              *--q = ((uint *)vtx)[4];
              *(float *)--q = vtx[7];
              *(float *)--q = vtx[6];
              *(float *)--q = vtx[2];
              *(float *)--q = vtx[1];
              *(float *)--q = vtx[0];
              *--q = ctrl;
              list_end_block_count = list_end_block_count + 1 & 3;
              sq_pref(q);
            }
            else {
              uint *q;
              p[9] = ((uint *)vtx)[5];
              p[8] = ((uint *)vtx)[4];
              p[7] = ((uint *)vtx)[5];
              p[6] = ((uint *)vtx)[4];
              q = p + 6;
              *(float *)--q = vtx[7];
              *(float *)--q = vtx[6];
              *(float *)--q = vtx[2];
              *(float *)--q = vtx[1];
              if (((uint)p & 0x1f) == 0x18) {
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
              *(float *)--q = vtx[0];
              *--q = ctrl;
              list_end_block_count = list_end_block_count + 1 & 3;
              sq_pref(q);
              p += 10;
            }
          }
        }
        if (type != -2) cur = va;
        else cur = vb;
        if ((int)*cur <= 0) break;
        flags = *cur;
        if ((flags >> 7 & 1) == 0) {
          uint init_count2;
          { uint t = h->w0 & ~2; h->w0 = (flags >> 6 & 1) << 1 | t; }
          if (rec_list == list - 1) h->w1 = h->w1 & 0xe7ffffff | 0x08000000;
          else { uint t = h->w1 & 0xe7ffffff; h->w1 = (flags & 3) << 27 | t; }
          init_count2 = 0;
          *(int *)(((uint)strip_hdr & 0x3ffffff | addr_hi) + 0x1c) = count;
          strip_hdr = p;
          count = 0;
          if (rec_list == list - 1) {
            {
              uint *q;
              p += 8;
              q = p;
              *--q = init_count2;
              *--q = 2;
              *--q = 0;
              *--q = 0;
              *--q = h->w3;
              *--q = h->w2;
              *--q = h->w1;
              *--q = h->w0 | 0x08000700;
              list_end_block_count = list_end_block_count + 1 & 3;
              sq_pref(q);
            }
          }
          else if (type >= -1) {
            if (modvol_two_volume == 0) {
              {
                uint *q;
                p += 8;
                q = p;
                *--q = init_count2;
                *--q = 10;
                *--q = 0;
                *--q = 0;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w1;
                *--q = h->w0 | 0x08000700;
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
            }
            else {
              {
                uint *q;
                p += 8;
                q = p;
                *--q = init_count2;
                *--q = 0x12;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w1;
                *--q = h->w0 | 0x08000700;
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
            }
          }
          else if (type == -2) {
            if (modvol_two_volume == 0) {
              {
                uint *q;
                p += 8;
                q = p;
                *--q = init_count2;
                *--q = 0x10a;
                *--q = 0;
                *--q = 0;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w1;
                *--q = h->w0 | 0x08000700;
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
            }
            else {
              {
                uint *q;
                p += 8;
                q = p;
                *--q = init_count2;
                *--q = 0x10a;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w1;
                *--q = h->w0 | 0x08000700;
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
            }
          }
          else {
            if (modvol_two_volume == 0) {
              {
                uint *q;
                p += 8;
                q = p;
                *--q = init_count2;
                *--q = 0x4a;
                *--q = 0;
                *--q = 0;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w1;
                *--q = h->w0 | 0x08000700;
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
            }
            else {
              {
                uint *q;
                p += 8;
                q = p;
                *--q = init_count2;
                *--q = 0xd2;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w3;
                *--q = h->w2;
                *--q = h->w1;
                *--q = h->w0 | 0x08000700;
                list_end_block_count = list_end_block_count + 1 & 3;
                sq_pref(q);
              }
            }
          }
        }
      }
      *(int *)(((uint)strip_hdr & 0x3ffffff | addr_hi) + 0x1c) = count;
    }
    cur = (uint *)((char *)strips + rec[0x13]);
  } while (*cur != 0);
  if (((uint)p & 0x18) != 0) {
    if (((uint)p & 0x18) == 0x18) {
      p[0] = 0x08000000;
      p += 2;
    }
    else if (((uint)p & 0x18) == 0x10) {
      p[0] = 0x08000000;
      p[2] = 0x08000000;
      p += 4;
    }
    else if (((uint)p & 0x18) == 8) {
      p[0] = 0x08000000;
      p[2] = 0x08000000;
      p[4] = 0x08000000;
      p += 6;
    }
    list_end_block_count = list_end_block_count + 1 & 3;
    sq_pref(p - 8);
  }
  dst = (uint)p & 0x3ffffff | addr_hi;
  if (dst != start) {
    if (expected_size >= 0 && expected_size != dst - start) {
      model_error_record.error_bits |= 0x4000000;
      model_error_record.field_0x4 = expected_size;
      model_error_record.field_0x8 = start;
      model_error_record.field_0xc = dst;
      model_error_record.field_0x10 = list;
      model_error_record.field_0x14 = (uint)&info;
      model_error_record.field_0x18 = (uint)cur;
      model_error_record.field_0x1c = 0;
      __asm__ volatile ("trapa #0x3a");
    }
    info.size = dst - start;
    info.start = start;
    span[0] = info.start - (int)span;
    span[1] = info.size;
  }
  }
  }
  return dst;
}

list_block_ref * dummy_tri_list_build(list_block_ref *rec, int kind)
{
  struct { uint pad[36]; uint w0, w1, w2, w3; uint r[8]; } hdr;
  int is_kind0;
  uint *p;
  uint *q;
  uint *start;
  uint addr_hi;
  int modvol;
  uint cmd;
  float y;
  
  start = (uint *)rec + 8;
  is_kind0 = kind == 0;
  modvol = is_kind0 == 0;
  *(volatile uint *)0xff000038 = (uint)start >> 0x18;
  *(volatile uint *)0xff00003c = (uint)start >> 0x18;
  p = (uint *)((uint)start & 0x3ffffff | 0xe0000000);
  addr_hi = (uint)start & 0xfc000000;
  hdr.w0 = modvol << 0x18 | modvol << 6 | 0x80000021;
  hdr.w1 = kind << 0x1d | 0x10400000;
  hdr.w2 = 0x20080440;
  hdr.w3 = 0;
  p = p + 8;
  q = p;
  *--q = 3;
  *--q = 2;
  *--q = 0;
  *--q = 0;
  *--q = hdr.w3;
  *--q = hdr.w2;
  *--q = hdr.w1;
  *--q = hdr.w0 | 0x08000700;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  cmd = 0;
  if (modvol == 0) {
    p[3] = 0xff800000;
    p[2] = 0xff800000;
    p[1] = 0xff800000;
    p[0] = cmd;
    if (((uint)p & 0x1f) == 0x10) {
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
    }
    p = p + 4;
    p[3] = 0xff800000;
    p[2] = 0xff800000;
    p[1] = 0xff800000;
    p[0] = cmd;
    if (((uint)p & 0x1f) == 0x10) {
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
    }
    p = p + 4;
    cmd = 0xe0000000;
    p[3] = 0xff800000;
    p[2] = 0xff800000;
    p[1] = 0xff800000;
  }
  else {
    ((float *)p)[3] = 0.0;
    ((float *)p)[2] = 0.0;
    ((float *)p)[1] = 0.0;
    p[0] = cmd;
    if (((uint)p & 0x1f) == 0x10) {
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
    }
    p = p + 4;
    ((float *)p)[3] = 0.0;
    ((float *)p)[2] = y = 0.1;
    ((float *)p)[1] = 0.0;
    p[0] = cmd;
    if (((uint)p & 0x1f) == 0x10) {
      list_end_block_count = list_end_block_count + 1 & 3;
      __asm__ __volatile__ ("pref @%0" : : "r" (p));
    }
    p = p + 4;
    cmd = 0xe0000000;
    ((float *)p)[3] = 0.0;
    ((float *)p)[2] = y = 0.05;
    ((float *)p)[1] = 0.0;
  }
  p[0] = cmd;
  if (((uint)p & 0x1f) == 0x10) {
    list_end_block_count = list_end_block_count + 1 & 3;
    __asm__ __volatile__ ("pref @%0" : : "r" (p));
  }
  p = p + 4;
  if (((uint)p & 0x18) != 0) {
    if (((uint)p & 0x18) == 0x18) {
      p[0] = 0x08000000;
      p = p + 2;
    }
    else if (((uint)p & 0x18) == 0x10) {
      p[0] = 0x08000000;
      p[2] = 0x08000000;
      p = p + 4;
    }
    else if (((uint)p & 0x18) == 8) {
      p[0] = 0x08000000;
      p[2] = 0x08000000;
      p[4] = 0x08000000;
      p = p + 6;
    }
    list_end_block_count = list_end_block_count + 1 & 3;
    __asm__ __volatile__ ("pref @%0" : : "r" (p - 8));
  }
  rec->length = ((uint)p & 0x3ffffff | addr_hi) - (int)start;
  rec->addr = (uint)start;
  return rec;
}
