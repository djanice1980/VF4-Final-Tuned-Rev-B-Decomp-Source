/*
 * vf4lib/libdebug/src/chara_load.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

/* unnamed globals: their types when this file matched (tools/match/pin_globals.py) */
extern char *obj_short_names[241]; /* 0x0c293950 */
extern char **texcol_names; /* 0x0c3253ac */
extern int texcol_reload_pending; /* 0x0c3253d4 */
extern int texcol_part; /* 0x0c3253d8 */
extern static_heap_rec rob_obj_ofs[3]; /* 0x0c3600ac */
extern static_heap_rec newopt_obj_ofs[2]; /* 0x0c360148 */
extern static_heap_rec objmem_heap_chr[3]; /* 0x0c3601bc */
extern objbank objbanks[21]; /* 0x0c3b64bc */
extern char texcol_part_name[64]; /* 0x0c3d8090 */
extern txn_entry part_txn_lists[4][32]; /* 0x0c3d80d0 */
extern txn_entry body_txn_merged[96]; /* 0x0c55d628 */
extern static_heap_rec heap_texdata; /* 0x0c3600d0 */

void bank_name_upper(int bank)
{
  char *p;
  int c;
  strcpy(texcol_part_name,obj_short_names[objbanks[bank].set]);
  for (p = texcol_part_name; *p != '\0'; p++) {
    c = *p;
    if ((ctype_table + 1)[c] & 2) c -= 0x20;
    *p = c;
  }
}

void rob_color_menu0_draw(void)
{
  rob_color_menu_text(0);
  return;
}

void rob_color_menu1_update(void)
{
  rob_color_menu_text(1);
  return;
}

void rob_color_menu2_update(void)
{
  rob_color_menu_text(2);
  return;
}

void rob_color_menu3_update(void)
{
  rob_color_menu_text(3);
  return;
}

void rob_color_menu_text(int menu)
{
  uchar side;
  rob_rec *rob;
  char *costumes;
  int busy;

  rob = rob_get(0);
  if ((texcol_reload_pending == 1) && (busy = poly_queue_busy_get(), busy == 0)) {
    side = rob->side;
    costumes = *(char **)((char *)chara_data_get(rob->chara) + 0x48);
    rob_hand_models_build(side,*(uint *)(rob->costume * 0x20 + costumes + 0xc));
    rob_disp_setup(rob);
    texcol_reload_pending = busy;
  }
  texcol_names = (char **)objbanks[5].tex_names;
  bank_name_upper(5);
  texcol_entries_to_text(texcol_body_lists[menu],texcol_body_text[menu],
               &texcol_body_text_len[menu],texcol_part_name);
}

void atam_color_menu_focus_update(void)
{
  texcol_part = 0;
  part_color_menu_text(0xd,0);
  return;
}

void face_color_menu_focus_update(void)
{
  texcol_part = 1;
  part_color_menu_text(0xf,1);
  return;
}

void joha_color_menu_focus_update(void)
{
  texcol_part = 2;
  part_color_menu_text(0x11,2);
  return;
}

void kaha_color_menu_focus_update(void)
{
  texcol_part = 3;
  part_color_menu_text(0x13,3);
  return;
}

void part_color_menu_text(int bank, int part)
{
  texcol_names = texcol_part_names = (char **)objbanks[bank].tex_names;
  bank_name_upper(bank);
  texcol_entries_to_text(part_txn_lists[part],texcol_part_text[part], &texcol_part_text_len[part],texcol_part_name);
}

void part_color_text_clear(void)
{
  memset(texcol_part_text,0,0x4000);
  return;
}

void part_color_text_load(int part)
{
  texcol_text_to_entries(part_txn_lists[part],texcol_part_text[part],texcol_part_name);
  return;
}

void debug_load_texcol_name(void)
{
  window_trap();
  part_color_text_load(texcol_part);
  return;
}

void texcol_entries_to_text(txn_entry *entries, char *text, int *length, char *name)
{
  char *start = text;
  int i;

  text += sprintf(text,&nam_state_log_format[4],name);
  for (i = 0; i < 0x20; entries++, i++) {
    if (entries->tex_no == -2) break;
    if (texcol_names != (char **)0x0 && entries->tex_no >= 0) {
      if (entries->swap_tex_no == -1) {
        memcpy(texcol_swap_name,null_text,5);
      }
      else {
        sprintf(texcol_swap_name,"txn_%s_%s",name,
                     texcol_names[entries->swap_tex_no]);
      }
      text += sprintf(text,"txn_%s_%s %d %d %d %s %d %d %d %d %d %d %d %d\n",name,
                           texcol_names[entries->tex_no],entries->brightness,entries->saturation,entries->hue,texcol_swap_name,
                           entries->field_0x8,entries->color_a[0],entries->color_a[1],entries->color_a[2],entries->color_on,entries->color_b[0],entries->color_b[1],entries->color_b[2]);
    }
  }
  *text = 0;
  *length = text - start;
}

