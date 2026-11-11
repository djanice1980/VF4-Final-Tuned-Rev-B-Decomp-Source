/*
 * vf4lib/libsprite/src/sprite.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern uint tex_vram_free; /* 0x0c292740 */
extern char ftttl_scenes_loaded; /* 0x0c307018 */
extern char sel_sprite_sets_loaded; /* 0x0c307019 */
extern float sprite_text_x_last; /* 0x0c30701c */
extern int sprite_make_errors; /* 0x0c307028 */
extern int sprite_drop_errors; /* 0x0c30702c */
extern float sprite_base_z; /* 0x0c307030 */
extern sprite_set sprite_sets[81]; /* 0x0c322154 */
extern static_heap_rec spr_sel2_top; /* 0x0c360130 */
extern static_heap_rec spr_sel_top; /* 0x0c36017c */
extern static_heap_rec spr_sel_base_top; /* 0x0c3601a4 */
extern static_heap_rec spr_sel3_top; /* 0x0c3601b0 */
extern int char_textures_loaded; /* 0x0c3ad290 */
extern int sprite_set_vram_used[81]; /* 0x0c3b6298 */
extern char sprite_set_own_buffer[81]; /* 0x0c3b6464 */
extern char sprite_clip_count; /* 0x0c4dd828 */
extern sprite_queue_state_rec sprite_queue_state; /* 0x0c4dd82c */
extern char sprite_queue[768][80]; /* 0x0c4ddb30 */
extern char sprite_set_make_failed[81]; /* 0x0c4ecb30 */
extern sprite_clip_rect sprite_clip_rects[20]; /* 0x0c4ecb81 */
extern static_heap_rec nick_comm_buf; /* 0x0c360018 */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */
extern static_heap_rec nick_game_buf; /* 0x0c3600dc */
extern static_heap_rec spr_cmn_top; /* 0x0c3601e0 */
extern char sprite_set_loaded[81]; /* 0x0c3b63dc */
extern char_side_tex_mask_rec char_side_tex_mask; /* 0x0c3b6430 */
extern sprite_put_rec sprite_put_state; /* 0x0c3b6438 */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_height; /* 0x0c32a700 */
extern short *sprite_index_to_record; /* 0x0c3e0350 */
extern int sprite_index_count; /* 0x0c3e0354 */
extern uint sprite_records; /* 0x0c3e0358 */
extern unit_rgb obj_light_color; /* 0x0c3258c0 */
extern sprite_coord_recip_pair sprite_coord_recip; /* 0x0c3e033c */
extern poly_header_or_pair poly_header_or_words; /* 0x0c41bb44 */

void sprite_set_fail_flags_clear(void)
{
  int i;
  char *failed;
  
  i = 0;
  failed = sprite_set_make_failed;
  do {
    failed[i] = 0;
    i++;
  } while (i < 0x51);
}

void sprite_sets_init(void)
{
  int i;
  char *base;
  char *page;

  for (i = 0; i < 0x51; i++) {
    {
      char *own_buffer = sprite_set_own_buffer;
      *(char *)((int)own_buffer + i) = 1;
    }
    switch (i) {
    case 7:
      base = spr_cmn_top.addr;
      break;
    case 54:
      base = spr_sel_base_top.addr;
      break;
    case 42:
      base = nick_game_buf.addr;
      break;
    case 41:
      base = nick_comm_buf.addr;
      break;
    case 56:
      base = spr_sel_top.addr;
      break;
    case 57:
      base = spr_sel2_top.addr;
      break;
    case 58:
      base = spr_sel3_top.addr;
      break;
    default:
      base = heap_texdata.addr;
      {
        char *own_buffer = sprite_set_own_buffer;
        *(char *)((int)own_buffer + i) = 0;
      }
      break;
    }
    page = (char *)sprite_sets[i].sprites;
    while (*(int *)(page + 4) != -1) {
      *(int *)(page + 8) += (int)base;
      page += 16;
    }
    {
      char *loaded = sprite_set_loaded;
      *(char *)((int)loaded + i) = 0;
    }
  }
  sel_ft_sprite_files_load();
  sprite_boot_noop();
  sprite_set_fail_flags_clear();
  ftttl_scenes_loaded = 0;
  sel_sprite_sets_loaded = 0;
}

void sprite_set_load(int set_no)
{
  sprite_set *set = &sprite_sets[set_no];
  char *loaded;
  int vram_free;
  int page;
  int skip;

  loaded = sprite_set_loaded;
  if (loaded[set_no] == '\0') {
    perf_color_mark(0xffff);
    load_file(set->name,heap_texdata.addr,heap_texdata.size);
    vram_free = tex_vram_free_total();
    texidx_base_put(set->tex_base);
    if (set_no == 6) {
      page = 0;
      skip = 0;
      for (; *(int *)(set->sprites + page * 0x10 + 4) != -1; page++) {
        if (page != sprite_set6_skip_tex[skip] - set->tex_base) {
          sprite_make_errors |= sprite_list_record_make(set->sprites,0,page,0,set_no);
        }
        else {
          skip++;
        }
      }
    }
    else {
      sprite_make_errors |= sprite_list_records_make(set->sprites,0,0,set_no);
    }
    tex_vram_free = tex_vram_free_total();
    vram_free -= tex_vram_free;
    sprite_set_vram_used[set_no] = vram_free;
    {
      char *loaded = sprite_set_loaded;
      loaded[set_no] = 1;
    }
    perf_color_mark(0);
  }
}

void ltop_frame_read(int frame)
{
  dimm_file *fp;
  char **name = &sprite_sets[0x33].name;
  perf_color_mark(0xffff);
  fp = dimm_fopen(*name,"rb");
  if (fp == (dimm_file *)0x0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/sprite.c",0x16f,"L-TOP %s open error.",*name);
  }
  if (dimm_fseek(fp,frame * 0x4800,0) != -1) {
    dimm_fread_dma(ltop_frame_buf,0x4800,1,fp);
  }
  dimm_fclose(fp);
  dcache_tags_clear_uncached();
  perf_color_mark(0);
}

void ltop_set_load(int set_no)
{
  int vram_free;
  sprite_set *set = &sprite_sets[set_no];
  int loaded = (int)sprite_set_loaded;
  int offset;
  
  if (*(char *)(loaded + set_no) != '\x01') {
    perf_color_mark(0xffff);
    vram_free = tex_vram_free_total();
    ltop_frame_read(0);
    texidx_base_put(set->tex_base);
    *(void **)(set->sprites + 8) = ltop_frame_buf;
    *(void **)(set->sprites + 0x18) = ltop_frame_buf;
    { int pages = set->sprites; offset = 0x3c8; *(void **)(pages + offset) = ltop_frame_buf; }
    { int pages = set->sprites; offset = 0x3d8; *(void **)(pages + offset) = ltop_frame_buf; }
    sprite_make_errors |= sprite_list_record_make(set->sprites,0,0,0,set_no);
    sprite_make_errors |= sprite_list_record_make(set->sprites,0,1,0,set_no);
    sprite_make_errors |= sprite_list_record_make(set->sprites,0,0x3c,0,set_no);
    sprite_make_errors |= sprite_list_record_make(set->sprites,0,0x3d,0,set_no);
    tex_vram_free = tex_vram_free_total();
    vram_free = vram_free - tex_vram_free;
    sprite_set_vram_used[set_no] = vram_free;
    *(char *)(loaded + set_no) = 1;
    perf_color_mark(0);
  }
  return;
}

void sprite_set_file_load(int set_no)
{
  sprite_set *set = &sprite_sets[set_no];
  perf_color_mark(0xffff);
  load_file(set->name,heap_texdata.addr,
            heap_texdata.size);
  perf_color_mark(0);
  return;
}

void sprite_set_records_make(int set_no)
{
  int vram_free;
  sprite_set *set;
  int loaded;

  set = &sprite_sets[set_no];
  loaded = (int)sprite_set_loaded;
  if (*(char *)(loaded + set_no) != '\x01') {
    vram_free = tex_vram_free_total();
    perf_color_mark(0xffff);
    texidx_base_put(set->tex_base);
    sprite_make_errors |= sprite_list_records_make(set->sprites,0,0,set_no);
    tex_vram_free = tex_vram_free_total();
    vram_free -= tex_vram_free;
    sprite_set_vram_used[set_no] = vram_free;
    *(char *)(loaded + set_no) = 1;
  }
}

