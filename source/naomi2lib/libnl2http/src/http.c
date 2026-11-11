/*
 * naomi2lib/libnl2http/src/http.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"
#define DIMM_BOARD_WORD ((volatile uint *)0xac01fc04)

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern http_dump_fn http_dump_hook; /* 0x0c3f16a4 */
extern http_slot http_slots[16]; /* 0x0c3f16a8 */
extern int http_ring_head; /* 0x0c5cd160 */
extern int http_wait_counts; /* 0x0c5cd164 */
extern int http_wait_send_frames; /* 0x0c5cd168 */
extern int http_wait_recv_frames; /* 0x0c5cd16c */
extern int http_wait_close_frames; /* 0x0c5cd170 */
extern int http_size_stats; /* 0x0c5cd174 */
extern int http_request_body_bytes; /* 0x0c5cd178 */
extern int http_reply_header_bytes; /* 0x0c5cd17c */
extern int http_reply_body_bytes; /* 0x0c5cd180 */
extern int http_pack_bodies; /* 0x0c5cd18c */
extern char http_own_ip_buf[16]; /* 0x0c5cd1bc */
extern char http_server_ip_buf[16]; /* 0x0c5cd1cc */
extern uint http_router_addr; /* 0x0c5cd1dc */
extern char http_error_text[256]; /* 0x0c412a28 */
extern int http_error_state; /* 0x0c5cb014 */
extern int http_failed_code; /* 0x0c5cb018 */
extern http_slot http_slot_backup; /* 0x0c5cb01c */
extern int http_retry_total; /* 0x0c5cd154 */
extern int http_ring_tail; /* 0x0c5cd158 */
extern int http_ring_active; /* 0x0c5cd15c */
extern int http_req14_done; /* 0x0c5cd184 */
extern int http_req14_pending; /* 0x0c5cd188 */
extern int http_ready_flag; /* 0x0c5cd190 */
extern int http_lookup_frame; /* 0x0c5cd194 */
extern int http_wait_frames; /* 0x0c5cd198 */
extern char http_user_agent_buf[32]; /* 0x0c5cd19c */

static inline short swap_bytes16(short x)
{
  return (x & 0xff) << 8 | (x & 0xff00) >> 8;
}

void http_startup(void)
{
  uint present;
  int found;
  int i;
  
  http_ready_flag = 0;
  present = net_board_present();
  if (present != 0) {
    dimm_g1_timing_apply(0);
    render_with_busy_flag();
    render_with_busy_flag();
    dimm_net_attach(0);
    http_board_req14_arm();
    http_failed_code = 0;
    http_user_agent_set("NAOMI-VF4FT/1.0");
    http_ring_head = 0;
    http_ring_active = 0;
    http_ring_tail = 0;
    http_retry_total = 0;
    http_wait_close_frames = 0;
    http_wait_recv_frames = 0;
    http_wait_send_frames = 0;
    http_wait_counts = 0;
    http_reply_body_bytes = 0;
    http_reply_header_bytes = 0;
    http_request_body_bytes = 0;
    http_size_stats = 0;
    http_wait_frames = 0;
    http_pack_bodies = 1;
    for (i = 0; i < 0x10; i++) {
      http_slots[i].state = 0;
    }
    http_lookup_frame = 0;
    for (i = 0; i < 600; i++) {
      found = http_server_lookup_step(&i);
      if (found != 0) {
        http_ready_flag = 1;
        break;
      }
      render_with_busy_flag();
    }
    http_router_lookup();
  }
  return;
}

void http_lookup_retry_step(void)
{
  int r;
  
  r = http_is_ready();
  if (r == 0) {
    if (600 < http_lookup_frame) {
      http_lookup_frame = 0;
      http_req14_pending = 0;
    }
    if (((http_lookup_frame != 0) || (r = http_board_done_locked(0), r == 1)) &&
       (r = http_board_req14_poll(), r != 0)) {
      r = http_server_lookup_step(&http_lookup_frame);
      if (r != 0) {
        http_ready_flag = 1;
      }
      http_lookup_frame = http_lookup_frame + 1;
    }
  }
  return;
}

void http_shutdown(void)
{
  http_slot *slot;
  
  if (http_is_ready() != 0) {
    slot = &http_slots[http_ring_active];
    if ((uint)(slot->state - 3) < 3 && http_close_step(slot) == 0) {
      slot->state = 8;
    }
    dimm_net_detach();
  }
}

