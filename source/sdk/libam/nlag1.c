/*
 * sdk/libam/nlag1.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int card_retry_limit; /* 0x0c329c88 */
extern int card_queue_len; /* 0x0c329c8c */
extern undefined card_keys; /* 0x0c3e0488 */
extern undefined1 card_status; /* 0x0c3e1ba4 */
extern undefined card_read_buffer; /* 0x0c3e1bbc */
extern undefined card_steps; /* 0x0c3e1d4c */

void card_units_poll(void)
{
  card_poll_count = card_poll_count + 1;
  if ((&card_units.unit0_flags)[(card_poll_count & 1) * 0x13] == 0) {
    card_unit_step(card_poll_count & 1);
  }
  return;
}

void card_queue_clear(int side)
{
  typedef struct { int a; int b; int c; int d; } e16;
  typedef struct { int a; int b; int c; int d; int e; } e20;
  e20 z;
  e16 *q;
  int i;
  char *b;
  z.a = 0;
  z.b = 0;
  z.c = 0;
  z.d = 0;
  z.e = 0;
  q = (e16 *)&card_steps + side;
  q->a = 0;
  q->b = 0;
  q->c = 0;
  q->d = 0;
  for (i = 0, b = (char *)&card_queues; i < 0x14; i++) {
    *(e20 *)(b + (i * 20 + side * 400)) = z;
  }
}

void card_state_reset(void)
{
  typedef struct { char a; char b; char c; char d; int e; } ent;
  int i;
  
  for (i = 0; i < 2; i++) {
    ((ent *)&card_status)[i].a = 0;
    ((ent *)&card_status)[i].e = 0;
    ((ent *)&card_status)[i].b = 0;
    ((ent *)&card_status)[i].c = 0;
    ((ent *)&card_status)[i].d = 0;
    card_queue_clear(i);
  }
  card_reset_hook();
  return;
}

int card_request_payload(int side, uchar *request)
{
  int i;
  int n;
  uchar *q;
  uint *d;

  n = 0;
  switch (*(int *)(request + 4)) {
  case 0xb1:
    n = *(byte *)(request + 0x11);
    for (i = 0; i < n; i++) {
      d = card_payload + i;
      uint *s = (uint *)card_write_buffer;
      d[1] = s[side * 32 + (*(byte *)(request + 0x10) + i)];
    }
    break;
  case 0xd4:
    q = request + 0x10;
    *q++ = ((uchar (*)[4])&card_keys)[side][3];
    *q++ = ((uchar (*)[4])&card_keys)[side][2];
    *q++ = ((uchar (*)[4])&card_keys)[side][1];
    *q = ((uchar (*)[4])&card_keys)[side][0];
    break;
  }
  card_payload[0] = request[0x10] | request[0x11] << 8 | request[0x12] << 16 | request[0x13] << 24;
  return n + 1;
}

int card_reply_bad(int side, byte *reply)
{
  int r = 0;
  uint a = reply[0];
  uint b = reply[1];
  uint c = reply[2];
  uint *e = (uint *)card_unit_maple_ids[side];
  if (e[0] == c && e[1] == b) {
    if (a - 0xfc > 1)
      goto done;
    card_bad_replies = card_bad_replies + 1;
  }
  r = 1;
done:
  return r;
}

bool card_reply_status(int n, uchar *p)
{
  typedef struct { byte a, b, c, d; int cnt; } ent;
  byte flags = *(byte *)(p + 4);

  if (flags & 1) {
    ((ent *)&card_status)[n].a = 0;
    ((ent *)&card_status)[n].cnt = 0;
  } else {
    if (((ent *)&card_status)[n].cnt < card_retry_limit) ((ent *)&card_status)[n].cnt = ((ent *)&card_status)[n].cnt + 1;
    else ((ent *)&card_status)[n].a = 1;
  }
  if (flags & 0x40) ((ent *)&card_status)[n].b = 1;
  else ((ent *)&card_status)[n].b = 0;
  if (((ent *)&card_status)[n].d == 1) {
    if (((ent *)&card_status)[n].a == 0) {
      ((ent *)&card_status)[n].d = 9;
      goto done;
    }
  }
  if (flags & 2) {
    ((ent *)&card_status)[n].d = 4;
  } else if (flags & 4) {
    ((ent *)&card_status)[n].d = 5;
  } else if (flags & 8) {
    ((ent *)&card_status)[n].d = 6;
  } else if (flags & 0x10) {
    ((ent *)&card_status)[n].d = 7;
  } else if (flags & 0x20) {
    ((ent *)&card_status)[n].d = 8;
  }
done:
  if (((ent *)&card_status)[n].d > 1) {
    ((ent *)&card_status)[n].c = 0;
    return 1;
  }
  return 0;
}