void sprite_set_free(int set_no)
{
  sprite_set *set = &sprite_sets[set_no];
  char *loaded = sprite_set_loaded;

  if (loaded[set_no] != '\0') {
    texidx_base_put(set->tex_base);
    sprite_drop_errors |= sprite_records_drop_list(set->sprites,0);
    loaded[set_no] = 0;
    sprite_set_vram_used[set_no] = 0;
    tex_vram_free = tex_vram_free_total();
  }
}

void sprite_set_sprite_load(int set_no, int tex_no)
{
  sprite_set *sets;
  sprite_set *set;
  int vram_free;
  char *loaded;
  sets = sprite_sets;
  set = &sets[set_no];
  if (tex_no != -1) {
    vram_free = tex_vram_free_total();
    texidx_base_put(set->tex_base);
    sprite_make_errors |= sprite_list_record_make(set->sprites,0,tex_no - set->tex_base,0,set_no);
    tex_vram_free = tex_vram_free_total();
    vram_free -= tex_vram_free;
    sprite_set_vram_used[set_no] += vram_free;
    loaded = sprite_set_loaded;
    loaded[set_no] = 1;
  }
  return;
}

void sprite_set_sprite_free(int set_no, int tex_no)
{
  sprite_set *set = &sprite_sets[set_no];
  int vram_free;
  if (tex_no != -1) {
    vram_free = tex_vram_free_total();
    texidx_base_put(set->tex_base);
    sprite_drop_errors = sprite_drop_errors | sprite_record_drop_by_pos(set->sprites, 0, tex_no - set->tex_base);
    tex_vram_free = tex_vram_free_total();
    vram_free = vram_free - tex_vram_free;
    sprite_set_vram_used[set_no] += vram_free;
  }
}

void common_sprite_sets_load(void)
{
  sprite_set_load(0);
  sprite_set_load(1);
  sprite_set_load(0x30);
  sprite_set_load(0x3c);
  return;
}

void common_sprite_sets_free(void)
{
  sprite_set_free(0);
  sprite_set_free(1);
  sprite_set_free(0x30);
  sprite_set_free(0x3c);
  return;
}

void sel_sprite_sets_load(void)
{
  if (sel_sprite_sets_loaded == '\0') {
    ltop_set_load(0x33);
    sprite_set_load(0x3b);
    sprite_set_load(0x3c);
    aet_set_load(10);
    aet_scene_textures_load(0xa0000);
    aet_scene_textures_load(0xa0001);
    aet_set_load(5);
    aet_scene_textures_load(0x50000);
    aet_set_load(6);
    aet_scene_textures_load(0x60000);
    aet_scene_textures_load(0x60001);
    aet_scene_textures_load(0x60002);
    aet_scene_textures_load(0x60003);
    aet_scene_textures_load(0x60004);
    aet_scene_textures_load(0x60005);
    aet_scene_textures_load(0x60006);
    aet_scene_textures_load(0x60007);
    aet_scene_textures_load(0x60008);
    aet_scene_textures_load(0x60009);
    aet_scene_textures_load(0x6000a);
    aet_scene_textures_load(0x6000b);
    aet_scene_textures_load(0x6000c);
    aet_scene_textures_load(0x6000d);
    aet_scene_textures_load(0x6000e);
    aet_set_load(0xb);
    aet_scene_textures_load(0xb0000);
    aet_scene_textures_load(0xb0001);
    aet_scene_textures_load(0xb0002);
    aet_scene_textures_load(0xb0003);
    aet_scene_textures_load(0xb0004);
    aet_scene_textures_load(0xb0005);
    aet_scene_textures_load(0xb0006);
    aet_scene_textures_load(0xb0007);
    aet_scene_textures_load(0xb0008);
    aet_scene_textures_load(0xb0009);
    aet_scene_textures_load(0xb000a);
    aet_scene_textures_load(0xb000b);
    aet_scene_textures_load(0xb000c);
    aet_scene_textures_load(0xb000d);
    aet_scene_textures_load(0xb000e);
    aet_set_load(7);
    aet_set_load(8);
    aet_set_load(9);
    sel_sprite_sets_loaded = '\x01';
  }
  return;
}

void sel_ft_sprite_files_load(void)
{
  uint i;
  
  perf_color_mark(0xffff);
  for (i = 0; i < 4; i++) {
    sprite_set_heap_rec *entry = &sel_ft_sprite_sets[i];
    sprite_set *set = &sprite_sets[entry->set_no];
    load_file(set->name, entry->heap->addr, entry->heap->size);
    sprite_set_loaded[entry->set_no] = 1;
  }
  perf_color_mark(0);
}

void sel_sprite_sets_free(void)
{
  if (sel_sprite_sets_loaded != '\0') {
    sprite_set_free(0x3b);
    sprite_set_free(0x33);
    sel_chara_scenes_unload();
    aet_scene_textures_free(0xa0001);
    aet_scene_textures_free(0xa0000);
    aet_set_free(10);
    sel_sprite_sets_loaded = '\0';
  }
  return;
}

void rank_sprite_sets_load(void)
{
  sprite_set_load(4);
  sprite_set_load(0x2e);
  return;
}

void rank_sprite_sets_free(void)
{
  sprite_set_free(4);
  sprite_set_free(0x2e);
  return;
}

void sprite_sets_noop_a(void)
{
  return;
}

void sprite_sets_noop_b(void)
{
  return;
}

void sprite_sets_noop_c(void)
{
  return;
}

void sprite_sets_noop_d(void)
{
  return;
}

void logo_sprite_set_load(void)
{
  sprite_set_load(2);
  return;
}

void logo_sprite_set_free(void)
{
  sprite_set_free(2);
  return;
}

void name_entry_sprite_sets_load(void)
{
  sprite_set_load(0x1c);
  sprite_set_load(0x2e);
  return;
}

void name_entry_sprite_sets_free(void)
{
  sprite_set_free(0x1c);
  sprite_set_free(0x2e);
  return;
}

void game_sprite_sets_load(void)
{
  sprite_set_load(6);
  game_mes_file_load();
  return;
}

void game_mes_file_load(void)
{
  char **name = &sprite_sets[7].name;
  perf_color_mark(0xffff);
  load_file(*name,spr_cmn_top.addr,
            spr_cmn_top.size);
  perf_color_mark(0);
  return;
}

void game_sprite_sets_free(void)
{
  sprite_set_free(6);
  sprite_set_free(7);
  sprite_set_free(0x1e);
  sprite_set_free(0x1f);
  sprite_set_free(0x20);
  sprite_set_free(0x21);
  sprite_set_free(0x22);
  sprite_set_free(0x50);
  sprite_set_free(0x24);
  sprite_set_free(0x28);
  game_mes_loaded = 0;
  return;
}

void ftttl_scenes_load(void)
{
  if (ftttl_scenes_loaded == '\0') {
    aet_set_load(0x1b);
    aet_scene_textures_load(0x1b0000);
    aet_scene_textures_load(0x1b0002);
    aet_scene_textures_load(0x1b0001);
    aet_scene_textures_load(0x1b0003);
    aet_scene_textures_load(0x1b0004);
    ltop_set_load(0x33);
    ftttl_scenes_loaded = '\x01';
  }
  return;
}

void ftttl_scenes_free(void)
{
  if (ftttl_scenes_loaded != '\0') {
    aet_objs_stop_by_set(0x1b);
    aet_scene_textures_free(0x1b0000);
    aet_scene_textures_free(0x1b0002);
    aet_scene_textures_free(0x1b0001);
    aet_scene_textures_free(0x1b0003);
    aet_scene_textures_free(0x1b0004);
    aet_set_free(0x1b);
    sprite_set_free(0x33);
    ftttl_scenes_loaded = '\0';
  }
  return;
}

void staff_sprite_set_load(void)
{
  sprite_set_load(0x1b);
  return;
}

void staff_sprite_set_free(void)
{
  sprite_set_free(0x1b);
  return;
}

void sprite_put_xy(uint sprite_no, float x, float y)
{
  sprite_put(sprite_no,x,y,0.04,1.0,1.0,0.0,0.0,1.0,0,5,0xffffff,1.0,1.0,
               0xff000000);
  return;
}