char * http_user_agent(void)
{
  return http_user_agent_buf;
}

void http_user_agent_set(char *text)
{
  snprintf(http_user_agent_buf,0x20,"%s",text);
  return;
}

char * http_own_ip_text(void)
{
  return http_own_ip_buf;
}

char * http_server_ip_text(void)
{
  return http_server_ip_buf;
}

uint http_router_address(void)
{
  return http_router_addr;
}

void ipv4_dotted_format(char *out, uint addr)
{
  byte *b = (byte *)&addr;
  
  sprintf(out,"%d.%d.%d.%d",b[0],b[1],b[2],b[3]);
}

int http_is_ready(void)
{
  return http_ready_flag;
}

void http_request_step(void)
{
  int again;
  int idx;
  http_slot *slot;

  if (http_is_ready() == 0) {
    return;
  }
  idx = http_ring_active;
  slot = &http_slots[idx];
  if (slot == 0 || slot->board_wait == 1) {
    if (http_board_done_locked(slot->sock) == 1) {
      http_board_result_take();
    }
    else {
      http_wait_tick(slot);
    }
    if (http_wait_frames < 0x2580) {
      return;
    }
    slot->state = 8;
  }
  do {
    http_wait_frames = 0;
    again = 0;
    switch (slot->state) {
    case 1:
      http_slot_backup = *slot;
      slot->state = 2;
      again = 1;
      break;
    case 2:
      if (http_socket_create_step(slot) == 0) {
        slot->state = 3;
      }
      else if (http_error_code() != 1) {
        slot->state = 8;
      }
      break;
    case 3:
      if (http_connect_step(slot) == 0) {
        slot->state = 4;
        slot->cursor = 0;
      }
      else if (http_error_code() != 1) {
        slot->state = 8;
      }
      slot->board_wait = 1;
      break;
    case 4:
      if (http_send_step(slot) == 0) {
        slot->state = 5;
        slot->cursor = 0;
      }
      else if (http_error_code() != 1) {
        slot->state = 8;
      }
      slot->board_wait = 1;
      break;
    case 5:
      if (http_recv_step(slot) == 0) {
        slot->state = 6;
        slot->cursor = 0;
      }
      else if (http_error_code() != 1) {
        slot->state = 8;
      }
      slot->board_wait = 1;
      break;
    case 6:
      if (http_close_step(slot) == 0) {
        slot->state = 7;
        http_active_advance();
      }
      else if (http_error_code() != 1) {
        slot->state = 8;
      }
      break;
    case 8:
      http_failed_code = http_error_code();
      slot->state = 9;
      again = 1;
    case 9:
      if (http_close_step(slot) == 0) {
        *slot = http_slot_backup;
        http_retry_total++;
      }
      else if (http_error_code() != 1) {
        slot->state = 10;
      }
      break;
    case 0:
    case 10:
    default:
      break;
    }
  } while (again);
}

void http_board_result_take(void)
{
  http_slot *slot = &http_slots[http_ring_active];
  slot->board_wait = ((int)http_board_result(slot->sock) < 0) * 2;
}

uint http_get_queue(char *url)
{
  http_slot *slot;
  int n;
  uint r;
  
  if (http_is_ready() == 0) {
    http_error_report(2,0x170);
    return http_error_code();
  }
  slot = http_slot_alloc();
  slot->state = 1;
  if (http_url_split(slot,url) == 0) {
    n = snprintf(slot->text,0x1000,"GET %s HTTP/1.0\r\n\r\n",slot->scratch);
    if ((slot->length = strlen(slot->text)) != n) {
      http_error_report(0xd,0x17b);
      return http_error_code();
    }
  }
  r = http_error_code();
  if (r == 1) r = 0;
  return r;
}

