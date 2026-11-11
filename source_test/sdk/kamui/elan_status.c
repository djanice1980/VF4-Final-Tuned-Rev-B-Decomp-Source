/*
 * sdk/kamui/elan_status.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * test.bin's copy of main.bin's source/sdk/kamui/elan_status.c: the same C
 * as main.bin's render_hang_recover (0x0C150BA0), built against test.bin's
 * symbols (the globals it reads are typed in ghidra/symbols_test/kamui.tsv).
 */
#include "vf4ft.h"

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