void sprite_put_xyz(uint sprite_no, float x, float y, float z)
{
  sprite_put(sprite_no,x,y,z,1.0,1.0,0.0,0.0,1.0,0,5,0xffffff,1.0,1.0,0xff000000);
  return;
}

void sprite_put_xy_scaled(uint sprite_no, float x, float y, float scale_x, float scale_y)
{
  sprite_put(sprite_no,x,y,0.04,scale_x,scale_y,0.0,0.0,1.0,0,5,0xffffff,1.0,1.0,
               0xff000000);
  return;
}

void sprite_put_flags_xy(uint sprite_no, uint flags, float x, float y)
{
  sprite_put(sprite_no,x,y,0.04,1.0,1.0,0.0,0.0,1.0,0,flags,0xffffff,1.0,1.0,0xff000000);
}

void sprite_put_xy_alpha(uint sprite_no, float x, float y, float alpha)
{
  sprite_put
    (sprite_no,x,y,0.04,1.0,1.0,0.0,0.0,1.0,0,5,0xffffff,1.0,alpha,0xff000000);
}

void sprite_put_colors_xy(uint sprite_no, uint col, uint offset_col, float x, float y)
{
  sprite_put(sprite_no,x,y,0.04,1.0,1.0,0.0,0.0,1.0,0,5,col,1.0,1.0,offset_col);
}

void sprite_put_xy_uv(uint sprite_no, float x, float y, float u0, float v0, float u1, float v1)
{
  sprite_put(sprite_no,x,y,0.04,1.0,1.0,u0,v0,u1,0,5,0xffffff,v1,1.0,0xff000000);
}

void sprite_put_xyz_scaled(uint sprite_no, float x, float y, float z, float scale_x, float scale_y )
{
  sprite_put(sprite_no,x,y,z,scale_x,scale_y,0.0,0.0,1.0,0,5,0xffffff,1.0,1.0
               ,0xff000000);
  return;
}

void sprite_put_flags_xyz(uint sprite_no, uint flags, float x, float y, float z)
{
  sprite_put(sprite_no,x,y,z,1.0,1.0,0.0,0.0,1.0,0,flags,0xffffff,1.0,1.0,0xff000000);
}

void sprite_put_xyz_alpha(uint sprite_no, float x, float y, float z, float alpha)
{
  sprite_put(sprite_no,x,y,z,1.0,1.0,0.0,0.0,1.0,0,5,0xffffff,1.0,alpha,0xff000000);
}

void sprite_put_xy_colors_z_alpha(uint sprite_no, float x, float y, uint col, uint offset_col, float z, float alpha)
{
  sprite_put(sprite_no,x,y,z,1.0,1.0,0.0,0.0,1.0,0,5,col,1.0,alpha,offset_col);
}

void sprite_put_xy_scaled_flags(uint sprite_no, float x, float y, float scale_x, float scale_y, uint flags)
{
  sprite_put(sprite_no,x,y,0.04,scale_x,scale_y,0.0,0.0,1.0,0,flags,0xffffff,1.0,
               1.0,0xff000000);
}

void sprite_put_xyz_scaled_alpha(uint sprite_no, float x, float y, float z, float scale_x, float scale_y, float alpha)
{
  sprite_put(sprite_no,x,y,z,scale_x,scale_y,0.0,0.0,1.0,0,5,0xffffff,1.0,
               alpha,0xff000000);
  return;
}

void sprite_put_xyz_alpha_flags(uint sprite_no, float x, float y, float z, float alpha, uint flags)
{
  sprite_put(sprite_no,x,y,z,1.0,1.0,0.0,0.0,1.0,0,flags,0xffffff,1.0,alpha,0xff000000);
  return;
}

void sprite_put_xyz_scaled_flags(uint sprite_no, float x, float y, float z, float scale_x, float scale_y, uint flags)
{
  sprite_put(sprite_no,x,y,z,scale_x,scale_y,0.0,0.0,1.0,0,flags,0xffffff,
               1.0,1.0,0xff000000);
  return;
}

void sprite_put_xyz_scaled_alpha_flags(uint sprite_no, float x, float y, float z, float scale_x, float scale_y, float alpha, uint flags)
{
  sprite_put(sprite_no,x,y,z,scale_x,scale_y,0.0,0.0,1.0,0,flags,0xffffff,
               1.0,alpha,0xff000000);
  return;
}

void sprite_put_xyz_scaled_alpha_angle_flags(uint sprite_no, float x, float y, float z, float scale_x, float scale_y, float alpha, short angle, uint flags)
{
  sprite_put(sprite_no,x,y,z,scale_x,scale_y,0.0,0.0,1.0,angle,
               flags,0xffffff,1.0,alpha,0xff000000);
  return;
}

void sprite_put_xy_alpha_flags(uint sprite_no, float x, float y, float alpha, uint flags)
{
  sprite_put(sprite_no,x,y,0.04,1.0,1.0,0.0,0.0,1.0,0,flags,0xffffff,1.0,alpha,0xff000000);
}

void sprite_put_xy_scaled_alpha(uint sprite_no, float x, float y, float scale_x, float scale_y, float alpha )
{
  sprite_put(sprite_no,x,y,0.04,scale_x,scale_y,0.0,0.0,1.0,0,5,0xffffff,1.0,
               alpha,0xff000000);
  return;
}

void sprite_put_xy_scaled_flags_alpha(uint sprite_no, float x, float y, float scale_x, float scale_y, uint flags, float alpha)
{
  sprite_put(sprite_no,x,y,0.04,scale_x,scale_y,0.0,0.0,1.0,0,flags,0xffffff,1.0,
               alpha,0xff000000);
  return;
}

void sprite_put_angle_flags_xy_scaled_alpha(uint sprite_no, int angle, uint flags, float x, float y, float scale_x, float scale_y, float alpha)
{
  short angle_short;
  angle_short = (short) angle;
  sprite_put(sprite_no, x, y, 0.04, scale_x, scale_y, 0.0, 0.0, 1.0, (int) angle_short, flags, 0xffffff, 1.0, alpha, 0xff000000);
  return;
}

void sprite_put_angle_flags_xy(uint sprite_no, short angle, uint flags, float x, float y)
{
  sprite_put(sprite_no,x,y,0.04,1.0,1.0,0.0,0.0,1.0,angle,flags,0xffffff,1.0,1.0,0xff000000);
}

void sprite_offset_set(float x, float y, float z)
{
  sprite_put_state.offset.x = x;
  sprite_put_state.offset.y = y;
  sprite_put_state.offset.z = z + sprite_base_z;
  return;
}

void sprite_offset_reset(void)
{
  sprite_put_state.offset.x = 0.0;
  sprite_put_state.offset.y = 0.0;
  sprite_put_state.offset.z = sprite_base_z;
  return;
}

void sprite_alpha_set(float alpha)
{
  sprite_put_state.alpha = alpha;
  return;
}

void sprite_alpha_reset(void)
{
  sprite_put_state.alpha = 1.0;
  return;
}

void sprite_colors_set(uint col, uint offset_col)
{
  sprite_put_state.col_override = col;
  sprite_put_state.offset_col_override = offset_col;
  return;
}

void sprite_colors_reset(void)
{
  sprite_put_state.col_override = 0xffffff;
  sprite_put_state.offset_col_override = 0xff000000;
  return;
}

void sprite_scale_set(float scale_x, float scale_y)
{
  sprite_put_state.scale_x = scale_x;
  sprite_put_state.scale_y = scale_y;
  return;
}

void sprite_scale_reset(void)
{
  sprite_put_state.scale_x = 1.0;
  sprite_put_state.scale_y = 1.0;
  return;
}

void sprite_flags_override_set(uint flags)
{
  sprite_put_state.flags_override = flags;
  return;
}

void sprite_flags_override_reset(void)
{
  sprite_put_state.flags_override = 0;
  return;
}

void sprite_angle_base_set(short angle_base)
{
  sprite_put_state.angle_base = angle_base;
  return;
}

void sprite_angle_base_reset(void)
{
  sprite_put_state.angle_base = 0;
  return;
}

void sprite_boot_noop(void)
{
  return;
}

void text_lines_measure(char *text, int *width_out, int *lines_out)
{
  int cur;
  int max;
  int lines;
  cur = 0;
  max = 0;
  lines = 0;
  for (; *text != '\0'; text++) {
    if (*text == '\n') {
      if (cur > max) max = cur;
      cur = 0;
      lines++;
    }
    else {
      cur++;
    }
  }
  lines++;
  if (cur > max) max = cur;
  *width_out = max;
  *lines_out = lines;
}

