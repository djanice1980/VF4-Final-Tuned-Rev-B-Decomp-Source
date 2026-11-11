/*
 * naomi2lib/libnl2http/src/map.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

bool form_map_parse(form_map *map, char *query)
{
  char *s;
  char *send;
  char *out;
  char *bend;
  char *eq;
  char *amp;
  int n;
  char *key;
  char *value;
  form_map_pair *e;

  s = query;
  memset(map,0,0x9ec);
  if (s == 0) return 0;
  map->count = 0;
  n = strlen(s);
  map->buf_size = 0x800;
  map->buf = map->buf_space;
  out = map->buf;
  bend = out + map->buf_size;
  send = s + n;
  while (s < send && out < bend) {
    key = out;
    eq = strchr(s,'=');
    if (eq == 0) break;
    n = url_unescape(out,bend - out,s,eq - s);
    if (n == 0) break;
    out += n;
    if (out >= bend) break;
    *out++ = 0;
    s = eq + 1;
    if (s == send) {
      value = out - 1;
    }
    else {
      value = out;
      amp = strchr(s,'&');
      if (amp == 0) amp = send;
      n = url_unescape(out,bend - out,s,amp - s);
      out += n;
      if (out >= bend) break;
      *out++ = 0;
      s = amp + 1;
    }
    if (map->count > 59) break;
    e = &map->pairs[map->count];
    e->key = key;
    e->value = value;
    map->count = map->count + 1;
  }
  return s < send && out >= bend;
}

int form_map_clear(form_map *map)
{
  memset(map,0,0x9ec);
  return 0;
}

int form_map_int(form_map *map, char *name)
{
  char *text;
  int value;
  
  text = form_map_find(map,name);
  if (!(text == 0)) {
    value = strtol(text,0,10);
  }
  else {
    value = 0;
  }
  return value;
}

uint form_map_uint(form_map *map, char *name)
{
  char *text;
  uint value;
  
  text = form_map_find(map,name);
  if (!(text == 0)) {
    value = strtoul(text,0,10);
  }
  else {
    value = 0;
  }
  return value;
}

uint form_map_hex(form_map *map, char *name)
{
  char *text;
  uint value;
  
  text = form_map_find(map,name);
  if (!(text == 0)) {
    value = strtoul(text,0,0x10);
  }
  else {
    value = 0;
  }
  return value;
}

char * form_map_find(form_map *map, char *name)
{
  int cmp;
  char *ret;
  int i;
  
  ret = 0;
  for (i = 0; i < map->count; i++) {
    if ((*name == *map->pairs[i].key) &&
       (cmp = strcmp(name,map->pairs[i].key), cmp == 0)) {
      ret = map->pairs[i].value;
      break;
    }
  }
  return ret;
}

int url_unescape(char *dst, int dst_size, char *src, int src_len)
{
  char *d;
  char *s;
  char *send;
  char *dend;
  int c;
  int a;
  int b;
  if ((dst == 0) | (src == 0)) return 0;
  s = src;
  send = s + src_len;
  d = dst;
  dend = dst + dst_size;
  while ((s < send) & (d < dend)) {
    c = *s++;
    if (c == 0) break;
    if (c == '+') c = ' ';
    else if (c == '%') {
      if (s + 2 > send) break;
      a = *s++;
      b = *s++;
      c = (hex_digit_value((char)a) << 4) | hex_digit_value((char)b);
    }
    *d++ = c;
  }
  return d - dst;
}

uint hex_digit_value(int ch)
{
  uint r;
  int c;
  
  c = (int)(char)ch;
  r = 0;
  if ((uchar)(c - 0x30) <= 9) {
    r = c - 0x30;
  }
  else if ((uchar)(c - 0x61) <= 5) {
    r = c - 0x57;
  }
  else if ((uchar)(c - 0x41) <= 5) {
    r = c - 0x37;
  }
  return r;
}

char * form_map_copy_out(form_map *map, char *name, char *out, uint out_size)
{
  char *r;
  int n;
  r = form_map_find(map,name);
  if (out != 0) {
    memset(out,0,out_size);
    if (r != 0) {
      n = snprintf(out,out_size,"%s",r);
      if (n + 1U > out_size) {
        fatal_at("/p71/prj71p/vf4e2/naomi2lib/libnl2http/src/map.c",0x131,
                 "out_buf too short %d + 1 > %d",n,out_size);

      }
    }
  }
  return r;
}
