/*
 * naomi2lib/libnl2http/src/token.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

int token_list_split(token_list *list, char *text, int separator)
{
  char c = separator;
  size_t len;
  char *dst;
  char *end;
  char *s;
  int n;

  list->count = 0;
  list->cursor = -1;
  list->text = 0;
  len = strlen(text);
  if (len + 1 > 0x100) return 1;
  dst = list->buf;
  strcpy(dst,text);
  list->text = dst;
  end = dst + len;
  for (s = dst; s < end; ) {
    n = list->count;
    if (n > 0x7f) break;
    list->tokens[n] = s;
    list->count = n + 1;
    s = strchr(s,c);
    if (s == 0) {
      s = end;
    }
    else {
      *s = 0;
      s++;
    }
  }
  return 0;
}

int token_list_reset(token_list *list)
{
  list->text = 0;
  list->count = 0;
  list->cursor = 0xffffffff;
  return 0;
}

int token_list_count(int *list)
{
  return *list;
}

bool token_list_has_next(token_list *list)
{
  return list->cursor + 1 < list->count;
}

char * token_list_next(token_list *list)
{
  int more;
  char *token;
  
  more = token_list_has_next(list);
  if (!(more == 0)) {
    list->cursor = list->cursor + 1;
    token = list->tokens[list->cursor];
  }
  else {
    token = 0;
  }
  return token;
}

