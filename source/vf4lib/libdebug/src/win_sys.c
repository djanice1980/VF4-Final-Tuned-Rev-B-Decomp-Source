/*
 * vf4lib/libdebug/src/win_sys.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint win_bg_colors[20]; /* 0x0c1e2a60 */
extern uint win_text_color_restore; /* 0x0c3227c8 */
extern uint win_text_color; /* 0x0c3227d0 */
extern int win_cell_w; /* 0x0c3227d8 */
extern int win_cell_h; /* 0x0c3227dc */
extern int win_input_on; /* 0x0c3227e8 */
extern int win_active; /* 0x0c3227ec */
extern uint win_bg_argb; /* 0x0c3227f0 */
extern float win_bg_z; /* 0x0c3227f4 */
extern int fb_text_cursor[2]; /* 0x0c3227fc */
extern char win_screen_text[80][128]; /* 0x0c3d22c8 */
extern win_border_glyph_set win_border_glyphs; /* 0x0c1e2b38 */

static inline void win_value_init(win_value *v, int type)
{
  v->type = type;
  switch (type) {
  case 0: v->u.i = 0; break;
  case 1: v->u.f = 0.0f; break;
  case 2: v->u.i = 0; break;
  case 3: v->u.i = 0; break;
  }
}

static inline void dbg_value_init(win_value *v, int type)
{
  v->type = type;
  switch (type) {
  case 1:
    v->u.f = 0.0f;
    break;
  case 0:
  case 2:
  case 3:
    v->u.i = 0;
    break;
  }
}

static inline size_t win_max(size_t a, size_t b)
{
  if (a < b) a = b;
  return a;
}

int win_addr_check(void *addr)
{
  if ((uint)addr + 0xf4000000 > 0x1ffffff) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libdebug/src/win_sys.c",0x141,"addr <%08x> out of range",
             addr);
    return 0;
  }
  return 1;
}

code * win_ref_address(win_ref *r)
{
  code *addr;

  addr = (code *)r->word;
  if ((r->flags & 1) != 0) {
    win_addr_check(addr);
    addr = (code *)(*addr)(r->arg);
  }
  return addr;
}

code * win_ref_resolve(win_ref *r)
{
  code *addr;

  addr = (code *)r->word;
  if ((r->flags & 1) != 0) {
    win_addr_check(addr);
    addr = (code *)(*addr)(r->arg);
  }
  if ((r->flags & 2) == 0) {
    win_addr_check(addr);
    addr = *(code **)addr;
  }
  return addr;
}

void win_sys_init(void)
{
  win_rec **p;
  win_rec *e;
  uint n;
  uint i;
  p = win_list;
  for (n = 0; *p != 0; p++) n++;
  win_count = n;
  for (i = 0; i < win_count; i++) {
    e = win_list[i];
    if (e->tmpl != 0) {
      e->items = e->tmpl->items;
      e->count = e->tmpl->count;
    }
    e->open = 0;
    e->focus = 0;
    e->z = 0;
  }
  {
  win_rec *f = win_list[0];
  f->open = 1;
  f->z = 1;
  {
  uint m; f->cursor = 2; m = 2; if (m >= f->count) f->cursor = f->count - 1;
  }
  }
}

void win_sys_update(void)
{
  int start;
  player_input_rec *input;
  int elapsed;

  start = win_timer_start();
  win_enable_check();
  if (win_active != 0) {
    win_update_all();
  }
  if (win_input_on != 0) {
    input = player_input_get(0);
    win_pad_keys_run(input);
    input = player_input_get(1);
    win_pad_keys_run(input);
    win_focused = win_focused_get();
    win_open_windows = win_open_count();
    win_top = win_top_get();
  }
  elapsed = win_timer_elapsed(start);
  win_update_usec = (float)elapsed;
  return;
}

