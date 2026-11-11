/*
 * vf4lib/libbase/src/card_log.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern card_data_log_rec *card_data_log_cur; /* 0x0c41eb30 */
extern side_costume_sides side_costume; /* 0x0c2926e8 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */

void card_log_session_start(void)
{
  user_back_data.card_log_stamp = date_stamp_now();
  user_back_mark_dirty();
  return;
}

int card_log_stamp_age(uint stamp)
{
  uint a;
  char ya, yb;
  uint ma, da, ha;
  uchar mb;
  uint db, hb;
  int ta;
  int r;

  a = date_stamp_now();
  ya = (a >> 20 & 0x7f) - 98;
  ma = a >> 16 & 0xf;
  da = a >> 11 & 0x1f;
  ha = a >> 6 & 0x1f;
  ta = a & 0x3f;
  yb = (stamp >> 20 & 0x7f) - 98;
  mb = stamp >> 16 & 0xf;
  db = stamp >> 11 & 0x1f;
  hb = stamp >> 6 & 0x1f;
  stamp = stamp & 0x3f;
  ta += ha * 60 + da * 1440;
  stamp += hb * 60 + db * 1440;
  ta += month_day_offsets[ma] * 1440;
  if ((ya & 3) == 0) ta += 1440;
  stamp += month_day_offsets[mb] * 1440;
  if ((yb & 3) == 0) stamp += 1440;
  if (ya > yb) {
    ta += (yb & 3) ? 0x80520 : 0x80ac0;
  }
  ta -= stamp;
  if (ta <= 0x10df) {
    r = 0;
  }
  else {
    r = ta > 0x275f;
    r += 1;
  }
  return r;
}

void card_item_log_add(char side, char item)
{
  card_item_log_rec *p;
  card_item_log_rec *oldest;
  card_item_log_rec *found;
  card_record_rec *r;
  uint key;
  int n;

  r = card_record_get(side);
  key = user_back_data.card_log_stamp;
  oldest = user_back_data.card_item_log;
  for (p = oldest, found = 0, n = 32; n != 0; n--, p++) {
    if (p->stamp == 0) {
      found = p;
      break;
    }
    if (p->stamp < oldest->stamp) oldest = p;
  }
  if (found == 0) found = oldest;
  found->card_id = r->access_key;
  found->chara = card_record_char_get((uchar)side);
  found->stamp = key;
  found->side = side;
  found->item = item;
  found->state = 1;
  found->slot = item_slot_of(found->chara,((int *)&side_costume)[side] ^ chara_costume_swap[found->chara],item);
  user_back_mark_dirty();
}

void card_item_log_reassign(char side, char item)
{
  card_record_rec *r;
  int stamp;
  card_item_log_rec *p;
  int i;
  
  r = card_record_get(side ^ 1);
  if (r->in_use != 0) {
    stamp = user_back_data.card_log_stamp;
    p = user_back_data.card_item_log;
    for (i = 0x20; i != 0; i--, p++) {
      if (p->stamp == stamp && p->item == item) {
        p->side ^= 1;
        p->chara = card_record_char_get((uchar)(side ^ 1));
        p->card_id = r->access_key;
      }
    }
    user_back_mark_dirty();
  }
}

void card_item_log_drop_player(char side)
{
  int id;
  card_item_log_rec *e;
  int n;
  id = user_back_data.card_log_stamp;
  e = user_back_data.card_item_log;
  if (id != 0) {
    for (n = 32; n != 0; n--, e++) {
      if (e->stamp == id) {
        switch (e->state) {
        case 4:
          if (card_log_stamp_age(e->stamp) != 2) break;
        case 1:
        case 2:
          if (e->side == side) {
            e->card_id = 0;
            e->stamp = 0;
            e->chara = 0;
            e->side = 0;
            e->item = 0;
            e->slot = -1;
            e->state = 0;
          }
          break;
        case 3:
          if (card_log_stamp_age(e->stamp) == 1) e->state = 4;
          break;
        }
      }
    }
    user_back_mark_dirty();
  }
}

