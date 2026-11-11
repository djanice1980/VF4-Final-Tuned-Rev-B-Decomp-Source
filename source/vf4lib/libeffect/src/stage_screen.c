/*
 * vf4lib/libeffect/src/stage_screen.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char demo_stage_select; /* 0x0c28c787 */
extern int game_frame_count; /* 0x0c292540 */
extern undefined *board_config; /* 0x0c360178 */
extern int stage_screen_count; /* 0x0c4510cc */
extern stage_screen_slot stage_screens[2]; /* 0x0c4510d0 */
extern float screen_noise_lines[257]; /* 0x0c451ba8 */
extern screen_noise_rec screen_noise_records[2]; /* 0x0c451fac */
extern camera_state camera; /* 0x0c370ce0 */
extern screen_proj_factors stage_screen_proj; /* 0x0c382418 */
extern water_spare_vec3 water_grid_spare_vec; /* 0x0c413460 */
extern water_state_rec water_state; /* 0x0c41347c */
extern view_side_planes_rec view_side_planes; /* 0x0c41cab0 */
extern stage_screen_tex_rec screen_texs_0c2a2618[2][2]; /* 0x0c2a2618 */
extern stage_screen_desc screen_descs0_0c2a26a8[2]; /* 0x0c2a26a8 */
extern stage_screen_desc screen_descs1_0c2a26e0[2]; /* 0x0c2a26e0 */
extern float water_height_scale; /* 0x0c2a2774 */
extern float water_amps_0c451164[64]; /* 0x0c451164 */
extern float water_speeds0_0c451264[64]; /* 0x0c451264 */
extern float water_speeds1_0c451364[64]; /* 0x0c451364 */

typedef struct { float x; float y; float z; } vec_t; typedef struct { float h; vec_t n; float u; float v; uint color; } vtx_t; static inline float fsrra_asm(float x)
{
  __asm__ ("fsrra %0" : "=f" (x) : "0" (x));
  return x;
}

void rendtx_task_start(void)
{
  return;
}

void rendtx_task_frame(void)
{
  stage_screens_draw();
  return;
}

void rendtx_task_stop(void)
{
  rob_draw_mode_set(0,0);
  rob_draw_mode_set(1,0);
  return;
}

void stage_screens_clear(void)
{
  uint i;

  stage_screen_count = 0;
  for (i = 0; i < 2; i++) {
    stage_screen_slot *base = stage_screens;
    stage_screen_slot *slot = base + i;
    slot->field_0x0 = 0;
    slot->screen.flags = 0;
    slot->screen.tex.target = 0;
    slot->screen.tex.tex_index = 0;
    slot->screen.tex.width = 0;
    slot->screen.tex.height = 0;
    slot->screen.stage = 0;
    slot->screen.robs[0] = 0;
    slot->screen.robs[1] = 0;
    slot->screen.callbacks[0] = 0;
    {
      void (**p)(void *) = &slot->screen.callbacks[1];
      *p = 0;
    }
    {
      void (**q)(void *) = &slot->screen.callbacks[2];
      *q = 0;
    }
  }
}

