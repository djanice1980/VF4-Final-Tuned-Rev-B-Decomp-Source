/*
 * sdk/nlajamma/jvs_replies.c
 * nlajamma: parsers for each command's part of a reply.
 * Reconstructed C: every function here matches the game byte for byte
 * (python3 -I tools/match/match.py checks the whole tree).
 */
#include "vf4ft.h"

int jvs_reply_reset(jvs_reply_cursor *c)
{
    c->cmd++;
    c->cmd_left--;
    c->cmd++;
    c->cmd_left--;
    return 1;
}

int jvs_reply_none(jvs_reply_cursor *c)
{
  c->cmd = c->cmd + 1;
  c->cmd_left = c->cmd_left + -1;
  return 1;
}

int jvs_reply_set_address(jvs_reply_cursor *c)
{
  byte status;
  
  status = *c->reply;
  c->reply = c->reply + 1;
  c->reply_left = c->reply_left + -1;
  c->cmd = c->cmd + 1;
  c->cmd_left = c->cmd_left + -1;
  return (uint)status;
}

int jvs_reply_dac(jvs_reply_cursor *c)
{
  byte channels;
  int left;
  
  c->reply = c->reply + 1;
  c->reply_left = c->reply_left + -1;
  c->cmd = c->cmd + 1;
  left = c->cmd_left + -1;
  c->cmd_left = left;
  channels = *c->cmd;
  c->cmd = c->cmd + (uint)channels * 2;
  c->cmd_left = left + (uint)channels * -2;
  return 1;
}

int jvs_reply_string(jvs_reply_cursor *c)
{
  byte ch;
  uint status;
  byte *next;
  uchar *p;
  
  status = (uint)*c->reply;
  next = c->reply + 1;
  c->reply = next;
  c->reply_left = c->reply_left + -1;
  c->cmd = c->cmd + 1;
  c->cmd_left = -1 + c->cmd_left;
  if (status != 2) {
    ch = *next;
    while (ch != 0) {
      p = c->reply;
      c->reply = 1 + p;
      c->reply_left = c->reply_left + -1;
      ch = p[1];
    }
  }
  return status;
}

int jvs_reply_main_board_id(jvs_reply_cursor *c)
{
    int r;
    r = *c->reply++;
    c->reply_left--;
    c->cmd++;
    c->cmd_left--;
    while (*c->cmd) {
        c->cmd++;
        c->cmd_left--;
    }
    return r;
}

int jvs_reply_counter(jvs_reply_cursor *c)
{
  byte v;
  int n;
  uchar *p;
  v = *c->reply++;
  c->reply_left--;
  p = c->cmd;
  n = c->cmd_left;
  c->cmd = p + 4;
  c->cmd_left = n - 4;
  return v;
}

int jvs_reply_byte(jvs_reply_cursor *c)
{
  int r;
  uchar *q;
  r = *c->reply;
  q = c->reply + 1;
  c->reply = q;
  c->reply_left--;
  c->cmd++;
  c->cmd_left--;
  if (r != 2) {
    c->reply++;
    c->reply_left--;
  }
  return r;
}

int jvs_reply_screen_pos(jvs_reply_cursor *c)
{
  int r;
  int i;
  uchar *q;
  r = *c->reply;
  q = c->reply + 1;
  c->reply = q;
  c->reply_left--;
  c->cmd++;
  c->cmd_left--;
  c->cmd++;
  c->cmd_left--;
  if (r != 2) {
    for (i = 0; i < 4; i++)
      c->reply++;
    c->reply_left -= 4;
  }
  return r;
}

int jvs_reply_hopper(jvs_reply_cursor *c)
{
  int r;
  int i;
  uchar *q;
  uchar *p;
  int n;
  r = *c->reply;
  q = c->reply + 1;
  c->reply = q;
  c->reply_left--;
  p = c->cmd;
  n = c->cmd_left;
  c->cmd = p + 2;
  c->cmd_left = n - 2;
  if (r != 2) {
    for (i = 0; i < 4; i++)
      c->reply++;
    c->reply_left -= 4;
  }
  return r;
}

int jvs_reply_var_byte(jvs_reply_cursor *c)
{
  int r;
  int n;
  int i;
  r = *c->reply++;
  c->reply_left--;
  c->cmd++;
  c->cmd_left--;
  n = *c->cmd++;
  c->cmd_left--;
  for (i = 0; i < n; i++) {
    c->cmd++;
    c->cmd_left--;
  }
  if (r != 2) {
    c->reply++;
    c->reply_left--;
  }
  return r;
}

int jvs_reply_output(jvs_reply_cursor *c)
{
  int r;
  int n;
  int i;
  r = *c->reply++;
  c->reply_left--;
  c->cmd++;
  c->cmd_left--;
  n = *c->cmd++;
  c->cmd_left--;
  for (i = 0; i < n; i++) {
    c->cmd++;
    c->cmd_left--;
  }
  return r;
}

int jvs_reply_channels(jvs_reply_cursor *c)
{
  int r;
  int n;
  int i;
  uchar *q;
  r = *c->reply;
  q = c->reply + 1;
  c->reply = q;
  c->reply_left--;
  c->cmd++;
  c->cmd_left--;
  n = *c->cmd++;
  c->cmd_left--;
  if (r != 2) {
    for (i = 0; i < n * 2; i++)
      c->reply++;
    c->reply_left -= n * 2;
  }
  return r;
}

int jvs_reply_switches(jvs_reply_cursor *c)
{
  int r;
  int players;
  int bytes;
  int i;
  int j;
  uchar *q;
  r = *c->reply;
  q = c->reply + 1;
  c->reply = q;
  c->reply_left--;
  c->cmd++;
  c->cmd_left--;
  players = *c->cmd++;
  c->cmd_left--;
  bytes = *c->cmd++;
  c->cmd_left--;
  if (r != 2) {
    c->reply++;
    c->reply_left--;
    for (i = 0; i < players; i++)
      for (j = 0; j < bytes; j++)
        c->reply++;
    c->reply_left -= players * bytes;
  }
  return r;
}
