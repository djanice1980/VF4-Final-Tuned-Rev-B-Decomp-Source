/*
 * vf4lib/librobdisp/src/rob_color.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern texcol_src texcol_scratch_a[16]; /* 0x0c4920b0 */
extern char rob_texcol_buffers[15360]; /* 0x0c4922f0 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern texcol_pair_entry texcol_scratch_c; /* 0x0c492270 */

void texcol_scratch_reset(void)
{
  texcol_scratch_a[0].id = -1;
  texcol_scratch_b[0].id = -1;
  texcol_scratch_c.field_0x4 = 0xffffffff;
  texcol_scratch_c.id = 0xffffffff;
  return;
}

void rob_texcol_buffers_clear(int side)
{
  int i = 0;
  char *p = rob_texcol_buffers;
  for (; i < 5; i++) {
    int q = side * 0x1e00;
    int o = i * 0x600;
    *(short *)(p + (q + o)) = -2;
  }
  texcol_scratch_reset();
}

void rob_color_build_custom(int side, int chara, int costume)
{
  rob_color_build(side,chara,(int)(char)costume,1);
  return;
}

void rob_color_build_plain(int chara, int costume)
{
  rob_color_build(0,chara,(int)(char)costume,0);
  return;
}

void rob_texcol_part_select(int side, int part)
{
  char *p;
  txn_list_current_clear();
  p = part_buffer_index;
  txn_list_current_set((txn_entry *)(rob_texcol_buffers + side * 0x1e00 + p[part] * 0x600));
}

void rob_color_build(int side, int chara, char costume, char with_card)
{
  typedef struct { void *a; void *b; void *c; } bufs;
  bufs bu;
  bufs *b;
  int i;
  int hair;
  int eye;
  int color_word;
  int count;
  card_image_rec *image;
  int card_on;
  int entry_flag;

  hair = 0;
  eye = 0;
  color_word = 0;
  count = 0;
  b = &bu;
  rob_texcol_buffers_clear(side);
  if ((rob_color_flags & 1) == 0) {
    txn_list_current_set(0);
    return;
  }
  for (i = 0; i < 5; i++) {
    texcol_scratch_reset();
    b->a = texcol_scratch_a;
    b->b = texcol_scratch_b;
    b->c = &texcol_scratch_c;
    costume_color_lists_copy(chara,costume,i,(void **)b);
    b->a = texcol_scratch_a;
    b->b = texcol_scratch_b;
    b->c = &texcol_scratch_c;
    count += texcol_lists_combine((void **)b,(short *)(rob_texcol_buffers + side * 0x1e00 + i * 0x600));
  }
  if (with_card != 0) {
    if ((rob_color_flags & 2) != 0) {
      card_on = 1;
    }
    else {
      card_on = (uchar)card_record_get(side)->in_use;
    }
    {
      byte *t = &side_entry_flags[0];
      entry_flag = t[side] >> 2 & 1;
    }
    if ((entry_flag != 0) & (card_on != 0)) {
      image = card_image_get(side);
      if (chara_costume_swap[chara] != costume) {
        if ((rob_color_flags & 2) != 0) {
          color_word = (&color_2p_a)[0] + (&color_2p_a)[1] * 0x20 + ((&color_2p_a)[2] << 10) + ((&color_2p_a)[3] << 15);
          hair = hair_2p;
          eye = eye_2p;
        }
        else {
          color_word = image->body.color_2p.w;
          hair = costume_hair_color_get(side,1);
          eye = costume_eye_color_get(side,1);
        }
      }
      else {
        if ((rob_color_flags & 2) != 0) {
          color_word = (&color_1p_a)[0] + (&color_1p_a)[1] * 0x20;
          color_word = color_word + ((&color_1p_a)[2] << 10) + ((&color_1p_a)[3] << 15);
          hair = hair_1p;
          eye = eye_1p;
        }
        else {
          color_word = image->body.color_1p.w;
          hair = costume_hair_color_get(side,0);
          eye = costume_eye_color_get(side,0);
        }
      }
    }
    count += rob_custom_colors_apply(side,chara,costume,color_word);
    count += rob_item_colors_apply(side,chara,costume,color_word);
    count += rob_hair_colors_apply(side,chara,costume,hair);
    count += rob_hair_item_pair_colors_apply(side,chara,costume,hair);
    if (count + rob_eye_colors_apply(side,chara,costume,eye) > 0x5f) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/librobdisp/src/rob_color.c",0x121,
               "COLOR_BUFFER 4.1 OVERFLOW.");
    }
  }
  txn_list_current_clear();
  txn_list_current_set((txn_entry *)(rob_texcol_buffers + side * 0x1e00));
}

