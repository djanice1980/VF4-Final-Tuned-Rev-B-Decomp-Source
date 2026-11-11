/*
 * sdk/libam/jamma.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar legacy_io_direction; /* 0x0c3e0470 */
extern uint mie_resend_count; /* 0x0c3e2b90 */
extern uchar legacy_io_meter_bits; /* 0x0c3e2b94 */
extern legacy_io_input_bytes legacy_io_inputs; /* 0x0c3e1d80 */
extern mie_request_bytes mie_request; /* 0x0c5cae7c */

void jvs_delay(void)
{
  volatile int i;
  i = 1000000;
  do {
    i = i + -1;
  } while (i != -1);
}

uint jvs_send_reset(void)
{
  byte *p;
  int *r;
  
  p = (byte *)&mie_request;
  *p++ = 0x17;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = 0;
  *p++ = 0xff;
  *p++ = 2;
  *p++ = 0xf0;
  *p++ = 0xd9;
  *p++ = 0;
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,3);
  if (*r == -1) return 0xffffffff;
  return *(byte *)((int)r + 5);
}

int jvs_send(int id, int node, int len, char *cmd)
{
  char *p;
  int i;
  int *r;

  if ((uint)len > 12) return -1;
  p = (char *)&mie_request;
  *p++ = 0x17;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = id;
  *p++ = node;
  *p++ = len;
  for (i = 0; i < len; i++) *p++ = *cmd++;
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,(len + 7) / 4 + 1);
  if (*r == -1) return -1;
  return ((unsigned char *)r)[5];
}

uchar *jvs_send_unchecked(int id, int node, uint len, char *cmd)
{
  uchar *p;
  int i;
  if (len > 12) return (uchar *)0xffffffff;
  p = (uchar *)&mie_request;
  *p++ = 0x17;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = id;
  *p++ = node;
  *p++ = len;
  for (i = 0; i < (int)len; i++) {
    *p++ = *cmd++;
  }
  return maple_send(0x86,0,0x20,(uint *)&mie_request,((int)(len + 7) / 4) + 1);
}

uint jvs_send_two_nodes_bare(int id, int nodes, uint len, char *cmd)
{
  uchar *p;
  int i;
  int *r;
  
  if (len > 0x14) return 0xffffffff;
  p = &mie_request.subcommand;
  *p++ = 0x25;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = (uchar)id;
  *p++ = (uchar)nodes;
  for (i = 0; i < (int)len; i++) *p++ = *cmd++;
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,((int)len + 7) / 4 + 1);
  if (*r == -1) return 0xffffffff;
  return *(byte *)((int)r + 5);
}

int jvs_send_two_nodes(int id, int nodes, int len, char *cmd)
{
  char *p;
  int *r;
  int i;
  
  if ((uint)len > 20) return -1;
  p = (char *)&mie_request;
  *p++ = 0x27;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = id;
  *p++ = nodes;
  for (i = 0; i < len; i++) *p++ = *cmd++;
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,(len + 7) / 4 + 1);
  if (*r == -1) return -1;
  return *(byte *)((int)r + 5);
}

uint jvs_send_two_nodes_alt(int id, int nodes, uint len, char *cmd)
{
  uchar *p;
  int *r;
  int i;

  if (len > 0x14) return 0xffffffff;
  p = &mie_request.subcommand;
  *p++ = 0x27;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = (uchar)id;
  *p++ = (uchar)nodes;
  for (i = 0; i < (int)len; i++) {
    *p++ = *cmd++;
  }
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,((int)len + 7) / 4 + 1);
  if (*r == -1) return 0xffffffff;
  return *(byte *)((int)r + 5);
}

jvs_reply_packet * jvs_get_reply(int id)
{
  int *reply;
  int i;
  char *p;

  p = (char *)&mie_request.subcommand;
  *p++ = 0x15;
  *p++ = (char)id;
  *p++ = 0;
  *p = 0;
  reply = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,1);
  if (*reply == -1) return (jvs_reply_packet *)0x0;
  if (*reply == 0x2000fc) {
    mie_resend_count = mie_resend_count + 1;
    return (jvs_reply_packet *)0x0;
  }
  p = (char *)((int)reply + 5);
  for (i = 0; i < 0x10; i++) {
    legacy_io_inputs.bytes[i] = *p++;
  }
  return (jvs_reply_packet *)((int)reply + 0x15);
}

