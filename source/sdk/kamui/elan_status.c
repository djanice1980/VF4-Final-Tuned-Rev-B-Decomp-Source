/*
 * sdk/kamui/elan_status.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define NOPROTO_xfer_job_next
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint xfer_pending[3]; /* 0x0c41e9d4 */
extern xfer_job_rec *xfer_current; /* 0x0c41e9e4 */
extern uint list_kind_sent; /* 0x0c41e9e8 */
extern uint bank_render_index; /* 0x0c41e9ec */
extern uint xfer_dma_queued; /* 0x0c41e9f8 */
extern uint xfer_trace_next; /* 0x0c41e9fc */
extern int xfer_done_count; /* 0x0c41ea00 */
extern uint xfer_trace_last; /* 0x0c41ea08 */
extern uint xfer_next_ticket; /* 0x0c41ea0c */
extern uint sq_save_top; /* 0x0c41eb28 */
extern uchar xfer_trace_ring; /* 0x0c607010 */
extern volatile gfx_state_rec gfx_state; /* 0x0c41e010 */
extern elan_rev_rec elan_rev_config; /* 0x0c41eb20 */
extern uchar list_end_irq_bits; /* 0x0c216be0 */
extern uint xfer_retry_mode; /* 0x0c32ac60 */
extern uint tile_split_parked; /* 0x0c617010 */

void elanirq_check_or_halt(int ok)
{
  if (ok == 0) {
    *(volatile uint *)0xa85f8040 = 0xff00ff;
    for (;;) ;
  }
}

