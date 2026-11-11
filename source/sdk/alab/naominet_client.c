/*
 * sdk/alab/naominet_client.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

int naominet_init(char *work, naominet_settings_rec *settings, naominet_id_rec *ids, int *timeouts)
{
  dimm_board_info_rec *board;
  naominet_settings_rec *p;
  naominet_id_rec *q;
  int t;
  uint ip;
  int v;
  byte ver_major[4];
  byte ver_minor[4];
  if (naominet_ready == 1) return -1;
  naominet_authed = 0;
  naominet_status = -0x65;
  naominet_bytes_fill((uchar *)&naominet_machine_info,0,0x58);
  naominet_bytes_fill((uchar *)&naominet_score_args,0,0x154);
  naominet_bytes_fill((uchar *)&naominet_score_extra,0,0x34);
  naominet_bytes_fill((uchar *)&naominet_config,0,0x1c);
  naominet_bytes_fill((uchar *)&naominet_last_error,0,0xc);
  naominet_bytes_fill((uchar *)&naominet_uri_host,0,8);
  naominet_bytes_fill((uchar *)&naominet_name_info,0,0xc);
  naominet_bytes_fill((uchar *)&naominet_region_info,0,0x18);
  naominet_bytes_fill((uchar *)&naominet_setting,0,8);
  naominet_bytes_fill((uchar *)&naominet_place_id,0,8);
  naominet_bytes_fill((uchar *)&naominet_timeouts,0,0x20);
  naominet_bytes_fill((uchar *)&naominet_server_time,0,0x10);
  naominet_bytes_fill((uchar *)&naominet_reauth_time,0,3);
  naominet_bytes_fill((uchar *)naominet_jobs,0,0xa00);
  board = dimm_board_info_fetch();
  if (board->id_word == 0) return -7;
  naominet_config.name_server1 = board->name_server1;
  naominet_config.name_server2 = board->name_server2;
  naominet_config.board_mem_size = board->module_bytes;
  naominet_config.board_top_start = board->module_base;
  if (dimm_firmware_version_get((uint *)ver_major,(uint *)ver_minor) != 0) return -4;
  naominet_machine_info.firm_ver_major = ver_major[0];
  naominet_machine_info.firm_ver_minor = ver_minor[0];
  if (settings->tag != 0x41424920) return -3;
  if ((naominet_machine_info.firm_ver_major & 0x7f) <= 2 && naominet_machine_info.firm_ver_minor <= 0x16) return -4;
  for (p = settings; p != 0; p = p->next) {
    if (settings->tag == 0x41424920) {
      if (settings->next != 0) return -3;
      if (work == 0) return -2;
      naominet_reply_text = work;
      naominet_bytes_fill((uchar *)work,0,settings->player_count * 0xc + 0x1000);
      naominet_player_er_flags = (int *)(naominet_reply_text + 0x1000);
      naominet_player_di_flags = (int *)(naominet_reply_text + settings->player_count * 4 + 0x1000);
      naominet_player_active = (int *)(naominet_reply_text + settings->player_count * 8 + 0x1000);
      naominet_conn_pool_area = naominet_reply_text + settings->player_count * 0xc + 0x1000;
      http_client_pool_init(naominet_conn_pool_area,settings->player_count,0);
      if (settings->auto_reauth == 1) {
        naominet_auto_reauth = settings->auto_reauth;
      }
      else if (settings->auto_reauth == 0) {
        naominet_auto_reauth = settings->auto_reauth;
      }
      else return -3;
      if (settings->player_count <= 0) return -3;
      naominet_config.player_count = settings->player_count;
      if ((uint)settings->field_0x10 > 1) return -3;
      if (settings->field_0x10 == 1) return -3;
      naominet_config.field_0x4 = settings->field_0x10;
      t = settings->ip_mode;
      if (((t != 0) & (t != 4)) && t != 6) return -3;
      t = settings->ip_mode;
      if (t == 6) return -3;
      naominet_config.ip_mode = t;
      if (t == 4) {
        ip = settings->ip;
        if (ip != 0) {
          sprintf((char *)&naominet_machine_info.ip_text,"%d.%d.%d.%d",ip & 0xff,(ip & 0xff00) >> 8,(ip & 0xff0000) >> 0x10,ip >> 0x18);
        }
        else return -3;
      }
      else if (naominet_config.ip_mode == 0) {
        ip = settings->ip;
        if (ip != 0) {
          sprintf((char *)&naominet_machine_info.ip_text,"%d.%d.%d.%d",ip & 0xff,(ip & 0xff00) >> 8,(ip & 0xff0000) >> 0x10,ip >> 0x18);
        }
        else {
          naominet_machine_info.ip_text = 0;
        }
      }
      if (settings->game_id == 0) return -3;
      if (strlen(settings->game_id) > 8) return -3;
      strncpy((char *)&naominet_machine_info.game_id,settings->game_id,4);
      if (settings->version == 0) return -3;
      if (strlen(settings->version) > 4) return -3;
      strncpy((char *)&naominet_machine_info.version,settings->version,5);
      if (settings->shift_jis == 0) {
        naominet_machine_info.shift_jis = settings->shift_jis;
      }
      else if (settings->shift_jis == 1) {
        naominet_machine_info.shift_jis = settings->shift_jis;
      }
      else return -3;
      if (settings->serial == 0) return -3;
      if (strlen(settings->serial) > 0x10) return -3;
      if (naominet_str_case_equal(settings->serial,"NO SERIAL ID.   ") == 0) return -3;
      strncpy((char *)&naominet_machine_info.serial,settings->serial,0xb);
    }
  }
  if (ids != 0) {
    q = ids;
    do {
      if (ids->tag == 0x49445820) {
        if (ids->next != 0) return -3;
        if (ids->hi_number < 0) return -3;
        if (ids->hi_number > 999) return -3;
        naominet_score_extra.hi_number = ids->hi_number;
        if (ids->si_number < 0) return -3;
        if (ids->si_number > 99) return -3;
        naominet_score_extra.si_number = ids->si_number;
      }
      q = q->next;
    } while (q != 0);
  }
  else {
    naominet_score_extra.hi_number = 0;
    naominet_score_extra.si_number = 0;
  }
  if (timeouts != 0) {
    v = timeouts[0];
    if (v == 0) v = 90000;
    naominet_timeouts.field_0x0 = v;
    v = timeouts[1];
    if (v == 0) v = 60000;
    naominet_timeouts.resolve_ms = v;
    v = timeouts[2];
    if (v == 0) v = 60000;
    naominet_timeouts.poweron_limit1 = v;
    v = timeouts[3];
    if (v == 0) v = 60000;
    naominet_timeouts.poweron_limit2 = v;
    v = timeouts[4];
    if (v == 0) v = 60000;
    naominet_timeouts.poweron_limit3 = v;
    v = timeouts[5];
    if (v == 0) v = 60000;
    naominet_timeouts.score_limit1 = v;
    v = timeouts[6];
    if (v == 0) v = 60000;
    naominet_timeouts.score_limit2 = v;
    {
    int w = timeouts[7];
    if (w == 0) w = 60000;
    naominet_timeouts.score_limit3 = w;
    }
  }
  else {
    naominet_timeouts.field_0x0 = 90000;
    naominet_timeouts.resolve_ms = 60000;
    naominet_timeouts.poweron_limit1 = 60000;
    naominet_timeouts.poweron_limit2 = 60000;
    naominet_timeouts.poweron_limit3 = 60000;
    naominet_timeouts.score_limit1 = 60000;
    naominet_timeouts.score_limit2 = 60000;
    naominet_timeouts.score_limit3 = 60000;
  }
  naominet_reauth_time.hour = 7;
  naominet_reauth_time.minute = 0;
  naominet_reauth_due = 0;
  naominet_reauth_done = 0;
  naominet_server_addr = 0;
  naominet_score_server_addr = 0;
  naominet_server_resolved = 0;
  naominet_score_server_resolved = 0;
  naominet_router_resolved = 0;
  naominet_retry_wait_ms = 1800000;
  naominet_conn = 0;
  naominet_job_queue.head = 0;
  naominet_job_queue.tail = 0;
  naominet_crc_table_build();
  naominet_lock_init();
  naominet_begin();
  naominet_status = 0x65;
  naominet_ready = 1;
  return 0;
}

int naominet_begin(void)
{
  int r;
  naominet_job_rec *job;
  if (naominet_status == 0x66) return -0xc;
  if (naominet_status == 0x69) return -0xd;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_lock_try() == -1) return -0xe;
  job = naominet_job_alloc();
  if (job == 0) return -0xb;
  job->state = 0;
  naominet_job_enqueue(job);
  naominet_unlock();
  return 0;
}

int naominet_reply_words_get(uint *out)
{
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if (out == (uint *)0x0) return -3;
  *out = naominet_uri_host.uri;
  out[1] = naominet_uri_host.host;
  return 0;
}

int naominet_name_info_get(uint *out)
{
  int r;

  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if (out == (uint *)0x0) return -3;
  *out = naominet_name_info.reply_name;
  out[1] = naominet_name_info.nickname;
  out[2] = naominet_name_info.nickname_tag;
  return 0;
}

int naominet_region_info_get(uint *out)
{
  int r;

  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if (out == (uint *)0x0) return -3;
  *out = naominet_region_info.region;
  out[1] = naominet_region_info.region_text;
  out[2] = naominet_region_info.region_name0;
  out[3] = naominet_region_info.region_name1;
  out[4] = naominet_region_info.region_name2;
  out[5] = naominet_region_info.region_name3;
  return 0;
}

int naominet_place_id_get(ushort *out)
{
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if (out == (ushort *)0x0) return -3;
  *out = naominet_place_id.value;
  *(uint *)(out + 2) = naominet_place_id.text;
  return 0;
}

int naominet_setting_get(uint *out)
{
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if (out == (uint *)0x0) return -3;
  *out = naominet_setting.value;
  out[1] = naominet_setting.text;
  return 0;
}

char * naominet_reply_value_find(char *key, int key_len)
{
  int i;
  int j;
  char *p;
  char *q;

  if (naominet_ready != 1) return (char *)0x0;
  if (naominet_board_check() < 0) return (char *)0x0;
  if (naominet_authed != 1) return (char *)0x0;
  if (key == (char *)0x0) return (char *)0x0;
  p = naominet_reply_text;
  q = key;
  for (i = 0; i < 0x1000; p++, i++) {
    if (*p == *q) {
      for (j = 0; j < key_len; j++) {
        p++;
        q++;
        if (*p != *q) {
          q = key;
          break;
        }
        if (q[1] == '\0') return p + 2;
      }
    }
  }
  return (char *)0x0;
}

int naominet_player_start(int player)
{
  int r;
  naominet_job_rec *job;

  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if ((player > naominet_config.player_count) | (player <= 0)) return -3;
  if (naominet_lock_try() == -1) return -0xe;
  job = naominet_job_alloc();
  if (job != (naominet_job_rec *)0x0) {
    job->player = player + -1;
    if (naominet_player_er_flags[player - 1] == 1) {
      naominet_player_er_flags[player - 1] = 0;
    }
    if (naominet_player_di_flags[player - 1] == 1) {
      naominet_player_di_flags[player - 1] = 0;
    }
    job->state = 0xf;
    job->kind = 2;
    job->er_flag = 0;
    job->di_flag = 0;
    naominet_player_active[player - 1] = 1;
    naominet_job_enqueue(job);
  }
  naominet_unlock();
  return 0;
}

int naominet_player_continue(int player)
{
  int r;
  naominet_job_rec *job;
  int bad;

  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  bad = (player > naominet_config.player_count) | (player <= 0);
  if (bad) return -3;
  if (naominet_player_active[player - 1] != 1) return -6;
  if (naominet_lock_try() == -1) return -0xe;
  job = naominet_job_alloc();
  if (job != (naominet_job_rec *)0x0) {
    job->player = player + -1;
    if (naominet_player_er_flags[player - 1] == 1) {
      naominet_player_er_flags[player - 1] = 0;
      job->er_flag = 1;
    }
    else {
      job->er_flag = 0;
    }
    if (naominet_player_di_flags[player - 1] == 1)
      job->di_flag = 1;
    else
      job->di_flag = 0;
    job->state = 0xf;
    job->kind = 3;
    naominet_job_enqueue(job);
  }
  naominet_unlock();
  return 0;
}

int naominet_player_end(int player)
{
  int r;
  naominet_job_rec *job;

  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if (player > naominet_config.player_count || player < 1) return -3;
  if (naominet_player_active[player - 1] != 1) return -6;
  if (naominet_lock_try() == -1) return -0xe;
  job = naominet_job_alloc();
  if (job != (naominet_job_rec *)0x0) {
    job->player = player + -1;
    if (naominet_player_er_flags[player - 1] == 1) {
      naominet_player_er_flags[player - 1] = 0;
      job->er_flag = 1;
    }
    else {
      job->er_flag = 0;
    }
    if (naominet_player_di_flags[player - 1] == 1) {
      naominet_player_di_flags[player - 1] = 0;
      job->di_flag = 1;
    }
    else {
      job->di_flag = 0;
    }
    naominet_player_active[player - 1] = 0;
    job->state = 0xf;
    job->kind = 4;
    naominet_job_enqueue(job);
  }
  naominet_unlock();
  return 0;
}

int naominet_player_er_set(int player)
{
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if ((player > naominet_config.player_count) | (player <= 0)) return -3;
  if (naominet_lock_try() == -1) return -0xe;
  naominet_player_er_flags[player - 1] = 1;
  naominet_unlock();
  return 0;
}

int naominet_player_di_set(int player)
{
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if ((player > naominet_config.player_count) | (player <= 0)) return -3;
  if (naominet_lock_try() == -1) return -14;
  naominet_player_di_flags[player - 1] = 1;
  naominet_unlock();
  return 0;
}

int naominet_player_er_clear(int player)
{
  int bad;
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  bad = (player > naominet_config.player_count) | (player <= 0);
  if (bad) return -3;
  if (naominet_lock_try() == -1) return -0xe;
  naominet_player_er_flags[player - 1] = 0;
  naominet_unlock();
  return 0;
}

int naominet_player_di_clear(int player)
{
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if ((player > naominet_config.player_count) | (player <= 0)) return -3;
  if (naominet_lock_try() == -1) return -0xe;
  naominet_player_di_flags[player - 1] = 0;
  naominet_unlock();
  return 0;
}

int naominet_status_poll(void)
{
  int old;

  if (naominet_ready != 1) {
    return -0x65;
  }
  if (naominet_authed != 1) {
    switch (naominet_status) {
    case -0x192:
    case -0x191:
    case -0x131:
    case -0x130:
    case -0x12f:
    case -0x12e:
    case -0x66:
      old = naominet_status;
      naominet_status = 0x69;
      naominet_retry_start_ticks = naominet_ticks_now();
      return old;
    case 0x69:
      if (naominet_ms_between(naominet_retry_start_ticks,naominet_ticks_now()) > naominet_retry_wait_ms) {
        naominet_status = 0x65;
      }
      break;
    }
  }
  else if (naominet_authed == 1) {
    switch (naominet_status) {
    case -0x194:
    case -0x193:
    case -0x131:
    case -0x130:
    case -0x12f:
    case -0x12e:
    case -0x66:
      old = naominet_status;
      naominet_status = 0x68;
      return old;
    }
  }
  return naominet_status;
}

naominet_error_rec * naominet_last_error_get(void)
{
  if (naominet_ready != 1) {
    return 0;
  }
  return &naominet_last_error;
}

void naominet_step(void)
{
  struct { uint date[3]; uint hour; uint minute; uint second; } clock;
  int st;
  naominet_job_rec *job;
  int r;
  int i;
  uint ip;
  uint now;

  if (naominet_ready != 1) return;
  if (naominet_status == -101) return;
  if (naominet_lock_try() == -1) return;
  st = naominet_authed;
  if (st == 1) {
    naominet_clock_get(clock.date);
    if (clock.hour == naominet_reauth_time.hour && clock.minute == naominet_reauth_time.minute &&
        clock.second == naominet_reauth_time.second) {
      if (naominet_reauth_done != 0) naominet_reauth_due = 0;
      else naominet_reauth_due = st;
    }
    else {
      naominet_reauth_done = 0;
    }
    if (naominet_reauth_due == 1 && naominet_reauth_done == 0) {
      if (naominet_auto_reauth == 1) {
        for (i = 0; i < naominet_config.player_count; i++) {
          if (naominet_player_active[i] == 1) goto run;
        }
        job = naominet_job_alloc();
        if (job != 0) {
          naominet_status = 101;
          naominet_authed = 0;
          naominet_reauth_due = 0;
          naominet_reauth_done = 1;
          job->state = 0;
          naominet_job_enqueue(job);
          goto done;
        }
      }
      else {
        for (i = 0; i < naominet_config.player_count; i++) {
          if (naominet_player_active[i] == 1) goto run;
        }
        naominet_status = 101;
        naominet_authed = 0;
        naominet_reauth_due = 0;
        naominet_reauth_done = 1;
        goto done;
      }
    }
  }
run:
  if (naominet_job_queue.head != 0) {
    job = naominet_job_queue.head;
    switch (job->state) {
    case 0:
      naominet_bytes_fill((uchar *)naominet_request_body, 0, 0x1000);
      naominet_bytes_fill((uchar *)&naominet_request_headers, 0, 0x1f18);
      naominet_bytes_fill((uchar *)&naominet_response_headers, 0, 0x1f0c);
      naominet_bytes_fill((uchar *)&naominet_response_body, 0, 0x1010);
      naominet_bytes_fill((uchar *)naominet_score_args.play_count, 0, 0x40);
      naominet_bytes_fill((uchar *)naominet_score_args.field_0x54, 0, 0x40);
      naominet_bytes_fill((uchar *)naominet_score_args.play_seconds, 0, 0x40);
      naominet_bytes_fill((uchar *)naominet_score_args.start_ticks, 0, 0x40);
      naominet_bytes_fill((uchar *)naominet_score_args.end_ticks, 0, 0x40);
      if (naominet_config.ip_mode == 0) {
        if (naominet_router_resolved == 0) job->state = 1;
        else if (naominet_server_resolved == 0) job->state = 4;
        else job->state = 7;
      }
      else if (naominet_config.ip_mode == 4) {
        if (naominet_server_resolved == 0) job->state = 4;
        else job->state = 7;
      }
      naominet_status = 102;
      break;
    case 1:
      job->start_ticks = naominet_ticks_now();
    case 2:
      r = host_resolve_start("tenporouter.loc", naominet_config.name_server1, naominet_config.name_server2, 0);
      if (r >= 0) { job->state = 3; break; }
      if (r == -4) {
        now = naominet_ticks_now();
        if (naominet_ms_between(job->start_ticks, now) <= naominet_timeouts.resolve_ms) { job->state = 2; break; }
      }
      naominet_status = -302;
      job->state = 13;
      break;
    case 3:
      if (sock_call_done(0) != 1) break;
      ip = sock_result_take(0);
      if (ip == 0) {
        if (naominet_machine_info.ip_text == 0) {
          naominet_status = -302;
          job->state = 13;
          break;
        }
        if (naominet_server_resolved == 0) job->state = 4;
        else job->state = 7;
        break;
      }
      sprintf((char *)&naominet_machine_info.ip_text, "%d.%d.%d.%d", ip & 0xff, (ip & 0xff00) >> 8, (ip & 0xff0000) >> 16, (ip & 0xff000000) >> 24);
      if (naominet_server_resolved == 0) job->state = 4;
      else job->state = 7;
      naominet_router_resolved = 1;
      break;
    case 4:
      job->start_ticks = naominet_ticks_now();
    case 5:
      r = host_resolve_start("naominet.jp", naominet_config.name_server1, naominet_config.name_server2, 0);
      if (r >= 0) { job->state = 6; break; }
      if (r == -4) {
        now = naominet_ticks_now();
        if (naominet_ms_between(job->start_ticks, now) <= naominet_timeouts.resolve_ms) { job->state = 5; break; }
      }
      naominet_status = -304;
      job->state = 13;
      break;
    case 6:
      if (sock_call_done(0) != 1) break;
      naominet_server_addr = sock_result_take(0);
      if (naominet_server_addr == 0) {
        naominet_status = -304;
        job->state = 13;
        break;
      }
      job->state = 7;
      naominet_server_resolved = 1;
      break;
    case 7:
      if (naominet_poweron_body_build(1, &naominet_machine_info, naominet_request_body) < 0) {
        job->state = 13;
        naominet_status = -102;
      }
      else if (naominet_poweron_request_build(1, &naominet_request_headers, naominet_request_body) < 0) {
        job->state = 13;
        naominet_status = -102;
      }
      else job->state = 8;
      break;
    case 8:
      if (http_client_conn_is_open(naominet_conn) == 0) {
        naominet_conn_idle_poll((int)naominet_conn);
        break;
      }
      naominet_conn = http_client_conn_alloc();
      if (naominet_conn == 0) {
        job->state = 13;
        naominet_status = -102;
        break;
      }
      http_client_server_addr_set(naominet_conn, naominet_server_addr);
      http_client_timeouts_set(naominet_conn, naominet_timeouts.poweron_limit1, naominet_timeouts.poweron_limit2, naominet_timeouts.poweron_limit3);
      http_client_request_set(naominet_conn, &naominet_request_headers, naominet_request_body);
      job->state = 9;
    case 9:
      r = http_client_connect_step(naominet_conn);
      if (r == 0) { job->state = 10; break; }
      if (r == 1) { job->state = 9; break; }
      goto fail1;
    case 10:
      r = http_client_send_step(naominet_conn);
      if (r == 0) {
        http_client_response_set(naominet_conn, &naominet_response_headers, &naominet_response_body);
        job->state = 11;
        break;
      }
      if (r == 1) { job->state = 10; break; }
      goto fail1;
    case 11:
      r = http_client_recv_step(naominet_conn);
      if (r == 0) {
        http_client_conn_close(naominet_conn);
        http_client_conn_reset(naominet_conn);
        naominet_conn = 0;
        job->state = 12;
        break;
      }
      if (r == 1) { job->state = 11; break; }
    fail1:
      if (r < 0) {
        job->state = 13;
        job->sock_error = http_client_sock_error(naominet_conn);
        naominet_last_error_keep(job);
        naominet_status = naominet_error_status_map(0, r);
      }
      break;
    case 12:
      r = naominet_poweron_reply_check(1, &naominet_response_headers, &naominet_response_body, naominet_reply_text, 0x1000);
      if (r < 0) {
        job->state = 13;
        naominet_status = naominet_error_status_map(0, r);
      }
      else if ((r = naominet_poweron_reply_parse(naominet_reply_text, strlen(naominet_reply_text))) < 0) {
        job->state = 13;
        naominet_status = naominet_error_status_map(0, r);
      }
      else {
        job->kind = 1;
        job->state = 15;
        naominet_authed = 1;
        naominet_status = 103;
      }
      break;
    case 13:
      if (naominet_conn != 0) {
        http_client_conn_close(naominet_conn);
        http_client_conn_reset(naominet_conn);
        naominet_conn = 0;
      }
      if (naominet_auto_reauth != 1) job->state = 26;
      else job->state = 14;
      break;
    case 14:
      if (naominet_status == 101) job->state = 0;
      break;
    case 15:
      naominet_bytes_fill((uchar *)naominet_request_body, 0, 0x1000);
      naominet_bytes_fill((uchar *)&naominet_request_headers, 0, 0x1f18);
      naominet_bytes_fill((uchar *)&naominet_response_headers, 0, 0x1f0c);
      naominet_bytes_fill((uchar *)&naominet_response_body, 0, 0x1010);
      naominet_status = 104;
      if (naominet_score_server_resolved == 0) job->state = 16;
      else job->state = 19;
      break;
    case 16:
      job->start_ticks = naominet_ticks_now();
    case 17:
      r = host_resolve_start("sc.naominet.jp", naominet_config.name_server1, naominet_config.name_server2, 0);
      if (r >= 0) { job->state = 18; break; }
      if (r == -4) {
        now = naominet_ticks_now();
        if (naominet_ms_between(job->start_ticks, now) <= naominet_timeouts.resolve_ms) { job->state = 17; break; }
      }
      naominet_status = -304;
      job->state = 25;
      break;
    case 18:
      if (sock_call_done(0) != 1) break;
      naominet_score_server_addr = sock_result_take(0);
      if (naominet_score_server_addr == 0) {
        naominet_status = -304;
        job->state = 25;
        break;
      }
      job->state = 19;
      naominet_score_server_resolved = 1;
      break;
    case 19:
      naominet_score_args.machine = &naominet_machine_info;
      naominet_score_args.player = job->player;
      naominet_score_args.kind = job->kind;
      naominet_score_args.er_flag = job->er_flag;
      naominet_score_args.di_flag = job->di_flag;
      r = naominet_score_body_build(1, &naominet_score_args, &naominet_score_extra, naominet_request_body);
      if (r < 0) {
        job->state = 25;
        naominet_db_counters_update(job->kind, r);
        naominet_status = -102;
      }
      else if ((r = naominet_score_request_build(&naominet_request_headers, naominet_request_body)) < 0) {
        job->state = 25;
        naominet_db_counters_update(job->kind, r);
        naominet_status = -102;
      }
      else if (http_client_conn_is_open(naominet_conn) < 0) job->state = 20;
      else {
        http_client_request_set(naominet_conn, &naominet_request_headers, naominet_request_body);
        job->state = 22;
      }
      break;
    case 20:
      naominet_conn = http_client_conn_alloc();
      if (naominet_conn == 0) {
        job->state = 25;
        naominet_status = -102;
        break;
      }
      http_client_server_addr_set(naominet_conn, naominet_score_server_addr);
      http_client_timeouts_set(naominet_conn, naominet_timeouts.score_limit1, naominet_timeouts.score_limit2, naominet_timeouts.score_limit3);
      http_client_request_set(naominet_conn, &naominet_request_headers, naominet_request_body);
      job->state = 21;
    case 21:
      r = http_client_connect_step(naominet_conn);
      if (r == 0) { job->state = 22; break; }
      if (r == 1) { job->state = 21; break; }
      goto fail2;
    case 22:
      r = http_client_send_step(naominet_conn);
      if (r == 0) {
        http_client_response_set(naominet_conn, &naominet_response_headers, &naominet_response_body);
        job->state = 23;
        break;
      }
      if (r == 1) { job->state = 22; break; }
      goto fail2;
    case 23:
      r = http_client_recv_step(naominet_conn);
      if (r == 0) { job->state = 24; break; }
      if (r == 1) { job->state = 23; break; }
    fail2:
      if (r < 0) {
        job->state = 25;
        naominet_db_counters_update(job->kind, r);
        job->sock_error = http_client_sock_error(naominet_conn);
        naominet_last_error_keep(job);
        naominet_status = naominet_error_status_map(1, r);
      }
      break;
    case 24:
      r = naominet_score_reply_check(1, &naominet_response_headers, &naominet_response_body, &naominet_score_args);
      if (r < 0) {
        job->state = 25;
        naominet_db_counters_update(job->kind, r);
        naominet_status = naominet_error_status_map(1, r);
        break;
      }
      naominet_db_counters_update(job->kind, r);
      job->state = 26;
      break;
    case 25:
      if (naominet_conn != 0) {
        http_client_conn_close(naominet_conn);
        http_client_conn_reset(naominet_conn);
        naominet_conn = 0;
      }
      job->state = 26;
      break;
    case 26:
      naominet_job_unlink(job);
      naominet_job_free(job);
      naominet_job_queue.head = job->next;
      break;
    }
  }
  else if (naominet_authed == 1) naominet_conn_idle_poll((int)naominet_conn);
done:
  naominet_unlock();
}

int naominet_clock_get(uint *out)
{
  rtc_time rtc;
  rtc_time server;
  uint t;
  int x;
  int e;

  if (naominet_ready != 1) {
    return -1;
  }
  e = naominet_board_check();
  if (e < 0) {
    return e;
  }
  if (naominet_authed != 1) {
    e = -5;
  }
  if (out == 0) {
    return -3;
  }
  naominet_rtc_date_read(&rtc);
  if (e < 0) {
    out[0] = rtc.year;
    out[1] = rtc.month;
    out[2] = rtc.day;
    out[3] = rtc.hour;
    out[4] = rtc.minute;
    out[5] = rtc.second;
  }
  else {
    date_to_seconds_1950(&rtc,&t);
    if (naominet_server_time.ahead == 1) {
      x = t + naominet_server_time.offset_seconds;
    }
    else {
      x = t - naominet_server_time.offset_seconds;
    }
    seconds_1950_to_date(x,&server);
    out[0] = server.year;
    out[1] = server.month;
    out[2] = server.day;
    out[3] = server.hour;
    out[4] = server.minute;
    out[5] = server.second;
    e = 0;
  }
  return e;
}

int naominet_auth_request(void)
{
  if (naominet_ready == 1) {
    return naominet_begin();
  }
  return -1;
}

int naominet_reauth_time_set(int hour, int minute, int second)
{
  int r;

  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (naominet_authed != 1) return -5;
  if ((hour < 0 || minute < 0 || second < 0) || (0x17 < hour || 0x3b < minute || 0x3b < second)) {
    return -3;
  }
  if (hour == 0 && minute == 0 && second == 0) {
    naominet_reauth_time.hour = 7;
    naominet_reauth_time.minute = 0;
    naominet_reauth_time.second = (uchar)second;
  }
  else {
    naominet_reauth_time.hour = (uchar)hour;
    naominet_reauth_time.minute = (uchar)minute;
    naominet_reauth_time.second = (uchar)second;
  }
  return 0;
}

int naominet_retry_wait_set(uint wait_ms)
{
  int r;
  if (naominet_ready != 1) return -1;
  r = naominet_board_check();
  if (r < 0) return r;
  if (wait_ms >= 0x36ee81) return -3;
  naominet_retry_wait_ms = wait_ms;
  return 0;
}

int naominet_stop(void)
{
  int i;

  if (naominet_ready != 1) {
    return -1;
  }
  naominet_ready = 0;
  naominet_authed = 0;
  for (i = 0; i < naominet_config.player_count; i++) {
    naominet_player_er_flags[i] = 0;
    naominet_player_di_flags[i] = 0;
    naominet_player_active[i] = 0;
  }
  naominet_status = -101;
  return 0;
}

int naominet_board_check(void)
{
  dimm_board_info_rec *board;

  board = dimm_board_info_fetch();
  if (board->id_word == 0) {
    return -7;
  }
  if ((naominet_machine_info.firm_ver_major & 0x7f) < 3 && naominet_machine_info.firm_ver_minor < 0x17) {
    return -4;
  }
  return 0;
}

int naominet_poweron_reply_parse(char *text, int len)
{
  char *p;
  ushort v;
  char c;
  int i;
  rtc_time now;
  rtc_time server;
  uint now_seconds;
  uint server_seconds;

  p = strstr(text,"uri=");
  if (p == 0) return -1;
  p += 4;
  naominet_uri_host.uri = (uint)p;
  p = strstr(text,"host=");
  if (p == 0) return -1;
  p += 5;
  naominet_uri_host.host = (uint)p;
  p = strstr(text,"place_id=");
  if (p != 0) {
    p += 9;
    naominet_place_id.text = (uint)p;
    c = *p;
    if (*p >= '0' && *p <= '9') { v = (c - '0') * 0x1000; p++; }
    else if (*p >= 'A' && *p <= 'F') { v = (c - 'A' + 10) * 0x1000; p++; }
    else if (*p >= 'a' && *p <= 'f') { v = (c - 'a' + 10) * 0x1000; p++; }
    c = *p;
    if (*p >= '0' && *p <= '9') { v += (c - '0') * 0x100; p++; }
    else if (*p >= 'A' && *p <= 'F') { v += (c - 'A' + 10) * 0x100; p++; }
    else if (*p >= 'a' && *p <= 'f') { v += (c - 'a' + 10) * 0x100; p++; }
    c = *p;
    if (*p >= '0' && *p <= '9') { v += (c - '0') * 0x10; p++; }
    else if (*p >= 'A' && *p <= 'F') { v += (c - 'A' + 10) * 0x10; p++; }
    else if (*p >= 'a' && *p <= 'f') { v += (c - 'a' + 10) * 0x10; p++; }
    c = *p;
    if (*p >= '0' && *p <= '9') { v += c - '0'; }
    else if (*p >= 'A' && *p <= 'F') { v += c - 'A' + 10; }
    else if (*p >= 'a' && *p <= 'f') { v += c - 'a' + 10; }
    naominet_place_id.value = v;
  }
  p = strstr(text,"name=");
  if (p != 0) {
    p += 5;
    naominet_name_info.reply_name = (uint)p;
  }
  p = strstr(text,"nickname=");
  if (p != 0) {
    p += 9;
    naominet_name_info.nickname = (uint)p;
  }
  p = strstr(text,"year=");
  if (p != 0) {
    p += 5;
    naominet_server_time.year = server.year = atol(p);
  }
  p = strstr(text,"month=");
  if (p != 0) {
    p += 6;
    naominet_server_time.month = server.month = atol(p);
  }
  p = strstr(text,"day=");
  if (p != 0) {
    p += 4;
    naominet_server_time.day = server.day = atol(p);
  }
  p = strstr(text,"hour=");
  if (p != 0) {
    p += 5;
    naominet_server_time.hour = server.hour = atol(p);
  }
  p = strstr(text,"minute=");
  if (p != 0) {
    p += 7;
    naominet_server_time.minute = server.minute = atol(p);
  }
  p = strstr(text,"second=");
  if (p != 0) {
    p += 7;
    naominet_server_time.second = server.second = atol(p);
  }
  naominet_rtc_date_read(&now);
  date_to_seconds_1950(&now,&now_seconds);
  date_to_seconds_1950(&server,&server_seconds);
  if (server_seconds > now_seconds) {
    naominet_server_time.ahead = 1;
    naominet_server_time.offset_seconds = server_seconds - now_seconds;
  }
  else {
    naominet_server_time.ahead = 0;
    naominet_server_time.offset_seconds = now_seconds - server_seconds;
  }
  p = strstr(text,"setting=");
  if (p != 0) {
    p += 8;
    naominet_setting.text = (uint)p;
    naominet_setting.value = atol(p);
  }
  p = strstr(text,"region0=");
  if (p != 0) {
    p += 8;
    naominet_region_info.region_text = (uint)p;
    naominet_region_info.region = atol(p);
  }
  p = strstr(text,"region_name0=");
  if (p != 0) {
    p += 13;
    naominet_region_info.region_name0 = (uint)p;
  }
  p = strstr(text,"region_name1=");
  if (p != 0) {
    p += 13;
    naominet_region_info.region_name1 = (uint)p;
  }
  p = strstr(text,"region_name2=");
  if (p != 0) {
    p += 13;
    naominet_region_info.region_name2 = (uint)p;
  }
  p = strstr(text,"region_name3=");
  if (p != 0) {
    p += 13;
    naominet_region_info.region_name3 = (uint)p;
  }
  for (i = 0, p = text; i < len; i++, p++) {
    if (*p == '&') *p = 0;
  }
  url_percent_decode((char *)naominet_name_info.reply_name);
  url_percent_decode((char *)naominet_name_info.nickname);
  url_percent_decode((char *)naominet_region_info.region_name0);
  url_percent_decode((char *)naominet_region_info.region_name1);
  url_percent_decode((char *)naominet_region_info.region_name2);
  url_percent_decode((char *)naominet_region_info.region_name3);
  p = strchr((char *)naominet_name_info.nickname,'#');
  if (p != 0) {
    *p = 0;
    p++;
    naominet_name_info.nickname_tag = (uint)p;
  }
  else {
    naominet_name_info.nickname_tag = 0;
  }
  return 0;
}

int url_percent_decode(char *text)
{
  int i;
  int j;
  int n;
  char c;
  int v;

  i = 0;
  j = 0;
  n = strlen(text);
  while (i < n) {
    c = text[i];
    if (c == '%') {
      c = text[i + 1];
      if (c >= '0' && c <= '9') c -= '0';
      else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
      else if (c >= 'a' && c <= 'f') c -= 'a' - 10;
      v = c * 16;
      c = text[i + 2];
      if (c >= '0' && c <= '9') c -= '0';
      else if (c >= 'A' && c <= 'F') c -= 'A' - 10;
      else if (c >= 'a' && c <= 'f') c -= 'a' - 10;
      v += c;
      text[j] = v;
      i += 3;
    }
    else {
      text[j] = c;
      i++;
    }
    j++;
  }
  text[j] = '\0';
  return 0;
}

int naominet_error_status_map(int score_server, int code)
{
  switch (code) {
  case -502: case -501: case -500:
  case -401: case -400:
  case -300:
  case -202: case -201: case -200:
  case -101: case -100:
    if (score_server == 0) return -402;
    return -404;
  case -22:
    if (score_server == 0) return -401;
    return -403;
  case -34: case -33: case -32: case -31:
    return -401;
  case -43: case -42: case -41:
    return -403;
  case -21:
  case 0:
    return -102;
  }
  return -102;
}

void naominet_last_error_keep(naominet_job_rec *job)
{
  naominet_last_error.job_state = job->state;
  naominet_last_error.player = job->player;
  naominet_last_error.sock_error = job->sock_error;
  return;
}

naominet_job_rec * naominet_job_alloc(void)
{
  int i;
  naominet_job_rec *job;
  for (i = 0; i < 0x40; i++) {
    naominet_job_rec *jobs = naominet_jobs;
    if (jobs[i].in_use == 0) break;
  }
  if (i >= 0x40) return 0;
  job = &naominet_jobs[i];
  job->in_use = 1;
  return job;
}

void naominet_job_free(naominet_job_rec *job)
{
  job->in_use = 0;
  return;
}

int naominet_job_enqueue(naominet_job_rec *job)
{
  job->next = 0;
  if (naominet_job_queue.tail != 0) {
    naominet_job_queue.tail->next = job;
    job->prev = naominet_job_queue.tail;
  }
  else {
    naominet_job_queue.head = job;
    job->prev = 0;
  }
  naominet_job_queue.tail = job;
  return 0;
}

int naominet_job_unlink(naominet_job_rec *job)
{
  if (!(job->prev == (naominet_job_rec *)0x0)) {
    job->prev->next = job->next;
  }
  else {
    naominet_job_queue.head = 0;
  }
  if (!(job->next == 0)) {
    job->next->prev = job->prev;
  }
  else {
    naominet_job_queue.tail = 0;
  }
  return 0;
}

void http_client_pool_init(char *area, uint count, int unused)
{
  uint i;
  uint conn_words;
  uint heap_size;
  int *send_area;
  int *recv_area;

  http_client_conn_count = count;
  http_client_bytes_fill((uchar *)area,0,count * 0x2074);
  http_client_conns = (http_client_conn_rec *)area;
  conn_words = count * 0x74 / 4;
  send_area = (int *)area + conn_words;
  heap_size = count << 0xc;
  recv_area = (int *)area + conn_words + heap_size / 4;
  http_client_send_heap = block_heap_init((block_heap_rec *)send_area,heap_size);
  http_client_recv_heap = block_heap_init((block_heap_rec *)recv_area,heap_size);
  for (i = 0; i < count; i++) {
    http_client_conns[i].slot = -1;
    http_client_conns[i].state = 0;
    http_client_conns[i].sock = -1;
    http_client_conns[i].connected = 0;
    http_client_conns[i].sock_error = 0;
    http_client_conns[i].start_ticks = 0;
    http_client_conns[i].addr.family = 2;
    http_client_conns[i].addr.port = 0x5000;
    http_client_conns[i].addr.ip = 0;
    http_client_conns[i].limit1 = 0;
    http_client_conns[i].limit2 = 0;
    http_client_conns[i].limit3 = 0;
    http_client_conns[i].send_state = 0;
    http_client_conns[i].sent = 0;
    http_client_conns[i].send_block = 0;
    http_client_conns[i].send_text = 0;
    http_client_conns[i].request = 0;
    http_client_conns[i].request_body = 0;
    http_client_conns[i].recv.state = 0;
    http_client_conns[i].recv.last_count = 0;
    http_client_conns[i].recv.received = 0;
    http_client_conns[i].recv.room = 0x800;
    http_client_conns[i].recv.buf = 0;
    http_client_conns[i].recv.buf_end = 0;
    http_client_conns[i].recv.status_line = 0;
    http_client_conns[i].recv.cursor = 0;
    http_client_conns[i].response = 0;
    http_client_conns[i].response_body = 0;
  }
}

http_client_conn_rec * http_client_conn_alloc(void)
{
  int i;
  http_client_conn_rec *conns = http_client_conns;

  for (i = 0; i < http_client_conn_count; i++) {
    if (conns[i].slot == -1) {
      conns[i].slot = 0;
      conns[i].state = 1;
      return &conns[i];
    }
  }
  return (http_client_conn_rec *)0x0;
}

void http_client_conn_reset(http_client_conn_rec *conn)
{
  conn->slot = -1;
  conn->state = -1;
  conn->sock = -1;
  conn->connected = 0;
  conn->sock_error = 0;
  conn->start_ticks = 0;
  conn->addr.family = 2;
  conn->addr.port = 0x5000;
  conn->addr.ip = 0;
  conn->limit1 = 0;
  conn->limit2 = 0;
  conn->limit3 = 0;
  conn->send_state = 0;
  conn->sent = 0;
  conn->send_block = 0;
  conn->send_text = 0;
  conn->request = 0;
  conn->request_body = 0;
  conn->recv.state = 0;
  conn->recv.last_count = 0;
  conn->recv.received = 0;
  conn->recv.room = 0x800;
  conn->recv.buf = 0;
  conn->recv.buf_end = 0;
  conn->recv.status_line = 0;
  conn->recv.cursor = 0;
  conn->response = 0;
  conn->response_body = 0;
  return;
}

int http_client_url_parse(http_client_url_rec *url, char *text)
{
  char *p;
  char *s;

  if (*text == '\0') {
    return -1;
  }
  if (strlen(text) > 0x100) {
    return -1;
  }
  strncpy(url->url,text,0x100);
  p = strchr(text,':');
  if (p == 0) {
    return -1;
  }
  if ((uint)(p - text) > 0x100) {
    return -1;
  }
  strncpy(url->proto,text,p - text);
  url->proto[p - text] = '\0';
  p = strstr(url->url,"http://");
  if (p == 0) {
    return -1;
  }
  p += 7;
  s = strchr(p,'/');
  if (s != 0) {
    strcpy(url->path,s);
    *s = '\0';
    strcpy(url->host,p);
  }
  else {
    *(short *)url->path = '/';
    strcpy(url->host,p);
  }
  p = strchr(url->host,':');
  if (p != 0) {
    if ((int)atol(p + 1) < 0x1000) {
      url->port = atol(p + 1);
    }
    else {
      url->port = 80;
    }
    *p = '\0';
  }
  else {
    url->port = 80;
  }
  return 0;
}

int http_client_request_line_set(http_client_request_rec *req, uint method, http_client_url_rec *url, int version)
{
  uint v;
  if (req == 0) return -401;
  if (method > 6) return -401;
  if (version == 10) v = 0x302e31;
  else if (version == 0xb) v = 0x312e31;
  else return -401;
  *(uint *)req->version = v;
  *(uint *)(req->version + 4) = 0;
  *(ushort *)(req->version + 8) = 0;
  strncpy(req->method,http_client_method_names[method],10);
  if (strlen(url->path) > 0x100) return -401;
  strncpy(req->path,url->path,0x100);
  return 0;
}

int http_client_header_add(http_client_request_rec *req, uint name_index, char *value)
{
  if (req == (http_client_request_rec *)0x0) {
    return -401;
  }
  if (name_index >= 0x33) {
    return -401;
  }
  if (req->header_count >= 0x10) {
    return -401;
  }
  strncpy(req->names[req->header_count],http_client_header_names[name_index],
          0x100);
  strncpy(req->values[req->header_count],value,0x100);
  req->header_count = req->header_count + 1;
  return 0;
}

int http_client_connect_step(http_client_conn_rec *conn)
{
  int r;
  int st;
  int e;
  uint now;

  switch (conn->state) {
  case 1:
    conn->start_ticks = naominet_ticks_now();
  case 2:
    r = sock_create(2,1,0);
    if (r < 0) {
      r = http_client_sock_error_map(conn,r);
      if (r != -0xcb) break;
      now = naominet_ticks_now();
      if (naominet_ms_between(conn->start_ticks,now) > conn->limit1) {
        conn->sock_error = 0x3c;
        r = -100;
        break;
      }
      st = 2;
    }
    else {
      conn->sock = r;
      st = 3;
    }
    goto set;
  case 3:
    if (conn->limit1 != 0 && conn->limit2 != 0 &&
        conn->limit3 != 0 &&
        (e = sock_op10_three_values(conn->sock,conn->limit1,
                             conn->limit2,conn->limit3)) < 0)
      return http_client_sock_error_map(conn,e);
    st = 4;
    goto set;
  case 4:
    e = sock_connect_start(conn->sock,&conn->addr,0x10);
    if (e < 0) return http_client_sock_error_map(conn,e);
    st = 5;
  set:
    conn->state = st;
    r = 1;
    break;
  case 5:
    r = 1;
    if (sock_call_done(conn->sock) == 1) {
      if (sock_result_take(conn->sock) < 0) {
        conn->sock_error = sock_error_get(conn->sock);
        return -100;
      }
      conn->connected = 1;
      conn->state = 6;
      r = 0;
    }
    break;
  default:
    r = -300;
    break;
  }
  return r;
}

int http_client_send_step(http_client_conn_rec *conn)
{
  int ret;
  http_client_request_rec *req;
  int body_len;
  char *body;
  int n;
  int i;
  int total;
  char *d;
  int block;
  int r;

  ret = 0;
  req = conn->request;
  body = conn->request_body;
  if (((conn->connected != 1) | (req == 0)) || body == 0) {
    return -400;
  }
  switch (conn->send_state) {
  case 0:
    n = strlen(req->version) + strlen(req->method) + strlen(req->path) + 9;
    for (i = 0; i < req->header_count; i++) {
      n += strlen(req->names[i]) + strlen(req->values[i]) + 4;
    }
    n += 2;
    body_len = strlen(body);
    total = body_len + n;
    block = (int)block_heap_alloc(http_client_send_heap, total + 0x101);
    conn->send_block = (char *)block;
    if (block == 0) goto fail;
    conn->send_text = (char *)block;
    d = (char *)block;
    http_client_bytes_fill((uchar *)block, 0, total + 1);
    d += sprintf(d, "%s %s HTTP/%s\r\n", req->method, req->path, req->version);
    i = 0;
    goto test;
  fail:
    return -200;
  header:
      d += sprintf(d, "%s: %s\r\n", req->names[i], req->values[i]);
      i++;
  test:
    if (i < req->header_count) goto header;
    memcpy(d, "\r\n", 3);
    d += 2;
    strncpy(d, body, body_len + 1);
    dimm_mem_write((uint)conn->send_text, naominet_config.board_mem_size - 0x12000,
                 strlen(conn->send_text), 0, 0);
    conn->sent = 0;
    conn->state = 7;
    conn->send_state = 1;
    ret = 1;
    break;
  case 1:
    r = sock_send_from_board_async(conn->sock,
                     naominet_config.board_mem_size + conn->sent - 0x12000,
                     strlen(conn->send_text) - conn->sent);
    if (r < 0) {
      ret = http_client_sock_error_map(conn, r);
      block_heap_free(http_client_send_heap, conn->send_block);
      break;
    }
    conn->send_state = 2;
    ret = 1;
    break;
  case 2:
    if (sock_call_done(conn->sock) == 1) {
      r = sock_result_take(conn->sock);
      if (r < 0) {
        conn->sock_error = sock_error_get(conn->sock);
        block_heap_free(http_client_send_heap, conn->send_block);
        return -100;
      }
      conn->sent += r;
      if ((uint)conn->sent >= strlen(conn->send_text)) {
        conn->send_state = 3;
      }
      else {
        conn->send_state = 1;
      }
    }
    ret = 1;
    break;
  case 3:
    block_heap_free(http_client_send_heap, conn->send_block);
    conn->state = 8;
    conn->send_state = 0;
    conn->sent = 0;
    conn->send_text = 0;
    conn->request = 0;
    conn->request_body = 0;
    ret = 0;
    break;
  }
  return ret;
}

int http_client_recv_step(http_client_conn_rec *conn)
{
  int r;
  int ret;
  http_client_response_rec *response;
  http_client_body_rec *body;
  ret = 0;
  response = conn->response;
  body = conn->response_body;
  if (((conn->connected != 1) | (response == 0)) || body == 0) {
    return -500;
  }
  switch(conn->recv.state) {
  case 0: {
    char *s = (char *)conn + 0x40;
    int n = 0x800;
    r = (int)block_heap_alloc(http_client_recv_heap,n);
    *(int *)(s + 0x1c) = r;
    if (r != 0) {
    http_client_bytes_fill((uchar *)r,0,n);
    *(int *)(s + 0x14) = 0;
    *(int *)(s + 0x20) = *(int *)(s + 0x1c);
    conn->state = 9;
    *(int *)(s + 0xc) = 1;
    goto one;
    }
    ret = -200;
    break;
    }
  case 1:
    r = sock_recv_to_board_async(conn->sock,
                     naominet_config.board_mem_size + conn->recv.received - 0x12000,
                     conn->recv.room);
    if (r < 0) {
      ret = http_client_sock_error_map(conn,r);
      block_heap_free(http_client_recv_heap,conn->recv.buf);
      break;
    }
    conn->recv.state = 2;
    ret = 1;
    break;
  case 2:
    r = sock_call_done(conn->sock);
    if (r != 1) {
      goto one;
    }
    r = sock_result_take(conn->sock);
    if (r < 0) {
      conn->sock_error = sock_error_get(conn->sock);
      block_heap_free(http_client_recv_heap,conn->recv.buf);
      return -100;
    }
    if (conn->recv.room < r) {
      block_heap_free(http_client_recv_heap,conn->recv.buf);
      return -100;
    }
    conn->recv.received = conn->recv.received + r;
    conn->recv.buf_end = conn->recv.buf_end + r;
    conn->recv.room = conn->recv.room - r;
    conn->recv.last_count = r;
    conn->recv.state = 3;
    ret = 1;
    break;
  case 3:
    dimm_mem_read(naominet_config.board_mem_size - 0x12000,(uint)conn->recv.buf,
                 conn->recv.received,0,0);
    if ((r = http_client_status_headers_parse(&conn->recv,response) < 0)) {
      block_heap_free(http_client_recv_heap,conn->recv.buf);
      return -501;
    }
    if (r == 1) {
      conn->recv.state = r;
      return 1;
    }
    if (response->status == 100) {
      conn->recv.received = 0;
      conn->recv.buf_end = conn->recv.buf;
      conn->recv.room = 0x800;
      conn->recv.state = 1;
      return 1;
    }
    conn->recv.state = 4;
    ret = 1;
    break;
  case 4:
    r = http_client_body_read(&conn->recv,response,body);
    if (r < 0) {
      block_heap_free(http_client_recv_heap,conn->recv.buf);
      return -502;
    }
    if (r == 1) {
      conn->recv.state = r;
      return 1;
    }
    conn->recv.state = 5;
  one:
    ret = 1;
    break;
  case 5:
    block_heap_free(http_client_recv_heap,conn->recv.buf);
    conn->state = 10;
    conn->recv.state = 0;
    conn->recv.buf_end = 0;
    conn->recv.room = 0x800;
    ret = 0;
    break;
  }
  return ret;
}

int http_client_conn_close(http_client_conn_rec *conn)
{
  if (conn != 0) {
    if (-1 < conn->sock) {
      sock_close(conn->sock);
    }
    conn->sock = -1;
    conn->connected = 0;
    return 0;
  }
  return -1;
}

int http_client_server_addr_set(http_client_conn_rec *conn, uint addr)
{
  if (!(conn != 0)) {
    return -1;
  }
  conn->addr.ip = addr;
  return 0;
}

int http_client_timeouts_set(http_client_conn_rec *conn, uint limit1, uint limit2, uint limit3)
{
  if (!(conn != 0)) {
    return -1;
  }
  conn->limit1 = limit1;
  conn->limit2 = limit2;
  conn->limit3 = limit3;
  return 0;
}

int http_client_request_set(http_client_conn_rec *conn, http_client_request_rec *request, char *body)
{
  if (!(conn != 0)) {
    return -1;
  }
  conn->request = request;
  conn->request_body = body;
  return 0;
}

int http_client_response_set(http_client_conn_rec *conn, http_client_response_rec *response, http_client_body_rec *body)
{
  if (!(conn != 0)) {
    return -1;
  }
  conn->response = response;
  conn->response_body = body;
  return 0;
}

int http_client_sock_error(http_client_conn_rec *conn)
{
  return conn->sock_error;
}

int http_client_conn_is_open(http_client_conn_rec *conn)
{
  if (conn == 0) return -1;
  if (conn->sock >= 0 && conn->connected == 1) return 0;
  return -1;
}

int http_client_status_headers_parse(http_client_recv_rec *recv, http_client_response_rec *response)
{
  char *end;
  char *p;
  char *start;
  int i;
  int ver;

  i = 0;
  end = strstr(recv->buf,"\r\n\r\n");
  if (end == (char *)0x0) return 1;
  recv->cursor = end + 4;
  p = recv->status_line = recv->buf;
  if (memcmp(p,"HTTP",4) != 0) return -501;
  if (p[5] == '1' && p[6] == '.' && p[7] == '1') ver = 0xb;
  else {
    if (p[5] != '1' || p[6] != '.' || p[7] != '0') return -501;
    ver = 10;
  }
  response->version = ver;
  response->status = (p[9] - '0') * 100;
  response->status += (p[10] - '0') * 10;
  response->status += p[11] - '0';
  start = p + 0xd;
  while (*p != '\r') p++;
  if ((uint)(p - start) > 0x100) return -501;
  strncpy(response->reason,start,p - start);
  response->reason[p - start] = 0;
  while (p != end) {
    if (i > 0xf) return -501;
    while (*p == '\r' || *p == '\n') p++;
    if (*p == ' ' || *p == '\t') return -501;
    start = p;
    while (*p != ':') p++;
    if ((uint)(p - start) > 0x100) return -501;
    strncpy(response->names[i],start,p - start);
    response->names[i][p - start] = 0;
    while (*p == ':' || *p == ' ') p++;
    start = p;
    while (*p != '\r') p++;
    if ((uint)(p - start) > 0x100) return -501;
    strncpy(response->values[i],start,p - start);
    response->values[i][p - start] = 0;
    i++;
  }
  return 0;
}

int http_client_body_read(http_client_recv_rec *recv, http_client_response_rec *response, http_client_body_rec *body)
{
  int r;
  int i;

  for (i = 0; i < 0xf; i++) {
    if (memcmp(response->names[i],"Connection",0xb) == 0) {
      if (http_client_str_case_equal(response->values[i],"close") != 0) {
        return -0x1f5;
      }
      body->conn_close = 1;
    }
  }
  for (i = 0; i < 0xf; i++) {
    if (memcmp(response->names[i],"Content-Length",0xf) == 0) {
      body->content_length = atol(response->values[i]);
      if (body->content_length != 0) {
        r = http_client_body_read_length(recv,body);
        if (r < 0) return -0x1f6;
        return r;
      }
    }
  }
  for (i = 0; i < 0xf; i++) {
    if (memcmp(response->names[i],"Transfer-Encoding",0x12) == 0) {
      if (http_client_str_case_equal(response->values[i],"chunked") == 0) {
        r = http_client_body_read_chunked(recv,body);
        if (r < 0) return -0x1f6;
        return r;
      }
      return -0x1f5;
    }
  }
  r = http_client_body_read_rest(recv,body);
  if (r < 0) return -0x1f6;
  return r;
}

int http_client_body_read_length(http_client_recv_rec *recv, http_client_body_rec *body)
{
  size_t len;

  if (0x1000 < body->content_length) return -200;
  len = strlen(recv->cursor);
  if (len < body->content_length) return 1;
  http_client_bytes_fill((uchar *)body->data,0,0x1000);
  http_client_bytes_copy((uchar *)body->data,(uchar *)recv->cursor,body->content_length);
  body->length = body->content_length;
  return 0;
}

int http_client_chunk_size_parse(http_client_recv_rec *recv, http_client_body_rec *body)
{
  char *s;
  char *e;
  int n;
  int i;
  char c;

  s = recv->cursor;
  e = strstr(s, "\r\n");
  if (e == 0) {
    return -502;
  }
  n = e - s;
  for (; s != e; s++) {
    c = *s;
    if ((((uchar)(c - '0') <= 9) | ((uchar)(c - 'a') <= 5)) == 0 && (uchar)(*s - 'A') > 5) {
      body->chunk_size = -1;
      return -502;
    }
  }
  for (i = 0; i < n; i++, recv->cursor += 1) {
    if ((uchar)(*recv->cursor - '0') <= 9) {
      body->chunk_size += (*recv->cursor - '0') << ((n - i) * 4 - 4);
    }
    else if ((uchar)(*recv->cursor - 'a') <= 5) {
      body->chunk_size += (*recv->cursor - 'W') << ((n - i) * 4 - 4);
    }
    else if (*recv->cursor > '@' && *s <= 'F') {
      body->chunk_size += (*recv->cursor - '7') << ((n - i) * 4 - 4);
    }
  }
  recv->cursor += 2;
  return 0;
}

int http_client_body_read_chunked(http_client_recv_rec *recv, http_client_body_rec *body)
{
  uchar *dst;
  uint rem;
  if (strstr(recv->buf,"0\r\n") == (char *)0x0) {
    return 1;
  }
  dst = (uchar *)body->data;
  rem = 0x1000;
  http_client_bytes_fill(dst,0,rem);
  while (memcmp(recv->cursor,"0\r\n",3) != 0) {
    if (http_client_chunk_size_parse(recv,body) < 0) {
      return -502;
    }
    if (body->chunk_size > rem) {
      return -200;
    }
    http_client_bytes_copy(dst,(uchar *)recv->cursor,body->chunk_size);
    body->length += body->chunk_size;
    dst += body->chunk_size;
    recv->cursor += body->chunk_size;
    rem -= body->chunk_size;
    while (*recv->cursor == '\r' || *recv->cursor == '\n') {
      recv->cursor++;
    }
  }
  return 0;
}

int http_client_body_read_rest(http_client_recv_rec *recv, http_client_body_rec *body)
{
  if ((uint)(recv->buf_end - recv->cursor) > 0x1000) {
    return -200;
  }
  if (recv->last_count != 0) {
    return 1;
  }
  http_client_bytes_fill((uchar *)body->data,0,0x1000);
  http_client_bytes_copy((uchar *)body->data,(uchar *)recv->cursor,
               recv->buf_end - recv->cursor);
  body->length = recv->buf_end - recv->cursor;
  return 0;
}

int http_client_bytes_copy(uchar *dst, uchar *src, int n)
{
  uchar c;
  int i;

  i = 0;
  while (i < n) {
      c = *src;
      src = src + 1;
      *dst = c;
      dst = dst + 1;
      i = i + 1;

  }
  return i;
}

uint http_client_bytes_fill(uchar *dst, uchar value, int n)
{
  int i;

  i = 0;
  for (; i < n; dst = dst + 1, i = i + 1) {
      *dst = (uchar)value;
  }
  return i;
}

int http_client_str_case_equal(char *a, char *b)
{
  char c1;
  char c2;
  if (strlen(a) != strlen(b)) return -1;
  while (*a != 0 && *b != 0) {
    c1 = *a;
    c2 = *b;
    if ((uchar)(c1 - 'A') < 26) c1 = c1 + 32;
    if ((uchar)(c2 - 'A') < 26) c2 = c2 + 32;
    if (c1 != c2) return -1;
    a++;
    b++;
  }
  return 0;
}

int http_client_sock_error_map(http_client_conn_rec *conn, int code)
{
  int r;

  switch(code) {
  case -1:
    conn->sock_error = sock_error_get(conn->sock);
    r = -100;
    break;
  case -2:
    r = -201;
    break;
  case -3:
    r = -202;
    break;
  case -4:
    r = -203;
    break;
  }
  return r;
}

int naominet_poweron_body_build(int pack, naominet_machine_rec *machine, char *body)
{
  size_t len;
  int r;
  int n;
  Bytef zbuf [1024];
  uchar text [1024];
  uLongf zlen [1];

  if (machine->shift_jis == 1) {
    sprintf(body,
    "game_id=%s&ver=%s&serial=%s&ip=%s&firm_ver=%02X%02X&boot_ver=%02X%02X&encode=Shift_JIS\r\n",
                 &machine->game_id,&machine->version,&machine->serial,&machine->ip_text,machine->firm_ver_major,
                 machine->firm_ver_minor,machine->boot_ver_major,
                 machine->boot_ver_minor);
  }
  else {
    sprintf(body,"game_id=%s&ver=%s&serial=%s&ip=%s&firm_ver=%02X%02X&boot_ver=%02X%02X\r\n",
                 &machine->game_id,&machine->version,&machine->serial,&machine->ip_text,machine->firm_ver_major,
                 machine->firm_ver_minor,machine->boot_ver_major,
                 machine->boot_ver_minor);
  }
  if (pack != 0) {
    naominet_bytes_fill(zbuf,0,0x400);
    zlen[0] = 0x400;
    len = strlen(body);
    r = compress(zbuf,zlen,(Bytef *)body,len);
    if (r < 0) {
      return -12;
    }
    naominet_bytes_fill(text,0,0x400);
    n = b64_encode_with_table(text,zbuf,zlen[0],b64_std_alphabet);
    naominet_bytes_fill((uchar *)body,0,0x1000);
    naominet_bytes_copy((uchar *)body,text,n);
  }
  return 0;
}

int naominet_score_body_build(int pack, naominet_score_args_rec *args, naominet_score_option_rec *options, char *body)
{
  Bytef zbuf[0x400];
  Bytef deflated[0x400];
  char b64[0x400];
  char escaped[0x400];
  int counters[16];
  uLongf zlen;
  char *s;
  char *p;
  int r;
  int n;
  int i;
  int j;
  int k;
  int k2;
  int k3;
  p = body;
  if (((uint) (args->kind - 1)) > 4)
  {
    return -11;
  }
  if (args->kind == 2)
  {
    k = args->player;
    args->play_count[k] = args->play_count[k] + 1;
    args->start_ticks[k] = naominet_ticks_now();
    options->er_text[0] = 0;
    options->di_text[0] = 0;
  }
  if (args->kind == 3)
  {
    if (args->er_flag == 1)
    {
      if (r)
      {
        options->er_text[0] = 0x31;
      }
      else
      {
        options->er_text[0] = 0x31;
      }
    }
    else
    {
      options->er_text[0] = 0;
    }
    if (args->di_flag == 1)
    {
      options->di_text[0] = 0x31;
    }
    else
    {
      options->di_text[0] = 0;
    }
  }
  if (args->kind == 4)
  {
    k2 = args->player;
    args->end_ticks[k2] = naominet_ticks_now();
    k3 = args->player;
    args->play_seconds[k3] = args->play_seconds[k3] + (naominet_ms_between(args->start_ticks[k3], args->end_ticks[k3]) / 1000);
    if (args->er_flag == 1)
    {
      options->er_text[0] = 0x31;
    }
    else
    {
      options->er_text[0] = 0;
    }
    if (args->di_flag == 1)
    {
      options->di_text[0] = 0x31;
    }
    else
    {
      options->di_text[0] = 0;
    }
  }
  p = p + sprintf(p, "zk=%d&gi=%s&gv=%s&bn=%s&gs=%d&ti=%s&gt=%d", args->kind, (char *) &args->machine->game_id, (char *) &args->machine->version, (char *) &args->machine->serial, args->play_count[args->player], (char *) &args->machine->ip_text, args->play_seconds[args->player]);
  if (options->hi_number != 0)
  {
    p = p + sprintf(p, "&hi=%d", options->hi_number);
  }
  if (options->si_number != 0)
  {
    p = p + sprintf(p, "&si=%d", options->si_number);
  }
  s = options->kc_text;
  if ((*s) != '\0')
  {
    p = p + sprintf(p, "&kc=%s", s);
  }
  s = options->sc_text;
  if ((*s) != '\0')
  {
    p = p + sprintf(p, "&sc=%s", s);
  }
  s = options->kk_text;
  if ((*s) != '\0')
  {
    p = p + sprintf(p, "&kk=%s", s);
  }
  s = options->ss_text;
  if ((*s) != '\0')
  {
    p = p + sprintf(p, "&ss=%s", s);
  }
  s = options->er_text;
  if ((*s) != '\0')
  {
    p = p + sprintf(p, "&er=%s", s);
  }
  s = options->di_text;
  if ((*s) != '\0')
  {
    p = p + sprintf(p, "&di=%s", s);
  }
  if ((args->kind == 1) && (naominet_config.board_mem_size != 0))
  {
    dimm_pio_read(naominet_config.board_mem_size - 0x10080, (ushort *) counters, 0x40);
    r = naominet_crc_update((char *) counters, 0xffffffff, 0x40);
    sprintf(p, "&db=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", r == 0, (r == 0) ? (counters[0]) : (0), (r == 0) ? (counters[1]) : (0), (r == 0) ? (counters[2]) : (0), (r == 0) ? (counters[3]) : (0), (r == 0) ? (counters[4]) : (0), (r == 0) ? (counters[5]) : (0), (r == 0) ? (counters[6]) : (0), (r == 0) ? (counters[7]) : (0), (r == 0) ? (counters[8]) : (0), (r == 0) ? (counters[9]) : (0));
    if (r != 0)
    {
      naominet_bytes_fill((uchar *) counters, 0, 0x40);
      counters[15] = naominet_crc_update((char *) counters, 0xffffffff, 0x3c);
      dimm_pio_write((ushort *) counters, naominet_config.board_mem_size - 0x10080, 0x40);
    }
  }
  if (pack != 0)
  {
    naominet_bytes_fill(zbuf, 0, 0x400);
    naominet_bytes_fill(deflated, 0, 0x400);
    zlen = 0x400;
    if (compress(zbuf, &zlen, (Bytef *) body, strlen(body)) < 0)
    {
      return -12;
    }
    naominet_bytes_copy(deflated, zbuf + 2, zlen - 6);
    naominet_bytes_fill((uchar *) b64, 0, 0x400);
    naominet_bytes_fill((uchar *) escaped, 0, 0x400);
    n = b64_encode_with_table((uchar *) b64, deflated, zlen - 6, b64_score_alphabet);
    for (i = 0, j = 0; i < n; i++, j++)
    {
      switch (b64[i])
      {
        case '+':
          escaped[j] = '%';
          j++;
          escaped[j] = '2';
          j++;
          escaped[j] = 'B';
          break;

        case '/':
          escaped[j] = '%';
          j++;
          escaped[j] = '2';
          j++;
          escaped[j] = 'F';
          break;

        case '=':
          escaped[j] = '%';
          j++;
          escaped[j] = '3';
          j++;
          escaped[j] = 'D';
          break;

        default:
          escaped[j] = b64[i];
          break;

      }

    }

    naominet_bytes_fill((uchar *) body, 0, 0x1000);
    sprintf(body, "SDF=%s", escaped);
  }
  return 0;
}

int naominet_poweron_request_build(int pack, http_client_request_rec *req, char *body)
{
  http_client_url_rec url;
  char len[12];

  if (http_client_url_parse(&url,"http://naominet.jp/sys/servlet/PowerOn") < 0) return -21;
  if (http_client_request_line_set(req,2,&url,10) != 0) return -21;
  if (http_client_header_add(req,1,"Close") != 0) return -21;
  if (pack != 0) {
    if (http_client_header_add(req,4,"DFI") != 0) return -21;
  }
  if (http_client_header_add(req,0x19,"NAOMI/ver1.0") != 0) return -21;
  if (http_client_header_add(req,0xf,url.host) != 0) return -21;
  if (http_client_header_add(req,0x2d,"application/x-www-form-urlencoded") != 0) return -21;
  sprintf(len,"%d",strlen(body));
  if (http_client_header_add(req,0x28,len) != 0) return -21;
  return 0;
}

int naominet_score_request_build(http_client_request_rec *req, char *body)
{
  http_client_url_rec url;
  char len [12];

  if (http_client_url_parse(&url,"http://sc.naominet.jp/game.php") < 0) return -21;
  if (http_client_request_line_set(req,2,&url,0xb) != 0) return -21;
  if (http_client_header_add(req,0x2d,"application/x-www-form-urlencoded") != 0) return -21;
  if (http_client_header_add(req,0xf,url.host) != 0) return -21;
  sprintf(len,"%d",strlen(body));
  if (http_client_header_add(req,0x28,len) != 0) return -21;
  if (http_client_header_add(req,0,"no-cache") != 0) return -21;
  return 0;
}

int naominet_poweron_reply_check(int packed, http_client_response_rec *response, http_client_body_rec *body, char *out, int out_size)
{
  Bytef buf[1024];
  Bytef zbuf[1024];
  uLongf len[1];
  char *s;
  char *t;
  char *st;
  Bytef *q;
  int n;
  int k;
  int r;

  if (response->status != 200) return -22;
  if (naominet_str_case_equal(response->reason,"OK") != 0) return -22;
  if (packed != 0) {
    n = 0;
    s = body->data;
    q = zbuf;
    naominet_bytes_fill(buf,0,0x400);
    naominet_bytes_fill(q,0,0x400);
    while (strstr(s,"\r\n") != 0) {
      t = strstr(s,"\r\n");
      k = b64_decode_with_table(q,(uchar *)s,t - s,(uchar *)b64_std_alphabet);
      q += k;
      n += k;
      s = t + 2;
      s += *s == ' ';
    }
    t = strchr(s,10);
    n += b64_decode_with_table(q,(uchar *)s,t - s,(uchar *)b64_std_alphabet);
    len[0] = 0x400;
    if (uncompress(buf,len,zbuf,n) != 0) return -13;
  }
  else {
    naominet_bytes_fill(buf,0,0x400);
    naominet_bytes_copy(buf,(uchar *)body->data,strlen(body->data));
  }
  st = strstr((char *)buf,"stat=");
  if (st == 0) return -34;
  st += 5;
  if (*st == '-') {
    st++;
    switch (*st) {
    case '1': r = -31; break;
    case '2': r = -32; break;
    case '3': r = -33; break;
    default: r = -34; break;
    }
    return r;
  }
  naominet_bytes_fill((uchar *)out,0,out_size);
  naominet_bytes_copy((uchar *)out,buf,strlen((char *)buf));
  return 0;
}

int naominet_score_reply_check(int packed, http_client_response_rec *response, http_client_body_rec *body, naominet_score_args_rec *args)
{
  uLong zlen;
  char *s;
  naominet_machine_rec *machine;
  char *u;
  Bytef text [1024];
  Bytef zbuf [1024];
  uLongf text_len [1];

  if (response->status != 200) {
    return -22;
  }
  if (naominet_str_case_equal(response->reason,"OK") != 0) {
    return -22;
  }
  naominet_bytes_fill(text,0,0x400);
  naominet_bytes_fill(zbuf,0,0x400);
  if (packed != 0) {
    u = body->data;
    zlen = b64_decode_with_table(zbuf,(uchar *)u,strlen(u),(uchar *)b64_score_alphabet);
    text_len[0] = 0x400;
    if (uncompress(text,text_len,zbuf,zlen) != 0) {
      return -13;
    }
  }
  else {
    naominet_bytes_fill(text,0,0x400);
    u = body->data;
    naominet_bytes_copy(text,(uchar *)u,strlen(u));
  }
  if (packed != 0) {
    s = strstr((char *)text,"st=");
    if (s == (char *)0x0) {
      return -43;
    }
    s += 3;
    if (memcmp(s,"NG",2) == 0) {
      return -43;
    }
    if (memcmp(s,"OK",2) == 0) {
      s = strstr((char *)text,"&bn=");
      if (s == (char *)0x0) {
        return -41;
      }
      s += 4;
      machine = args->machine;
      if (strncmp(s,(char *)&machine->serial,strlen((char *)&machine->serial)) != 0) {
        return -41;
      }
      s = strstr((char *)text,"&ti=");
      if (s == (char *)0x0) {
        return -42;
      }
      s += 4;
      machine = args->machine;
      if (strncmp(s,(char *)&machine->ip_text,strlen((char *)&machine->ip_text)) != 0) {
        return -42;
      }
    }
  }
  else {
    if (strstr((char *)text,"OK") == (char *)0x0) {
      return -43;
    }
  }
  return 0;
}

void naominet_db_counters_update(int kind, int result)
{
  int counters[16];

  dimm_pio_read(naominet_config.board_mem_size - 0x10080,(ushort *)counters,0x40);
  if (result < 0) {
    if (kind == 2) {
      counters[1]++;
    }
    else if (kind == 3) {
      counters[3]++;
    }
    else if (kind == 4) {
      counters[5]++;
    }
    if (kind >= 2 && kind <= 4) {
      switch (result) {
      case -502: case -501: case -500:
      case -401: case -400:
      case -300:
      case -203: case -202: case -201: case -200:
      case -101: case -100:
        counters[6]++;
        break;
      case -22:
        counters[7]++;
        break;
      case -43: case -42: case -41:
        counters[8]++;
        break;
      case -21:
      case -13: case -12: case -11:
        counters[9]++;
        break;
      }
    }
  }
  else {
    if (kind == 1) {
      naominet_bytes_fill((uchar *)counters,0,0x40);
    }
    else if (kind == 2) {
      counters[0]++;
    }
    else if (kind == 3) {
      counters[2]++;
    }
    else if (kind == 4) {
      counters[4]++;
    }
  }
  counters[15] = naominet_crc_update((char *)counters,0xffffffff,0x3c);
  dimm_pio_write((ushort *)counters,naominet_config.board_mem_size - 0x10080,0x40);
}

int naominet_conn_idle_poll(int conn_word)
{
  int r;
  int ok;
  uint mask[8];
  uint timeout[2];

  if (conn_word == 0) {
    return -1;
  }
  if (((http_client_conn_rec *)conn_word)->sock < 0 || ((http_client_conn_rec *)conn_word)->connected == 0) {
    return -1;
  }
  timeout[0] = 0;
  timeout[1] = 0;
  mask[0] = 0;
  mask[1] = 0;
  mask[2] = 0;
  mask[3] = 0;
  mask[4] = 0;
  mask[5] = 0;
  mask[6] = 0;
  mask[7] = 0;
  mask[(uint)((http_client_conn_rec *)conn_word)->sock >> 5] |= 1 << ((uint)((http_client_conn_rec *)conn_word)->sock & 0x1f);
  r = sock_select(((http_client_conn_rec *)conn_word)->sock + 1,mask,0,0,timeout);
  if (r < 1) {
    ok = (r == -4) | (r == 0);
    if (ok) return 1;
    http_client_conn_close((http_client_conn_rec *)conn_word);
    http_client_conn_reset((http_client_conn_rec *)conn_word);
    return -1;
  }
  if (sock_recv_to_board(((http_client_conn_rec *)conn_word)->sock,
                   naominet_config.board_mem_size - 0x12000,
                   ((http_client_conn_rec *)conn_word)->recv.room) == 0) {
    http_client_conn_close((http_client_conn_rec *)conn_word);
    http_client_conn_reset((http_client_conn_rec *)conn_word);
    return -1;
  }
  return 1;
}

int b64_char_index(uchar c, uchar *table)
{
  uchar i;
  for (i = 0; i < 0x40; i++) {
    if (table[i] == c) return i;
  }
  return -1;
}

int b64_encode_with_table(uchar *dst, uchar *src, int len, char *table)
{
  uchar *d;
  int i;
  int j;
  uchar b[3];

  if (len == 0) {
    return 0;
  }
  d = dst;
  for (i = 0; i < len - 3; i += 3) {
    *d++ = table[(src[0] & 0xfc) >> 2];
    *d++ = table[(src[0] & 3) << 4 | (src[1] & 0xf0) >> 4];
    *d++ = table[(src[1] & 0xf) << 2 | (src[2] & 0xc0) >> 6];
    *d++ = table[src[2] & 0x3f];
    src += 3;
  }
  b[0] = b[1] = b[2] = 0;
  for (j = i; j < len; j++) {
    b[j - i] = *src++;
  }
  *d++ = table[(b[0] & 0xfc) >> 2];
  *d++ = table[(b[0] & 3) << 4 | (b[1] & 0xf0) >> 4];
  *d++ = table[(b[1] & 0xf) << 2 | (b[2] & 0xc0) >> 6];
  *d++ = table[b[2] & 0x3f];
  switch (len - i) {
  case 1:
    d[-1] = d[-2] = '=';
    break;
  case 2:
    d[-1] = '=';
    break;
  }
  return d - dst;
}

int b64_decode_with_table(uchar *dst, uchar *src, int len, uchar *table)
{
  uchar b[4];
  int pos;
  int total;
  int i;
  int n;

  total = 0;
  for (pos = 0; pos < len; pos += 4, src += 4) {
    for (i = 0; i < 4; i++) {
      b[i] = 0;
    }
    for (i = 0; i < 4; i++) {
      if (pos + i >= len) break;
      b[i] = b64_char_index(src[i],table);
      if (b[i] == 0xff) {
        b[i] = 0;
        break;
      }
    }
    n = i - 1;
    if (n < 0) n = 0;
    if (dst != (uchar *)0x0) {
      if (n > 0) {
        *dst = b[0] << 2 | b[1] >> 4;
      }
      if (n > 1) {
        dst[1] = b[1] << 4 | b[2] >> 2;
      }
      if (n > 2) {
        dst[2] = b[2] << 6 | b[3];
      }
      dst = dst + 3;
    }
    total = total + n;
    if (n < 3) break;
  }
  return total;
}
uint naominet_ticks_now(void)
{
  uint ticks;

  ticks = tmu0_elapsed_ticks();
  return ticks;
}

uint naominet_ms_between(uint from, uint to)
{
  uint ticks;
  uint usec;

  ticks = ticks_between(from,to);
  usec = tmu0_ticks_to_usec(ticks);
  return usec / 1000;
}

void naominet_rtc_date_read(rtc_time *date)
{
  struct { ushort year; uchar month; uchar day; uchar hour; uchar minute; uchar second; uchar weekday; uchar moon_age; } t;
  clock_to_calendar(&t.year);
  date->year = t.year;
  date->month = t.month;
  date->day = t.day;
  date->hour = t.hour;
  date->minute = t.minute;
  date->second = t.second;
}

void date_to_seconds_1950(rtc_time *date, uint *seconds)
{
  int tbl[12];
  uint y;
  int d;

  tbl[0] = 0;
  tbl[1] = 0x1f;
  tbl[2] = 0x3b;
  tbl[3] = 0x5a;
  tbl[4] = 0x78;
  tbl[5] = 0x97;
  tbl[6] = 0xb5;
  tbl[7] = 0xd4;
  tbl[8] = 0xf3;
  tbl[9] = 0x111;
  tbl[10] = 0x130;
  tbl[11] = 0x14e;
  y = date->year - 1950;
  d = y * 365 + (y + 2) / 4;
  if (((y + 2) & 3) == 0) {
    if (date->month < 3) d--;
  }
  d = d + tbl[date->month - 1] + date->day - 1;
  *seconds = ((d * 24 + date->hour) * 60 + date->minute) * 60 + date->second;
}

void seconds_1950_to_date(uint seconds, rtc_time *date)
{
  uint days[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  uint y, q, rem, mo;
  uchar *dp;

  date->second = seconds % 60;
  seconds /= 60;
  date->minute = seconds % 60;
  seconds /= 60;
  date->hour = seconds % 24;
  seconds /= 24;
  y = seconds / 365;
  rem = seconds % 365;
  q = y + 1 >> 2;
  if (q <= rem) {
    rem -= q;
    date->year = y + 0x79e;
  } else {
    rem = (rem - (y + 1 >> 2)) + 365;
    date->year = y + 0x79d;
    rem += (date->year & 3) == 0;
  }
  mo = 1;
  if ((date->year & 3) == 0) {
    uint k = 31;
    if (rem >= k) {
      for (mo = 2; mo < 12; mo++) {
        if (rem < days[mo] + 1) break;
      }
    }
  } else {
    for (mo = 1; mo < 12; mo++) {
      if (rem < days[mo]) break;
    }
  }
  date->month = mo;
  dp = &date->day;
  rem -= *(char *)&days[mo - 1];
  *dp = rem + 1;
  if ((date->year & 3) == 0 && (uchar)mo > 2) *dp = rem;
}

void naominet_crc_table_build(void)
{
  int i;
  int j;
  uint c;
  uint poly = 0xedb88320;
  for (i = 0; i < 0x100; i++) {
    c = 0;
    for (j = 0; j < 8; j++) {
      c = (c >> 1) ^ ((((i >> j) ^ ~c) & 1) - 1 & poly);
    }
    naominet_crc_table[i] = c;
  }
}

uint naominet_crc_update(char *data, uint crc, int len)
{
  char *p;
  for (p = data; p < data + len; p++)
    crc = naominet_crc_table[(crc ^ *p) & 0xff] ^ (crc >> 8);
  return crc;
}

char * ip_to_dotted_text(uint ip)
{
  sprintf(ip_dotted_text_buf,"%d.%d.%d.%d",(uchar)ip,(ip & 0xff00) >> 8,(ip & 0xff0000) >> 0x10
               ,ip >> 0x18);
  return ip_dotted_text_buf;
}

int naominet_bytes_copy(uchar *dst, uchar *src, int n)
{
  uchar c;
  int i;

  i = 0;
  for (; i < n; dst = dst + 1, i = i + 1) {
      c = *src;
      src = src + 1;
      *dst = c;
  }
  return i;
}

uint naominet_bytes_fill(uchar *dst, uchar value, int n)
{
  int i;

  i = 0;
  for (; i < n; dst = dst + 1, i = i + 1) {
      *dst = (uchar)value;
  }
  return i;
}

int naominet_str_case_equal(char *a, char *b)
{
  int ca;
  int cb;
  if (strlen(a) != strlen(b)) return -1;
  while (*a != 0 && *b != 0) {
    ca = *a;
    cb = *b;
    if ((uchar)(ca - 'A') < 26) ca = (char)(ca + 32);
    if ((uchar)(cb - 'A') < 26) cb = (char)(cb + 32);
    if (ca != cb) return -1;
    a++;
    b++;
  }
  return 0;
}

void naominet_lock_init(void)
{
  naominet_lock_flag = 0;
  return;
}

int naominet_lock_try(void)
{
  uint s;
  int r;

  s = naominet_irq_mask_all();
  if (naominet_lock_flag != 0) {
    r = -1;
  }
  else {
    naominet_lock_flag = 1;
    r = 0;
  }
  naominet_irq_level_restore(s);
  return r;
}

void naominet_unlock(void)
{
  int s;
  s = naominet_irq_mask_all();
  naominet_lock_flag = 0;
  naominet_irq_level_restore(s);
}

int naominet_irq_mask_all(void)
{
  unsigned int sr;
  unsigned int lvl;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  return lvl;
}

void naominet_irq_level_restore(int level)
{
  unsigned int sr;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  sr = (sr & 0xffffff0f) | ((level & 15) << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
}
