/*
 * sdk/nlajamma/legacy_io.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar legacy_io_direction; /* 0x0c3e0470 */
extern char mie_ident_text[128]; /* 0x0c3e1cbc */
extern uchar legacy_io_meter_bits; /* 0x0c3e2b94 */
extern legacy_io_input_bytes legacy_io_inputs; /* 0x0c3e1d80 */

int legacy_io_setup(void)
{
  int i;
  char *s;
  jvs_node_desc *n;
  legacy_io_meter_bits = 0;
  legacy_io_out_byte2 = 0;
  legacy_io_out_byte3 = 0;
  legacy_io_direction = 0x77;
  i = 0;
  n = jvs_nodes;
  s = mie_ident_text;
  for (; i < 0x80; i++) {
    n->id[i] = s[i];
    if (s[i] == '\0') break;
  }
  jvs_nodes[0].switches[0] = '\x02';
  jvs_nodes[0].switches[1] = '\v';
  jvs_nodes[0].switches[2] = '\x02';
  jvs_nodes[0].coin_slots = '\x02';
  jvs_nodes[0].analog_channels = '\b';
  jvs_nodes[0].cmd_format_version = '\0';
  return i;
}

int legacy_io_poll(int node, int *credit_io, int *coins)
{
  uchar p1_bits;
  uchar p2_bits;
  uchar sys_bits;
  uchar *in;
  char *out;

  legacy_io_transfer();
  in = (uchar *)&legacy_io_inputs;
  p1_bits = in[0];
  p2_bits = in[1];
  sys_bits = in[2];
  p1_bits = ~p1_bits;
  p2_bits = ~p2_bits;
  sys_bits = ~sys_bits;
  jvs_reply_offset[0][0x20] = (char *)0x0;
  jvs_reply_offset[0][0x21] = (char *)0x6;
  jvs_reply_offset[0][0x22] = (char *)0xb;
  out = jvs_reply_data[0];
  out[0] = 1;
  out[1] = ((sys_bits & 0x4) >> 2) << 7;
  out[2] = ((sys_bits & 0x10) >> 4) << 7 | ((sys_bits & 0x8) >> 3) << 6 | ((p1_bits & 0x20) >> 5) << 5 | ((p1_bits & 0x10) >> 4) << 4 | ((p1_bits & 0x80) >> 7) << 3 | ((p1_bits & 0x40) >> 6) << 2 | ((p1_bits & 0x1) >> 0) << 1 | ((p1_bits & 0x2) >> 1);
  out[3] = ((p1_bits & 0x4) >> 2) << 7 | ((p1_bits & 0x8) >> 3) << 6 | ((sys_bits & 0x40) >> 6) << 5;
  out[4] = ((sys_bits & 0x20) >> 5) << 7 | ((sys_bits & 0x8) >> 3) << 6 | ((p2_bits & 0x20) >> 5) << 5 | ((p2_bits & 0x10) >> 4) << 4 | ((p2_bits & 0x80) >> 7) << 3 | ((p2_bits & 0x40) >> 6) << 2 | ((p2_bits & 0x1) >> 0) << 1 | ((p2_bits & 0x2) >> 1);
  out[5] = ((p2_bits & 0x4) >> 2) << 7 | ((p2_bits & 0x8) >> 3) << 6 | ((sys_bits & 0x80) >> 7) << 5;
  out[6] = 1;
  out[7] = 0;
  out[8] = ((sys_bits & 0x1) >> 0);
  out[9] = 0;
  out[10] = (sys_bits & 0x2) >> 1;
  out[0xb] = 1;
  out[0xc] = in[8];
  out[0xd] = 0;
  out[0xe] = in[9];
  out[0xf] = 0;
  out[0x10] = in[10];
  out[0x11] = 0;
  out[0x12] = in[11];
  out[0x13] = 0;
  out[0x14] = in[12];
  out[0x15] = 0;
  out[0x16] = in[13];
  out[0x17] = 0;
  out[0x18] = in[14];
  out[0x19] = 0;
  out[0x1a] = in[15];
  out[0x1b] = 0;
  *credit_io = (sys_bits & 0x3) | ((sys_bits & 0x8) >> 3) << 4;
}