int texcol_list_merge(short *src, short *list)
{
  short s;
  int added;
  short *dst;
  
  added = 0;
  if ((src == (short *)0x0) | (list == (short *)0x0)) return 0;
  while (*src != -2) {
    dst = list;
    if (*list != -2) {
      do {
        if (*dst == *src) break;
        dst = dst + 8;
      } while (*dst != -2);
    }
    s = *dst;
    if (s == -2) {
      short *d = dst;
      dst += 8;
      memcpy(d,src,0x10);
      *dst = s;
      added = added + 1;
    }
    else {
      memcpy(dst,src,0x10);
    }
    src = src + 8;
  }
  return added;
}

short * color_table_list_get(int chara, int costume_arg, int column, int row)
{
  chara_color_table_rec *rec;
  short **table;
  char costume = (char)costume_arg;
  
  rec = &chara_color_tables[chara][costume];
  table = (short **)rec->table;
  if (table == 0) return 0;
  if (row >= rec->rows) return 0;
  return table[row * 8 + column];
}

short * custom_color_list_get(int chara, char costume, int part, int slot, int color)
{
  int **part_tables;
  int *table;
  char *o;
  
  o = (char *)(chara * 24 + costume * 12);
  part_tables = ((chara_color_table_rec *)(o + (int)((char *)&chara_color_tables[0][0].custom - 8)))->custom;
  if (part_tables == 0) return 0;
  if (part > 4) return 0;
  table = part_tables[part];
  if (table == 0) return 0;
  if (color > 0x12) return 0;
  return (short *)table[color * 4 + slot];
}

int rob_custom_colors_apply(int side, int chara, char costume, uint color_word)
{
  int slot;
  int part;
  int count = 0;
  uchar color;
  
  for (slot = 0; slot < 4; slot++) {
    color = (color_word >> slot * 5) & 0x1f;
    if ((uchar)(color - 1) < 0x13) {
      color--;
      for (part = 0; part < 5; part++) {
        short *list = custom_color_list_get(chara,costume,part,slot,color);
        short *buffer = (short *)(rob_texcol_buffers + side * 0x1e00 + part * 0x600);
        count += texcol_list_merge(list,buffer);
      }
    }
  }
  return count;
}

void item_color_override(rob_item_slot *slot, int chara, uint color_word, char *column, char *row)
{
  byte color;
  
  if (((chara == 1) && ((char)slot->card_item == -0x5d)) &&
     (color = ((byte)color_word & 0x1f) - 1, color < 0x13)) {
    *column = 2;
    *row = color;
  }
  return;
}

int rob_item_colors_apply(uint side, int chara, char costume, uint color_word)
{
  int i;
  int color;
  rob_item_slot *slot;
  char column;
  char row;
  int count;
  short *base;
  
  count = 0;
  slot = (rob_item_slot *)(rob_get(side & 0xff))->items;
  for (i = 0; i < 4; i++, slot++) {
    if ((slot->flags & 2) != 0) {
      switch (slot->card_kind) {
      case 0:
        {
        char *rec = (char *)slot->card_data;
        column = rec[1];
        row = rec[2];
        color = rec[3];
        }
        break;
      case 1:
        {
        char *rec = (char *)slot->card_data;
        column = rec[1];
        row = rec[2];
        color = rec[3];
        }
        break;
      default:
        column = -1;
        row = -1;
        color = -1;
        break;
      }
      item_color_override(slot,chara,color_word,&column,&row);
      if ((column != -1) && (row != -1)) {
        short *list = color_table_list_get(chara,costume,column,row);
        base = (short *)(rob_texcol_buffers + side * 0x1e00);
        count += texcol_list_merge(list,base);
      }
      if (color != -1) {
        char *columns = part_color_column;
        short *list = color_table_list_get(chara,costume,columns[i],color);
        char *buffers = part_buffer_index;
        base = (short *)(rob_texcol_buffers + side * 0x1e00 + buffers[i] * 0x600);
        count += texcol_list_merge(list,base);
      }
    }
  }
  return count;
}