uint http_post_queue(char *url, Bytef *body, uint body_size)
{
  http_slot *slot;
  uLong crc;
  char *p;
  uint len;
  int hdr;
  uint r;

  int err;
  int line;

  if (http_is_ready() == 0) {
    err = 2;
    line = 0x196;
    goto fail;
  }
  slot = http_slot_alloc();
  slot->state = 1;
  hdr = 0;
  if (http_url_split(slot,url) == 0) {
    crc = crc32(0,0,0);
    crc = crc32(crc,body,body_size);
    p = slot->text;
    p += sprintf(p,"POST %s HTTP/1.0\r\n",slot->scratch);
    len = 0x1000;
    if (http_pack_bodies != 0) {
      r = http_body_pack(slot->scratch,(int *)&len,body,body_size);
      if (r != 0) {
        return r;
      }
      p += sprintf(p,"Pragma: DFI\r\n");
    }
    else {
      if (len < body_size) {
        err = 0xd;
        line = 0x1b1;
        goto fail;
      }
      len = body_size;
      memcpy(slot->scratch,body,body_size);
    }
    p += sprintf(p,"x-rd2-crc: %08lx\r\n",crc);
    p += sprintf(p,"x-rd2-wait: %04x,%04x,%04x,%04x\r\n",http_wait_counts,http_wait_send_frames,http_wait_recv_frames,http_wait_close_frames);
    p += sprintf(p,"x-rd2-size: %04x,%04x,%04x,%04x\r\n",http_size_stats,http_request_body_bytes,http_reply_header_bytes,http_reply_body_bytes);
    p += sprintf(p,"Content-Type: application/x-www-form-urlencoded\r\n");
    p += sprintf(p,"Host: %s\r\n",slot->host);
    p += sprintf(p,"Connection: Close\r\n");
    p += sprintf(p,"User-Agent: %s\r\n",http_user_agent());
    p += sprintf(p,"Content-Length: %d\r\n\r\n",len);
    if (http_dump_hook != 0) {
      (*http_dump_hook)(slot->text,p - slot->text);
      (*http_dump_hook)(body,body_size);
      (*http_dump_hook)("\n",1);
    }
    hdr = p - slot->text;
    if (hdr + len > 0x1000) {
      fatal_at("/p71/prj71p/vf4e2/naomi2lib/libnl2http/src/http.c",0x1cf,"assertion `%s\' failed.\n","0");
      err = 0xd;
      line = 0x1d0;
fail:
      http_error_report(err,line);
      return http_error_code();
    }
    slot->length = len + hdr;
    memcpy(p,slot->scratch,len);
  }
  http_wait_close_frames = 0;
  http_wait_recv_frames = 0;
  http_wait_send_frames = 0;
  http_wait_counts = 0;
  http_size_stats = hdr;
  http_request_body_bytes = len;

  r = http_error_code();
  if (r == 1) r = 0;
  return r;
}

int http_reply_ready(void)
{
  int ready;
  http_slot *slot;

  slot = &http_slots[http_ring_head];
  ready = 0;
  if ((slot->board_wait == 0) && (slot->state == 7)) {
    ready = 1;
  }
  return ready;
}

void http_reply_release(void)
{
  if (http_reply_ready() != 0) {
    http_slots[http_ring_head].state = 0;
    http_head_advance();
  }
}

char * http_reply_text(void)
{
  int ready;
  
  ready = http_reply_ready();
  if (!(ready != 0)) {
    return (char *)0x0;
  }
  return http_slots[http_ring_head].text;
}

char * http_header_end_find(char *text)
{
  char s[] = "\r\n\r\n";
  char *p;
  p = strstr(text, s);
  return p + strlen(s);
}

int http_reply_body_size(void)
{
  if (http_reply_ready() == 0) return 0xffffffff;
  return http_slots[http_ring_head].body_size;
}

int http_reply_header_size(void)
{
  if (http_reply_ready() == 0) return 0xffffffff;
  return http_slots[http_ring_head].header_size;
}

int http_ring_distance(int a, int b)
{
  if (a < b) {
    a = a + 0x10;
  }
  return a - b;
}

int http_queued_count(void)
{
  int n;
  int i;
  int t;

  n = 0;
  for (i = 0; i < 0x10; i++) {
    t = http_slots[i].state == 1;
    n += t != 0;
  }
  return n;
}

int http_server_lookup_step(int *frame)
{
  dimm_board_info_rec *info;
  uint dns1;
  uint dns2;
  int r;
  int found;
  char dns1_text [16];
  char dns2_text [16];

  found = 0;
  if (*frame == 0) {
    info = dimm_board_info_fetch();
    ipv4_dotted_format(http_own_ip_buf,info->own_ip);
    ipv4_dotted_format(dns1_text,info->name_server1);
    ipv4_dotted_format(dns2_text,info->name_server2);
    dns1 = ip_text_parse(dns1_text);
    dns2 = ip_text_parse(dns2_text);
    r = host_resolve_start("naominet.jp",dns1,dns2,0);
    if (r == -1) {
      *frame = 600;
      return 0;
    }
  }
  else {
    r = http_board_done_locked(0);
    if (r == 1) {
      r = http_board_result(0);
      if (r == 0) {
        *frame = 600;
      }
      else {
        ipv4_dotted_format(http_server_ip_buf,r);
        found = 1;
      }
    }
  }
  return found;
}