int texcol_name_lookup(char *token, char *prefix, char **names)
{
  int result = -2;
  int i;
  if (memcmp(token,null_text,5) == 0) {
    result = -1;
  }
  else if (strncmp(token,prefix,strlen(prefix)) == 0) {
    i = 0;
    token += strlen(prefix);
    for (; *names != 0; names++, i++) {
      if (strcmp(*names,token) == 0) break;
    }
    if (*names != 0) result = i;
  }
  return result;
}

void texcol_text_to_entries(txn_entry *entries, char *text, char *name)
{
  char prefix[64];
  char tex_name[64];
  char swap_name[64];
  int color_lo[4];
  int color_hi[4];
  int saturation;
  int hue;
  int brightness;
  int i;

  i = 0;
  snprintf(prefix,0x40,"txn_%s_",name);
  if (strncmp(prefix,text,strlen(prefix)) == 0) {
    for (; i < 0x20; i++, entries++) {
      if (text == 0) break;
      if (sscanf(text,"%63s %d %d %d %63s %d %d %d %d %d %d %d %d\n",tex_name,&brightness,&saturation,&hue,
                       swap_name,&color_lo[0],&color_lo[1],&color_lo[2],&color_lo[3],&color_hi[0],&color_hi[1],&color_hi[2],&color_hi[3]) != 13) break;
      entries->tex_no = texcol_name_lookup(tex_name,prefix,texcol_names);
      entries->brightness = brightness;
      entries->saturation = saturation;
      entries->hue = hue;
      entries->swap_tex_no = texcol_name_lookup(swap_name,prefix,texcol_names);
      entries->field_0x8 = color_lo[0];
      entries->color_a[0] = color_lo[1];
      entries->color_a[1] = color_lo[2];
      entries->color_a[2] = color_lo[3];
      entries->color_on = color_hi[0];
      entries->color_b[0] = color_hi[1];
      entries->color_b[1] = color_hi[2];
      entries->color_b[2] = color_hi[3];
      text = strchr(text,10);
      { int t = text == 0; text += t == 0; }
    }
  }
  for (; i < 0x20; i++, entries++) {
    memcpy(entries,&txn_entry_end,0x10);
  }
}

void texcol_entry_clear(txn_entry *entry)
{
  entry->tex_no = -2;
  entry->brightness = 0;
  entry->saturation = 0;
  entry->hue = 0;
  entry->swap_tex_no = -1;
  entry->field_0x8 = 0xff;
  entry->color_a[0] = 0xff;
  entry->color_a[1] = 0xff;
  entry->color_a[2] = 0xff;
  entry->color_on = 0;
  entry->color_b[0] = 0;
  entry->color_b[1] = 0;
  entry->color_b[2] = 0;
  return;
}

void clear_material(void)
{
  int i;
  int list;

  list = 0;
  do {
    i = 0;
    do {
      texcol_entry_clear(&texcol_body_lists[0][list * 0x20 + i]);
      i = i + 1;
    } while (i < 0x20);
    list = list + 1;
  } while (list < 4);
  return;
}

void clear_opt_material(void)
{
  int i;

  i = 0;
  do {
    texcol_entry_clear(&part_txn_lists[0][i]);
    texcol_entry_clear(&part_txn_lists[1][i]);
    texcol_entry_clear(&part_txn_lists[2][i]);
    texcol_entry_clear(&part_txn_lists[3][i]);
    i = i + 1;
  } while (i < 0x20);
  return;
}

void txn_lists_clear_all(void)
{
  clear_material();
  clear_opt_material();
  return;
}

void chara_body_load_player0(void)
{
  rob_rec *rob;

  rob = rob_get(0);
  chara_body_load(rob);
  return;
}

