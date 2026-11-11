/*
 * vf4lib/libpoly/src/zanzou.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern float screen_line_width; /* 0x0c322778 */
extern float proj_rect_px_width; /* 0x0c32a6fc */
extern float proj_rect_px_height; /* 0x0c32a700 */
extern void * zanzou_elan_block; /* 0x0c3d22b4 */
extern void * zanzou_ram_block; /* 0x0c3d22b8 */
extern float zanzou_proj_saved[16]; /* 0x0c55557c */

static inline float fsqrt_asm(float x)
{
  __asm__ ("fsqrt %0" : "=f" (x) : "0" (x));
  return x;
}


void screen_line_draw(float x0, float y0, float x1, float y1, float depth, uint color)
{
  struct { float x; float y; float w; uint c; } v[4];
  float dx;
  float dy;
  float s;
  float l;
  float h;
  depth = 1.0 / depth;
  dx = x0 - x1;
  dy = y0 - y1;
  l = dx * dx + dy * dy;
  h = screen_line_width * 0.5;
  s = (1.0 / fsqrt_asm(l)) * h;
  dx = dx * s;
  dy = dy * s;
  v[0].x = x0 + dy + dx;
  v[0].y = y0 - dx + dy;
  v[0].w = depth;
  v[1].x = x0 - dy + dx;
  v[1].y = y0 + dx + dy;
  v[1].w = depth;
  v[2].x = x1 + dy - dx;
  v[2].y = y1 - dx - dy;
  v[2].w = depth;
  v[3].x = x1 - dy - dx;
  v[3].y = y1 + dx - dy;
  v[3].w = depth;
  v[0].c = color;
  v[1].c = color;
  v[2].c = color;
  v[3].c = color;
  polystrip_plain_opaque(4, (uint *)v);
}

void screen_hline_draw(uint color, float x0, float x1, float y, float depth)
{
  struct { float x; float y; float z; uint c; } v[4];
  float t;
  float y2;
  float x2;

  if (x0 > x1) {
    t = x0;
    x0 = x1;
    x1 = t;
  }
  depth = 1.0 / depth;
  v[0].x = x0;
  v[0].y = y;
  v[0].z = depth;
  v[1].x = x0;
  y2 = y + screen_line_width;
  v[1].y = y2;
  v[1].z = depth;
  x2 = x1 + 1.0;
  v[2].x = x2;
  v[2].y = y;
  v[2].z = depth;
  v[3].x = x2;
  v[3].y = y2;
  v[3].z = depth;
  v[0].c = color;
  v[1].c = color;
  v[2].c = color;
  v[3].c = color;
  polystrip_plain_opaque(4,(uint *)v);
}

void screen_vline_draw(uint color, float x, float y0, float y1, float depth)
{
  struct { float x; float y; float z; uint c; } v[4];

  if (y1 < y0) {
    y0 = y1;
  }
  depth = 1.0 / depth;
  v[0].x = x;
  v[0].y = y0;
  v[0].z = depth;
  v[1].x = x;
  v[1].y = y1 + 1.0;
  v[1].z = depth;
  v[2].x = screen_line_width + x;
  v[2].y = y0;
  v[2].z = depth;
  v[3].x = screen_line_width + x;
  v[3].y = y1 + 1.0;
  v[3].z = depth;
  v[0].c = color;
  v[1].c = color;
  v[2].c = color;
  v[3].c = color;
  polystrip_plain_opaque(4,(uint *)v);
  return;
}

void screen_line_width_set(float width)
{
  screen_line_width = width;
  return;
}

void text_cursor_put(int x, int y)
{
  text_margin_x = x;
  text_cursor_x = x;
  text_cursor_y = y;
  return;
}

void zanzou_heaps_create(void)
{
  zanzou_ram_block = malloc(0x20000);
  if (zanzou_ram_block == (void *)0x0) {
    __asm__ volatile ("trapa #0x50");
  }
  zanzou_elan_block = elan_heap_alloc(0x20000);
  if (zanzou_elan_block == (void *)0x0) {
    __asm__ volatile ("trapa #0x50");
  }
  zanzou_heaps_reset();
  return;
}

void zanzou_heaps_reset(void)
{
  heap_init(&zanzou_ram_heap,zanzou_ram_block,0x20000);
  heap_init(&zanzou_elan_heap,zanzou_elan_block,0x20000);
  return;
}

void * zanzou_ram_alloc(int size)
{
  void *p;
  if (size == 0) return 0;
  p = heap_malloc(&zanzou_ram_heap,size);
  if (p == (void *)0x0) {
    __asm__ volatile ("trapa #0x50");
  }
  heap_stats_get(&zanzou_ram_heap,&zanzou_ram_heap_stats);
  return p;
}

void * zanzou_elan_alloc(int size)
{
  void *p;
  if (size == 0) return 0;
  p = heap_malloc(&zanzou_elan_heap,size);
  if (p == (void *)0x0) {
    __asm__ volatile ("trapa #0x50");
  }
  heap_stats_get(&zanzou_elan_heap,&zanzou_elan_heap_stats);
  return p;
}