void win_sys_draw(void)
{
  int start;
  int n;
  int cnt;
  int i;
  int j;
  win_rec *p;

  start = win_timer_start();
  if (win_active != 0) {
    n = win_open_count();
    if (n != 0) {
      cnt = win_count;
      memset(&win_screen_text[0][0],0x20,0x2800);
      for (i = 1; i <= n; i++) {
        p = win_list[0];
        for (j = 0; j < cnt; j++, p++) {
          if (p->z == i) {
            win_draw(p);
            if (p->draw_fn != 0) {
              (*p->draw_fn)(p,p->draw_arg);
            }
            break;
          }
        }
      }
      win_text_flush(&win_screen_text[0][0]);
    }
  }
  win_draw_usec = (float)win_timer_elapsed(start);
  win_draw_frames = win_draw_frames + 1;
}

void win_update_one(win_rec *w)
{
  int base;

  if (w->open) {
    base = 0;
    if (w->value_ref.word != 0) {
      base = (int)win_ref_resolve(&w->value_ref);
    }
    w->value_base = base;
    if (w->focus && (w->focus_fn != 0)) {
      (*w->focus_fn)(w,w->focus_arg);
    }
    if (w->update_fn != 0) {
      (*w->update_fn)(w,w->update_arg);
    }
  }
  return;
}

void win_update_all(void)
{
  uint i;

  for (i = 0; i < win_count; i = i + 1) {
      win_update_one(win_list[i]);
  }
  return;
}

void win_pad_keys_run(player_input_rec *input)
{
  win_rec *r;
  r = win_focused_get();
  if (r != 0) {
    win_key_table_run(r,input,win_keys_focused,0x12);
  }
  else {
    win_key_table_run(r,input,win_keys_idle,2);
  }
}

bool win_pad_condition_test(player_input_rec *input, uint cond)
{
  uint hi = cond & 0xff000000;
  uint lo = cond & 0xffffff;
  uint v;

  if (lo == 0) return 1;
  switch (hi & 0x7000000) {
  case 0x1000000:
    v = input->pressed;
    break;
  case 0x2000000:
    v = input->released;
    break;
  case 0x3000000:
    v = input->now;
    break;
  case 0x4000000:
    v = input->long_held;
    break;
  case 0x5000000:
    v = input->repressed;
    break;
  case 0:
  default:
    v = input->repeated;
    break;
  }
  if ((int)hi < 0) v = ~v;
  return (v & lo) == lo;
}

int win_key_table_run(win_rec *w, player_input_rec *input, uint (*keys)[3], uint count)
{
  int ok1;
  int ok2;
  uint *key;
  uint i;

  for (i = 0; i < count; i = i + 1) {
      key = keys[i];
      ok1 = win_pad_condition_test(input,key[0]);
      ok2 = win_pad_condition_test(input,key[1]);
      if (ok1 != 0 && ok2 != 0) {
        (*(void (*)())key[2])(w);
        return 1;
      }
  }
  return 0;
}

void win_unfocus(win_rec *w)
{
  if (w != 0) {
    w->focus = 0;
  }
  return;
}

void win_item_select(win_rec *w)
{
  win_item *it;

  if ((w != 0) && (w->count != 0)) {
    it = w->items[w->cursor];
    switch (it->kind) {
    case 1:
      if (it->act.word != 0) {
        win_open(w,(win_rec *)it->act.word);
      }
      break;
    case 2:
      win_ref_resolve(&it->act);
      break;
    }
  }
}

void win_open(win_rec *parent, win_rec *w)
{
  void (*f)(win_rec *, int);
  if ((parent != 0) || (parent = win_top_get(), parent != 0)) {
    if (w->open) {
      parent->z = w->z;
    }
    parent->focus = 0;
  }
  w->open = 1;
  w->focus = 1;
  w->z = win_open_count();
  f = w->open_fn;
  if (f != 0) {
    (*f)(w,parent->open_arg);
  }
  win_update_one(w);
  return;
}

