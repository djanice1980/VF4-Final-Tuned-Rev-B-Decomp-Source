/*
 * sdk/sy/address_checker.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint *trap_dump_regs[69]; /* 0x0c2167d8 */
extern uint *trap_frame; /* 0x0c5d5c34 */
extern uint trap_serial_is_sci; /* 0x0c5d5c38 */

void address_checker_trap(uint event, uint *frame, float unused_fr4, float unused_fr5, float unused_fr6, float unused_fr7, float unused_fr8, float unused_fr9, float unused_fr10, float unused_fr11)
{
  int code;
  int r;
  int i;
  uint save;

  code = event & 0xffff;
  switch (code) {
  case 0x300:
  case 0x340:
  case 0x380:
  case 0x3c0:
    trap_frame = frame;

    r = trap_serial_setup();
    trap_serial_send_text("\r\n --- ADDRESS CHECKER TRAP --- \r\n");
    trap_print_reg_row("FR0-7:    ",10,8);
    trap_print_reg_row("FR8-15:   ",0x12,8);
    trap_print_reg_row("FR0-7(B): ",0x1a,8);
    trap_print_reg_row("FR8-15(B):",0x22,8);
    trap_serial_send_text("FPUL: ");
    trap_serial_send_hex(trap_frame[0x2a]);
    trap_serial_send_text(" FPSCR: ");
    trap_serial_send_hex(trap_frame[0x2b]);
    trap_print_reg_row("\r\nR0-7:     ",0x2d,8);
    save = trap_frame[0x3c];
    trap_frame[0x3c] = trap_frame[1];
    trap_print_reg_row("R8-15:    ",0x35,8);
    trap_frame[0x3c] = save;
    trap_print_reg_row("\r\nR0-7(B):  ",2,8);
    trap_serial_send_text("MACL: ");
    trap_serial_send_hex(trap_frame[0x3c]);
    trap_serial_send_text(" MACH: ");
    trap_serial_send_hex(trap_frame[0x3d]);
    trap_serial_send_text("\r\nVBR: ");
    trap_serial_send_hex(trap_frame[0x3e]);
    trap_serial_send_text(" GBR: ");
    trap_serial_send_hex(trap_frame[0x3f]);
    trap_serial_send_text(" DBR: ");
    trap_serial_send_hex(trap_frame[0]);
    trap_serial_send_text("\r\nPR: ");
    trap_serial_send_hex(trap_frame[0x40]);
    trap_serial_send_text(" PC: ");
    trap_serial_send_hex(trap_frame[0x41]);
    trap_serial_send_text(" SR: ");
    trap_serial_send_hex(trap_frame[0x42]);
    for (i = 0; i < 0x45; i++) {
      if ((i & 3) == 0) {
        trap_serial_send_text("\r\n");
      }
      trap_serial_send_hex((uint)trap_dump_regs[i]);
      trap_serial_send_text(":");
      trap_serial_send_hex(*trap_dump_regs[i]);
      trap_serial_send_text(" ");
    }
    trap_serial_send_text("\r\n");
    if (r == 0) {
      trap_serial_send_text("Please, cancel the interrupt from ADRESS CHECKER,\r\n then hit RETURN key.");
    }
    else {
      trap_serial_send_text("Please, cancel the interrupt from ADRESS CHECKER.");
    }
    while (trap_serial_read_key() != 0xd) {
    }
    exception_event_reg = code;
    trap_serial_teardown();
  }
}

void trap_print_reg_row(char *label, int first, int count)
{
  int i;

  trap_serial_send_text(label);
  for (i = first; i < first + count; i++) {
    trap_serial_send_hex(trap_frame[i]);
    trap_serial_send_text(" ");
  }
  trap_serial_send_text("\r\n");
  return;
}

