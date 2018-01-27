#ifndef RAMDISK_H
#define RAMDISK_H

/* ramdisk provides a fake filesystem.
 * It is read-only, and its contents
 * are assembled at compile time.
 */

#include "scheduler.h"
typedef void (*Process)(void);

typedef struct
{
  const char *  filename;
  Process       process;
  task_type_t   task_type;
} File;

Process ramdisk_find(const char* filename);


#endif