void win_focus_cycle(void)
{
  int open_n;
  uint z;
  uint j;
  uint i;
  win_rec *w;

  open_n = win_open_count();
  for (i = 0; i < win_count; i++) {
    win_list[i]->cycled = 0;
  }
  for (z = 1; (int)z <= open_n; z++) {
    for (j = 0; j < win_count; j++) {
      w = win_list[j];
      if ((w->cycled == 0) && (w->z == z)) {
        if (z == 1) {
          w->focus = 1;
          w->z = (byte)open_n;
          w->cycled = 1;
        }
        else {
          w->focus = 0;
          w->z = (char)z + -1;
          w->cycled = 1;
        }
        break;
      }
    }
  }
}

void win_cursor_up(win_rec *w)
{
  if (w != 0) {
    if (w->cursor != 0) {
      w->cursor = w->cursor - 1;
    }
    if (w->cursor <= w->scroll) {
      if (w->cursor == 0) {
        w->scroll = w->cursor;
      }
      else {
        w->scroll = w->cursor - 1;
      }
    }
  }
  return;
}

void win_cursor_down(win_rec *w)
{
  ushort d;
  ushort h;

  if (w != 0) {
    if (w->cursor + 1 < w->count) {
      w->cursor = w->cursor + 1;
    }
    d = w->cursor - w->scroll;
    h = w->height - 1;
    if (d >= h) {
      if (w->cursor == w->count - 1) {
        w->scroll = w->cursor - h;
      }
      else {
        w->scroll = w->cursor - h + 1;
      }
    }
  }
  return;
}

void win_value_inc(win_rec *w)
{
  win_value_step(w,0,1);
  return;
}

void win_value_inc_fast(win_rec *w)
{
  win_value_step(w,1,1);
  return;
}

void win_value_dec(win_rec *w)
{
  win_value_step(w,0,0xffffffff);
  return;
}

void win_value_dec_fast(win_rec *w)
{
  win_value_step(w,1,0xffffffff);
  return;
}

void win_value_step(win_rec *w,int fast,int dir)
{
  win_item *item;
  uint sel;
  int flag;
  win_limits *lim;
  int t;
  float f;
  float v;
  float hi;
  float lo;
  float s;
  int ihi;
  int ilo;
  uint m;
  int n;
  int x;
  int *g;
  win_value fv;
  win_value iv;
  win_value bv;

  if (w == 0) return;
  item = w->items[w->cursor];
  if (item->kind != 4) return;
  sel = item->mode & 0xf;
  flag = 0;
  switch (sel) {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    dbg_value_init(&fv, 1);
    win_item_values_access(w, item, &fv, 0);
    t = (int)item->limits; /* ORIGINAL SHAPE (corroborated 2026-11-10: the integer branch of this function, cases 7, 8 and 15, holds the limits pointer in t the same way, and there too one win_limits * local does not match): the limits pointer held in the int t and tested there, then copied to a pointer; one win_limits * local loses the register copy */
    if (t != 0) {
      win_limits *fl;
      fl = (win_limits *)t;
      f = fl->step.f;
      win_float_step_last = f;
      if (fast == 1) f = f * win_float_fast_step;
      s = (float)dir;
      v = fv.u.f;
      v = f * s + v;
      t = fl->clamp;
      if (t == 1) {
        hi = fl->hi.f;
        lo = fl->lo.f;
        if (v > hi) v = hi;
        if (v < lo) v = lo;
      }
      fv.u.f = v;
    }
    win_item_values_access(w, item, &fv, 1);
    break;
  case 9:
  case 10:
  case 11:
    flag = 1;
  case 7:
  case 8:
  case 15:
    dbg_value_init(&iv, 0);
    win_item_values_access(w, item, &iv, 0);
    t = (int)item->limits;
    if (t != 0) {
      lim = (win_limits *)t;
      n = dir * lim->step.i;
      if (fast == 1) {
        if (flag == 1) g = (int *)&win_hex_fast_step;
        else g = (int *)&win_int_fast_step;
        n = n * *g;
      }
      x = iv.u.i;
      if (sel == 8) {
        m = item->flags & 0xf;
        if (m == 0) x = (uchar)x;
        else if (m == 1) x = (ushort)x;
      }
      x = x + n;
      ilo = lim->clamp;
      if (ilo == 1) {
        ihi = lim->hi.i;
        ilo = lim->lo.i;
        if (x > ihi) x = ihi;
        if (x < ilo) x = ilo;
      }
      iv.u.i = x;
    }
    win_item_values_access(w, item, &iv, 1);
    break;
  case 14:
    dbg_value_init(&bv, 0);
    win_item_values_access(w, item, &bv, 0);
    bv.u.i ^= 1 << item->bit;
    win_item_values_access(w, item, &bv, 1);
    break;
  }
}

