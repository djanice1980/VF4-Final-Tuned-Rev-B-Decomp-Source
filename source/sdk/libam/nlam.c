/*
 * sdk/libam/nlam.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"
#define BIOS_ROM_SIGNATURE ((char *)0xa01ffd00)
/* the real-time clock: a seconds count, high and low 16 bits in two 32-bit
 * registers (as the homebrew libnaomi's rtc.c documents) */
#define RTC ((volatile unsigned int *)0xa0710000)

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern undefined credit_state; /* 0x0c3e11c4 */
extern uint boot_rtc_seconds; /* 0x0c3e1bb4 */
extern undefined4 coin_input_held; /* 0x0c3e1d44 */
extern board_settings_rec board_settings; /* 0x0c3e0378 */
extern mie_start_rec mie_start_state; /* 0x0c3e12a8 */
extern card_unit_pair card_units; /* 0x0c3e1af4 */
extern coin_settings_rec coin_settings; /* 0x0c3e2b4c */

void board_serial_numbers(uchar *serial_1, uchar *serial_2)
{
  int i;
  uchar *s;
  uchar *d;
  s = board_serial_1;
  d = serial_1;
  for (i = 0; i < 0x10; i++) *d++ = *s++;
  *d = 0;
  s = board_serial_2;
  d = serial_2;
  for (i = 0; i < 0x10; i++) *d++ = *s++;
  *d = 0;
}

int media_board_probe(uint *info)
{
  typedef struct { uint lo : 30; uint hi : 2; } B;
  uint v;
  uint x;
  uint h;
  uint two = 2;

  if (info == (uint *)0x0) {
    return 1;
  }
  if (*(volatile int *)0xa05f7418 != 0) {
    return 1;
  }
  if (*(volatile ushort *)0xa05f703c != 0xffff) {
    info[0] = 2;
    v = dimm_board_id_word;
    h = v >> 30 & 3;
    if (h == 1) x = 1;
    else if (h != 2) x = 0;
    else x = two;
    info[1] = x;
    info[2] = v << 20;
    if ((v == 0) | (v == 0xffffffff)) {
      return 2;
    }
  }
  else if (*(volatile ushort *)0xa05f7014 != 0xffff) {
    info[0] = 1;
    info[1] = 0;
    info[2] = 0x0c000000;
  }
  else {
    info[0] = 0;
    info[1] = 0;
    info[2] = 0x0b000000;
  }
  return 0;
}

float nlam_version_number_get(void)
{
  return nlam_version_number;
}

int board_config_validate(uint *config)
{
  int bad;
  bad = 0;
  if ((uint)config[1] > 1) bad++;
  if ((uint)config[2] > 1) bad++;
  if (bad != 0) {
    *config = 0;
    config[1] = 1;
    config[2] = 0;
    config[3] = 1;
  }
  return bad;
}

void coin_config_pack(coin_settings_rec *settings, uchar *packed)
{
  int i;
  char *s = (char *)settings;
  char *d = (char *)packed;

  d[1] = (unsigned char)(s[4] << 4) + s[8];
  *d = *s;
  d[2] = s[0x10];
  d[3] = s[0x40];
  d[4] = s[0x34];
  d[5] = s[0x38];
  d[6] = s[0x3c];
  for (i = 0; i < 4; i++) {
    d[i + 7] = (unsigned char)(s[i * 8 + 0x14] << 4) + s[i * 8 + 0x18];
  }
  return;
}

int coin_config_unpack(uchar *packed, uint *settings, int country)
{
  char c;
  uint *q;
  int i;
  uint rates [4][2];
  uint max_credits [8];

  max_credits[0] = 9;
  max_credits[1] = 0x18;
  max_credits[2] = 0x18;
  max_credits[3] = 9;
  max_credits[4] = 9;
  max_credits[5] = 9;
  max_credits[6] = 9;
  max_credits[7] = 9;
  settings[1] = (uint)(packed[1] >> 4);
  settings[2] = (int)(char)packed[1] & 0xf;
  *settings = max_credits[country];
  settings[4] = (uint)packed[2];
  settings[0x10] = (uint)packed[3];
  settings[0xd] = (uint)packed[4];
  settings[0xe] = (uint)packed[5];
  settings[0xf] = (uint)packed[6];
  for (i = 0; i < 4; i++) {
    q = (uint *)(i * 8 + (int)rates);
    c = *(char *)(packed + i + 7);
    q[0] = (uint)((byte)c >> 4);
    q[1] = c & 0xf;
  }
  for (i = 0; i < 8; i++) {
    settings[i + 5] = ((uint *)rates)[i];
  }
  return bios_coin_config_check((coin_settings_rec *)settings);
}