void chara_body_load(rob_rec *rob)
{
  int bank; uchar side; int busy; int costumes; int costumes2;

  busy = poly_queue_busy_get();
  if (busy == 0) {
    rob_disp_release(rob);
    if ((rob_color_flags & 2) == 0) {
      txn_list_append(body_txn_merged,0);
      txn_list_append(body_txn_merged,texcol_body_lists[0]);
      txn_list_append(body_txn_merged,texcol_body_lists[1]);
      txn_list_append(body_txn_merged,texcol_body_lists[2]);
      txn_list_append(body_txn_merged,texcol_body_lists[3]);
      txn_list_current_set(body_txn_merged);
    }
    obj_tex_base_set(rob_tex_bases[rob->side]);
    bank = rob->side + 5;
    costumes = *(int *)((int)chara_data_get((uint)rob->chara) + 0x48);
    load_poly(bank,*(int *)((uint)rob->costume * 0x20 + costumes + 4),
              &rob_obj_ofs[(uint)rob->side],
              &objmem_heap_chr[(uint)rob->side]);
    side = rob->side;
    costumes2 = *(int *)((int)chara_data_get((uint)rob->chara) + 0x48);
    rob_hand_models_build((uint)side,*(uint *)((uint)rob->costume * 0x20 + costumes2 + 0xc));
    rob_disp_setup(rob);
  }
  return;
}

void chara_body_load_req_player0(void)
{
  rob_rec *rob;
  uint (*costumes)[8];

  rob = rob_get(0);
  if (poly_queue_busy_get() != 0) {
    return;
  }
  objbank_free_req(rob->side + 5);
  rob_hand_models_clear((uint)rob->side);
  rob_disp_release(rob);
  if ((rob_color_flags & 2) == 0) {
    txn_list_append(body_txn_merged,0);
    txn_list_append(body_txn_merged,texcol_body_lists[0]);
    txn_list_append(body_txn_merged,texcol_body_lists[1]);
    txn_list_append(body_txn_merged,texcol_body_lists[2]);
    txn_list_append(body_txn_merged,texcol_body_lists[3]);
    txn_list_current_set(body_txn_merged);
  }
  load_poly_req(rob->side + 5,(costumes = *(uint (**)[8])((int)chara_data_get((uint)rob->chara) + 0x48), costumes[rob->costume][1]),0,
               &rob_obj_ofs[(uint)rob->side],&heap_texdata,&heap_texdata,
               rob_tex_bases[rob->side]);
  texcol_reload_pending = 1;
}

void chara_parts_load(rob_rec *rob)
{
  char *chara_data;
  int *objs;
  int set;
  int sets;
  int i;
  static_heap_rec bufs[4];
  int tex_bases;

  chara_data = (char *)chara_data_get((uint)rob->chara);
  sets = *(int *)(chara_data + 0x48) + (uint)rob->costume * 0x10 + 0x9c;
  if (poly_queue_busy_get() == 0) {
    for (i = 0; i < 4; i++) {
      objbank_free((uint)rob->side + i * 2 + 0xd);
    }
    newopt_obj_addr_assign((int *)sets,bufs,&newopt_obj_ofs[(uint)rob->side]);
    for (i = 0; i < 4; i++) {
      set = *(int *)(i * 4 + sets);
      objs = rob_card_item_obj_list((int)(char)rob->side,(int)(char)rob->chara,(int)(char)rob->costume,i);
      if ((0 < *objs) && (mdl_keep_node_set(*objs), set != -1)) {
        txn_list_current_set(part_txn_lists[i]);
        if ((rob_color_flags & 2) != 0) {
          rob_texcol_part_select((uint)rob->side,i);
        }
        tex_bases = (int)rob_item_tex_bases;
        obj_tex_base_set(rob_item_tex_bases[rob->side] + i * 0x80);
        load_poly(i * 2 + (uint)rob->side + 0xd,set,&bufs[i],(static_heap_rec *)0x0);
      }
    }
  }
}

void pol_send_itm1p(void)
{
  rob_rec *rob;

  rob = rob_get(0);
  objbank_free(0xd);
  objbank_free(0xf);
  objbank_free(0x11);
  objbank_free(0x13);
  chara_parts_load(rob);
  rob_items_from_part_test((char)rob->side,(char)rob->chara,(char)rob->costume);
  rob_disp_setup(rob);
  return;
}

void pol_send_itm2p(void)
{
  rob_rec *rob;

  rob = rob_get(1);
  objbank_free(0xe);
  objbank_free(0x10);
  objbank_free(0x12);
  objbank_free(0x14);
  chara_parts_load(rob);
  rob_items_from_part_test((char)rob->side,(char)rob->chara,(char)rob->costume);
  rob_disp_setup(rob);
  return;
}