void sprite_text_printf(sprite_euc_font *font, float x, float y, float z, uint flags, char *format, ...)
{
  va_list ap;
  byte buf[256];
  __builtin_stdarg_start(ap, format);
  vsnprintf((char *)buf, 0x100, format, ap);
  __builtin_va_end(ap);
  sprite_text_put(font, x, y, z, flags, buf);
}

void sprite_text_put(sprite_euc_font *font, float x, float y, float z, uint flags, byte *text)
{
  byte *p = text;

  byte uc;
  short adv;
  short ix;
  short d;
  short rows;
  short col;
  short row;
  short off;
  short off2;
  ushort code;
  short base;
  float u0;
  float v0;
  float u1;
  float v1;
  float x0;
  short *id;

  strlen((char *)text);
  sprite_text_x_last = 0.0f;
  if (flags & 2) {
    for (; *p != 0; p++) {
      if ((flags & 0x80) && *p <= 0x9f)
        x -= font->cell_width * sprite_put_state.scale_x;
      else if (*p != 0x8e)
        x -= font->cell_width * 0.5f * sprite_put_state.scale_x;
    }
  }
  else if (flags & 4) {
    for (; *p != 0; p++) {
      if (*p != 0x8e) x -= font->cell_width * 0.5f * sprite_put_state.scale_x;
    }
  }
  x0 = x;
  while (*(char *)text != 0) {
    if ((byte)*(char *)text < 0xa0) {
      adv = font->cell_width * 0.5f;
      if (*(char *)text == '\n') {
        if (!(flags & 0x20)) {
          y += font->line_height;
          x = x0;
        }
        text++;
        continue;
      }
      if ((char)*text == (char)0x8e && text[1] >= 0xa0 && text[1] <= 0xdf) {
        ix = 30;
        uc = text[1];
        col = uc & 15;
        row = (byte)(text[1] + 0x60) >> 4;
        u0 = col * 0.0625f;
        u1 = u0 + 0.0625f;
        v0 = row * 0.125f;
        v1 = 0.125f + v0;
        text += 2;
      }
      else if ((byte)(*text - 0x20) > 0x5e) {
        x += adv;
        text++;
        continue;
      }
      else {
      if ((flags & 0x80) && (char)*text >= '0' && (char)*text <= '9') {
        code = (char)*text + 0xa380;
        base = 0xa1a1;
        adv = font->cell_width;
        ix = 1;
        off = 0;
        off2 = 0;
        d = code - base;
        rows = (code - (code & 0xff)) - (base - (base & 0xff));
        rows = rows / 256;
        if (rows > 0) d -= rows * 161;
        d += off;
        col = d % 16;
        row = d / 16;
        row += off2;
        for (; row > 15; row -= 16) ix++;
        u0 = col * 0.0625f;
        u1 = u0 + 0.0625f;
        v0 = row * 0.0625f;
        v1 = 0.0625f + v0;
      }
      else {
        ix = 0;
        uc = *text;
        col = uc & 15;
        row = (uc >> 4) - 2;
        u0 = col * 0.0625f;
        u1 = u0 + 0.0625f;
        v0 = row * 0.125f;
        v1 = 0.125f + v0;
      }
      text++;
      }
    }
    else {
      code = (ushort)((char)text[0] << 8) | text[1];
      adv = font->cell_width;
      base = 0xa1a1;
      if (code >= base) {
        if (code <= 0xa8c0) {
          ix = 1;
          off = 0;
          goto calc;
        }
      }
      base = 0xb0a1;
      if (code >= base) {
        if (code <= 0xf4a4) {
          ix = 4;
          off = 1;
          goto calc;
        }
      }
      x += adv;
      text += 2;
      continue;
    calc:
      off2 = 0;
      d = code - base;
      rows = (code - (code & 0xff)) - (base - (base & 0xff));
      rows = rows / 256;
      if (rows > 0) d -= rows * 161;
      d += off;
      col = d % 16;
      row = d / 16;
      row += off2;
      for (; row > 15; row -= 16) ix++;
      u0 = col * 0.0625f;
      u1 = u0 + 0.0625f;
      v0 = row * 0.0625f;
      v1 = 0.0625f + v0;
      text += 2;
    }
    if (ix < font->page_count) {
      sprite_put(font->page_sprites[ix],x,y,z,1.0f,1.0f,u0,v0,u1,0,5,0xffffff,v1,1.0f,0xff000000);
      if (flags & 0x40) {
        sprite_colors_set(0,0);
        sprite_put(font->page_sprites[ix],x + 2.0f,y + 1.0f,z + 1e-05f,1.0f,1.0f,u0,v0,u1,0,5,0xffffff,v1,1.0f,0xff000000);
        sprite_colors_reset();
      }
      x += adv;
    }
  }
  sprite_text_x_last = x;
}

float sprite_text_end_x(void)
{
  return sprite_text_x_last;
}

void sprite_font_text_put(sprite_font *font, char *text, float x, float y_arg, uint flags_arg, float z_arg)
{
  float y = y_arg;
  volatile float z = z_arg;
  uint flags = flags_arg;
  int row0;
  float cu;
  float cv;
  int px;
  char *p;
  int w;
  float u0;
  float v0;
  float u1;
  float v1;

  row0 = font->first_row;
  cu = font->cell_u;
  cv = font->cell_v;
  px = (int)x;
  strlen(text);
  p = text;
  if (flags & 2) {
    for (; *p != 0; p++) {
      {
        if (flags & 0x10) px = (int)((float)px - font->cell_width * sprite_put_state.scale_x);
        else px = (int)((float)px - (float)font_char_width[*p - 32][font->size] * sprite_put_state.scale_x);
      }
    }
  }
  else if (flags & 4) {
    for (; *p != 0; p++) {
      {
      if (flags & 0x10) px = (int)((float)px - font->cell_width * 0.5 * sprite_put_state.scale_x);
      else px = (int)((float)px - (float)font_char_width[*p - 32][font->size] * 0.5 * sprite_put_state.scale_x);
      }
    }
  }
  if (*text != 0) {
    do {
      if (flags & 0x10) {
        w = font_char_width[*text - 32][font->size];
        if (w < (int)(font->cell_width - 4.0 * sprite_put_state.scale_x)) {
          px = (int)((float)px + (font->cell_width - (float)w) * 0.5 * sprite_put_state.scale_x);
        }
      }
      u0 = (float)(char)(*text % 16);
      u0 = u0 * cu;
      u1 = u0 + cu;
      v0 = (float)((char)(*text / 16) - row0) * cv;
      v1 = v0 + cv;
      sprite_put(font->sprite_no,(float)px,y,z,1.0,1.0,u0,v0,u1,0,5,
                   0xffffff,v1,1.0,0xff000000);
      if (flags & 0x10) {
        w = font_char_width[*text - 32][font->size];
        if (w < (int)font->cell_width - 4) {
          px = (int)((float)px - (font->cell_width - (float)w) * 0.5 * sprite_put_state.scale_x);
        }
        px = (int)((float)px + font->cell_width * sprite_put_state.scale_x);
      }
      else {
        px = (int)((float)px + (float)font_char_width[*text - 32][font->size] * sprite_put_state.scale_x);
      }
      text++;
    } while (*text != 0);
  }
}

void sprite_task_init(void)
{
  int i;
  
  sprite_clip_count = '\0';
  sprite_make_errors = 0;
  game_mes_loaded = 0;
  char_side_tex_mask.side0 = 0;
  char_side_tex_mask.side1 = 0;
  char_textures_loaded = 0;
  sprite_queue_state.count = 0;
  for (i = 0; i < 0x300; i++) {
    sprite_queue_state.marks[i] = 0;
  }
  sprite_offset_reset();
  sprite_alpha_reset();
  sprite_colors_reset();
  sprite_scale_reset();
  sprite_flags_override_reset();
}