void elanirq_sq_save(uint *save)
{
  uint s;
  uint old;
  uint l0;
  uint t;
  uint p;
  uint v;
  __asm__ volatile ("stc sr,%0" : "=r" (s));
  l0 = (s & 0xf0) >> 4;
  old = l0;
  s = s | 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (s));
  p = sq_save_top & 0x3ffffff | 0xe0000000;
  elanirq_check_or_halt((p & 0x3f) == 0);
  save[0] = *(volatile uint *)0xff000038;
  save[1] = *(volatile uint *)0xff00003c;
  save[2] = sq_save_top;
  sq_save_top = sq_save_top + 0x40;
  v = sq_save_top >> 0x18 & 0x1c;
  *(volatile uint *)0xff000038 = v;
  *(volatile uint *)0xff00003c = v;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p += 32;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  ((volatile uint *)0xff000038)[0] = save[0];
  ((volatile uint *)0xff000038)[1] = save[1];
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (old << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void elanirq_sq_restore(uint *save)
{
  uint s;
  uint old;
  uint t;
  uint *sq;
  uint *p;
  __asm__ volatile ("stc sr,%0" : "=r" (s));
  old = (s & 0xf0) >> 4;
  s = s | 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (s));
  sq = (uint *)0xe0000000;
  p = (uint *)(save[2] & 0x1fffffff | 0xa0000000);
  sq_save_top = sq_save_top + -0x40;
  elanirq_check_or_halt(save[2] == sq_save_top);
  *(volatile uint *)0xff000038 = save[0];
  *(volatile uint *)0xff00003c = save[1];
  sq[0] = p[0];
  sq[1] = p[1];
  sq[2] = p[2];
  sq[3] = p[3];
  sq[4] = p[4];
  sq[5] = p[5];
  sq[6] = p[6];
  sq[7] = p[7];
  sq[8] = p[8];
  sq[9] = p[9];
  sq[10] = p[10];
  sq[11] = p[11];
  sq[12] = p[12];
  sq[13] = p[13];
  sq[14] = p[14];
  sq[15] = p[15];
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (old << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

void xfer_trace_push(uint event)
{
  uchar c = event;
  int t;
  if (*(volatile int *)&xfer_trace_last != -1) {
    t = (&xfer_trace_ring)[*(volatile int *)&xfer_trace_last];
    switch (t) {
    case 0: elanirq_check_or_halt((uchar)(c - 1) < 2); break;
    case 1: case 2: elanirq_check_or_halt(c == 0); break;
    default: elanirq_check_or_halt(0); break;
    }
  }
  (&xfer_trace_ring)[*(volatile int *)&xfer_trace_next] = c;
  *(volatile int *)&xfer_trace_last = *(volatile int *)&xfer_trace_next;
  *(volatile int *)&xfer_trace_next = *(volatile int *)&xfer_trace_next + 1 & 0xffff;
}

void sq_copy_blocks(uint *src, uint dst, uint len)
{
  uint q;
  uint i;
  uint *p;
  q = dst >> 24 & 0x1c;
  ((volatile uint *)0xff000038)[0] = q;
  ((volatile uint *)0xff000038)[1] = q;
  p = (uint *)(dst & 0x3ffffff | 0xe0000000);
  len >>= 5;
  for (i = 0; i < len; i++, p += 8, src += 8) {
    p[0] = src[0];
    p[1] = src[1];
    p[2] = src[2];
    p[3] = src[3];
    p[4] = src[4];
    p[5] = src[5];
    p[6] = src[6];
    p[7] = src[7];
    __asm__ __volatile__ ("pref @%0" : : "r" (p));
  }
}

int block_write_ticketed(uint src, uint dst, uint len)
{
  if ((elan_rev_config.flags & 1) != 0) {
    if ((dst & 0x1f800000) != 0x09000000) {
      sq_copy_blocks((uint *)src,dst,len);
      return -1;
    }
  }
  else {
    if (gfx_state.kept_list == 0 && gfx_state.single_bank == 1) {
      sq_copy_blocks((uint *)src,dst,len);
      return -1;
    }
    if (((src | dst | len) & 0x1f) != 0) {
      sq_copy_blocks((uint *)src,dst,len);
      return -1;
    }
    if (((0x1e000000 & src) == 0x0c000000) | ((0x1e000000 & src) == 0x0a000000)) {
      xfer_dma_enqueue(src,len,dst);
      *(volatile int *)&xfer_dma_queued = xfer_dma_queued + 1;
    }
    else {
      sq_copy_blocks((uint *)src,dst,len);
      return -1;
    }
  }
  return xfer_next_ticket++;
}

int cmd_area_write_ticketed(uint src, uint len)
{
  int ticket;
  
  ticket = block_write_ticketed(src,0xa9000000,len);
  return ticket;
}

bool ticket_done_poll(int ticket)
{
  int pending;
  
  if (!(ticket >= xfer_done_count)) {
    return true;
  }
  pvr_irq_service();
  pending = ticket >= xfer_done_count;
  return pending == 0;
}

void ticket_wait(int ticket)
{
  int done;
  
  do {
    done = ticket_done_poll(ticket);
  } while (done == 0);
  return;
}

int elan_status_service(void)
{
  uint v;
  int r;
  unsigned int sr;
  unsigned int lvl;
  unsigned int m;
  uint sq_save[3];
  uint sq;
  uint *p;
  volatile list_bank_rec *tbl;
  v = *(volatile uint *)0xa8800074;
  v ^= 0x3c;
  if ((v & 3) == 0) {
    if ((v & 0xc) && (v & 0x30)) return 6;
    if (v & 0xc) return 1;
    if (v & 0x30) return 2;
  }
  else {
    r = 0;
    if (v & 1) {
      *(volatile uint *)0xa8800074 = 1;
      __asm__ volatile ("stc sr,%0" : "=r" (sr));
      m = sr & 0xf0;
      lvl = m >> 4;
      sr |= 0xf0;
      __asm__ volatile ("ldc %0,sr" : : "r" (sr));
      xfer_done_count++;
      elanirq_sq_save(sq_save);
      xfer_trace_push(1);
      xfer_job_retire();
      xfer_job_next();
      xfer_dma_finished++;
      elanirq_sq_restore(sq_save);
      {
        unsigned int t;
        __asm__ volatile ("stc sr,%0" : "=r" (t));
        t = (t & 0xffffff0f) | (lvl << 4);
        __asm__ volatile ("ldc %0,sr" : : "r" (t));
      }
    }
    if (v & 2) {
      *(volatile uint *)0xa8800074 = 2;
      __asm__ volatile ("stc sr,%0" : "=r" (sr));
      m = sr & 0xf0;
      lvl = m >> 4;
      sr |= 0xf0;
      __asm__ volatile ("ldc %0,sr" : : "r" (sr));
      elanirq_sq_save(sq_save);
      switch (((volatile list_bank_rec *)list_banks)[bank_render_index].state) {
      case 1:
        {
          volatile uint *hw = (volatile uint *)0xa85f8188;
          hw[12] = 0xf0f0f0f0;
        }
        if (*(volatile int *)0xa8800030 != (int)0x80000000) xfer_retry_mode = 0;
        xfer_trace_push(1);
        xfer_job_retire();
        ((volatile list_bank_rec *)list_banks)[bank_render_index].job_count--;
        if (((volatile list_bank_rec *)list_banks)[bank_render_index].job_count == 0) {
          sq = 0xa9000000;
          while (*(volatile uint *)0xa880000c >= elan_rev_config.queue_limit) ;
          *(volatile uint *)0xff000038 = sq >> 24;
          *(volatile uint *)0xff00003c = sq >> 24;
          p = (uint *)(sq & 0x3ffffff | 0xe0000000);
          p[0] = 0;
          p[1] = 0;
          p[2] = 0;
          p[3] = 0;
          p[4] = 0;
          p[5] = 0;
          p[6] = 0;
          p[7] = 0;
          __asm__ __volatile__ ("pref @%0" : : "r" (p));
          tbl = (volatile list_bank_rec *)list_banks;
          tbl[*(volatile uint *)&bank_render_index].state = 2;
          elan_cmd_wait_list_end(tbl[*(volatile uint *)&bank_render_index].last_list);
          r = 5;
        }
        else {
          elanirq_check_or_halt(((volatile list_bank_rec *)list_banks)[bank_render_index].job_count != 0);
          xfer_job_next();
          r = 5;
        }
        break;
      case 2:
        {
          volatile uint *hw = (volatile uint *)0xa85f818c;
          hw[12] = 0xf0f0f0f0;
        }
        if (elan_rev_config.flags & 0x20) {
          *(volatile uint *)0xa8800008 = 1;
          *(volatile uint *)0xa8800008 = 0;
        }
        list_kind_sent = 0xffffffff;
        if (!(elan_rev_config.flags & 4) || (*(volatile uint *)0xa05f6900 & *(volatile uint *)0xa25f6900 & ((uint *)&list_end_irq_bits)[((volatile list_bank_rec *)list_banks)[bank_render_index].last_list]) != 0) {
          *(volatile uint *)0xa05f6908 = 0x10;
          *(volatile uint *)0xa25f6908 = 0x10;
          ((volatile list_bank_rec *)list_banks)[*(volatile uint *)&bank_render_index].state = 0;
          *(volatile uint *)&bank_render_index = *(volatile uint *)&bank_render_index + 1 & 3;
          r = 4;
        }
        break;
      }
      elanirq_sq_restore(sq_save);
      {
        unsigned int t;
        __asm__ volatile ("stc sr,%0" : "=r" (t));
        t = (t & 0xffffff0f) | (lvl << 4);
        __asm__ volatile ("ldc %0,sr" : : "r" (t));
      }
      return r;
    }
  }
  return 0;
}

void elan_pvr_reset_drop_frame(void)
{
  uint i;
  
  *(volatile uint *)0xa05f6808 = 0;
  *(volatile uint *)0xa8800008 = 5;
  for (i = 0; i < 4; i++) {
    *(volatile uint *)0xa8800000 = 0;
    *(volatile uint *)0xa8800004 = 0;
  }
  *(volatile uint *)0xa8800008 = 0;
  *(volatile uint *)0xa85f8008 = 1;
  *(volatile uint *)0xa85f8008 = 0;
  while (*(volatile uint *)0xa05f6808 != 0) ;
  *(volatile uint *)0xffa0002c &= ~1;
  *(volatile uint *)0xa85f8008 = 1;
  *(volatile uint *)0xa85f8008 = 0;
  *(volatile uint *)0xa85f6900 = 0x200780;
  list_banks[bank_render_index].state = 0;
  bank_render_index = *(volatile uint *)&bank_render_index + 1 & 3;
  if (xfer_current != (xfer_job_rec *)0x0) {
    xfer_trace_push(2);
    xfer_job_retire();
  }
  jobq_reset((int *)xfer_pending,1);
  return;
}

void render_hang_recover(void)
{
  int order[5];
  volatile uint dummy;
  int i;
  int last;
  int sel;
  uint sq;
  uint cnt;
  int k;
  uint hi;
  uint ta_pos1;
  uint ta_pos2;
  uint end1;
  uint end2;
  int v;
  int flag;
  uint *p;
  uint *q;
  xfer_job_rec *job;
  volatile list_bank_rec *banks;

  order[0] = 0;
  order[1] = 1;
  order[2] = 2;
  order[3] = 3;
  order[4] = 4;
  sq = 0xa9000000;
  cnt = 0;
  {
    volatile uint *hw = (volatile uint *)0xa85f8190;
    hw[12] = 0xf0f0f0f0;
  }
  if (((volatile list_bank_rec *)list_banks)[bank_render_index].state == 1) {
    *(volatile uint *)0xa05f6808 = 0;
    *(volatile uint *)0xa8800008 = 1;
    for (k = 0; k < 0x20; k++) {
      *(volatile uint *)0xa8800000 = 0;
      *(volatile uint *)0xa8800004 = 0;
    }
    *(volatile uint *)0xa8800008 = 0;
    *(volatile uint *)0xa8800008 = 4;
    for (k = 0; k < 0x20; k++) {
      *(volatile uint *)0xa8800000 = 0;
      *(volatile uint *)0xa8800004 = 0;
    }
    *(volatile uint *)0xa8800008 = 0;
    while (*(volatile uint *)0xa05f6808 != 0) ;
    *(volatile uint *)0xffa0002c &= 0xfffffffe;
    *(volatile uint *)0xa8800030 = tile_split_word;
    v = *(volatile uint *)0xa8800074;
    *(volatile uint *)0xa8800074 = v & 3;
    do {
      ta_pos1 = *(volatile uint *)0xa05f8138;
      ta_pos2 = *(volatile uint *)0xa25f8138;
      busy_wait_scaled(0.01);
    } while (ta_pos1 != *(volatile uint *)0xa05f8138 || ta_pos2 != *(volatile uint *)0xa25f8138);
    *(volatile uint *)0xff000038 = sq >> 24;
    *(volatile uint *)0xff00003c = sq >> 24;
    p = (uint *)(sq & 0x3ffffff | 0xe0000000);
    hi = sq & 0xfc000000;
    p[0] = 0x20000000;
    p[1] = 0x20000000;
    p[2] = 0x20000000;
    p[3] = 0x20000000;
    p[4] = 0x20000000;
    p[5] = 0x20000000;
    p[6] = 0x20000000;
    p[7] = 0x20000000;
    list_end_block_count = list_end_block_count + 1 & 3;
    __asm__ __volatile__ ("pref @%0" : : "r" (p));
    p += 8;
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    p[5] = 0;
    p[6] = 0;
    p[7] = 0;
    __asm__ __volatile__ ("pref @%0" : : "r" (p));
    sq = (uint)(p + 8) & 0x3ffffff | hi;
    elan_cmd_wait_list_end(list_kind_sent);
    banks = (volatile list_bank_rec *)list_banks;
    while (*(xfer_job_rec * volatile *)&xfer_current != 0 && banks[bank_render_index].state == 1) {
      if ((*(xfer_job_rec * volatile *)&xfer_current)->dma_ctrl != 0 && (*(xfer_job_rec * volatile *)&xfer_current)->mode != 0) {
        elanirq_check_or_halt(0);
      }
      xfer_trace_push(1);
      xfer_job_retire();
      job = (xfer_job_rec *)jobq_pop((int *)xfer_pending);
      xfer_current = job;
      if (*(xfer_job_rec * volatile *)&xfer_current != 0) {
        xfer_trace_push(0);
        if ((*(xfer_job_rec * volatile *)&xfer_current)->dma_ctrl != 0 && (*(xfer_job_rec * volatile *)&xfer_current)->mode != 0) {
          elanirq_check_or_halt(0);
        }
      }
      {
        volatile list_bank_rec *tbl = (volatile list_bank_rec *)list_banks;
        if (--tbl[*(volatile int *)&bank_render_index].job_count != 0) {
          elanirq_check_or_halt(tbl[*(volatile int *)&bank_render_index].job_count != 0);
        }
      }
    }
    if (gfx_state.direct_list != 0xff) {
      elanirq_check_or_halt(gfx_state.direct_list < 5);
      for (i = gfx_state.direct_list; i > 0; i--) {
        order[i] = order[i - 1];
      }
      order[0] = gfx_state.direct_list;
    }
    for (i = 0; i < 5; i++) {
      if (order[i] == *(volatile int *)&list_kind_sent) break;
    }
    elanirq_check_or_halt(i != 5);
    for (k = 4; k >= 0; k--) {
      last = order[k];
      if ((gfx_state.send_mask & (1 << last)) != 0) {
        sel = last;
        break;
      }
    }
  }
  do {
    v = *(volatile uint *)0xa8800074;
    v ^= 0x3c;
    if ((v & 2) == 0) {
      busy_wait_scaled(1.0);
      cnt++;
    }
  } while (((v & 2) == 0) & (cnt < 6));
  if (cnt == 6) {
    {
      volatile uint *hw = (volatile uint *)0xa85f8194;
      hw[12] = 0xf0f0f0f0;
    }
    *(volatile uint *)0xa8800008 = 1;
    for (k = 0; k < 0x20; k++) {
      *(volatile uint *)0xa8800000 = 0;
      *(volatile uint *)0xa8800004 = 0;
    }
    *(volatile uint *)0xa8800008 = 0;
    *(volatile uint *)0xa8800008 = 4;
    for (k = 0; k < 0x20; k++) {
      *(volatile uint *)0xa8800000 = 0;
      *(volatile uint *)0xa8800004 = 0;
    }
    *(volatile uint *)0xa8800008 = 0;
  }
  for (k = 0; k < 0x20; k++) {
    *(volatile uint *)0xa8800000 = 0;
    *(volatile uint *)0xa8800004 = 0;
  }
  if (((volatile list_bank_rec *)list_banks)[bank_render_index].state == 1) {
    flag = 0;
    end1 = *(volatile uint *)0xa05f8138;
    end2 = *(volatile uint *)0xa25f8138;
    *(volatile uint *)0xa85f8008 = 1;
    *(volatile uint *)0xa85f8008 = 0;
    if ((ta_pos1 < end1) | (ta_pos2 < end2)) {
      volatile uint *hw = (volatile uint *)0xa85f81a0;
      hw[12] = 0xf0f0f0f0;
      flag = 1;
    }
    elan_ctrl_bit0_off_or_back(0);
    for (; end1 > ta_pos1; ta_pos1 += 4) {
      uint vram = 0xa5000000;
      *(volatile uint *)(ta_pos1 + vram) = 0x08000000;
    }
    for (; end2 > ta_pos2; ta_pos2 += 4) {
      uint vram = 0xa7000000;
      *(volatile uint *)(ta_pos2 + vram) = 0x08000000;
    }
    if (flag) {
      volatile uint *hw = (volatile uint *)0xa85f81a0;
      hw[12] = 0xf0f0f0f0;
    }
    elan_ctrl_bit0_off_or_back(1);
    ((volatile list_bank_rec *)list_banks)[bank_render_index].state = 2;
    if (sel != list_kind_sent) {
      *(volatile uint *)0xa8800074 = 2;
      *(volatile uint *)0xa05f8128 = end1;
      *(volatile uint *)0xa25f8128 = end2;
      *(volatile uint *)0xa85f8144 = 0x80000000;
      last = sel;
      for (k = sel; k > i; k--) {
        if (k == sel) {
          list_seg_link_to((int *)&list_banks[*(volatile int *)&bank_render_index].lists[order[k]].first_seg,
                           (uint *)&list_banks[*(volatile int *)&bank_render_index].lists[order[k]].end_seg);
          list_seg_link_to((int *)&list_banks[*(volatile int *)&bank_render_index].lists[order[k]].end_seg,
                           (uint *)&blank_end_block);
        }
        else if ((gfx_state.send_mask & (1 << order[k])) != 0) {
          list_seg_link_to((int *)&list_banks[*(volatile int *)&bank_render_index].lists[order[k]].first_seg,
                           (uint *)&list_banks[*(volatile int *)&bank_render_index].lists[order[k]].end_seg);
          list_seg_link_to((int *)&list_banks[*(volatile int *)&bank_render_index].lists[order[k]].end_seg,
                           (uint *)&list_banks[*(volatile int *)&bank_render_index].lists[order[last]].first_seg);
          last = k;
        }
      }
      dummy = *(volatile uint *)0xaa000000;
      *(volatile uint *)0xff000038 = sq >> 24;
      *(volatile uint *)0xff00003c = sq >> 24;
      q = (uint *)(sq & 0x3ffffff | 0xe0000000);
      q[0] = 0x08000f00;
      q[1] = list_banks[bank_render_index].lists[last].first_seg.addr + 0x20 & 0x1fffff8;
      q[2] = 0x09000000;
      q[3] = list_banks[bank_render_index].lists[last].first_seg.length - 0x20;
      __asm__ __volatile__ ("pref @%0" : : "r" (q));
      elan_cmd_wait_list_end(order[sel]);
      do {
        v = *(volatile uint *)0xa8800074;
      } while ((v & 2) == 0);
    }
  }
  for (k = 0; k < 0x20; k++) {
    *(volatile uint *)0xa8800000 = 0;
    *(volatile uint *)0xa8800004 = 0;
  }
  if (elan_status_service() != 4) {
    elanirq_check_or_halt(0);
  }
  jobq_reset((int *)xfer_pending,1);
}

bool elan_pvr_reset_retry_frame(void)
{
  volatile uint *pvr = (volatile uint *)0xa85f8198;
  uint i;

  pvr[12] = 0xf0f0f0f0;
  *(volatile uint *)0xa05f6808 = 0;
  *(volatile uint *)0xa8800008 = 1;
  for (i = 0; i < 4; i++) {
    *(volatile uint *)0xa8800000 = 0;
    *(volatile uint *)0xa8800004 = 0;
  }
  *(volatile uint *)0xa8800008 = 0;
  *(volatile uint *)0xa8800008 = 4;
  for (i = 0; i < 4; i++) {
    *(volatile uint *)0xa8800000 = 0;
    *(volatile uint *)0xa8800004 = 0;
  }
  *(volatile uint *)0xa8800008 = 0;
  *(volatile uint *)0xa85f8008 = 1;
  *(volatile uint *)0xa85f8008 = 0;
  while (*(volatile uint *)0xa05f6808 != 0) ;
  *(volatile uint *)0xffa0002c &= 0xfffffffe;
  *(volatile uint *)0xa85f8008 = 1;
  *(volatile uint *)0xa85f8008 = 0;
  *(volatile uint *)0xa85f6900 = 0x200780;
  *(volatile uint *)0xffa0002c &= 0xfffffffe;
  if (xfer_retry_mode != 0) {
    *(volatile uint *)0xa85f8000 = xfer_retry_mode;
    if (xfer_retry_mode == 2) {
      tile_split_parked = *(volatile uint *)0xa8800030;
      *(volatile uint *)0xa8800030 = 0x80000000;
    }
    else if (xfer_retry_mode == 1) {
      *(volatile uint *)0xa8800030 = tile_split_parked;
    }
    *(volatile uint *)0xa85f8144 = 0x80000000;
    xfer_job_restart();
  }
  else {
    *(volatile int *)&list_banks[*(volatile uint *)&bank_render_index].state = 0;
    bank_render_index = bank_render_index + 1 & 3;
    if (xfer_current != (xfer_job_rec *)0x0) {
      xfer_trace_push(2);
      xfer_job_retire();
    }
    jobq_reset((int *)xfer_pending,1);
  }
  return xfer_retry_mode != 0;
}

void ta_timeout_hook(void)
{
  return;
}

void elan_cmd_wait_list_end(uint kind)
{
  uint *q;
  uint v;

  v = ((uint *)&list_end_irq_bits)[kind];
  while (*(volatile uint *)0xa880000c >= elan_rev_config.queue_limit) ;
  *(volatile uint *)0xff000038 = 0xa9;
  *(volatile uint *)0xff00003c = 0xa9;
  {
  uint *q = (uint *)0xe1000000;
  q[0] = 0x08000e00;
  q[1] = 0x025f6903;
  q[2] = 0x1080000d;
  q[3] = v;
  q[4] = 0x08000000;
  q[6] = 0x08000000;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  }
  while (*(volatile uint *)0xa880000c >= elan_rev_config.queue_limit) ;
  *(volatile uint *)0xff000038 = 0xa9;
  *(volatile uint *)0xff00003c = 0xa9;
  {
  uint *q = (uint *)0xe1000000;
  q[0] = 0x08000e00;
  q[1] = 0x005f6903;
  q[2] = 0x1080000d;
  q[3] = v;
  q[4] = 0x08000000;
  q[6] = 0x08000000;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  }
  while (*(volatile uint *)0xa880000c >= elan_rev_config.queue_limit) ;
  *(volatile uint *)0xff000038 = 0xa9;
  *(volatile uint *)0xff00003c = 0xa9;
  q = (uint *)0xe1000000;
  q[0] = 0x08000f00;
  q[1] = 0x40000000;
  q[2] = 0x09000000;
  q[3] = 0;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
}
