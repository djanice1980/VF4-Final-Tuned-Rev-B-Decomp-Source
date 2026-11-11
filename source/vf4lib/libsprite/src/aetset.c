/*
 * vf4lib/libsprite/src/aetset.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern sprite_set sprite_sets[81]; /* 0x0c322154 */
extern int aet_tex_vram_free; /* 0x0c322664 */
extern static_heap_rec aet_buf; /* 0x0c3600f4 */
extern char sprite_set_own_buffer[81]; /* 0x0c3b6464 */
extern aet_set_rec aet_sets[30]; /* 0x0c4ecc04 */
extern int aet_tex_list[128]; /* 0x0c4edecc */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */
extern char sprite_set_loaded[81]; /* 0x0c3b63dc */
extern aet_stack_rec aet_stack; /* 0x0c4edce4 */
extern aet_file_list aet_file_table; /* 0x0c1e2794 */
extern aet_file_table_hdr_rec aet_file_table_hdr; /* 0x0c4ecbfc */

void aet_globals_init(void)
{
  aet_file_table_hdr.count = 4;
  aet_file_table_hdr.list = &aet_file_table;
  strip_state_clear();
  aet_tex_vram_free = 0;
  return;
}

void aet_init(void)
{
  int i;
  
  aet_globals_init();
  i = 0;
  do {
    aet_set_clear(i);
    i = i + 1;
  } while (i < 0x1e);
  aet_stack_init();
  aet_tex_list_clear();
  return;
}

int aet_set_load(int set_no)
{
  aet_set_rec *set;
  aet_set_info *info;
  aet_stack_entry *entry;

  set = aet_set_get(set_no);
  if (set == 0) return 0;
  info = &set->info;
  if (info->loaded == 0) {
    strcpy(set->name, aet_file_table.rows[set_no].name);
    entry = aet_stack_push(set_no, aet_file_size(set->name));
    load_file(set->name, (void *)entry->addr, entry->size);
    info->stack_entry = entry;
    info->file = (aet_file_header *)entry->addr;
    aet_file_relocate(info->file);
    info->state = 4;
    info->loaded = 1;
  }
  return 1;
}

int aet_scene_textures_load(int scene_id)
{
  int *spr_set_list;
  int *tex_table;
  aet_scene *scene;
  int k;
  int i;
  int j;
  aet_image *image;
  aet_picture *picture;
  char *spr_set_no;
  int tex_no;
  sprite_set *spr_set;
  int loaded;
  char *own_buffer;

  spr_set_list = aet_scene_sprite_sets_get(scene_id);
  tex_table = aet_scene_tex_table_get(scene_id);
  scene = aet_scene_get(scene_id);
  if ((spr_set_list == 0 || tex_table == 0) || (scene == 0)) return 0;
  aet_tex_vram_free = tex_vram_free_total();
  for (k = 0; spr_set_list[k] != -1; k++) {
    spr_set_no = (char *)spr_set_list[k];
    spr_set = &sprite_sets[(int)spr_set_no];
    loaded = (int)sprite_set_loaded;
    if (spr_set_no[loaded] == 0 && (own_buffer = sprite_set_own_buffer, own_buffer[(int)spr_set_no] == 0)) {
      load_file(spr_set->name,heap_texdata.addr,heap_texdata.size);
      loaded = (int)sprite_set_loaded;
      spr_set_no[loaded] = 1;
    }
    for (i = 0; i < scene->image_count; i++) {
      image = &scene->images[i];
      picture = image->pictures;
      for (j = 0; j < image->picture_count; j++) {
        tex_no = tex_table[picture->tex_index];
        if (aet_tex_list_add(tex_no) != 0) {
          if (tex_no > 11999) {
            fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aetset.c",0x152,
                     "assertion `%s\' failed.\n","tex_no < MAX_DEF_TEX");
          }
          if (strip_texidx_ref(tex_no,spr_set->sprites + (tex_no - spr_set->tex_base) * 0x10) != 0) {
            fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aetset.c",0x155,
                     "ERROR!: Can\'t load strip texture. aut_id=%d, tex_no=%d",scene_id,tex_no);
          }
        }
      }
    }
  }
  aet_tex_vram_free = tex_vram_free_total();
  return 1;
}

