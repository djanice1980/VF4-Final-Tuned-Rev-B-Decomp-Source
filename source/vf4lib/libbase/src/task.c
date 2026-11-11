/*
 * vf4lib/libbase/src/task.c
 * Decompiler output that compiles to the game's bytes (found by
 * tools/match/auto_match.py, moved here by tools/match/adopt.py); every
 * function here matches (python3 -I tools/match/match.py checks the tree).
 * match-flags: -fno-delayed-branch
 */
#include "vf4ft.h"

void task_tree_init(void)
{
  task_list_link(task_tree);
  return;
}

void task_list_link(task_node_rec *node)
{
  task_rec *task;
  for (; node->task != 0; node++) {
    if (node->before != 0) {
      task_list_link(node->before);
    }
    task = node->task;
    task->node = node;
    task->name = node->name;
    task->flags = 2;
    task_stats_clear(task);
    if (node->after != 0) {
      task_list_link(node->after);
    }
  }
  task_tree_init_value = 0xe;
  task_stats_window = 0x708;
  task_stats_frame = 0;
}

void task_tree_frame(void)
{
  task_tree_usec = (float)task_list_run(task_tree);
  task_tree_stats_update();
  perf_color_mark(0xffffff);
  return;
}

float task_list_run(task_node_rec *list)
{
  float total = 0.0;
  task_node_rec *node = list;
  task_rec *task;
  uint flags;
  float self;
  float sum;
  float t0;

  for (; node->task != 0; node++) {
    task = node->task;
    task_current = task;
    flags = task->flags;
    self = 0.0;
    sum = 0.0;
    if ((flags & 3) == 3) {
      if (node->before != 0) {
        sum += task_list_run(node->before);
      }
      perf_color_mark(node->color);
      t0 = (float)(int)stopwatch_usec();
      task->exec();
      self = (float)(int)stopwatch_usec() - t0;
      sum += self;
      if (node->after != 0) {
        sum += task_list_run(node->after);
      }
    }
    task->self_usec = self;
    task->total_usec = sum;
    total += sum;
  }
  return total;
}

void task_start(task_rec *task)
{
  void (*init)(void);

  task->flags = task->flags | 1;
  init = task->node->init;
  task->exec = task->node->exec;
  task_stats_clear(task);
  if (init != 0) {
    (*init)();
  }
  return;
}

void task_start_with_init(task_rec *task, void (*init)(void))
{
  task->flags = task->flags | 1;
  task->exec = task->node->exec;
  task_stats_clear(task);
  if (init != 0) {
    (*init)();
  }
  return;
}

void task_exec_set(task_rec *task, void (*exec)(void))
{
  task->exec = exec;
  return;
}

void task_stop(task_rec *task)
{
  task->flags = 2;
  if (task->node->free != 0) {
    (*task->node->free)();
  }
  return;
}

void task_tree_stop(void)
{
  task_list_stop(task_tree);
  return;
}

void task_list_stop(task_node_rec *node)
{
  for (; node->task != 0; node = node + 1) {
    if (node->before != 0) {
      task_list_stop(node->before);
    }
    if ((node->resident & 1) == 0) {
      task_stop(node->task);
    }
    if (node->after != 0) {
      task_list_stop(node->after);
    }
  }
  return;
}

void task_resume(task_rec *task)
{
  task->flags = task->flags | 2;
  return;
}

void task_tree_resume(void)
{
  task_list_resume(task_tree);
  return;
}

void task_list_resume(task_node_rec *node)
{
  for (; node->task != 0; node = node + 1) {
    if (node->before != 0) {
      task_list_resume(node->before);
    }
    node->task->flags = node->task->flags | 2;
    if (node->after != 0) {
      task_list_resume(node->after);
    }
  }
}

void task_pause(task_rec *task)
{
  task->flags = task->flags & 0xfffffffd;
  return;
}

void task_tree_pause(void)
{
  task_list_pause(task_tree);
  return;
}

void task_list_pause(task_node_rec *node)
{
  for (; node->task != 0; node++) {
    if (node->before != 0) {
      task_list_pause(node->before);
    }
    node->task->flags = node->task->flags & 0xfffffffd;
    if (node->after != 0) {
      task_list_pause(node->after);
    }
  }
}

void task_tree_stats_update(void)
{
  uint paused;
  paused = pause_frame_held();
  if (paused == 0) {
    task_stats_frame = task_stats_frame + 1;
    task_list_stats_update(task_tree);
    if (task_stats_frame > task_stats_window) {
      task_stats_frame = paused;
    }
  }
}

void task_list_stats_update(task_node_rec *node)
{
  task_rec *task;
  float self;
  float max;
  float sum;
  float count;

  for (; node->task != 0; node++) {
    task = node->task;
    if ((task->flags & 1) != 0) {
      if (node->before != 0) {
        task_list_stats_update(node->before);
      }
      self = task->self_usec;
      max = task->max_usec;
      sum = task->sum_usec;
      count = task->count;
      sum += self;
      count += 1.0;
      if (self > max) max = self;
      task->max_usec = max;
      task->sum_usec = sum;
      task->count = count;
      task->average_usec = sum / count;
      if (task_stats_frame > task_stats_window) {
        task->max_usec = 0.0;
        task->sum_usec = 0.0;
        task->count = 0.0;
        task->average_usec = 0.0;
      }
      if (node->after != 0) {
        task_list_stats_update(node->after);
      }
    }
  }
}

void task_stats_clear(task_rec *task)
{
  float z = 0;
  task->max_usec = z;
  task->sum_usec = z;
  task->count = z;
  task->average_usec = z;
  return;
}

void cpu_usage_capture(void)
{
  cpu_usage_us = stopwatch_usec();
  return;
}

uint * cpu_usage_us_ptr(void)
{
  return &cpu_usage_us;
}