void win_move_up(win_rec *w)
{
  if (w != 0) {
    if (w->y != 0) {
      w->y = w->y - 1;
    }
  }
  return;
}

void win_move_down(win_rec *w)
{
  if (w != 0) {
    if (w->y + w->height + 3 < 0x4f) {
      w->y = w->y + 1;
    }
  }
  return;
}

void win_move_left(win_rec *w)
{
  if (w != 0) {
    if (w->x != 0) {
      w->x = w->x - 1;
    }
  }
  return;
}

void win_move_right(win_rec *w)
{
  if (w != 0) {
    if (w->x + w->width + 3 < 0x7f) {
      w->x = w->x + 1;
    }
  }
  return;
}

void win_height_shrink(win_rec *w)
{
  if (w != 0) {
    if (w->height != 0) {
      w->height = w->height - 1;
    }
  }
  return;
}

void win_height_grow(win_rec *w)
{
  if (w != 0) {
    if (w->height < 0x3c) {
      w->height = w->height + 1;
    }
  }
  return;
}

void win_width_grow(win_rec *w)
{
  if (w != 0) {
    if (w->width < 0x51) {
      w->width = w->width + 1;
    }
  }
  return;
}

void win_width_shrink(win_rec *w)
{
  if (w != 0) {
    if (w->width > w->name_cols) {
      w->width = w->width - 1;
    }
  }
  return;
}

void win_close(win_rec *w)
{
  win_rec *q;

  if (w != 0) {
    w->open = 0;
    w->focus = 0;
    w->z = 0;
    if (w->close_fn != 0) {
      (*w->close_fn)(w, w->close_arg);
    }
    q = win_top_get();
    if (q != 0) {
      q->focus = 1;
    }
  }
}

void win_focus_top(void)
{
  win_rec *p;

  p = win_top_get();
  if (p == 0) {
    p = win_list[0];
    p->z = 1;
  }
  p->focus = 1;
}

void win_main_menu_open(void)
{
  win_open(0,win_list[0]);
}

int win_open_count(void)
{
  uint i;
  int n;

  n = 0;
  i = 0;
  for (; i < win_count; n = n + win_list[i]->open, i = i + 1) {

  }
  return n;
}

win_rec * win_focused_get(void)
{
  win_rec *ret;
  uint i;
  win_rec *p;

  ret = 0;
  for (i = 0; i < win_count; i++) {
    p = win_list[i];
    if (p->open && p->focus) {
      ret = p;
      break;
    }
  }
  return ret;
}

win_rec * win_top_get(void)
{
  win_rec *w;
  uint i;
  uint top_z;
  win_rec *top;

  top = (win_rec *)0x0;
  top_z = 0;
  for (i = 0; i < win_count; i++) {
    w = win_list[i];
    if (w->open && (top_z < w->z)) {
      top_z = w->z;
      top = w;
    }
  }
  return top;
}

void win_draw(win_rec *w)
{
  int row;
  int col;
  int top;
  char *q;

  win_background_draw(w);
  win_contents_draw(w);
  if (w->focus) {
    row = (uint)w->cursor - (uint)w->scroll;
    col = w->x + 1;
    top = w->y + 2;
    if ((w->count != 0) && (row <= (int)(w->height - 1))) {
      q = (char *)(col + (top + row) * 0x80);
      q[(int)&win_screen_text[0][0]] = 4;
    }
    if (w->scroll != 0) {
      q = (char *)(col + top * 0x80);
      q[(int)&win_screen_text[0][0]] = 2;
    }
    if ((int)((uint)w->scroll + (uint)w->height + -1) <
        (int)(w->count - 1)) {
      q = (char *)(col + (top + (uint)w->height) * 0x80);
      q[(int)&win_screen_text[-1][0]] = 1;
    }
  }
  win_frame_draw(w);
  return;
}