uint trap_serial_setup(void)
{
  volatile int i;

  *(volatile uchar *)0xffc00004 &= ~1;
  i = *(volatile uchar *)0xffc00004;
  for (i = 0; i < 0x50000; i++) {
  }
  if (*(volatile uchar *)0xffe0001c & 4) trap_serial_is_sci = 0;
  else trap_serial_is_sci = 1;
  if (trap_serial_is_sci != 0) {
    *(volatile uchar *)0xffe00008 = 0;
    *(volatile uchar *)0xffe00008 = 0;
    *(volatile uchar *)0xffe00000 = 0;
    *(volatile uchar *)0xffe00004 = 0x28;
    i = 0;
    goto test;
  }
  else {
    *(volatile ushort *)0xffe80008 = 0;
    *(volatile ushort *)0xffe80018 = 6;
    *(volatile ushort *)0xffe80008 = 0;
    *(volatile ushort *)0xffe80000 = 0;
    *(volatile uchar *)0xffe80004 = 0x28;
    for (i = 0; i < 0x80000; i++) {
    }
    *(volatile ushort *)0xffe80018 = 0x308;
    *(volatile ushort *)0xffe80008 = 0x30;
  }
ret:
  return trap_serial_is_sci;
body:
  i++;
test:
  if (i < 0x80000) goto body;
  *(volatile uchar *)0xffe00008 = 0x30;
  goto ret;
}

void trap_serial_send_byte(c) char c;
{
  switch (trap_serial_is_sci) {
  case 0:
    while ((*(volatile ushort *)0xffe80010 & 0x20) == 0) ;
    *(volatile char *)0xffe8000c = c;
    *(volatile ushort *)0xffe80010 &= ~0x20;
    *(volatile ushort *)0xffe80010 &= ~0x40;
    while ((*(volatile ushort *)0xffe80010 & 0x40) == 0) ;
    break;
  case 1:
    while ((*(volatile uchar *)0xffe00010 & 0x80) == 0) ;
    *(volatile char *)0xffe0000c = c;
    *(volatile uchar *)0xffe00010 &= 0x7f;
    while ((*(volatile uchar *)0xffe00010 & 4) == 0) ;
    break;
  }
}

char trap_serial_read_key(void)
{
  char c;
  
  switch (trap_serial_is_sci) {
  case 0:
    while ((uchar)*(volatile ushort *)0xffe8001c == 0) ;
    c = *(volatile char *)0xffe80014;
    break;
  case 1:
    if (*(volatile char *)0xffe0001c & 4) c = 13;
    break;
  }
  return c;
}

void trap_serial_teardown(void)
{
  volatile ushort *new_var;
  if (trap_serial_is_sci <= 0)
  {
    if (trap_serial_is_sci)
    {
      *((volatile ushort *) 0xffe80008) = 0;
      *((volatile ushort *) 0xffe80018) = 6;
      *((volatile ushort *) 0xffe80000) = 0;
      *((volatile uchar *) 0xffe80004) = 0xff;
      *((volatile ushort *) 0xffe80008) = 0;
      *((volatile ushort *) 0xffe80010) = 0;
      *((volatile ushort *) 0xffe80018) = 0;
      *((volatile ushort *) 0xffe80020) = 0;
      *((volatile ushort *) 0xffe80024) = 0;
    }
    else
    {
      *((volatile ushort *) 0xffe80008) = 0;
      *((volatile ushort *) 0xffe80018) = 6;
      *((volatile ushort *) 0xffe80000) = 0;
      *((volatile uchar *) 0xffe80004) = 0xff;
      *(new_var = (volatile ushort *) 0xffe80008) = 0;
      *((volatile ushort *) 0xffe80010) = 0;
      *((volatile ushort *) 0xffe80018) = 0;
      *((volatile ushort *) 0xffe80020) = 0;
      *((volatile ushort *) 0xffe80024) = 0;
    }
  }
  *((volatile uchar *) 0xffc00004) |= 1;
}

void trap_serial_send_text(char *text)
{
  while (*text != 0) {
    trap_serial_send_byte(*text++);
  }
}

void trap_serial_send_hex(uint value)
{
  int i;
  uchar c;
  for (i = 28; i >= 0; i -= 4) {
    c = (value >> i) & 0xf;

    if (c > 9) c += '7'; else c += '0';
    trap_serial_send_byte((char)c);
  }
  return;
}