int board_config_unpack(uchar *packed, uint *config, uint country)
{
  int result;

  *config = country;
  config[3] = 1;
  if ((dip_switches & 1) == 0) {
    config[3] = 0;
  }
  config[1] = (uint)(*packed >> 4);
  config[2] = (int)(char)*packed & 0xf;
  *(uchar *)(config + 4) = packed[1];
  *(uchar *)((int)config + 0x11) = packed[2];
  *(uchar *)((int)config + 0x12) = packed[3];
  *(uchar *)((int)config + 0x13) = packed[4];
  result = board_config_validate(config);
  return result;
}

void board_config_pack(char *config, uchar *packed)
{
  *packed = ((uint)config[4] << 4) + config[8];
  packed[1] = config[0x10];
  packed[2] = config[0x11];
  packed[3] = config[0x12];
  packed[4] = config[0x13];
}

int coin_config_from_args(int max_credit, uint value_4, uint separate_credits, int coin_setting, uint rate_1, uint rate_2, uint rate_3, uint rate_0, uint price_0, uint price_1)
{
  byte *p;
  int i;
  int r;

  coin_settings.max_credit = max_credit;
  coin_settings.field_0x4 = value_4;
  coin_settings.separate_credits = separate_credits;
  coin_settings.coin_setting = coin_setting;
  coin_settings.price_0 = price_0;
  coin_settings.price_1 = price_1;
  for (i = 2; i < 8; i++) {
    ((struct { char pad[20]; int f; } *)((int *)&coin_settings + i))->f = 1;
  }
  if (coin_setting != 0x1b) {
    p = (byte *)bios_coin_setting(coin_setting);
    coin_settings.coin_rate_0 = *p++;
    coin_settings.coin_rate_1 = *p++;
    coin_settings.coin_rate_2 = *p++;
    coin_settings.coin_rate_3 = *p;
  }
  else {
    coin_settings.coin_rate_0 = rate_0;
    coin_settings.coin_rate_1 = rate_1;
    coin_settings.coin_rate_2 = rate_2;
    coin_settings.coin_rate_3 = rate_3;
  }
  r = bios_coin_config_check(&coin_settings);
  if (r != 0) return r;
  coin_settings.max_credit = max_credit;
  if (max_credit - 1U >= 999) return 1;
  coin_config_pack(&coin_settings,eeprom_coin_config);
  eeprom_flush();
  bios_credit_init(&coin_settings,&credit_state,credit_ledger,credit_status);
  bios_helper_34(sram_bank_flag);
  return 0;
}

int board_config_from_args(uint country, uint value_4, uint value_8, uint dip_switch_1)
{
  int bad;
  uint config[5];

  config[0] = country;
  config[1] = value_4;
  config[2] = value_8;
  config[3] = dip_switch_1;
  bad = board_config_validate(config);
  if (bad != 0) return bad;
  board_settings.country = country;
  board_settings.field_0x4 = value_4;
  board_settings.field_0x8 = value_8;
  board_settings.dip_switch_1 = dip_switch_1;
  return 0;
}

uint * board_config_ptr(void)
{
  return &board_settings.country;
}

void bios_signature_check(void)
{
  int i;
  char *p;
  char *q;

  bios_signature_found = 0;
  p = bios_signature;
  q = BIOS_ROM_SIGNATURE;
  for (i = 0; i < 0x70; i++) {
    if (*p - i % 8 != *q) return;
    p++;
    q++;
  }
  bios_signature_found = 1;
}

int bios_signature_found_get(void)
{
  return bios_signature_found;
}

uint sram_time_get(void)
{
  return *sram_time;
}

