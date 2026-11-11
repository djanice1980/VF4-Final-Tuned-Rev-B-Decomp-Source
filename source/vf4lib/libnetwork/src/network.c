/*
 * vf4lib/libnetwork/src/network.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#define KRPROTO_text_draw_formatted
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char *net_side_suffixes[2]; /* 0x0c187988 */
extern char *net_test_map_keys[5]; /* 0x0c187998 */
extern char *net_test_map_keys2[4]; /* 0x0c1879b0 */
extern char *net_quest_keys[2][2]; /* 0x0c1879c0 */
extern char *net_start_reply_keys[2][25]; /* 0x0c1879d0 */
extern uchar *http_recv_out; /* 0x0c1e3928 */
extern char play_style; /* 0x0c2925cc */
extern uint course_time_total; /* 0x0c2925f8 */
extern ushort net_req_count; /* 0x0c2a0124 */
extern int net_ack_bits; /* 0x0c2a0128 */
extern int net_ticket_get; /* 0x0c2a0134 */
extern uchar net_db_stat; /* 0x0c2a0138 */
extern int net_message_timer; /* 0x0c2a0140 */
extern net_check_log_rec net_check_log; /* 0x0c37d848 */
extern net_game_config_rec net_game_config; /* 0x0c4223c0 */
extern char net_message_text[512]; /* 0x0c422a68 */
extern char net_req_text[2048]; /* 0x0c423184 */
extern int net_auth_result; /* 0x0c423994 */
extern uchar net_hours_flags; /* 0x0c4239a9 */
extern player_chara_pair player_chara; /* 0x0c2926e0 */
extern char side_entry_flags[3]; /* 0x0c370c84 */
extern user_back_data_rec user_back_data; /* 0x0c3744e8 */
extern net_info_t net_info; /* 0x0c37d804 */
extern char net_bookmark_flags[2]; /* 0x0c423180 */
extern char * net_color_keys[2]; /* 0x0c187a98 */
extern char * net_equip_keys[2]; /* 0x0c187aa0 */
extern char * net_sv_item_keys[2]; /* 0x0c187aa8 */
extern char * net_emblem_keys[2]; /* 0x0c187ab0 */
extern int net_month_days[13]; /* 0x0c187ab8 */
extern net_cmd_rec net_cmd_table[8]; /* 0x0c187b00 */
extern uchar net_ready_at_stage; /* 0x0c2a0130 */
extern int net_frame_usec_max; /* 0x0c2a013c */
extern char alab_step_every_frame; /* 0x0c2a016c */
extern int alab_step_countdown; /* 0x0c2a0170 */
extern int net_log_slot_count; /* 0x0c37e3a4 */
extern char * net_log_base; /* 0x0c37e3a8 */
extern char net_play_id[17]; /* 0x0c423998 */
extern char net_select_reply_log[512]; /* 0x0c4239ac */
extern int net_nushi_ranks[2][2]; /* 0x0c423bac */
extern char net_player_active[2]; /* 0x0c423bbc */
extern char net_log_local_buf[32768]; /* 0x0c423bc0 */
extern char net_log_local; /* 0x0c42bbc0 */
extern uchar net_log_slot_cur; /* 0x0c42bbc1 */
extern char net_upload_url[256]; /* 0x0c42bbc4 */
extern alab_work_rec alab_work; /* 0x0c42bd2c */
extern char alab_enabled; /* 0x0c430fb0 */
extern int alab_step_count; /* 0x0c430fb4 */
extern uint alab_step_usec; /* 0x0c430fb8 */
extern credit_copy_rec credit_copy; /* 0x0c370b80 */
extern net_select_rec net_select_scratch; /* 0x0c37d758 */
extern net_select_rec net_select_pending[2]; /* 0x0c37d791 */
extern auth_client_config_rec auth_client_config; /* 0x0c430e54 */
extern auth_client_settings_rec auth_client_settings; /* 0x0c430edc */
extern auth_reply_word_pair auth_reply_words; /* 0x0c430f5c */
extern auth_place_id_rec auth_place_id; /* 0x0c430f88 */

int net_debug_enabled(void)
{
  return 0;
}

void net_reply_release(void)
{
  http_reply_release();
  return;
}

char * net_play_id_get(void)
{
  return net_play_id;
}

char * net_play_id_new(void)
{
  char *id;
  uint games;
  tm now;
  char stamp [29];

  games = user_back_data.bookkeep.games_total % 10000;
  get_now_tm(&now);
  strftime(stamp,0x20,"%y%m%d%H%M%S",&now);
  snprintf(net_play_id,0x11,"%s%04d",stamp,games);
  id = net_play_id_get();
  return id;
}

char * nam_state_name(int state)
{
  char *name;

  switch(state) {
  case 0:
    name = "NAM_NORMAL";
    break;
  case 1:
    name = "NAM_SUSPEND_REQUEST";
    break;
  case 3:
    name = "NAM_REQ_HTTP_AVAILABLE";
    break;
  case 2:
    name = "NAM_RETRY_WAIT_HTTP_AVAILABLE";
    break;
  case 4:
    name = "NAM_RETRY_WAIT_AUTH";
    break;
  case 5:
    name = "NAM_REQ_AUTH";
    break;
  case 6:
    name = "NAM_RETRY_WAIT_GAME_INITIALIZE";
    break;
  case 7:
    name = "NAM_REQ_GAME_INITIALIZE";
    break;
  case 8:
    name = "NAM_ERROR_NOT_RECOVER";
    break;
  default:
    name = "unknown";
  }
  return name;
}

void nam_state_set(int state)
{
  char *name;
  char clock_text[12];
  tm now;
  name = nam_state_name(state);
  net_clock_now(&net_check_log, &now);
  strftime(clock_text, 9, "%H:%M:%S", &now);
  ((int (*)(uint, uint, ...))console_printf)((uint)debug_out, (uint)nam_state_log_format, clock_text, name);
  net_info.nam_state = state;
}

int net_serial_hash(void)
{
  char *serial;
  int hash;

  serial = net_serial_get();
  hash = string_hash_x5(serial);
  return hash;
}

void net_reply_card_id_apply(int side, char *text)
{
  card_image_rec *card;
  uint card_id;

  card = card_image_get(side);
  if (text != 0) {
    card_id = atol(text);
    card_hdr_card_id_set(&card->header,card_id);
  }
  return;
}

int card_quest_hour_get(int side)
{
  card_image_rec *card;
  int hour;

  card = card_image_get(side);
  hour = card_body_quest_limit_hour_get(&card->body);
  return hour;
}

int card_quest_minute_get(int side)
{
  card_image_rec *card;
  int minute;

  card = card_image_get(side);
  minute = card_body_quest_limit_minute_get(&card->body);
  return minute;
}

int card_quest_year_get(int side)
{
  card_image_rec *card;
  int year;

  card = card_image_get(side);
  year = card_body_quest_limit_year_get(&card->body);
  return year;
}

bool card_emblem_rank_is_set(int side)
{
  card_image_rec *card;
  int dan;

  card = card_image_get(side);
  dan = card_body_emblem_dan_get(&card->body);
  int is_set = 0;
  if (dan != 0) is_set = 1;
  return is_set;
}

void net_reply_tenpo_info_apply(int side, char *text)
{
  char *values[2];
  token_list tokens;
  values[0] = 0; values[1] = 0;
  if (text) {
  if (token_list_split(&tokens,text,0x2c) == 0) {
    if (token_list_has_next(&tokens)) {
      values[0] = token_list_next(&tokens);
      if (token_list_has_next(&tokens)) values[1] = token_list_next(&tokens);
    }
    token_list_reset(&tokens);
  }
  net_reply_tenpo_id_apply(side,values[0]);
  net_reply_tenpo_value2_apply(side,values[1]);
  }
}

void net_reply_tenpo_id_apply(int side, char *text)
{
  card_image_rec *card;
  uint tenpo_id;

  card = card_image_get(side);
  if (text != 0) {
    tenpo_id = strtol(text,0,0x10);
    if ((card->body.my_tenpo_id != 0) && (card->body.my_tenpo_id != tenpo_id)) {
      card->body.tenpo_pnt = 0;
    }
    card->body.my_tenpo_id = (short)tenpo_id;
  }
  return;
}

void net_reply_tenpo_value2_apply(int side, char *text)
{
  card_image_rec *card;
  uint value;

  card = card_image_get(side);
  if (text != 0) {
    value = atol(text);
    if (net_nushi_is_on() != 0) {
      net_nushi_rank_set(side,value);
    }
    if (value > 3) value = 0;
    card->body.misc.f.tenpo_value2 = value;
  }
  return;
}

void net_reply_disp_apply(int side, char *text)
{
  card_image_rec *card;
  uint disp;

  card = card_image_get(side);
  if (text != 0) {
    disp = atol(text);
    card->body.misc.f.disp = disp;
  }
  return;
}

void net_reply_clan_name_apply(int side, char *name)
{
  if (name == (char *)0x0) {
    team_name_set((int)(char)side,empty_text);
  }
  else {
    team_name_set((int)(char)side,name);
  }
}

void net_reply_revise_apply(int side, char *text)
{
  int values[2];
  card_image_rec *cards[2];
  cards[side] = card_image_get(side);
  if (text != 0) {
    values[side] = atol(text);
    card_body_syu_b11_set(&cards[side]->body,*(ushort *)&values[side]);
  }
}

void net_game_start_send(uint vs_game)
{
  strbuf_rec *buf;
  int i;
  uchar is_vs_game;
  short cnt;

  is_vs_game = vs_game;
  buf = &net_req_buf;
  strbuf_init(buf,net_req_text,0x800,0,0);
  strbuf_printf(buf,"cmd=start&");
  if (is_vs_game == 0) {
    strbuf_printf(buf,"cpu=1&");
  }
  strbuf_printf(buf,"id=%s&",net_play_id_new());
  strbuf_printf(buf,"serial=%s&",net_serial_get());
  strbuf_printf(buf,"place_id=%04x&",net_check_log.place_id);
  for (i = 0; i < 2; i++) {
    if (card_record_get(i)->in_use != 0) {
      char *name = net_side_suffixes[i];
      card_image_rec *card = card_image_get(i);
      char *entry_flags;
      char code[17] = "0000000000000000";
      card_access_code_make(code,card_image_get(i)->access_key);
      entry_flags = (char *)side_entry_flags;
      if ((entry_flags[i] & 1) == 0) {
        if (is_vs_game != 0) {
          fatal_at("/p71/prj71p/vf4e2/vf4lib/libnetwork/src/network.c",0x29a,
                   "assertion `%s\' failed.\n","!is_vs_game");
        }
      }
      strbuf_printf(buf,"access_code_%s=%s&",name,code);
      strbuf_printf(buf,"card_id_%s=%d&",name,card_hdr_card_id_get(&card->header));
      strbuf_printf(buf,"char_%s=%d&",name,(&player_chara.side0)[i]);
      if (card_money_send_pending(i) != 0) {
        strbuf_printf(buf,"money_%s=%d&",name,card->body.money);
      }
      strbuf_printf(buf,"quest_%s=%d,%d,%d&",name,card_body_quest0_b0_get(&card->body),
                   card->body.quest.b.quest0,card->body.quest.b.quest1);
      net_status_request_note(i,net_play_id_get(),"start");
    }
    else {
      net_status_request_note(i,"0000000000000000","start");
    }
  }
  cnt = net_req_count++;
  strbuf_printf(buf,"cnt=%d",(ushort)cnt);
  net_upload_send(buf);
  strbuf_release_nop(buf);
  net_pr_texts_clear();
}