void win_contents_draw(win_rec *w)
{
  int row_off;
  win_item *it;
  int row;
  int off;
  uint item_no;
  uint n;
  char title[84];
  int col;
  int top;
  uint count;
  char *line;

  col = w->x + 1;
  top = w->y + 1;
  n = (uint)w->width;
  off = col + top * 0x80;
  memset(&win_screen_text[0][0] + off,0x20,n);
  snprintf(title,0x51,"%s(%d,%d)(%dx%d)",w->title,w->x,
           w->y,w->width,
           w->height);
  str_copy_no_nul(&win_screen_text[0][0] + off,title,n);
  count = (uint)w->count;
  item_no = (uint)w->scroll;
  for (row = 0; row < (int)(uint)w->height; row++) {
    row_off = col + (top + row) * 0x80;
    line = &win_screen_text[1][0] + row_off;
    memset(line,0x20,n);
    if ((int)item_no < (int)count) {
      it = w->items[item_no];
      if (it->names_flags == 4) {
        win_bitmask_item_draw(w,it,line,n);
        return;
      }
      win_row_item = item_no;
      win_item_row_draw(w,it,line + 1,n - 1);
    }
    item_no = item_no + 1;
  }
  return;
}

void win_item_row_draw(win_rec *w, win_item *item, char *text, int width)
{
  char buf[84];
  uint type;
  uint c;
  char *fmt;
  uint n;
  uint len;
  char *s;
  char **names;

  type = item->mode & 0xf;
  c = item->mode & 0x30;
  switch (c) {
  case 0x20: fmt = (char *)win_formats_3[type][1]; break;
  case 0x10: fmt = (char *)win_formats_2[type][1]; break;
  default: fmt = (char *)win_formats_1[type][1]; break;
  }
  str_copy_no_nul(text, item->label, w->name_cols);
  if (item->kind != 3 && item->kind != 4) return;
  n = width + 1;
  if (n > 0x51) n = 0x51;
  len = 0;
  s = 0;
  switch (type) {
  case 0: case 1: case 2: case 3: case 4: case 5: case 6:
    {
      win_value a[3];
      win_value_init(&a[0], 1);
      win_value_init(&a[1], 1);
      win_value_init(&a[2], 1);
      win_item_values_access(w, item, a, 0);
      len = snprintf(buf, n, fmt, a[0].u.f, a[1].u.f, a[2].u.f);
    }
    len = len < n - 1 ? len : n - 1;
    s = buf;
    break;
  case 7: case 9: case 10: case 11:
    {
      win_value b[3];
      win_value_init(&b[0], 0);
      win_value_init(&b[1], 0);
      win_value_init(&b[2], 0);
      win_item_values_access(w, item, b, 0);
      switch (type) {
      case 9:
        len = snprintf(buf, n, fmt, (uchar)b[0].u.i, (uchar)b[1].u.i, (uchar)b[2].u.i);
        break;
      case 10:
        len = snprintf(buf, n, fmt, (ushort)b[0].u.i, (ushort)b[1].u.i, (ushort)b[2].u.i);
        break;
      case 7: case 11:
        len = snprintf(buf, n, fmt, b[0].u.i, b[1].u.i, b[2].u.i);
        break;
      }
    }
    len = len < n - 1 ? len : n - 1;
    s = buf;
    break;
  case 8:
    {
      win_value d[3];
      win_value_init(&d[0], 0);
      win_value_init(&d[1], 0);
      win_value_init(&d[2], 0);
      win_item_values_access(w, item, d, 0);
      switch (item->flags & 0xf) {
      case 0:
        len = snprintf(buf, n, fmt, (uchar)d[0].u.i, (uchar)d[1].u.i, (uchar)d[2].u.i);
        break;
      case 1:
        len = snprintf(buf, n, fmt, (ushort)d[0].u.i, (ushort)d[1].u.i, (ushort)d[2].u.i);
        break;
      case 2:
        len = snprintf(buf, n, fmt, d[0].u.i, d[1].u.i, d[2].u.i);
        break;
      }
    }
    len = len < n - 1 ? len : n - 1;
    s = buf;
    break;
  case 12: case 13:
    {
      win_value e;
      win_value_init(&e, 3);
      if (item->mode & 0x80) e.type = 2;
      win_item_values_access(w, item, &e, 0);
      {
        char *t = e.u.s;
        if (t == 0) t = "(null)";
        s = t;
      }
      len = strlen(s);
    }
    break;
  case 14:
    s = e_off_on_names[win_item_bit_get(w, item)];
    len = strlen(s);
    break;
  case 15:
    {
      win_value f;
      int mode;
      names = item->names;
      win_value_init(&f, 0);
      win_item_values_access(w, item, &f, 0);
      mode = item->names_flags;
      if (mode > 3) {
        names = 0; /* ORIGINAL SHAPE (corroborated 2026-11-10: cse1: a branch body is needed here to keep this branch apart until cse1 has run, after which it vanishes; what the body held is assumed): the list is cleared for a names mode it does not handle; the store is dead after the break, but it keeps this branch through cse1, so the +24 byte is read again below (else-if without it: 5 lines) */
        break;
      }
      if (item->names_flags & 2) {
        if (win_addr_check(names) == 0) return;
        names = *(char ***)names;
      }
      if (names == 0 || f.u.i < 0) break;
      s = names[f.u.i];
      if (s == 0) s = "(null)";
      len = strlen(s);
    }
    break;
  }
  if (s != 0) str_copy_right_aligned(text, width, s, len);
}

