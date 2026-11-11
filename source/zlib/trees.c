/*
 * zlib/trees.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void tr_static_init(void)
{
  return;
}

int build_bl_tree(deflate_state *s)
{
  int max_blindex;
  
  scan_tree(s,s->dyn_ltree,(s->l_desc).max_code);
  scan_tree(s,s->dyn_dtree,(s->d_desc).max_code);
  build_tree(s,&s->bl_desc);
  max_blindex = 0x12;
  do {
    if (s->bl_tree[bl_order[max_blindex]].dl.dad != 0) break;
    max_blindex = max_blindex + -1;
  } while (2 < max_blindex);
  s->opt_len = max_blindex * 3 + s->opt_len + 0x11;
  return max_blindex;
}

void set_data_type(deflate_state *s)
{
  uint ascii_freq;
  int n;
  uint bin_freq;
  
  n = 0;
  ascii_freq = 0;
  bin_freq = 0;
  do {
    bin_freq = bin_freq + ((s->dyn_ltree + n)->fc).freq;
    n = n + 1;
  } while (7 > n);
  for (; n < 0x80; n = n + 1) {
    ascii_freq = ascii_freq + s->dyn_ltree[n].fc.freq;
  }
  for (; n < 0x100; n = n + 1) {
    bin_freq = bin_freq + s->dyn_ltree[n].fc.freq;
  }
  s->data_type = bin_freq <= ascii_freq >> 2;
  return;
}

void init_block(deflate_state *s)
{
  ct_data_s *tree;
  int n;
  
  n = 0;
  while (n < 0x11e) {
    tree = s->dyn_ltree + n;
    n = n + 1;
    (tree->fc).freq = 0;
  
  }
  n = 0;
  do {
    tree = s->dyn_dtree + n;
    n = n + 1;
    (tree->fc).freq = 0;
  } while (n < 0x1e);
  n = 0;
  do {
    tree = s->bl_tree + n;
    n = n + 1;
    (tree->fc).freq = 0;
  } while (n < 0x13);
  s->dyn_ltree[0x100].fc.freq = 1;
  s->static_len = 0;
  s->opt_len = 0;
  s->matches = 0;
  s->last_lit = 0;
  return;
}

void gen_codes(ct_data *tree, int max_code, ushf *bl_count)
{
  ush next_code[16];
  ush code = 0;
  int bits;
  int n;
  
  for (bits = 1; bits <= 15; bits++) {
    next_code[bits] = code = (code + bl_count[bits-1]) << 1;
  }
  for (n = 0; n <= max_code; n++) {
    int len = tree[n].dl.dad;
    if (len == 0) continue;
    tree[n].fc.freq = (ush)bi_reverse(next_code[len]++, len);
  }
}

uint bi_reverse(uint code, int len)
{
  register uint res = 0;
  do {
    res |= code & 1;
    code >>= 1, res <<= 1;
  } while (--len > 0);
  return res >> 1;
}

void pqdownheap(deflate_state *s, ct_data *tree, int k)
{
  int v = s->heap[k];
  int j = k << 1;
  while (j <= s->heap_len) {
    if (j < s->heap_len &&
        (tree[s->heap[j+1]].fc.freq < tree[s->heap[j]].fc.freq ||
         (tree[s->heap[j+1]].fc.freq == tree[s->heap[j]].fc.freq &&
          s->depth[s->heap[j+1]] <= s->depth[s->heap[j]]))) {
      j++;
    }
    if (tree[v].fc.freq < tree[s->heap[j]].fc.freq ||
        (tree[v].fc.freq == tree[s->heap[j]].fc.freq && s->depth[v] <= s->depth[s->heap[j]])) break;
    s->heap[k] = s->heap[j];  k = j;
    j <<= 1;
  }
  s->heap[k] = v;
}

void bi_flush(deflate_state *s)
{
  if (s->bi_valid == 16) {
    s->pending_buf[s->pending++] = (Bytef)((s->bi_buf) & 0xff);
    s->pending_buf[s->pending++] = (Bytef)((ush)(s->bi_buf) >> 8);
    s->bi_buf = 0;
    s->bi_valid = 0;
  } else if (s->bi_valid >= 8) {
    s->pending_buf[s->pending++] = (Bytef)s->bi_buf;
    s->bi_buf >>= 8;
    s->bi_valid -= 8;
  }
}

void copy_block(deflate_state *s, charf *buf, uint len, int header)
{
  bi_windup(s);
  s->last_eob_len = 8;
  if (header) {
    {s->pending_buf[s->pending++] = (Bytef)((ush)len & 0xff);}
    {s->pending_buf[s->pending++] = (Bytef)((ush)len >> 8);}
    {s->pending_buf[s->pending++] = (Bytef)((ush)~len & 0xff);}
    {s->pending_buf[s->pending++] = (Bytef)((ush)~len >> 8);}
  }
  while (len--) {
    {s->pending_buf[s->pending++] = (Bytef)(*buf++);}
  }
}

void bi_windup(deflate_state *s)
{
  if (s->bi_valid > 8) {
    s->pending_buf[s->pending++] = (Bytef)((s->bi_buf) & 0xff);
    s->pending_buf[s->pending++] = (Bytef)((ush)(s->bi_buf) >> 8);
  } else if (s->bi_valid > 0) {
    s->pending_buf[s->pending++] = (Byte)s->bi_buf;
  }
  s->bi_buf = 0;
  s->bi_valid = 0;
}

void gen_bitlen(deflate_state *s, tree_desc *desc)
{
  ct_data *tree = desc->dyn_tree;
  int max_code = desc->max_code;
  const ct_data *stree = desc->stat_desc->static_tree;
  const intf *extra = desc->stat_desc->extra_bits;
  int base = desc->stat_desc->extra_base;
  int max_length = desc->stat_desc->max_length;
  int h;
  int n, m;
  int bits;
  int xbits;
  ush f;
  int overflow = 0;

  for (bits = 0; bits <= 15; bits++) s->bl_count[bits] = 0;

  tree[s->heap[s->heap_max]].dl.len = 0;

  for (h = s->heap_max+1; h < 573; h++) {
    n = s->heap[h];
    bits = tree[tree[n].dl.dad].dl.len + 1;
    if (bits > max_length) bits = max_length, overflow++;
    tree[n].dl.len = (ush)bits;

    if (n > max_code) continue;

    s->bl_count[bits]++;
    xbits = 0;
    if (n >= base) xbits = extra[n-base];
    f = tree[n].fc.freq;
    s->opt_len += (ulg)f * (bits + xbits);
    if (stree) s->static_len += (ulg)f * (stree[n].dl.len + xbits);
  }
  if (overflow == 0) return;

  do {
    bits = max_length-1;
    while (s->bl_count[bits] == 0) bits--;
    s->bl_count[bits]--;
    s->bl_count[bits+1] += 2;
    s->bl_count[max_length]--;
    overflow -= 2;
  } while (overflow > 0);

  for (bits = max_length; bits != 0; bits--) {
    n = s->bl_count[bits];
    while (n != 0) {
      m = s->heap[--h];
      if (m > max_code) continue;
      if (tree[m].dl.len != (unsigned) bits) {
        s->opt_len += ((long)bits - (long)tree[m].dl.len)
                      *(long)tree[m].fc.freq;
        tree[m].dl.len = (ush)bits;
      }
      n--;
    }
  }
}

void scan_tree(deflate_state *s, ct_data *tree, int max_code)
{
    int n;
    int prevlen = -1;
    int curlen;
    int nextlen = tree[0].dl.len;
    int count = 0;
    int max_count = 7;
    int min_count = 4;

    if (nextlen == 0) max_count = 138, min_count = 3;
    tree[max_code+1].dl.len = (ush)0xffff;

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].dl.len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            s->bl_tree[curlen].fc.freq += count;
        } else if (curlen != 0) {
            if (curlen != prevlen) s->bl_tree[curlen].fc.freq++;
            s->bl_tree[16].fc.freq++;
        } else if (count <= 10) {
            s->bl_tree[17].fc.freq++;
        } else {
            s->bl_tree[18].fc.freq++;
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

void build_tree(deflate_state *s, tree_desc *desc)
{
    ct_data *tree         = desc->dyn_tree;
    const ct_data *stree  = desc->stat_desc->static_tree;
    int elems             = desc->stat_desc->elems;
    int n, m;
    int max_code = -1;
    int node;

    s->heap_len = 0, s->heap_max = 573;

    for (n = 0; n < elems; n++) {
        if (tree[n].fc.freq != 0) {
            s->heap[++(s->heap_len)] = max_code = n;
            s->depth[n] = 0;
        } else {
            tree[n].dl.len = 0;
        }
    }

    while (s->heap_len < 2) {
        node = s->heap[++(s->heap_len)] = (max_code < 2 ? ++max_code : 0);
        tree[node].fc.freq = 1;
        s->depth[node] = 0;
        s->opt_len--; if (stree) s->static_len -= stree[node].dl.len;
    }
    desc->max_code = max_code;

    for (n = s->heap_len/2; n >= 1; n--) pqdownheap(s, tree, n);

    node = elems;
    do {
        n = s->heap[1];
        s->heap[1] = s->heap[s->heap_len--];
        pqdownheap(s, tree, 1);
        m = s->heap[1];

        s->heap[--(s->heap_max)] = n;
        s->heap[--(s->heap_max)] = m;

        tree[node].fc.freq = tree[n].fc.freq + tree[m].fc.freq;
        s->depth[node] = (uch) ((s->depth[n] >= s->depth[m] ? s->depth[n] : s->depth[m]) + 1);
        tree[n].dl.dad = tree[m].dl.dad = (ush)node;
        s->heap[1] = node++;
        pqdownheap(s, tree, 1);

    } while (s->heap_len >= 2);

    s->heap[--(s->heap_max)] = s->heap[1];

    gen_bitlen(s, (tree_desc *)desc);

    gen_codes ((ct_data *)tree, max_code, s->bl_count);
}

void send_tree(deflate_state *s, ct_data *tree, int max_code)
{
    int n;
    int prevlen = -1;
    int curlen;
    int nextlen = tree[0].dl.len;
    int count = 0;
    int max_count = 7;
    int min_count = 4;
    if (nextlen == 0) max_count = 138, min_count = 3;
    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].dl.len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            do { { int len = s->bl_tree[curlen].dl.len; if (s->bi_valid > (int)16 - len) { int val = s->bl_tree[curlen].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (s->bl_tree[curlen].fc.code) << s->bi_valid; s->bi_valid += len; } }; } while (--count != 0);
        } else if (curlen != 0) {
            if (curlen != prevlen) {
                { int len = s->bl_tree[curlen].dl.len; if (s->bi_valid > (int)16 - len) { int val = s->bl_tree[curlen].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (s->bl_tree[curlen].fc.code) << s->bi_valid; s->bi_valid += len; } }; count--;
            }
            { int len = s->bl_tree[16].dl.len; if (s->bi_valid > (int)16 - len) { int val = s->bl_tree[16].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (s->bl_tree[16].fc.code) << s->bi_valid; s->bi_valid += len; } }; { int len = 2; if (s->bi_valid > (int)16 - len) { int val = count-3; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (count-3) << s->bi_valid; s->bi_valid += len; } };
        } else if (count <= 10) {
            { int len = s->bl_tree[17].dl.len; if (s->bi_valid > (int)16 - len) { int val = s->bl_tree[17].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (s->bl_tree[17].fc.code) << s->bi_valid; s->bi_valid += len; } }; { int len = 3; if (s->bi_valid > (int)16 - len) { int val = count-3; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (count-3) << s->bi_valid; s->bi_valid += len; } };
        } else {
            { int len = s->bl_tree[18].dl.len; if (s->bi_valid > (int)16 - len) { int val = s->bl_tree[18].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (s->bl_tree[18].fc.code) << s->bi_valid; s->bi_valid += len; } }; { int len = 7; if (s->bi_valid > (int)16 - len) { int val = count-11; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (count-11) << s->bi_valid; s->bi_valid += len; } };
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

void send_all_trees(deflate_state *s, int lcodes, int dcodes, int blcodes)
{
    int rank;
    { int len = 5; if (s->bi_valid > (int)16 - len) { int val = lcodes-257; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (lcodes-257) << s->bi_valid; s->bi_valid += len; } };
    { int len = 5; if (s->bi_valid > (int)16 - len) { int val = dcodes-1; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (dcodes-1) << s->bi_valid; s->bi_valid += len; } };
    { int len = 4; if (s->bi_valid > (int)16 - len) { int val = blcodes-4; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (blcodes-4) << s->bi_valid; s->bi_valid += len; } };
    for (rank = 0; rank < blcodes; rank++) {
        { int len = 3; if (s->bi_valid > (int)16 - len) { int val = s->bl_tree[bl_order[rank]].dl.len; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (s->bl_tree[bl_order[rank]].dl.len) << s->bi_valid; s->bi_valid += len; } };
    }
    send_tree(s, (ct_data *)s->dyn_ltree, lcodes-1);
    send_tree(s, (ct_data *)s->dyn_dtree, dcodes-1);
}

void compress_block(deflate_state *s, ct_data *ltree, ct_data *dtree)
{
    unsigned dist;
    int lc;
    unsigned lx = 0;
    unsigned code;
    int extra;
    if (s->last_lit != 0) do {
        dist = s->d_buf[lx];
        lc = s->l_buf[lx++];
        if (dist == 0) {
            { int len = ltree[lc].dl.len; if (s->bi_valid > (int)16 - len) { int val = ltree[lc].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (ltree[lc].fc.code) << s->bi_valid; s->bi_valid += len; } };
        } else {
            code = _length_code[lc];
            { int len = ltree[code+256+1].dl.len; if (s->bi_valid > (int)16 - len) { int val = ltree[code+256+1].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (ltree[code+256+1].fc.code) << s->bi_valid; s->bi_valid += len; } };
            extra = extra_lbits[code];
            if (extra != 0) {
                lc -= base_length[code];
                { int len = extra; if (s->bi_valid > (int)16 - len) { int val = lc; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (lc) << s->bi_valid; s->bi_valid += len; } };
            }
            dist--;
            code = ((dist) < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)]);
            { int len = dtree[code].dl.len; if (s->bi_valid > (int)16 - len) { int val = dtree[code].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (dtree[code].fc.code) << s->bi_valid; s->bi_valid += len; } };
            extra = extra_dbits[code];
            if (extra != 0) {
                dist -= base_dist[code];
                { int len = extra; if (s->bi_valid > (int)16 - len) { int val = dist; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (dist) << s->bi_valid; s->bi_valid += len; } };
            }
        }
    } while (lx < s->last_lit);
    { int len = ltree[256].dl.len; if (s->bi_valid > (int)16 - len) { int val = ltree[256].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (ltree[256].fc.code) << s->bi_valid; s->bi_valid += len; } };
    s->last_eob_len = ltree[256].dl.len;
}

void _tr_init(deflate_state *s)
{
  tr_static_init();
  (s->l_desc).dyn_tree = s->dyn_ltree;
  (s->l_desc).stat_desc = &static_l_desc;
  (s->d_desc).dyn_tree = s->dyn_dtree;
  (s->d_desc).stat_desc = &static_d_desc;
  (s->bl_desc).dyn_tree = s->bl_tree;
  (s->bl_desc).stat_desc = &static_bl_desc;
  s->bi_buf = 0;
  s->bi_valid = 0;
  s->last_eob_len = 8;
  init_block(s);
  return;
}

void _tr_stored_block(deflate_state *s, charf *buf, ulg stored_len, int eof)
{
    { int len = 3; if (s->bi_valid > (int)16 - len) { int val = (0<<1)+eof; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= ((0<<1)+eof) << s->bi_valid; s->bi_valid += len; } };
    copy_block(s, buf, (unsigned)stored_len, 1);
}

void _tr_align(deflate_state *s)
{
    { int len = 3; if (s->bi_valid > (int)16 - len) { int val = 1<<1; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (1<<1) << s->bi_valid; s->bi_valid += len; } };
    { int len = static_ltree[256].dl.len; if (s->bi_valid > (int)16 - len) { int val = static_ltree[256].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (static_ltree[256].fc.code) << s->bi_valid; s->bi_valid += len; } };
    bi_flush(s);
    if (1 + s->last_eob_len + 10 - s->bi_valid < 9) {
        { int len = 3; if (s->bi_valid > (int)16 - len) { int val = 1<<1; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (1<<1) << s->bi_valid; s->bi_valid += len; } };
        { int len = static_ltree[256].dl.len; if (s->bi_valid > (int)16 - len) { int val = static_ltree[256].fc.code; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= (static_ltree[256].fc.code) << s->bi_valid; s->bi_valid += len; } };
        bi_flush(s);
    }
    s->last_eob_len = 7;
}

void _tr_flush_block(deflate_state *s, charf *buf, ulg stored_len, int eof)
{
    ulg opt_lenb, static_lenb;
    int max_blindex = 0;
    if (s->level > 0) {
        if (s->data_type == 2) set_data_type(s);
        build_tree(s, (tree_desc *)(&(s->l_desc)));
        build_tree(s, (tree_desc *)(&(s->d_desc)));
        max_blindex = build_bl_tree(s);
        opt_lenb = (s->opt_len+3+7)>>3;
        static_lenb = (s->static_len+3+7)>>3;
        if (static_lenb <= opt_lenb) opt_lenb = static_lenb;
    } else {
        opt_lenb = static_lenb = stored_len + 5;
    }
    if (stored_len+4 <= opt_lenb && buf != (char*)0) {
        _tr_stored_block(s, buf, stored_len, eof);
    } else if (static_lenb == opt_lenb) {
        { int len = 3; if (s->bi_valid > (int)16 - len) { int val = (1<<1)+eof; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= ((1<<1)+eof) << s->bi_valid; s->bi_valid += len; } };
        compress_block(s, (ct_data *)static_ltree, (ct_data *)static_dtree);
    } else {
        { int len = 3; if (s->bi_valid > (int)16 - len) { int val = (2<<1)+eof; s->bi_buf |= (val << s->bi_valid); { {s->pending_buf[s->pending++] = ((uch)((s->bi_buf) & 0xff));}; {s->pending_buf[s->pending++] = ((uch)((ush)(s->bi_buf) >> 8));}; }; s->bi_buf = (ush)val >> (16 - s->bi_valid); s->bi_valid += len - 16; } else { s->bi_buf |= ((2<<1)+eof) << s->bi_valid; s->bi_valid += len; } };
        send_all_trees(s, s->l_desc.max_code+1, s->d_desc.max_code+1, max_blindex+1);
        compress_block(s, (ct_data *)s->dyn_ltree, (ct_data *)s->dyn_dtree);
    }
    init_block(s);
    if (eof) {
        bi_windup(s);
    }
}

int _tr_tally(deflate_state *s, unsigned dist, unsigned lc)
{
    s->d_buf[s->last_lit] = (ush)dist;
    s->l_buf[s->last_lit++] = (uch)lc;
    if (dist == 0) {
        s->dyn_ltree[lc].fc.freq++;
    } else {
        s->matches++;
        dist--;
        s->dyn_ltree[_length_code[lc]+256+1].fc.freq++;
        s->dyn_dtree[((dist) < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)])].fc.freq++;
    }
    return (s->last_lit == s->lit_bufsize-1);
}
