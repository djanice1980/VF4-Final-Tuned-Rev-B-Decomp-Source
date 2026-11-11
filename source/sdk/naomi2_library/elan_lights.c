/*
 * sdk/naomi2_library/elan_lights.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 */
#define KRPROTO_fsca_cos16
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern obj_light_dir_vec obj_light_dir; /* 0x0c325890 */
extern single_light_color_rec single_light_color; /* 0x0c3258f0 */
extern lights_elan_color_bits_rec lights_elan_color_bits; /* 0x0c3dc200 */
extern light_elan_mode_rec lights_elan_modes[16]; /* 0x0c3dc228 */
extern light_ambient_color_rec light_ambient_color; /* 0x0c41c64c */

static inline void sq_pref(void *a)
{
  __asm__ __volatile__ ("pref @%0" : : "r" (a));
}

void lights_elan_state_init(void)
{
  int i;
  
  light_table_fill_defaults();
  lights_elan_on_bits = 0xffff;
  lights_elan_second_bits = 0;
  lights_elan_last_flag = 0;
  lights_elan_alt_color_on = 0;
  lights_elan_alt_color = 0;
  lights_elan_color_bits.alt_bits = 0xff000000;
  lights_elan_color_bits.ambient_bits = 0xff000000;
  lights_elan_hdr_bits_16 = 0;
  lights_elan_hdr_bits_12 = 0;
  lights_elan_on_mask_high = 0xffff;
  lights_elan_on_mask_low = 0xffff;
  for (i = 0; i < 0x10; i++) {
    lights_elan_modes[i].mode_b = 0;
    lights_elan_modes[i].mode_a = 0;
    lights_elan_modes[i].mode_c = 1;
  }
}

void light_elan_mode_a_set(int light, uint value)
{
  lights_elan_modes[light].mode_a = value;
  return;
}

void light_elan_mode_b_set(int light, uint value)
{
  lights_elan_modes[light].mode_b = value;
  return;
}

void light_elan_mode_c_set(int light, uint value)
{
  lights_elan_modes[light].mode_c = value;
  return;
}

void light_elan_flags_set(int light, uint flags)
{
  lights_elan_modes[light].field_0xc = flags;
  if (flags & 1) {
    lights_elan_on_bits |= bit_masks[light];
  }
  else {
    lights_elan_on_bits &= ~bit_masks[light];
  }
  if (flags & 2) {
    lights_elan_second_bits |= bit_masks[light];
  }
  else {
    lights_elan_second_bits &= ~bit_masks[light];
  }
  return;
}

void lights_elan_last_flag_set(int value)
{
  if (value != 0) {
    lights_elan_last_flag = 1;
  }
  else {
    lights_elan_last_flag = 0;
  }
  return;
}

void lights_elan_alt_color_on_set(uchar on)
{
  lights_elan_alt_color_on = on;
  return;
}

void lights_elan_alt_color_set(float r, float g, float b)
{
  lights_elan_alt_color = (((int)(r * 255.0f) & 0xff) << 16) + (((int)(g * 255.0f) & 0xff) << 8) + ((int)(b * 255.0f) & 0xff);
  return;
}

uint spot_falloff_from_cosines(float cos_inner, float cos_outer)
{
  float pair[2];
  float inv;

  inv = 1.0f / (cos_inner - cos_outer);
  cos_outer = -cos_outer * inv;
  pair[0] = inv;
  pair[1] = cos_outer;
  return (((short *)pair)[1] << 16) | ((ushort *)pair)[3];
}

uint spot_falloff_from_angles(int inner_angle, int outer_angle)
{
  float pair[2];
  float cos_inner;
  float cos_outer;
  float inv;
  cos_inner = fsca_cos16(inner_angle);
  cos_outer = fsca_cos16(outer_angle);
  inv = 1.0f / (cos_inner - cos_outer);
  cos_outer = -fsca_cos16(outer_angle) * inv;
  pair[0] = inv;
  pair[1] = cos_outer;
  return (((short *)pair)[1] << 16) | ((ushort *)pair)[3];
}

void elan_list_ref_block_queue(int list, list_block_ref *dlist)
{
  list_block_ref block;
  uint v;
  uint hi;
  uint *p;
  uint *q;

  v = (uint)elan_cmd_write_ptr;
  *(volatile uint *)0xff000038 = v >> 0x18;
  *(volatile uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  block.addr = (uint)p;
  block.length = 0;
  p = p + 8;
  q = p;
  *--q = 0;
  *--q = dlist->length;
  *--q = 1;
  *--q = dlist->addr & 0x1fffff8 | 0x80000000;
  *--q = 0;
  *--q = 0;
  *--q = 0x18000000;
  *--q = 0x08000800;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  block.length = (int)p - block.addr;
  block.addr = block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | hi);
  elan_list_block_queue(list,&block);
  return;
}

