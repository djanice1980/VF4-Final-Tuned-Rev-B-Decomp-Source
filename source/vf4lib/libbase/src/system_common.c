/*
 * vf4lib/libbase/src/system_common.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern system_param_value_rec system_param_value; /* 0x0c3601ec */

void dimm_request_hook_install(void)
{
  dimm_request_hook_vector = (uint)dimm_board_request_serve;
  return;
}

void board_ids_read(void)
{
  uchar *ids = system_param_value.main_board_id;
  board_serial_numbers(ids,ids + 0x11);
}

uchar * main_board_id_get(void)
{
  return system_param_value.main_board_id;
}

uchar * dimm_board_id_get(void)
{
  return system_param_value.dimm_board_id;
}

bool media_is_dimm(void)
{
  return system_param_value.media_kind == 2;
}

uint media_limit_get(void)
{
  return system_param_value.media_limit;
}

void media_probe_and_ids(void)
{
  media_board_probe(&system_param_value.media_kind);
  board_ids_read();
  return;
}

void system_param_country_init(void)
{
  int country;
  int nonzero;
  int *config;
  config = (int *)board_config_ptr();
  country = *config;
  nonzero = 0;
  switch(country) {
  case 1:
    country = 1;
    break;
  case 2:
    country = 2;
    break;
  case 3:
    country = 3;
    break;
  case 4:
    country = 4;
    break;
  default:
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/system_common.c",0x72,"assertion `%s\' failed.\n"
             ,"0");
  case 0:
    country = 0;
  }
  if (country != 0) nonzero = 1;
  system_param_value.country = country;
  system_param_value.country_nonzero = nonzero;
  system_param_value.country_copy = country;
  system_param_value.country_nonzero_copy = nonzero;
  system_param_value.initialized_country = 1;
}

int system_param_country_get(void)
{
  if (system_param_value.initialized_country == '\0') {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libbase/src/system_common.c",0x87,"assertion `%s\' failed.\n"
             ,"system_param_value.initialized_country");
  }
  return system_param_value.country;
}

uchar system_param_byte0_get(void)
{
  return system_param_value.field_0x0;
}

void sys_frame_end(void)
{
  frame_end_with_targets();
  return;
}