void net_game_end_send(uint vs_game, int winner, int chain_of_win)
{
  card_record_rec *record;
  card_image_rec *card;
  int i;
  uint ack;
  char *name;
  uchar on[2];
  uint is_vs_game;
  int vs_chara;
  int other;
  strbuf_rec *buf = &net_req_buf;

  is_vs_game = vs_game & 0xff;
  strbuf_init(buf,net_req_text,0x800,0,0);
  for (i = 0; i < 2; i++) {
    record = card_record_get(i);
    if (is_vs_game != 0) {
      on[i] = ((int)card_sides_mask() >> i) & 1;
    }
    else {
      on[i] = record->in_use != 0;
    }
  }
  strbuf_printf(buf,"cmd=end&");
  if (is_vs_game == 0) {
    strbuf_printf(buf,"cpu=1&");
  }
  strbuf_printf(buf,"id=%s&",net_play_id_new());
  strbuf_printf(buf,"serial=%s&",net_serial_get());
  strbuf_printf(buf,"place_id=%04x&",(ushort)net_check_log.place_id);
  strbuf_printf(buf,"free_play=%d&",net_free_play_is_on() != 0);
  strbuf_printf(buf,"ng_play=%d&",jisaku_flag_get());
  if (is_vs_game != 0) {
    strbuf_printf(buf,"winner=%d&",winner);
    strbuf_printf(buf,"chain_of_win=%d&",net_free_play_is_on() ? 0 : chain_of_win);
  }
  ack = 0;
  if (on[0] != 0) {
    ack = net_ack_bits & 5;
  }
  if (on[1] != 0) {
    ack |= net_ack_bits & 10;
  }
  if (ack != 0) {
    net_end_ack_append(buf,ack);
    strbuf_printf(buf,"&");
  }
  net_ack_bits = 0;
  for (i = 0; i < 2; i++) {
    if (on[i] != 0) {
      name = net_side_suffixes[i];
      {
      char code[17] = "0000000000000000";
      card = card_image_get(i);
      card_access_code_make(code,card_image_get(i)->access_key);
      strbuf_printf(buf,"access_code_%s=%s&",name,code);
      strbuf_printf(buf,"card_id_%s=%d&",name,card_hdr_card_id_get(&card->header));
      strbuf_printf(buf,"exp_%s=%d&",name,(uint)card->header.level * 100 + (uint)card->body.exp);
      strbuf_printf(buf,"char_%s=%d&",name,((int *)&player_chara)[i]);
      strbuf_printf(buf,"level_%s=%d&",name,card->header.level);
      strbuf_printf(buf,"win_%s=%d&",name,card->header.all_win);
      strbuf_printf(buf,"lose_%s=%d&",name,card->header.all_lose);
      strbuf_printf(buf,"level_other_%s=%d,%d,%d&",name,card->body.point,card_body_rensho_get(&card->body),card_body_renpai_get(&card->body));
      net_item_params_append(buf,i);
      strbuf_printf(buf,"&");
      net_color_param_append(buf,i);
      strbuf_printf(buf,"&");
      if (card_money_send_pending(i) != 0) {
        strbuf_printf(buf,"money_%s=%d&",name,card->body.money);
      }
      if (card_hdr_recover_get(&card->header) == 0) {
        strbuf_printf(buf,"ranking_point_%s=%d&",name,card->body.rank_pnt);
        strbuf_printf(buf,"my_tenpo_point_%s=%d&",name,card->body.tenpo_pnt);
      }
      strbuf_printf(buf,"quest_%s=%d,%d,%d&",name,card_body_quest0_b0_get(&card->body),card->body.quest.b.quest0,card->body.quest.b.quest1);
      if (is_vs_game != 0) {
        other = i == 0;
        vs_chara = ((int *)&player_chara)[other];
        strbuf_printf(buf,"vs_char_%s=%d&",name,vs_chara);
        net_nickname_params_append(i,buf);
        {
          char *bookmark = (char *)(int)net_bookmark_flags;
          if (*(char *)(bookmark + i) != '\0') {
            strbuf_printf(buf,"bookmark_%s=1&",name);
          }
        }
      }
      else {
        if ('\n' < (char)rank_round) {
          strbuf_printf(buf,"clear_time_%s=%d&",name,course_time_total);
        }
        strbuf_printf(buf,"stage_progress_%s=%d&",name,(int)(char)rank_round);
      }
      net_status_request_note(i,net_play_id_get(),net_end_text);
      }
    }
    else {
      net_status_request_note(i,"0000000000000000",net_end_text);
    }
  }
  strbuf_printf(buf,"cnt=%d",(ushort)net_req_count++);
  net_upload_send(buf);
  strbuf_release_nop(buf);
  net_bookmark_flags[0] = 0;
  net_bookmark_flags[1] = 0;
  if (net_info.service_state == 2) {
    net_info.service_state = 3;
    net_info.end_frames = 0xf0;
  }
  return;
}

void net_game_start_send_cpu(void)
{
  net_game_start_send(0);
  return;
}

void net_game_end_send_cpu(void)
{
  net_game_end_send(0,0,0);
  return;
}

void net_game_start_send_vs(void)
{
  net_game_start_send(1);
  return;
}

void net_game_end_send_vs(int winner, int chain_of_win)
{
  net_game_end_send(1,(int)(char)winner,chain_of_win);
  return;
}

void net_auth_request_start(void)
{
  alab_start_auth();
  net_log_slot_advance();
  net_log_record_fill(&net_check_log,(char *)(http_own_ip_text()),(net_serial_get()),"NO DATA");
  net_log_save(&net_check_log);
  return;
}

bool net_auth_result_poll(void)
{
  net_auth_result = alab_auth_result_get();
  if (net_auth_result == 0) return 0;
  net_auth_result_store(net_auth_result,&net_check_log);
  net_info.error_frames = 0;
  net_log_save(&net_check_log);
  return 1;
}

void net_power_on_send(void)
{
  short cnt;
  char *serial;
  strbuf_rec *buf;

  if (net_info.auth_stat != 1) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libnetwork/src/network.c",0x3b6,"assertion `%s\' failed.\n",
             "NAS_OK == net_info.auth_stat");
  }
  buf = &net_req_buf;
  strbuf_init(buf,net_req_text,0x800,0,0);
  serial = net_serial_get();
  strbuf_printf(buf,"cmd=power_on&serial=%s&place_id=%04x&",serial,net_check_log.place_id);
  cnt = net_req_count;
  net_req_count = cnt + 1;
  strbuf_printf(buf,"cnt=%d",(unsigned short)cnt);
  net_upload_send_authed(buf);
  strbuf_release_nop(buf);
  return;
}

void net_message_request_step(void)
{
  int send;
  short cnt;
  e_mode_main mode;
  int sub;
  char *serial;
  strbuf_rec *buf;

  send = 0;
  mode = mode_get_main();
  if ((mode == MODE_MAIN_OVER) && (sub = mode_get_sub(), sub == 2)) {
    net_message_text[0] = 0;
  }
  mode = mode_get_main();
  if ((mode == MODE_MAIN_ADVERTISE) && (net_message_timer == 0)) {
    if ((user_back_data.advertise_connect != 0) && (pause_frame_held() == 0)) {
      send = 1;
    }
    else {
      net_message_text[0] = 0;
    }
    net_message_timer = 0x1a5e0;
  }
  if (net_message_timer != 0) {
    net_message_timer = net_message_timer + -1;
  }
  if (send) {
    buf = &net_req_buf;
    strbuf_init(buf,net_req_text,0x800,0,0);
    net_message_text[0] = 0;
    serial = net_serial_get();
    cnt = net_req_count++;
    strbuf_printf(buf,"cmd=message&serial=%s&cnt=%d",serial,(ushort)cnt);
    net_upload_send(buf);
    strbuf_release_nop(buf);
  }
}

char * net_message_text_get(void)
{
  return net_message_text;
}

void net_strbuf_post(char *url, strbuf_rec *buf)
{
  http_post_queue(url,(Bytef *)buf->base,buf->write - buf->read);
  return;
}

void net_upload_send(strbuf_rec *buf)
{
  char *url;

  if (net_info.nam_state == 0) {
    url = net_upload_url_build();
    net_strbuf_post(url,buf);
  }
  return;
}

void net_upload_send_authed(strbuf_rec *buf)
{
  int active;
  char *url;

  active = net_service_active();
  if ((active != 0) && (net_info.auth_stat == 1)) {
    url = net_upload_url_build();
    net_strbuf_post(url,buf);
  }
  return;
}

void net_reply_dispatch(void)
{
  int cmd;
  char *body;
  char *reply_text;

  if (http_error_code() != 0) {
    net_info.error_frames = net_info.error_frames + 1;
  }
  if (net_info.error_frames <= 0xb3) net_info.error_level = 0;
  else if (net_info.error_frames <= 599) net_info.error_level = 1;
  else if (net_info.error_frames <= 0x707) net_info.error_level = 2;
  else net_info.error_level = 3;
  if (http_queued_count() > 4) net_info.busy = 1; else net_info.busy = 0;
  if (net_info.error_level == 3) net_info.busy = 1;
  if (http_reply_ready() != 0) {
    reply_text = (char *)http_reply_text();
    if (net_reply_has_stat_header(reply_text,http_reply_header_size()) != 0) net_server_busy = 1; else net_server_busy = 0;
    cmd = net_reply_cmd_find((char *)http_reply_text());
    body = http_header_end_find((char *)http_reply_text());
    switch (cmd) {
    case 2:
      net_power_on_reply_parse(body);
      break;
    case 0:
      net_start_reply_parse(body);
      break;
    case 1:
      net_end_reply_parse(body);
      break;
    case 3:
      net_message_reply_take(body);
      break;
    case 6:
      net_select_reply_handle(body);
      break;
    default:
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libnetwork/src/network.c",0x45c,"assertion `%s\' failed.\n","0");
    case -1:
      net_server_busy = 1;
      net_info.busy = 1;
      break;
    }
    net_reply_release();
    net_info.error_frames = 0;
  }
  if (net_server_busy != 0) net_info.busy = 1;
}

void net_service_state_update(void)
{
  int mode;
  if (net_info.hours_open != 0) {
    net_info.service_state = 1;
  }
  else {
    mode = mode_get_main();
    if (mode == MODE_MAIN_SELECTOR || mode_get_main() == MODE_MAIN_ADVERTISE) {
      net_info.service_state = 0;
    }
    else if (net_info.service_state == 1) {
      if (play_style == 1) net_info.service_state = 0;
      else if (play_style == 2) net_info.service_state = 2;
    }
    else if (play_style == 1 && net_info.service_state != 0) {
      net_info.service_state = 0;
    }
  }
  if (net_info.service_state == 3) {
    if ((int)--net_info.end_frames <= 0) {
      net_info.service_state = 0;
      net_info.end_frames = 0;
    }
  }
}