void lights_elan_block_queue(int list, uint select)
{
  list_block_ref block;
  uint cmd[16];
  elan_light_params_rec args;
  uint pack[16];
  uint v;
  uint hi;
  uint *p;
  uint *q;
  uint *dst;
  uint *src;
  uint *src2;
  uint *dst2;
  uint ambient;
  uint second_color;
  uint t;
  int color;
  int i;
  int x;
  int y;
  int z;
  int m;
  float scale;
  int rec_size;
  light_table_entry *light;

  v = (uint)elan_cmd_write_ptr;
  *(uint *)0xff000038 = v >> 0x18;
  *(uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  block.addr = (uint)p;
  block.length = 0;
  ambient = lights_elan_color_bits.ambient_bits | ((int)(light_ambient_color.red * 255.0) & 0xffU) << 0x10 |
      ((int)(light_ambient_color.green * 255.0) & 0xffU) << 8 |
      (int)(light_ambient_color.blue * 255.0) & 0xffU;
  if (lights_elan_alt_color_on != '\0') {
    second_color = lights_elan_color_bits.alt_bits | lights_elan_alt_color;
  }
  else {
    second_color = ambient;
  }
  p = p + 8;
  q = p;
  *--q = 0;
  *--q = second_color;
  *--q = (lights_on_mask & lights_elan_second_bits) & 0xffff | (lights_on_mask & lights_elan_second_bits) << 16;
  *--q = 0;
  *--q = ambient;
  *--q = lights_on_mask & lights_elan_on_bits & lights_elan_on_mask_low & 0xffff | (lights_on_mask & lights_elan_on_bits & lights_elan_on_mask_high) << 16;
  *--q = obj_alt_lighting << 9 & 0x200 | lights_elan_hdr_bits_12 << 12 & 0xf000 | (lights_elan_hdr_bits_16 & 0xf) << 16 | 0x1f0;
  *--q = 0x08000400;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  if (lights_on_mask != 0) {
    i = 0;
    rec_size = sizeof(light_table_entry);
    scale = 65535.0;
    for (; i < 16; i++) {
      light = (light_table_entry *)((char *)light_table + i * rec_size);
      color = (((int)(light->color.red * 127.0) & 0xffU) << 16) +
            (((int)(light->color.green * 127.0) & 0xffU) << 8) +
            ((int)(light->color.blue * 127.0) & 0xffU);
      if (light->on != 0 && (select & bit_masks[i]) != 0) {
        if (light->type == 2) {
          elan_light_pack_directional(color, (int)(-light->view_dir[0] * 65535.0),
                       (int)(-light->view_dir[1] * 65535.0),
                       (int)(-light->view_dir[2] * 65535.0),
                       lights_elan_modes[i].mode_a, lights_elan_modes[i].mode_b,
                       lights_elan_modes[i].mode_c, cmd);
          cmd[7] = lights_elan_last_flag << 20 & 0x100000 | cmd[7];
          src = cmd;
          p = p + 8;
          dst = p;
          *--dst = *src++;
          *--dst = *src++;
          *--dst = *src++;
          *--dst = *src++;
          *--dst = *src++;
          *--dst = *src++;
          *--dst = i & 0xf | *src++;
          *--dst = *src;
          list_end_block_count = list_end_block_count + 1 & 3;
          sq_pref(dst);
        }
        else {
          m = 0xffff; /* WORKAROUND (kept as possibly original; no game instruction of its own): m is set and never read. Without it the game loses no instruction; only r2 and r3 swap. Kept as plausible original code: a limit set and not used in this build. */
          x = (int)(light_table[i].view_dir[0] * scale);
          y = (int)(light_table[i].view_dir[1] * scale);
          z = (int)(light_table[i].view_dir[2] * scale);
          if (x > 0xffff)
            x = 0xffff;
          if (x < -0xffff)
            x = -0xffff;
          if (y > 0xffff)
            y = 0xffff;
          if (y < -0xffff)
            y = -0xffff;
          if (z > 0xffff)
            z = 0xffff;
          if (z < -0xffff)
            z = -0xffff;
          args.color = color;
          args.dir_fixed[0] = x;
          args.dir_fixed[1] = y;
          args.dir_fixed[2] = z;
          args.pos[0] = light_table[i].view_pos[0];
          args.pos[1] = light_table[i].view_pos[1];
          args.pos[2] = light_table[i].view_pos[2];
          args.mode[0] = lights_elan_modes[i].mode_a;
          args.mode[1] = lights_elan_modes[i].mode_b;
          args.mode[2] = lights_elan_modes[i].mode_c;
          args.on = 1;
          args.field_0x3c = lights_elan_last_flag;
          if (light_table[i].type == 3) {
            args.far_dist = light_table[i].range;
            args.near_dist = 0.1;
            args.cone_angle[0] = 0.0;
            args.cone_angle[1] = 0.0;
            elan_light_pack_point(&args, pack);
          }
          else {
            args.far_dist = light_table[i].range;
            args.near_dist = 0.1;
            args.cone_angle[1] = (float)acos_ang16(light_table[i].cone_cos) * 3.1415927 * 3.0517578e-05;
            args.cone_angle[0] = 0.0;
            elan_light_pack_spot(&args, pack);
          }
          src2 = pack;
          p = p + 8;
          dst2 = p;
          *--dst2 = *src2++;
          *--dst2 = *src2++;
          *--dst2 = *src2++;
          *--dst2 = *src2++;
          *--dst2 = *src2++;
          *--dst2 = *src2++;
          *--dst2 = i & 0xf | *src2++;
          *--dst2 = *src2;
          list_end_block_count = list_end_block_count + 1 & 3;
          sq_pref(dst2);
        }
      }
    }
  }
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  block.length = (int)p - block.addr;
  block.addr = block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | hi);
  elan_list_block_queue(list,&block);
}