void card_reset_hook(void)
{
  return;
}

void card_reply_hook(int side, int *reply)
{
  return;
}

void card_reply_store(int side, int kind, int *reply)
{
  int i;
  int key;
  int *buf;
  uint *d;
  
  switch (kind) {
  case 1:
    i = 0;
    buf = (int *)&card_read_buffer;
    for (; i < 0x20; i++) {
      buf[side * 0x20 + i] = (reply + 2)[i];
    }
    break;
  case 2:
    key = (int)((char *)reply)[0xc] << 0x18 |
                         (uint)((byte *)reply)[0xd] << 0x10 |
                         (uint)((byte *)reply)[0xe] << 8 | (uint)((byte *)reply)[0xf];
    d = (uint *)&card_keys + side;
    *d = card_key_hash(key);
    break;
  case 0:
    break;
  }
}

void card_unit_step(int side)
{
  typedef struct { int a; int b; } jvs_pair;
  typedef struct { int state; int cnt; int retry; int idx; } jvs_step_state;
  typedef struct { int f0; int f4; int f8; int fc; int f10; } jvs_step;
  typedef struct { char b0; char b1; char b2; uchar b3; char r[4]; } jvs_step_status;
  int port;
  int address;
  jvs_step_state *st;
  jvs_step *e;
  int *r;
  int x;
  int zero = 0;

  port = card_unit_maple_ids[side][0];
  address = card_unit_maple_ids[side][1];
  st = &((jvs_step_state *)&card_steps)[side];
  e = (jvs_step *)((uchar *)card_queues + side * 400 + st->idx * 20);
  switch (st->state) {
  case 0:
    st->retry = 0;
    st->state = 1;
  case 1:
    if (e->f4 != 0) {
      x = card_request_payload(side,(uchar *)e);
      r = (int *)maple_send(e->f4,port,address,card_payload,x);
      card_reply_hook(side,r);
      st->state = 2;
    }
    else {
      r = (int *)maple_send_no_data(0x90,port,address);
    }
    if (*r == -1) {
      if (((jvs_step_status *)&card_status)[side].b3 < 2) ((jvs_step_status *)&card_status)[side].b3 = 3;
      ((jvs_step_status *)&card_status)[side].b2 = 0;
    }
    else if (card_reply_bad(side,(byte *)r) != 0) {
      st->state = 1;
      if (st->retry++ <= 4) return;
      ((jvs_step_status *)&card_status)[side].b3 = 3;
      ((jvs_step_status *)&card_status)[side].b2 = 0;
    }
    else if (card_reply_status(side,(uchar *)r) == 0) return;
    break;
  case 2:
    st->cnt = e->f0;
    st->retry = 0;
    st->state = 3;
  case 3:
    r = (int *)maple_send_no_data(e->f8,port,address);
    card_reply_hook(side,r);
    if (*r == -1) {
      if (st->cnt-- >= 0) return;
      ((jvs_step_status *)&card_status)[side].b3 = 2;
      ((jvs_step_status *)&card_status)[side].b2 = 0;
    }
    else if (card_reply_bad(side,(byte *)r) != 0) {
      if (st->retry++ <= 4) return;
      ((jvs_step_status *)&card_status)[side].b3 = 3;
      ((jvs_step_status *)&card_status)[side].b2 = 0;
    }
    else if (card_reply_status(side,(uchar *)r) == 0) {
      card_reply_store(side,e->fc,r);
      st->idx++;
      st->state = zero;
      e = (jvs_step *)((uchar *)card_queues + side * 400 + st->idx * 20);
      if (e->f4 != 0) return;
      ((jvs_step_status *)&card_status)[side].b3 = 0;
      ((jvs_step_status *)&card_status)[side].b2 = 0;
    }
    break;
  default:
    return;
  }
  card_queue_clear(side);
}

void card_queue_begin(int side)
{
  char *p;
  card_queue_len = 0;
  p = &card_status + side * 8;
  p[2] = 2;
  p[3] = 1;
  card_queue_clear(side);
}