int mie_start(void)
{
  volatile int i;
  volatile int j;
  volatile int k;
  volatile int l;
  int *p;
  uchar buf[256];
  mie_start_state.result = 0;
  mie_start_state.field_0x4 = 0;
  mie_start_state.field_0x8 = 0;
  maple_hw_init();
  maple_set_trigger(0);
  maple_port0_reset();
  i = 10000;
  do { i--; } while (i != -1);
  j = 40000;
  while (--j != -1) {
    i = 10000;
    do { i--; } while (i != -1);
    p = (int *)mie_send_no_data(3);
    if (*p != -1) break;
  }
  if (*p == -1) {
    mie_start_state.result = 2;
    return 2;
  }
  j = 40000;
  while (--j != -1) {
    i = 10000;
    do { i--; } while (i != -1);
    p = (int *)mie_send_no_data(1);
    if (*p != -1) break;
  }
  if (*p == -1) {
    mie_start_state.result = 2;
    return 2;
  }
  if (mie_read_ident(buf) != 0) {
    mie_start_state.result = 4;
    return 4;
  }
  if (mie_upload_firmware() != 0) {
    mie_start_state.result = 8;
    return 8;
  }
  j = 40000;
  while (--j != -1) {
    i = 10000;
    do { i--; } while (i != -1);
    p = (int *)mie_send_no_data(1);
    if (*p != -1) break;
  }
  return 0;
}

void card_units_start(void)
{
  int wait;
  typedef struct 
  {
    uint flags;
    int code;
    int x;
    char pad[0x40];
  } dev_ent;
  volatile int delay;
  volatile int retry;
  uchar buf[256];
  int ports[2];
  int addresses[2];
  int i;
  int *r;
  dev_ent *e;
  ports[0] = 0x40;
  ports[1] = 0x80;
  addresses[0] = 0x20;
  addresses[1] = 0x20;
  if (card_units_enabled == 0)
  {
    e = (dev_ent *) (&card_units);
    e[0].flags |= 2;
    ((uint *) e)[0x13] |= 2;
    return;
  }
  delay = 10000;
  do
  {
    delay--;
  }
  while (delay != (-1));
  for (i = 0; i < 2; i++)
  {
    ((dev_ent *) (&card_units))[i].flags = 0;
    wait = 10000;
    ((dev_ent *) (&card_units))[i].code = 0;
    ((dev_ent *) (&card_units))[i].x = 0;
    r = (int *) maple_port_reset(ports[i], addresses[i]);
    delay = wait;
    do
    {
      delay--;
    }
    while (delay != (-1));
    retry = 1000;
    while ((--retry) != (-1))
    {
      delay = 10000;
      do
      {
        delay--;
      }
      while (delay != (-1));
      r = (int *) maple_send_no_data(3, ports[i], addresses[i]);
      if ((*r) != (-1))
      {
        break;
      }
    }

    if ((*r) == (-1))
    {
      ((dev_ent *) (&card_units))[i].flags |= 2;
      ((dev_ent *) (&card_units))[i].code = 1;
      continue;
    }
    retry = 1000;
    while ((--retry) != (-1))
    {
      delay = 10000;
      do
      {
        delay--;
      }
      while (delay != (-1));
      r = (int *) maple_send_no_data(1, ports[i], addresses[i]);
      if ((*r) != (-1))
      {
        break;
      }
    }

    if ((*r) == (-1))
    {
      ((dev_ent *) (&card_units))[i].flags |= 2;
      ((dev_ent *) (&card_units))[i].code = 2;
      continue;
    }
    if (maple_unit_read_ident(buf, ports[i], addresses[i]) != 0)
    {
      ((dev_ent *) (&card_units))[i].flags |= 4;
      ((dev_ent *) (&card_units))[i].code = 3;
      continue;
    }
    if (maple_unit_upload_firmware(ports[i], addresses[i]) != 0)
    {
      ((dev_ent *) (&card_units))[i].flags |= 8;
      ((dev_ent *) (&card_units))[i].code = 4;
      continue;
    }
    retry = 1000;
    while ((--retry) != (-1))
    {
      delay = 10000;
      do
      {
        delay--;
      }
      while (delay != (-1));
      r = (int *) maple_send_no_data(1, ports[i], addresses[i]);
      if ((*r) != (-1))
      {
        break;
      }
    }

    if ((*r) == (-1))
    {
      ((dev_ent *) (&card_units))[i].flags |= 8;
      ((dev_ent *) (&card_units))[i].code = 5;
    }
  }

  card_state_reset();
}

