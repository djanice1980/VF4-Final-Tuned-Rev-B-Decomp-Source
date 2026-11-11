/*
 * vf4lib/libcard/src/sol.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

int sol_card_find(char *deck, int card, int count)
{
  int i = 0;
  while (count-- != 0) {
    if (*deck == card) return i;
    deck++;
    i++;
  }
  return -1;
}

int sol_keystream_next(char *deck)
{
  int a;
  int b;
  int c;
  char buf[22];

  do {
    c = sol_card_find(deck,0x15,0x16);
    if (c < 0x15) {
      a = c + 1;
      deck[c] = deck[a];
    }
    else {
      for (a = 0x15; a > 1; a--) {
        deck[a] = deck[a - 1];
      }
      a = 1;
    }
    deck[a] = 0x15;
    b = sol_card_find(deck,0x16,0x16);
    if (b < 0x14) {
      c = b + 1;
      deck[b] = deck[c];
      if (a == c) {
        a = b;
      }
      b = c + 1;
      deck[c] = deck[b];
      if (a == b) {
        a = c;
      }
    }
    else {
      c = b;
      b -= 0x13;
      for (; c > b; c--) {
        deck[c] = deck[c - 1];
        if (a == c - 1) {
          a = c;
        }
      }
    }
    deck[b] = 0x16;
    if (a > b) {
      c = a;
      a = b;
      b = c;
    }
    buf[21] = deck[b];
    b++;
    c = 0;
    while (b < 0x16) {
      buf[c++] = deck[b++];
    }
    b = a;
    while (deck[b] != buf[21]) {
      buf[c++] = deck[b++];
    }
    buf[c++] = buf[21];
    b = 0;
    while (b < a) {
      buf[c++] = deck[b++];
    }
    if (c != 0x16) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/sol.c",0x5f,"assertion `%s\' failed.\n",
               "c == 22");
    }
    b = buf[21];
    if (b == 0x16) {
      b = 0x15;
    }
    a = 0;
    for (c = b; c < 0x15; c++) {
      deck[a++] = buf[c];
    }
    for (c = 0; c < b; c++) {
      deck[a++] = buf[c];
    }
    if (a != 0x15) {
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libcard/src/sol.c",0x6c,"assertion `%s\' failed.\n",
               "a == 21");
    }
    deck[0x15] = buf[21];
    a = *deck;
    if (a == 0x16) {
      a = 0x15;
    }
    a = deck[a];
  } while (a > 0x14);
  return (a - 1) % 10 + 1;
}

int sol_digit_shift(int digit, int shift)
{
  return (digit + shift - 0x26) % 10 + 0x30;
}

void sol_deck_key(char *deck, byte *key)
{
  char c;
  int i;
  int j;
  int n;
  char buf [21];
  
  for (j = 0; j < 0x16; j++) {
    deck[j] = j + 1;
  }
  while (*key != 0) {
    n = *key++;
    if ((uint)(n - 0x30) < 10) {
      n -= 0x2f;
      sol_keystream_next(deck);
      j = 0;
      for (i = n; i < 0x15; i++) {
        buf[j++] = deck[i];
      }
      for (i = 0; i < n; i++) {
        buf[j++] = deck[i];
      }
      for (i = 0; i < 0x15; i++) {
        deck[i] = buf[i];
      }
    }
  }
  return;
}

void sol_digits_crypt(int decrypt, byte *key, byte *src, char *dst)
{
  int digit;
  int shift;
  int i;
  char *p;
  char deck [24];
  
  i = 0;
  sol_deck_key(deck,key);
  while (*src != 0) {
    digit = *src++;
    p = dst + i;
    i++;
    if (decrypt != 0) {
      shift = -sol_keystream_next(deck);
    }
    else {
      shift = sol_keystream_next(deck);
    }
    *p = sol_digit_shift(digit,shift);
  }
  dst[i] = 0;
  return;
}
