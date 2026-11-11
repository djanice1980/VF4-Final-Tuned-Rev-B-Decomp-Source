/*
 * sdk/sy/syextchk.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern int ext_probe_state; /* 0x0c32a71c */

static inline unsigned int raise_imask(void)
{
  unsigned int sr;
  unsigned int lvl;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  return lvl;
}

static inline void restore_imask(unsigned int lvl)
{
  unsigned int t;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

static inline void imask_restore(uint imask)
{
  unsigned int t;

  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (imask << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

int ext_devices_probe(void)
{
  char id[2];

  if (ext_probe_state != 0) {
    return 0xffffffff;
  }
  g2_timeout_flag_clear();
  if (g2_cpu_copy_from_bus((void *)0xa0600004,&id[0],1,1,0,0) != 0) {
    return 0xffffffff;
  }
  if (g2_cpu_copy_from_bus((void *)0xa0600000,&id[1],1,1,0,0) != 0) {
    return 0xffffffff;
  }
  if (id[0] == 0x10) {
    ext_id10_found = 0xffffffff;
    switch (id[1] & 0xc0) {
    case 0:
      ext_id10_high_code = 1;
      break;
    case 0x40:
      ext_id10_high_code = 2;
      break;
    default:
      ext_id10_high_code = 0xffffffff;
      break;
    }
    {
      char v = id[1] & 0xf;
      if (v == 0) ext_id10_low_code = 1;
      else if (v == 2) ext_id10_low_code = 2;
      else if (v == 6) ext_id10_low_code = 3;
      else if (v == 0xd) ext_id10_low_code = 4;
      else ext_id10_low_code = 0xffffffff;
    }
  }
  if (id[0] == 0x12) {
    ext_id12_found = 0xffffffff;
  }
  if (ext_bridge_signature_ok(0xa1000400) == 1) {
    ext_bridge_clear_reg14(0xa1000400);
    ext_bridge_hold_off(0xa1000400);
    ext_bridge_400_found = 0xffffffff;
  }
  if (ext_bridge_signature_ok(0xa1000800) == 1) {
    ext_bridge_clear_reg14(0xa1000800);
    ext_bridge_hold_off(0xa1000800);
    ext_bridge_800_found = 0xffffffff;
  }
  if (ext_bridge_signature_ok(0xa1001400) == 1) {
    ext_bridge_clear_reg14(0xa1001400);
    ext_bridge_hold_off(0xa1001400);
    ext_bridge_1400_found = 0xffffffff;
  }
  if (ext_bridge_signature_ok(0xa1001800) == 1) {
    ext_bridge_clear_reg14(0xa1001800);
    ext_bridge_hold_off(0xa1001800);
    ext_bridge_1800_found = 0xffffffff;
  }
  g2_timeout_flag_clear();
  ext_probe_state = 0xffffffff;
  return 0;
}

int ext_devices_release(void)
{
  if (ext_probe_state != -1) return 0xffffffff;
  if (ext_bridge_400_found == -1) {
    ext_bridge_clear_reg14(0xa1000400);
    ext_bridge_hold_off(0xa1000400);
  }
  if (ext_bridge_800_found == -1) {
    ext_bridge_clear_reg14(0xa1000800);
    ext_bridge_hold_off(0xa1000800);
  }
  if (ext_bridge_1400_found == -1) {
    ext_bridge_clear_reg14(0xa1001400);
    ext_bridge_hold_off(0xa1001400);
  }
  if (ext_bridge_1800_found == -1) {
    ext_bridge_clear_reg14(0xa1001800);
    ext_bridge_hold_off(0xa1001800);
  }
  g2_timeout_flag_clear();
  ext_probe_state = 0;
  ext_id10_high_code = -2;
  ext_id10_low_code = -2;
  ext_id10_found = 0;
  ext_id12_found = 0;
  ext_bridge_400_found = 0;
  ext_bridge_800_found = 0;
  ext_bridge_1400_found = 0;
  ext_bridge_1800_found = 0;
  return 0;
}

int ext_device_status(int kind)
{
  int found;

  if (ext_probe_state != -1) return -2;
  if (kind == 1) found = ext_id10_found;
  else if (kind == 5) found = ext_id12_found;
  else if (kind == 2) found = ext_bridge_400_found;
  else if (kind == 3) found = ext_bridge_800_found;
  else if (kind == 4) found = ext_bridge_1400_found;
  else if (kind == 6) found = ext_bridge_1800_found;
  else return -2;
  if (found == -1) return 0;
  return -1;
}

int ext_id10_low_code_get(void)
{
  return ext_id10_low_code;
}

int ext_id10_high_code_get(void)
{
  return ext_id10_high_code;
}

void g2_timeout_flag_clear(void)
{
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;

  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  *(volatile uint *)0xa05f6908 |= 0x08000000;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
}

bool ext_bridge_signature_ok(uint bridge)
{
  int x;
  unsigned int sr;
  unsigned int lvl;
  unsigned int t;
  uint st;
  x = 0;
  g2_timeout_flag_clear();
  if ((int)g2_cpu_copy_from_bus((void *)bridge,&x,4,1,1,1) != 0) return 0;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  lvl = (sr & 0xf0) >> 4;
  sr |= 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  st = *(volatile uint *)0xa05f6908;
  __asm__ volatile ("stc sr,%0" : "=r" (t));
  t = (t & 0xffffff0f) | (lvl << 4);
  __asm__ volatile ("ldc %0,sr" : : "r" (t));
  if (st & 0x08000000) {
    g2_timeout_flag_clear();
    return 0;
  }
  return x == 0x53504147;
}

int ext_bridge_clear_reg14(int bridge)
{
  uint buf[1];
  unsigned int lvl;
  int r;

  buf[0] = 0;
  lvl = raise_imask();
  r = g2_cpu_copy_to_bus(buf, (void *)(bridge + 0x14), 4, 1, 1, 1);
  restore_imask(lvl);
  return r == 0;
}

int ext_bridge_hold_off(uint bridge)
{
  uint buf[1];
  unsigned int sr;
  uint imask;
  int r;

  buf[0] = ((bridge >> 8) & 0xff) << 16 | 0x5a00a500;
  __asm__ volatile ("stc sr,%0" : "=r" (sr));
  imask = (sr & 0xf0) >> 4;
  sr = sr | 0xf0;
  __asm__ volatile ("ldc %0,sr" : : "r" (sr));
  r = g2_cpu_copy_to_bus(buf,(void *)(bridge + 0x18),4,1,1,1);

  imask_restore(imask);
  return r == 0;
}