void http_router_lookup(void)
{
  dimm_board_info_rec *info;
  uint dns1;
  uint dns2;
  uint r;
  uint k;
  char a [16];
  char b [16];
  
  info = dimm_board_info_fetch();
  ipv4_dotted_format(a,info->name_server1);
  ipv4_dotted_format(b,info->name_server2);
  dns1 = ip_text_parse(a);
  dns2 = ip_text_parse(b);
  r = host_resolve("tenporouter.loc",dns1,dns2,0);
  k = 0xfffffffc;
  if (r != 0) {
    if (r < k) goto good;
  }
  http_router_addr = info->own_ip;

  return;
good:
  http_router_addr = r;
}

int http_socket_create_step(http_slot *slot)
{
  int r;
  int s;
  int *q;
  int a, b;

  q = (int *)&slot->addr;
  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  r = ip_text_parse(slot->host);
  slot->addr.ip = r;
  slot->addr.family = 2;
  slot->addr.port = swap_bytes16(slot->port);
  if (r == -1) {
    a = 5;
    b = 0x28b;
  } else {
    s = http_socket_locked(2,1,0);
    slot->sock = s;
    if (s < 0) {
      a = 6;
      b = 0x290;
    } else {
      slot->connect_started = 0;
      a = 0;
      b = 0;
    }
  }
  http_error_report(a,b);
  return http_error_code();
}

int http_connect_step(http_slot *slot)
{
  if (slot->connect_started == 0) {
    ((int (*)())nsock_connect_start)(slot->sock,&slot->addr,0x10);
    slot->connect_started = 1;
    http_error_report(1,0);
  }
  else if (http_board_result(slot->sock) == -1) {
    http_error_report(7,0x2a0);
  }
  else {
    http_error_report(0,0);
  }
  return http_error_code();
}

int http_close_step(http_slot *slot)
{
  int a, b;
  if (http_close_locked(slot->sock) == -1) {
    a = 10;
    b = 0x2ab;
  } else {
    a = 0;
    b = 0;
  }
  http_error_report(a, b);
  return http_error_code();
}

int http_url_split(http_slot *slot, char *url)
{
  char *p;
  char *src;
  ushort *port;
  int v;
  http_slot *dest = slot;

  p = slot->text;
  strcpy(p,url);
  p = strstr(p,"http://");
  if ((p == (char *)0x0) || (*p == '\0')) {
    http_error_report(3,0x2c4);
    return http_error_code();
  }
  p = p + 7;
  src = strchr(p,0x2f);
  if (src != (char *)0x0) {
    strcpy(slot->scratch,src);
    *src = '\0';
    strcpy(dest->host,p);
  }
  else {
    *(ushort *)slot->scratch = 0x2f;
    strcpy(slot->host,p);
  }
  src = strchr(dest->host,0x3a);
  if (src != (char *)0x0) {
    port = &dest->port;
    v = atol(src + 1);
    *port = v;
    if ((ushort)v == 0)
      v = 0x50;
    *port = v;
    *src = '\0';
  }
  else {
    dest->port = 0x50;
  }
  return 0;
}

int http_recv_step(http_slot *slot)
{
  int got;
  int code;
  int line;
  
  if (slot->cursor == 0) {
    got = -1;
    slot->cursor = slot->text;
    slot->left = 0x1000;
    slot->length = 0;
  }
  else {
    nsock_recv_collect(slot->sock);
    got = http_board_result(slot->sock);
    if (got < 0) {
      fatal_at("/p71/prj71p/vf4e2/naomi2lib/libnl2http/src/http.c",0x2f4,"assertion `%s\' failed.\n"
               ,"0");
      code = 8;
      line = 0x2f5;
      goto report;
    }
    slot->left = slot->left - got;
    slot->cursor = slot->cursor + got;
    slot->length = slot->length + got;
  }
  if ((got == 0) || (slot->left == 0)) {
    return http_reply_finish(slot);
  }
  ((int (*)())nsock_recv_start)(slot->sock,slot->cursor,slot->left,0);
  code = 1;
  line = 0;
report:
  http_error_report(code,line);
  return http_error_code();
}