void ledger_to_sram(void)
{
  uint w;
  uint *dst;
  uint i;
  uint *src;
  
  src = credit_ledger;
  dst = (uint *)(sram_ledger_bank + 8);
  i = 0;
  do {
    w = *src;
    src = src + 1;
    *dst = w;
    dst = dst + 1;
    i = i + 1;
  } while (i < 0x38);
  return;
}

void ledger_from_sram(void)
{
  uint w;
  uint i;
  uint *dst;
  uint *src;
  
  src = (uint *)(sram_ledger_bank + 8);
  dst = credit_ledger;
  i = 0;
  do {
    w = *src;
    src = src + 1;
    *dst = w;
    dst = dst + 1;
    i = i + 1;
  } while (i < 0x38);
  return;
}

void board_start_core(int country, int timing_mode)
{
  int i;
  int n;
  
  bios_helpers_copy();
  coin_input_held = 0;
  dip_switches = 0;
  board_start_marker = 0x04444444;
  bios_signature_check();
  if (*(volatile int *)0xffa00040 != 0x8201) {
    *(volatile int *)0xffa00040 = 0x8201;
  }
  *(volatile int *)0xffa00040 = 0x8201;
  if (timing_mode == 1) {
    cart_timing_slowest();
  }
  cart_control_clear();
  start_cleared_word = 0;
  for (i = 0; i < 4; i++) {
    start_cleared_words[i] = 0;
  }
  mie_start();
  card_units_start();
  legacy_io_start();
  dip_switches = legacy_io_inputs.bytes[5] & 0xf;
  if (bios_signature_found == 0) {
    cart_timing_from_ns(0);
    {
      int t = (char)dip_switches;
      country = (~t & 0xf) >> 1;
    }
  }
  boot_country = country;
  sram_save_ready = 0;
  eeprom_save_ready = 0;
  sram_check_banks();
  sram_bank_flag = sram_read_bank_flag();
  n = eeprom_check_copies();
  if (n == 3) {
    *eeprom_board_config = 0x10;
  }
  else {
    n = 0;
  }
  n += board_config_unpack(eeprom_board_config,(uint *)&board_settings,country);
  n += coin_config_unpack(eeprom_coin_config,(uint *)&coin_settings,country);
  ledger_from_sram();
  if (bios_ledger_check(coin_settings.max_credit,credit_ledger) != 0) {
    while (*(volatile int *)0xa05f7418 != 0) ;
    ledger_to_sram();
    sram_sync_from_bank0();
  }
  bios_credit_init(&coin_settings,&credit_state,credit_ledger,credit_status);
  bios_helper_34(sram_bank_flag);
  bios_credit_refresh(&coin_settings,&credit_state,credit_status);
  if (n != 0) {
    board_config_pack((char *)&board_settings,eeprom_board_config);
    coin_config_pack(&coin_settings,eeprom_coin_config);
    eeprom_flush();
  }
}

