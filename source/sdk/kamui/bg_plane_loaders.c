/*
 * sdk/kamui/bg_plane_loaders.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

uint bg_plane_tag_bits_set(uint skip, int shadow, int bypass)
{
  uint bits;
  gfx_state.bg_tag_bits = bits = (skip & 7) << 0x18;
  if (bypass != 0) {
    gfx_state.bg_tag_bits |= 0x10000000;
  }
  if (shadow != 0) {
    gfx_state.bg_tag_bits |= 0x08000000;
  }
  return bits;
}

int bg_plane_skip_from_isp_word(uint isp)
{
  int skip;
  uint a;
  uint b;
  int ta;
  int tb;

  skip = 1;
  if ((isp & 0x2000000) != 0) {
    a = isp & 0x1000000;
    ta = a == 0;
    skip = ta == 0;
    skip += 2;
    b = isp & 0x400000;
    tb = b == 0;
    skip += tb != 0;
  }
  return skip;
}

int bg_plane_append_offset(uint isp, int n, uint color)
{
  if ((isp & 0x1000000) != 0) {
    gfx_state.bg_plane[n] = color;
    n++;
  }
  return n;
}

int bg_plane_append_uv16(uint isp, int n, uint uv)
{
  if ((isp & 0x2000000) != 0) {
    if ((isp & 0x400000) != 0) {
      gfx_state.bg_plane[n++] = uv;
    }
    else {
      gfx_state.bg_plane[n++] = uv & 0xffff0000;
      gfx_state.bg_plane[n++] = uv << 0x10;
    }
  }
  return n;
}

int bg_plane_append_uv_floats(uint isp, int n, uint u, uint v)
{
  if ((isp & 0x2000000) != 0) {
    gfx_state.bg_plane[n] = u;
    n++;
    gfx_state.bg_plane[n] = v;
    n++;
  }
  return n;
}

int bg_plane_defaults_set(void)
{
  gfx_state.bg_plane[0] = 0x80800000;
  gfx_state.bg_plane[1] = 0x20800000;
  gfx_state.bg_plane[2] = 0;
  gfx_state.bg_plane[3] = 0x40000000;
  gfx_state.bg_plane[4] = 0x43820000;
  gfx_state.bg_plane[5] = 0x3727c5ac;
  gfx_state.bg_plane[6] = 0xff000000;
  gfx_state.bg_plane[7] = 0x43160000;
  gfx_state.bg_plane[8] = 0x40000000;
  gfx_state.bg_plane[9] = 0x3727c5ac;
  gfx_state.bg_plane[10] = 0xff000000;
  gfx_state.bg_plane[11] = 0x440c0000;
  gfx_state.bg_plane[12] = 0x43820000;
  gfx_state.bg_plane[13] = 0x3727c5ac;
  gfx_state.bg_plane[14] = 0xff000000;
  gfx_state.bg_depth = gfx_state.bg_plane[5];
  gfx_state.bg_tag_bits = 0x1000000;
  return 0;
}

int bg_plane_from_flat_packed(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  gfx_state.bg_plane[n++] = vtx.m[0][6];
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  gfx_state.bg_plane[n++] = vtx.m[1][6];
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  gfx_state.bg_plane[n++] = vtx.m[2][6];
  gfx_state.bg_plane_bytes = 0x3c;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_flat_argb_floats(int *vertex_ptrs)
{
  typedef struct { int *a; int *b; int *c; } P;
  P p;
  int isp, tsp, tex;
  int n;
#define COL(q) ((int)(*(float *)((q) + 4) * 255.0) << 24 | (int)(*(float *)((q) + 5) * 255.0) << 16 | (int)(*(float *)((q) + 6) * 255.0) << 8 | (int)(*(float *)((q) + 7) * 255.0))

  p = *(P *)vertex_ptrs;
  isp = gfx_state.bg_isp_word;
  tsp = gfx_state.bg_tsp_word;
  tex = gfx_state.bg_tex_word;
  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = p.a[1];
  gfx_state.bg_plane[n++] = p.a[2];
  gfx_state.bg_plane[n++] = p.a[3];
  gfx_state.bg_plane[n++] = COL(p.a);
  gfx_state.bg_plane[n++] = p.b[1];
  gfx_state.bg_plane[n++] = p.b[2];
  gfx_state.bg_plane[n++] = p.b[3];
  gfx_state.bg_plane[n++] = COL(p.b);
  gfx_state.bg_plane[n++] = p.c[1];
  gfx_state.bg_plane[n++] = p.c[2];
  gfx_state.bg_plane[n++] = p.c[3];
  gfx_state.bg_plane[n++] = COL(p.c);
  gfx_state.bg_plane_bytes = 0x3c;
  bg_plane_tag_bits_set(bg_plane_skip_from_isp_word(isp), 0, 0);
  gfx_state.bg_depth = p.a[3];
  return 0;
#undef COL
}

int bg_plane_from_flat_intensity(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0);
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0);
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0);
  gfx_state.bg_plane_bytes = 0x3c;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_packed(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[0][4], vtx.m[0][5]);
  gfx_state.bg_plane[n++] = vtx.m[0][6];
  n = bg_plane_append_offset(isp, n, vtx.m[0][7]);
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[1][4], vtx.m[1][5]);
  gfx_state.bg_plane[n++] = vtx.m[1][6];
  n = bg_plane_append_offset(isp, n, vtx.m[1][7]);
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[2][4], vtx.m[2][5]);
  gfx_state.bg_plane[n++] = vtx.m[2][6];
  n = bg_plane_append_offset(isp, n, vtx.m[2][7]);
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_packed_uv16(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = vtx.m[0][6];
  n = bg_plane_append_offset(isp, n, vtx.m[0][7]);
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[1][4]);
  gfx_state.bg_plane[n++] = vtx.m[1][6];
  n = bg_plane_append_offset(isp, n, vtx.m[0][7]);
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[2][4]);
  gfx_state.bg_plane[n++] = vtx.m[2][6];
  n = bg_plane_append_offset(isp, n, vtx.m[0][7]);
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_argb_floats(int *vertex_ptrs)
{
  typedef struct { char *a; char *b; char *c; } trio;
  trio v;
  int n;
  uint isp;
  uint tsp;
  uint tex;

  v = *(trio *)vertex_ptrs;
  isp = gfx_state.bg_isp_word;
  tsp = gfx_state.bg_tsp_word;
  tex = gfx_state.bg_tex_word;
  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = *(int *)(v.a + 4);
  gfx_state.bg_plane[n++] = *(int *)(v.a + 8);
  gfx_state.bg_plane[n++] = *(int *)(v.a + 12);
  n = bg_plane_append_uv_floats(isp, n, *(int *)(v.a + 16), *(int *)(v.a + 20));
  gfx_state.bg_plane[n++] = (int)(*(float *)(v.a + 32) * 255.0) << 24 | (int)(*(float *)(v.a + 36) * 255.0) << 16 | (int)(*(float *)(v.a + 40) * 255.0) << 8 | (int)(*(float *)(v.a + 44) * 255.0);
  n = bg_plane_append_offset(isp, n, (int)(*(float *)(v.a + 48) * 255.0) << 24 | (int)(*(float *)(v.a + 52) * 255.0) << 16 | (int)(*(float *)(v.a + 56) * 255.0) << 8 | (int)(*(float *)(v.a + 60) * 255.0));
  gfx_state.bg_plane[n++] = *(int *)(v.b + 4);
  gfx_state.bg_plane[n++] = *(int *)(v.b + 8);
  gfx_state.bg_plane[n++] = *(int *)(v.b + 12);
  n = bg_plane_append_uv_floats(isp, n, *(int *)(v.b + 16), *(int *)(v.b + 20));
  gfx_state.bg_plane[n++] = (int)(*(float *)(v.b + 32) * 255.0) << 24 | (int)(*(float *)(v.b + 36) * 255.0) << 16 | (int)(*(float *)(v.b + 40) * 255.0) << 8 | (int)(*(float *)(v.b + 44) * 255.0);
  n = bg_plane_append_offset(isp, n, (int)(*(float *)(v.b + 48) * 255.0) << 24 | (int)(*(float *)(v.b + 52) * 255.0) << 16 | (int)(*(float *)(v.b + 56) * 255.0) << 8 | (int)(*(float *)(v.b + 60) * 255.0));
  gfx_state.bg_plane[n++] = *(int *)(v.c + 4);
  gfx_state.bg_plane[n++] = *(int *)(v.c + 8);
  gfx_state.bg_plane[n++] = *(int *)(v.c + 12);
  n = bg_plane_append_uv_floats(isp, n, *(int *)(v.c + 16), *(int *)(v.c + 20));
  gfx_state.bg_plane[n++] = (int)(*(float *)(v.c + 32) * 255.0) << 24 | (int)(*(float *)(v.c + 36) * 255.0) << 16 | (int)(*(float *)(v.c + 40) * 255.0) << 8 | (int)(*(float *)(v.c + 44) * 255.0);
  n = bg_plane_append_offset(isp, n, (int)(*(float *)(v.c + 48) * 255.0) << 24 | (int)(*(float *)(v.c + 52) * 255.0) << 16 | (int)(*(float *)(v.c + 56) * 255.0) << 8 | (int)(*(float *)(v.c + 60) * 255.0));
  gfx_state.bg_plane_bytes = n << 2;
  bg_plane_tag_bits_set(bg_plane_skip_from_isp_word(isp), 0, 0);
  gfx_state.bg_depth = *(int *)(v.a + 12);
  return 0;
}

int bg_plane_from_tex_argb_floats_uv16(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = (int)(*(float *)(vtx.m[0] + 8) * 255.0) << 24 | (int)(*(float *)(vtx.m[0] + 9) * 255.0) << 16 | (int)(*(float *)(vtx.m[0] + 10) * 255.0) << 8 | (int)(*(float *)(vtx.m[0] + 11) * 255.0);
  n = bg_plane_append_offset(isp, n, (int)(*(float *)(vtx.m[0] + 12) * 255.0) << 24 | (int)(*(float *)(vtx.m[0] + 13) * 255.0) << 16 | (int)(*(float *)(vtx.m[0] + 14) * 255.0) << 8 | (int)(*(float *)(vtx.m[0] + 15) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = (int)(*(float *)(vtx.m[1] + 8) * 255.0) << 24 | (int)(*(float *)(vtx.m[1] + 9) * 255.0) << 16 | (int)(*(float *)(vtx.m[1] + 10) * 255.0) << 8 | (int)(*(float *)(vtx.m[1] + 11) * 255.0);
  n = bg_plane_append_offset(isp, n, (int)(*(float *)(vtx.m[1] + 12) * 255.0) << 24 | (int)(*(float *)(vtx.m[1] + 13) * 255.0) << 16 | (int)(*(float *)(vtx.m[1] + 14) * 255.0) << 8 | (int)(*(float *)(vtx.m[1] + 15) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = (int)(*(float *)(vtx.m[2] + 8) * 255.0) << 24 | (int)(*(float *)(vtx.m[2] + 9) * 255.0) << 16 | (int)(*(float *)(vtx.m[2] + 10) * 255.0) << 8 | (int)(*(float *)(vtx.m[2] + 11) * 255.0);
  n = bg_plane_append_offset(isp, n, (int)(*(float *)(vtx.m[2] + 12) * 255.0) << 24 | (int)(*(float *)(vtx.m[2] + 13) * 255.0) << 16 | (int)(*(float *)(vtx.m[2] + 14) * 255.0) << 8 | (int)(*(float *)(vtx.m[2] + 15) * 255.0));
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_intensity(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[0][4], vtx.m[0][5]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0));
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0);
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[1][4], vtx.m[1][5]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[2][4], vtx.m[2][5]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0));
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_intensity_uv16(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0));
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_flat_packed_2vol(int *vertex_ptrs)
{
  typedef struct { int *a; int *b; int *c; } P;
  P p;
  int isp, tsp, tex;
  int n;

  p = *(P *)vertex_ptrs;
  isp = gfx_state.bg_isp_word;
  tsp = gfx_state.bg_tsp_word;
  tex = gfx_state.bg_tex_word;
  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = p.a[1];
  gfx_state.bg_plane[n++] = p.a[2];
  gfx_state.bg_plane[n++] = p.a[3];
  gfx_state.bg_plane[n++] = p.a[4];
  gfx_state.bg_plane[n++] = p.b[1];
  gfx_state.bg_plane[n++] = p.b[2];
  gfx_state.bg_plane[n++] = p.b[3];
  gfx_state.bg_plane[n++] = p.b[4];
  gfx_state.bg_plane[n++] = p.c[1];
  gfx_state.bg_plane[n++] = p.c[2];
  gfx_state.bg_plane[n++] = p.c[3];
  gfx_state.bg_plane[n++] = p.c[4];
  gfx_state.bg_plane_bytes = 0x3c;
  bg_plane_tag_bits_set(bg_plane_skip_from_isp_word(isp), 0, 0);
  gfx_state.bg_depth = p.a[3];
  return 0;
}

int bg_plane_from_flat_intensity_2vol(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  gfx_state.bg_plane[n++] = (int)(*(float *)(vtx.m[0] + 4) * 255.0f) << 24 | (int)(*(float *)(vtx.m[0] + 4) * 255.0f) << 16 | (int)(*(float *)(vtx.m[0] + 4) * 255.0f) << 8 | (int)(*(float *)(vtx.m[0] + 4) * 255.0f);
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  gfx_state.bg_plane[n++] = (int)(*(float *)(vtx.m[1] + 4) * 255.0f) << 24 | (int)(*(float *)(vtx.m[1] + 4) * 255.0f) << 16 | (int)(*(float *)(vtx.m[1] + 4) * 255.0f) << 8 | (int)(*(float *)(vtx.m[1] + 4) * 255.0f);
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  gfx_state.bg_plane[n++] = (int)(*(float *)(vtx.m[2] + 4) * 255.0f) << 24 | (int)(*(float *)(vtx.m[2] + 4) * 255.0f) << 16 | (int)(*(float *)(vtx.m[2] + 4) * 255.0f) << 8 | (int)(*(float *)(vtx.m[2] + 4) * 255.0f);
  gfx_state.bg_plane_bytes = 0x3c;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_packed_2vol(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[0][4], vtx.m[0][5]);
  gfx_state.bg_plane[n++] = vtx.m[0][6];
  n = bg_plane_append_offset(isp, n, vtx.m[0][7]);
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[1][4], vtx.m[1][5]);
  gfx_state.bg_plane[n++] = vtx.m[1][6];
  n = bg_plane_append_offset(isp, n, vtx.m[1][7]);
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[2][4], vtx.m[2][5]);
  gfx_state.bg_plane[n++] = vtx.m[2][6];
  n = bg_plane_append_offset(isp, n, vtx.m[2][7]);
  gfx_state.bg_plane_bytes = n << 2;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_packed_uv16_2vol(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = vtx.m[0][6];
  n = bg_plane_append_offset(isp, n, vtx.m[0][7]);
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = vtx.m[1][6];
  n = bg_plane_append_offset(isp, n, vtx.m[1][7]);
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[2][4]);
  gfx_state.bg_plane[n++] = vtx.m[2][6];
  n = bg_plane_append_offset(isp, n, vtx.m[2][7]);
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_intensity_2vol(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[0][4], vtx.m[0][5]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[1][4], vtx.m[1][5]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv_floats(isp, n, vtx.m[2][4], vtx.m[2][5]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0));
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_from_tex_intensity_uv16_2vol(int *vertex_ptrs)
{
  typedef struct { int *m[3]; } ptrs;
  ptrs vtx = *(ptrs *)vertex_ptrs;
  uint isp = gfx_state.bg_isp_word;
  uint tsp = gfx_state.bg_tsp_word;
  uint tex = gfx_state.bg_tex_word;
  int n;
  int skip;

  n = 0;
  gfx_state.bg_plane[n++] = isp;
  gfx_state.bg_plane[n++] = tsp;
  gfx_state.bg_plane[n++] = tex;
  gfx_state.bg_plane[n++] = vtx.m[0][1];
  gfx_state.bg_plane[n++] = vtx.m[0][2];
  gfx_state.bg_plane[n++] = vtx.m[0][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[0][4]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[0] + 7) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[1][1];
  gfx_state.bg_plane[n++] = vtx.m[1][2];
  gfx_state.bg_plane[n++] = vtx.m[1][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[1][4]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[1] + 7) * 255.0));
  gfx_state.bg_plane[n++] = vtx.m[2][1];
  gfx_state.bg_plane[n++] = vtx.m[2][2];
  gfx_state.bg_plane[n++] = vtx.m[2][3];
  n = bg_plane_append_uv16(isp, n, vtx.m[2][4]);
  gfx_state.bg_plane[n++] = (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 6) * 255.0);
  n = bg_plane_append_offset(isp, n, (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 24 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 16 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0) << 8 | (uint)(int)(*(float *)(vtx.m[2] + 7) * 255.0));
  gfx_state.bg_plane_bytes = n * 4;
  skip = bg_plane_skip_from_isp_word(isp);
  bg_plane_tag_bits_set(skip,0,0);
  gfx_state.bg_depth = vtx.m[0][3];
  return 0;
}

int bg_plane_refuse_sprite(void)
{
  return 10;
}

int bg_plane_refuse_sprite_textured(void)
{
  return 10;
}

int bg_plane_refuse_modifier_volume(void)
{
  return 10;
}