void card_queue_write(int side, int block, int words, uchar *data)
{
  typedef struct { int a; int b; int c; int d; char e; char f; } ent;
  uchar *d;
  int i;
  int n;
  ent *q;
  if (card_queue_len < 0x14) {
    d = (uchar *)&card_write_buffer[side][block];
    for (i = 0; i < words * 4; i++) {
      *d++ = *data++;
    }
    n = card_queue_len;
    q = &((ent (*)[20])card_queues)[side][n];
    q->b = 0xb1;
    q->c = 0x91;
    q->d = 0;
    q->a = 400;
    q->e = block;
    q->f = words;
    card_queue_len = n + 1;
  }
}

void card_queue_c1(int side)
{
  struct { int a; int b; int c; int d; int e; } *e;
  int n = card_queue_len;
  
  if (n < 0x14) {
    e = (void *)(side * 400 + n * 0x14 + (int)card_queues);
    e->b = 0xc1;
    e->c = 0x91;
    e->d = 0;
    e->a = 400;
    card_queue_len = n + 1;
  }
}

void card_queue_d0(int side)
{
  typedef struct { int a; int b; int c; int d; int e; } ent;
  ent *p;
  int n;
  
  n = card_queue_len;
  if (n < 0x14) {
    p = &((ent (*)[20])card_queues)[side][n];
    p->b = 0xd0;
    p->c = 0x91;
    p->d = 0;
    p->a = 400;
    card_queue_len = n + 1;
  }
}

void card_queue_a0(int side)
{
  typedef struct { int a; int b; int c; int d; int e; } ent;
  ent *p;
  int n;
  
  n = card_queue_len;
  if (n < 0x14) {
    ((int *)&card_keys)[side] = 0;
    p = &((ent (*)[20])card_queues)[side][n];
    p->b = 0xa0;
    p->c = 0x91;
    p->d = 0;
    p->a = 400;
    card_queue_len = n + 1;
  }
}

void card_queue_read_key(int side)
{
  int offset;
  
  if (card_queue_len < 0x14) {
    offset = side * 400 + card_queue_len * 0x14;
    ((struct { int a; int b; int c; int d; int e; } *)((char *)card_queues + offset))->b = 0xa1;
    ((struct { int a; int b; int c; int d; int e; } *)((char *)card_queues + offset))->c = 0xa1;
    ((struct { int a; int b; int c; int d; int e; } *)((char *)card_queues + offset))->d = 2;
    ((struct { int a; int b; int c; int d; int e; } *)((char *)card_queues + offset))->a = 400;
    card_queue_len = card_queue_len + 1;
  }
}

void card_queue_send_key(int side)
{
  typedef struct { int a; int b; int c; int d; int e; } ent;
  ent *t;
  ent *u;
  int n;
  int m;
  int row;
  char *b;

  n = card_queue_len;
  if (n < 0x14) {
    b = (char *)card_queues;
    row = side * 400;
    t = (ent *)(b + (row + n * 20));
    t->b = 0xd4;
    t->c = 0x91;
    t->d = 0;
    t->a = 400;
    m = n + 1;
    u = (ent *)(b + (row + m * 20));
    u->b = 0xa0;
    u->c = 0x91;
    u->d = 0;
    u->a = 400;
    card_queue_len = m + 1;
  }
}

void card_queue_read(int side)
{
  int offset;
  uchar *p;

  if (card_queue_len < 0x14) {
    offset = side * 400 + card_queue_len * 0x14;
    p = (uchar *)card_queues + offset;
    *(uint *)(p + 4) = 0xa1;
    *(uint *)(p + 8) = 0xa1;
    *(uint *)(p + 12) = 1;
    *(uint *)p = 400;
    card_queue_len = card_queue_len + 1;
  }
}

void card_queue_d9(int side)
{
  uint *p;
  
  if (card_queue_len < 0x14) {
    p = (uint *)((uchar *)card_queues + (side * 400 + card_queue_len * 0x14));
    p[1] = 0xd9;
    p[2] = 0x91;
    p[3] = 0;
    p[0] = 400;
    card_queue_len = card_queue_len + 1;
  }
  return;
}

void card_queue_da(int side)
{
  int *p;
  int n;
  
  n = card_queue_len;
  if (n < 0x14) {
    p = (int *)((uchar *)card_queues + side * 400 + n * 0x14);
    p[1] = 0xda;
    p[2] = 0x91;
    p[3] = 0;
    p[0] = 400;
    card_queue_len = n + 1;
  }
}