void sprite_task_exec(void)
{
  float start_usec;
  int i;
  char *entry;
  char prev;
  char clip;
  char *p;
  char *q;
  char mark;
  
  clip = 0;
  start_usec = (int)stopwatch_usec();
  entry = (char *)sprite_queue;
  prev = 0;
  for (i = 0; i < sprite_queue_state.count; i++) {
    if (sprite_queue_state.marks[i] != 0) {
      if ((sprite_queue_state.marks[i] & 1) != 0) {
        if ((prev & 1) == 0) {
          blend_factor_pair_keep(4,1);
        }
        q = &sprite_queue_state.marks[i];
        mark = *q;
        prev = mark & 1;
        *q = mark & 0xfe;
      }
      else if (sprite_queue_state.marks[i] & 2) {
        if ((prev & 2) == 0) {
          blend_factor_pair_keep(3,0);
        }
        q = &sprite_queue_state.marks[i];
        mark = *q;
        prev = mark & 2;
        *q = mark & 0xfd;
      }
      else if (sprite_queue_state.marks[i] & 4) {
        if ((prev & 4) == 0) {
          blend_factor_pair_keep(4,5);
        }
        q = &sprite_queue_state.marks[i];
        mark = *q;
        prev = mark & 4;
        *q = mark & 0xfb;
      }
      else {
        prev = sprite_queue_state.marks[i];
      }
      p = &sprite_queue_state.marks[i];
      if ((*p & 8) != 0) {
        sprite_clip_apply(clip);
        clip = clip + 1;
        *p = *p & 0xf7;
      }
    }
    sprite_quad_send_unrotated((sprite_quad_args *)entry);
    sprite_queue_state.marks[i] = 0;
    entry = entry + 0x50;
  }
  blend_factor_pair_keep(4,5);
  p = &sprite_queue_state.marks[i];
  if ((*p & 8) != 0) {
    sprite_clip_apply((char)(sprite_clip_count - 1));
    *p = *p & 0xf7;
  }
  sprite_queue_last_count = sprite_queue_state.count;
  sprite_queue_state.count = 0;
  sprite_queue_state.marks[i] = 0;
  sprite_clip_count = 0;
  sprite_offset_reset();
  sprite_alpha_reset();
  sprite_colors_reset();
  sprite_scale_reset();
  sprite_angle_base_reset();
  sprite_task_exec_usec = (int)((float)(int)stopwatch_usec() - start_usec);
}

void sprite_put(uint sprite_no, float x, float y, float z, float scale_x, float scale_y, float u0, float v0, float u1, int angle, uint flags, uint col, float v1, float alpha, uint offset_col)
{
  typedef struct { uint page_id; float x; float y; float z; float zoom_x; float zoom_y; float u0; float v0; float u1; float v1; uint angle; float alpha; int list; uint flags; uint col; uint offset_col; } queue_entry;
  float a = alpha;
  uint f = flags;
  uint c = col;
  short k = angle;
  queue_entry *entry;
  short sz = 80;
  sprite_cell *cell;

  entry = (queue_entry *)((int)sprite_queue_state.count * sz + (int)sprite_queue);
  cell = &sprite_set_cells[(int)sprite_no / 400][(int)sprite_no % 400];
  entry->page_id = cell->page_id;
  entry->x = x + sprite_put_state.offset.x;
  entry->y = y + sprite_put_state.offset.y;
  entry->z = z + sprite_put_state.offset.z;
  entry->zoom_x = (u1 - u0) * (cell->u1 - cell->u0) * scale_x * sprite_put_state.scale_x;
  entry->zoom_y = (v1 - v0) * (cell->v1 - cell->v0) * scale_y * sprite_put_state.scale_y;
  if (f & 0x10) {
    entry->u0 = 1.0 - (u1 * (cell->u1 - cell->u0) + cell->u0);
    entry->u1 = 1.0 - (u0 * (cell->u1 - cell->u0) + cell->u0);
  }
  else {
    entry->u0 = cell->u0 + u0 * (cell->u1 - cell->u0);
    entry->u1 = cell->u0 + u1 * (cell->u1 - cell->u0);
  }
  if (f & 0x20) {
    entry->v0 = 1.0 - (cell->v0 + v1 * (cell->v1 - cell->v0));
    entry->v1 = 1.0 - (cell->v0 + v0 * (cell->v1 - cell->v0));
  }
  else {
    entry->v0 = cell->v0 + v0 * (cell->v1 - cell->v0);
    entry->v1 = cell->v0 + v1 * (cell->v1 - cell->v0);
  }
  entry->angle = (unsigned short)(sprite_put_state.angle_base + k);
  entry->alpha = sprite_put_state.alpha * a;
  entry->list = -1;
  if (sprite_put_state.flags_override)
    entry->flags = sprite_put_state.flags_override | 0x20000;
  else
    entry->flags = f | 0x20000;
  if (sprite_put_state.col_override != 0xffffff)
    entry->col = sprite_put_state.col_override & 0xffffff;
  else
    entry->col = (uint)c & 0xffffff;
  if (sprite_put_state.offset_col_override != 0xff000000)
    entry->offset_col = sprite_put_state.offset_col_override | 0xff000000;
  else
    entry->offset_col = offset_col | 0xff000000;
  if (sprite_queue_state.count < 0x2ff)
    sprite_queue_state.count++;
}

void sprite_next_blend_set(int mode)
{
  switch ((char)mode) {
  case 0:
    sprite_queue_state.marks[sprite_queue_state.count] |= 1;
    break;
  case 1:
    sprite_queue_state.marks[sprite_queue_state.count] |= 2;
    break;
  }
}

void sprite_next_blend_normal(void)
{
  typedef struct { unsigned char a : 1; unsigned char b : 1; unsigned char c : 1; } bits;
  sprite_queue_state_rec *t = &sprite_queue_state; bits *p = (bits *)&t->marks[t->count];
  p->a = 0;
  p->b = 0;
  p->c = 1;
  return;
}

void vram_used_total_update(void)
{
  int i;
  int total;

  total = 0;
  for (i = 0; i < 0x51; i++) {
    total += sprite_set_vram_used[i];
    sprite_vram_used_total = total;
  }
  vram_used_total = sprite_vram_used_total + stage_obj_vram_used + stage_ef_obj_vram_used + common_obj_vram_used +
                    rob_body_vram_used.side0 + rob_body_vram_used.side1 + rob_items_vram_used.side0 +
                    rob_items_vram_used.side1;
}

void sprite_list_put(int *list, float z)
{
  while (*list != -1) {
    sprite_put_xyz(*list,*(float *)(list + 1),*(float *)(list + 2),z);
    list = list + 3;
  }
  return;
}

void sprite_clip_push(x0,y0,x1,y1,mode,hdr) char x0; char y0; char x1; char y1; char mode; char hdr;
{
  if (sprite_clip_count < 20) {
    sprite_queue_state.marks[sprite_queue_state.count] |= 8;
    sprite_clip_rects[sprite_clip_count].x0 = x0;
    sprite_clip_rects[sprite_clip_count].y0 = y0;
    sprite_clip_rects[sprite_clip_count].x1 = x1;
    sprite_clip_rects[sprite_clip_count].y1 = y1;
    sprite_clip_rects[sprite_clip_count].mode = mode;
    sprite_clip_rects[sprite_clip_count].hdr = hdr;
    sprite_clip_count++;
  }
}

void sprite_clip_apply(char clip)
{
  pcw_user_clip_mode_put(0x1f,sprite_clip_rects[clip].mode);
  clip_rect_per_list_bits(sprite_clip_rects[clip].x0,sprite_clip_rects[clip].y0,sprite_clip_rects[clip].x1,sprite_clip_rects[clip].y1,0x1f);
  elan_lists_end_with_clip(sprite_clip_rects[clip].x0,sprite_clip_rects[clip].y0,sprite_clip_rects[clip].x1,sprite_clip_rects[clip].y1,0x1f);
  hdr_or_user_clip(sprite_clip_rects[clip].hdr);
}

int sprite_list_records_make(int pages, uint value, int mode, int set_no)
{
  int format;
  int i;
  int r;
  char *failed;
  
  if (mode != 0) {
    if (mode == 1) {
      i = 0;
      format = *(int *)(pages + 4);
      while (format != -1) {
        tex_pixels_yuv_adjust((short *)(pages + i * 0x10),1.0,0.0,0.0);
        i = i + 1;
        format = *(int *)(i * 0x10 + pages + 4);
      }
    }
    else {
      i = 0;
      format = *(int *)(pages + 4);
      while (format != -1) {
        tex_pixels_yuv_tint((short *)(pages + i * 0x10),1.0,0.1,1.0,0.8,0.5);
        i = i + 1;
        format = *(int *)(i * 0x10 + pages + 4);
      }
    }
  }
  r = sprite_records_make_list(pages,value);
  if (r != 0) {
    failed = sprite_set_make_failed;
    failed[set_no] = 1;
  }
  return r;
}

