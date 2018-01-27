#ifndef FILES_H
#define FILES_H

#include "ramdisk.h"

/* return the number of 'files' in this filesystem' */
int get_num_files();

/* get a pointer to the n-th file */
File *get_nth_file(int n);

#endif

