/*
 * vf4lib/libcard/src/card_common.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern ushort card_crc_tables[2][256]; /* 0x0c4317bc */
extern ushort card_crc_polys[2]; /* 0x0c18a86c */

static inline uchar *card_header(int side)
{
  return &card_read_buffer + side * 0x80;
}

void card_crc_tables_init(void)
{
  int i;
  
  i = 0;
  do {
    card_crc_table_build((uint)(ushort)card_crc_polys[i],card_crc_tables[i]);
    i = i + 1;
  } while (i < 2);
  return;
}

void card_crc_table_build(uint poly, ushort *table)
{
  ushort c;
  ushort p = poly;
  int i;
  int j;
  for (i = 0; i < 0x100; i++) {
    c = i << 8;
    for (j = 0; j < 8; j++) {
      if (c & 0x8000) c = (c << 1) ^ p;
      else c = c << 1;
    }
    table[i] = c;
  }
}

uint card_crc16(uint type, int len, char *src)
{
  ushort crc = 0;
  short *tab = (short *)(card_crc_tables[type]);
  int i;
  
  if (1 < type) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/card_common.c",0x60,"assertion `%s\' failed.\n",
             "type < CRC_TYPE_MAX");
  }
  for (i = 0; i < len; i++) {
    crc = crc << 8 ^ tab[(crc >> 8 ^ *src++) & 0xff];
  }
  return crc;
}

uchar card_unit_result(int side)
{
  return (&card_status)[side * 8 + 3];
}

uchar card_read_start(int side)
{
  uchar status;

  if ((status = (&card_status)[side * 8 + 2]) == 0) {
    card_queue_begin(side);
    card_queue_d0(side);
    card_queue_a0(side);
    card_queue_read_key(side);
    card_queue_send_key(side);
    card_queue_read(side);
    return 0;
  }
  return status;
}

uchar card_write_start(int side, int crc_update)
{
  uchar update = crc_update;
  short *header_crc = &card_records[side].header.crc;
  card_header_rec *header_copy = &card_records[side].header2;
  short *body_crc = &card_records[side].body.crc;
  uchar status;
  int blank;
  int i;
  uchar packed[108];
  
  status = (&card_status)[side * 8 + 2];
  if (status != 0) return status;
  {
    card_pack(side,packed);
    if (update == 1) {
      *header_crc = card_crc16(1,0x16,packed + 2);
      *body_crc = card_crc16(1,0x3a,packed + 0x1a);
    }
    blank = 1;
    if (packed[107] != 0xff) blank = 0;
    for (i = 0; i < 107; i++) {
      if (packed[i] != 0) blank = 0;
    }
    if (!blank) {
      memcpy(header_copy,header_crc,0x1c);
    }
    card_pack(side,card_records[side].write_buf);
    card_queue_begin(side);
    card_queue_d0(side);
    card_queue_write(side,5,6,card_records[side].write_buf);
    card_queue_write(side,0xb,6,&card_records[side].write_buf[24]);
    card_queue_write(side,0x11,6,&card_records[side].write_buf[48]);
    card_queue_write(side,0x17,6,&card_records[side].write_buf[72]);
    card_queue_write(side,0x1d,3,&card_records[side].write_buf[96]);
    card_queue_c1(side);
  }
  return status;
}

void card_read_raw_copy(int side, uchar *out)
{
  uchar *src;
  int i;

  src = &card_read_buffer + side * 0x80 + 0x14;
  for (i = 0; i < 0x6c; i++) {
    *out++ = *src++;
  }
  return;
}

void card_read_finish(int side)
{
  uchar *src;
  uchar *dst;
  int i;

  src = &card_read_buffer + side * 0x80 + 0x14;
  card_unpack(side,src);
  src = card_header(side);
  card_records[side].card_word0 = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
  card_records[side].access_key = src[4] << 24 | src[5] << 16 | src[6] << 8 | src[7];
  card_records[side].card_word3 = src[12] << 24 | src[13] << 16 | src[14] << 8 | src[15];
  card_records[side].unit_key = *(uint *)(&card_keys + side * 4);
  src = &card_read_buffer + side * 0x80 + 0x14;
  dst = card_records[side].raw;
  for (i = 0; i < 0x6c; i++) {
    *dst++ = *src++;
  }
}

void card_unpack_format2(int side, uchar *src)
{
  card_fmt2_header_rec *header = &card_records[side].fmt2_header;
  card_fmt2_header_rec *header_copy = &card_records[side].fmt2_header2;
  card_fmt2_body_rec *body = &card_records[side].fmt2_body;
  int i;
  
  header->crc = src[1] << 8 | src[0];
  header->flags = src[2];
  header->char_num = src[3];
  header->level = src[4];
  header->all_win = src[6] << 8 | src[5];
  header->all_lose = src[8] << 8 | src[7];
  for (i = 0; i < 8; i++) {
    header->item_have[i] = src[9 + i];
  }
  header->use_count = src[18] << 8 | src[17];
  for (i = 0; i < 3; i++) {
    header->card_id[i] = src[19 + i];
  }
  header->rank_pnt = src[23] << 8 | src[22];
  header->tenpo_pnt = src[25] << 8 | src[24];
  body->crc = src[27] << 8 | src[26];
  body->play_id = src[29] << 8 | src[28];
  body->exp = src[30];
  body->point = src[31];
  body->color_1p_packed.h = src[33] << 8 | src[32];
  body->color_2p_packed.h = src[35] << 8 | src[34];
  for (i = 0; i < 4; i++) {
    body->equip_1p[i] = src[36 + i];
  }
  for (i = 0; i < 4; i++) {
    body->equip_2p[i] = src[40 + i];
  }
  body->curse = src[44];
  body->emblem = src[45];
  for (i = 0; i < 20; i++) {
    body->name[i] = src[46 + i];
  }
  body->other_col = src[66];
  body->temp = src[67];
  body->my_tenpo_id = src[69] << 8 | src[68];
  body->money = src[71] << 8 | src[70];
  body->emblem_dan = src[72];
  body->syuutyuu = src[74] << 8 | src[73];
  body->quest0 = src[76] << 8 | src[75];
  body->quest1 = src[78] << 8 | src[77];
  for (i = 0; i < 3; i++) {
    body->misc[i] = src[79 + i];
  }
  header_copy->crc = src[83] << 8 | src[82];
  header_copy->flags = src[84];
  header_copy->char_num = src[85];
  header_copy->level = src[86];
  header_copy->all_win = src[88] << 8 | src[87];
  header_copy->all_lose = src[90] << 8 | src[89];
  for (i = 0; i < 8; i++) {
    header_copy->item_have[i] = src[91 + i];
  }
  header_copy->use_count = src[100] << 8 | src[99];
  for (i = 0; i < 3; i++) {
    header_copy->card_id[i] = src[101 + i];
  }
  header_copy->rank_pnt = src[105] << 8 | src[104];
  header_copy->tenpo_pnt = src[107] << 8 | src[106];
}

void card_unpack_blocks(int side, uchar *src)
{
  card_header_rec *header;
  card_header_rec *header_copy;
  card_body_rec *body;
  int i;
  uint color;

  header = &card_records[side].header;
  header_copy = &card_records[side].header2;
  body = &card_records[side].body;
  header->crc = src[1] << 8 | src[0];
  header->flags = src[2];
  header->char_num = src[3];
  header->level = src[4];
  header->all_win = src[6] << 8 | src[5];
  header->all_lose = src[8] << 8 | src[7];
  for (i = 0; i < 8; i++)
    header->item_have[i] = src[i + 9];
  header->use_count = (src[18] & 1) << 8 | src[17];
  header->ftf.b[0] = src[18] & 0xfe;
  for (i = 0; i < 3; i++)
    header->card_id[i] = src[i + 19];
  header->nickname = src[23] << 8 | src[22];
  body->crc = src[25] << 8 | src[24];
  body->play_id = src[27] << 8 | src[26];
  body->exp = src[28];
  body->point = src[29];
  body->color_1p_packed.h = src[31] << 8 | src[30];
  body->color_2p_packed.h = src[33] << 8 | src[32];
  for (i = 0; i < 4; i++)
    body->equip_1p[i] = src[i + 34];
  for (i = 0; i < 4; i++)
    body->equip_2p[i] = src[i + 38];
  body->curse.b.curse = src[42];
  body->curse.b.emblem = src[43];
  for (i = 0; i < 20; i++)
    body->name[i] = src[i + 44];
  body->other_col = src[64];
  body->temp = src[65];
  body->my_tenpo_id = src[67] << 8 | src[66];
  body->money = src[69] << 8 | src[68];
  body->syu.b.emblem_dan = src[70];
  body->syu.b.syuutyuu = src[72] << 8 | src[71];
  body->quest.b.quest0 = src[74] << 8 | src[73];
  body->quest.b.quest1 = src[76] << 8 | src[75];
  for (i = 0; i < 3; i++)
    body->misc.b[i] = src[i + 77];
  body->rank_pnt = src[81] << 8 | src[80];
  body->tenpo_pnt = src[83] << 8 | src[82];
  header_copy->crc = src[85] << 8 | src[84];
  header_copy->flags = src[86];
  header_copy->char_num = src[87];
  header_copy->level = src[88];
  header_copy->all_win = src[90] << 8 | src[89];
  header_copy->all_lose = src[92] << 8 | src[91];
  for (i = 0; i < 8; i++)
    header_copy->item_have[i] = src[i + 93];
  header_copy->use_count = (src[102] & 1) << 8 | src[101];
  header_copy->ftf.b[0] = src[102] & 0xfe;
  for (i = 0; i < 3; i++)
    header_copy->card_id[i] = src[i + 103];
  header_copy->nickname = src[107] << 8 | src[106];
  color = 0;
  for (i = 0; i < 4; i++)
    color |= ((body->color_1p_packed.h >> (i * 4)) & 0xf) << (i * 5);
  color |= (body->curse.f.color_1p_hi0 << 4 | body->curse.f.color_1p_hi1 << 9 | body->curse.f.color_1p_hi2 << 14 | body->curse.f.color_1p_hi3 << 19);
  body->color_1p.w = color;
  color = 0;
  for (i = 0; i < 4; i++)
    color |= ((body->color_2p_packed.h >> (i * 4)) & 0xf) << (i * 5);
  color |= (body->misc.f.color_2p_hi0 << 4 | body->misc.f.color_2p_hi1 << 9 | body->misc.f.color_2p_hi2 << 14 | body->misc.f.color_2p_hi3 << 19);
  body->color_2p.w = color;
  card_format2_area_clear((char)side);
}

uint card_format_get(card_header_rec *header)
{
  uint format;
  
  format = header->flags >> 4 & 3;
  if (format == 3) {
    format = (header->ftf.b[0] >> 1 & 3) + 3;
  }
  return format;
}

uint card_format_field_get(card_header_rec *header)
{
  return header->flags >> 4 & 3;
}

void card_unpack(int side, uchar *src)
{
  card_record_rec *rec;
  uint format;
  rec = &card_records[side];
  rec->header.flags = src[2];
  rec->header.ftf.b[0] = src[0x12] & 0xfe;
  format = card_format_get(&rec->header);
  if ((uchar)(format - 3) < 2) {
    card_unpack_blocks(side,src);
  }
  else if (format == 2) {
    card_unpack_format2(side,src);
  }
  else {
    card_unpack_blocks(side,src);
  }
}

void card_pack(int side, uchar *dst)
{
  card_header_rec *header;
  card_header_rec *header_copy;
  card_body_rec *body;
  int i;

  header = &card_records[side].header;
  header_copy = &card_records[side].header2;
  body = &card_records[side].body;
  card_colors_to_packed(body);
  dst[0] = ((char *)&header->crc)[0];
  dst[1] = ((char *)&header->crc)[1];
  dst[2] = header->flags;
  dst[3] = header->char_num;
  dst[4] = header->level;
  dst[5] = ((char *)&header->all_win)[0];
  dst[6] = ((char *)&header->all_win)[1];
  dst[7] = ((char *)&header->all_lose)[0];
  dst[8] = ((char *)&header->all_lose)[1];
  for (i = 0; i < 8; i++) dst[i + 9] = header->item_have[i];
  dst[17] = ((char *)&header->use_count)[0];
  dst[18] = ((char *)&header->use_count)[1] | header->ftf.b[0] & 0xfe;
  for (i = 0; i < 3; i++) dst[i + 19] = header->card_id[i];
  dst[22] = ((char *)&header->nickname)[0];
  dst[23] = ((char *)&header->nickname)[1];
  dst[24] = ((char *)&body->crc)[0];
  dst[25] = ((char *)&body->crc)[1];
  dst[26] = ((char *)&body->play_id)[0];
  dst[27] = ((char *)&body->play_id)[1];
  dst[28] = body->exp;
  dst[29] = body->point;
  dst[30] = body->color_1p_packed.b[0];
  dst[31] = body->color_1p_packed.b[1];
  dst[32] = body->color_2p_packed.b[0];
  dst[33] = body->color_2p_packed.b[1];
  for (i = 0; i < 4; i++) dst[i + 34] = body->equip_1p[i];
  for (i = 0; i < 4; i++) dst[i + 38] = body->equip_2p[i];
  dst[42] = body->curse.b.curse;
  dst[43] = body->curse.b.emblem;
  for (i = 0; i < 20; i++) dst[i + 44] = body->name[i];
  dst[64] = body->other_col;
  dst[65] = body->temp;
  dst[66] = ((char *)&body->my_tenpo_id)[0];
  dst[67] = ((char *)&body->my_tenpo_id)[1];
  dst[68] = ((char *)&body->money)[0];
  dst[69] = ((char *)&body->money)[1];
  dst[70] = body->syu.b.emblem_dan;
  dst[71] = ((char *)&body->syu.b.syuutyuu)[0];
  dst[72] = ((char *)&body->syu.b.syuutyuu)[1];
  dst[73] = ((char *)&body->quest.b.quest0)[0];
  dst[74] = ((char *)&body->quest.b.quest0)[1];
  dst[75] = ((char *)&body->quest.b.quest1)[0];
  dst[76] = ((char *)&body->quest.b.quest1)[1];
  for (i = 0; i < 3; i++) dst[i + 77] = body->misc.b[i];
  dst[80] = ((char *)&body->rank_pnt)[0];
  dst[81] = ((char *)&body->rank_pnt)[1];
  dst[82] = ((char *)&body->tenpo_pnt)[0];
  dst[83] = ((char *)&body->tenpo_pnt)[1];
  dst[84] = ((char *)&header_copy->crc)[0];
  dst[85] = ((char *)&header_copy->crc)[1];
  dst[86] = header_copy->flags;
  dst[87] = header_copy->char_num;
  dst[88] = header_copy->level;
  dst[89] = ((char *)&header_copy->all_win)[0];
  dst[90] = ((char *)&header_copy->all_win)[1];
  dst[91] = ((char *)&header_copy->all_lose)[0];
  dst[92] = ((char *)&header_copy->all_lose)[1];
  for (i = 0; i < 8; i++) dst[i + 93] = header_copy->item_have[i];
  dst[101] = ((char *)&header_copy->use_count)[0];
  dst[102] = ((char *)&header_copy->use_count)[1] | header_copy->ftf.b[0] & 0xfe;
  for (i = 0; i < 3; i++) dst[i + 103] = header_copy->card_id[i];
  dst[106] = ((char *)&header_copy->nickname)[0];
  dst[107] = ((char *)&header_copy->nickname)[1];
}

uchar card_write_word_start(int side, int block, uint word)
{
  unsigned char status;

  if (block - 5U > 26) return 1;
  status = (&card_status)[side * 8 + 2];
  if (status == 0) {
    card_queue_begin(side);
    card_queue_d0(side);
    card_queue_write(side,block,1,(uchar *)&word);
    card_queue_c1(side);
  }
  return status;
}

int card_lock_start(int side)
{
  unsigned char status;
  
  status = (&card_status)[side * 8 + 2];
  if (status == 0) {
    card_queue_begin(side);
    card_queue_d9(side);
    return 0;
  }
  return status;
}

uchar card_unlock_start(int side)
{
  uchar status = (&card_status)[side * 8 + 2];
  if (status == 0) {
    card_queue_begin(side);
    card_queue_da(side);
    return 0;
  }
  return status;
}

bool card_unit_ok(int side)
{
  return (&card_units.unit0_flags)[side * 0x13] == 0;
}

bool card_header_check(int side)
{
  uint word0;
  uint word3;
  word0 = card_records[side].card_word0;
  word3 = card_records[side].card_word3;
  if ((word0 & 0xf0000000) != 0x10000000) return 0;
  return (word3 & 0xffff0000) == 0x046c0000;
}

void card_colors_to_packed(card_body_rec *body)
{
  uint color;
  uint packed;
  int i;
  int hi_bit;

  color = body->color_1p.w;
  packed = 0;
  for (i = 0; i < 4; i++) {
    {
      uint u = (color >> (i * 5)) & 0x10;
      int z = u == 0;
      hi_bit = z == 0;
    }
    switch (i) {
    case 0: body->curse.f.color_1p_hi0 = hi_bit; break;
    case 1: body->curse.f.color_1p_hi1 = hi_bit; break;
    case 2: body->curse.f.color_1p_hi2 = hi_bit; break;
    case 3: body->curse.f.color_1p_hi3 = hi_bit; break;
    }
    packed |= ((color >> (i * 5)) & 0xf) << (i * 4);
  }
  body->color_1p_packed.h = packed;
  color = body->color_2p.w;
  packed = 0;
  for (i = 0; i < 4; i++) {
    {
      uint u = (color >> (i * 5)) & 0x10;
      int z = u == 0;
      hi_bit = z == 0;
    }
    switch (i) {
    case 0: body->misc.f.color_2p_hi0 = hi_bit; break;
    case 1: body->misc.f.color_2p_hi1 = hi_bit; break;
    case 2: body->misc.f.color_2p_hi2 = hi_bit; break;
    case 3: body->misc.f.color_2p_hi3 = hi_bit; break;
    }
    packed |= ((color >> (i * 5)) & 0xf) << (i * 4);
  }
  body->color_2p_packed.h = packed;
}

void card_format2_area_clear(int side_in)
{
  char side = side_in;
  char *dst;
  char *dst_copy;
  int n;
  dst = (char *)&card_records[side].fmt2_header;
  dst_copy = (char *)&card_records[side].fmt2_header2;
  n = 28;
  while (n--) {
    *dst++ = 0;
    *dst_copy++ = 0;
  }
  dst = (char *)&card_records[side].fmt2_body;
  n = 58;
  while (n--) {
    *dst++ = 0;
  }
}