void card_logs_settle(void)
{
  card_item_log_rec *p;
  card_data_log_rec *q;
  int i;

  p = user_back_data.card_item_log;
  q = user_back_data.card_data_log;
  for (i = 0x20; i != 0; i--, p++) {
    switch (p->state) {
    case 1:
      p->state = 3;
      break;
    case 2:
      p->card_id = 0;
      p->stamp = 0;
      p->chara = 0;
      p->side = 0;
      p->item = 0;
      p->slot = 0xff;
      p->state = 0;
      break;
    }
  }
  for (i = 0x20; i != 0; i--, q++) {
    if (q->state == 1) {
      q->card_id = 0;
      q->stamp = 0;
      q->chara = 0;
      q->state = 0;
      card_snapshot_clear(&q->snapshots[0]);
      card_snapshot_clear(&q->snapshots[1]);
    }
  }
  user_back_data.card_log_stamp = 0;
  user_back_mark_dirty();
}

void card_item_log_lottery(void)
{
  char chara;
  int result;
  uint rnd;
  uint stamp_bits;
  char n;
  card_item_log_rec *e;
  uint side;
  card_record_rec *r;
  
  e = user_back_data.card_item_log;
  side = 0;
  do {
    r = card_record_get(side);
    if (r->in_use != 0) {
      card_image_get(side);
      n = ' ';
      do {
        if ((e->stamp != 0) && (e->state == '\x04')) {
          result = item_has_carded(side,(int)e->item);
          if ((result == 0) &&
             ((chara = e->chara, result = card_image_char_get(side & 0xff), chara == result ||
              (e->item < '\x02')))) {
            rnd = lcg_next15();
            stamp_bits = date_stamp_now() & e->stamp & 0xfff;
            if ((rnd & 0xfff) == stamp_bits) {
              item_give_carded(side,(int)e->item);
              e->card_id = 0;
              e->stamp = 0;
              e->chara = 0;
              e->side = 0;
              e->item = 0;
              e->slot = 0xff;
              e->state = 0;
              break;
            }
          }
        }
        n = n + -1;
        e++;
      } while (n != '\0');
    }
    side = (uint)(char)((char)side + '\x01');
    if (1 < (int)side) {
      user_back_mark_dirty();
      return;
    }
  } while( true );
}

void card_item_log_restore(side) char side;
{
  card_item_log_rec *e;
  int id;
  char chara;
  char n;
  int item;
  char *q;
  int slot;
  card_record_rec *p;
  
  e = user_back_data.card_item_log;
  p = card_record_get(side);
  chara = card_record_char_get((uchar)side);
  id = p->access_key;
  for (n = 32; n != 0; n--, e++) {
    if (e->stamp != 0 && e->card_id == id && e->chara == chara && e->state == 3) {
      item = e->item;
      q = &e->slot;
      e->stamp = 0;
      e->card_id = 0;
      e->state = 0;
      e->chara = 0;
      e->side = 0;
      e->item = 0;
      *q = -1;
      if (item_has_unchecked(side,item) == 0) {
        item_give_both(side,item);
        slot = (char)item_slot_of(chara,0,item);
        if (slot != -1 && item_slot_lock_test(side,slot) == 0) {
          item_equip_unchecked(side,0,slot,item);
        }
        slot = (char)item_slot_of(chara,1,item);
        if (slot != -1 && item_slot_lock_test(side,slot) == 0) {
          item_equip_unchecked(side,1,slot,item);
        }
      }
    }
  }
  user_back_mark_dirty();
}

void card_item_log_state_update(void)
{
  card_item_log_rec *p;
  int key;
  int changed;
  char n;
  uint u;

  p = user_back_data.card_item_log;
  key = user_back_data.card_log_stamp;
  changed = 0;
  for (n = 32; n != 0; n--, p++) {
    if (p->stamp == key) {
      u = card_flags_get((int)p->side);
      switch (p->state) {
      case 1:
        if ((u & 1) == 0) {
          p->state = 2;
          changed = 1;
        }
        break;
      case 2:
        if ((u & 1) != 0) {
          p->state = 1;
          changed = 1;
        }
        break;
      }
    }
  }
  if (changed) {
    user_back_mark_dirty();
  }
}

void card_snapshot_clear(card_data_log_snap *snap)
{
  snap->level = 0;
  snap->all_win = 0;
  snap->all_lose = 0;
  snap->rank_pnt = 0;
  snap->tenpo_pnt = 0;
  snap->exp = 0;
  snap->point = 0;
  snap->color_1p = 0;
  snap->color_2p = 0;
  return;
}

void card_data_log_pick(void)
{
  card_data_log_rec *p;
  card_data_log_rec *best;
  int n;
  card_data_log_cur = 0;
  best = user_back_data.card_data_log;
  p = user_back_data.card_data_log;
  for (n = 0x20; n != 0; n--) {
    if (p->stamp == 0) {
      card_data_log_cur = p;
      break;
    }
    if (p->stamp < best->stamp) best = p;
    p++;
  }
  if (card_data_log_cur == 0) card_data_log_cur = best;
  card_snapshot_clear(&card_data_log_cur->snapshots[0]);
  card_snapshot_clear(&card_data_log_cur->snapshots[1]);
}

