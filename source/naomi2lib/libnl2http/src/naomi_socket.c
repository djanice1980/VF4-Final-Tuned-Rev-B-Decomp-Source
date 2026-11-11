/*
 * naomi2lib/libnl2http/src/naomi_socket.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char nsock_recv_buf[0x1000]; /* 0x0c5ce1e0 */
extern void *nsock_recv_dest; /* 0x0c5cf1e0 */
extern char nsock_send_buf[0x1000]; /* 0x0c5cd1e0 */
extern uint nsock_recv_size; /* 0x0c5cf1e4 */

void nsock_connect_start(int sock, void *addr, int addr_len)
{
  sock_connect_start(sock,addr,addr_len);
  return;
}

int nsock_send_start(int sock, void *data, uint size, int flags)
{
  if (!(size < 0x1001)) {
    return 0xffffffff;
  }
  memcpy(nsock_send_buf,data,size);
  send_area_write((uint)nsock_send_buf,0,size);
  return sock_send_from_area_async(sock,0,size,flags);
}

void nsock_recv_start(int sock, void *dst, uint size, int flags)
{
  if (0x1000 < size) {
    size = 0x1000;
  }
  nsock_recv_dest = dst;
  nsock_recv_size = size;
  sock_recv_to_area_async(sock,0,size,flags);
  return;
}

size_t nsock_recv_collect(int sock)
{
  size_t n;
  size_t r;
  if (sock_call_done(sock) == 0) return 0;
  n = sock_result_take(sock);
  if (n + 2 <= 2) return 0;
  r = recv_area_read(0,(uint)nsock_recv_buf,n);
  if (r == 0xffffffff) {
    fatal_at("/p71/prj71p/vf4e2/naomi2lib/libnl2http/src/naomi_socket.c",0x59,
             "assertion `%s\' failed.\n","recv_size != SOCKET_ERR");
  }
  if (r != n) {
    fatal_at("/p71/prj71p/vf4e2/naomi2lib/libnl2http/src/naomi_socket.c",0x5a,
             "assertion `%s\' failed.\n","recv_size == sz");
  }
  memcpy(nsock_recv_dest,nsock_recv_buf,n);

  return n;
}
