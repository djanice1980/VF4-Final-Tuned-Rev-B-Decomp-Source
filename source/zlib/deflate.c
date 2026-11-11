/*
 * zlib/deflate.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void lm_init(deflate_state *s)
{
  int level;
  
  s->window_size = s->w_size * 2;
  s->head[s->hash_size - 1] = 0;
  memset(s->head,0,s->hash_size * 2 - 2);
  level = s->level;
  s->max_lazy_match = (uint)configuration_table[level].max_lazy;
  s->good_match = (uint)configuration_table[level].good_length;
  s->nice_match = (uint)configuration_table[level].nice_length;
  s->max_chain_length = (uint)configuration_table[level].max_chain;
  s->strstart = 0;
  s->block_start = 0;
  s->lookahead = 0;
  s->prev_length = 2;
  s->match_length = 2;
  s->match_available = 0;
  s->ins_h = 0;
  return;
}

void flush_pending(z_streamp strm)
{
  deflate_state *s;
  uint n;
  
  n = ((deflate_state *)strm->state)->pending;
  if (((deflate_state *)strm->state)->pending > strm->avail_out) {
    n = strm->avail_out;
  }
  if (n != 0) {
    memcpy(strm->next_out,((deflate_state *)strm->state)->pending_out,n);
    strm->next_out = strm->next_out + n;
    ((deflate_state *)strm->state)->pending_out = ((deflate_state *)strm->state)->pending_out + n;
    strm->total_out = strm->total_out + n;
    strm->avail_out = strm->avail_out - n;
    ((deflate_state *)strm->state)->pending = ((deflate_state *)strm->state)->pending - n;
    s = (deflate_state *)strm->state;
    if (s->pending == 0) {
      s->pending_out = s->pending_buf;
    }
  }
  return;
}

int deflateReset(z_streamp strm)
{
  deflate_state *s;
  if (strm == (z_streamp)0x0 || strm->state == (internal_state *)0x0 ||
      strm->zalloc == (alloc_func)0x0 || strm->zfree == (free_func)0x0) return -2;
  strm->total_in = strm->total_out = 0;
  strm->msg = (char *)0x0;
  strm->data_type = 2;
  s = (deflate_state *)strm->state;
  s->pending = 0;
  s->pending_out = s->pending_buf;
  if (s->noheader < 0) {
    s->noheader = 0;
  }
  s->status = s->noheader ? 0x71 : 0x2a;
  strm->adler = 1;
  s->last_flush = 0;
  _tr_init(s);
  lm_init(s);
  return 0;
}

int read_buf(z_streamp strm, Bytef *buf, uint size)
{
  uint len = strm->avail_in;
  if (len > size) len = size;
  if (len == 0) return 0;
  strm->avail_in -= len;
  if (!((deflate_state *)strm->state)->noheader) {
    strm->adler = adler32(strm->adler, strm->next_in, len);
  }
  memcpy(buf, strm->next_in, len);
  strm->next_in += len;
  strm->total_in += len;
  return (int)len;
}

void putShortMSB(deflate_state *s, uInt b)
{
  s->pending_buf[s->pending++] = (Byte)(b >> 8);
  s->pending_buf[s->pending++] = (Byte)(b & 0xff);
}

int deflateParams(z_streamp strm, int level, int strategy)
{
  deflate_state *s;
  compress_func func;
  int err = 0;

  if (strm == 0 || strm->state == 0) return -2;
  s = (deflate_state *)strm->state;

  if (level == -1) {
    level = 6;
  }
  if (level < 0 || level > 9 || strategy < 0 || strategy > 2) {
    return -2;
  }
  func = configuration_table[s->level].func;

  if (func != configuration_table[level].func && strm->total_in != 0) {
    err = deflate(strm, 1);
  }
  if (s->level != level) {
    s->level = level;
    s->max_lazy_match   = configuration_table[level].max_lazy;
    s->good_match       = configuration_table[level].good_length;
    s->nice_match       = configuration_table[level].nice_length;
    s->max_chain_length = configuration_table[level].max_chain;
  }
  s->strategy = strategy;
  return err;
}

int deflateCopy(z_streamp dest, z_streamp source)
{
    deflate_state *ds;
    deflate_state *ss;
    ushf *overlay;

    if (source == 0 || dest == 0 || source->state == 0) {
        return -2;
    }
    ss = (deflate_state *)source->state;

    *dest = *source;

    ds = (deflate_state *) (*dest->zalloc)(dest->opaque, 1, sizeof(deflate_state));
    if (ds == 0) return -4;
    dest->state = (struct internal_state *) ds;
    *ds = *ss;
    ds->strm = dest;

    ds->window = (Bytef *) (*dest->zalloc)(dest->opaque, ds->w_size, 2*sizeof(Byte));
    ds->prev   = (Posf *)  (*dest->zalloc)(dest->opaque, ds->w_size, sizeof(Pos));
    ds->head   = (Posf *)  (*dest->zalloc)(dest->opaque, ds->hash_size, sizeof(Pos));
    overlay = (ushf *) (*dest->zalloc)(dest->opaque, ds->lit_bufsize, sizeof(ush)+2);
    ds->pending_buf = (uchf *) overlay;

    if (ds->window == 0 || ds->prev == 0 || ds->head == 0 ||
        ds->pending_buf == 0) {
        deflateEnd (dest);
        return -4;
    }
    memcpy(ds->window, ss->window, ds->w_size * 2 * sizeof(Byte));
    memcpy(ds->prev, ss->prev, ds->w_size * sizeof(Pos));
    memcpy(ds->head, ss->head, ds->hash_size * sizeof(Pos));
    memcpy(ds->pending_buf, ss->pending_buf, (uInt)ds->pending_buf_size);

    ds->pending_out = ds->pending_buf + (ss->pending_out - ss->pending_buf);
    ds->d_buf = overlay + ds->lit_bufsize/sizeof(ush);
    ds->l_buf = ds->pending_buf + (1+sizeof(ush))*ds->lit_bufsize;

    ds->l_desc.dyn_tree = ds->dyn_ltree;
    ds->d_desc.dyn_tree = ds->dyn_dtree;
    ds->bl_desc.dyn_tree = ds->bl_tree;

    return 0;
}

int deflateEnd(z_streamp strm)
{
  int status;
  if (strm == 0 || strm->state == 0) return -2;
  status = ((deflate_state *)strm->state)->status;
  if (status != 0x2a && status != 0x71 && status != 0x29a) {
    return -2;
  }
  if (((deflate_state *)strm->state)->pending_buf) (*strm->zfree)(strm->opaque, ((deflate_state *)strm->state)->pending_buf);
  if (((deflate_state *)strm->state)->head) (*strm->zfree)(strm->opaque, ((deflate_state *)strm->state)->head);
  if (((deflate_state *)strm->state)->prev) (*strm->zfree)(strm->opaque, ((deflate_state *)strm->state)->prev);
  if (((deflate_state *)strm->state)->window) (*strm->zfree)(strm->opaque, ((deflate_state *)strm->state)->window);
  (*strm->zfree)(strm->opaque, strm->state);
  strm->state = 0;
  return status == 0x71 ? -3 : 0;
}

int deflate(z_streamp strm, int flush)
{
  int old_flush;
  deflate_state *s;
  if (strm == 0 || strm->state == 0 || flush > 4 || flush < 0) {
    return -2;
  }
  s = (deflate_state *)strm->state;
  if (strm->next_out == 0 || (strm->next_in == 0 && strm->avail_in != 0) ||
      (s->status == 666 && flush != 4)) {
    return (strm->msg = z_errmsg[4], -2);
  }
  if (strm->avail_out == 0) return (strm->msg = z_errmsg[7], -5);
  s->strm = strm;
  old_flush = s->last_flush;
  s->last_flush = flush;
  if (s->status == 42) {
    uint header = (8 + ((s->w_bits - 8) << 4)) << 8;
    uint level_flags = (s->level - 1) >> 1;
    if (level_flags > 3) level_flags = 3;
    header |= (level_flags << 6);
    if (s->strstart != 0) header |= 0x20;
    header += 31 - (header % 31);
    s->status = 113;
    putShortMSB(s, header);
    if (s->strstart != 0) {
      putShortMSB(s, (uint)(strm->adler >> 16));
      putShortMSB(s, (uint)(strm->adler & 0xffff));
    }
    strm->adler = 1;
  }
  if (s->pending != 0) {
    flush_pending(strm);
    if (strm->avail_out == 0) {
      s->last_flush = -1;
      return 0;
    }
  } else if (strm->avail_in == 0 && flush <= old_flush && flush != 4) {
    return (strm->msg = z_errmsg[7], -5);
  }
  if (s->status == 666 && strm->avail_in != 0) {
    return (strm->msg = z_errmsg[7], -5);
  }
  if (strm->avail_in != 0 || s->lookahead != 0 ||
      (flush != 0 && s->status != 666)) {
    block_state bstate;
    bstate = (*(configuration_table[s->level].func))(s, flush);
    if (bstate == finish_started || bstate == finish_done) {
      s->status = 666;
    }
    if (bstate == need_more || bstate == finish_started) {
      if (strm->avail_out == 0) {
        s->last_flush = -1;
      }
      return 0;
    }
    if (bstate == block_done) {
      if (flush == 1) {
        _tr_align(s);
      } else {
        _tr_stored_block(s, (charf *)0, 0, 0);
        if (flush == 3) {
          s->head[s->hash_size - 1] = 0;
          memset((Bytef *)s->head, 0, (unsigned)(s->hash_size - 1) * sizeof(*s->head));
        }
      }
      flush_pending(strm);
      if (strm->avail_out == 0) {
        s->last_flush = -1;
        return 0;
      }
    }
  }
  if (flush != 4) return 0;
  if (s->noheader) return 1;
  putShortMSB(s, (uint)(strm->adler >> 16));
  putShortMSB(s, (uint)(strm->adler & 0xffff));
  flush_pending(strm);
  s->noheader = -1;
  return s->pending != 0 ? 0 : 1;
}

void fill_window(deflate_state *s)
{
    register unsigned n, m;
    register Posf *p;
    unsigned more;
    uInt wsize = s->w_size;

    do {
        more = (unsigned)(s->window_size -(ulg)s->lookahead -(ulg)s->strstart);

        if (more == 0 && s->strstart == 0 && s->lookahead == 0) {
            more = wsize;

        } else if (more == (unsigned)(-1)) {
            more--;

        } else if (s->strstart >= wsize+(s->w_size-262)) {

            memcpy(s->window, s->window+wsize, (unsigned)wsize);
            s->match_start -= wsize;
            s->strstart    -= wsize;
            s->block_start -= (long) wsize;

            n = s->hash_size;
            p = &s->head[n];
            do {
                m = *--p;
                *p = (Pos)(m >= wsize ? m-wsize : 0);
            } while (--n);

            n = wsize;
            p = &s->prev[n];
            do {
                m = *--p;
                *p = (Pos)(m >= wsize ? m-wsize : 0);
            } while (--n);
            more += wsize;
        }
        if (s->strm->avail_in == 0) return;

        n = read_buf(s->strm, s->window + s->strstart + s->lookahead, more);
        s->lookahead += n;

        if (s->lookahead >= 3) {
            s->ins_h = s->window[s->strstart];
            s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[s->strstart+1])) & s->hash_mask;
        }
    } while (s->lookahead < 262 && s->strm->avail_in != 0);
}

block_state deflate_stored(deflate_state *s, int flush)
{
    ulg max_block_size = 0xffff;
    ulg max_start;

    if (max_block_size > s->pending_buf_size - 5) {
        max_block_size = s->pending_buf_size - 5;
    }

    for (;;) {
        if (s->lookahead <= 1) {
            fill_window(s);
            if (s->lookahead == 0 && flush == 0) return need_more;
            if (s->lookahead == 0) break;
        }

        s->strstart += s->lookahead;
        s->lookahead = 0;

        max_start = s->block_start + max_block_size;
        if (s->strstart == 0 || (ulg)s->strstart >= max_start) {
            s->lookahead = (uInt)(s->strstart - max_start);
            s->strstart = (uInt)max_start;
            _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (0));
            s->block_start = s->strstart;
            flush_pending(s->strm);
            if (s->strm->avail_out == 0) return (0) ? finish_started : need_more;
        }
        if (s->strstart - (uInt)s->block_start >= (s->w_size - 262)) {
            _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (0));
            s->block_start = s->strstart;
            flush_pending(s->strm);
            if (s->strm->avail_out == 0) return (0) ? finish_started : need_more;
        }
    }
    _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (flush == 4));
    s->block_start = s->strstart;
    flush_pending(s->strm);
    if (s->strm->avail_out == 0) return (flush == 4) ? finish_started : need_more;
    return flush == 4 ? finish_done : block_done;
}

uInt longest_match(deflate_state *s, IPos cur_match)
{
    unsigned chain_length = s->max_chain_length;
    register Bytef *scan = s->window + s->strstart;
    register Bytef *match;
    register int len;
    int best_len = s->prev_length;
    int nice_match = s->nice_match;
    IPos limit = s->strstart > (IPos)((s)->w_size - 262) ?
        s->strstart - (IPos)((s)->w_size - 262) : 0;
    Posf *prev = s->prev;
    uInt wmask = s->w_mask;
    register Bytef *strend = s->window + s->strstart + 258;
    register Byte scan_end1  = scan[best_len-1];
    register Byte scan_end   = scan[best_len];

    if (s->prev_length >= s->good_match) {
        chain_length >>= 2;
    }
    if ((uInt)nice_match > s->lookahead) nice_match = s->lookahead;

    do {
        match = s->window + cur_match;

        if (match[best_len]   != scan_end  ||
            match[best_len-1] != scan_end1 ||
            *match            != *scan     ||
            *++match          != scan[1])      continue;

        scan += 2, match++;

        do {
        } while (*++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 scan < strend);

        len = 258 - (int)(strend - scan);
        scan = strend - 258;

        if (len > best_len) {
            s->match_start = cur_match;
            best_len = len;
            if (len >= nice_match) break;
            scan_end1  = scan[best_len-1];
            scan_end   = scan[best_len];
        }
    } while ((cur_match = prev[cur_match & wmask]) > limit
             && --chain_length != 0);

    if ((uInt)best_len <= s->lookahead) return (uInt)best_len;
    return s->lookahead;
}

block_state deflate_fast(deflate_state *s, int flush)
{
    IPos hash_head = 0;
    int bflush;

    for (;;) {
        if (s->lookahead < 262) {
            fill_window(s);
            if (s->lookahead < 262 && flush == 0) {
                return need_more;
            }
            if (s->lookahead == 0) break;
        }

        if (s->lookahead >= 3) {
            (s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[(s->strstart) + (3-1)])) & s->hash_mask), s->prev[(s->strstart) & s->w_mask] = hash_head = s->head[s->ins_h], s->head[s->ins_h] = (Pos)(s->strstart);
        }

        if (hash_head != 0 && s->strstart - hash_head <= (s->w_size - 262)) {
            if (s->strategy != 2) {
                s->match_length = longest_match (s, hash_head);
            }
        }
        if (s->match_length >= 3) {
            { uch len = (s->match_length - 3); ush dist = (s->strstart - s->match_start); s->d_buf[s->last_lit] = dist; s->l_buf[s->last_lit++] = len; dist--; s->dyn_ltree[_length_code[len]+256+1].fc.freq++; s->dyn_dtree[((dist) < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)])].fc.freq++; bflush = (s->last_lit == s->lit_bufsize-1); }

            s->lookahead -= s->match_length;

            if (s->match_length <= s->max_lazy_match &&
                s->lookahead >= 3) {
                s->match_length--;
                do {
                    s->strstart++;
                    (s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[(s->strstart) + (3-1)])) & s->hash_mask), s->prev[(s->strstart) & s->w_mask] = hash_head = s->head[s->ins_h], s->head[s->ins_h] = (Pos)(s->strstart);
                } while (--s->match_length != 0);
                s->strstart++;
            } else {
                s->strstart += s->match_length;
                s->match_length = 0;
                s->ins_h = s->window[s->strstart];
                (s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[s->strstart+1])) & s->hash_mask);
            }
        } else {
            { uch cc = (s->window[s->strstart]); s->d_buf[s->last_lit] = 0; s->l_buf[s->last_lit++] = cc; s->dyn_ltree[cc].fc.freq++; bflush = (s->last_lit == s->lit_bufsize-1); }
            s->lookahead--;
            s->strstart++;
        }
        if (bflush) {
            _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (0));
            s->block_start = s->strstart;
            flush_pending(s->strm);
            if (s->strm->avail_out == 0) return (0) ? finish_started : need_more;
        }
    }
    _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (flush == 4));
    s->block_start = s->strstart;
    flush_pending(s->strm);
    if (s->strm->avail_out == 0) return (flush == 4) ? finish_started : need_more;
    return flush == 4 ? finish_done : block_done;
}

block_state deflate_slow(deflate_state *s, int flush)
{
    IPos hash_head = 0;
    int bflush;
    for (;;) {
        if (s->lookahead < 262) {
            fill_window(s);
            if (s->lookahead < 262 && flush == 0) {
                return need_more;
            }
            if (s->lookahead == 0) break;
        }
        if (s->lookahead >= 3) {
            ((s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[(s->strstart) + (3-1)])) & s->hash_mask), s->prev[(s->strstart) & s->w_mask] = hash_head = s->head[s->ins_h], s->head[s->ins_h] = (Pos)(s->strstart));
        }
        s->prev_length = s->match_length, s->prev_match = s->match_start;
        s->match_length = 3-1;
        if (hash_head != 0 && s->prev_length < s->max_lazy_match &&
            s->strstart - hash_head <= ((s)->w_size-262)) {
            if (s->strategy != 2) {
                s->match_length = longest_match (s, hash_head);
            }
            if (s->match_length <= 5 && (s->strategy == 1 ||
                 (s->match_length == 3 &&
                  s->strstart - s->match_start > 4096))) {
                s->match_length = 3-1;
            }
        }
        if (s->prev_length >= 3 && s->match_length <= s->prev_length) {
            uInt max_insert = s->strstart + s->lookahead - 3;
            { uch len = (s->prev_length - 3); ush dist = (s->strstart -1 - s->prev_match); s->d_buf[s->last_lit] = dist; s->l_buf[s->last_lit++] = len; dist--; s->dyn_ltree[_length_code[len]+256+1].fc.freq++; s->dyn_dtree[((dist) < 256 ? _dist_code[dist] : _dist_code[256+((dist)>>7)])].fc.freq++; bflush = (s->last_lit == s->lit_bufsize-1); };
            s->lookahead -= s->prev_length-1;
            s->prev_length -= 2;
            do {
                if (++s->strstart <= max_insert) {
                    ((s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[(s->strstart) + (3-1)])) & s->hash_mask), s->prev[(s->strstart) & s->w_mask] = hash_head = s->head[s->ins_h], s->head[s->ins_h] = (Pos)(s->strstart));
                }
            } while (--s->prev_length != 0);
            s->match_available = 0;
            s->match_length = 3-1;
            s->strstart++;
            if (bflush) { { _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (0)); s->block_start = s->strstart; flush_pending(s->strm); ; }; if (s->strm->avail_out == 0) return (0) ? finish_started : need_more; };
        } else if (s->match_available) {
            { uch cc = (s->window[s->strstart-1]); s->d_buf[s->last_lit] = 0; s->l_buf[s->last_lit++] = cc; s->dyn_ltree[cc].fc.freq++; bflush = (s->last_lit == s->lit_bufsize-1); };
            if (bflush) {
                { _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (0)); s->block_start = s->strstart; flush_pending(s->strm); ; };
            }
            s->strstart++;
            s->lookahead--;
            if (s->strm->avail_out == 0) return need_more;
        } else {
            s->match_available = 1;
            s->strstart++;
            s->lookahead--;
        }
    }
    if (s->match_available) {
        { uch cc = (s->window[s->strstart-1]); s->d_buf[s->last_lit] = 0; s->l_buf[s->last_lit++] = cc; s->dyn_ltree[cc].fc.freq++; bflush = (s->last_lit == s->lit_bufsize-1); };
        s->match_available = 0;
    }
    { { _tr_flush_block(s, (s->block_start >= 0L ? (charf *)&s->window[(unsigned)s->block_start] : (charf *)0), (ulg)((long)s->strstart - s->block_start), (flush == 4)); s->block_start = s->strstart; flush_pending(s->strm); ; }; if (s->strm->avail_out == 0) return (flush == 4) ? finish_started : need_more; };
    return flush == 4 ? finish_done : block_done;
}

int deflateInit_(z_streamp strm, int level, const char *version, int stream_size)
{
  int err;
  
  err = deflateInit2_(strm,level,8,0xf,8,0,version,stream_size);
  return err;
}

int deflateInit2_(z_streamp strm, int level, int method, int windowBits, int memLevel, int strategy, const char *version, int stream_size)
{
    deflate_state *s;
    int noheader = 0;

    ushf *overlay;

    if (version == 0 || version[0] != my_version[0] ||
        stream_size != sizeof(z_stream)) {
	return -6;
    }
    if (strm == 0) return -2;

    strm->msg = 0;
    if (strm->zalloc == 0) {
	strm->zalloc = zcalloc;
	strm->opaque = (voidpf)0;
    }
    if (strm->zfree == 0) strm->zfree = zcfree;

    if (level == -1) level = 6;

    if (windowBits < 0) {
        noheader = 1;
        windowBits = -windowBits;
    }
    if (memLevel < 1 || memLevel > 9 || method != 8 ||
        windowBits < 9 || windowBits > 15 || level < 0 || level > 9 ||
	strategy < 0 || strategy > 2) {
        return -2;
    }
    s = (deflate_state *) (*((strm)->zalloc))((strm)->opaque, (1), (sizeof(deflate_state)));
    if (s == 0) return -4;
    strm->state = (struct internal_state *)s;
    s->strm = strm;

    s->noheader = noheader;
    s->w_bits = windowBits;
    s->w_size = 1 << s->w_bits;
    s->w_mask = s->w_size - 1;

    s->hash_bits = memLevel + 7;
    s->hash_size = 1 << s->hash_bits;
    s->hash_mask = s->hash_size - 1;
    s->hash_shift =  ((s->hash_bits+3-1)/3);

    s->window = (Bytef *) (*((strm)->zalloc))((strm)->opaque, (s->w_size), (2*sizeof(Byte)));
    s->prev   = (Posf *)  (*((strm)->zalloc))((strm)->opaque, (s->w_size), (sizeof(Pos)));
    s->head   = (Posf *)  (*((strm)->zalloc))((strm)->opaque, (s->hash_size), (sizeof(Pos)));

    s->lit_bufsize = 1 << (memLevel + 6);

    overlay = (ushf *) (*((strm)->zalloc))((strm)->opaque, (s->lit_bufsize), (sizeof(ush)+2));
    s->pending_buf = (uchf *) overlay;
    s->pending_buf_size = (ulg)s->lit_bufsize * (sizeof(ush)+2L);

    if (s->window == 0 || s->prev == 0 || s->head == 0 ||
        s->pending_buf == 0) {
        strm->msg = (char*)z_errmsg[2-(-4)];
        deflateEnd (strm);
        return -4;
    }
    s->d_buf = overlay + s->lit_bufsize/sizeof(ush);
    s->l_buf = s->pending_buf + (1+sizeof(ush))*s->lit_bufsize;

    s->level = level;
    s->strategy = strategy;
    s->method = (Byte)method;

    return deflateReset(strm);
}

int deflateSetDictionary(z_streamp strm, const Bytef *dictionary, uInt dictLength)
{
  deflate_state *s;
  uInt length = dictLength;
  uInt n;
  IPos hash_head = 0;

  if (strm == 0 || strm->state == 0 || dictionary == 0 ||
      ((deflate_state *)strm->state)->status != 42) return -2;

  s = (deflate_state *)strm->state;
  strm->adler = adler32(strm->adler, dictionary, dictLength);

  if (length < 3) return 0;
  if (length > s->w_size - 262) {
    length = s->w_size - 262;
    dictionary += dictLength - length;
  }
  memcpy(s->window, dictionary, length);
  s->strstart = length;
  s->block_start = (long)length;

  s->ins_h = s->window[0];
  s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[1])) & s->hash_mask;
  for (n = 0; n <= length - 3; n++) {
    ((s->ins_h = (((s->ins_h)<<s->hash_shift) ^ (s->window[(n) + (3-1)])) & s->hash_mask), s->prev[(n) & s->w_mask] = hash_head = s->head[s->ins_h], s->head[s->ins_h] = (Pos)(n));
  }
  if (hash_head) hash_head = 0;
  return 0;
}