int aet_scene_textures_free(int scene_id)
{
  int *spr_set_list;
  int *tex_table;
  aet_scene *scene;
  int k;
  int i;
  int j;
  aet_image *image;
  aet_picture *picture;
  int tex_no;

  aet_tex_vram_free = tex_vram_free_total();
  spr_set_list = aet_scene_sprite_sets_get(scene_id);
  tex_table = aet_scene_tex_table_get(scene_id);
  scene = aet_scene_get(scene_id);
  if ((spr_set_list == 0 || tex_table == 0) || (scene == 0)) return 0;
  for (k = 0; spr_set_list[k] != -1; k++) {
    for (i = 0; i < scene->image_count; i++) {
      image = &scene->images[i];
      picture = image->pictures;
      for (j = 0; j < image->picture_count; j++) {
        tex_no = tex_table[picture->tex_index];
        if (aet_tex_list_remove(tex_no) != 0) {
          strip_texidx_unref(tex_no);
        }
      }
    }
    { char *loaded = sprite_set_loaded; loaded[spr_set_list[k]] = 0; }
  }
  aet_tex_vram_free = tex_vram_free_total();
  return 1;
}

int aet_set_free(int set_no)
{
  aet_set_rec *set;
  aet_stack_entry *entry;
  aet_set_info *info;

  set = aet_set_get(set_no);
  if (set == 0) {
    return 0;
  }
  entry = aet_stack_pop();
  info = &set->info;
  if (info->stack_entry != entry) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aetset.c",0x1c2,
             "Error!: AET illigal stack-memory free order.");
  }
  aet_set_clear(set_no);
  return 1;
}

void aet_all_free(void)
{
  int tex_no;
  int i;
  
  i = 0;
  do {
    aet_set_clear(i);
    i = i + 1;
  } while (i < 0x1e);
  aet_stack_init();
  i = 0;
  do {
    tex_no = aet_tex_list[i];
    if ((tex_no != -1) && (tex_no = aet_tex_list_remove(tex_no), tex_no != 0)) {
      strip_texidx_unref(aet_tex_list[i]);
    }
    i = i + 1;
  } while (i < 0x80);
  return;
}

aet_scene * aet_scene_get(uint scene_id)
{
  aet_set_rec *set;
  aet_set_info *info;
  uint scene_no = scene_id & 0xffff;
  uint set_no = scene_id >> 0x10 & 0xffff;

  set = aet_set_get(set_no);
  if (set == 0) return 0;
  info = &set->info;
  if (info->state != 4) return 0;
  if (info->file == 0) return 0;
  return info->file->scenes[scene_no];
}

char * aet_scene_table2_get(uint scene_id)
{
  uint scene_no = scene_id & 0xffff;
  aet_set_rec *set;
  aet_set_info *info;
  set = aet_set_get(scene_id >> 0x10 & 0xffff);
  if (set == 0) return 0;
  info = &set->info;
  return info->file->table2[scene_no];
}

int * aet_scene_sprite_sets_get(uint scene_id)
{
  aet_set_rec *set;
  aet_set_info *info;
  uint mask = 0xffff;
  uint scene_no = scene_id & mask;
  int *list;
  set = aet_set_get((scene_id >> 16) & mask);
  if (set == 0) list = 0;
  else {
    info = &set->info;
    if (info->file == 0) list = 0;
    else {
      list = info->file->scene_tex[scene_no].sprite_sets;
    }
  }
  return list;
}

int * aet_scene_tex_table_get(uint scene_id)
{
  uint scene_no = scene_id & 0xffff;
  aet_set_rec *set;
  aet_set_info *info;
  set = aet_set_get(scene_id >> 0x10 & 0xffff);
  if (set == 0) return 0;
  info = &set->info;
  if (info->file == 0) return 0;
  return info->file->scene_tex[scene_no].tex_table;
}

char * aet_file_label_get(int row)
{
  aet_file_list *list = aet_file_table_hdr.list;
  aet_file_row *entry = &list->rows[row];

  return entry->label;
}

int aet_set_scene_count(int set_no)
{
  aet_set_rec *set;
  aet_set_info *info;

  set = aet_set_get(set_no);
  if (set == 0) return 0;
  info = &set->info;
  return *info->file->scene_count;
}

aet_set_rec * aet_set_get(uint set_no)
{
  if (!(set_no < 0x1e)) {
    return 0;
  }
  return &aet_sets[set_no];
}

void aet_set_clear(int set_no)
{
  aet_set_rec *set;
  aet_set_info *info;

  set = aet_set_get(set_no);
  if (set != 0) {
    info = &set->info;
    info->state = 0;
    info->loaded = 0;
    info->file = 0;
  }
}