void board_start_core_dup(int country, int timing_mode)
{
  int i;
  int n;
  
  bios_helpers_copy();
  coin_input_held = 0;
  dip_switches = 0;
  board_start_marker = 0x04444444;
  bios_signature_check();
  if (*(volatile int *)0xffa00040 != 0x8201) {
    *(volatile int *)0xffa00040 = 0x8201;
  }
  *(volatile int *)0xffa00040 = 0x8201;
  if (timing_mode == 1) {
    cart_timing_slowest();
  }
  cart_control_clear();
  start_cleared_word = 0;
  for (i = 0; i < 4; i++) {
    start_cleared_words[i] = 0;
  }
  mie_start();
  card_units_start();
  legacy_io_start();
  dip_switches = legacy_io_inputs.bytes[5] & 0xf;
  if (bios_signature_found == 0) {
    char c;
    cart_timing_from_ns(0);
    c = *(volatile char *)&dip_switches;
    country = (int)(~c & 0xfU) >> 1;
  }
  boot_country = country;
  sram_save_ready = 0;
  eeprom_save_ready = 0;
  sram_check_banks();
  sram_bank_flag = sram_read_bank_flag();
  n = eeprom_check_copies();
  if (n == 3) {
    *eeprom_board_config = 0x10;
    n = 3;
  }
  else {
    n = 0;
  }
  n += board_config_unpack(eeprom_board_config,(uint *)&board_settings,country);
  n += coin_config_unpack(eeprom_coin_config,(uint *)&coin_settings,country);
  ledger_from_sram();
  if (bios_ledger_check(coin_settings.max_credit,credit_ledger) != 0) {
    while (*(volatile int *)0xa05f7418 != 0) ;
    ledger_to_sram();
    sram_sync_from_bank0();
  }
  bios_credit_init(&coin_settings,&credit_state,credit_ledger,credit_status);
  bios_helper_34(sram_bank_flag);
  bios_credit_refresh(&coin_settings,&credit_state,credit_status);
  if (n != 0) {
    board_config_pack((char *)&board_settings,eeprom_board_config);
    coin_config_pack(&coin_settings,eeprom_coin_config);
    eeprom_flush();
  }
}

void board_start(void)
{
  volatile int *p; int t; boot_rtc_seconds = bios_rtc_seconds; p = (volatile int *)0xa0200000; t = *p++; *p = t;
  bios_helpers_copy();
  bios_helper_28(&board_settings);
  bios_helper_2c(&coin_settings);
  board_start_core(board_settings.country,1);
}

void board_start_no_load(void)
{
  bios_helper_28(&board_settings);
  bios_helper_2c(&coin_settings);
  board_start_core(board_settings.country,1);
  return;
}

void board_prestart(void)
{
  bios_signature_check();
  bios_helpers_copy();
  bios_helper_28(&board_settings);
  bios_helper_2c(&coin_settings);
  return;
}

void led_latch_byte(uint bits)
{
  led_latch_inverted(bits & 0xff);
  return;
}

coin_settings_rec * coin_settings_get(void)
{
  return &coin_settings;
}

void credit_snapshot(int dst)
{
  uchar *s;
  uint *d;
  int i;

  s = credit_status;
  for (i = 0; i < 4; i++) {
    d = (uint *)(i * 0x10 + dst);
    d[0] = *s++;
    d[1] = *s++;
    d[2] = *s++;
    d[3] = *s++;
  }
  *(uint *)(dst + 0x40) = coin_settings.free_play;
  *(uint *)(dst + 0x44) = coin_settings.separate_credits;
  *(uint *)(dst + 0x48) = coin_settings.field_0x4;
  for (i = 0; i < 8; i++)
    *(uint *)(i * 4 + dst + 0x4c) = ((struct { char pad[0x14]; uint f[8]; } *)&coin_settings)->f[i];
}

void credit_snapshot_dup(int dst)
{
  uchar *s;
  uint *d;
  int i;

  s = credit_status;
  for (i = 0; i < 4; i++) {
    d = (uint *)(i * 0x10 + dst);
    d[0] = *s++;
    d[1] = *s++;
    d[2] = *s++;
    d[3] = *s++;
  }
  *(uint *)(dst + 0x40) = coin_settings.free_play;
  *(uint *)(dst + 0x44) = coin_settings.separate_credits;
  *(uint *)(dst + 0x48) = coin_settings.field_0x4;
  for (i = 0; i < 8; i++)
    *(uint *)(i * 4 + dst + 0x4c) = ((struct { char pad[0x14]; uint f[8]; } *)&coin_settings)->f[i];
}

uint credit_event_pop(void)
{
  int i;

  i = ((struct { char pad[0xb4]; int tail; int head; } *)&credit_state)->head;
  if (i == ((struct { char pad[0xb4]; int tail; int head; } *)&credit_state)->tail) return 0;
  ((struct { char pad[0xb4]; int tail; int head; } *)&credit_state)->head = (i + 1) & 0xf;
  return ((struct { char pad[0x74]; uint buf[16]; } *)&credit_state)->buf[i];
}