int net_service_active(void)
{
  if (net_info.service_state != 0) {
    if (net_info.service_state < 4) goto one;
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libnetwork/src/network.c",0x4bb,"assertion `%s' failed.\n","0");
  }
  return 0;
one:
  return 1;
}

void net_hours_flags_clear(void)
{
  net_hours_flags = 0;
  return;
}

void net_hours_flag_set(uint bit)
{
  net_hours_flags |= 1 << bit;
}

void net_hours_flag_clear(uint bit)
{
  net_hours_flags &= ~(1 << bit);
}

char * net_hours_message_get(void)
{
  char *message;
  
  message = (char *)0x0;
  if (!((net_hours_flags & 1) == 0)) {
    message = net_msg_card_closed;
  }
  else {
    if (!((net_hours_flags & 2) == 0)) {
      message = net_msg_card_closing;
    }
    else {
      if (!((net_hours_flags & 4) == 0)) {
        message = net_msg_maintenance;
      }
      else {
        if ((net_hours_flags & 8) != 0) {
          message = net_msg_service_closing;
        }
      }
    }
  }
  return message;
}

void net_service_hours_check(void)
{
  tm now_tm;
  int hour;
  int now;
  uchar *service_state;
  int card_hour;
  int card_minute;
  int weekday;
  int minute;
  int net_hour;
  int net_minute;
  int close;
  uchar open;

  net_hours_flags_clear();
  net_clock_now(&net_check_log,&now_tm);
  hour = now_tm.tm_hour;
  minute = now_tm.tm_min;
  if (hour < 7) {
    hour = hour + 24;
  }
  now = hour * 60 + minute;
  service_state = card_service_state_get();
  if (user_back_data.close_by_weekday == 0) {
    card_hour = user_back_data.close_hour;
    card_minute = user_back_data.close_minute;
  }
  else {
    weekday = now_tm.tm_wday;
    if (hour > 23) {
      weekday = (weekday + 6) % 7;
    }
    card_hour = user_back_data.close_weekday_hour[weekday];
    card_minute = user_back_data.close_weekday_minute[weekday];
  }
  if (card_hour == 8) {
    open = 1;
  }
  else {
    if (card_minute < 15) {
      card_hour = (card_hour + 23) % 24;
      card_minute = card_minute + 45;
    }
    else {
      card_minute = card_minute - 15;
    }
    if (card_hour < 7) {
      card_hour = card_hour + 24;
    }
    close = card_hour * 60 + card_minute;
    if (now > close) {
      open = 0;
      if (*(int *)(service_state + 4) == -1) {
        net_hours_flag_set(0);
      }
      else {
        net_hours_flag_set(1);
      }
    }
    else {
      open = 1;
      if (now + 15 > close) {
        net_hours_flag_set(1);
      }
    }
  }
  if (net_debug_enabled() != 0) {
    if (card_force_open != OFF_ON_OFF) {
      open = 1;
      net_hours_flag_clear(0);
    }
    else if (card_force_close != OFF_ON_OFF) {
      open = 0;
      net_hours_flag_set(0);
    }
  }
  *service_state = open;
  net_hour = 25;
  net_minute = 45;
  if (net_debug_enabled() != 0) {
    net_hour = ((struct { char pad[4]; int close_hour; int close_minute; } *)&net_debug)->close_hour;
    net_minute = ((struct { char pad[4]; int close_hour; int close_minute; } *)&net_debug)->close_minute;
  }
  close = net_hour * 60 + net_minute;
  if (now > close) {
    open = 0;
    net_hours_flag_set(2);
  }
  else {
    open = 1;
    if (now + 15 > close) {
      net_hours_flag_set(3);
    }
  }
  if (net_debug_enabled() != 0) {
    if (force_open != OFF_ON_OFF) {
      open = 1;
      net_hours_flag_clear(2);
    }
    else if (force_close != OFF_ON_OFF) {
      open = 0;
      net_hours_flag_set(2);
    }
  }
  net_info.hours_open = open;
}

void nam_normal_enter(void)
{
  nam_state_set(0);
  return;
}

void nam_suspend_enter(void)
{
  nam_state_set(1);
  return;
}

void nam_retry_wait_http_enter(void)
{
  int debug;
  
  if (!(net_info.http_no_wait == '\0')) {
    net_info.http_wait = 0;
  }
  else {
    net_info.http_wait = 0xe10;
    debug = net_debug_enabled();
    if (debug != 0) {
      net_info.http_wait = 600;
    }
  }
  net_info.http_no_wait = 0;
  nam_state_set(2);
  return;
}

void nam_req_http_enter(void)
{
  nam_state_set(3);
  return;
}

void nam_req_auth_enter(void)
{
  nam_state_set(5);
  net_info.timeout = user_back_data.net_timeout;
  net_info.timeout_left = user_back_data.net_timeout;
  net_auth_request_start();
  return;
}

void nam_retry_wait_auth_enter(void)
{
  uchar hash;
  if (net_info.auth_no_wait != '\0') {
    net_info.auth_wait = 0;
  }
  else {
    hash = net_serial_hash() & 0x1f;
    net_info.auth_wait = hash * 0xf0 + 0x2a30;
    if (net_debug_enabled() != 0) {
      net_info.auth_wait = (hash >> 2) * 0x3c + 600;
    }
  }
  net_info.auth_no_wait = 0;
  nam_state_set(4);
}

void nam_req_game_init_enter(void)
{
  nam_state_set(7);
  if (net_service_active() == 0) {
    nam_suspend_enter();
  }
  else {
    net_info.timeout = user_back_data.net_timeout;
    net_info.timeout_left = user_back_data.net_timeout;
    net_power_on_send();
  }
}

void nam_retry_wait_game_init_enter(void)
{
  uint hash;
  if (net_info.game_init_no_wait != '\0') {
    net_info.game_init_wait = 0;
  }
  else {
    hash = net_serial_hash() & 0xff;
    net_info.game_init_wait = hash * 0x78 + 54000;
    if (net_debug_enabled() != 0) {
      net_info.game_init_wait = (hash >> 5) * 0x3c + 600;
    }
  }
  net_info.game_init_no_wait = 0;
  nam_state_set(6);
}

void nam_error_enter(void)
{
  nam_state_set(8);
  return;
}

void nam_step(void)
{
  int again;

  again = 1;
  if (net_is_on() == 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libnetwork/src/network.c",0x61f,"assertion `%s\' failed.\n",
             "0");
    return;
  }
  while (again) {
    again = 0;
    switch (net_info.nam_state) {
    case 0:
      if (net_service_active() == 0) {
        net_players_active_end();
        nam_suspend_enter();
        again = 1;
      }
      break;
    case 1:
      if (net_service_active() != 0) {
        nam_retry_wait_auth_enter();
        again = 1;
      }
      break;
    case 2:
      if (net_info.http_wait < 1) {
        nam_req_http_enter();
        again = 1;
      }
      else {
        net_info.http_wait--;
      }
      break;
    case 3:
      http_lookup_retry_step();
      if (http_is_ready() != 0) {
        net_info.auth_no_wait = 1;
        nam_retry_wait_auth_enter();
        again = 1;
      }
      else {
        nam_retry_wait_http_enter();
        again = 1;
      }
      break;
    case 5:
      if (net_info.timeout_left < 1) {
        net_info.auth_stat = 2;
        net_log_save(&net_check_log);
        alab_step_every_frame_off();
        nam_retry_wait_auth_enter();
        again = 1;
      }
      else {
        net_info.timeout_left--;
        if (net_auth_result_poll()) {
          net_log_status_write(net_is_on(),system_param_byte0_get(),
                       net_info.timeout - net_info.timeout_left,
                       net_info.timeout);
          alab_step_every_frame_off();
          if (net_info.auth_stat == 1) {
            net_info.game_init_no_wait = 1;
            nam_retry_wait_game_init_enter();
            again = 1;
          }
          else {
            nam_error_enter();
            again = 1;
          }
        }
      }
      break;
    case 4:
      if (net_info.auth_wait < 1) {
        alab_step_every_frame_on();
        if ((int)alab_ready_check() > 0) {
          nam_req_auth_enter();
          again = 1;
        }
      }
      else {
        net_info.auth_wait--;
        if (net_info.auth_wait < 3) {
          alab_step_every_frame_on();
        }
      }
      break;
    case 7:
      if (net_service_active() == 0) {
        nam_suspend_enter();
        again = 1;
      }
      else if (net_info.timeout_left < 1) {
        nam_retry_wait_game_init_enter();
        again = 1;
      }
      else {
        net_info.timeout_left--;
      }
      break;
    case 6:
      if (net_service_active() == 0) {
        nam_suspend_enter();
        again = 1;
      }
      else if (net_info.game_init_wait < 1) {
        nam_req_game_init_enter();
        again = 1;
      }
      else {
        net_info.game_init_wait--;
      }
      break;
    case 8:
      break;
    default:
      fatal_at("/p71/prj71p/vf4e2/vf4lib/libnetwork/src/network.c",0x6cb,"unknown NAM (%d)\n",
               net_info.nam_state);
      nam_error_enter();
      again = 1;
    }
  }
}

void net_power_on_reply_parse(char *body)
{
  char *config = (char *)&net_game_config;
  uint i;
  short *rates;
  token_list tokens;
  form_map form;

  net_info.game_config_received = 0;
  if (form_map_parse(&form,body) != 0) return;
  form_map_copy_out(&form,net_form_key_rare,config,0x40);
  form_map_copy_out(&form,"syuchu",config + 0x50,0x20);
  form_map_copy_out(&form,"kumite",config + 0x7c,0x400);
  form_map_copy_out(&form,"nushi",config + 0x5a8,0x10);
  form_map_copy_out(&form,net_form_key_masa,config + 0x5bc,0x10);
  form_map_copy_out(&form,"ryuko",config + 0x5d0,0x10);
  form_map_copy_out(&form,"struggle_revise",config + 0x5e4,0x80);
  form_map_copy_out(&form,"nickname_revise",config + 0xd30,0x80);
  net_ticket_get = form_map_uint(&form,"ticket_get");
  net_db_stat = form_map_uint(&form,"db_stat");
  form_map_clear(&form);
  if (net_db_stat_get() != 1) return;
  if (token_list_split(&tokens,config,0x2c) == 0) {
    for (i = 0; i < 4; i++) *(int *)(config + i * 4 + 0x40) = 0;
    for (i = 0; i < 4 && token_list_has_next(&tokens); i++)
      *(int *)(config + i * 4 + 0x40) = atol(token_list_next(&tokens));
    token_list_reset(&tokens);
  }
  if (token_list_split(&tokens,config + 0x50,0x2c) == 0) {
    for (i = 0; i < 3; i++) *(int *)(config + i * 4 + 0x70) = 0;
    for (i = 0; i < 3 && token_list_has_next(&tokens); i++)
      *(int *)(config + i * 4 + 0x70) = atol(token_list_next(&tokens));
    token_list_reset(&tokens);
  }
  if (token_list_split(&tokens,config + 0x7c,0x2c) == 0) {
    int value;
    uint j;
    for (i = 0; i < 0x4b; i++) *(int *)(config + i * 4 + 0x47c) = 0;
    for (j = 0; j < 0x4b && token_list_has_next(&tokens); j++) {
      value = atol(token_list_next(&tokens));
      if (value == 0) break;
      *(int *)(config + j * 4 + 0x47c) = value;
    }
    token_list_reset(&tokens);
  }
  *(int *)(config + 0x5b8) = 0;
  if (memcmp(config + 0x5a8,"1",2) == 0) *(int *)(config + 0x5b8) = 1;
  *(int *)(config + 0x5cc) = 0;
  if (memcmp(config + 0x5bc,"1",2) == 0) *(int *)(config + 0x5cc) = 1;
  *(int *)(config + 0x5e0) = 0;
  if (memcmp(config + 0x5d0,"1",2) == 0) *(int *)(config + 0x5e0) = 1;
  if (token_list_split(&tokens,config + 0x5e4,0x2c) == 0) {
    for (i = 0; i < 0x11; i++) *(int *)(config + i * 4 + 0x664) = 0;
    for (i = 0; i < 0x11 && token_list_has_next(&tokens); i++)
      *(int *)(config + i * 4 + 0x664) = atol(token_list_next(&tokens));
    token_list_reset(&tokens);
  }
  if (token_list_split(&tokens,config + 0xd30,0x2c) == 0) {
    rates = (short *)(config + 0xdb0);
    for (i = 0; i < 8; i++) rates[i] = 0;
    for (i = 0; i < 7; i++) {
      if (!token_list_has_next(&tokens)) break;
      rates[i] = atol(token_list_next(&tokens));
    }
    token_list_reset(&tokens);
    nickname_rates_set(rates);
  }
  net_info.game_config_received = 1;
  net_players_active_clear();
  nam_normal_enter();
}

void net_test_st(void)
{
  token_list tokens;
  ((int (*)())console_printf)(http_recv_out,net_test_title_format,"net_test_st");
  token_list_split(&tokens,"25,352,32768",0x2c);
  while (token_list_has_next(&tokens) != 0) {
    ((int (*)())console_printf)(http_recv_out,&nam_state_log_format[4],token_list_next(&tokens));
  }
  token_list_reset(&tokens);
}

void net_test_map(void)
{
  uint i;
  form_map form;

  ((int (*)())console_printf)(http_recv_out,net_test_title_format,"net_test_map");
  form_map_parse(&form,"id=0123&handle=&name=%2B");
  for (i = 0; i < 5; i++) {
    ((int (*)())console_printf)(http_recv_out,"[%s]=[%s]\n",net_test_map_keys[i],form_map_find(&form,net_test_map_keys[i]));
  }
  form_map_clear(&form);
  form_map_parse(&form,
               "a0=0&a1=1&a2=2&a3=3&a4=4&a5=5&a6=6&a7=7&a8=8&a9=9&aa=10&ab=11&ac=12&ad=13&ae=14&af=15&ag=16&ah=17&ai=18&aj=19&ak=20&al=21&am=22&an=23&ao=24&ap=25&aq=26&ar=27&as=28&at=29&au=30&av=31&aw=32&ax=33&ay=34&az=35"
              );
  for (i = 0; i < 4; i++) {
    ((int (*)())console_printf)(http_recv_out,"[%s]=[%s]\n",net_test_map_keys2[i],form_map_find(&form,net_test_map_keys2[i]));
  }
  form_map_clear(&form);
  return;
}

