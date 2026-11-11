/*
 * vf4lib/libnetwork/src/network.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

int net_log_slot_count_get(void)
{
  return net_log_slot_count;
}

char *network_result_text(int result)
{
  char *s;

  switch (result) {
  case 1:
    s = "CONNECT OK";
    break;
  case 2:
    s = "TIME OUT (NO RESPONSE)";
    break;
  case -1:
    s = "GAME ID VERSION ERROR";
    break;
  case -2:
    s = "SERIAL ERROR";
    break;
  case -3:
    s = "IP ERROR";
    break;
  default:
    s = "TIME OUT (OTHER)";
    break;
  }
  return s;
}

char *network_mode_text(uint mode, int in_test)
{
  char *s;

  if (in_test != 0) {
    switch (mode) {
    case 1:
      s = "(TEST) POWER ON INITIAL";
      break;
    case 2:
      s = "(TEST) NETWORK OFF";
      break;
    case 3:
      s = "(TEST) getIPbyDNS TIME OUT";
      break;
    default:
      s = "(TEST) ???";
      break;
    }
  }
  else {
    switch (mode) {
    case 1:
      s = "(GAME) POWER ON INITIAL";
      break;
    case 2:
      s = "(GAME) NETWORK OFF";
      break;
    case 3:
      s = "(GAME) getIPbyDNS TIME OUT";
      break;
    default:
      s = "(GAME) ???";
      break;
    }
  }
  return s;
}