void stage_extras_setup(int chara0, int costume0, int chara1, int costume1, int stage_no)
{
  stage_rec *stage;
  stage_screen_desc *desc;
  int n;
  int done0; /* ORIGINAL SHAPE (assumed from the match; the 2026-11-10 audit narrowed it: all the bytes need is a second zero-valued local, and a text with one done flag plus side = 0; rob_obj_alt_request(side); for side 0 also matches): one 'already taken' flag per side, each side checking the other's; side 0's !done1 is always true when it runs and side 1's done1 = 1 is never read */
  int done1;
  uint frames;
  uint lim;
  float *a;
  float *b;

  stage = stage_table[stage_no];
  desc = (stage_screen_desc *)stage->objs_on;
  if (desc != 0) {
    bg_color_set(stage->bg_colors);
    n = 0;
    if ((int)desc->flags >= 0) {
      do {
        stage_screen_add(&stage_screens[n], desc, stage);
        desc++;
        n++;
      } while ((desc->flags & 0x80000000) == 0);
    }
    stage_screen_count = n;
    if (stage_no == 0x13) {
      water_surface_init(1.5);
      water_colors_set(0,1.0,1.0,1.0,1.0,1.0,0.0,0.0,0.0,5.0,1.0);
      water_amplitudes_set(water_table_scale(4.0,water_amplitude_table_build(1.0,water_amps_0c451164)));
      a = water_table_scale(0.04,water_dispersion_table_build(0.5,water_speeds0_0c451264));
      b = water_table_scale(0.04,water_dispersion_table_build(0.5,water_speeds1_0c451364));
      water_speeds_set(a,b);
      stage_screen_vert_scale = 1.0;
      water_height_scale = 2.5;
      stage_screen_view_scale = 0.95;
    }
    else if (stage_no == 0x14) {
      water_surface_init(1.5);
      water_colors_set(0,1.0,1.0,1.0,1.0,1.0,0.0,0.0,0.0,5.0,1.0);
      water_amplitudes_set(water_table_scale(5.0,water_amplitude_table_build(1.0,water_amps_0c451164)));
      a = water_table_scale(0.06,water_dispersion_table_build(0.5,water_speeds0_0c451264));
      b = water_table_scale(0.06,water_dispersion_table_build(0.5,water_speeds1_0c451364));
      water_speeds_set(a,b);
      stage_screen_vert_scale = 1.1;
      water_height_scale = 1.0;
      stage_screen_view_scale = 0.91;
    }
    else if (stage_no == 0x15 || stage_no == 7) {
      water_surface_init(1.5);
      water_colors_set(1,1.0,1.0,0.65,0.55,0.5,0.07,0.05,0.05,4.0,1.0);
      water_amplitudes_set(water_table_scale(4.0,water_amplitude_table_build(1.0,water_amps_0c451164)));
      a = water_table_scale(0.04,water_dispersion_table_build(0.5,water_speeds0_0c451264));
      b = water_table_scale(0.04,water_dispersion_table_build(0.5,water_speeds1_0c451364));
      water_speeds_set(a,b);
      stage_screen_vert_scale = 1.0;
      water_height_scale = 2.5;
      stage_screen_view_scale = 1.0;
    }
    else if (stage_no == 0xb) {
      done0 = 0;
      done1 = 0;
      frames = user_back_data._1p_match_point * 7200;
      lim = frames + 7200;
      if (chara0 == 0xd) {
        dural_alt_active = 0;
        if (!done1 && lim >= course_time_total) {
          rob_obj_alt_request(0);
          screen_descs0_0c2a26a8[0].tex = &screen_texs_0c2a2618[0][costume0];
          stage_screen_add(&stage_screens[stage_screen_count++], &screen_descs0_0c2a26a8[0], stage);
          done0 = 1;
          dural_alt_active = 1;
        }
      }
      if (chara1 == 0xd) {
        dural_alt_active = 0;
        if (!done0 && lim >= course_time_total) {
          rob_obj_alt_request(1);
          screen_descs1_0c2a26e0[0].tex = &screen_texs_0c2a2618[1][costume1];
          stage_screen_add(&stage_screens[stage_screen_count++], &screen_descs1_0c2a26e0[0], stage);
          dural_alt_active = 1;
          done1 = 1;
        }
      }
      stage_screen_vert_scale = 1.05;
      stage_screen_view_scale = 1.0;
    }
    else if (stage_no == 0x12) {
      screen_noise_init();
      rob_obj_alt_request(0);
      rob_obj_alt_request(1);
    }
    else if (stage_no == 0xf) {
      screen_noise_init();
    }
    if (*(int *)(board_config + 0xc) == 0) {
      stage_screen_view_scale = stage_screen_view_scale * 0.9;
    }
  }
}

void stage_screen_add(stage_screen_slot *slot, stage_screen_desc *desc, stage_rec *stage)
{
  rob_rec *rob;
  stage_screen_tex_rec *tex;

  slot->screen.flags = desc->flags;
  slot->screen.view = desc->view;
  slot->screen.after = desc->after;
  slot->screen.stage = stage;
  rob = rob_get(0);
  slot->screen.robs[0] = rob;
  rob = rob_get(1);
  slot->screen.robs[1] = rob;
  slot->screen.callbacks[0] = desc->callbacks[0];
  slot->screen.callbacks[1] = desc->callbacks[1];
  slot->screen.callbacks[2] = desc->callbacks[2];
  tex = desc->tex;
  if (tex != 0) {
    slot->screen.tex = *tex;
    slot->screen.tex.flags = slot->screen.tex.flags | 1;
    stage_screen_texture_bind(stage_screen_bank,desc->tex);
  }
  return;
}

void stage_screens_draw(void)
{
  uint i;

  mtx_stack_select(1);
  camera_view_load();
  for (i = 0; i < stage_screen_count; i = i + 1) {
      stage_screen_render(stage_screen_bank,&stage_screens[i].screen);
  }
  if (stage_screen_count != 0) {
    target_draw_select(0);
  }
  for (i = 0; i < stage_screen_count; i = i + 1) {
      stage_screen_callbacks_run(&stage_screens[i].screen);
  }
  return;
}

void stage_screens_free(void)
{
  uint i;

  for (i = 0; i < stage_screen_count; i = i + 1) {
      stage_screen_texture_release(&stage_screens[i].screen.tex);
  }
  stage_screens_clear();
  return;
}

void stage_screens_boot(void)
{
  uint i;

  vram_pair_pool_alloc(0x40000);
  target_passes_install();
  tex_queue_install();
  i = 0;
  do {
    target_resize(&stage_screen_targets[0][i],0x200,0x100);
    target_resize(&stage_screen_targets[1][i],0x200,0x100);
    i = i + 1;
  } while (i < 2);
  return;
}

void stage_screen_texture_bind(int bank, stage_screen_tex_rec *tex)
{
  target_texidx_create((int)tex->tex_index,(int)tex->width,
               (int)tex->height,tex->format);
  target_rebind_texture(&stage_screen_targets[bank][tex->target],
               (int)tex->tex_index);
  return;
}

