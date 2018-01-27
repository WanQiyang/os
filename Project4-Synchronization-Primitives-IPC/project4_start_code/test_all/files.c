#include "files.h"
#include "processes.h"

/* This file represents a table
 * mapping file names to entry
 * points.  It is analogous to
 * tasks.c in the previous project.
 */

static File files[] =
  {
    {.filename = "init", .process = PROC4_ADDR, .task_type= PROCESS },
    {.filename = "barrier1", .process = &barrier1, .task_type = KERNEL_THREAD},
    {.filename = "barrier2", .process = &barrier2, .task_type = KERNEL_THREAD},
    {.filename = "barrier3", .process = &barrier3, .task_type = KERNEL_THREAD},
    {.filename = "clock_thread", .process = &clock_thread, .task_type = KERNEL_THREAD},
    {.filename = "thread2", .process = &thread2, .task_type = KERNEL_THREAD},
    {.filename = "thread3", .process = &thread3, .task_type = KERNEL_THREAD},
    {.filename = "num", .process = &num, .task_type = KERNEL_THREAD},
    {.filename = "caps", .process = &caps, .task_type = KERNEL_THREAD},
    {.filename = "scroll_th", .process = &scroll_th, .task_type = KERNEL_THREAD},
    {.filename = "PROC1", .process = PROC1_ADDR, .task_type = PROCESS},
    {.filename = "PROC2", .process = PROC2_ADDR, .task_type = PROCESS},
  };

#define NUM_FILES     ( sizeof(files) / sizeof(File) )

int get_num_files()
{
  return NUM_FILES;
}

File *get_nth_file(int n)
{
  return &files[n];
}