int rob_hair_colors_apply(uint side, int chara, int costume_arg, uint hair_arg)
{
  byte key;
  rob_item_slot *slot;
  hair_color_row *row;
  hair_face_color_row *face_row;
  int costume;
  uint hair;
  int count;
  short *base;
  short *list;
  
  costume = (int)(char)costume_arg;
  hair = hair_arg & 0xff;
  count = 0;
  slot = (rob_item_slot *)(rob_get(side & 0xff))->items;
  if ((byte)(hair - 1) > 1) return 0;
  row = hair_color_tables[chara][costume][hair - 1];
  if (row == 0) return 0;
  if ((slot->flags & 1) != 0) {
    key = 0;
  }
  else if ((slot->flags & 2) != 0) {
    key = slot->card_item;
  }
  else {
    return 0;
  }
  for (; row->item != 0xff; row++) {
    if (row->item == key) break;
  }
  if (row->item == 0xff) return 0;
  if ((row->column != -1) && (row->row != -1)) {
    list = color_table_list_get(chara,costume,row->column,row->row);
    base = (short *)(rob_texcol_buffers + side * 0x1e00);
    count += texcol_list_merge(list,base);
  }
  if (row->buffer1_row != -1) {
    count += texcol_list_merge(color_table_list_get(chara,costume,4,row->buffer1_row),(short *)(&rob_texcol_buffers[0x600] + side * 0x1e00));
  }
  if ((row->column2 != -1) && (row->row2 != -1)) {
    list = color_table_list_get(chara,costume,row->column2,row->row2);
    base = (short *)(rob_texcol_buffers + side * 0x1e00);
    count += texcol_list_merge(list,base);
  }
  slot = (rob_item_slot *)(rob_get(side & 0xff))->items;
  slot = slot + 1;
  if ((slot->flags & 2) != 0) {
    face_row = hair_face_item_color_tables[chara][costume][hair - 1];
    if (face_row == 0) return count;
    for (; face_row->item != 0xff; face_row++) {
      if (slot->card_item == face_row->item) break;
    }
    if (face_row->item == 0xff) return count;
    list = color_table_list_get(chara,costume,face_row->column,face_row->row);
    base = (short *)(rob_texcol_buffers + side * 0x1e00);
    count += texcol_list_merge(list,base);
  }
  return count;
}

int rob_hair_item_pair_colors_apply(uint side, int chara, int costume_arg, uint hair_arg)
{
  char costume = costume_arg;
  uchar hair = hair_arg;
  int count = 0;
  rob_item_slot *slot;
  rob_item_slot *head;
  rob_item_slot *face;
  hair_item_pair_color_row *row;
  if (hair > 2) return count;
  face = (rob_item_slot *)(rob_get((uchar)side))->items;
  slot = face;
  head = face;
  face += 1;
  if ((slot->flags & 2) != 0 && (face->flags & 2) != 0) {
    row = hair_item_pair_color_tables[chara][costume];
    if (row == 0) return count;
    while (row->head_item != 0xff) {
      if (row->head_item == head->card_item && row->face_item == face->card_item) break;
      row++;
    }
    if (row->head_item == 0xff) return count;
    count += texcol_list_merge(color_table_list_get(chara, costume, row->pair[hair][0], row->pair[hair][1]), (short *)(rob_texcol_buffers + side * 0x1e00));
  }
  return count;
}