void stage_screen_render(int bank, stage_screen_rec *screen)
{
  int cmp;
  uint flags;
  float mtx[16];

  flags = screen->flags;
  if (((screen->tex.flags & 1) != 0) && (((flags & 0x10) == 0 || (demo_stage_select == '\x01')))) {
    if (((flags & 1) != 0) && (stage_placed_objs_on == 0)) {
      stage_screen_stage_draw(screen->stage,flags);
    }
    target_draw_select(&stage_screen_targets[bank][screen->tex.target]);
    stage_screen_view_apply(screen);
    if ((flags & 1) != 0) {
      stage_screen_stage_draw(screen->stage,flags);
    }
    if ((flags & 0x100) != 0) {
      stage_screen_rob_draw(screen->robs[0],flags);
    }
    if ((flags & 0x10000) != 0) {
      stage_screen_rob_draw(screen->robs[1],flags);
    }
    cmp = memcmp("VIP STAGE",screen->stage->name,3);
    if (cmp == 0) {
      mtx_push(0);
      mtx_store_wordwise(mtx);
      mtx_reset_3x3();
      mtx_translate(0.0,0.0,0.01);
      mtx_set_rot_3x3_stride4(mtx);
      if ((flags & 0x100) != 0) {
        rob_draw_mode_set(0,1);
        stage_screen_rob_draw(screen->robs[0],flags);
      }
      if ((flags & 0x10000) != 0) {
        rob_draw_mode_set(1,1);
        stage_screen_rob_draw(screen->robs[1],flags);
      }
      mtx_pop(1);
    }
    target_draw_finish();
    stage_screen_after_run(screen);
  }
  return;
}

void stage_screen_view_apply(stage_screen_rec *screen)
{
  void (*f)(stage_screen_rec *) = screen->view;
  if (f != 0) {
    f(screen);
  }
  else {
    screen->rect[0] = 0.0;
    screen->rect[1] = 0.0;
    screen->rect[2] = (float)screen->tex.width;
    screen->rect[3] = (float)screen->tex.height;
    proj_map_to_rect(screen->rect[0],screen->rect[1],screen->rect[2],
                 screen->rect[3]);
  }
}

void stage_screen_after_run(stage_screen_rec *screen)
{
  if (screen->after != 0) {
    (*screen->after)(screen);
  }
  return;
}

void stage_screen_stage_draw(stage_rec *stage, uint flags)
{
  uint i;
  int *list;
  stage_obj_place *place;

  if ((flags & 8) != 0) {
    mtx_push_identity();
    mtx_scale_xyz(-1.0,1.0,-1.0);
    mtx_times_last_pushed();
  }
  if ((flags & 2) != 0) {
    for (i = 0; i < 0x10; i++) {
      light_sets[0][i].dir[1] = -light_sets[0][i].dir[1];
    }
  }
  light_set_send(0,0);
  if ((flags & 4) != 0) {
    place = (stage_obj_place *)stage->field_0x40;
    mtx_push(0);
    for (; 0 < place->obj; place++) {
      mtx_peek_pushed(0);
      mtx_translate(place->pos.x,place->pos.y,place->pos.z);
      obj_draw_culled(place->obj);
    }
    mtx_pop(1);
  }
  else {
    list = *stage->draw_lists;
    while (*list > 0) {
      obj_draw_culled(*list++);
    }
  }
  if ((flags & 2) != 0) {
    for (i = 0; i < 0x10; i++) {
      light_sets[0][i].dir[1] = -light_sets[0][i].dir[1];
    }
  }
  if ((flags & 8) != 0) {
    mtx_pop(1);
  }
}

void stage_screen_rob_draw(rob_rec *rob, uint flags)
{
  uint i;
  uint side_flags;

  if ((rob->flags & 1) == 0) return;
  if ((flags & 0x1000000) != 0) {
    stage_screen_lights_b_load();
  }
  if ((flags & 0x3000000) != 0) {
    stage_screen_lights_a_load();
  }
  side_flags = flags;
  if (rob->side == 0) {
    side_flags >>= 8;
  }
  else {
    side_flags >>= 0x10;
  }
  if ((side_flags & 2) != 0) {
    for (i = 0; i < 0x10; i++) {
      light_sets[1][i].dir[1] = -light_sets[1][i].dir[1];
    }
    mtx_push(0);
    mtx_scale_xyz(1.0,-1.0,1.0);
    if ((flags & 0x2000000) != 0) {
      mtx_translate(0.0,-25.2,0.0);
    }
    else {
      mtx_translate(0.0,-8.0,0.0);
    }
    hdr_or_top_bit(1);
  }
  light_set_send(0,1);
  if ((side_flags & 4) != 0) {
    rob_draw_mode_set((uint)rob->side,1);
  }
  rob_draw(rob);
  rob_item_hook_stub(rob);
  if ((side_flags & 4) != 0) {
    rob_draw_mode_set((uint)rob->side,0);
  }
  if ((side_flags & 2) != 0) {
    for (i = 0; i < 0x10; i++) {
      light_sets[1][i].dir[1] = -light_sets[1][i].dir[1];
    }
    mtx_pop(1);
    hdr_or_top_bit(0);
  }
}

