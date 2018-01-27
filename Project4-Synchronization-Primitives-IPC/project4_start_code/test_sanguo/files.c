#include "files.h"
#include "processes.h"

/* This file represents a table
 * mapping file names to entry
 * points.  It is analogous to
 * tasks.c in the previous project.
 */

static File files[] =
  {
    {.filename = "init", .process = PROC4_ADDR, .task_type = PROCESS},
    {.filename = "clock_thread", .process = &clock_thread, .task_type = KERNEL_THREAD},
    {.filename = "SunQuan", .process = PROC1_ADDR , .task_type = PROCESS},
    {.filename = "LiuBei", .process = PROC2_ADDR, .task_type = PROCESS },
    {.filename = "CaoCao", .process = PROC3_ADDR, .task_type = PROCESS },
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