int http_send_step(http_slot *slot)
{
  int sent;
  int code;
  int line;
  
  if (slot->cursor == 0) {
    sent = -1;
    slot->cursor = slot->text;
    slot->left = slot->length;
  }
  else {
    sent = http_board_result(slot->sock);
    if (sent < 0) {
      code = 9;
      line = 0x312;
      goto report;
    }
    slot->left = slot->left - sent;
    slot->cursor = slot->cursor + sent;
  }
  if ((sent == 0) || (slot->left == 0)) {
    code = 0;
  }
  else {
    nsock_send_start(slot->sock,slot->cursor,slot->left,0);
    code = 1;
  }
  line = 0;
report:
  http_error_report(code,line);
  return http_error_code();
}

int http_reply_finish(http_slot *slot)
{
  char *dst;
  char *src;
  int r;
  uLongf n;

  if (slot->length == 0) {
    slot->body_size = 0;
    slot->header_size = 0;
    http_reply_body_bytes = 0;
    http_reply_header_bytes = 0;
  }
  else {
    src = slot->text;
    dst = http_header_end_find(src);
    http_reply_header_bytes = slot->header_size = dst - src;
    http_reply_body_bytes = slot->body_size = slot->length - slot->header_size;
    if (http_reply_is_packed(src) && http_reply_body_bytes != 0) {
      n = 0x1000;
      r = http_body_unpack((Bytef *)slot->scratch,&n,(byte *)dst,http_reply_body_bytes);
      if (r != 0) {
        return r;
      }
      memcpy(dst,slot->scratch,n);
      slot->length = http_reply_header_bytes + n;
      slot->body_size = n;
    }
    dst[slot->body_size] = 0;
  }
  return 0;
}

uint http_ring_next(int index)
{
  return index + 1U & 0xf;
}

void http_board_req14_arm(void)
{
  dimm_board_info_rec *info;
  
  info = dimm_board_info_fetch();
  http_req14_done = (uint)(info->word_18_bit16 == '\0');
  http_req14_pending = 0;
  return;
}

int http_board_req14_poll(void)
{
  int r;
  
  if (http_req14_done == 1) {
    return 1;
  }
  if (http_req14_pending == 0) {
    r = board_switch_start(1);
    if (r == -3) {
      return 1;
    }
    if (r == 0) {
      http_req14_pending = 1;
    }
  }
  else {
    http_req14_pending = 0;
    r = http_board_result(0);
    if (r == 0) {
      http_req14_done = 1;
    }
  }
  return 0;
}

uint http_ring_prev(int index)
{
  return index + 0xfU & 0xf;
}


void http_head_advance(void)
{
  while (http_ring_head != http_ring_active) {
    if (http_slots[http_ring_head].state == 7) break;
    {
      uint (*next)(int) __attribute__((const)) = http_ring_next;
      http_ring_head = next(http_ring_head);
    }
  }
}

void http_active_advance(void)
{
  typedef uint idx_next_fn(int);
  
  while (http_ring_active != http_ring_tail) {
    const idx_next_fn *next;
    if (http_slots[http_ring_active].state == 1) break;
    next = (const idx_next_fn *)http_ring_next;
    http_ring_active = next(http_ring_active);
  }
}

void http_queued_drop_one(void)
{
  int cur;
  int i;
  int p;
  int o;
  int s;
  int *q;
  i = http_ring_active;
  cur = http_ring_tail;
  if (i != cur) {
    for (i = http_ring_next(i); i != cur; i = http_ring_next(i)) {
      s = (int)&http_slots;
      p = s + i * 0x2138;
      o = 0x11c;
      q = (int *)(p + o);
      if (*q == 1) {
        *q = 0;
        return;
      }
    }
  }
}

http_slot * http_slot_alloc(void)
{
  int *tail;
  int old;
  int n;
  int cur;
  int p;
  int o;
  int q;
  tail = &http_ring_tail;

  old = *tail;
  n = http_queued_count();
  cur = *tail;
  p = (int)&http_slots + cur * 0x2138;
  o = 0x11c;
  *(int *)(p + o) = 0;
  *tail = http_ring_next(cur);
  if (http_ring_head == *tail) {
    if (http_ring_distance(old, http_ring_active) > 7) {
      p = (int)&http_slots + http_ring_prev(old) * 0x2138;
      q = p + o;
      *(int *)q = 0;
      *tail = old;
    }
    else {
      http_head_advance();
    }
  }
  else {
    if (n > 7) {
      http_queued_drop_one();
    }
  }
  http_slots[http_ring_tail].state = 0;
  return (http_slot *)p;

}


