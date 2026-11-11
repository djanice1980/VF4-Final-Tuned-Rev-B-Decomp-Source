/*
 * sdk/nlajamma/jvs.c
 * nlajamma: the JVS transport (its own build string: "nlajamma Ver 1.01").
 * Reconstructed C: every function here matches the game byte for byte
 * (python3 -I tools/match/match.py checks the whole tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined4 mie_resend_count; /* 0x0c3e2b90 */

int jvs_index_replies(int node, int len, char *reply, char *cmd)
{
    jvs_reply_cursor c;
    int command;
    char *offset;

    if (*cmd == 0)
        return 0;
    c.reply_left = len;
    c.reply = (unsigned char *)reply;
    c.cmd = (unsigned char *)cmd;
    while (c.reply_left > 0) {
        command = *c.cmd;
        offset = (char *)((char *)c.reply - reply);
        jvs_reply_offset[node][command] = (char *)-1;
        if (jvs_reply_handlers[command](&c) != 1) {
            jvs_reply_error_count++;
            return 0;
        }
        jvs_reply_offset[node][command] = offset;
    }
    return 1;
}

void jvs_set_main_node(int node)
{
    jvs_main_node = node;
}

void jvs_wait(void)
{
    volatile int count;

    count = 1000000;
    while (count--)
        ;
}

int jvs_read_node(int node, int *service, int *coins)
{
  jvs_reply_packet *rp;
  jvs_node_desc *inf;
  
  rp = jvs_get_reply(0);
  inf = jvs_nodes + node;
  if (jvs_check_reply(node,inf,rp) == 0) {
    jvs_save_command_before(node,jvs_extra_command_len[node],jvs_extra_command[node]);
    jvs_queue_command(0,node,jvs_extra_command_len[node],jvs_extra_command[node]);
    jvs_extra_command_len[node] = 0;
  }
  else {
    jvs_update_coins(node,inf,coins);
    jvs_update_service_buttons(node,inf,service);
    jvs_save_command_before(node,jvs_extra_command_len[node],jvs_extra_command[node]);
    jvs_queue_command(0,node,jvs_extra_command_len[node],jvs_extra_command[node]);
    jvs_extra_command_len[node] = 0;
  }
}

int jvs_check_reply_header(int node, jvs_node_desc *inf, jvs_reply_packet *rp)
{
  if (rp == (jvs_reply_packet *)0x0) {
    inf->reply_error = 1;
    inf->report_error = 1;
    jvs_reply_fail_count = jvs_reply_fail_count + 1;
    return 0;
  }
  else {
    if ((rp->ack_status != '\0') || (rp->status != '\x01') || (rp->sync != -0x20) ||
        (rp->packet_len == '\0')) {
      inf->reply_error = 1;
      inf->report_error = 1;
      jvs_reply_fail_count = jvs_reply_fail_count + 1;
      if ((byte)rp->ack_status != 0) {
        jvs_ack_status_errors[node][(byte)rp->ack_status] =
             jvs_ack_status_errors[node][(byte)rp->ack_status] + 1;
      }
      if ((byte)rp->status != 1) {
        jvs_status_errors[node][(byte)rp->status] = jvs_status_errors[node][(byte)rp->status] + 1;
      }
      return 0;
    }
    return (uint)(node == (uint)(byte)rp->from_node);
  }
}

void jvs_save_command_before(int node, int len, char *cmd)
{
  char *d;
  int i;
  d = jvs_last_command[node];
  for (i = 0; i < len; i++) *d++ = *cmd++;
  cmd = jvs_frame_command[node];
  for (i = 0; i < jvs_frame_command_len[node]; i++) *d++ = *cmd++;
}

int jvs_find_nodes(void)
{
  jvs_reply_packet *r;
  int i;
  char cmd[2];
  
  jvs_wait();
  jvs_send_reset();
  jvs_wait();
  jvs_send_reset();
  for (i = 0; i < 10; i++) {
    jvs_wait();
    jvs_wait();
    jvs_wait();
    jvs_wait();
    jvs_wait();
    jvs_wait();
    jvs_wait();
    jvs_wait();
  }
  for (i = 1; i < 32; i++) {
    cmd[0] = 0xf1;
    cmd[1] = i;
    jvs_wait();
    jvs_send(0,0xff,2,cmd);
    jvs_wait();
    r = jvs_get_reply(0);
    if (r == 0) return -2;
    if (r->ack_status != 0 || r->status != 1 || r->packet_len == 0) return -1;
    if (r->sense == -114) break;
  }
  if (i == 32) return 0;
  return i;
}

