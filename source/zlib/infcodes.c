/*
 * zlib/infcodes.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

void inflate_codes_free(inflate_codes_state *c, z_streamp z)
{
  (*z->zfree)(z->opaque,c);
  return;
}

inflate_codes_state * inflate_codes_new(uInt bl, uInt bd, inflate_huft *tl, inflate_huft *td, z_streamp z)
{
  inflate_codes_state *c;
  
  c = (*z->zalloc)(z->opaque,1,0x1c);
  if (c != (inflate_codes_state *)0x0) {
    c->mode = START;
    c->lbits = (Byte)bl;
    c->dbits = (Byte)bd;
    c->ltree = tl;
    c->dtree = td;
  }
  return c;
}

int inflate_codes(inflate_blocks_state *s, z_streamp z, int r)
{
  uInt j;
  inflate_huft *t;
  uInt e;
  uLong b;
  uInt k;
  Bytef *p;
  uInt n;
  Bytef *q;
  uInt m;
  Bytef *f;
  inflate_codes_state *c = s->sub.decode.codes;
  {{p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;} {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}}
  while (1) switch (c->mode)
  {
    case START:
      if (m >= 258 && n >= 10)
      {
        {{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}}
        r = inflate_fast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
        {{p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;} {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}}
        if (r != 0)
        {
          c->mode = r == 1 ? WASH : BADCODE;
          break;
        }
      }
      c->sub.code.need = c->lbits;
      c->sub.code.tree = c->ltree;
      c->mode = LEN;
    case LEN:
      j = c->sub.code.need;
      {while(k<(j)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      {b>>=(t->word.what.Bits);k-=(t->word.what.Bits);}
      e = (uInt)(t->word.what.Exop);
      if (e == 0)
      {
        c->sub.lit = t->base;
        c->mode = LIT;
        break;
      }
      if (e & 16)
      {
        c->sub.copy.get = e & 15;
        c->len = t->base;
        c->mode = LENEXT;
        break;
      }
      if ((e & 64) == 0)
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      if (e & 32)
      {
        c->mode = WASH;
        break;
      }
      c->mode = BADCODE;
      z->msg = (char*)"invalid literal/length code";
      r = (-3);
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
    case LENEXT:
      j = c->sub.copy.get;
      {while(k<(j)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
      c->len += (uInt)b & inflate_mask[j];
      {b>>=(j);k-=(j);}
      c->sub.code.need = c->dbits;
      c->sub.code.tree = c->dtree;
      c->mode = DIST;
    case DIST:
      j = c->sub.code.need;
      {while(k<(j)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      {b>>=(t->word.what.Bits);k-=(t->word.what.Bits);}
      e = (uInt)(t->word.what.Exop);
      if (e & 16)
      {
        c->sub.copy.get = e & 15;
        c->sub.copy.dist = t->base;
        c->mode = DISTEXT;
        break;
      }
      if ((e & 64) == 0)
      {
        c->sub.code.need = e;
        c->sub.code.tree = t + t->base;
        break;
      }
      c->mode = BADCODE;
      z->msg = (char*)"invalid distance code";
      r = (-3);
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
    case DISTEXT:
      j = c->sub.copy.get;
      {while(k<(j)){{if(n)r=0;else {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}};b|=((uLong)(n--,*p++))<<k;k+=8;}}
      c->sub.copy.dist += (uInt)b & inflate_mask[j];
      {b>>=(j);k-=(j);}
      c->mode = COPY;
    case COPY:
      f = q - c->sub.copy.dist;
      while (f < s->window)
        f += s->end - s->window;
      while (c->len)
      {
        {if(m==0){{if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} if(m==0){{{s->write=q;} r=inflate_flush(s,z,r); {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} if(m==0) {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}}}r=0;}
        {*q++=(Byte)(*f++);m--;}
        if (f == s->end)
          f = s->window;
        c->len--;
      }
      c->mode = START;
      break;
    case LIT:
      {if(m==0){{if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} if(m==0){{{s->write=q;} r=inflate_flush(s,z,r); {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}} if(m==0) {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}}}r=0;}
      {*q++=(Byte)(c->sub.lit);m--;}
      c->mode = START;
      break;
    case WASH:
      if (k > 7)
      {
        k -= 8;
        n++;
        p--;
      }
      {{s->write=q;} r=inflate_flush(s,z,r); {q=s->write;m=(uInt)(uInt)(q<s->read?s->read-q-1:s->end-q);}}
      if (s->read != s->write)
        {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
      c->mode = C_END;
    case C_END:
      r = 1;
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
    case BADCODE:
      r = (-3);
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
    default:
      r = (-2);
      {{{s->bitb=b;s->bitk=k;} {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;} {s->write=q;}} return inflate_flush(s,z,r);}
  }
}