void stage_screen_texture_release(stage_screen_tex_rec *tex)
{
  if ((tex->flags & 1) != 0) {
    target_texidx_release((int)tex->tex_index);
    tex->flags = tex->flags & 0xfffffffe;
  }
  return;
}

void stage_screen_lights_a_load(void)
{
  stage_screen_lights_load(stage_screen_lights_a);
  return;
}

void stage_screen_lights_b_load(void)
{
  stage_screen_lights_load(stage_screen_lights_b);
  return;
}

void stage_screen_lights_load(stage_screen_light_src *light)
{
  light_src_rec *dst;
  float v[3];

  mtx_push(0);
  mtx_transpose();
  for (dst = light_sets[1]; light->id != 0; light++, dst++) {
    dst->type = light->id;
    mtx_xform_dir(light->dir,v);
    vec3_normalize(v);
    dst->dir[0] = v[0];
    dst->dir[1] = v[1];
    dst->dir[2] = v[2];
    dst->field_0x2c = light->value[0];
    dst->color[0] = light->value[1];
    dst->color[1] = light->value[2];
    dst->color[2] = light->value[3];
  }
  mtx_pop(1);
  return;
}

void water_patches_draw(stage_rec *stage)
{
  water_state_rec *water;
  stage_obj_place *place;
  compiled_model_rec *model;

  place = (stage_obj_place *)stage->objs;
  water = &water_state;
  if ((place->obj != 0) && (((place->flags & 2) == 0 || (demo_stage_select == '\x01')))) {
    water_step(water);
    water_grid = water_grid_verts[0];
    water_grid_build(water);
    mtx_push(0);
    for (; place->obj != 0; place = place + 1) {
      model = 0;
      mtx_peek_pushed(0);
      mtx_translate_by_vec3(&place->pos.x);
      if (stage_obj_visible(place,&model) != 0) {
        ((void (*)(void))water_grid_project)();
        water_patch_submit((int)model);
      }
    }
    mtx_pop(1);
  }
  return;
}

void water_grid_build(water_state_rec *water)
{
  float *h;
  vec_t light;
  vec_t c1;
  vec_t c2;
  vtx_t (*grid)[17];
  uint col1;
  uint col2;
  int i;
  int j;
  int c;
  float *hp;
  vec_t n;
  float k;
  uint *d;
  uint *d2;
  uint (*colors)[17];
  uchar a;
  uchar r;
  uchar g;
  uchar b;

  h = water->grid_re[0];
  light = *(vec_t *)&water_grid_spare_vec; /* redundant, but game code: light is copied and negated below but never read; the game performs the copy and all three negations. */
  c1 = *(vec_t *)&water->color1_r;
  c2 = *(vec_t *)&water->color2_r;
  grid = (vtx_t (*)[17])water_grid;
  colors = water_grid_colors;
  light.x = -light.x;
  light.y = -light.y;
  light.z = -light.z;
  water_spacing = *(float *)&water->grid_spacing;
  a = (int)(water_alpha * 255.0f);
  r = (int)(c1.x * 255.0f);
  g = (int)(c1.y * 255.0f);
  b = (int)(c1.z * 255.0f);
  col1 = a << 24 | r << 16 | g << 8 | b;
  r = (int)(c2.x * 255.0f);
  g = (int)(c2.y * 255.0f);
  b = (int)(c2.z * 255.0f);
  col2 = r << 16 | g << 8 | b | 0xff000000;
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      hp = &h[i * 17 + j];
      n.x = hp[0] - hp[1];
      n.y = water_spacing;
      n.z = hp[0] - ((float (*)[17])h)[i + 1][j];
      k = fsrra_asm(n.x * n.x + n.y * n.y + n.z * n.z);
      n.x = n.x * k;
      n.y = k * n.y;
      n.z = k * n.z;
      grid[i][j].color = col1;
      colors[i][j] = col2;
      grid[i][j].h = hp[0];
      grid[i][j].n = n;
    }
    grid[i][16].h = grid[i][0].h;
    grid[i][16].n = grid[i][0].n;
    grid[i][16].color = grid[i][0].color;
    d = colors[i];
    d2 = d + 16;
    *d2 = *d;
  }
  for (c = 0; c < 17; c++) {
    grid[16][c].h = grid[0][c].h;
    grid[16][c].n = grid[0][c].n;
    grid[16][c].color = grid[0][c].color;
    colors[16][c] = colors[0][c];
  }
}