uint jvs_mie_poll(void)
{
  int *reply;
  uchar *p = (uchar *)&mie_request;
  *p++ = 0x11; *p++ = 0; *p++ = 0; *p = 0;
  reply = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,1);
  if (*reply == -1) return 0xffffffff;
  if (*reply == 0x2000fc) {
    mie_resend_count = mie_resend_count + 1;
    return 0xffffffff;
  }
  return *(byte *)((int)reply + 5);
}

int jvs_set_frame_command(int node, int len, char *cmd)
{
  int *reply;
  int i;
  char *p;
  
  p = (char *)&mie_request;
  for (i = 0; i < 0x20; i++) *p++ = 0;
  p = (char *)&mie_request;
  *p++ = 0x13;
  *p++ = node;
  *p++ = len;
  for (i = 0; i < len; i++) *p++ = *cmd++;
  reply = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,(len + 2) / 4 + 1);
  if (*reply == -1) return -1;
  return *(byte *)((int)reply + 5);
}

uint jvs_write_mie_buffer(uint offset, uint len, char *data)
{
  uchar *p;
  uint i;
  int *r;
  
  if (len > 0x19) return 0xffffffff;
  if (offset > 0xff) return 0xffffffff;
  p = (uchar *)&mie_request;
  *p++ = 0x37;
  *p++ = (uchar)offset;
  *p++ = (uchar)len;
  for (i = 0; i < len; i++) {
    *p++ = *data++;
  }
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,(len + 2 >> 2) + 1);
  if (*r == -1) return 0xffffffff;
  return *(byte *)((int)r + 5);
}

uint jvs_queue_command_after(int id, int node, uint len, char *cmd)
{
  uchar *q;
  int *r;
  int i;
  
  if (len > 0xc) return 0xffffffff;
  q = &mie_request.subcommand;
  *q++ = 0x19;
  *q++ = legacy_io_direction;
  *q++ = legacy_io_meter_bits;
  *q++ = legacy_io_out_byte2;
  *q++ = legacy_io_out_byte3;
  *q++ = id;
  *q++ = node;
  *q++ = len;
  for (i = 0; i < (int)len; i++) {
    *q++ = *cmd++;
  }
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,((int)(len + 7) / 4) + 1);
  if (*r == -1) return 0xffffffff;
  return *(byte *)((int)r + 5);
}

int jvs_queue_command(int id, int node, int len, char *cmd)
{
  char *p;
  int i;
  uchar *r;
  
  if ((uint)len > 12) return -1;
  p = (char *)&mie_request;
  *p++ = 0x21;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = id;
  *p++ = node;
  *p++ = len;
  for (i = 0; i < len; i++) {
    *p++ = *cmd++;
  }
  r = maple_send(0x86,0,0x20,(uint *)&mie_request,(len + 7) / 4 + 1);
  return *(byte *)(r + 5);
}

void led_latch_inverted(uint bits)
{
  led_latch_register = ~bits;
  return;
}

int legacy_io_transfer(void)
{
  char *p;
  int i;
  int *r;
  p = (char *)&mie_request;
  *p++ = 0x31;
  *p++ = legacy_io_direction;
  *p++ = legacy_io_meter_bits;
  *p++ = legacy_io_out_byte2;
  *p++ = legacy_io_out_byte3;
  *p++ = 0;
  *p++ = 0;
  *p = 0;
  r = (int *)maple_send(0x86,0,0x20,(uint *)&mie_request,2);
  if (*r == -1) return 0;
  p = (char *)r + 5;
  for (i = 0; i < 0x10; i++) {
    legacy_io_inputs.bytes[i] = *p++;
  }
  return 1;
}

void legacy_io_start(void)
{
  int i;
  int pad[4]; /* ORIGINAL SHAPE (corroborated 2026-11-10: mie_start in nlam.c has the same 16 bytes below its buffer, there as four volatile counters, which are forced): the game's frame has 16 unused bytes below buf, so some 16-byte local came first; what it held is not in the bytes */
  char buf[128];

  legacy_io_meter_bits = 0;
  legacy_io_out_byte2 = 0;
  legacy_io_out_byte3 = 0;
  legacy_io_direction = 0x77;
  jvs_delay();
  legacy_io_transfer();
  jvs_delay();
  mie_read_ident(buf);
  for (i = 0; i < 0x80; i++) {
    mie_ident_text[i] = 0;
  }
  for (i = 0; i < 0x80; i++) {
    mie_ident_text[i] = buf[i];
    if (buf[i] == '\0') break;
  }
}