void aet_ptr_array_relocate(int *ptrs, int base, int count)
{
  int i;
  int *p = ptrs;
  for (i = 0; i < count; i++) {
    if (*p != 0) {
      aet_ptr_relocate(p,base);
      p++;
    }
  }
}

void aet_file_relocate(aet_file_header *file)
{
  int i;
  int n;
  aet_file_header *header = file;

  aet_ptr_relocate((int *)&file->scene_count,(int)file);
  aet_ptr_relocate((int *)&file->scenes,(int)file);
  aet_ptr_relocate((int *)&file->table2,(int)file);
  aet_ptr_relocate((int *)&file->scene_tex,(int)file);
  n = *file->scene_count;
  aet_ptr_array_relocate((int *)file->scenes,(int)file,n);
  aet_ptr_array_relocate((int *)file->table2,(int)file,n);
  aet_ptr_array_relocate((int *)file->scene_tex,(int)file,n * 2);
  for (i = 0; i < n; i++) {
    int scenes = (int)header->scenes; int offset = i * 4; aet_scene_relocate(*(aet_scene **)(scenes + offset),(int)file);
  }
}

void aet_ptr_relocate(int *ptr, int base)
{
  if (*ptr != 0) {
    *ptr = *ptr + base;
  }
  return;
}

int aet_tex_vram_free_get(void)
{
  return aet_tex_vram_free;
}

void aet_stack_init(void)
{
  int i;

  aet_stack.n = 0;
  aet_stack.top = (int)aet_buf.addr;
  i = 0;
  do {
    aet_stack_entry_clear(&aet_stack.e[i]);
    i = i + 1;
  } while (i < 0x1e);
  return;
}

aet_stack_entry * aet_stack_push(int set_no, int size)
{
  aet_stack_entry *entry;
  int top;
  if (aet_stack.n > 29) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aetset.c",0x370,"Error!: AET statck num over.");
  }
  top = aet_stack.top;
  top += size;
  if (top >= (int)aet_buf.addr + 0xccccc) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aetset.c",0x375,"Error!: AET statck memory over.");
  }
  entry = &aet_stack.e[aet_stack.n];
  entry->used = 1;
  entry->set = set_no;
  entry->addr = aet_stack.top;
  entry->size = size;
  aet_stack.top = top;
  aet_stack.n++;
  return entry;
}

aet_stack_entry * aet_stack_pop(void)
{
  int last;
  aet_stack_entry *entry;
  int addr;

  last = aet_stack.n + -1;
  if (last < 0) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aetset.c",0x393,"Error!: AET stack memory pop."
            );
  }
  entry = &aet_stack.e[last];
  addr = entry->addr;
  aet_stack_entry_clear(entry);
  aet_stack.top = addr;
  aet_stack.n = aet_stack.n + -1;
  return entry;
}

void aet_stack_entry_clear(aet_stack_entry *entry)
{
  entry->used = 0;
  entry->set = 0;
  entry->addr = 0;
  entry->size = 0;
  return;
}

uint aet_file_size(char *name)
{
  dimm_file *fp;
  uint size;
  
  fp = dimm_fopen(name,"rb");
  size = 0;
  if (fp == (dimm_file *)0x0) return size;
  size = fp->size;
  dimm_fclose(fp);
  return size;
}

void aet_tex_list_clear(void)
{
  int i;
  
  i = 0;
  do {
    aet_tex_list[i] = 0xffffffff;
    i = i + 1;
  } while (i < 0x80);
  return;
}

int aet_tex_list_add(int tex_no)
{
  int i;
  
  i = 0;
  do {
    if (aet_tex_list[i] == tex_no) {
      return 0;
    }
    i = i + 1;
  } while (i < 0x80);
  i = 0;
  do {
    if (aet_tex_list[i] == -1) break;
    i = i + 1;
  } while (i < 0x80);
  if (0x7f < i) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aetset.c",0x3e6,
             "Error!: AET load tex max over.");
  }
  aet_tex_list[i] = tex_no;
  return 1;
}

int aet_tex_list_remove(int tex_no)
{
  int i;
  
  i = 0;
  do {
    if (aet_tex_list[i] == tex_no) {
      aet_tex_list[i] = -1;
      return 1;
    }
    i = i + 1;
  } while (i < 0x80);
  return 0;
}