void net_reply_quest_apply(form_map *form, uint side)
{
  typedef struct { char *data_key; char *clear_key; } quest_keys;
  token_list tokens;
  int values[10];
  card_image_rec *card;
  card_body_rec *body;
  char *text;
  int level;
  rob_rec *rob;
  quest_keys *keys;
  char **clear_key;
  body = &card_image_get(side)->body;
  card = card_image_get(side);
  if (card_body_quest0_b0_get(body) == 1) return;
  keys = (quest_keys *)&net_quest_keys + side; clear_key = &keys->clear_key;
  if (form_map_uint(form,*clear_key) != 0) {
    card_body_quest0_b0_set(body,0);
    return;
  }
  text = (char *)form_map_find(form,((quest_keys *)&net_quest_keys)[side].data_key);
  if (text == 0) return;
  if (token_list_split(&tokens,text,0x2c) != 0) return;
  text = 0;
  while (token_list_has_next(&tokens)) {
    values[(int)text] = atol(token_list_next(&tokens));
    text++;
  }
  token_list_reset(&tokens);
  card_body_quest_category_set(body,(ushort)values[0]);
  card_body_quest0_b3_set(body,(ushort)values[1]);
  card_body_quest_limit_year_set(body,(uchar)values[5]);
  card_body_quest_limit_month_set(body,(ushort)values[6]);
  card_body_quest_limit_day_set(body,(ushort)values[7]);
  card_body_quest_limit_hour_set(body,(uchar)values[8]);
  card_body_quest_limit_minute_set(body,(uchar)values[9]);
  card_body_quest0_b0_set(body,1);
  if (quest_kind_valid(side) == 0) {
    card_body_quest0_b3_set(body,0);
    card_body_quest_category_set(body,0);
    card_body_quest0_b0_set(body,3);
  }
  if (card_body_quest_category_get(body) == 1 && (quest_kind_get(side) == 12 || quest_kind_get(side) == 13)) {
    if (card_hdr_quest_count_get(&card->header) == 0) card_hdr_quest_count_set(&card->header,15);
    card_body_quest_param_a_set(body,63);
    card_body_quest_param_b_set(body,15);
    card_body_quest_param_c_set(body,1);
  }
  else {
    if (card_hdr_quest_count_get(&card->header) == 0) card_hdr_quest_count_set(&card->header,(uchar)values[4]);
    card_body_quest_param_a_set(body,values[2] == 100 ? 0 : (ushort)values[2]);
    card_body_quest_param_b_set(body,values[3] == 16 ? 0 : (ushort)values[3]);
    card_body_quest_param_c_set(body,values[4] == 100 ? 0 : (ushort)values[4]);
  }
  if (card_body_quest_category_get(body) != 2) return;
  rob = rob_get(side & 0xff);
  level = rob_card_level_pick(side,1);
  if (level < 0) return;
  rob_card_mark_set(rob,level);
}

void net_start_reply_parse(char *body)
{
  typedef struct {
    char *ring_name;
    char *clan_name;
    char *equip;
    char *sv_item;
    char *emblem;
    char *color;
    char *leader;
    char *money;
    char *chat[4];
    char *card_id;
    char *tenpo_info;
    char *disp;
    char *user;
    char *foul;
    char *revise;
    char *pr;
    char *nickname_lv;
    char *nickname_pt;
    char *nickname_para;
    char *ranking_point;
    char *my_tenpo_point;
    char *tec;
  } start_reply_values;
  char id[17];
  form_map form;
  start_reply_values values[2];
  card_record_rec *records[2];
  card_image_rec *cards[2];
  uint i;
  start_reply_values *values_p;

  records[0] = card_record_get(0);
  records[1] = card_record_get(1);
  cards[0] = card_image_get(0);
  cards[1] = card_image_get(1);
  memset(id, 0, 17);
  for (i = 0; i < 2; i++) {
    if (records[i]->in_use != 0) {
      cards[i]->header.flags |= 4;
    }
  }
  for (i = 0, values_p = values; i < 2; i++) {
    void *p = values_p + i;
    memset(p, 0, 100);
  }
  if (form_map_parse(&form,body) == 0) {
    form_map_copy_out(&form,net_form_key_id,id,0x11);
    for (i = 0; i < 2; i++) {
      start_reply_values *v = &values[i];
      if (records[i]->in_use != 0) {
        char **keys = net_start_reply_keys[i];
        v->ring_name = (char *)form_map_find(&form,keys[0]);
        v->clan_name = (char *)form_map_find(&form,keys[1]);
        v->equip = (char *)form_map_find(&form,keys[2]);
        v->sv_item = (char *)form_map_find(&form,keys[3]);
        v->emblem = (char *)form_map_find(&form,keys[4]);
        v->color = (char *)form_map_find(&form,keys[5]);
        v->leader = (char *)form_map_find(&form,keys[6]);
        v->money = (char *)form_map_find(&form,keys[7]);
        v->chat[0] = (char *)form_map_find(&form,keys[8]);
        v->chat[1] = (char *)form_map_find(&form,keys[9]);
        v->chat[2] = (char *)form_map_find(&form,keys[10]);
        v->chat[3] = (char *)form_map_find(&form,keys[11]);
        v->card_id = (char *)form_map_find(&form,keys[12]);
        v->tenpo_info = (char *)form_map_find(&form,keys[13]);
        v->disp = (char *)form_map_find(&form,keys[14]);
        v->user = (char *)form_map_find(&form,keys[15]);
        v->foul = (char *)form_map_find(&form,keys[16]);
        v->revise = (char *)form_map_find(&form,keys[17]);
        v->pr = (char *)form_map_find(&form,keys[18]);
        v->nickname_lv = (char *)form_map_find(&form,keys[19]);
        v->nickname_pt = (char *)form_map_find(&form,keys[20]);
        v->nickname_para = (char *)form_map_find(&form,keys[21]);
        v->ranking_point = (char *)form_map_find(&form,keys[22]);
        v->my_tenpo_point = (char *)form_map_find(&form,keys[23]);
        v->tec = (char *)form_map_find(&form,keys[24]);
      }
      if (mode_get_main() == MODE_MAIN_GAME && strcmp(id,net_play_id_get()) == 0) {
        net_reply_timer_set(0xb4);
        net_db_stat = form_map_uint(&form,"db_stat");
        net_status_reply_note(0,0xb4,(uint)net_db_stat,id);
        net_status_reply_note(1,0xb4,(uint)net_db_stat,id);
        if (net_db_stat == 1) {
          net_reply_quest_apply(&form,i);
          net_reply_user_apply(i,v->user);
          net_reply_foul_apply(i,v->foul);
          net_reply_card_id_apply(i,v->card_id);
          net_reply_tenpo_info_apply(i,v->tenpo_info);
          net_reply_clan_name_apply(i,v->clan_name);
          net_reply_disp_apply(i,v->disp);
          net_reply_ring_name_apply(i,v->ring_name);
          net_sv_item_receive(i,v->sv_item);
          if (0 < net_equip_receive(i,v->equip)) {
            if (i == 0) {
              net_ack_bits = net_ack_bits | 4;
            }
            else if (i == 1) {
              net_ack_bits = net_ack_bits | 8;
            }
            else {
              fatal_at("/p71/prj71p/vf4e2/vf4lib/libnetwork/src/network.c",0x921,
                       "assertion `%s\' failed.\n","0");
            }
          }
          net_emblem_receive(i,v->emblem);
          net_reply_color_apply(i & 0xff,v->color);
          card_money_reply_apply(i,v->money);
          net_reply_revise_apply(i,v->revise);
          net_reply_chat_apply(i,0,v->chat[0]);
          net_reply_chat_apply(i,1,v->chat[1]);
          net_reply_chat_apply(i,2,v->chat[2]);
          net_reply_chat_apply(i,3,v->chat[3]);
          net_pr_text_set(i,v->pr);
          net_nickname_reply_store(i,v->nickname_lv,v->nickname_pt,v->nickname_para);
          net_points_reply_store(i,v->ranking_point,v->my_tenpo_point);
          net_ranking_points_to_card(i);
          net_tec_flags_store(i,v->tec);
        }
      }
    }
    form_map_clear(&form);
  }
}

void net_reply_chat_apply(int side, int slot, char *text)
{
  int size[2];
  if (text != 0) {
    text_lines_measure(text,&size[0],&size[1]);
    player_message_text_set((char)side,(char)slot,text,size[0],size[1]);
  }
  else {
    player_message_text_set((char)side,(char)slot,empty_text,0,0);
  }
}

void net_end_reply_parse(char *body)
{
  int result;
  char *play_id;
  form_map form;
  char id [20];

  result = form_map_parse(&form,body);
  if (result == 0) {
    form_map_copy_out(&form,net_form_key_id,id,0x11);
    play_id = net_play_id_get();
    result = strcmp(id,play_id);
    if (result == 0) {
      net_reply_timer_set(0xb4);
      net_db_stat = form_map_uint(&form,"db_stat");
      net_status_reply_note(0,0xb4,(uint)net_db_stat,id);
      net_status_reply_note(1,0xb4,(uint)net_db_stat,id);
    }
    form_map_clear(&form);
  }
  if (net_info.service_state == 3) {
    net_info.service_state = 0;
    net_info.end_frames = 0;
  }
  return;
}

void net_message_reply_take(char *text)
{
  if ((text == (char *)0x0) || (*text == '\0')) {
    net_message_text[0] = 0;
  }
  else {
    snprintf(net_message_text,0x200,"%s",text);
  }
  return;
}

void net_reply_color_apply(uint side, char *text)
{
  int i;
  uint j;
  uchar pl;
  ushort colors[20];
  token_list tokens;
  card_image_rec *card;

  pl = side;
  card = card_image_get(pl);
  if (text == 0) return;
  for (j = 0; j < 20; j++) colors[j] = 0;
  if (token_list_split(&tokens,text,0x2c) == 0) {
    for (i = 0; token_list_has_next(&tokens) != 0; i++) {
      colors[i] = atol(token_list_next(&tokens));
    }
    token_list_reset(&tokens);
  }
  if (pl == 0) {
    net_ack_bits = net_ack_bits | 4;
  }
  else {
    net_ack_bits = net_ack_bits | 8;
  }
  costume_colors_apply(pl,0,colors[0],colors[1],colors[2],colors[3],colors[8],colors[9]);
  costume_colors_apply(pl,1,colors[4],colors[5],colors[6],colors[7],colors[10],colors[11]);
  card->body.misc.f.net_colors = 1;
}

void net_reply_ring_name_apply(byte side, char *name)
{
  card_image_rec *card = card_image_get(side);
  if (name != 0) {
    if (card_body_name_compare(&card->body,name) != 0) {
      card_body_name_set(&card->body,name);
      if (side == 0) {
        net_ack_bits = net_ack_bits | 1;
      }
      else {
        net_ack_bits = net_ack_bits | 2;
      }
    }
  }
}

void net_ranking_points_to_card(int side)
{
  card_image_rec *card;
  int recover;
  card_body_rec *body;
  char *points;

  card = card_image_get(side);
  recover = card_hdr_recover_get(&card->header);
  if (recover != 0) {
    body = &card->body;
    points = (char *)&net_game_config + side * 2;
    card_body_tenpo_pnt_set(body,*(ushort *)(points + 0xd2c));
    card_body_rank_pnt_set(body,*(ushort *)(points + 0xce8));
    card_hdr_recover_clear(&card->header);
  }
}

void net_reply_user_apply(int side, char *text)
{
  card_image_rec *card;
  int user_class;

  card = card_image_get(side);
  if (text == 0) {
    card->body.syu.f.user_class = 0;
  }
  else {
    user_class = atol(text);
    if (user_class == 1) {
      card->body.syu.f.user_class = 1;
    }
    else {
      user_class = atol(text);
      if (user_class == 2) {
        card->body.syu.f.user_class = 2;
      }
    }
  }
}

