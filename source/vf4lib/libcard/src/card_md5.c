/*
 * vf4lib/libcard/src/card_md5.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uchar card_md5_padding[64]; /* 0x0c2a02ac */

void card_md5_init(card_md5_rec *ctx)
{
  ctx->bit_count[1] = 0;
  ctx->bit_count[0] = 0;
  ctx->state[0] = 0x67452301;
  ctx->state[1] = 0xefcdab89;
  ctx->state[2] = 0x98badcfe;
  ctx->state[3] = 0x10325476;
  return;
}

void card_md5_update(card_md5_rec *ctx, uchar *src, uint len)
{
  uint i;
  uint used;
  uint room;

  used = (ctx->bit_count[0] >> 3) & 0x3f;
  if ((ctx->bit_count[0] += len << 3) < (len << 3))
    (ctx->bit_count[1])++;
  ctx->bit_count[1] += len >> 29;
  room = 64 - used;
  if (len >= room) {
    card_md5_copy(&ctx->block[used],src,room);
    card_md5_transform(ctx->state,ctx->block);
    for (i = room; i + 63 < len; i += 64)
      card_md5_transform(ctx->state,src + i);
    used = 0;
  }
  else
    i = 0;
  card_md5_copy(&ctx->block[used],src + i,len - i);
}

void card_md5_final(uchar *digest, card_md5_rec *ctx)
{
  uint used;
  int pad_len;
  uchar count_bytes [8];

  card_md5_encode(count_bytes,ctx->bit_count,8);
  used = ctx->bit_count[0] >> 3 & 0x3f;
  pad_len = (used < 56) ? (56 - used) : (120 - used);
  card_md5_update(ctx,card_md5_padding,pad_len);
  card_md5_update(ctx,count_bytes,8);
  card_md5_encode(digest,ctx->state,0x10);
  card_md5_fill((uchar *)ctx,0,0x58);
  return;
}