int sprite_list_record_make(int pages, uint value, int pos, int mode, int set_no)
{
  int r;
  char *failed;
  
  if (mode != 0) {
    if (mode == 1) {
      tex_pixels_yuv_adjust((short *)(pages + pos * 0x10),1.0,0.0,0.0);
    }
    else {
      tex_pixels_yuv_tint((short *)(pages + pos * 0x10),1.0,0.1,1.0,0.8,0.5);
    }
  }
  r = sprite_record_make_by_pos(pages,value,pos);
  if (r != 0) {
    failed = sprite_set_make_failed;
    failed[set_no] = 1;
  }
  return r;
}

void sprite_list_texture_reload(int pos, int pages, int mode)
{
  if (mode != 0) {
    if (mode == 1) {
      tex_pixels_yuv_adjust((short *)(pages + pos * 0x10),1.0,0.0,0.0);
    }
    else {
      tex_pixels_yuv_tint((short *)(pages + pos * 0x10),1.0,0.1,1.0,0.8,0.5);
    }
  }
  texidx_slot_overwrite(pos,*(uint *)(pages + 8));
  return;
}

int tex_pixels_yuv_adjust(short *tex, float luma_scale, float chroma_scale, float offset)
{
  typedef struct { short w; short h; int flags; ushort *data; } texhead;
  texhead *t = (texhead *)tex;
  ushort *p;
  ushort *end;
  int fmt;
  int n;
  int px;
  float r, g, b;
  float y, u, v;

  if (t->data == 0 || t->w == 0) {
    return 0;
  }
  fmt = t->flags & 3;
  p = t->data;
  switch (t->flags & 0xf00) {
  case 0x300:
  case 0x400:
    end = (ushort *)((char *)t->data + 0x800);
    break;
  case 0x100:
  case 0x900:
  case 0xd00:
    end = (ushort *)((char *)t->data + t->w * t->h * 2);
    break;
  case 0x200:
    end = t->data;
    for (n = t->w; n > 0; n >>= 1) {
      end += n * n;
    }
    break;
  default:
    return -1;
  }
  switch (fmt) {
  case 0: {
    ushort *q;
    for (q = p; q < end; q++) {
      r = (*q & 0x7c00) >> 10;
      g = ((short)*q & 0x3e0) >> 5;
      b = (short)*q & 0x1f;
      r *= 0.032258064f;
      g *= 0.032258064f;
      b *= 0.032258064f;
      y = (r * 0.299f + g * 0.587f + b * 0.114f) * luma_scale;
      u = (r * -0.147f - g * 0.289f + b * 0.436f) * chroma_scale;
      v = (r * 0.615f - g * 0.515f - b * 0.1f) * chroma_scale;
      r = y + u * 0.0f + v * 1.14f + offset;
      g = y - u * 0.396f - v * 0.581f + offset;
      b = y + u * 2.029f + v * 0.0f + offset;
      if (r > 1.0f) r = 1.0f; else if (r < 0.0f) r = 0.0f;
      if (g > 1.0f) g = 1.0f; else if (g < 0.0f) g = 0.0f;
      if (b > 1.0f) b = 1.0f; else if (b < 0.0f) b = 0.0f;
      r *= 31.0f;
      g *= 31.0f;
      b *= 31.0f;
      *q = (*q & 0x8000) | (((int)r & 0x1f) << 10) | ((uchar)((int)g << 3) << 2) | ((int)b & 0x1f);
    }
    break;
  }
  case 1: {
    ushort *q;
    for (q = p; q < end; q++) {
      r = ((short)*q & 0xf800) >> 11;
      g = ((short)*q & 0x7e0) >> 5;
      b = (short)*q & 0x1f;
      r *= 0.032258064f;
      g *= 0.015873017f;
      b *= 0.032258064f;
      y = (r * 0.3f + g * 0.59f + b * 0.11f) * luma_scale;
      u = (r - y) * chroma_scale;
      v = (b - y) * chroma_scale;
      r = y + u + offset;
      g = y - u * 0.50847465f - v * 0.18644069f + offset;
      b = y + v + offset;
      y = (r * 0.299f + g * 0.587f + b * 0.114f) * luma_scale;
      u = (r * -0.147f - g * 0.289f + b * 0.436f) * chroma_scale;
      v = (r * 0.615f - g * 0.515f - b * 0.1f) * chroma_scale;
      r = y + u * 0.0f + v * 1.14f + offset;
      g = y - u * 0.396f - v * 0.581f + offset;
      b = y + u * 2.029f + v * 0.0f + offset;
      if (r > 1.0f) r = 1.0f; else if (r < 0.0f) r = 0.0f;
      if (g > 1.0f) g = 1.0f; else if (g < 0.0f) g = 0.0f;
      if (b > 1.0f) b = 1.0f; else if (b < 0.0f) b = 0.0f;
      r *= 31.0f;
      g *= 63.0f;
      b *= 31.0f;
      *q = (((int)r & 0x1f) << 11) | ((uchar)((int)g << 2) << 3) | ((int)b & 0x1f);
    }
    break;
  }
  case 2: {
    ushort *q;
    for (q = p; q < end; q++) {
      px = (short)*q;
      r = (px >> 8) & 0xf;
      g = (px & 0xf0) >> 4;
      b = px & 0xf;
      r *= 0.06666667f;
      g *= 0.06666667f;
      b *= 0.06666667f;
      y = (r * 0.3f + g * 0.59f + b * 0.11f) * luma_scale;
      u = (r - y) * chroma_scale;
      v = (b - y) * chroma_scale;
      r = y + u + offset;
      g = y - u * 0.50847465f - v * 0.18644069f + offset;
      b = y + v + offset;
      y = (r * 0.299f + g * 0.587f + b * 0.114f) * luma_scale;
      u = (r * -0.147f - g * 0.289f + b * 0.436f) * chroma_scale;
      v = (r * 0.615f - g * 0.515f - b * 0.1f) * chroma_scale;
      r = y + u * 0.0f + v * 1.14f + offset;
      g = y - u * 0.396f - v * 0.581f + offset;
      b = y + u * 2.029f + v * 0.0f + offset;
      if (r > 1.0f) r = 1.0f; else if (r < 0.0f) r = 0.0f;
      if (g > 1.0f) g = 1.0f; else if (g < 0.0f) g = 0.0f;
      if (b > 1.0f) b = 1.0f; else if (b < 0.0f) b = 0.0f;
      r *= 15.0f;
      g *= 15.0f;
      b *= 15.0f;
      *q = (*q & 0xf000) | (((int)r & 0xf) << 8) | (((int)g & 0xf) << 4) | ((int)b & 0xf);
    }
    break;
  }
  default:
    return -2;
  }
  return 0;
}