void card_queue_d1(int side, int value)
{
  int *p;
  int n;

  n = card_queue_len;
  if (n < 0x14) {
    p = (int *)((uchar *)card_queues + side * 400 + n * 0x14);
    p[1] = 0xd1;
    p[2] = 0x91;
    p[3] = 0;
    p[0] = 400;
    *(uchar *)(p + 4) = value;
    card_queue_len = n + 1;
  }
}

void md5_init(card_md5_rec *ctx)
{
  ctx->bit_count[1] = 0;
  ctx->bit_count[0] = 0;
  ctx->state[0] = 0x67452301;
  ctx->state[1] = 0xefcdab89;
  ctx->state[2] = 0x98badcfe;
  ctx->state[3] = 0x10325476;
  return;
}

void md5_update(card_md5_rec *ctx, uchar *src, uint len)
{
  uint i, used, room;
  
  used = (ctx->bit_count[0] >> 3) & 0x3f;
  if ((ctx->bit_count[0] += (len << 3)) < (len << 3))
    (ctx->bit_count[1])++;
  ctx->bit_count[1] += (len >> 29);
  room = 64 - used;
  if (len >= room) {
    md5_memcpy(&ctx->block[used],src,room);
    md5_transform(ctx->state,ctx->block);
    for (i = room; i + 63 < len; i += 64)
      md5_transform(ctx->state,src + i);
    used = 0;
  }
  else
    i = 0;
  md5_memcpy(&ctx->block[used],src + i,len - i);
}

void md5_final(uchar *digest, card_md5_rec *ctx)
{
  uint used;
  int pad_len;
  uchar count_bytes [8];
  
  md5_encode(count_bytes,ctx->bit_count,8);
  used = ctx->bit_count[0] >> 3 & 0x3f;
  pad_len = (used < 0x38) ? (0x38 - used) : (0x78 - used);
  md5_update(ctx,md5_padding,pad_len);
  md5_update(ctx,count_bytes,8);
  md5_encode(digest,ctx->state,0x10);
  md5_memset((uchar *)ctx,0,0x58);
  return;
}

