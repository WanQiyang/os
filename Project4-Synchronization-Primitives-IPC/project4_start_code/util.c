/* util.c: Various utility functions that can be linked with both the kernel and user code. */
/* Do not change this file */

#include "util.h"

int line_all =0;

#define PORT3F8  (0xbfe48000)
#define PORT3FD	 (0xbfe48006)

void printstr(char *s){
	int i;
	unsigned long port = PORT3F8;
	while(*s){
		i = 3000;
		while(i--);
		*(unsigned char *)port = *s;
		s++;
	}
}

void print_location(int x, int y){
	printstr("\e[");
	char temp1[32];

	itoa(x,temp1);
	printstr(temp1);

	printstr(";");
	char temp2[32];
	itoa(y, temp2);
	printstr(temp2);
	printstr("H");
}

void print_color(char *s, char *color){
	printstr(color);
	printstr(s);
	printstr(" \e[0m");
}

void delays(void){
	unsigned char h,i,j,k;
	for(h = 5; h > 0; h--)
		for ( i = 4; i > 0; i--)
			for (j = 116; j > 0; j--)
				for (k = 214; k > 0; k--);
}

/* Write an integer to the screen */
void print_int(int line, int col, int num)
{
     int i, n, neg_flag;
     char buf[12];

     neg_flag = num < 0;
     if (neg_flag)
          num = ~num + 1;

     itoa(num, buf);

     n = strlen(buf);
     if (neg_flag)
          print_char(line, col++, '-');
     for (i = 0; i < n; i++)
          print_char(line_all, col++, buf[i]);
     line_all ++;
}

/* Write an integer to the screen, base 16 */
void print_hex(int line, int col, uint32_t num)
{
     int i, n;
     char buf[12];

     itohex(num, buf);

     n = strlen(buf);
     for (i = 0; i < n; i++)
          print_char(line_all, col + i, buf[i]);
     line_all ++;
}

/* Write a string to the screen */
void print_str(int line, int col, char *str)
{
     int i, n;

     n = strlen(str);
//	 print_int(6,0,n);
     for (i = 0; i < n; i++)
          print_char(line, col + i, str[i]);
}


void clear_screen(int minx, int miny, int maxx, int maxy)
{
     int i, j, k;

     for (j = miny; j < maxy; j++) {
		 print_location(j, 0);
		 printstr("                                                                              ");
		 /*
          for (i = minx; i < maxx; i++) {
               print_char(i,j,' ');
          }
		  */
     }
}

/* Convert an ASCII string (like "234") to an integer */
uint32_t atoi(char *s)
{
    int n;
    for (n = 0; *s >= '0' && *s <= '9'; n = n * 10 + *s++ - '0');
    return n;
}

/*
 * Functions from Kernighan/Ritchie - The C Programming Language
 */

/* Convert an integer to an ASCII string, Page 64 */
void itoa(uint32_t n, char *s)
{
    int i;

    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i++] = 0;
    reverse(s);
}

/* Convert an integer to an ASCII string, base 16 */
void itohex(uint32_t n, char *s)
{
    int i, d;

    i = 0;
    do {
        d = n % 16;
        if (d < 10)
            s[i++] = d + '0';
        else
            s[i++] = d - 10 + 'a';
    } while ((n /= 16) > 0);
    s[i++] = 0;
    reverse(s);
}

/* Reverse a string, Page 62 */
void reverse(char *s)
{
    int c, i, j;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* Get the length of a null-terminated string, Page 99 */
int strlen(char *s)
{
    int n;
    for (n = 0; *s != '\0'; s++)
        n++;
    return n;
}

/* return TRUE if string s1 and string s2 are equal */
int same_string(char *s1, char *s2)
{
    while ((*s1 != 0) && (*s2 != 0)) {
        if (*s1 != *s2)
            return FALSE;
        s1++;
        s2++;
    }
    return (*s1 == *s2);
}

/* Block copy: copy size bytes from source to dest. The arrays can overlap
 */
void bcopy(char *source, char *destin, int size)
{
    int i;

    if (size == 0)
        return;

    if (source < destin) {
        for (i = size - 1; i >= 0; i--)
            destin[i] = source[i];
    } else {
        for (i = 0; i < size; i++)
            destin[i] = source[i];
    }
}

/* Zero out size bytes starting at area */
void bzero(char *area, int size)
{
    int i;

    for (i = 0; i < size; i++)
        area[i] = 0;
}

static uint32_t rand_val;

void srand(uint32_t seed)
{
    rand_val = seed;
}

/* Return a pseudo random number */
uint32_t rand(void)
{
  rand_val = rand_step( rand_val );
  return rand_val;
}

/* Perform a single step of random
 * number generation.  Does not
 * rely on global state */
uint32_t rand_step(uint32_t seed)
{
  return (1103515245*seed + 12345) >> 16;
}