void water_strip_submit(uint *dst, uint *row, uint *colors, float row_pos)
{
  typedef struct { int w[2]; } int_pair;
  uint *p;
  uint *q;
  uint *a;
  uint *b;
  uint *c;
  uint *d;
  float y;
  int i;

  y = -water_spacing * 8.0;
  p = dst;
  a = row;
  b = colors;
  c = row + 0x77;
  d = colors + 0x11;
  p[7] = *b++;
  p[6] = a[6];
  *(int_pair *)(p + 4) = *(int_pair *)(a + 4);
  ((float *)p)[3] = row_pos;
  ((float *)p)[2] = *(float *)a;
  ((float *)p)[1] = y;
  p[0] = 0;
  q = p;
  p += 8;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  a += 7;
  p[7] = *d++;
  p[6] = c[6];
  *(int_pair *)(p + 4) = *(int_pair *)(c + 4);
  ((float *)p)[3] = water_spacing + row_pos;
  ((float *)p)[2] = *(float *)c;
  ((float *)p)[1] = y;
  p[0] = 0;
  q = p;
  p += 8;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  c += 7;
  y = y + water_spacing;
  p[7] = *b++;
  p[6] = a[6];
  *(int_pair *)(p + 4) = *(int_pair *)(a + 4);
  ((float *)p)[3] = row_pos;
  ((float *)p)[2] = *(float *)a;
  ((float *)p)[1] = y;
  p[0] = 0x60000000;
  q = p;
  p += 8;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  a += 7;
  p[7] = *d++;
  p[6] = c[6];
  *(int_pair *)(p + 4) = *(int_pair *)(c + 4);
  ((float *)p)[3] = water_spacing + row_pos;
  ((float *)p)[2] = *(float *)c;
  ((float *)p)[1] = y;
  p[0] = 0x20000000;
  q = p;
  p += 8;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  c += 7;
  y = y + water_spacing;
  for (i = 2; i < 0x10; i++) {
    p[7] = *b++;
    p[6] = a[6];
    *(int_pair *)(p + 4) = *(int_pair *)(a + 4);
    ((float *)p)[3] = row_pos;
    ((float *)p)[2] = *(float *)a;
    ((float *)p)[1] = y;
    p[0] = 0x20000000;
    q = p;
    p += 8;
    __asm__ __volatile__ ("pref @%0" : : "r" (q));
    a += 7;
    p[7] = *d++;
    p[6] = c[6];
    *(int_pair *)(p + 4) = *(int_pair *)(c + 4);
    ((float *)p)[3] = water_spacing + row_pos;
    ((float *)p)[2] = *(float *)c;
    ((float *)p)[1] = y;
    p[0] = 0x20000000;
    q = p;
    p += 8;
    __asm__ __volatile__ ("pref @%0" : : "r" (q));
    c += 7;
    y = y + water_spacing;
  }
  p[7] = *b;
  p[6] = a[6];
  *(int_pair *)(p + 4) = *(int_pair *)(a + 4);
  ((float *)p)[3] = row_pos;
  ((float *)p)[2] = *(float *)a;
  ((float *)p)[1] = y;
  p[0] = 0x20000000;
  q = p;
  p += 8;
  __asm__ __volatile__ ("pref @%0" : : "r" (q));
  a += 7;
  p[7] = *d;
  p[6] = c[6];
  *(int_pair *)(p + 4) = *(int_pair *)(c + 4);
  ((float *)p)[3] = water_spacing + row_pos;
  ((float *)p)[2] = *(float *)c;
  ((float *)p)[1] = y;
  p[0] = 0xa0000000;
  __asm__ __volatile__ ("pref @%0" : : "r" (p));
}

void water_patch_submit(int model)
{
  water_grid_vtx *row;
  int i;
  uint *colors;
  uint dst;
  float row_pos;

  row = water_grid;
  colors = water_grid_colors[0];
  if (model != 0) {
    model += 0xc0;
    { uint q = (uint)model >> 0x18 & 0x1c;
    *(volatile uint *)0xff000038 = q;
    *(volatile uint *)0xff00003c = q; }
    dst = model & 0x3ffffe0 | 0xe0000000;
    row_pos = -water_spacing * 8.0;
    for (i = 0; i < 0x10; i++) {
      __asm__ volatile ("pref @%0" : : "r" (colors + 0x11));
      water_strip_submit((uint *)dst,(uint *)row,colors,row_pos);
      row = row + 17;
      colors = colors + 0x11;
      dst = dst + 0x440;
      row_pos = row_pos + water_spacing;
    }
  }
}

void stage_screen_objs_project(stage_rec *stage)
{
  stage_obj_place *place;
  compiled_model_rec *model;

  place = (stage_obj_place *)stage->objs;
  if (place->obj != 0) {
    mtx_push(0);
    for (; place->obj != 0; place = place + 1) {
      mtx_peek_pushed(0);
      mtx_translate_by_vec3(&place->pos.x);
      if (stage_obj_visible(place,&model) != 0) {
        { int *q = (int *)((char *)model + 0x6c); *q = stage_screen_obj_word; }
        model_verts_project(model);
      }
    }
    mtx_pop(1);
  }
  return;
}