void md5_transform(uint *state, uchar *block)
{
  uint a = state[0], b = state[1], c = state[2], d = state[3], x[16];
  
  md5_decode(x,block,64);
  a += ((b & c) | (~b & d)) + x[0] + 0xd76aa478;
  a = a << 7 | a >> 25;
  a += b;
  d += ((a & b) | (~a & c)) + x[1] + 0xe8c7b756;
  d = d << 12 | d >> 20;
  d += a;
  c += ((d & a) | (~d & b)) + x[2] + 0x242070db;
  c = c << 17 | c >> 15;
  c += d;
  b += ((c & d) | (~c & a)) + x[3] + 0xc1bdceee;
  b = b << 22 | b >> 10;
  b += c;
  a += ((b & c) | (~b & d)) + x[4] + 0xf57c0faf;
  a = a << 7 | a >> 25;
  a += b;
  d += ((a & b) | (~a & c)) + x[5] + 0x4787c62a;
  d = d << 12 | d >> 20;
  d += a;
  c += ((d & a) | (~d & b)) + x[6] + 0xa8304613;
  c = c << 17 | c >> 15;
  c += d;
  b += ((c & d) | (~c & a)) + x[7] + 0xfd469501;
  b = b << 22 | b >> 10;
  b += c;
  a += ((b & c) | (~b & d)) + x[8] + 0x698098d8;
  a = a << 7 | a >> 25;
  a += b;
  d += ((a & b) | (~a & c)) + x[9] + 0x8b44f7af;
  d = d << 12 | d >> 20;
  d += a;
  c += ((d & a) | (~d & b)) + x[10] + 0xffff5bb1;
  c = c << 17 | c >> 15;
  c += d;
  b += ((c & d) | (~c & a)) + x[11] + 0x895cd7be;
  b = b << 22 | b >> 10;
  b += c;
  a += ((b & c) | (~b & d)) + x[12] + 0x6b901122;
  a = a << 7 | a >> 25;
  a += b;
  d += ((a & b) | (~a & c)) + x[13] + 0xfd987193;
  d = d << 12 | d >> 20;
  d += a;
  c += ((d & a) | (~d & b)) + x[14] + 0xa679438e;
  c = c << 17 | c >> 15;
  c += d;
  b += ((c & d) | (~c & a)) + x[15] + 0x49b40821;
  b = b << 22 | b >> 10;
  b += c;
  a += ((b & d) | (c & ~d)) + x[1] + 0xf61e2562;
  a = a << 5 | a >> 27;
  a += b;
  d += ((a & c) | (b & ~c)) + x[6] + 0xc040b340;
  d = d << 9 | d >> 23;
  d += a;
  c += ((d & b) | (a & ~b)) + x[11] + 0x265e5a51;
  c = c << 14 | c >> 18;
  c += d;
  b += ((c & a) | (d & ~a)) + x[0] + 0xe9b6c7aa;
  b = b << 20 | b >> 12;
  b += c;
  a += ((b & d) | (c & ~d)) + x[5] + 0xd62f105d;
  a = a << 5 | a >> 27;
  a += b;
  d += ((a & c) | (b & ~c)) + x[10] + 0x2441453;
  d = d << 9 | d >> 23;
  d += a;
  c += ((d & b) | (a & ~b)) + x[15] + 0xd8a1e681;
  c = c << 14 | c >> 18;
  c += d;
  b += ((c & a) | (d & ~a)) + x[4] + 0xe7d3fbc8;
  b = b << 20 | b >> 12;
  b += c;
  a += ((b & d) | (c & ~d)) + x[9] + 0x21e1cde6;
  a = a << 5 | a >> 27;
  a += b;
  d += ((a & c) | (b & ~c)) + x[14] + 0xc33707d6;
  d = d << 9 | d >> 23;
  d += a;
  c += ((d & b) | (a & ~b)) + x[3] + 0xf4d50d87;
  c = c << 14 | c >> 18;
  c += d;
  b += ((c & a) | (d & ~a)) + x[8] + 0x455a14ed;
  b = b << 20 | b >> 12;
  b += c;
  a += ((b & d) | (c & ~d)) + x[13] + 0xa9e3e905;
  a = a << 5 | a >> 27;
  a += b;
  d += ((a & c) | (b & ~c)) + x[2] + 0xfcefa3f8;
  d = d << 9 | d >> 23;
  d += a;
  c += ((d & b) | (a & ~b)) + x[7] + 0x676f02d9;
  c = c << 14 | c >> 18;
  c += d;
  b += ((c & a) | (d & ~a)) + x[12] + 0x8d2a4c8a;
  b = b << 20 | b >> 12;
  b += c;
  a += (b ^ c ^ d) + x[5] + 0xfffa3942;
  a = a << 4 | a >> 28;
  a += b;
  d += (a ^ b ^ c) + x[8] + 0x8771f681;
  d = d << 11 | d >> 21;
  d += a;
  c += (d ^ a ^ b) + x[11] + 0x6d9d6122;
  c = c << 16 | c >> 16;
  c += d;
  b += (c ^ d ^ a) + x[14] + 0xfde5380c;
  b = b << 23 | b >> 9;
  b += c;
  a += (b ^ c ^ d) + x[1] + 0xa4beea44;
  a = a << 4 | a >> 28;
  a += b;
  d += (a ^ b ^ c) + x[4] + 0x4bdecfa9;
  d = d << 11 | d >> 21;
  d += a;
  c += (d ^ a ^ b) + x[7] + 0xf6bb4b60;
  c = c << 16 | c >> 16;
  c += d;
  b += (c ^ d ^ a) + x[10] + 0xbebfbc70;
  b = b << 23 | b >> 9;
  b += c;
  a += (b ^ c ^ d) + x[13] + 0x289b7ec6;
  a = a << 4 | a >> 28;
  a += b;
  d += (a ^ b ^ c) + x[0] + 0xeaa127fa;
  d = d << 11 | d >> 21;
  d += a;
  c += (d ^ a ^ b) + x[3] + 0xd4ef3085;
  c = c << 16 | c >> 16;
  c += d;
  b += (c ^ d ^ a) + x[6] + 0x4881d05;
  b = b << 23 | b >> 9;
  b += c;
  a += (b ^ c ^ d) + x[9] + 0xd9d4d039;
  a = a << 4 | a >> 28;
  a += b;
  d += (a ^ b ^ c) + x[12] + 0xe6db99e5;
  d = d << 11 | d >> 21;
  d += a;
  c += (d ^ a ^ b) + x[15] + 0x1fa27cf8;
  c = c << 16 | c >> 16;
  c += d;
  b += (c ^ d ^ a) + x[2] + 0xc4ac5665;
  b = b << 23 | b >> 9;
  b += c;
  a += (c ^ (b | ~d)) + x[0] + 0xf4292244;
  a = a << 6 | a >> 26;
  a += b;
  d += (b ^ (a | ~c)) + x[7] + 0x432aff97;
  d = d << 10 | d >> 22;
  d += a;
  c += (a ^ (d | ~b)) + x[14] + 0xab9423a7;
  c = c << 15 | c >> 17;
  c += d;
  b += (d ^ (c | ~a)) + x[5] + 0xfc93a039;
  b = b << 21 | b >> 11;
  b += c;
  a += (c ^ (b | ~d)) + x[12] + 0x655b59c3;
  a = a << 6 | a >> 26;
  a += b;
  d += (b ^ (a | ~c)) + x[3] + 0x8f0ccc92;
  d = d << 10 | d >> 22;
  d += a;
  c += (a ^ (d | ~b)) + x[10] + 0xffeff47d;
  c = c << 15 | c >> 17;
  c += d;
  b += (d ^ (c | ~a)) + x[1] + 0x85845dd1;
  b = b << 21 | b >> 11;
  b += c;
  a += (c ^ (b | ~d)) + x[8] + 0x6fa87e4f;
  a = a << 6 | a >> 26;
  a += b;
  d += (b ^ (a | ~c)) + x[15] + 0xfe2ce6e0;
  d = d << 10 | d >> 22;
  d += a;
  c += (a ^ (d | ~b)) + x[6] + 0xa3014314;
  c = c << 15 | c >> 17;
  c += d;
  b += (d ^ (c | ~a)) + x[13] + 0x4e0811a1;
  b = b << 21 | b >> 11;
  b += c;
  a += (c ^ (b | ~d)) + x[4] + 0xf7537e82;
  a = a << 6 | a >> 26;
  a += b;
  d += (b ^ (a | ~c)) + x[11] + 0xbd3af235;
  d = d << 10 | d >> 22;
  d += a;
  c += (a ^ (d | ~b)) + x[2] + 0x2ad7d2bb;
  c = c << 15 | c >> 17;
  c += d;
  b += (d ^ (c | ~a)) + x[9] + 0xeb86d391;
  b = b << 21 | b >> 11;
  b += c;
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  md5_memset((uchar *)x,0,64);
}