int jvs_build_two_node_command(int node1, int node2)
{
  int i;
  char *p;
  
  p = jvs_two_node_command;
  jvs_save_command_before(node1,jvs_extra_command_len[node1],jvs_extra_command[node1]);
  *p++ = node1;
  *p++ = jvs_extra_command_len[node1];
  for (i = 0; i < jvs_extra_command_len[node1]; i++) {
    *p++ = jvs_extra_command[node1][i];
  }
  jvs_save_command_before(node2,jvs_extra_command_len[node2],jvs_extra_command[node2]);
  *p++ = node2;
  *p++ = jvs_extra_command_len[node2];
  for (i = 0; i < jvs_extra_command_len[node2]; i++) {
    *p++ = jvs_extra_command[node2][i];
  }
  return p - jvs_two_node_command;
}

int jvs_update_service_buttons(int node, jvs_node_desc *inf, int *service)
{
  char *off;
  char *p;
  int stride;
  int n;
  int i;

  off = jvs_reply_offset[node][0x20];
  p = jvs_reply_data[node] + (int)off;
  if (off != (char *)-1 && *p == 1) {
    *service = 0;
    stride = inf->switches[2];
    n = inf->switches[0];
    if (n > 4) n = 4;
    p += 2;
    for (i = 0; i < n; i++) {
      if (*p & 0x40) if (i < 4) {
        *service |= 16 << i;
      }
      p += stride;
    }
  }
}

void jvs_bus_setup(void)
{
  char pad[256];
  int coins[8];
  int service[2];
  int i;
  int j;
  int n;
  int k;
  char *p;

  credit_backup_pending = 0;
  jvs_reply_fail_count = 0;
  jvs_reply_error_count = 0;
  mie_resend_count = 0;
  for (i = 0; i < 31; i++) {
    for (j = 0; j < 8; j++) {
      jvs_ack_status_errors[i][j] = 0;
      jvs_status_errors[i][j] = 0;
    }
  }
  for (i = 0; i < 31; i++) {
    for (j = 0; j < 4; j++) {
      jvs_coin_counter_last[i][j] = 0;
      jvs_coin_subtract_sent[i][j] = -1;
    }
  }
  jvs_main_node = 0;
  for (i = 0; i < 31; i++) {
    k = sizeof(jvs_node_desc);
    p = (char *)&jvs_nodes[i];
    while (k--) *p++ = 0;
    jvs_nodes[i].node = i;
    jvs_frame_command_len[i] = 0;
    jvs_extra_command_len[i] = 0;
    for (j = 0; j < 256; j++) jvs_reply_offset[i][j] = (char *)-1;
  }
  for (i = 0; i < 31; i++) {
    p = jvs_reply_data[i];
    k = 512;
    while (k--) *p++ = 0;
  }
  for (i = 0; i < 3; i++) {
    n = jvs_find_nodes();
    if (n > 0) break;
  }
  if (n <= 0) jvs_node_count = 0;
  else jvs_node_count = n;
  for (i = 1; i <= n; i++) {
    for (j = 0; j < 10; j++) {
      if (jvs_query_node(i) == 1) break;
    }
  }
  for (i = 1; i <= n; i++) {
    if (jvs_frame_command_len[i] != 0) {
      jvs_wait();
      jvs_set_frame_command(i,jvs_frame_command_len[i],jvs_frame_command[i]);
      jvs_wait();
      jvs_save_command_before(i,0,0);
      jvs_queue_command(0,i,0,0);
    }
  }
  jvs_read_all_coin_counters();
  if (n <= 0) {
    legacy_io_setup();
    jvs_wait();
    legacy_io_poll(0,&service[0],coins);
  }
  else if (n == 1) {
    jvs_wait();
    jvs_read_node(1,&service[0],coins);
    jvs_wait();
    jvs_read_node(1,&service[0],coins);
    jvs_wait();
  }
  else if (n == 2) {
    jvs_wait();
    jvs_read_two_nodes(2,&service[1],&coins[2],1,&service[0],coins);
    jvs_wait();
    jvs_read_two_nodes(2,&service[1],&coins[2],1,&service[0],coins);
    jvs_wait();
  }
}

int jvs_check_reply(int node, jvs_node_desc *inf, jvs_reply_packet *rp)
{
  int i;
  char *d;
  char *s;

  if (rp == (jvs_reply_packet *)0x0) {
    inf->reply_error = 1;
    inf->report_error = 1;
    jvs_reply_fail_count = jvs_reply_fail_count + 1;
    return 0;
  }
  if (rp->ack_status != 0 || rp->status != 1 || rp->sync != -0x20 || rp->packet_len == 0) {
    inf->reply_error = 1;
    inf->report_error = 1;
    jvs_reply_fail_count = jvs_reply_fail_count + 1;
    if ((uchar)rp->ack_status != 0) {
      jvs_ack_status_errors[node][(uchar)rp->ack_status]++;
    }
    if ((uchar)rp->status != 1) {
      jvs_status_errors[node][(uchar)rp->status]++;
    }
    return 0;
  }
  if (node != (uchar)rp->from_node) return 0;
  s = &rp->report;
  d = jvs_reply_data[node];
  for (i = 0; i < (uchar)rp->packet_len; i++) *d++ = *s++;
  i = jvs_index_replies(node,(uchar)rp->length - 2,jvs_reply_data[node],jvs_last_command[node]);
  if (i != 1) {
    inf->report_error = 1;
    return 0;
  }
  return 1;
}