void stage_screen_rob_parts_project(rob_rec *rob)
{
  obj_node *node;
  compiled_model_rec *hand_model;
  obj_node **parts;
  uint i;
  uint count;
  float *mtx;

  mtx = rob->mtx;
  node = obj_node_get(rob->body_model);
  if (node != 0) {
    if (node->type == 0x13) {
      count = 0x7a;
    }
    else if (node->type == 0x15) {
      count = 0xa2;
    }
    else {
      return;
    }
    parts = (obj_node **)(node->bones + (int)node->file);
    node = *parts++;
    if ((node->type > 0) && (frame_parity != 0)) {
      model_verts_project_scaled(node->model2);
    }
    else {
      model_verts_project_scaled(node->model);
    }
    mtx = mtx + 0x10;
    mtx_push(0);
    for (i = 1; i < count; i++, mtx += 0x10, parts++) {
      node = *parts;
      if (node != 0) {
        mtx_peek_pushed(0);
        mtx_mul_by(mtx);
        __asm__ volatile ("pref @%0" : : "r" (mtx + 32));
        model_verts_project_scaled(node->model);
      }
    }
    mtx_pop(1);
    mtx = rob->mtx;
    mtx_push(0);
    mtx_mul_by(mtx + 0x70);
    mtx_scale_xyz(1.0,1.0,-1.0);
    {
    compiled_model_rec **t = rob_hand_models[0];
    hand_model = t[(uint)rob->side * 0x20 + (int)rob->hands[0].shape];
    model_verts_project_scaled(hand_model);
    mtx_peek_pushed(0);
    mtx_mul_by(mtx + 0xc0);
    model_verts_project_scaled(t[(uint)rob->side * 0x20 + (int)rob->hands[1].shape]);
    }
    mtx_pop(1);
  }
  return;
}

void screen_noise_apply(rob_rec *rob)
{
  compiled_model_rec *model;

  model = obj_model_get(rob_single_objects_stage18[rob->side]);
  if (model != 0) {
    noise_line_clear(screen_noise_lines);
    noise_line_jitter(&screen_noise_records[rob->side],screen_noise_lines);
    noise_roll_bar_add(&screen_noise_records[rob->side],screen_noise_lines);
    noise_spike_add(screen_noise_lines);
    screen_noise_uv_write(screen_noise_lines,model);
  }
  return;
}

void noise_line_clear(float *lines)
{
  uint i;
  for (i = 0; i < 0x101; i++) {
    lines[i] = 0.0f;
  }
}

void noise_line_jitter(screen_noise_rec *noise, float *lines)
{
  uint i;

  for (i = 0; i < 0x101; i++) {
    float *p = (float *)(i * 4 + (int)lines);
    *p = noise_jitter_scale * lcg_next_unit() + *p;
  }
}

void screen_noise_init(void)
{
  uint i;
  uint j;
  screen_noise_rec *o;
  (void)&i;
  i = 0;
  do {
    o = &screen_noise_records[i];
    o->level = lcg_next_unit() * 0.02;
    o->angle = (int)lcg_next15() % 180;
    for (j = 0; j < 1; j++) {
      screen_noise_bar *e = &o->bars[j];
      e->line = (int)(lcg_next_unit() * 257.0);
      e->speed = 5;
      e->c = 0.03;
      e->d = 0.02;
    }
    i++;
  } while (i < 2);
}

void noise_roll_bar_add(screen_noise_rec *noise, float *lines)
{
  screen_noise_bar *p;
  float *q;
  int k;
  int i;
  int j;
  int n;
  float a;
  float b;
  float c;

  for (k = 0; k < 1; k++) {
    p = &noise->bars[k];
    a = (lcg_next_unit() * 2.0 - 1.0) * noise_bar_speed_range + noise_bar_speed;
    i = 0;
    do {
      c = (lcg_next_unit() * 2.0 - 1.0) * noise_bar_accel_range + noise_bar_accel;
      b = a * (float)i + c * 0.5 * (float)i * (float)i;
      if (i != 0 && (b <= 0.0 || b >= 1.0)) break;
      p->offsets[i] = -b;
      i++;
    } while (i < 0x101);
    n = p->line;
    if (i + n < 0x101) {
      for (j = 0; j < i; j++, n++) {
        lines[n] += p->offsets[j];
      }
    } else {
      j = 0;
      for (; n < 0x101; n++) {
        lines[n] += p->offsets[j];
        j++;
      }
    }
    if (game_frame_count % p->speed == 0) {
      p->line = p->line + 1;
    }
    p->line = p->line % 0x101;
  }
}

void noise_spike_add(float *lines)
{
  int i;
  float f;

  if ((lcg_next15() & 7) == 0) {
    i = __fixunssfsi(lcg_next_unit() * 257.0f);
    f = lcg_next_unit();
    f = f + f - 1.0f;
    lines[i] = f * 0.1f;
  }
}

