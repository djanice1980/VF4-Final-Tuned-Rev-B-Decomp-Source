/*
 * vf4lib/libsprite/src/aet_anim_if.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

float aet_key_curve_eval(aet_track *track, float frame)
{
  int n;
  int i;
  aet_key *k;
  float t0;
  float d;
  float inv;
  float inv2;
  float d2;
  float a;
  float d3;
  float c2;
  float b;

  n = track->key_count;
  k = track->keys;
  if (n <= 0) {
    return 0.0;
  }
  if (n == 1) {
    return k->value;
  }
  for (i = 0; i < n && k->frame < frame; i++, k++) {
  }
  if (i >= n) {
    return k[-1].value;
  }
  if (i == 0) {
    return k->value;
  }
  t0 = k[-1].frame;
  d = frame - t0;
  if (fabsf(k->frame - t0) < 1.1920929e-07) {
    fatal_at("/p71/prj71p/vf4e2/vf4lib/libsprite/src/aet_anim_if.c",0x28,"ERROR!\n");
  }
  inv = 1.0 / (k->frame - t0);
  inv2 = inv * inv;
  d2 = d * d;
  a = d2 * inv;
  d3 = d2 * d * inv2;
  c2 = (d3 + d3) * inv;
  b = d2 * 3.0 * inv2;
  d3 = d3 - a;
  return k[-1].value * ((c2 - b) + 1.0) + k->value * (b - c2) + k[-1].out_slope * ((d3 - a) + d) + k->in_slope * d3;
}

void aet_image_picture_set(aet_image *image, float frame)
{
  int index;
  int picture;
  if (image != 0) {
    if (image->picture_count <= 1) {
      picture = 0;
    }
    else {
      index = (int)((float)image->picture_rate * frame);
      if (index < image->picture_count) {
        image->picture = index;
        return;
      }
      picture = image->picture_count - 1;
    }
    image->picture = picture;
  }
}

void aet_layer_tracks_eval(aet_layer *layer, float frame)
{
  float *out;

  if (layer == 0) return;
  if ((layer->flags & 1) == 0) {
    layer->visible = 1;
  }
  else if (layer->start_frame > frame || !(layer->end_frame > frame)) {
    layer->visible = 0;
  }
  else {
    layer->visible = 1;
  }
  out = &layer->anchor_x;
  *out = aet_key_curve_eval(&layer->tracks[0],frame);
  out++;
  *out = aet_key_curve_eval(&layer->tracks[1],frame);
  out++;
  *out = aet_key_curve_eval(&layer->tracks[2],frame);
  out++;
  *out = aet_key_curve_eval(&layer->tracks[3],frame);
  out++;
  *out = aet_key_curve_eval(&layer->tracks[4],frame);
  out++;
  *out = aet_key_curve_eval(&layer->tracks[5],frame);
  out++;
  *out = aet_key_curve_eval(&layer->tracks[6],frame);
  out++;
  *out = aet_key_curve_eval(&layer->tracks[7],frame);
}

void aet_comp_visible_update(aet_comp *comp, float frame)
{
  if (comp != 0) {
    if ((frame < comp->start_frame) || (comp->end_frame <= frame)) {
      comp->visible = 0;
    }
    else {
      comp->visible = 1;
    }
  }
  return;
}

aet_layer * aet_comp_layer_find(aet_comp *comp, char *name)
{
  aet_layer *layer;
  int i;

  if (comp != 0 && name != 0) {
    for (i = 0; i < comp->layer_count; i++) {
      if (comp->layers[i].name != 0) {
        if (strcmp(name,comp->layers[i].name) == 0) {
          return &comp->layers[i];
        }
        if ((comp->layers[i].type == '\x01') &&
           (layer = aet_comp_layer_find(comp->layers[i].item.comp,name), layer != 0)) {
          return layer;
        }
      }
    }
  }
  return 0;
}

aet_marker * aet_comp_marker_find(aet_comp *comp, char *name)
{
  aet_marker *marker;
  int j;
  int i;

  if (comp != 0 && name != 0) {
    for (i = 0; i < comp->layer_count; i++) {
      for (j = 0; j < comp->layers[i].marker_count; j++) {
        if (comp->layers[i].markers[j].name != 0 && strcmp(name,comp->layers[i].markers[j].name) == 0) {
          return &comp->layers[i].markers[j];
        }
      }
      if (comp->layers[i].type == '\x01') {
        marker = aet_comp_marker_find(comp->layers[i].item.comp,name);
        if (marker != 0) return marker;
      }
    }
  }
  return 0;
}

aet_comp * aet_main_comp_get(aet_scene *scene)
{
  aet_comp *comp;

  if (scene != 0 && scene->comp_count > 0) {
    comp = &scene->comps[scene->comp_count - 1];
  }
  else {
    comp = 0;
  }
  return comp;
}

void aet_scene_relocate(aet_scene *scene, int base)
{
  int i;

  if (scene != 0) {
    if (scene->comps != 0) {
      scene->comps = (aet_comp *)((int)scene->comps + base);
    }
    if (scene->images != 0) {
      scene->images = (aet_image *)((int)scene->images + base);
    }
    if (scene->table3 != 0) {
      scene->table3 = (aet_table3_item *)((int)scene->table3 + base);
    }
    for (i = 0; i < scene->comp_count; i = i + 1) {
        aet_comp_relocate(&scene->comps[i],base);
    }
    for (i = 0; i < scene->image_count; i = i + 1) {
        aet_image_relocate(&scene->images[i],base);
    }
    for (i = 0; i < scene->table3_count; i = i + 1) {
        aet_table3_item_relocate(&scene->table3[i],base);
    }
  }
  return;
}

void aet_table3_item_relocate(aet_table3_item *item, int base)
{
  return;
}

void aet_image_relocate(aet_image *image, int base)
{
  if (image->pictures != 0) {
    image->pictures = (aet_picture *)((int)image->pictures + base);
  }
  return;
}

void aet_track_relocate(aet_track *track, int base)
{
  if (track->keys != 0) {
    track->keys = (aet_key *)((int)track->keys + base);
  }
  return;
}

void aet_marker_relocate(aet_marker *marker, int base)
{
  if (marker->name != 0) {
    marker->name = marker->name + base;
  }
  return;
}

void aet_layer_relocate(aet_layer *layer, int base)
{
  int i;
  if (layer->name != 0) {
    layer->name = layer->name + base;
  }
  if (layer->item.image != 0) {
    layer->item.image = (aet_image *)((int)layer->item.image + base);
  }
  if (layer->parent != 0) {
    layer->parent = (aet_layer *)((int)layer->parent + base);
  }
  if (layer->markers != 0) {
    layer->markers = (aet_marker *)((int)layer->markers + base);
  }
  aet_track_relocate(&layer->tracks[0],base);
  aet_track_relocate(&layer->tracks[1],base);
  aet_track_relocate(&layer->tracks[2],base);
  aet_track_relocate(&layer->tracks[3],base);
  aet_track_relocate(&layer->tracks[4],base);
  aet_track_relocate(&layer->tracks[5],base);
  aet_track_relocate(&layer->tracks[6],base);
  aet_track_relocate(&layer->tracks[7],base);
  for (i = 0; i < layer->marker_count; i++) {
    aet_marker_relocate(&layer->markers[i],base);
  }
}

void aet_comp_relocate(aet_comp *comp, int base)
{
  int i;

  if (comp->layers != 0) {
    comp->layers = (aet_layer *)((int)comp->layers + base);
  }
  for (i = 0; i < comp->layer_count; i = i + 1) {
      aet_layer_relocate(&comp->layers[i],base);
  }
  return;
}