void net_reply_foul_apply(int side, char *text)
{
  card_image_rec *card;
  card = card_image_get(side);
  if (text == 0) {
    card->body.misc.f.foul_first = 0;
    card->body.misc.f.foul_second = 0;
  }
  else {
    if ((card->body.misc.w & 0x800) != 0) {
      card->body.misc.f.foul_second = 1;
    }
    card->body.misc.f.foul_first = 1;
  }
}

int net_hex_digit_value(int digit)
{
  char c = digit;
  short value = 0;
  if ((uchar)(c - '0') <= 9) value = c - '0';
  else if ((uchar)(c - 'a') <= 5) value = c - 'a' + 10;
  else if ((uchar)(c - 'A') <= 5) value = c - 'A' + 10;
  return value;
}

void net_end_ack_append(strbuf_rec *buf, uint ack)
{
  int n;
  n = 0;
  if (ack != 0) {
    strbuf_printf(buf,net_ack_prefix);
    if ((ack & 4) != 0) {
      if (n != 0) strbuf_printf(buf,net_list_separator);
      strbuf_printf(buf,"color_1p");
      n++;
    }
    if ((ack & 4) != 0) {
      if (n != 0) strbuf_printf(buf,net_list_separator);
      strbuf_printf(buf,"equip_1p");
      net_equip_ack_count++;
      n++;
    }
    if ((ack & 8) != 0) {
      if (n != 0) strbuf_printf(buf,net_list_separator);
      strbuf_printf(buf,"color_2p");
      n++;
    }
    if ((ack & 8) != 0) {
      if (n != 0) strbuf_printf(buf,net_list_separator);
      strbuf_printf(buf,"equip_2p");
      net_equip_ack_count++;
      n++;
    }
  }
}

void net_color_param_append(strbuf_rec *buf, int side)
{
  card_image_rec *card;
  uint color;
  int color1p_0, color1p_1, color1p_2, color1p_3, color2p_0, color2p_1, color2p_2, color2p_3;
  int hair1p, eye1p, hair2p, eye2p;

  card = card_image_get(side);
  color = card->body.color_1p.w;
  color1p_0 = color & 0x1f;
  color1p_1 = color >> 5 & 0x1f;
  color1p_2 = color >> 10 & 0x1f;
  color1p_3 = color >> 15 & 0x1f;
  color = card->body.color_2p.w;
  color2p_0 = color & 0x1f;
  color2p_1 = color >> 5 & 0x1f;
  color2p_2 = color >> 10 & 0x1f;
  color2p_3 = color >> 15 & 0x1f;
  hair1p = costume_hair_color_get(side,0);
  eye1p = costume_eye_color_get(side,0);
  hair2p = costume_hair_color_get(side,1);
  eye2p = costume_eye_color_get(side,1);
  strbuf_printf(buf,"color_%dp=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",side + 1,color1p_0,color1p_1,color1p_2,color1p_3,color2p_0,color2p_1,color2p_2,color2p_3,hair1p,eye1p,hair2p,eye2p);
}

void net_item_params_append(strbuf_rec *buf, int side)
{
  card_image_rec *card;
  int n;
  int i;
  int item;
  int curse;
  int byte_index;
  int bit;
  card = card_image_get(side);
  strbuf_printf(buf,"item_equip_%dp=%02x%02x%02x%02x%02x%02x%02x%02x&",side + 1,
               card->body.equip_1p[0],card->body.equip_1p[1],
               card->body.equip_1p[2],card->body.equip_1p[3],
               card->body.equip_2p[0],card->body.equip_2p[1],
               card->body.equip_2p[2],card->body.equip_2p[3]);
  strbuf_printf(buf,"item_have_%dp=",side + 1);
  n = 0;
  for (i = 0; i < 0x40; i++) {
    byte_index = i / 8; bit = i % 8;
    if ((card->header.item_have[byte_index] >> bit) & 1) {
      item = item_from_bit(side,i & 0xff);
      if (item >= 0) {
        if (n != 0) strbuf_printf(buf,net_list_separator);
        strbuf_printf(buf,"%d",item);
        n++;
      }
    }
  }
  strbuf_printf(buf,"&");
  curse = card->body.curse.b.curse & 0xf;
  curse |= curse << 4;
  strbuf_printf(buf,"item_curse_%dp=%d",side + 1,curse);
}

int net_reply_color_parse(char *text)
{
  uint i;
  token_list tokens;
  int mask;
  char *token;

  mask = -1;
  if (text == 0) return 0;
  if (token_list_split(&tokens,text,0x2c) == 0) {
    for (i = 0; i < 12; i++) {
      if (token_list_has_next(&tokens) == 0) break;
      token = (char *)token_list_next(&tokens);
      net_select_scratch.color[i] = atol(token);
    }
    if (i == 12) mask = 0x20;
    token_list_reset(&tokens);
  }
  return mask;
}

int net_reply_equip_parse(char *text)
{
  int mask;
  token_list tokens;
  int value;
  char *token;
  uint length;
  uint i;
  char *list;
  list = text;
  mask = -1;
  if (list == 0) return 0;
  if (token_list_split(&tokens, list, 0x2c) == 0) {
    if (token_list_has_next(&tokens)) {
      token = (char *)token_list_next(&tokens);
      length = strlen(token);
      for (i = 0; i < 8; i++) {
        if (i * 2 < length) {
          sscanf(token + i * 2, net_hex_byte_format, &value);
          net_select_scratch.equip[i] = value;
        } else {
          net_select_scratch.equip[i] = 0;
        }
      }
      mask = 2;
    } else {
      mask = -1;
    }
    token_list_reset(&tokens);
  }
  net_equip_parse_count = net_equip_parse_count + 1;
  return mask;
}

uint net_reply_sv_item_parse(char *text)
{
  uint mask;
  int count;
  token_list tokens;
  char *token;

  mask = 0xffffffff;
  if (text == 0) return 0;
  if (token_list_split(&tokens,text,0x2c) == 0) {
    memset(net_select_scratch.items,0,0x10);
    for (count = 0; token_list_has_next(&tokens); count++) {
      token = (char *)token_list_next(&tokens);
      net_select_scratch.items[count] = atol(token);
    }
    mask = -(uint)(count == 0) | 4;
    token_list_reset(&tokens);
  }
  net_sv_item_parse_count = net_sv_item_parse_count + 1;
  return mask;
}

int net_reply_emblem_parse(char *text)
{
  int result;
  int mask;
  char *token;
  token_list tokens;

  mask = 0xffffffff;
  if (text == 0) return 0;
  result = token_list_split(&tokens,text,0x2c);
  if (result == 0) {
    mask = 0;
    result = token_list_has_next(&tokens);
    if (result != 0) {
      token = (char *)token_list_next(&tokens);
      net_select_scratch.emblem = strtol(token,0,0x10);
      mask = 8;
      net_emblem_parse_count = net_emblem_parse_count + 1;
      result = token_list_has_next(&tokens);
      if (result != 0) {
        token = (char *)token_list_next(&tokens);
        net_select_scratch.emblem_dan = strtol(token,0,0x10);
        mask = 0x18;
      }
    }
    token_list_reset(&tokens);
  }
  return mask;
}

void net_card_reply_apply(int side, net_select_rec *select)
{
  card_image_rec *card;
  uint item;
  int i;

  card = card_image_get(side);
  i = 0;
  if ((select->mask & 4) != 0) {
    while (1) {
      item = select->items[i];
      if (item == 0) break;
      item_give_carded(side,item);
      i = i + 1;
    }
  }
  if ((select->mask & 8) != 0) {
    card_body_emblem_set(&card->body,(uint)select->emblem);
  }
  if ((select->mask & 0x10) != 0) {
    card_body_emblem_dan_set(&card->body,(uint)select->emblem_dan);
  }
  if ((select->mask & 2) != 0) {
    item_equip_if_flagged(side,0,0,(uint)select->equip[0]);
    item_equip_if_flagged(side,0,1,(uint)select->equip[1]);
    item_equip_if_flagged(side,0,2,(uint)select->equip[2]);
    item_equip_if_flagged(side,0,3,(uint)select->equip[3]);
    item_equip_if_flagged(side,1,0,(uint)select->equip[4]);
    item_equip_if_flagged(side,1,1,(uint)select->equip[5]);
    item_equip_if_flagged(side,1,2,(uint)select->equip[6]);
    item_equip_if_flagged(side,1,3,(uint)select->equip[7]);
  }
  return;
}

void net_reply_sv_items_give(int side, net_select_rec *select)
{
  uint item;
  int i;

  i = 0;
  if ((select->mask & 4) != 0) {
    while (1) {
      item = select->items[i];
      if (item == 0) break;
      item_give_carded(side,item);
      i = i + 1;
    }
    net_sv_items_give_count = net_sv_items_give_count + 1;
  }
}

void net_reply_equip_set(int side, net_select_rec *select)
{
  if ((select->mask & 2) != 0) {
    item_equip_if_flagged(side,0,0,(uint)select->equip[0]);
    item_equip_if_flagged(side,0,1,(uint)select->equip[1]);
    item_equip_if_flagged(side,0,2,(uint)select->equip[2]);
    item_equip_if_flagged(side,0,3,(uint)select->equip[3]);
    item_equip_if_flagged(side,1,0,(uint)select->equip[4]);
    item_equip_if_flagged(side,1,1,(uint)select->equip[5]);
    item_equip_if_flagged(side,1,2,(uint)select->equip[6]);
    item_equip_if_flagged(side,1,3,(uint)select->equip[7]);
    net_equip_set_count = net_equip_set_count + 1;
  }
  return;
}

void net_reply_emblem_set(int side, net_select_rec *select)
{
  card_image_rec *card;

  card = card_image_get(side);
  if ((select->mask & 8) != 0) {
    card_body_emblem_set(&card->body,(uint)select->emblem);
    net_emblem_set_count = net_emblem_set_count + 1;
  }
  if ((select->mask & 0x10) != 0) {
    card_body_emblem_dan_set(&card->body,(uint)select->emblem_dan);
  }
  return;
}

int net_sv_item_receive(int side, char *text)
{
  int mask;
  mask = net_reply_sv_item_parse(text);
  if (mask <= 0) return mask;
  net_select_scratch.mask = mask;
  net_reply_sv_items_give(side,&net_select_scratch);
  return mask;
}

int net_equip_receive(int side, char *text)
{
  int mask;
  mask = net_reply_equip_parse(text);
  if (mask <= 0) return mask;
  net_select_scratch.mask = mask;
  net_reply_equip_set(side,&net_select_scratch);
  return mask;
}

int net_emblem_receive(int side, char *text)
{
  int mask = net_reply_emblem_parse(text);
  if (mask <= 0) return mask;
  net_select_scratch.mask = mask;
  net_reply_emblem_set(side,&net_select_scratch);
  return mask;
}

void net_select_pending_clear(void)
{
  net_select_pending[0].valid = 0;
  net_select_pending[1].valid = 0;
  return;
}

void net_select_pending_apply(int side)
{
  net_select_rec *pending;

  pending = &net_select_pending[side];
  if ((card_record_get(side)->in_use != 0) && (pending->valid != 0)) {
    char code[17] = "0000000000000000";
    card_access_code_make(code, card_image_get(side)->access_key);
    if (strcmp(code, pending->access_code) != 0) {
      pending->valid = 0;
    }
    else {
      net_card_reply_apply(side, pending);
      if ((pending->mask & 0x20) != 0) {
        costume_colors_apply(side, 0, pending->color[0], pending->color[1], pending->color[2], pending->color[3], pending->color[8], pending->color[9]);
        costume_colors_apply(side, 1, pending->color[4], pending->color[5], pending->color[6], pending->color[7], pending->color[10], pending->color[11]);
        if (side == 0) {
          net_ack_bits = net_ack_bits | 4;
        }
        else {
          net_ack_bits = net_ack_bits | 8;
        }
      }
      pending->valid = 0;
      net_select_apply_count = net_select_apply_count + 1;
    }
  }
}