void screen_noise_uv_write(float *lines, compiled_model_rec *model)
{
  float lo, hi, y, d;
  uint i;
  char *q;
  if (noise_uv_inset != 0 && *(int *)(board_config + 0xc) == 0) {
    lo = 0.05;
    hi = 0.95;
  }
  else {
    lo = 0.0;
    hi = 1.0;
  }
  d = hi - lo;
  q = (char *)model + 0xc0;
  i = 0;
  y = hi;
  do {
    *(float *)(q + 0x10) = lines[i] + hi;
    *(float *)(q + 0x14) = y;
    q += 0x18;
    *(float *)(q + 0x10) = lines[i] + lo;
    *(float *)(q + 0x14) = y;
    q += 0x18;
    y -= d / 257.0;
    i++;
  } while (i < 0x101);
}

void empty_fn_0c0af9e0(void)
{
  return;
}

void stage_screen_callbacks_run(stage_screen_rec *screen)
{
  stage_screen_proj_factors(screen);
  camera_view_load();
  stage_screen_vert_scale_inv = 1.0 / stage_screen_vert_scale;
  if (screen->callbacks[0] != 0) {
    (*screen->callbacks[0])(screen->stage);
  }
  if (screen->callbacks[1] != 0) {
    (*screen->callbacks[1])(screen->robs[0]);
  }
  if (screen->callbacks[2] != 0) {
    (*screen->callbacks[2])(screen->robs[1]);
  }
  return;
}

void stage_screen_proj_factors(stage_screen_rec *screen)
{
  float m[16];
  float *c;
  if ((screen->tex.flags & 1) != 0) {
    proj_map_to_rect(screen->rect[0] / (float)(int)screen->tex.width,
                 screen->rect[1] / (float)(int)screen->tex.height,1.0,1.0);
    mtx_stack_select(3);
    mtx_load_identity();
    c = &camera.proj.near_clip;
    proj_from_vfov((int)((c[2] * 65536.0) / 360.0 + 0.5) & 0xffff,1.3333334,c[0],c[1]);
    proj_rebuild_rect_proj();
    mtx_stack_select(1);
    mtx_push_identity();
    mtx_premul_rect_proj(0.0,0.0);
    mtx_store_wordwise(m);
    mtx_pop(1);
    proj_map_to_rect(0.0,0.0,640.0,480.0);
    stage_screen_proj.x_scale = m[0] * stage_screen_view_scale;
    stage_screen_proj.y_scale = m[5] * stage_screen_view_scale;
    stage_screen_proj.x_shift = m[8];
    stage_screen_proj.y_shift = m[9];
  }
}

int stage_obj_visible(stage_obj_place *place, compiled_model_rec **model)
{
  compiled_model_rec *found;
  uint i;
  float v[3];
  float *t;
  float th;

  found = obj_model_get(place->obj);
  *model = found;
  if (found == 0) return 0;
  mtx_xform_point((*model)->sphere,v);
  th = (*model)->sphere[3];
  for (i = 0; i < 4; i++) {
    t = (float *)(&view_side_planes.field_0x0 + i * 0x10);
    if (th < t[0] * v[0] + t[1] * v[1] + t[2] * v[2]) {
      return 0;
    }
  }
  return -1;
}

void model_verts_project_scaled_a(compiled_model_rec *model)
{
  typedef struct { float x; float y; float z; } v3;
  typedef struct { float x; float y; } v2;
  v3 a;
  v3 b;
  int sum;
  int cnt;
  uint *p;
  uint *q;
  int c;
  uint w;
  uint u;
  float t;
  float inv;
  v2 s;
  
  if (model != 0) {
    u = (uint)model << 4;
    p = (uint *)(u >> 4) + 9;
    cnt = *p++;
    p += 14;
    sum = 0;
    while (sum < cnt) {
      if (*p == 0x08000500) {
        p += 16;
      }
      else {
        p += 7;
        c = *p++;
        sum += c;
        while (--c != -1) {
          q = p;
          w = *q++;
          b.x = (float)(char)w / 127.0;
          b.y = (float)(char)((int)w >> 8) / 127.0;
          b.z = (float)(char)((int)w >> 16) / 127.0;
          mtx_xform_dir(&b.x,&b.x);
          mtx_xform_point((float *)q,&a.x);
          p += 4;
          t = (1.0 - stage_screen_vert_scale_inv) * (a.x * b.x + a.y * b.y + a.z * b.z);
          b.x = b.x * t;
          b.y = b.y * t;
          b.z = b.z * t;
          a.x = a.x * stage_screen_vert_scale_inv + b.x;
          a.y = a.y * stage_screen_vert_scale_inv + b.y;
          a.z = a.z * stage_screen_vert_scale_inv + b.z;
          inv = 1.0 / -a.z;
          s.x = stage_screen_proj.x_scale * a.x;
          s.x += stage_screen_proj.x_shift * a.z;
          s.y = stage_screen_proj.y_scale * a.y;
          s.y += stage_screen_proj.y_shift * a.z;
          ((float *)p)[0] = s.x * inv;
          ((float *)p)[1] = inv * s.y;
          p += 2;
        }
      }
    }
  }
}

