/*
 * gfxlib/dlist_walk.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

void words_copy_aligned8(uint *dst, uint *list)
{
  uint word;
  uint offset;
  uint *src;

  if (((uint)dst & 7) != 0) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  if ((list[1] & 7) != 0) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  src = (uint *)*list;
  offset = 0;
  for (; offset < (uint)list[1]; dst = dst + 1, offset = offset + 4) {
      word = *src;
      src = src + 1;
      *dst = word;
  }
  return;
}

uint * verts_xform_in_place(uint *vert, uint format, int count)
{
  float pos[3];
  float pos_out[3];
  float normal[3];
  float normal_out[3];
  float dir[3];
  float dir_out[3];
  uint packed;
  for (; count != 0; count--) {
    if (format & 2) {
      pos[0] = ((float *)vert)[1];
      pos[1] = ((float *)vert)[2];
      pos[2] = ((float *)vert)[3];
      mtx_xform_point(pos, pos_out);
      ((float *)vert)[1] = pos_out[0];
      ((float *)vert)[2] = pos_out[1];
      ((float *)vert)[3] = pos_out[2];
      packed = vert[0];
      normal[0] = (char)packed / 127.0;
      { float t = (char)(packed >> 8) / 127.0; normal[1] = t; }
      { float t = (char)(packed >> 16) / 127.0; normal[2] = t; }
      mtx_xform_dir(normal, normal_out);
      vert[0] = packed & 0xff000000 | (int)(normal_out[0] * 127.0) & 0xff | ((int)(normal_out[1] * 127.0) & 0xff) << 8 | ((int)(normal_out[2] * 127.0) & 0xff) << 16;
      vert += 4;
    }
    if (format & 4) {
      dir[0] = ((float *)vert)[0];
      dir[1] = ((float *)vert)[1];
      dir[2] = ((float *)vert)[2];
      mtx_xform_dir(dir, dir_out);
      ((float *)vert)[0] = dir_out[0];
      ((float *)vert)[1] = dir_out[1];
      ((float *)vert)[2] = dir_out[2];
      vert += 4;
    }
    if (format & 8) vert += 2;
    if (format & 0x10) vert += 2;
    if (format & 0x20) vert += 2;
    if (format & 0x40) vert += 2;
    if (format & 0x80) vert += 2;
    if (format & 0x100) vert += 4;
    if (format & 0x200) vert += 4;
  }
  return vert;
}

void dlist_xform_in_place(uint *list)
{
  uint *cmd;
  uint *end;
  uint format, count;

  cmd = (uint *)*list;
  end = (uint *)((int)cmd + list[1]);
  while (cmd < end) {
    switch(*cmd >> 8 & 0xf) {
    case 0:
      cmd = cmd + 2;
      break;
    case 1:
    case 2:
      cmd = cmd + 0x10;
      break;
    case 3:
    case 4:
    case 5:
    case 6:
      cmd = cmd + 8;
      break;
    case 7:
      format = cmd[6];
      count = cmd[7];
      cmd += 8;
      cmd = verts_xform_in_place(cmd,format,count);
      break;
    case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    default:
      return;
    }
  }
}

void dlist_hdr_and_mask(uint *list, uint mask)
{
  uint *cmd;
  uint format;
  uint count;
  uint *end;
  int vert_words;

  cmd = (uint *)*list;
  end = (uint *)((int)cmd + list[1]);
  while (cmd < end) {
    switch(*cmd >> 8 & 0xf) {
    case 0:
      cmd = cmd + 2;
      break;
    case 1:
    case 2:
      cmd = cmd + 0x10;
      break;
    case 3:
    case 4:
    case 5:
    case 6:
      cmd = cmd + 8;
      break;
    case 7:
      format = cmd[6];
      count = cmd[7];
      cmd[2] = cmd[2] & mask;
      cmd[4] = cmd[4] & mask;
      cmd[3] = 0;
      cmd[5] = 0;
      cmd = cmd + 8;
      vert_words = 0;
      if ((format & 2) != 0) {
        vert_words = 4;
      }
      if ((format & 4) != 0) {
        vert_words = vert_words + 4;
      }
      if ((format & 8) != 0) {
        vert_words = vert_words + 2;
      }
      if ((format & 0x10) != 0) {
        vert_words = vert_words + 2;
      }
      if ((format & 0x20) != 0) {
        vert_words = vert_words + 1;
      }
      if ((format & 0x40) != 0) {
        vert_words = vert_words + 2;
      }
      if ((format & 0x80) != 0) {
        vert_words = vert_words + 2;
      }
      if ((format & 0x100) != 0) {
        vert_words = vert_words + 4;
      }
      if ((format & 0x200) != 0) {
        vert_words = vert_words + 4;
      }
      cmd = cmd + vert_words * count;
      break;
    case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    default:
      return;
    }
  }
  return;
}
