/*
 * zlib/infblock.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void inflate_blocks_reset(inflate_blocks_state *s, z_streamp z, uLongf *c)
{
  uLong check;
  
  if (c != (uLongf *)0x0) {
    *c = s->check;
  }
  if (s->mode - BTREE < 2) {
    (*z->zfree)(z->opaque,(s->sub).trees.blens);
  }
  if (s->mode == CODES) {
    inflate_codes_free((inflate_codes_state *)(s->sub).left,z);
  }
  s->mode = TYPE;
  s->bitk = 0;
  s->bitb = 0;
  s->write = s->window;
  s->read = s->window;
  if (s->checkfn != (check_func)0x0) {
    check = (*s->checkfn)(0,(Bytef *)0x0,0);
    s->check = check;
    z->adler = check;
  }
  return;
}

int inflate_blocks_free(inflate_blocks_state *s, z_streamp z)
{
  inflate_blocks_reset(s,z,(uLongf *)0x0);
  (*z->zfree)(z->opaque,s->window);
  (*z->zfree)(z->opaque,s->hufts);
  (*z->zfree)(z->opaque,s);
  return 0;
}

int inflate_blocks_sync_point(inflate_blocks_state *s)
{
  return (uint)(s->mode == LENS);
}

inflate_blocks_state * inflate_blocks_new(z_streamp z, check_func c, uInt w)
{
  inflate_blocks_state *s;

  if ((s = (inflate_blocks_state *)(*z->zalloc)(z->opaque,1,sizeof(inflate_blocks_state))) == 0)
    return s;
  if ((s->hufts = (inflate_huft *)(*z->zalloc)(z->opaque,8,0x5a0)) == 0)
  {
    (*z->zfree)(z->opaque,s);
    return 0;
  }
  if ((s->window = (Bytef *)(*z->zalloc)(z->opaque,1,w)) == 0)
  {
    (*z->zfree)(z->opaque,s->hufts);
    (*z->zfree)(z->opaque,s);
    return 0;
  }
  s->end = s->window + w;
  s->checkfn = c;
  s->mode = TYPE;
  inflate_blocks_reset(s,z,0);
  return s;
}

int inflate_blocks(inflate_blocks_state *s, z_streamp z, int r)
{
  uInt t;
  uLong b;
  uInt k;
  Bytef *p;
  uInt n;
  Bytef *q;
  uInt m;
  {{p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;} {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}}
  while (1) switch (s->mode)
  {
    case TYPE:
      {while(k<(3)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
      t = (uInt)b & 7;
      s->last = t & 1;
      switch (t >> 1)
      {
        case 0:
          {b>>=(3);k-=(3);}
          t = k & 7;
          {b>>=(t);k-=(t);}
          s->mode = LENS;
          break;
        case 1:
          {
            uInt bl, bd;
            inflate_huft *tl, *td;
            inflate_trees_fixed(&bl, &bd, &tl, &td, z);
            s->sub.decode.codes = inflate_codes_new(bl, bd, tl, td, z);
            if (s->sub.decode.codes == 0)
            {
              r = (-4);
              {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
            }
          }
          {b>>=(3);k-=(3);}
          s->mode = CODES;
          break;
        case 2:
          {b>>=(3);k-=(3);}
          s->mode = TABLE;
          break;
        case 3:
          {b>>=(3);k-=(3);}
          s->mode = BAD;
          z->msg = (char*)"invalid block type";
          r = (-3);
          {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      }
      break;
    case LENS:
      {while(k<(32)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
      if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
      {
        s->mode = BAD;
        z->msg = (char*)"invalid stored block lengths";
        r = (-3);
        {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      }
      s->sub.left = (uInt)b & 0xffff;
      b = k = 0;
      s->mode = s->sub.left ? STORED : (s->last ? DRY : TYPE);
      break;
    case STORED:
      if (n == 0)
        {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      {if(m==0){{if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} if(m==0){{{s->write=q;} r=inflate_flush(s,z,r); {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} if(m==0) {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}}}r=0;}
      t = s->sub.left;
      if (t > n) t = n;
      if (t > m) t = m;
      memcpy(q, p, t);
      p += t; n -= t;
      q += t; m -= t;
      if ((s->sub.left -= t) != 0)
        break;
      s->mode = s->last ? DRY : TYPE;
      break;
    case TABLE:
      {while(k<(14)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
      s->sub.trees.table = t = (uInt)b & 0x3fff;
      if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
      {
        s->mode = BAD;
        z->msg = (char*)"too many length or distance symbols";
        r = (-3);
        {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      }
      t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
      if ((s->sub.trees.blens = (uIntf*)(*((z)->zalloc))((z)->opaque, (t), (sizeof(uInt)))) == 0)
      {
        r = (-4);
        {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      }
      {b>>=(14);k-=(14);}
      s->sub.trees.index = 0;
      s->mode = BTREE;
    case BTREE:
      while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
      {
        {while(k<(3)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
        s->sub.trees.blens[border[s->sub.trees.index++]] = (uInt)b & 7;
        {b>>=(3);k-=(3);}
      }
      while (s->sub.trees.index < 19)
        s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
      s->sub.trees.bb = 7;
      t = inflate_trees_bits(s->sub.trees.blens, &s->sub.trees.bb,
                             &s->sub.trees.tb, s->hufts, z);
      if (t != 0)
      {
        r = t;
        if (r == (-3))
        {
          (*((z)->zfree))((z)->opaque, (voidpf)(s->sub.trees.blens));
          s->mode = BAD;
        }
        {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      }
      s->sub.trees.index = 0;
      s->mode = DTREE;
    case DTREE:
      while (t = s->sub.trees.table,
             s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
      {
        inflate_huft *h;
        uInt i, j, c;
        t = s->sub.trees.bb;
        {while(k<(t)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
        h = s->sub.trees.tb + ((uInt)b & inflate_mask[t]);
        t = h->word.what.Bits;
        c = h->base;
        if (c < 16)
        {
          {b>>=(t);k-=(t);}
          s->sub.trees.blens[s->sub.trees.index++] = c;
        }
        else
        {
          i = c == 18 ? 7 : c - 14;
          j = c == 18 ? 11 : 3;
          {while(k<(t + i)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
          {b>>=(t);k-=(t);}
          j += (uInt)b & inflate_mask[i];
          {b>>=(i);k-=(i);}
          i = s->sub.trees.index;
          t = s->sub.trees.table;
          if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
              (c == 16 && i < 1))
          {
            (*((z)->zfree))((z)->opaque, (voidpf)(s->sub.trees.blens));
            s->mode = BAD;
            z->msg = (char*)"invalid bit length repeat";
            r = (-3);
            {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
          }
          c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
          do {
            s->sub.trees.blens[i++] = c;
          } while (--j);
          s->sub.trees.index = i;
        }
      }
      s->sub.trees.tb = 0;
      {
        uInt bl, bd;
        inflate_huft *tl, *td;
        inflate_codes_state *c;
        bl = 9;
        bd = 6;
        t = s->sub.trees.table;
        t = inflate_trees_dynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
                                  s->sub.trees.blens, &bl, &bd, &tl, &td,
                                  s->hufts, z);
        if (t != 0)
        {
          if (t == (uInt)(-3))
          {
            (*((z)->zfree))((z)->opaque, (voidpf)(s->sub.trees.blens));
            s->mode = BAD;
          }
          r = t;
          {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
        }
        if ((c = inflate_codes_new(bl, bd, tl, td, z)) == 0)
        {
          r = (-4);
          {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
        }
        s->sub.decode.codes = c;
      }
      (*((z)->zfree))((z)->opaque, (voidpf)(s->sub.trees.blens));
      s->mode = CODES;
    case CODES:
      {{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}}
      if ((r = inflate_codes(s, z, r)) != 1)
        return inflate_flush(s, z, r);
      r = 0;
      inflate_codes_free(s->sub.decode.codes, z);
      {{p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;} {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}}
      if (!s->last)
      {
        s->mode = TYPE;
        break;
      }
      s->mode = DRY;
    case DRY:
      {{s->write=q;} r=inflate_flush(s,z,r); {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}}
      if (s->read != s->write)
        {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      s->mode = DONE;
    case DONE:
      r = 1;
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
    case BAD:
      r = (-3);
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
    default:
      r = (-2);
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
  }
}

void inflate_set_dictionary(inflate_blocks_state *s, const Bytef *d, uInt n)
{
  memcpy(s->window,d,n);
  s->write = s->window + n;
  s->read = s->window + n;
  return;
}