void credit_select_price(int player, uint selection)
{
  uint *p = (uint *)&credit_state;
  p[player] = selection;
  bios_credit_refresh(&coin_settings,&credit_state,credit_status);
}

int credit_try_spend(int player, int selection)
{
  int k;
  int *q;
  int n;
  int *p0;
  int *p1;
  int *p4;
  int *p2;
  int *p3;
  int *a;
  char *c;

  p0 = (int *)&credit_state;
  p0[player] = selection;
  bios_credit_refresh(&coin_settings,&credit_state,credit_status);
  c = (char *)credit_status + player * 4;
  if (c[3] != 2) return 0;
  if (coin_settings.separate_credits != 0) k = player;
  else k = 0;
  if (coin_settings.free_play != 1) {
    if (coin_settings.separate_credits == 0) {
      p1 = (int *)&credit_state;
      p1[13] = coin_settings.separate_credits;
      p1[12] = 0;
    }
    else {
      p2 = (int *)&credit_state + k;
      p2[12] = 0;
    }
    p3 = (int *)&credit_state + k;
    a = (int *)&coin_settings + selection;
    p3[4] -= a[5];
  }
  bios_credit_refresh(&coin_settings,&credit_state,credit_status);
  n = (player * 8 + selection) * 4;
  q = (int *)&credit_ledger[16];
  saturating_increment((int *)((char *)q + n));
  q -= 16;
  p4 = (int *)&credit_state + k;
  q[k] = p4[4];
  while (*(volatile int *)0xa05f7418 != 0) {
  }
  ledger_to_sram();
  sram_sync_from_bank0();
  return 1;
}

void sram_recheck(void)
{
  sram_check_banks();
  return;
}

void settings_to_eeprom(void)
{
  board_config_pack((char *)&board_settings,eeprom_board_config);
  coin_config_pack(&coin_settings,eeprom_coin_config);
  eeprom_flush();
  return;
}

void settings_from_boot_id(void)
{
  char *e;
  byte *p;
  int k;
  int i;

  e = (char *)boot_id_coin_table + board_settings.country * 16;
  switch (e[1]) {
  case 0:
    board_settings.field_0x4 = 1;
    board_settings.field_0x8 = 0;
    break;
  case 1:
    board_settings.field_0x4 = 1;
    board_settings.field_0x8 = 1;
    break;
  case 2:
    board_settings.field_0x4 = 0;
    board_settings.field_0x8 = 0;
    break;
  case 3:
    board_settings.field_0x4 = 0;
    board_settings.field_0x8 = 1;
    break;
  }
  k = e[3] - 1;
  p = (byte *)bios_coin_setting(k);
  coin_settings.separate_credits = e[2];
  coin_settings.coin_setting = k;
  coin_settings.coin_rate_0 = *p++;
  coin_settings.coin_rate_1 = *p++;
  coin_settings.coin_rate_2 = *p++;
  coin_settings.coin_rate_3 = *p;
  if (k == 27) {
    coin_settings.coin_rate_1 = e[4];
    coin_settings.coin_rate_2 = e[5];
    coin_settings.coin_rate_0 = e[6];
    coin_settings.coin_rate_3 = e[7];
  }
  for (i = 0; i < 8; i++) {
    { int *a = (int *)&coin_settings; a[i + 5] = e[i + 8]; }
  }
  board_config_pack((char *)&board_settings,eeprom_board_config);
  coin_config_pack(&coin_settings,eeprom_coin_config);
  eeprom_flush();
}

uint shared_page_f400_get(void)
{
  return 0x0c01f400;
}

void sram_bank_snapshot(uint *dst)
{
  int n;
  uint w;
  uint *src;
  
  do {
  } while (g1_dma_status_reg != 0);
  src = (uint *)sram_ledger_bank;
  n = 0x3d;
  do {
    w = *src;
    src = src + 1;
    *dst = w;
    dst = dst + 1;
    n = n + -1;
  } while (n != -1);
  return;
}

void credit_sram_reset(void)
{
  do {
  } while (g1_dma_status_reg != 0);
  sram_banks_clear();
  ledger_from_sram();
  bios_credit_init(&coin_settings,&credit_state,credit_ledger,credit_status);
  bios_helper_34(sram_bank_flag);
  return;
}