int net_select_reply_store(form_map *form)
{
  net_select_rec *scratch;
  char *text;
  int side;
  int i;
  uint mask;
  int part;

  mask = 0;
  scratch = &net_select_scratch;
  net_select_pending[0].valid = mask;
  net_select_pending[0].mask = mask;
  text = (char *)form_map_find(form,"access_code_2p");
  if (text != 0) {
    strncpy(scratch->access_code,text,0x10);
    scratch->access_code[0x10] = mask;
    side = 1;
  }
  else {
    text = (char *)form_map_find(form,"access_code_1p");
    if (text == 0) return 0xffffffff;
    strncpy(scratch->access_code,text,0x10);
    scratch->access_code[0x10] = 0;
    side = 0;
  }
  mask |= 1;
  part = net_reply_color_parse((char *)form_map_find(form,net_color_keys[side]));
  if (part < 0) return 0xffffffff;
  mask |= part;
  part = net_reply_sv_item_parse((char *)form_map_find(form,net_sv_item_keys[side]));
  if (part < 0) return 0xffffffff;
  mask |= part;
  part = net_reply_equip_parse((char *)form_map_find(form,net_equip_keys[side]));
  if (part < 0) return 0xffffffff;
  mask |= part;
  part = net_reply_emblem_parse((char *)form_map_find(form,net_emblem_keys[side]));
  if (part < 0) return 0xffffffff;
  mask |= part;
  strcpy(net_select_pending[side].access_code,net_select_scratch.access_code);
  if (mask & 2) {
    for (i = 0; i < 8; i++) {
      net_select_pending[side].equip[i] = net_select_scratch.equip[i];
    }
  }
  if (mask & 0x20) {
    for (i = 0; i < 0xc; i++) {
      net_select_pending[side].color[i] = net_select_scratch.color[i];
    }
  }
  if (mask & 4) {
    for (i = 0; i < 0x10; i++) {
      net_select_pending[side].items[i] = net_select_scratch.items[i];
    }
  }
  if (mask & 8) {
    net_select_pending[side].emblem = net_select_scratch.emblem;
  }
  if (mask & 0x10) {
    net_select_pending[side].emblem_dan = net_select_scratch.emblem_dan;
  }
  net_select_pending[side].valid = 1;
  net_select_pending[side].mask = mask;
  net_select_store_count = net_select_store_count + 1;
  return 0;
}

void net_select_request_send(int side)
{
  card_record_rec *record;
  uint card_id;
  char code [17] = "0000000000000000";
  strbuf_rec *buf = &net_req_buf;

  record = card_record_get(side);
  net_select_pending[side].valid = 0;
  card_access_code_make(code,record->access_key);
  strbuf_init(buf,net_req_text,0x800,0,0);
  strbuf_printf(buf,"cmd=select&");
  strbuf_printf(buf,"access_code_%dp=%s&",side + 1,code);
  card_id = card_hdr_card_id_get(&record->header);
  strbuf_printf(buf,"card_id_%dp=%d&",side + 1,card_id);
  strbuf_printf(buf,"cnt=%d",(ushort)net_req_count++);
  net_upload_send(buf);
  strbuf_release_nop(buf);
  net_select_request_count = net_select_request_count + 1;
}

void net_select_reply_handle(char *body)
{
  int result;
  form_map form;

  net_select_reply_count = net_select_reply_count + 1;
  snprintf(net_select_reply_log,0x200,"%s",body);
  result = form_map_parse(&form,body);
  if (result == 0) {
    net_select_reply_store(&form);
    net_tec_reply_read(&form);
    form_map_clear(&form);
  }
  return;
}

void net_frame_service(void)
{
  int usec;
  int on;

  usec = stopwatch_usec();
  net_service_hours_check();
  card_service_state_update();
  on = net_is_on();
  if (on != 0) {
    net_service_state_update();
    nam_step();
    net_message_request_step();
    net_reply_dispatch();
    http_request_step();
    if ((alab_step_every_frame == '\x01') || (alab_step_countdown = alab_step_countdown - 1, alab_step_countdown == 0)) {
      perf_color_mark(0xd09090);
      alab_step_timed();
      alab_step_countdown = (uint)alab_freq;
    }
    alab_debug_draw();
  }
  usec = stopwatch_usec() - usec;
  if (((uint)usec > net_frame_usec_max) || (++net_frame_usec_max_frames > 0xb3)) {
    net_frame_usec_max = usec;
    net_frame_usec_max_frames = 0;
  }
  return;
}

void alab_step_every_frame_on(void)
{
  alab_step_every_frame = 1;
  return;
}

void alab_step_every_frame_off(void)
{
  alab_step_every_frame = 0;
  return;
}

void net_bookmark_set(int side)
{
  char *flags;
  flags = &net_bookmark_flags[0];
  flags[side] = 1;
  return;
}

int net_rare_value_get(int index)
{
  return ((int *)&net_game_config.rare_syuchu)[index];
}

void net_rare_value_set(int index, int value)
{
  ((int *)&net_game_config.rare_syuchu)[index] = value;
  return;
}

int net_ready_and_idle(void)
{
  if (net_is_on() != 0 && net_service_active() != 0 &&
      net_info.nam_state == 0 && net_info.busy == 0) {
    return 1;
  }
  return 0;
}

int net_ready(void)
{
  int ready;

  ready = net_is_on();
  if (((ready != 0) && (net_info.nam_state == 0)) &&
     (ready = net_service_active(), ready != 0)) {
    return 1;
  }
  return 0;
}

void net_ready_latch(void)
{
  net_ready_at_stage = net_ready();
  return;
}

int net_ready_latched(void)
{
  return net_ready_at_stage;
}

bool net_card_place_matches(uint side)
{
  int matches;
  card_image_rec *card;
  ushort place_id;
  int i;

  i = side & 0xff;
  matches = 0;
  card = card_image_get(i);
  place_id = net_check_log.place_id;
  if ((card->body.my_tenpo_id == place_id) & (place_id != 0)) matches = 1;
  return matches;
}

bool net_free_play_is_on(void)
{
  int debug;

  debug = net_debug_enabled();
  if (debug != 0) return 0;
  return credit_copy.credit_mode == 1;
}

int net_syuchu_value0_get(void)
{
  return net_game_config.rare_syuchu.syuchu_value0;
}

int net_syuchu_value1_get(void)
{
  return net_game_config.rare_syuchu.syuchu_value1;
}


int net_syuchu_value2_get(void)
{
  return net_game_config.rare_syuchu.syuchu_value2;
}

int net_kumite_card_listed(int side)
{
  int card_id;
  int i;
  int listed;
  card_id = card_hdr_card_id_get(&card_image_get(side)->header);
  if (card_id == 0) return 0;
  i = 0;
  listed = 0;
  for (; net_game_config.kumite_values[i] != 0; i++) {
    if (net_game_config.kumite_values[i] == card_id) {
      listed = 1;
      break;
    }
  }
  return listed;
}

bool net_nushi_is_on(void)
{
  return net_game_config.nushi_on == 1;
}

void net_nushi_rank_set(int side, int rank)
{
  int card_id;

  if (rank - 1U < 10) {
    card_id = card_hdr_card_id_get(&card_image_get(side)->header);
    if (card_id != 0) {
      ((struct { int ranked; int card_id; } *)&net_nushi_ranks[0][0])[side].card_id = card_id;
      ((struct { int ranked; int card_id; } *)&net_nushi_ranks[0][0])[side].ranked = 1;
    }
  }
  return;
}

void net_nushi_ranks_validate(void)
{
  typedef struct { int ranked; int card_id; } nushi_rank;
  card_record_rec *record;
  card_image_rec *card;
  nushi_rank *rank;
  int i;
  int clear;
  for (i = 0; i < 2; i++) {
    record = card_record_get(i);
    card = card_image_get(i);
    rank = &((nushi_rank *)&net_nushi_ranks[0][0])[i];
    clear = 0;
    if (record->in_use == 0) clear = 1;
    if (rank->ranked == 1 && rank->card_id != card_hdr_card_id_get(&card->header)) clear = 1;
    if (net_nushi_is_on() == 0) clear = 1;
    if (clear) {
      rank->card_id = 0;
      rank->ranked = 0;
    }
  }
}

int net_nushi_rank_get(int side)
{
  return (&net_nushi_ranks[0][0])[side * 2];
}

bool net_masa_is_on(void)
{
  return net_game_config.masa_on == 1;
}

bool net_ryuko_is_on(void)
{
  return net_game_config.ryuko_on == 1;
}

uint net_struggle_revise_get(int index)
{
  switch (index) {
  case 0:
    if (2 < net_game_config.struggle_revise[index]) net_game_config.struggle_revise[index] = 0;
    break;
  case 1:
    if (2 < net_game_config.struggle_revise[index]) net_game_config.struggle_revise[index] = 0;
    break;
  case 2:
    if (2 < net_game_config.struggle_revise[index]) net_game_config.struggle_revise[index] = 0;
    break;
  }
  return net_game_config.struggle_revise[index];
}

int date_to_minutes(uint years, int month, int day, int hour, int minute)
{
  int days;
  int month_days;
  int i;
  int leap;
  int r;

  days = 0;
  r = 0; /* WORKAROUND (kept as possibly original; no game instruction of its own): r = 0 is never read. With it, CSE takes this 0 for the guard's compare, which is the game's mov #0,r0 before cmp/ge; without it GCC compares against another register. Kept as a plausible original initialization whose reader is not in this build. */
  for (i = 0; i < (int)years; i++) {
    if ((i & 3) == 0) {
      days = days + 0x16e;
    }
    else {
      days = days + 0x16d;
    }
  }
  for (i = 1; i < month; i++) {
    month_days = net_month_days[i];
    if (i == 2) {
      leap = (years & 3) == 0;
      month_days = month_days + (leap != 0);
    }
    days = days + month_days;
  }
  days += day - 1;
  r = minute + (hour + days * 0x18) * 0x3c;
  return r;
}

uchar net_db_stat_get(void)
{
  return net_db_stat;
}

void net_nickname_params_append(int side, strbuf_rec *buf)
{
  nickname_result_rec *result;
  char *name;
  int i;

  name = net_side_suffixes[side];
  result = nickname_result_get(side);
  if (result == 0) return;
  strbuf_printf(buf,"nickname_lv_%s=",name);
  for (i = 0; i < 8; i++) {
    strbuf_printf(buf,net_int_list_format,result->level[i]);
  }
  strbuf_printf(buf,"%d&",(uchar)result->god);
  strbuf_printf(buf,"nickname_pt_diff_%s=",name);
  for (i = 0; i < 7; i++) {
    if (i != 0) {
      strbuf_printf(buf,net_list_separator);
    }
    strbuf_printf(buf,"%x,%x,%x",result->pt_diff[i][0],result->pt_diff[i][1],result->pt_diff[i][2]);
  }
  strbuf_printf(buf,"&");
}

void net_pr_texts_clear(void)
{
  int i; for (i = 0; i < 2; i++) ((struct { char pad[0x8a8]; char text0; } *)((char *)&net_game_config + i * 0x200))->text0 = 0;
}

void net_pr_text_set(int side, char *text)
{
  char *pr_text = net_pr_texts[side];
  *pr_text = 0;
  if (text != 0) {
    snprintf(pr_text,0x200,"%s",text);
  }
}

char * net_pr_text_get(int side)
{
  return net_pr_texts[side];
}

void net_nickname_reply_store(int side, char *level_text, char *point_text, char *para_text)
{
  nickname_recv_rec recv;
  token_list tokens;
  int i, j;
  int missing;
  char *token;

  missing = (level_text == 0) | (point_text == 0);
  if (missing == 0) {
    if (para_text != 0) {
      if (token_list_split(&tokens,level_text,0x2c) == 0) {
        for (i = 0; i < 8; i++) {
          if (token_list_has_next(&tokens) == 0) return;
          token = (char *)token_list_next(&tokens);
          recv.level[i] = atol(token);
        }
        if (token_list_has_next(&tokens) == 0) return;
        token = (char *)token_list_next(&tokens);
        recv.god = atol(token);
        token_list_reset(&tokens);
      }
      if (token_list_split(&tokens,point_text,0x2c) == 0) {
        for (i = 0; i < 7; i++) {
          for (j = 0; j < 3; j++) {
            if (token_list_has_next(&tokens) == 0) return;
            token = (char *)token_list_next(&tokens);
            recv.pt[i][j] = strtoul(token,0,0x10);
          }
        }
        token_list_reset(&tokens);
        recv.pt[7][0] = 0;
        recv.pt[7][1] = 0;
        recv.pt[7][2] = 0;
      }
      if (token_list_split(&tokens,para_text,0x2c) == 0) {
        for (i = 0; i < 3; i++) {
          if (token_list_has_next(&tokens) == 0) return;
          token = (char *)token_list_next(&tokens);
          if (i == 0) recv.games = atol(token);
          else if (i == 1) recv.disp_type = atol(token);
          else recv.disp_name = atol(token);
        }
        token_list_reset(&tokens);
      }
      nickname_recv_data_set(side,&recv);
    }
  }
}