void lights_elan_send_all(void)
{
  if ((elan_list_enable_mask & 1) != 0) {
    lights_elan_block_queue(0,0xffff);
  }
  if ((elan_list_enable_mask & 4) != 0) {
    lights_elan_block_queue(2,0xffff);
  }
  if ((elan_list_enable_mask & 0x10) != 0) {
    lights_elan_block_queue(4,0xffff);
  }
  return;
}

void lights_elan_send_masked(uint select)
{
  if ((elan_list_enable_mask & 1) != 0) {
    lights_elan_block_queue(0,select);
  }
  if ((elan_list_enable_mask & 4) != 0) {
    lights_elan_block_queue(2,select);
  }
  if ((elan_list_enable_mask & 0x10) != 0) {
    lights_elan_block_queue(4,select);
  }
  return;
}

void light_single_elan_block_queue(int list)
{
  list_block_ref block;
  float dir[3];
  uint cmd[16];
  uint v;
  uint hi;
  uint *p;
  uint *q;
  uint *dst;
  uint ambient;
  uint second_color;
  uint second_bit;
  uint color;

  v = (uint)elan_cmd_write_ptr;
  *(uint *)0xff000038 = v >> 0x18;
  *(uint *)0xff00003c = v >> 0x18;
  p = (uint *)(v & 0x3ffffff | 0xe0000000);
  hi = v & 0xfc000000;
  block.addr = (uint)p;
  block.length = 0;
  ambient = lights_elan_color_bits.ambient_bits | ((int)(light_ambient_color.red * 255.0) & 0xffU) << 0x10 |
      ((int)(light_ambient_color.green * 255.0) & 0xffU) << 8 |
      (int)(light_ambient_color.blue * 255.0) & 0xffU;
  if (lights_elan_alt_color_on != '\0') {
    second_color = lights_elan_color_bits.alt_bits | lights_elan_alt_color;
  }
  else {
    second_color = ambient;
  }
  p = p + 8;
  q = p;
  *--q = 0;
  *--q = second_color;
  second_bit = lights_elan_second_bits & 1;
  *--q = second_bit | second_bit << 16;
  *--q = 0;
  *--q = ambient;
  *--q = lights_elan_on_bits & lights_elan_on_mask_low & 1 | (lights_elan_on_bits & lights_elan_on_mask_high & 1) << 0x10;
  *--q = obj_alt_lighting << 9 & 0x200 | 0xf01f0;
  *--q = 0x08000400;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  color = (((int)(single_light_color.red * 127.0) & 0xffU) << 16) +
        (((int)(single_light_color.green * 127.0) & 0xffU) << 8) +
        ((int)(single_light_color.blue * 127.0) & 0xffU);
  vec3_normalize_to((float *)&obj_light_dir.x,dir);
  elan_light_pack_directional(color,(int)(-dir[0] * 65535.0),(int)(-dir[1] * 65535.0),(int)(-dir[2] * 65535.0),
               lights_elan_modes[0].mode_a,lights_elan_modes[0].mode_b,
               lights_elan_modes[0].mode_c,cmd);
  cmd[7] |= 0x100000;
  p = p + 8;
  dst = p;
  *--dst = cmd[0];
  *--dst = cmd[1];
  *--dst = cmd[2];
  *--dst = cmd[3];
  *--dst = cmd[4];
  *--dst = cmd[5];
  *--dst = cmd[6];
  *--dst = cmd[7];
  do { /* this block is needed: its end flushes the pending pop of the call's stack arguments (expand_end_null_loop); the same update earlier in the function is a plain statement */
    list_end_block_count = list_end_block_count + 1 & 3;
  } while (0);
  __asm__ __volatile__ ("pref @%0" : : "r" (dst));
  p[0] = 0x08000000;
  p[2] = 0x08000000;
  p[4] = 0x08000000;
  p[6] = 0x08000000;
  list_end_block_count = list_end_block_count + 1 & 3;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
  p = p + 8;
  block.length = (int)p - block.addr;
  block.addr = block.addr & 0x3ffffff | hi;
  elan_cmd_write_ptr = (uint *)((uint)p & 0x3ffffff | hi);
  elan_list_block_queue(list,&block);
}

void light_single_elan_send(void)
{
  if ((elan_list_enable_mask & 1) != 0) {
    light_single_elan_block_queue(0);
  }
  if ((elan_list_enable_mask & 4) != 0) {
    light_single_elan_block_queue(2);
  }
  if ((elan_list_enable_mask & 0x10) != 0) {
    light_single_elan_block_queue(4);
  }
  return;
}
