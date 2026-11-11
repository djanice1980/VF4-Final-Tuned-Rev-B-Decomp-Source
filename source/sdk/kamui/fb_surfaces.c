/*
 * sdk/kamui/fb_surfaces.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint fb_surface_slot; /* 0x0c32a764 */
extern render_setup_rec render_setup; /* 0x0c41df94 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */

int fb_vram_alloc(surface_rec *surface, uint word, uint height, int strip_mode, int clear)
{
  int r;
  uint i;
  uint off;
  uint buf [640];
  uint tbl [6];
  char *a;
  char *e;

  tbl[0] = 1;
  tbl[1] = 1;
  tbl[2] = 1;
  tbl[3] = 1;
  tbl[4] = 2;
  tbl[5] = 3;
  a = (char *)&render_setup;
  e = a + fb_surface_slot * 16;
  *(surface_rec **)(e + 0x1c) = surface;
  *(uint *)(e + 0x20) = word;
  *(uint *)(e + 0x24) = height;
  render_setup.fb_mode = strip_mode;
  *(int *)(e + 0x28) = clear;
  fb_surface_slot = fb_surface_slot + 1 & 1;
  gfx_state.fb_sof_hold = strip_mode;
  if (*(volatile int *)(a + 0x54) != 0) {
    fb_size_from_display_mode(0xff,strip_mode,height);
  }
  r = setup_steps_read();
  if (r == -0x80000000) {
    r = fb_surface_alloc_paired(render_setup.fb_bytes,surface);
    if (r != 0) {
      return r;
    }
    surface->kind = 1;
    surface->texel_size_class = tbl[gfx_state.fb_format];
    surface->pixel_format = gfx_state.fb_format;
    surface->width = render_setup.fb_width;
    surface->height = render_setup.fb_height;
    if (strip_mode == 1) {
      surface->format_flags = 1;
    }
    else {
      surface->format_flags = 0;
    }
    if (clear != 1) {
      return 0;
    }
    for (i = 0; i < 0x280; i++) {
      buf[i] = 0;
    }
    for (off = 0; off < surface->bytes; off += 0xa00) {
      vram32_write(off + surface->tex_address,buf,0xa00);
    }
  }
  else {
    setup_steps_mark(0x200);
  }
  return 0;
}

void fb_size_from_display_mode(int pixel_mode, int strip_mode, int height)
{
  int m = strip_mode;
  int h = height;
  uint b;
  uint w;
  uint t;
  uint pixel_bits [6];
  
  pixel_bits[0] = 0x10;
  pixel_bits[1] = 0x10;
  pixel_bits[2] = 0x10;
  pixel_bits[3] = 0x10;
  pixel_bits[4] = 0x18;
  pixel_bits[5] = 0x20;
  if (gfx_state.video_out == 1) w = 0x280;
  else if ((gfx_state.display_mode & 0x10) != 0) w = 0x280;
  else w = 0x140;
  if (m == 1) {
    h = h + 0x1fU & 0xffffffe0;
    t = gfx_state.read_ctrl_word & 0xffa0ffff | 0x400000;
    gfx_state.read_ctrl_word = t | ((uint)h >> 2 & 0xf8) << 0xd;
  }
  else if (gfx_state.video_out == 1) h = 0x1e0;
  else {
    if ((gfx_state.display_mode & 1) == 0) {
      if ((gfx_state.display_mode & 0x2c) == 0x20) { h = 0xf0; goto done; }
    }
    if ((gfx_state.display_mode & 0x20) != 0) h = 0x1e0;
    else h = 0xf0;
  }
done:
  render_setup.fb_width = w;
  render_setup.fb_height = h;
  if (pixel_mode != 0xff) {
    render_setup.fb_pixel_bytes = pixel_bits[pixel_mode] >> 3;
  }
  else {
    render_setup.fb_pixel_bytes = pixel_bits[gfx_state.fb_format] >> 3;
  }
  render_setup.fb_bytes =
       render_setup.fb_width * render_setup.fb_height * render_setup.fb_pixel_bytes;
  return;
}