uint win_item_bit_get(win_rec *w, win_item *it)
{
  uint b = it->bit;
  win_value v;
  win_value_init(&v, 0);
  win_item_values_access(w, it, &v, 0);
  return (v.u.i >> b) & 1;
}

void win_value_load(win_value *v, int *addr, int width)
{
  int i;
  float f;
  int w;

  switch (v->type) {
  case 0:
    i = 0;
    switch ((uint)width) {
    case 0: i = *(char *)addr; break;
    case 1: i = *(short *)addr; break;
    case 2: i = *addr; break;
    }
    v->u.i = i;
    break;
  case 1:
    f = 0;
    if (width == 3) f = *(float *)addr;
    v->u.f = f;
    break;
  case 2:
    w = (int)addr;
    if (addr != 0) w = *addr;
    v->u.i = w;
    break;
  case 3:
    v->u.s = (char *)addr;
    break;
  }
}

void win_value_store(win_value *v, int *addr, int width)
{
  int i;
  float f;

  switch(v->type) {
  case 0:
    i = v->u.i;
    switch ((uint)width) {
    case 0:
      *(char *)addr = (char)i;
      break;
    case 1:
      *(short *)addr = (short)i;
      break;
    case 2:
      *addr = i;
      break;
    }
    break;
  case 1:
    f = v->u.f;
    if (width == 3) {
      *(float *)addr = f;
    }
    break;
  case 2:
    *addr = v->u.i;
    break;
  case 3:
    break;
  }
}