int tex_pixels_yuv_tint(short *tex, float luma_scale, float chroma_scale, float r_scale, float g_scale, float b_scale)
{
  typedef struct {
    short w;
    short h;
    int fmt;
    short *data;
  } tex_hdr;
  tex_hdr *t = (tex_hdr *)tex;
  short *s;
  short *p;
  short *end;
  int pf;
  int i;
  float r, g, b, y, u, v;

  if (t->data == 0 || t->w == 0) {
    return 0;
  }
  pf = t->fmt & 3;
  s = t->data;
  switch (t->fmt & 0xf00) {
  case 0x300:
  case 0x400:
    end = t->data + 0x400;
    break;
  case 0x100:
  case 0x900:
  case 0xd00:
    end = t->data + t->w * t->h;
    break;
  case 0x200:
    end = t->data;
    for (i = t->w; i > 0; i >>= 1) {
      end += i * i;
    }
    break;
  default:
    return -1;
  }
  switch (pf) {
  case 0:
    for (p = s; p < end; p++) {
      r = ((*p & 0x7c00) >> 10);
      g = ((*p & 0x3e0) >> 5);
      b = (*p & 0x1f);
      r *= (1.0 / 31.0);
      g *= (1.0 / 31.0);
      b *= (1.0 / 31.0);
      y = (r * 0.299 + g * 0.587 + b * 0.114) * luma_scale;
      u = (r * -0.147 - g * 0.289 + b * 0.436) * chroma_scale;
      v = (r * 0.615 - g * 0.515 - b * 0.1) * chroma_scale;
      r = (y + u * 0.0 + v * 1.14) * r_scale;
      g = (y - u * 0.396 - v * 0.581) * g_scale;
      b = (y + u * 2.029 + v * 0.0) * b_scale;
      if (r > 1.0) r = 1.0;
      else if (r < 0.0) r = 0.0;
      if (g > 1.0) g = 1.0;
      else if (g < 0.0) g = 0.0;
      if (b > 1.0) b = 1.0;
      else if (b < 0.0) b = 0.0;
      r *= 31.0;
      g *= 31.0;
      b *= 31.0;
      *p = *p & 0x8000 | ((int)r & 0x1f) << 10 | ((int)g & 0x1f) << 5 | (int)b & 0x1f;
    }
    break;
  case 1:
    for (p = s; p < end; p++) {
      r = (*p & 0xf800) >> 11;
      g = ((*p & 0x7e0) >> 5);
      b = (*p & 0x1f);
      r *= (1.0 / 31.0);
      g *= (1.0 / 63.0);
      b *= (1.0 / 31.0);
      y = (r * 0.299 + g * 0.587 + b * 0.114) * luma_scale;
      u = (r * -0.147 - g * 0.289 + b * 0.436) * chroma_scale;
      v = (r * 0.615 - g * 0.515 - b * 0.1) * chroma_scale;
      r = (y + u * 0.0 + v * 1.14) * r_scale;
      g = (y - u * 0.396 - v * 0.581) * g_scale;
      b = (y + u * 2.029 + v * 0.0) * b_scale;
      if (r > 1.0) r = 1.0;
      else if (r < 0.0) r = 0.0;
      if (g > 1.0) g = 1.0;
      else if (g < 0.0) g = 0.0;
      if (b > 1.0) b = 1.0;
      else if (b < 0.0) b = 0.0;
      r *= 31.0;
      g *= 63.0;
      b *= 31.0;
      *p = ((int)r & 0x1f) << 11 | ((int)g & 0x3f) << 5 | (int)b & 0x1f;
    }
    break;
  case 2:
    for (p = s; p < end; p++) {
      r = ((*p & 0xf00) >> 8);
      g = ((*p & 0xf0) >> 4);
      b = (*p & 0xf);
      r *= (1.0 / 15.0);
      g *= (1.0 / 15.0);
      b *= (1.0 / 15.0);
      y = (r * 0.299 + g * 0.587 + b * 0.114) * luma_scale;
      u = (r * -0.147 - g * 0.289 + b * 0.436) * chroma_scale;
      v = (r * 0.615 - g * 0.515 - b * 0.1) * chroma_scale;
      r = (y + u * 0.0 + v * 1.14) * r_scale;
      g = (y - u * 0.396 - v * 0.581) * g_scale;
      b = (y + u * 2.029 + v * 0.0) * b_scale;
      if (r > 1.0) r = 1.0;
      else if (r < 0.0) r = 0.0;
      if (g > 1.0) g = 1.0;
      else if (g < 0.0) g = 0.0;
      if (b > 1.0) b = 1.0;
      else if (b < 0.0) b = 0.0;
      r *= 15.0;
      g *= 15.0;
      b *= 15.0;
      *p = *p & 0xf000 | ((int)r & 0xf) << 8 | ((int)g & 0xf) << 4 | (int)b & 0xf;
    }
    break;
  default:
    return -2;
  }
  return 0;
}

void keys_vec3_sample(float frame, int *track, float *base, float *out)
{
  int n;
  int j;
  int k;
  int i;
  float *a;
  float *b;
  float *p;
  float f;
  float d;

  n = *track - 1;
  track = (int *)track[2];
  j = -1;
  k = -1;
  i = 0;
  for (; i < n; i++) {
    if (((float *)track)[i * 4] + *base <= frame && frame < (((float *)track) + i * 4)[4] + *base) {
      k = i;
      j = i + 1;
      break;
    }
  }
  if (*((float *)track) + *base > frame) {
    out[0] = ((float *)track)[1] + base[1];
    out[1] = ((float *)track)[2] + base[2];
    out[2] = ((float *)track)[3] + base[3];
  } else if (k == -1) {
    p = (float *)(i * 16 + (int)track);
    out[0] = p[1] + base[1];
    out[1] = p[2] + base[2];
    out[2] = p[3] + base[3];
  } else {
    a = (float *)(k * 16 + (int)track);
    d = frame - (*a + *base);
    b = (float *)(j * 16 + (int)track);
    f = d / (*b - *a);
    out[0] = (a[1] - f * (a[1] - b[1])) + base[1];
    out[1] = (a[2] - (a[2] - b[2]) * f) + base[2];
    out[2] = (a[3] - f * (a[3] - b[3])) + base[3];
  }
}

void sprite_anim_scale_eval(float frame, int *track, float *base, float *out)
{
  float *t;
  float f;
  int n;
  int hi;
  int lo;
  int i;

  n = *track - 1;
  t = (float *)track[2];
  hi = -1;
  lo = -1;
  for (i = 0; i < n; i++) {
    if ((t[i * 3] + *base <= frame) && (frame < t[i * 3 + 3] + *base)) {
      lo = i;
      hi = i + 1;
      break;
    }
  }
  if (t[0] + *base > frame) {
    out[0] = t[1] * base[4];
    out[1] = t[2] * base[5];
  }
  else if (lo == -1) {
    out[0] = (t + i * 3)[1] * base[4];
    out[1] = (t + i * 3)[2] * base[5];
  }
  else {
    f = (frame - (t[lo * 3] + *base)) / (t[hi * 3] - t[lo * 3]);
    out[0] = (t[lo * 3 + 1] - (t[lo * 3 + 1] - t[hi * 3 + 1]) * f) * base[4];
    out[1] = (t[lo * 3 + 2] - f * (t[lo * 3 + 2] - t[hi * 3 + 2])) * base[5];
  }
}

void sprite_anim_alpha_eval(float frame, int *track, float *base, float *out)
{
  int n = *track - 1;
  float *k = (float *)track[2];
  int hi = -1;
  int lo = -1;
  int i;
  float r;
  for (i = 0; i < n; i++) {
    if (k[i * 2] + *base <= frame && frame < k[i * 2 + 2] + *base) {
      lo = i;
      hi = i + 1;
      break;
    }
  }
  if (k[0] + *base > frame) *out = 0.0;
  else if (lo == -1) {
    if (base[6] < 1.0) *out = 0.0;
    else *out = k[i * 2 + 1] * base[6];
  }
  else {
    float x0 = k[lo * 2];
    *out = (k[lo * 2 + 1] - ((frame - (*base + x0)) / (k[hi * 2] - x0)) * (k[lo * 2 + 1] - k[hi * 2 + 1])) * base[6];
  }
}

void sprite_anim_play(int *anim, float frame)
{
  typedef struct { int w[7]; } blk;
  float k = frame;
  int *p = anim;
  blk b;
  float t[3];
  float s[2];
  float z;
  int *q;
  void (*f)(float, float, float);

  while (*p != 0) {
    t[0] = t[1] = t[2] = 0.0;
    z = s[0] = s[1] = 1.0;
    b = *(blk *)(p + 2);
    q = (int *)p[1];
    while (*q != -1) {
      switch (q[1]) {
      case 0:
        keys_vec3_sample(k,q,(float *)&b,t);
        break;
      case 1:
        sprite_anim_scale_eval(k,q,(float *)&b,s);
        break;
      case 2:
        sprite_anim_alpha_eval(k,q,(float *)&b,&z);
        break;
      }
      q += 3;
    }
    f = (void (*)(float, float, float))*p;
    sprite_alpha_set(z);
    sprite_scale_set(s[0],s[1]);
    f(t[0],t[1],t[2]);
    sprite_scale_reset();
    sprite_alpha_reset();
    p += 9;
  }
}

