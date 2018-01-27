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