void net_points_reply_store(int side, char *ranking_text, char *tenpo_text)
{
  char *config = (char *)&net_game_config;

  *(short *)(config + side * 2 + 0xce8) = 0;
  *(short *)(config + side * 2 + 0xd2c) = 0;
  if (ranking_text != 0) {
    config[side * 0x20 + 0xca8] = 0;
    strcpy(net_ranking_point_texts[side],ranking_text);
    *(short *)(config + side * 2 + 0xce8) = atol(net_ranking_point_texts[side]);
  }
  if (tenpo_text != 0) {
    char *config = (char *)&net_game_config;
    config[side * 0x20 + 0xcec] = 0;
    strcpy(net_tenpo_point_texts[side],tenpo_text);
    *(short *)(config + side * 2 + 0xd2c) = atol(net_tenpo_point_texts[side]);
  }
  return;
}

void net_tec_reply_read(form_map *form)
{
  char *text;
  int side;
  char *keys [2];

  keys[0] = "tec_1p";
  keys[1] = "tec_2p";
  side = 0;
  do {
    text = (char *)form_map_find(form,keys[side]);
    net_tec_flags_store(side,text);
    side = side + 1;
  } while (side < 2);
  return;
}

void net_tec_flags_store(int side, char *text)
{
  int i;
  card_image_rec *card;
  token_list tokens;
  byte skills [2];
  if (text != 0) {
    if (token_list_split(&tokens,text,0x2c) == 0) {
      for (i = 0; i < 2; i++) {
        skills[i] = 0;
        if (token_list_has_next(&tokens) == 0) {
          return;
        }
        if (*(char *)token_list_next(&tokens) == '1') {
          skills[i] = 1;
        }
      }
      token_list_reset(&tokens);
    }
    card = card_image_get(side);
    card_hdr_ftf_skill_set(&card->header,(uint)skills[0],(uint)skills[1]);
  }
}

int net_ticket_get_value(void)
{
  return net_ticket_get;
}

void net_players_active_clear(void)
{
  int i;
  char *active;
  i = 0;
  active = net_player_active;
  for (; i < 2; i++) active[i] = 0;
}

void net_players_active_end(void)
{
  uint i;
  char *active;
  i = 0;
  active = net_player_active;
  for (; (int)i < 2; i++) {
    if (active[i] != 0) net_player_end(i & 0xff);
  }
}

void net_player_start(uint player)
{
  int ready;
  uint side;

  side = player & 0xff;
  ready = net_ready_and_idle();
  if (ready == 1) {
    alab_play_start(side);
    net_player_event_nop(0,side);
    { char *active = net_player_active; active[side] = 1; }
  }
  else {
    { char *active = net_player_active; active[side] = 0; }
  }
}

void net_player_continue(uint player)
{
  char *active;
  uint side;

  active = net_player_active;
  side = player & 0xff;
  if (active[side] != '\0') {
    alab_play_continue(side);
    net_player_event_nop(1,side);
  }
  return;
}

void net_player_end(uint player)
{
  uint side = player & 0xff;
  char *active = net_player_active;

  if (active[side] != 0) {
    alab_play_end(side);
    net_player_event_nop(2,side);
    active[side] = 0;
  }
}

void net_player_event_nop(int event, int side)
{
  return;
}

net_info_t * net_info_get(void)
{
  return &net_info;
}

void net_log_storage_init(void)
{
  char *limit;
  int known;

  if (media_is_dimm() != 0) {
    limit = (char *)media_limit_get();
    known = (limit == (char *)0xf000000) | (limit == (char *)0x1f000000);
    if (!known) {
      limit = (char *)0xf000000;
    }
    net_log_base = limit;
    net_log_slot_count = 100;
    net_log_local = 0;
  }
  else {
    net_log_base = net_log_local_buf;
    net_log_slot_count = 1;
    net_log_local = 1;
  }
}

void net_log_record_init(net_check_log_rec *log)
{
  dimm_board_info_rec *board;

  memset((void *)log,0,0xb5c);
  snprintf(log->date_text,0x20,"%s","2004-11-15 23:01:01");
  board = dimm_board_info_fetch();
  memcpy(&log->board_info[0],&board->own_ip,4);
  memcpy(&log->board_info[1],&board->word_4c,4);
  memcpy(&log->board_info[2],&board->word_50,4);
  memcpy(&log->board_info[3],&board->name_server1,4);
  memcpy(&log->board_info[4],&board->name_server2,4);
  return;
}

int net_init(int with_http)
{
  int firmware_ok;

  memset(&net_info,0,0x44);
  if (with_http != 0) {
    http_startup();
  }
  firmware_ok = alab_init();
  net_log_storage_init();
  net_log_record_init(&net_check_log);
  net_info.on = '\x01' - (with_http == 0);
  net_info.error_frames = 0;
  net_info.error_level = 0;
  net_info.busy = 0;
  net_info.end_frames = 0;
  net_info.service_state = 0;
  net_info.hours_open = 0;
  net_info.auth_no_wait = 1;
  net_info.nam_state = 2;
  net_info.auth_stat = 0;
  net_info.game_config_received = 0;
  return firmware_ok;
}

void net_shutdown(void)
{
  int on;

  on = net_is_on();
  if (on != 0) {
    http_shutdown();
  }
  return;
}

uint net_log_slot_wrap(uint slot)
{
  uint wrapped;
  int t;
  wrapped = slot;
  if ((int)slot < 0) {
    wrapped = net_log_slot_count - 1;
  }
  else {
    t = slot >= net_log_slot_count;
    t = t == 0;
    wrapped &= -t;
  }
  return wrapped;
}

char * net_log_slot_addr(uint slot)
{
  int wrapped;

  wrapped = net_log_slot_wrap(slot);
  return wrapped * 0x4000 + net_log_base + 0x4000;
}

void net_log_slot_advance(void)
{
  int slot;
  slot = net_log_slot_wrap(user_back_data.net_log_slot_next);
  user_back_data.net_log_slot_next = slot;
  net_log_slot_cur = user_back_data.net_log_slot_next;
  user_back_data.net_log_slot_next = slot + 1;
  user_back_data.net_log_slot_next = net_log_slot_wrap(user_back_data.net_log_slot_next);
  g1_dma_spin_idle();
  sram_save_store_dma_safe();
}

char * net_log_slot_current(void)
{
  char *slot_addr;

  slot_addr = net_log_slot_addr((uint)net_log_slot_cur);
  return slot_addr;
}

int net_log_slot_count_get(void)
{
  return net_log_slot_count;
}

uchar net_is_on(void)
{
  return net_info.on;
}

bool url_is_power_on(char *url)
{
  int diff;

  diff = memcmp(url,"http://211.132.254.10/sys/servlet/PowerOn",0x2a);
  return diff == 0;
}

bool url_is_net_server(char *url)
{
  int diff;

  diff = memcmp(url,"http://211.132.254.10",0x15);
  return diff == 0;
}

char * net_upload_url_build(void)
{
  snprintf(net_upload_url,0x100,"%sVF4NetUpload",(net_upload_base_get()));
  return net_upload_url;
}

int net_reply_has_stat_header(char *text, int header_size)
{
  char *found = strstr(text,"x-rd2-stat");
  if (found == 0 || found > text + header_size - 10) return 0;
  return 1;
}

char * net_receive_status_text(int status)
{
  char *text;

  switch(status) {
  case 1:
    text = "CONNECT OK";
    break;
  case 2:
    text = "TIME OUT (NO RESPONSE)";
    break;
  case -1:
    text = "GAME ID VERSION ERROR";
    break;
  case -2:
    text = "SERIAL ERROR";
    break;
  case -3:
    text = "IP ERROR";
    break;
  default:
    text = "TIME OUT (OTHER)";
    break;
  }
  return text;
}

char * net_mode_status_text(uint mode, int test)
{
  char *s;
  if (test != 0) {
    switch (mode) {
    case 1: s = "(TEST) POWER ON INITIAL"; break;
    case 2: s = "(TEST) NETWORK OFF"; break;
    case 3: s = "(TEST) getIPbyDNS TIME OUT"; break;
    default: s = "(TEST) ???"; break;
    }
  }
  else {
    switch (mode) {
    case 1: s = "(GAME) POWER ON INITIAL"; break;
    case 2: s = "(GAME) NETWORK OFF"; break;
    case 3: s = "(GAME) getIPbyDNS TIME OUT"; break;
    default: s = "(GAME) ???"; break;
    }
  }
  return s;
}

void net_log_status_write(int net_on, int system_param, int response_frames, int timeout_frames)
{
  net_check_log_rec *log = &net_check_log;

  log->system_param = system_param;
  if (net_on != 0) {
    if (http_is_ready() != 0) {
      log->mode = 1;
    }
    else {
      log->mode = 3;
    }
  }
  else {
    log->mode = 2;
  }
  log->response_frames = response_frames;
  log->timeout_frames = timeout_frames;
  net_log_save(log);
  return;
}

int net_reply_cmd_find(char *header_text)
{
  uint i;
  char line [256];
  int cmd;

  cmd = 0xffffffff;
  for (i = 0; i < 8; i++) {
    snprintf(line,0x100,"%s: %s","x-rd2-cmd",net_cmd_table[i].name);
    if (strstr(header_text,line) != (char *)0x0) {
      cmd = net_cmd_table[i].cmd;
      break;
    }
  }
  return cmd;
}

void net_log_write(char *slot_addr, net_check_log_rec *log)
{
  if (net_log_local != '\0') {
    *(net_check_log_rec *)slot_addr = *log;
  }
  else {
    dimm_write((uint *)log,(int)slot_addr,0xb5c);
  }
}

void net_log_read(net_check_log_rec *log, char *slot_addr)
{
  if (net_log_local != '\0') {
    *log = *(net_check_log_rec *)slot_addr;
  }
  else {
    dimm_read(log,(uint)slot_addr,0xb5c);
  }
}

void net_log_save(net_check_log_rec *log)
{
  char *slot_addr;

  slot_addr = net_log_slot_current();
  net_log_write(slot_addr,log);
  return;
}

void net_log_record_fill(net_check_log_rec *log, char *ip_text, char *serial_text, char *note_text)
{
  get_now_tm(&log->request_time);
  snprintf(log->ip_text,0x40,"%s",ip_text);
  snprintf(log->serial_text,0x40,"%s",serial_text);
  snprintf(log->note_text,0x100,"%s",note_text);
  return;
}

void net_log_record_time_stamp(net_check_log_rec *log)
{
  get_now_tm(&log->local_time);
  return;
}