void card_md5_transform(uint *state, uchar *block)
{
  uint a = state[0];
  uint b = state[1];
  uint c = state[2];
  uint d = state[3];
  uint x[16];

  card_md5_decode(x, block, 64);
  a += ((b & c) | (~b & d)) + x[0] + 0xd76aa478U;
  a = (a << 7) | (a >> 25);
  a += b;
  d += ((a & b) | (~a & c)) + x[1] + 0xe8c7b756U;
  d = (d << 12) | (d >> 20);
  d += a;
  c += ((d & a) | (~d & b)) + x[2] + 0x242070dbU;
  c = (c << 17) | (c >> 15);
  c += d;
  b += ((c & d) | (~c & a)) + x[3] + 0xc1bdceeeU;
  b = (b << 22) | (b >> 10);
  b += c;
  a += ((b & c) | (~b & d)) + x[4] + 0xf57c0fafU;
  a = (a << 7) | (a >> 25);
  a += b;
  d += ((a & b) | (~a & c)) + x[5] + 0x4787c62aU;
  d = (d << 12) | (d >> 20);
  d += a;
  c += ((d & a) | (~d & b)) + x[6] + 0xa8304613U;
  c = (c << 17) | (c >> 15);
  c += d;
  b += ((c & d) | (~c & a)) + x[7] + 0xfd469501U;
  b = (b << 22) | (b >> 10);
  b += c;
  a += ((b & c) | (~b & d)) + x[8] + 0x698098d8U;
  a = (a << 7) | (a >> 25);
  a += b;
  d += ((a & b) | (~a & c)) + x[9] + 0x8b44f7afU;
  d = (d << 12) | (d >> 20);
  d += a;
  c += ((d & a) | (~d & b)) + x[10] + 0xffff5bb1U;
  c = (c << 17) | (c >> 15);
  c += d;
  b += ((c & d) | (~c & a)) + x[11] + 0x895cd7beU;
  b = (b << 22) | (b >> 10);
  b += c;
  a += ((b & c) | (~b & d)) + x[12] + 0x6b901122U;
  a = (a << 7) | (a >> 25);
  a += b;
  d += ((a & b) | (~a & c)) + x[13] + 0xfd987193U;
  d = (d << 12) | (d >> 20);
  d += a;
  c += ((d & a) | (~d & b)) + x[14] + 0xa679438eU;
  c = (c << 17) | (c >> 15);
  c += d;
  b += ((c & d) | (~c & a)) + x[15] + 0x49b40821U;
  b = (b << 22) | (b >> 10);
  b += c;
  a += ((b & d) | (c & ~d)) + x[1] + 0xf61e2562U;
  a = (a << 5) | (a >> 27);
  a += b;
  d += ((a & c) | (b & ~c)) + x[6] + 0xc040b340U;
  d = (d << 9) | (d >> 23);
  d += a;
  c += ((d & b) | (a & ~b)) + x[11] + 0x265e5a51U;
  c = (c << 14) | (c >> 18);
  c += d;
  b += ((c & a) | (d & ~a)) + x[0] + 0xe9b6c7aaU;
  b = (b << 20) | (b >> 12);
  b += c;
  a += ((b & d) | (c & ~d)) + x[5] + 0xd62f105dU;
  a = (a << 5) | (a >> 27);
  a += b;
  d += ((a & c) | (b & ~c)) + x[10] + 0x02441453U;
  d = (d << 9) | (d >> 23);
  d += a;
  c += ((d & b) | (a & ~b)) + x[15] + 0xd8a1e681U;
  c = (c << 14) | (c >> 18);
  c += d;
  b += ((c & a) | (d & ~a)) + x[4] + 0xe7d3fbc8U;
  b = (b << 20) | (b >> 12);
  b += c;
  a += ((b & d) | (c & ~d)) + x[9] + 0x21e1cde6U;
  a = (a << 5) | (a >> 27);
  a += b;
  d += ((a & c) | (b & ~c)) + x[14] + 0xc33707d6U;
  d = (d << 9) | (d >> 23);
  d += a;
  c += ((d & b) | (a & ~b)) + x[3] + 0xf4d50d87U;
  c = (c << 14) | (c >> 18);
  c += d;
  b += ((c & a) | (d & ~a)) + x[8] + 0x455a14edU;
  b = (b << 20) | (b >> 12);
  b += c;
  a += ((b & d) | (c & ~d)) + x[13] + 0xa9e3e905U;
  a = (a << 5) | (a >> 27);
  a += b;
  d += ((a & c) | (b & ~c)) + x[2] + 0xfcefa3f8U;
  d = (d << 9) | (d >> 23);
  d += a;
  c += ((d & b) | (a & ~b)) + x[7] + 0x676f02d9U;
  c = (c << 14) | (c >> 18);
  c += d;
  b += ((c & a) | (d & ~a)) + x[12] + 0x8d2a4c8aU;
  b = (b << 20) | (b >> 12);
  b += c;
  a += (b ^ c ^ d) + x[5] + 0xfffa3942U;
  a = (a << 4) | (a >> 28);
  a += b;
  d += (a ^ b ^ c) + x[8] + 0x8771f681U;
  d = (d << 11) | (d >> 21);
  d += a;
  c += (d ^ a ^ b) + x[11] + 0x6d9d6122U;
  c = (c << 16) | (c >> 16);
  c += d;
  b += (c ^ d ^ a) + x[14] + 0xfde5380cU;
  b = (b << 23) | (b >> 9);
  b += c;
  a += (b ^ c ^ d) + x[1] + 0xa4beea44U;
  a = (a << 4) | (a >> 28);
  a += b;
  d += (a ^ b ^ c) + x[4] + 0x4bdecfa9U;
  d = (d << 11) | (d >> 21);
  d += a;
  c += (d ^ a ^ b) + x[7] + 0xf6bb4b60U;
  c = (c << 16) | (c >> 16);
  c += d;
  b += (c ^ d ^ a) + x[10] + 0xbebfbc70U;
  b = (b << 23) | (b >> 9);
  b += c;
  a += (b ^ c ^ d) + x[13] + 0x289b7ec6U;
  a = (a << 4) | (a >> 28);
  a += b;
  d += (a ^ b ^ c) + x[0] + 0xeaa127faU;
  d = (d << 11) | (d >> 21);
  d += a;
  c += (d ^ a ^ b) + x[3] + 0xd4ef3085U;
  c = (c << 16) | (c >> 16);
  c += d;
  b += (c ^ d ^ a) + x[6] + 0x04881d05U;
  b = (b << 23) | (b >> 9);
  b += c;
  a += (b ^ c ^ d) + x[9] + 0xd9d4d039U;
  a = (a << 4) | (a >> 28);
  a += b;
  d += (a ^ b ^ c) + x[12] + 0xe6db99e5U;
  d = (d << 11) | (d >> 21);
  d += a;
  c += (d ^ a ^ b) + x[15] + 0x1fa27cf8U;
  c = (c << 16) | (c >> 16);
  c += d;
  b += (c ^ d ^ a) + x[2] + 0xc4ac5665U;
  b = (b << 23) | (b >> 9);
  b += c;
  a += (c ^ (b | ~d)) + x[0] + 0xf4292244U;
  a = (a << 6) | (a >> 26);
  a += b;
  d += (b ^ (a | ~c)) + x[7] + 0x432aff97U;
  d = (d << 10) | (d >> 22);
  d += a;
  c += (a ^ (d | ~b)) + x[14] + 0xab9423a7U;
  c = (c << 15) | (c >> 17);
  c += d;
  b += (d ^ (c | ~a)) + x[5] + 0xfc93a039U;
  b = (b << 21) | (b >> 11);
  b += c;
  a += (c ^ (b | ~d)) + x[12] + 0x655b59c3U;
  a = (a << 6) | (a >> 26);
  a += b;
  d += (b ^ (a | ~c)) + x[3] + 0x8f0ccc92U;
  d = (d << 10) | (d >> 22);
  d += a;
  c += (a ^ (d | ~b)) + x[10] + 0xffeff47dU;
  c = (c << 15) | (c >> 17);
  c += d;
  b += (d ^ (c | ~a)) + x[1] + 0x85845dd1U;
  b = (b << 21) | (b >> 11);
  b += c;
  a += (c ^ (b | ~d)) + x[8] + 0x6fa87e4fU;
  a = (a << 6) | (a >> 26);
  a += b;
  d += (b ^ (a | ~c)) + x[15] + 0xfe2ce6e0U;
  d = (d << 10) | (d >> 22);
  d += a;
  c += (a ^ (d | ~b)) + x[6] + 0xa3014314U;
  c = (c << 15) | (c >> 17);
  c += d;
  b += (d ^ (c | ~a)) + x[13] + 0x4e0811a1U;
  b = (b << 21) | (b >> 11);
  b += c;
  a += (c ^ (b | ~d)) + x[4] + 0xf7537e82U;
  a = (a << 6) | (a >> 26);
  a += b;
  d += (b ^ (a | ~c)) + x[11] + 0xbd3af235U;
  d = (d << 10) | (d >> 22);
  d += a;
  c += (a ^ (d | ~b)) + x[2] + 0x2ad7d2bbU;
  c = (c << 15) | (c >> 17);
  c += d;
  b += (d ^ (c | ~a)) + x[9] + 0xeb86d391U;
  b = (b << 21) | (b >> 11);
  b += c;
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  card_md5_fill((uchar *)x, 0, 64);
}

void card_md5_encode(uchar *dst, uint *src, uint len)
{
  uint j;
  int i = 0;
  for (j = 0; j < len; j += 4) {
    dst[j] = src[i];
    dst[j + 1] = src[i] >> 8;
    dst[j + 2] = src[i] >> 16;
    dst[j + 3] = src[i] >> 24;
    i++;
  }
}

void card_md5_decode(uint *dst, uchar *src, uint len)
{
  int i;
  uint j;
  i = 0;
  for (j = 0; j < len; j += 4) {
    dst[i] = src[j] | src[j + 1] << 8 | src[j + 2] << 16 | src[j + 3] << 24;
    i++;
  }
}

void card_md5_copy(uchar *dst, uchar *src, uint len)
{
  uint i;
  for (i = 0; i < len; i++) {
    dst[i] = src[i];
  }
}

void card_md5_fill(uchar *dst, int value, uint len)
{
  uint i;
  
  for (i = 0; i < len; i++) {
    dst[i] = value;
  }
}