int rob_eye_colors_apply(int side_arg, int chara_arg, int costume_arg, uint eye_arg)
{
  int side = side_arg;
  int chara = chara_arg;
  char costume = costume_arg;
  unsigned char eye = eye_arg;
  char *pair;
  if ((unsigned char)(eye - 1) > 2) return 0;
  pair = eye_color_tables[chara][costume];
  if (pair == 0) return 0;
  pair += eye * 2 - 2;
  return texcol_list_merge(color_table_list_get(chara, costume, pair[0], pair[1]), (short *)(rob_texcol_buffers + side * 0x1e00));
}

void costume_color_lists_copy(int chara, int costume_arg, uint part, void **lists)
{
  typedef struct { int w[4]; } s16;
  typedef struct { int w[3]; } s12;
  typedef struct { int w[2]; } s8;
  s16 *d1, *s1;
  s12 *d2, *s2;
  s8 *d3, *s3;
  int **q;
  int ***part_lists;
  char costume;
  int i, n1, n2, n3;
  
  costume = costume_arg;
  d1 = (s16 *)lists[0];
  d2 = (s12 *)lists[1];
  d3 = (s8 *)lists[2];
  q = (int **)0x0;
  part_lists = costume_color_lists[chara][costume];
  if (((part_lists != (int ***)0x0) & ((int)part >= 0)) && ((int)part < 5)) {
    q = part_lists[part];
  }
  if (q != (int **)0x0) {
    n3 = 0;
    n2 = 0;
    n1 = 0;
    for (i = 0; i < 4; i++, q += 3) {
      if (q[0] != 0) {
        for (s1 = (s16 *)q[0]; s1->w[0] != -1; s1++, d1++, n1++) *d1 = *s1;
      }
      if (q[1] != 0) {
        for (s2 = (s12 *)q[1]; s2->w[0] != -1; s2++, d2++, n2++) *d2 = *s2;
      }
      if (q[2] != 0) {
        for (s3 = (s8 *)q[2]; s3->w[0] != -1; s3++, d3++, n3++) *d3 = *s3;
      }
    }
    if ((0xf < n1 || 0xf < n2) || (0xf < n3)) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/librobdisp/src/rob_color.c",0x2df,
               "COLOR_BUFFER 4.0 overflow.");
    }
  }
  d1->w[0] = -1;
  d2->w[0] = -1;
  d3->w[1] = -1;
  d3->w[0] = -1;
  lists[0] = d1;
  lists[1] = d2;
  lists[2] = d3;
  return;
}

int texcol_lists_combine(void **lists, short *dst)
{
  typedef struct { short id; uchar a; uchar b; short c; short m; uchar x[4]; uchar y[4]; } Out;
  texcol_src *s;
  texcol_xy_entry *tab;
  texcol_xy_entry *t;
  Out *o;
  Out *p;
  Out *base;
  int n;
  int k;

  n = 0;
  s = (texcol_src *)lists[0];
  tab = (texcol_xy_entry *)lists[1];
  base = (Out *)dst;
  o = (Out *)dst;
  while (s->id != -1) {
    o->id = s->id;
    o->a = (int)s->a;
    o->b = (int)s->b;
    {
      ushort m = 180;
      o->c = (int)(s->c * m) >> 15;
    }
    for (t = tab; t->id != -1; t++) {
      if (t->id == o->id) break;
    }
    if (t->id != -1) {
      for (k = 0; k < 4; k++) {
        o->x[k] = t->x[k];
        o->y[k] = t->y[k];
      }
    }
    else {
      for (k = 0; k < 4; k++) {
        o->x[k] = o->y[k] = 0;
      }
    }
    o->m = -1;
    o++;
    n++;
    s++;
  }
  o->id = -2;
  for (t = tab; t->id != -1; t++) {
    for (p = base; p->id != -2; p++) {
      if (t->id == p->id) break;
    }
    if (p->id == -2) {
      o->id = t->id;
      o->a = o->b = 0;
      o->c = 0;
      o->m = -1;
      for (k = 0; k < 4; k++) {
        o->x[k] = t->x[k];
        o->y[k] = t->y[k];
      }
      o++;
      n++;
      o->id = -2;
    }
  }
  if (n > 0x5f) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/librobdisp/src/rob_color.c",0x32f,
             "COLOR_BUFFER 4.1 OVERFLOW.");
  }
  return n;
}