int jvs_update_coins(int node, jvs_node_desc *inf, int *coins)
{
  char *off;
  uchar *p;
  int n;
  int i;
  int v;
  int r;
  int l;

  off = jvs_reply_offset[node][0x21];
  p = (uchar *)jvs_reply_data[node] + (int)off;
  if (off == (char *)-1) {
    r = 0;
  }
  else if (*p != 1) {
    r = 0;
  }
  else {
    n = inf->coin_slots;
    p++;
    if (n > 4) n = 4;
    for (i = 0; i < n; i++) {
      v = p[0] << 8 | p[1];
      p += 2;
      if (v & 0xc000) {
        inf->coin_error |= v & 0xc000;
      }
      else if (v == 0) {
        jvs_coin_subtract_sent[node][i] = v;
      }
      else if (v > 0x1000) {
        if (jvs_coin_subtract_sent[node][i] == 0) {
          coins[i] = v - jvs_coin_counter_last[node][i];
          jvs_coin_counter_last[node][i] = 0;
        }
        l = jvs_extra_command_len[node];
        jvs_extra_command[node][l] = '0';
        jvs_extra_command[node][l + 1] = i + 1;
        jvs_extra_command[node][l + 2] = v >> 8;
        jvs_extra_command[node][l + 3] = v;
        jvs_extra_command_len[node] = l + 4;
        jvs_coin_subtract_sent[node][i] = 1;
      }
      else {
        if (jvs_coin_counter_last[node][i] != v) {
          coins[i] = v - jvs_coin_counter_last[node][i];
          jvs_coin_counter_last[node][i] = v;
        }
      }
    }
    r = 1;
  }
  return r;
}

int jvs_read_all_coin_counters(void)
{
  jvs_reply_packet *rp;
  jvs_node_desc *inf;
  uchar *p;
  int node;
  int retry;
  int i;
  int v;
  char cmd[16];

  for (node = 1; node <= jvs_node_count; node++) {
    inf = &jvs_nodes[node];
    if (inf->coin_slots != 0) {
      retry = 100;
      do {
        cmd[0] = 0x21;
        cmd[1] = inf->coin_slots;
        jvs_wait();
        jvs_send(0,node,2,cmd);
        jvs_wait();
        rp = jvs_get_reply(0);
        if (jvs_check_reply_header(node,inf,rp) != 0) {
          p = (uchar *)&rp->report;
          if (*(char *)p++ == 1) {
            for (i = 0; i < inf->coin_slots && i < 4; i++) {
              v = (p[0] << 8 | p[1]) & ~0xc000;
              jvs_coin_counter_last[node][i] = v;
              p += 2;
            }
            retry = 0;
          }
        }
      } while (retry > 0);
    }
  }
}

int jvs_read_two_nodes(int node1, int *service1, int *coins1, int node2, int *service2, int *coins2)
{
  jvs_reply_packet *rp;
  jvs_node_desc *inf;
  int r;

  rp = jvs_get_reply(0);
  inf = jvs_nodes + node1;
  r = jvs_check_reply(node1,inf,rp);
  if (r == 0) {
    jvs_send_two_nodes(0,2,jvs_build_two_node_command(node1,node2),jvs_two_node_command);
    jvs_extra_command_len[node1] = 0;
    jvs_extra_command_len[node2] = 0;
  }
  else {
    jvs_update_coins(node1,inf,coins1);
    jvs_update_service_buttons(node1,inf,service1);
    rp = (jvs_reply_packet *)(&rp->ack_status + (uchar)rp->packet_len);
    inf = jvs_nodes + node2;
    r = jvs_check_reply(node2,inf,rp);
    if (r == 0) {
      jvs_send_two_nodes(0,2,jvs_build_two_node_command(node1,node2),jvs_two_node_command);
      jvs_extra_command_len[node1] = 0;
      jvs_extra_command_len[node2] = 0;
    }
    else {
      jvs_update_coins(node2,inf,coins2);
      jvs_update_service_buttons(node2,inf,service2);
      jvs_send_two_nodes(0,2,jvs_build_two_node_command(node1,node2),jvs_two_node_command);
      jvs_extra_command_len[node1] = 0;
      jvs_extra_command_len[node2] = 0;
    }
  }
}