void zanzou_trail_init(zanzou_trail_rec *trail, uint obj_id, compiled_model_rec *model_buf, void *hist_buf)
{
  vec3 neg_z = {0.0, 0.0, -1.0};
  vec3 pos_z = {0.0, 0.0, 1.0};
  int cnt;
  compiled_model_rec *buf;
  compiled_model_rec *second;
  obj_node *node;
  char *file;
  uint size;
  int i;
  vec3 *p;

  trail->hist = 0;
  node = obj_node_get(obj_id);
  if (node != 0 && node->type == 0xf) {
    file = (char *)node->file;
    cnt = *(int *)(file + (int)node->list_80 + 4);
    size = model_size((int *)node->model) + 0x1fU & 0xffffffe0;
    if (model_buf != 0) {
      buf = model_buf;
    }
    else {
      buf = zanzou_elan_alloc(size * 2);
    }
    second = (compiled_model_rec *)((char *)buf + size);
    memcpy(buf,node->model,size);
    memcpy(second,node->model,size);
    if (hist_buf != 0) {
      p = (vec3 *)hist_buf;
    }
    else {
      p = zanzou_ram_alloc(cnt * 0x60);
    }
    trail->hist = (zanzou_trail_entry *)p;
    trail->on = 0;
    trail->cur = cnt - 1;
    trail->count = cnt;
    trail->value = 1;
    trail->field_0xc = 0.1;
    trail->field_0x10 = 1.0;
    trail->node = node;
    trail->model[0] = buf;
    trail->model[1] = second;
    for (i = 0; i < cnt * 2; i++, p += 4) {
      p[0] = neg_z;
      p[1] = pos_z;
      p[2] = neg_z;
      p[3] = pos_z;
    }
  }
}

void zanzou_trail_free(zanzou_trail_rec *trail)
{
  elan_heap_free(trail->model[0]);
  free(trail->hist);
  trail->model[0] = 0;
  trail->model[1] = 0;
  trail->hist = 0;
  return;
}

void zanzou_trail_cur_shift(zanzou_trail_rec *trail, int steps)
{
  short *cur;
  int sum;

  cur = &trail->cur;
  sum = *cur + steps;
  *cur = sum % trail->count;
}

void zanzou_trail_make(zanzou_trail_rec *trail)
{
  obj_node *node = trail->node;
  int hist = (int)trail->hist;
  int *list;
  if (hist != 0 && trail->on != 0) {
    list = (int *)((int)node->list_80 + (int)node->file);
    zanzou_vertices_write(trail->model[frame_parity], (int *)(list[6] + (int)node->file), trail->cur * 0x30 + hist);
  }
}

void zanzou_trail_restart(zanzou_trail_rec *trail, vec3 *edge_a, vec3 *edge_b, int value)
{
  zanzou_trail_entry *e4;
  zanzou_trail_entry *e3;
  int i;
  e4 = trail->hist;
  e3 = (zanzou_trail_entry *)(trail->count * 48 + (int)e4);
  if (e4 != 0 && trail->value < trail->count) {
    trail->value = value;
    trail->on = 1;
    trail->cur = trail->count - 1;
    for (i = 0; i < trail->count; i++, e4++, e3++) {
      e4->edge_a = e3->edge_a = *edge_a;
      e4->edge_b = e3->edge_b = *edge_b;
    }
  }
}

void zanzou_trail_push(zanzou_trail_rec *trail, vec3 *edge_a, vec3 *edge_b)
{
  short *cur;
  zanzou_trail_entry *e4;
  zanzou_trail_entry *e3;

  if (trail->hist != 0) {
    cur = &trail->cur;
    if (--*cur < 0) {
      *cur = trail->count - 1;
    }
    e4 = trail->hist + trail->cur;
    e3 = e4 + trail->count;
    e4->edge_a = e3->edge_a = *edge_a;
    e4->edge_b = e3->edge_b = *edge_b;
    trail->edge_a_z_prev = trail->edge_a_z;
    trail->edge_a_z = edge_a->z;
  }
  return;
}

void zanzou_trail_off(zanzou_trail_rec *trail)
{
  trail->on = 0;
  return;
}

void zanzou_trail_draw(zanzou_trail_rec *trail)
{
  if (trail->on != 0) {
    model_draw(trail->model[frame_parity]);
  }
  return;
}

void zanzou_proj_begin(void)
{
  mtx_push(0);
  mtx_stack_select(3);
  mtx_store_wordwise(zanzou_proj_saved);
  mtx_load_identity();
  proj_from_vfov(0x4000,1.3333334,0.01,1000.0);
  proj_cmds_send();
  mtx_stack_select(1);
  mtx_load_identity();
  return;
}

void zanzou_proj_end(void)
{
  mtx_stack_select(3);
  mtx_set_current_wordwise(zanzou_proj_saved);
  proj_cmds_send();
  mtx_stack_select(1);
  mtx_pop(1);
  return;
}

void zanzou_point_project(float *point, float *out)
{
  float v[4];
  float r;

  mtx_xform_point_xyzw(point,v);
  r = 1.0 / v[3];
  *out = (r * v[0] * proj_rect_px_width) / proj_rect_px_height;
  out[1] = r * v[1];
  out[2] = v[3];
}