void md5_encode(uchar *dst, uint *src, uint len)
{
  uint i;
  int j;
  j = 0;
  for (i = 0; i < len; i += 4) {
    dst[i] = src[j];
    dst[i + 1] = src[j] >> 8;
    dst[i + 2] = src[j] >> 16;
    dst[i + 3] = src[j] >> 24;
    j++;
  }
}

void md5_decode(uint *dst, uchar *src, uint len)
{
  uint i;
  uint j;
  for (i = 0, j = 0; j < len; i++, j += 4)
    dst[i] = ((uint)src[j]) | (((uint)src[j+1]) << 8) | (((uint)src[j+2]) << 16) | (((uint)src[j+3]) << 24);
}

void md5_memcpy(uchar *dst, uchar *src, uint len)
{
  uint i;
  for (i = 0; i < len; i++) {
    dst[i] = src[i];
  }
}

void md5_memset(uchar *dst, int value, uint len)
{
  uint i;
  for (i = 0; i < len; i++) dst[i] = (uchar)value;
}

uint card_key_hash(int value)
{
  card_md5_rec ctx;
  uchar d[16];
  uint h;

  h = value * 2 ^ 0xdeadbeef;
  md5_init(&ctx);
  h = h >> 0x18 | (h & 0xff0000) >> 8 | (h >> 8 & 0xff) << 0x10 | h << 0x18;
  md5_update(&ctx,(uchar *)&h,4);
  md5_final(d,&ctx);
  h = 0;
  h ^= (d[2] << 24) + (d[4] << 16) + (d[0] << 8) + d[11];
  h ^= (d[8] << 24) + (d[7] << 16) + (d[5] << 8) + d[13];
  h ^= (d[15] << 24) + (d[1] << 16) + (d[12] << 8) + d[14];
  h ^= (d[10] << 24) + (d[3] << 16) + (d[9] << 8) + d[6];
  return h;
}