void jvs_save_command_after(int node, int len, char *cmd)
{
  char *d;
  char *s;
  int i;
  d = jvs_last_command[node];
  s = jvs_frame_command[node];
  for (i = 0; i < jvs_frame_command_len[node]; i++) *d++ = *s++;
  s = cmd;
  for (i = 0; i < len; i++) *d++ = *s++;
}

int jvs_query_node(int node)
{
  jvs_reply_packet *rp;
  jvs_node_desc *inf;
  char *p;
  char *q;
  int i, a, b, c;
  int cmd;
  char buf[16];

  inf = &jvs_nodes[node];
  buf[0] = 0x10;
  jvs_wait();
  jvs_send(0, node, 1, buf);
  jvs_wait();
  rp = jvs_get_reply(0);
  if (jvs_check_reply_header(node, inf, rp) == 0) return 0;
  p = &rp->data;
  for (i = 0; i < (uchar)rp->length - 3; i++)
    jvs_nodes[node].id[i] = *p++;

  buf[0] = 0x11;
  jvs_wait();
  jvs_send(0, node, 1, buf);
  jvs_wait();
  rp = jvs_get_reply(0);
  if (jvs_check_reply_header(node, inf, rp) == 0) return 0;
  p = &rp->data;
  jvs_nodes[node].cmd_format_version = *p;

  buf[0] = 0x12;
  jvs_wait();
  jvs_send(0, node, 1, buf);
  jvs_wait();
  rp = jvs_get_reply(0);
  if (jvs_check_reply_header(node, inf, rp) == 0) return 0;
  p = &rp->data;
  jvs_nodes[node].jvs_version = *p;

  buf[0] = 0x13;
  jvs_wait();
  jvs_send(0, node, 1, buf);
  jvs_wait();
  rp = jvs_get_reply(0);
  if (jvs_check_reply_header(node, inf, rp) == 0) return 0;
  p = &rp->data;
  jvs_nodes[node].comm_version = *p;

  buf[0] = 0x14;
  jvs_wait();
  jvs_send(0, node, 1, buf);
  jvs_wait();
  rp = jvs_get_reply(0);
  if (jvs_check_reply_header(node, inf, rp) == 0) return 0;

  q = jvs_frame_command[node];
  for (i = 0; i < 32; i++) *q++ = 0;
  q = jvs_frame_command[node];
  p = &rp->data;
  while (*(uchar *)p != 0) {
    cmd = *(uchar *)p;
    p++;
    a = (uchar)*p++;
    b = (uchar)*p++;
    c = (uchar)*p++;
    switch (cmd) {
    case 0:
    case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    case 0x13:
      break;
    case 1:
      jvs_nodes[node].switches[0] = a;
      jvs_nodes[node].switches[1] = b;
      if (b != 0) { b -= 1; b = b / 8 + 1; }
      jvs_nodes[node].switches[2] = b;
      if (a != 0) {
        *q++ = 0x20;
        *q++ = a;
        *q++ = b;
      } else {
        *q++ = 0x20;
        *q++ = 0;
        *q++ = 0;
      }
      break;
    case 2:
      jvs_nodes[node].coin_slots = a;
      if (a != 0) { *q++ = 0x21; *q++ = a; }
      break;
    case 3:
      jvs_nodes[node].analog_channels = a;
      if (a != 0) { *q++ = 0x22; *q++ = a; }
      break;
    case 4:
      jvs_nodes[node].rotary_channels = a;
      if (a != 0) { *q++ = 0x23; *q++ = a; }
      break;
    case 5:
      jvs_nodes[node].keycode = 1;
      *q++ = 0x24;
      break;
    case 6:
      jvs_nodes[node].screen[0] = a;
      jvs_nodes[node].screen[1] = b;
      jvs_nodes[node].screen[2] = c;
      if (a != 0 && b != 0 && c != 0) { *q++ = 0x25; *q++ = 1; }
      break;
    case 0x10:
      jvs_nodes[node].card = a;
      break;
    case 0x11:
      jvs_nodes[node].hopper = a;
      if (a != 0) { *q++ = 0x2e; *q++ = a; }
      break;
    case 0x12:
      jvs_nodes[node].driver = a;
      break;
    case 0x14:
      jvs_nodes[node].character[0] = a;
      jvs_nodes[node].character[1] = b;
      break;
    case 0x15:
      jvs_nodes[node].backup = 1;
      break;
    }
  }
  jvs_frame_command_len[node] = q - jvs_frame_command[node];
  return 1;
}
