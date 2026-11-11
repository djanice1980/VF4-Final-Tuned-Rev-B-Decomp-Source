/*
 * sdk/nlasock/dimm_link.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* the DIMM board's command registers: dimm_cmd_post waits for bit 0 of the
   status word, then writes a command and its three operands; meanwhile it
   masks bit 3 of the level-2 external interrupt mask */
#define IRQ_L2_EXT_MASK ((volatile uint *)0xa05f6914)
#define DIMM_REG_COMMAND ((volatile uint *)0xa05f703c)
#define DIMM_REG_OFFSET ((volatile uint *)0xa05f7040)
#define DIMM_REG_PARAM_LO ((volatile uint *)0xa05f7044)
#define DIMM_REG_PARAM_HI ((volatile uint *)0xa05f7048)
#define DIMM_REG_STATUS ((volatile uint *)0xa05f704c)

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */

char * nlasock_version_get(void)
{
  return "\nNLASOCK Ver.0.12 Build: Oct 19 2004 19:52:09\n";
}

int dimm_net_attach(uint unused)
{
  dimm_board_info_rec *info;
  int i;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  info = dimm_board_info_fetch();
  busy_word_clear(&dimm_busy_word);
  dimm_cmd_unhandled.cmd_word = 0xffff;
  dimm_cmd_unhandled.data_offset = 0xffff;
  dimm_cmd_unhandled.param_lo = 0xffff;
  dimm_cmd_unhandled.param_hi = 0xffff;
  for (i = 0; i < 0x40; i++) {
    dimm_cmd_handler_table[i] = 0;
  }
  sock_slots_prepare(info->module_base);
  ctl_channel_prepare(info->module_base);
  peer_area_reset(info->module_base);
  dimm_cmd24_hook_install(info->module_base);
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  dimm_hook_saved = dimm_cmd_hook_vector;
  dimm_cmd_hook_vector = (uint)dimm_cmd_dispatch;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return 0;
}

int dimm_net_detach(void)
{
  int i;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  dimm_cmd_hook_vector = dimm_hook_saved;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  for (i = 0; i < 0x40; i++) {
    dimm_cmd_handler_table[i] = 0;
  }
  return 0;
}

int dimm_cmd_handler_put(uint cmd, void *routine)
{
  if (!(cmd < 0x40)) {
    return -1;
  }
  dimm_cmd_handler_table[cmd] = (dimm_cmd_handler_fn)routine;
  return 0;
}

int dimm_cmd_handler_clear(uint id)
{
  if (id >= 0x40) {
    return -1;
  }
  dimm_cmd_handler_table[id] = 0;
  return 0;
}

int dimm_cmd_post(uint cmd, uint sub, uint offset, uint param_lo, uint param_hi)
{
  uint v;
  dimm_link_flags = dimm_link_flags | 8;
  v = *IRQ_L2_EXT_MASK;
  v &= 0xfffffff7;
  *IRQ_L2_EXT_MASK = v;
  while ((*DIMM_REG_STATUS & 1) == 0) {
  }
  *DIMM_REG_COMMAND = (cmd & 0x3f) << 9 | sub & 0xff;
  *DIMM_REG_OFFSET = offset;
  *DIMM_REG_PARAM_LO = param_lo;
  *DIMM_REG_PARAM_HI = param_hi;
  bios_helper_60(1);
  v |= 8;
  *IRQ_L2_EXT_MASK = v;
  dimm_link_flags = dimm_link_flags & 0xfffffff7;
  return 0;
}

int dimm_cmd_post_bare(uint cmd, uint sub)
{
  int r;

  r = dimm_cmd_post(cmd,sub,0,0,0);
  return r;
}

int dimm_cmd_dispatch(uint cmd_word, uint offset, uint param_lo, uint param_hi)
{
  int r;
  uint i;

  i = cmd_word >> 9 & 0x3f;
  r = 0xff;
  if (dimm_cmd_handler_table[i] != 0) {
    r = dimm_cmd_handler_table[i](cmd_word,offset,param_lo,param_hi);
  }
  else {
    dimm_cmd_unhandled.cmd_word = cmd_word;
    dimm_cmd_unhandled.data_offset = offset;
    dimm_cmd_unhandled.param_lo = param_lo;
    dimm_cmd_unhandled.param_hi = param_hi;
  }
  return r;
}

void busy_word_clear(int *word)
{
  *word = 0;
  return;
}

uint busy_word_drop(int *word)
{
  uint held;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  held = (uint)(0 < *word);
  *word = *word - held;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return held;
}

int busy_word_try_set(int *word)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  int v;
  int r;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  v = *word;
  r = v < 1;
  *word = r + v;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  return r;
}