void card_data_log_snapshot(side, which) char side; char which;
{
  card_data_log_rec *rec;
  card_record_rec *r;
  uint stamp;
  card_data_log_snap *snap;
  
  if (card_data_log_cur != 0) {
    rec = card_data_log_cur;
    r = card_record_get(side);
    stamp = user_back_data.card_log_stamp;
    snap = which == 0 ? &rec->snapshots[0] : &rec->snapshots[1];
    rec->stamp = stamp;
    rec->state = 1;
    rec->card_id = r->access_key;
    rec->chara = card_record_char_get((uchar)side);
    snap->level = r->header.level;
    snap->all_win = r->header.all_win;
    snap->all_lose = r->header.all_lose;
    snap->rank_pnt = r->body.rank_pnt;
    snap->tenpo_pnt = r->body.tenpo_pnt;
    snap->exp = r->body.exp;
    snap->point = r->body.point;
    snap->color_1p = r->body.color_1p.w;
    snap->color_2p = r->body.color_2p.w;
  }
}

void card_data_log_restore(side) char side;
{
  card_record_rec *pl;
  card_image_rec *pl2;
  card_header_rec *p;
  card_body_rec *q;
  char c;
  int id;
  card_data_log_rec *e;
  card_data_log_snap *t1;
  card_data_log_snap *t2;
  char n;

  e = user_back_data.card_data_log;
  pl = card_record_get(side);
  pl2 = card_image_get(side);
  c = card_record_char_get((uchar)side);
  id = pl->access_key;
  p = &pl->header;
  q = &pl->body;
  n = 32;
  do {
    if (e->stamp != 0 && e->card_id == id && e->chara == c && e->state == 3) {
      t1 = &e->snapshots[0];
      t2 = &e->snapshots[1];
      e->stamp = 0;
      e->card_id = 0;
      e->state = 0;
      e->chara = 0;
      if (t2->level == p->level && t2->all_win == p->all_win &&
          t2->all_lose == p->all_lose && t2->rank_pnt == q->rank_pnt && t2->tenpo_pnt == q->tenpo_pnt &&
          t2->exp == q->exp && t2->point == q->point &&
          t2->color_1p == q->color_1p.w && t2->color_2p == q->color_2p.w) {
        p->level = t1->level;
        p->all_win = t1->all_win;
        p->all_lose = t1->all_lose;
        q->rank_pnt = t1->rank_pnt;
        q->tenpo_pnt = t1->tenpo_pnt;
        q->exp = t1->exp;
        q->point = t1->point;
        q->color_1p.w = t1->color_1p;
        q->color_2p.w = t1->color_2p;
        pl2->header.level = t1->level;
        pl2->header.all_win = t1->all_win;
        pl2->header.all_lose = t1->all_lose;
        pl2->body.rank_pnt = t1->rank_pnt;
        pl2->body.tenpo_pnt = t1->tenpo_pnt;
        pl2->body.exp = t1->exp;
        pl2->body.point = t1->point;
        pl2->body.color_1p.w = t1->color_1p;
        pl2->body.color_2p.w = t1->color_2p;
      }
      card_snapshot_clear(t1);
      card_snapshot_clear(t2);
    }
    n--;
    e++;
  } while (n != 0);
  user_back_mark_dirty();
}

void card_logs_session_clear(void)
{
  int k;
  card_item_log_rec *a;
  card_data_log_rec *b;
  int n;
  k = user_back_data.card_log_stamp;
  a = user_back_data.card_item_log;
  b = user_back_data.card_data_log;
  for (n = 0x20; n != 0; n--) {
    if (a->stamp == k) {
      a->card_id = 0;
      a->stamp = 0;
      a->chara = 0;
      a->side = 0;
      a->item = 0;
      a->slot = 0;
      a->state = 0;
    }
    if (b->stamp == k) {
      b->card_id = 0;
      b->stamp = 0;
      b->chara = 0;
      b->state = 0;
      card_snapshot_clear(&b->snapshots[0]);
      card_snapshot_clear(&b->snapshots[1]);
    }
    a++;
    b++;
  }
  user_back_data.card_log_stamp = 0;
  user_back_mark_dirty();
}