void sprite_anim_play_with_arg(int arg, int *anim, float frame)
{
  int *p = anim;
  int a[7];
  float z[3];
  float m[2];
  float s[1];
  int *e;
  int *src;
  void (*fn)(int, float, float, float);

  while (*p != 0) {
    z[0] = z[1] = z[2] = 0.0;
    m[0] = m[1] = 1.0;
    s[0] = 1.0;
    src = p + 2;
    a[0] = src[0];
    a[1] = src[1];
    a[2] = src[2];
    a[3] = src[3];
    a[4] = src[4];
    a[5] = src[5];
    a[6] = src[6];
    for (e = (int *)p[1]; *e != -1; e += 3) {
      switch (e[1]) {
      case 0:
        keys_vec3_sample(frame, e, (float *)a, z);
        break;
      case 1:
        sprite_anim_scale_eval(frame, e, (float *)a, m);
        break;
      case 2:
        sprite_anim_alpha_eval(frame, e, (float *)a, s);
        break;
      }
    }
    fn = (void (*)(int, float, float, float))*p;
    sprite_alpha_set(s[0]);
    sprite_scale_set(m[0], m[1]);
    fn(arg, z[0], z[1], z[2]);
    sprite_scale_reset();
    sprite_alpha_reset();
    p += 9;
  }
}

uint sprite_color_pack(float alpha, float red, float green, float blue)
{
  uint a;
  uint r;
  uint g;
  uint b;

  alpha *= 255.0;
  red *= 255.0;
  green *= 255.0;
  blue *= 255.0;
  a = __fixunssfsi(alpha);
  r = __fixunssfsi(red);
  g = __fixunssfsi(green);
  b = __fixunssfsi(blue);
  if (0xff < a) {
    a = 0xff;
  }
  if (0xff < r) {
    r = 0xff;
  }
  if (0xff < g) {
    g = 0xff;
  }
  if (0xff < b) {
    b = 0xff;
  }
  a = a << 8 | r;
  a = a << 8 | g;
  a = a << 8 | b;
  return a;
}

int sprite_quad_send_unrotated(sprite_quad_args *entry)
{
  int *s = (int *)entry;
  float px;
  float py;
  float *tp;
  float *sp;
  float tw;
  float sw;
  float th;
  float sh;
  uint *t;
  uint *d;
  uint list;
  uint f;
  uint m;
  uint acol;
  uint a;
  uint g;
  uint c;
  volatile uint *q;
  int idx;
  int blend;
  int n;
  float alpha;
  float sx;
  float sy;
  float inv;
  float u0;
  float u1;
  float v0;
  float v1;
  float x[4];
  float y[4];

  alpha = 1.0f;
  acol = 0xff000000;
  if (entry->angle != 0) {
    return sprite_quad_to_list(entry);
  }
  if (((entry->attr & 0xf) != 0) & ((entry->attr & 0xf) != 5)) {
    return sprite_quad_to_list(entry);
  }
  if (*s >= sprite_index_count) return 0xffffffff;
  idx = sprite_index_to_record[*s];
  if (idx == -1) return 0;
  t = (uint *)(sprite_records + idx * 0x20);
  px = *(float *)(s + 1);
  py = *(float *)(s + 2);
  sx = proj_rect_px_width * ((float *)&sprite_coord_recip)[0];
  sy = proj_rect_px_height * ((float *)&sprite_coord_recip)[1];
  tp = (float *)t + 6;
  sp = (float *)s + 4;
  tw = tp[0];
  sw = sp[0];
  tp++;
  sp++;
  th = *tp;
  sh = *sp;
  x[0] = px * sx;
  x[3] = x[0];
  x[1] = (px + tw * sw) * sx;
  x[2] = x[1];
  y[0] = py * sy;
  y[1] = y[0];
  y[2] = (py + th * sh) * sy;
  y[3] = y[2];
  t[0] = t[0] & 0xf8fcffff;
  t[1] = t[1] & 0x1fffffff;
  f = t[2];
  t[2] = f & 0x3278fff;
  if ((s[13] & 0x800) == 0) t[2] = f & 0x3278fff | 0x800000;
  if ((s[13] & 0x1000) != 0) t[2] = t[2] | 0x2000;
  if ((s[13] & 0x10000) != 0) t[2] = t[2] | 0x1000;
  m = (uint)s[13] >> 8 & 7;
  if (m == 0) m = 4;
  t[1] = m << 29 | t[1];
  switch (s[12]) {
  case 0:
    blend = 0;
    break;
  default:
    alpha = *(float *)(s + 11);
    blend = 0;
    if (alpha < 1.0f || *(short *)(t + 5) != 0) blend = 1;
    break;
  case 2:
  case 4:
    alpha = *(float *)(s + 11);
    blend = 1;
    break;
  }
  if (blend) {
    if (s[12] == 4) {
      t[2] = t[2] | 0x94100000;
      t[0] = t[0] | 0x04000000;
    }
    else {
      t[2] = (blend_pair_bits ^ 0xfc) << 24 | t[2] | 0x100000;
      t[0] = t[0] | 0x02000000;
    }
    a = __fixunssfsi(alpha * 255.0f);
    if (a > 0xff) a = 0xff;
    acol = a << 24;
  }
  else {
    t[2] = t[2] | 0x20080000;
  }
  if (s[13] & 0x2000) {
    t[4] = sprite_color_pack(alpha, ((float *)&obj_light_color)[0], ((float *)&obj_light_color)[1], ((float *)&obj_light_color)[2]);
  }
  else {
    if (s[13] & 0x20000) c = s[14];
    else c = 0xffffff;
    t[4] = c | acol;
  }
  switch (s[13] & 0xc000) {
  case 0x8000:
    t[0] = t[0] | 0x20000;
    break;
  case 0xc000:
    t[0] = t[0] | 0x30000;
    break;
  }
  n = s[12];
  if (n == -1) n = blend * 2;
  d = (uint *)list_write_ptrs[n];
  q = (volatile uint *)0xff000038;
  q[0] = (uint)d >> 24;
  q[1] = (uint)d >> 24;
  d = (uint *)((uint)d & 0x3ffffff | 0xe0000000);
  if (s[13] & 0x20000) d[0] = t[0] | poly_header_or_words.control_or | 4;
  else d[0] = t[0] | poly_header_or_words.control_or;
  d[1] = t[1];
  d[2] = t[2] | poly_header_or_words.tsp_or;
  d[3] = t[3];
  d[4] = t[4];
  d[5] = s[15] | acol;
  __asm__ __volatile__ ("pref @%0" : : "r" (d));
  d += 8;
  inv = 1.0f / *(float *)(s + 3);
  d[0] = 0xf0000000;
  d[1] = ((uint *)x)[0];
  d[2] = ((uint *)y)[0];
  ((float *)d)[3] = inv;
  d[4] = ((uint *)x)[1];
  d[5] = ((uint *)y)[1];
  ((float *)d)[6] = inv;
  d[7] = ((uint *)x)[2];
  d[8] = ((uint *)y)[2];
  ((float *)d)[9] = inv;
  d[10] = ((uint *)x)[3];
  d[11] = ((uint *)y)[3];
  d[12] = idx | 0x53500000;
  g = s[13] & 0x30;
  u0 = *(float *)(s + 6);
  u1 = *(float *)(s + 8);
  if (g & 0x10) {
    u0 = 1.0f - u0;
    u1 = 1.0f - u1;
  }
  v0 = *(float *)(s + 7);
  v1 = *(float *)(s + 9);
  if ((g & 0x20) == 0) {
    v0 = 1.0f - v0;
    v1 = 1.0f - v1;
  }
  {
    union { float f; uint i; } fb;
    uint hu0;
    uint hu1;
    uint lv0;
    uint lv1;
    fb.f = u0;
    hu0 = fb.i & 0xffff0000;
    fb.f = u1;
    hu1 = fb.i & 0xffff0000;
    fb.f = v0;
    lv0 = fb.i >> 16;
    fb.f = v1;
    lv1 = fb.i >> 16;
    d[13] = hu0 | lv0;
    d[14] = lv0 | hu1;
    d[15] = hu1 | lv1;
  }
  __asm__ __volatile__ ("pref @%0" : : "r" (d));
  d += 8;
  __asm__ __volatile__ ("pref @%0" : : "r" (d));
  d += 8;
  list_write_ptrs[n] = list_write_ptrs[n] & 0xfc000000 | (uint)d & 0x3ffffff;
  return 0;
}

void sprite_white_rect_draw(float x, float y, float scale_x, float scale_y, float alpha)
{
  sprite_put(0,x,y,0.03,scale_x,scale_y,0.0,0.0,1.0,0,0x805,0xffffffff,1.0,alpha,
               0xffffffff);
  return;
}