void win_item_values_access(win_rec *w, win_item *it, win_value *v, int store)
{
  int *addr;
  int base;
  win_ref *ref;
  int n_values;
  uint width;
  uint mode;

  base = w->value_base;
  width = it->flags & 0xf;
  ref = it->values;
  mode = it->mode & 0x30;
  if (mode == 0x10) n_values = 2;
  else if (mode == 0x20) n_values = 3;
  else n_values = 1;
  for (; n_values != 0; n_values--) {
    addr = (int *)win_ref_address(ref);
    if ((ref->flags & 3) == 2) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libdebug/src/win_sys.c",0x6cf,"assertion `%s\' failed.\n",
               "0");
    }
    if ((it->flags & 0x100) != 0) {
      addr = (int *)((int)addr + base);
    }
    if ((it->flags & 0x2000) != 0) {
      win_addr_check(addr);
      addr = (int *)*addr;
    }
    win_addr_check(addr);
    if (store != 0) {
      win_value_store(v,addr,width);
    }
    else {
      win_value_load(v,addr,width);
    }
    v++;
    ref = ref + 1;
  }
}

int win_text_cols(win_rec *w)
{
  return w->width - 1;
}

void win_text_put(win_rec *w, int col, int row, char *text, uint len)
{
  int n;
  uint c;
  uint i;
  uchar buf [84];

  n = win_text_cols(w) - col;
  if (len < (uint)n) {
    n = len;
  }
  if (0 < n) {
    if (-1 < row) {
      if ((int)(uint)w->height > row) {
        if (0x50 < n) {
          n = 0x50;
        }
        for (i = 0; i < 0x50 && i < n; i++) {
          c = (uint)*(uchar *)(text + i);
          if (((int)(char)(ctype_table + 1)[c] & 0x97U) != 0) {
            buf[i] = c;
          }
          else if (c == 0xd) {
            buf[i] = 3;
          }
          else if (c == 10) {
            buf[i] = 4;
          }
          else {
            buf[i] = 0x2e;
          }
        }
        buf[i] = 0;
        str_copy_no_nul(&win_screen_text[2][2] +
                     (uint)w->x + col +
                     ((uint)w->y + row) * 0x80,buf,n);
      }
    }
  }
  return;
}

void win_text_put_all(win_rec *w, int col, int row, char *text)
{
  win_text_put(w,col,row,text,0xffffffff);
  return;
}

void win_frame_draw(win_rec *win)
{
  int w;
  char *t;
  char *d;
  int i;
  int j;
  int k;
  char l;
  char r;
  int h;
  
  w = win->width + 1;
  if (win->focus) t = (char *)&win_border_glyphs_focus;
  else t = (char *)&win_border_glyphs;
  d = &win_screen_text[0][0] + win->x + win->y * 0x80;
  d[0] = t[4];
  for (i = 1; i < w; i++) d[i] = t[0];
  d[i] = t[5];
  d = &win_screen_text[2][0] + win->x + (win->y + win->height) * 0x80;
  d[0] = t[6];
  for (j = 1; j < w; j++) d[j] = t[1];
  d[j] = t[7];
  l = t[2];
  r = t[3];
  h = win->height + 1;
  d = &win_screen_text[1][0] + win->x + win->y * 0x80;
  for (k = 0; k < h; k++, d += 0x80) {
    d[0] = l;
    d[w] = r;
  }
}

void win_bitmask_item_draw(win_rec *win,win_item *it,void *line,size_t cols)
{
  win_value s;
  int w;
  int i;
  int mask;
  char *name;
  size_t len;
  char **names;

  w = 12;
  win_value_init(&s, 0);
  names = it->names;
  win_item_values_access(win,it,&s,0);
  mask = s.u.i;
  for (i = 0; i < 0x10; i++, line += 0x80) {
    memset(line,0x20,cols);
    if ((mask >> i) & 1) {
      name = names[i];
      len = strlen(name);
      len = win_max(w, len);
      str_copy_no_nul(line + 1,name,len);
    }
    if ((mask >> (i + 0x10)) & 1) {
      name = names[i + 0x10];
      len = strlen(name);
      len = win_max(w, len);
      str_copy_no_nul(line + w + 2,name,len);
    }
  }
}

void str_copy_no_nul(char *dst, char *src, int n)
{
  char c;

  if (*src != '\0' && n != 0) {
    do {
      c = *src;
      src = src + 1;
      *dst = c;
      dst = dst + 1;
      n = n + -1;
    } while (*src != '\0' && n != 0);
  }
  return;
}

void str_copy_right_aligned(char *dst, uint width, char *src, uint len)
{
  if (len > width) {
    len = width;
  }
  str_copy_no_nul((dst + width) - len,src,len);
  return;
}

int win_timer_start(void)
{
  int now;

  now = stopwatch_usec();
  return now;
}

int win_timer_elapsed(int start)
{
  int now;

  now = stopwatch_usec();
  return now - start;
}

void win_enable_check(void)
{
  int on;

  win_input_on = 0;
  win_active = 0;
  on = debug_info_enabled();
  if (on != 0) {
    win_active = 1;
    on = pause_is_on();
    if (on != 0) {
      win_input_on = 1;
      on = win_hide_combo_held();
      if (on != 0) {
        win_active = 0;
      }
    }
  }
  return;
}

void win_text_flush(char *text)
{
  int row;

  text_color_set(win_text_color);
  text_cell_size_set(win_cell_w,win_cell_h);
  text_cursor_to_cell(0,0);
  row = 0;
  do {
    text[0x50] = 10;
    text[0x51] = 0;
    text_draw_string(text);
    row = row + 1;
    text = text + 0x80;
  } while (row < 0x50);
  text_cell_size_set(win_cell_w_restore,win_cell_h_restore);
  text_color_set(win_text_color_restore);
  return;
}

void win_background_draw(win_rec *win)
{
  typedef struct { float x; float y; float z; uint c; } vtx;
  vtx v[4];
  uint t;
  float x0;
  float y0;
  float w;
  float h;
  int i;
  t = win_bg_trans;
  win_bg_argb = t << 0x18 | win_bg_colors[(char)win_bg_color] & 0xffffff;
  x0 = (float)win_cell_w * (float)(win->x + 1) - 3.0;
  y0 = (float)win_cell_h * (float)(win->y + 1) - 3.0;
  w = (float)win_cell_w * (float)win->width + 6.0;
  h = (float)win_cell_h * (float)(win->height + 1) + 6.0;
  for (i = 0; i < 4; i++) {
    v[i].z = win_bg_z;
    v[i].c = win_bg_argb;
  }
  v[0].x = x0;
  v[0].y = y0;
  v[1].x = x0;
  v[1].y = y0 + h;
  v[2].x = x0 + w;
  v[2].y = y0;
  v[3].x = x0 + w;
  v[3].y = y0 + h;
  if (t == 0xff) {
    polystrip_plain_opaque(4, (uint *)v);
  }
  else if (t != 0) {
    polystrip_plain_blended(4, (uint *)v);
  }
}

uint rgb444_to_rgb565(uint color)
{
  ushort c = color;
  int red;
  ushort v;
  ushort g;
  red = (((c & 0xf00) >> 7) | 1) << 11;
  v = red;
  g = ((c & 0xf0) * 8) | 0x60;
  v |= g;
  v |= ((c & 0xf) * 2) | 1;
  return v;
}

uint fb_glyph_pixel(int glyph, int x, int y)
{
  return rgb444_to_rgb565(text_glyph_pixels[y * 8 + (glyph << 6) + x]);
}

void fb_glyph_put(int *cursor, int glyph)
{
  int fb;
  int base;
  int i;
  int j;
  ushort *q;
  fb = *(volatile int *)0xa05f8114 + 0xa5000000;
  base = cursor[1] * 0x280 + cursor[0];
  base = base * 0x10 + fb;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      q = (ushort *)((i * 0x280 + j) * 2 + base);
      *q = fb_glyph_pixel(glyph,j,i);
    }
  }
}

void fb_text_cursor_set(int column, int row)
{
  fb_text_cursor[0] = column;
  fb_text_cursor[1] = row;
  return;
}

void fb_text_print(char *text)
{
  int *cur;
  int c;

  cur = fb_text_cursor;
  while ((c = *text++) != 0) {
    if (c == 0xd) {
      cur[0] = 0;
    }
    else if (c == 10) {
      cur[0] = 0;
      cur[1]++;
    }
    else {
      int a;
      if (((ctype_table + 1)[c] & 0x97) != 0) {
        a = c;
      }
      else {
        a = 0x2e;
      }
      fb_glyph_put(cur,a);
      cur[0]++;
    }
    if (cur[0] > 0x4f) {
      cur[0] = 0;
      cur[1]++;
    }
    cur[1] = cur[1] % 60;
  }
}