void sram_wipe(void)
{
  do {
  } while (g1_dma_status_reg != 0);
  sram_clear_all();
  ledger_from_sram();
  bios_credit_init(&coin_settings,&credit_state,credit_ledger,credit_status);
  bios_helper_34(sram_bank_flag);
  return;
}

uint sram_save_load_dma_safe(uint size, void *buffer)
{
  uint result;
  
  do {
  } while (g1_dma_status_reg != 0);
  result = sram_save_load(size,buffer);
  return result;
}

void eeprom_save_load_wrap(uint size, uchar *buffer)
{
  eeprom_save_load(size,buffer);
  return;
}

void sram_save_store_dma_safe(void)
{
  do {
  } while (g1_dma_status_reg != 0);
  sram_save_store();
  return;
}

void eeprom_save_store_flush(void)
{
  eeprom_save_store();
  eeprom_flush();
  return;
}

void sram_save_store_range_dma_safe(int offset, int n)
{
  do {
  } while (g1_dma_status_reg != 0);
  sram_save_store_range(offset,n);
  return;
}

int sram_save_size_limit(void)
{
  int size;
  
  size = sram_save_max_size();
  return size;
}

void eeprom_save_size_query(void)
{
  eeprom_save_max_size();
  return;
}

void clock_to_calendar(ushort *date)
{
  uint t;
  uint m;
  uint h;
  uint d;
  uint y;
  uint r;
  uint mo;
  uint ph;
  uint lim;
  int b = (int)rtc_days_before_month;
#define P ((byte *)date)

  t = (rtc_seconds_high << 16) | (rtc_seconds_low & 0xffff);
  P[6] = t % 60;
  m = t / 60;
  P[5] = m % 60;
  h = m / 60;
  P[4] = h % 24;
  d = h / 24;
  y = d / 365;
  r = d % 365;
  if ((y + 1) >> 2 <= r) {
    r -= (y + 1) >> 2;
    *date = y + 0x79e;
  } else {
    r -= (y + 1) >> 2;
    r += 365;
    y += 0x79d;
    *date = y;
    r += (y & 3) == 0;
  }
  mo = 1;
  lim = 31;
  if ((*date & 3) == 0) {
    if (r >= lim) {
      mo = 2;
      do {
        if (r < *(int *)((uchar *)&rtc_days_before_month[1] + mo * 4) + 1U) break;
        mo++;
      } while (mo < 12);
    }
  } else {
    mo = 1;
    do {
      if (r < *(uint *)((uchar *)&rtc_days_before_month[1] + mo * 4)) break;
      mo++;
    } while (mo < 12);
  }
  P[2] = mo;
  P[3] = (r -= *(char *)(b + mo * 4)) + 1;
  if ((*date & 3) == 0 && (byte)mo > 2) {
    P[3] = r;
  }
  mo = P[2];
  y = *date;
  if (mo <= 2) {
    y -= 1;
    mo += 12;
  }
  P[7] = (y + (y >> 2) + (mo * 13 + 8) / 5 + P[3] - 15) % 7;
  if (t >= 0x5cb417d1) {
    ph = (t - 0x5cb417d1) % 0x26ee7f;
  } else {
    ph = (0x5cb417d1 - t) % 0x26ee7f;
    ph = 0x26ee7f - ph;
  }
  P[8] = ph / 86400;
}

void coin_input_hold(void)
{
  coin_input_held = 1;
  return;
}

void coin_input_release(void)
{
  coin_input_held = 0;
  return;
}

unsigned int *coin_meter_table(void)
{
    return coin_meter_counts;
}

int coin_input_update(uint coin_switches, uint *coins)
{
  return bios_coin_frame(&coin_settings,&credit_state,credit_status,coin_switches,coins,credit_ledger);
}

void uptime_save(void)
{
    unsigned int now = RTC[0] << 16 | (RTC[1] & 0xffff);

    *sram_time = now - boot_rtc_seconds + sram_time[1];
}

void credit_backup_update(void)
{
    ledger_to_sram();
    sram_sync_from_bank0();
}

void empty_fn_0c119780(void)
{
  return;
}

long io_ready_check(void)
{
    return 0;
}

void frame_io_done(void)
{
}