int net_auth_result_store(int result, net_check_log_rec *log)
{
  alab_auth_info_rec *info;
  int server_seconds;
  int local_seconds;
  int offset;

  log->place_id = 0;
  get_now_tm(&log->local_time);
  log->server_time = log->local_time;
  log->upload_base_text[0] = 0;
  log->place_text[0] = 0;
  if (result > 0) net_info.auth_stat = 1; else net_info.auth_stat = 0;
  sprintf(log->auth_stat_text,"%d",net_info.auth_stat);
  if (net_info.auth_stat == 1) {
    info = alab_auth_info_get();
    if (info != (alab_auth_info_rec *)0x0) {
      strcpy(log->upload_base_text,info->upload_base_text);
      strcpy(log->place_text,info->place_text);
      log->place_id = *(ushort *)&info->place_id;
      sprintf(log->year_text,"%d",info->year);
      sprintf(log->month_text,"%d",info->month);
      sprintf(log->day_text,"%d",info->day);
      sprintf(log->hour_text,"%d",info->hour);
      sprintf(log->minute_text,"%d",info->minute);
      sprintf(log->second_text,"%d",info->second);
      log->server_time.tm_year = info->year - 1900;
      log->server_time.tm_mon = info->month - 1;
      log->server_time.tm_mday = info->day;
      log->server_time.tm_hour = info->hour;
      log->server_time.tm_min = info->minute;
      log->server_time.tm_sec = info->second;
      log->server_time.tm_isdst = -1;
    }
  }
  server_seconds = mktime(&log->server_time);
  local_seconds = mktime(&log->local_time);
  if (server_seconds == -1 || local_seconds == -1) {
    log->clock_offset = 0;
  }
  else {
    offset = server_seconds - local_seconds;
    if ((uint)(offset + 600) <= 1200) offset = 0;
    log->clock_offset = offset;
  }
  return 0;
}

tm * net_clock_now(net_check_log_rec *log, tm *now)
{
  int t;
  get_now_tm(now);
  t = mktime(now);
  if (t != -1) {
    t = log->clock_offset + t;
    localtime_r(&t,now);
  }
  return now;
}

void net_log_record_draw(net_check_log_rec *log)
{
  char *status_text;
  text_cursor_to_cell(2,10);
  status_text = log->auth_stat_text;
  text_draw_formatted("RECEIVE STATUS   = %s\n",status_text);
  text_draw_formatted("RESPONSE TIME    = %d FRAMES (%d SECONDS)\n",
               log->response_frames, log->response_frames / 0x3c);
  text_draw_formatted("TIME OUT TIME    = %d SECONDS\n",
               (int)user_back_data.net_timeout / 0x3c);
  text_draw_formatted("MODE             = %d\n\n",log->mode);
  text_draw_formatted("\n");
  text_draw_formatted("RECEIVE STATUS   ");
  text_draw_formatted("%s",net_receive_status_text(atol(status_text)));
  text_draw_formatted("\n\n");
  text_draw_formatted("MODE STATUS      ");
  text_draw_formatted(&nam_state_log_format[4],net_mode_status_text(log->mode,0));
}

void net_log_current_draw(void)
{
  char *slot_addr;
  net_check_log_rec log;

  slot_addr = net_log_slot_current();
  net_log_read(&log,slot_addr);
  net_log_record_draw(&log);
  return;
}

char * net_serial_get(void)
{
  char *buf = net_serial;
  snprintf(buf,0xc,"%s",main_board_id_get());
  return buf;
}

char * net_game_id_get(void)
{
  return net_game_id;
}

char * net_version_text_get(void)
{
  return e_net_ver_names[(char)net_ver];
}

char * net_upload_base_get(void)
{
  return net_check_log.upload_base_text;
}

int alab_init(void)
{
  alab_work_rec *work;
  int firmware_ok;
  int i;

  work = alab_work_get();
  work->started = 0;
  work->auth_state = 0;
  work->status = 0;
  work->dimm_state = 0;
  i = 0;
  do {
    work->ends[i] = 0;
    work->continues[i] = 0;
    work->starts[i] = 0;
    i = i + 1;
  } while (i < 2);
  alab_step_enable();
  alab_step_count = 0;
  alab_disp = OFF_ON_OFF;
  firmware_ok = alab_dimm_firmware_check();
  return firmware_ok;
}

void alab_step_enable(void)
{
  alab_enabled = 1;
  return;
}

void alab_step_disable(void)
{
  alab_enabled = 0;
  return;
}

int alab_dimm_firmware_check(void)
{
  alab_work_rec *work;
  dimm_board_info_rec *board;
  int error;
  int major;
  int minor;

  work = alab_work_get();
  work->dimm_state = 2;
  board = dimm_board_info_fetch();
  if (board->id_word == 0) return 0;
  error = dimm_firmware_version_get(&major,&minor);
  if (error != 0) return 0;
  if (major <= 2) return 0;
  if (major == 3) {
    if (minor <= 0x16) return 0;
  }
  work->dimm_state = 1;
  return 1;
}

int alab_ready_check(void)
{
  alab_work_rec *work;

  work = alab_work_get();
  if (work->auth_state == 0) return 1;
  if (work->auth_state != 1) return 0xffffffff;
  return alab_status_poll_keep() == 0x65;
}

bool alab_start_auth(void)
{
  typedef struct { int w[32]; } blk;
  alab_work_rec *work;
  char *game_id;
  char *version;
  char *serial;
  int *settings;
  naominet_settings_rec *config;
  int result;
  work = alab_work_get();
  if (work->started != 1) {
    game_id = auth_game_id_text;
    strcpy(game_id, net_game_id_get());
    version = auth_version_text;
    strcpy(version, net_version_text_get());
    serial = auth_serial_text;
    strcpy(serial, net_serial_get());
    settings = (int *)&auth_client_settings;
    settings[0] = 0;
    settings[1] = 2;
    settings[2] = 0;
    settings[9] = (int)game_id;
    settings[10] = (int)version;
    settings[11] = (int)serial;
    settings[4] = 0;
    settings[3] = 4;
    settings[5] = http_router_address();
    config = (naominet_settings_rec *)&auth_client_config;
    config->tag = 0x41424920;
    config->next = 0;
    *(blk *)&config->auto_reauth = *(blk *)settings;
    result = naominet_init(auth_client_work, config, 0, 0);
    if (result < 0) { work->auth_state = 2; } else { work->auth_state = 1; }
    work->started = 1;
  }
  else if (work->auth_state == 1 && (int)naominet_auth_request() < 0) {
    work->auth_state = 2;
  }
  return work->auth_state == 1;
}

int alab_auth_result_get(void)
{
  alab_work_rec *work;
  int result;
  int status;

  work = alab_work_get();
  result = 0;
  if (work->auth_state != 1) {
    return 0xffffffff;
  }
  status = alab_status_poll_keep();
  switch (status) {
  case 0x65:
  case 0x66:
    break;
  case -0x191:
    result = -1;
    break;
  case 0x67:
    naominet_reply_words_get(&auth_reply_words.word0);
    naominet_name_info_get(auth_name_info);
    naominet_region_info_get(auth_region_info);
    naominet_place_id_get(&auth_place_id.id);
    naominet_setting_get(auth_setting);
    result = 1;
    break;
  default:
    result = 0;
    break;
  }
  return result;
}

alab_auth_info_rec * alab_auth_info_get(void)
{
  alab_auth_info_rec *info = &alab_auth_info;
  uint *clock;
  if (alab_work_get()->auth_state != 1) return 0;
  clock = auth_server_clock;
  naominet_clock_get(clock);
  info->upload_base_text = (char *)auth_reply_words.word0;
  info->reply_word1 = auth_reply_words.word1;
  info->place_id = (uint)auth_place_id.id;
  info->place_text = (char *)auth_place_id.id_word2;
  info->year = clock[0];
  info->month = clock[1];
  info->day = clock[2];
  info->hour = clock[3];
  info->minute = clock[4];
  info->second = clock[5];
  return &alab_auth_info;
}

void alab_play_start(uint player)
{
  alab_work_rec *work;
  uint i;

  i = player & 0xff;
  work = alab_work_get();
  if (alab_enabled == '\0' || i > 1) {
    return;
  }
  work->starts[i]++;
  user_back_data.billing.starts[i]++;
  naominet_player_start(i + 1);
}

void alab_play_continue(uint player)
{
  alab_work_rec *work;
  uint i;

  i = player & 0xff;
  work = alab_work_get();
  if ((alab_enabled == '\0') | (i > 1)) return;
  work->continues[i]++;
  user_back_data.billing.continues[i]++;
  naominet_player_continue(i + 1);
}

void alab_play_end(uint player)
{
  alab_work_rec *work;
  uint i;

  i = player & 0xff;
  work = alab_work_get();
  if (((alab_enabled == '\0') | (i > 1)) == 0) {
    work->ends[i]++;
    user_back_data.billing.ends[i]++;
    naominet_player_end(i + 1);
  }
  return;
}

void alab_step_timed(void)
{
  uint *usec;
  if (alab_enabled != '\0') {
    alab_step_count = alab_step_count + 1;
    usec = &alab_step_usec;
    *usec = stopwatch_usec();
    g1_bus_hold();
    naominet_step();
    g1_bus_release();
    *usec = stopwatch_usec() - *usec;
  }
}

void alab_debug_draw(void)
{
  alab_work_rec *work;
  int i;

  work = alab_work_get();
  if (alab_disp != OFF_ON_OFF) {
    text_cursor_to_cell(10,0x1e);
    text_draw_formatted(&nam_state_log_format[4],e_dimm_names[work->dimm_state]);
    text_draw_formatted(&nam_state_log_format[4],e_alab_stat_names[work->auth_state]);
    text_draw_formatted("%d\n",work->status);
    if (0x1f4 < alab_step_usec) {
      alab_step_over500_count = alab_step_over500_count + 1;
    }
    if ((alab_step_usec > alab_step_usec_max) || (alab_step_max_frames = alab_step_max_frames + 1, 0xb3 < alab_step_max_frames)) {
      alab_step_usec_max = alab_step_usec;
      alab_step_max_frames = 0;
    }
    if ((alab_step_usec < alab_step_usec_min) || (alab_step_min_frames = alab_step_min_frames + 1, 0xb3 < alab_step_min_frames)) {
      alab_step_usec_min = alab_step_usec;
      alab_step_min_frames = 0;
    }
    alab_step_usec_sum = alab_step_usec_sum + (float)alab_step_usec;
    alab_step_samples = alab_step_samples + 1;
    text_draw_formatted(alab_step_usec_sum / (float)alab_step_samples,"CPU cur: %d max: %d min: %d avg:%f\n",
                 alab_step_usec,alab_step_usec_max,alab_step_usec_min);
    text_draw_formatted("    over500ms: %d\n",alab_step_over500_count);
    text_draw_formatted("    CallCount: %d\n",alab_step_count);
    text_draw_formatted("\n");
    i = 0;
    do {
      text_draw_formatted("START: %d CONT: %d END: %d\n",work->starts[i],
                   work->continues[i],
                   work->ends[i]);
      i = i + 1;
    } while (i < 2);
  }
  return;
}

int alab_status_poll_keep(void)
{
  alab_work_rec *work;
  int status;

  work = alab_work_get();
  status = naominet_status_poll();
  work->status = status;
  return status;
}

alab_work_rec * alab_work_get(void)
{
  return &alab_work;
}

void card_access_code_make(char *access_code, int access_key)
{
  card_md5_rec ctx;
  char key_digits[12];
  char crypt_digits[12];
  char check_digits[12];
  uchar digest[16];
  uint words[6];
  snprintf(key_digits,9,access_code_key_format,access_key);
  card_md5_init(&ctx);
  card_md5_update(&ctx,(uchar *)key_digits,strlen(key_digits));
  card_md5_final(digest,&ctx);
  words[0] = (digest[2] & 0x7f) << 16 | digest[1] << 8 | digest[0];
  words[1] = (digest[5] & 0x3f) << 17 | digest[4] << 9 | digest[3] << 1 | (digest[2] & 0x80) >> 7;
  words[2] = (digest[8] & 0x1f) << 18 | digest[7] << 10 | digest[6] << 2 | (digest[5] & 0xc0) >> 6;
  words[3] = (digest[11] & 0xf) << 19 | digest[10] << 11 | digest[9] << 3 | (digest[8] & 0xe0) >> 5;
  words[4] = (digest[14] & 7) << 20 | digest[13] << 12 | digest[12] << 4 | (digest[11] & 0xf0) >> 4;
  words[5] = digest[15] << 5 | (digest[14] & 0xf8) >> 3;
  words[0] = words[0] ^ words[1] ^ words[2] ^ words[3] ^ words[4] ^ words[5];
  snprintf(check_digits,9,access_code_check_format,words[0]);
  sol_digits_crypt(0,(byte *)check_digits,(byte *)key_digits,crypt_digits);
  snprintf(access_code,0x11,"4%.8s%.7s",crypt_digits,check_digits);
}