int http_error_code(void)
{
  return http_error_state;
}

int http_retry_total_get(void)
{
  return http_retry_total;
}

void http_error_code_store(int code)
{
  http_error_state = code;
  return;
}

void http_error_report(int code, int line)
{
  char *s;
  
  s = "http:unknown error in %d";
  http_error_code_store(code);
  switch(http_error_code()) {
  case 2:
    s = "http:not initialized in %d";
    break;
  case 3:
    s = "http:uri invalid error in %d";
    break;
  case 4:
    s = "http:bad host name in %d";
    break;
  case 5:
    s = "http:bad IP/ bad ETHER BD in %d";
    break;
  case 6:
    s = "http:socket for server failed in %d";
    break;
  case 7:
    s = "http:connect failed in %d";
    break;
  case 8:
    s = "http:recv failed in %d";
    break;
  case 9:
    s = "http:send failed in %d";
    break;
  case 10:
    s = "http:close failed in %d";
    break;
  case 0:
    memcpy(http_error_text,"http:ok",8);
    return;
  case 1:
    memcpy(http_error_text,"http:wait",10);
    return;
  }
  snprintf(http_error_text,0x100,s,line);
}

int http_board_result(int sock)
{
  int r;
  
  r = sock_result_take(sock);
  return r;
}

uint net_board_present(void)
{
  return *DIMM_BOARD_WORD & 0x20000000;
}

void http_wait_tick(http_slot *slot)
{
  int *count;
  
  http_wait_frames = http_wait_frames + 1;
  switch((uint)slot->state) {
  case 3:
    count = &http_wait_counts;
    break;
  case 4:
    count = &http_wait_send_frames;
    break;
  case 5:
    count = &http_wait_recv_frames;
    break;
  case 6:
    count = &http_wait_close_frames;
    break;
  default:
    goto done;
  }
  *count = *count + 1;
done:
  return;
}

int http_body_pack(char *out, int *out_size, Bytef *body, uLong body_size)
{
  int r;
  Bytef packed [4096];
  uLongf len;
  len = 0x1000;
  r = compress(packed,&len,body,body_size);
  if (r != 0) {
    http_error_report(0xb,0x454);
    return http_error_code();
  }
  r = base64_encode(out,out_size,packed,len);
  if (r < 0) {
    http_error_report(0xd,0x459);
    return http_error_code();
  }
  return 0;
}

int http_body_unpack(Bytef *out, uLongf *out_size, byte *text, int text_size)
{
  int r;
  Bytef buf [4096];
  uLong len;
  
  len = 0x1000;
  r = base64_decode(buf,(int *)&len,text,text_size);
  if (r < 0) {
    http_error_report(0xd,0x467);
    return http_error_code();
  }
  *out_size = 0x1000;
  r = uncompress(out,out_size,buf,len);
  if (r != 0) {
    http_error_report(0xc,0x46d);
    return http_error_code();
  }
  return 0;
}

bool http_reply_is_packed(char *text)
{
  char *mark;
  
  mark = strstr(text,"Pragma: DFI\r\n");
  return mark != (char *)0x0;
}

void http_ring_state_get(int *tail, int *active, int *head)
{
  *tail = http_ring_tail;
  *active = http_ring_active;
  *head = http_ring_head;
  return;
}

void http_pack_bodies_set(int on)
{
  http_pack_bodies = on;
  return;
}

void http_dump_hook_set(http_dump_fn hook)
{
  http_dump_hook = hook;
  return;
}

int http_board_done_locked(int sock)
{
  int done;
  
  g1_bus_hold();
  done = sock_call_done(sock);
  g1_bus_release();
  return done;
}

int http_socket_locked(int family, int type, int protocol)
{
  int sock;
  
  g1_bus_hold();
  sock = sock_create(family,type,protocol);
  g1_bus_release();
  return sock;
}

int http_close_locked(int sock)
{
  int r;
  
  g1_bus_hold();
  r = sock_close(sock);
  g1_bus_release();
  return r;
}