void model_verts_project_scaled_b(compiled_model_rec *model)
{
  float v[3];
  float n[3];
  int count;
  int total;
  int *p;
  int cnt;
  int w;
  float d;
  float inv;
  typedef struct { float x; float y; } scr2;
  scr2 s;

  if (model == 0) return;
  {
    uint a = (uint)model << 4;
    p = (int *)((a >> 4) + 0x24);
  }
  total = *p++;
  p += 14;
  count = 0;
  if (count >= total) return;
  do {
    if (*p == 0x08000500) {
      p += 16;
    } else {
      p += 7;
      cnt = *p++;
      count += cnt;
      for (cnt--; cnt != -1; cnt--) {
        w = *p;
        n[0] = (float)(char)w / 127.0f;
        n[1] = (float)(char)(w >> 8) / 127.0f;
        n[2] = (float)(char)(w >> 16) / 127.0f;
        mtx_xform_dir(n, n);
        mtx_xform_point((float *)(p + 1), v);
        p += 4;
        d = (1.0f - stage_screen_vert_scale_inv) * (v[0] * n[0] + v[1] * n[1] + v[2] * n[2]);
        v[0] = v[0] * stage_screen_vert_scale_inv + n[0] * d;
        v[1] = stage_screen_vert_scale_inv * v[1] + n[1] * d;
        v[2] = stage_screen_vert_scale_inv * v[2] + d * n[2];
        inv = 1.0f / -v[2];
        s.x = stage_screen_proj.x_scale * v[0];
        s.x += stage_screen_proj.x_shift * v[2];
        s.y = stage_screen_proj.y_scale * v[1];
        s.y += stage_screen_proj.y_shift * v[2];
        *(float *)p = s.x * inv;
        *(float *)(p + 1) = inv * s.y;
        p += 4;
      }
    }
  } while (count < total);
}

void stage_screen_view_shifted(stage_screen_rec *screen)
{
  screen->rect[0] = 0.0;
  screen->rect[1] = -64.0;
  screen->rect[2] = (float)screen->tex.width;
  screen->rect[3] = (float)screen->tex.height;
  proj_map_to_rect(screen->rect[0],screen->rect[1],screen->rect[2],
               screen->rect[3]);
  mtx_stack_select(3);
  proj_cmds_send();
  mtx_stack_select(1);
}

void water_view_scale(void)
{
  float mtx[16];

  mtx_store_wordwise(mtx);
  mtx_load_identity();
  mtx_scale_xyz(stage_screen_view_scale,stage_screen_view_scale,1.0);
  mtx_mul_by(mtx);
  return;
}

void stage_screen_rob0_view(stage_screen_rec *screen)
{
  rob_rec *rob;

  rob = rob_get(0);
  stage_screen_rob_view(screen,rob);
  return;
}

void stage_screen_rob1_view(stage_screen_rec *screen)
{
  rob_rec *rob;

  rob = rob_get(1);
  stage_screen_rob_view(screen,rob);
  return;
}

void stage_screen_rob_view(stage_screen_rec *screen, rob_rec *rob)
{
  float v[3];
  float o[4];
  float m[16];
  float r;
  float sx;
  float sy;
  float w;
  float h;
  float hw;
  float hh;
  target_background_words_set(0,0,0);
  mtx_push(0);
  mtx_xform_point(&rob->pos.x,v);
  mtx_load_identity();
  mtx_premul_rect_proj(0.0,0.0);
  mtx_store_wordwise(m);
  mtx_xform_point_xyzw(v,o);
  r = 1.0 / o[3];
  o[0] = o[0] * r;
  o[1] = o[1] * r;
  mtx_pop(1);
  sx = m[0] * 3.2 * r;
  sy = m[5] * 3.2 * r;
  w = (float)screen->tex.width;
  h = (float)screen->tex.height;
  hw = w * 0.5;
  hh = h * 0.5;
  screen_map_cmds_queue(w / sx,h / sy,(o[0] - hw) / hw,(o[1] - hh) / hh);
  mtx_stack_select(1);
  rob_draw_mode_set((uint)rob->side,0);
}

void stage_screen_rob0_done(stage_screen_rec *screen)
{
  rob_rec *rob;

  rob = rob_get(0);
  stage_screen_rob_done(screen,rob);
  return;
}

void stage_screen_rob1_done(stage_screen_rec *screen)
{
  rob_rec *rob;

  rob = rob_get(1);
  stage_screen_rob_done(screen,rob);
  return;
}

void stage_screen_rob_done(stage_screen_rec *screen, rob_rec *rob)
{
  rob_draw_mode_set((uint)rob->side,2);
  return;
}

void stage_screen_view_full(stage_screen_rec *screen)
{
  stage_obj_place *place;
  place = (stage_obj_place *)screen->stage->field_0x40;
  screen->rect[0] = 0.0;
  screen->rect[1] = 0.0;
  screen->rect[2] = (float)(int)screen->tex.width;
  screen->rect[3] = (float)(int)screen->tex.height;
  proj_map_to_rect(screen->rect[0],screen->rect[1],screen->rect[2],screen->rect[3]);
  (place++)->pos.z = dural_stage_scroll_get(0);
  place->pos.z = dural_stage_scroll_get(1);
  return;
}
