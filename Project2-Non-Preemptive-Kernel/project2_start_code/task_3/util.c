/*	util.c

	Various utility functions that can be linked with both the kernel
	and user code.

	K&R = "The C Programming Language", Kernighan, Ritchie

	Best viewed with tabs set to 4 spaces.
 */

#include "util.h"


static short *screen = SCREEN_ADDR;
static int line_all = 0;


static char pic[]={"O(∩_∩)O"};


#define PORT3f8 0xbfe48000
#define PORT3fd 0xbfe48006


void printstr(char *s)
{
	int i;
	unsigned long port = PORT3f8;
	while (*s) {
		i=10000;
		while(i--);
		*(unsigned char*)port = *s;
		s++;
	}
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
		print_char(line, col++, buf[i]);
}

/* Write an integer to the screen, base 16 */
void print_hex(int line, int col, uint32_t num)
{
	int i, n;
	char buf[12];

	itohex(num, buf);

	n = strlen(buf);
	for (i = 0; i < n; i++)
		print_char(line, col + i, buf[i]);
}

/* Write a string to the screen */
void print_str(int line, int col, char *str)
{
	int i, n;

	print_location(line, col);
	printstr(str);
}

/* Write a character on the screen */
void print_char(int line, int col, char c)
{
	unsigned long port = PORT3f8;
	print_location(line, col);
	*(unsigned char *)port = c;
}

/* Read the character stored at location (x,y) on the screen */
int peek_screen(int x, int y)
{
	return screen[y * 80 + x] & 0xff;
}

void clear_screen(int minx, int miny, int maxx, int maxy)
{
	int i, j, k;
	for ( i = miny; i<maxx; ++i)
	{
		print_location(0,i);
		printstr("                                                                 ");

	}
}

/* scroll screen */
void scroll(int minx, int miny, int maxx, int maxy)
{
	int i, j, k;

	for (j = miny; j < maxy; j++) {     /* all lines */
		for (i = minx; i < maxx; i++) { /* all characters */
			k = j * 80 + i;
			if (j < maxy - 1)   /* if not in first line */
				screen[k] = screen[k + 80];     /* move character one line up */
			else
				screen[k] = 0x0700;     /* 0x0700 = blank */
		}
	}
}

extern uint64_t get_timer(void);
/* Simple delay loop to slow things down */
void delay(int millis)
{
	uint64_t t;
	t = get_timer() + millis * 1000 * MHZ;
	while (get_timer() < t) {
	}
}


/* Read the 1C time stamp counter */

/* Convert an ASCII string (like "234") to an integer */
uint32_t atoi(char *s)
{
	int n;
	for (n = 0; *s >= '0' && *s <= '9'; n = n * 10 + *s++ - '0');
	return n;
}

/* 
 * Functions from Kerninghan/Ritchie - The C Programming Language
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

/* Block copy: copy size bytes from source to dest.
 * The arrays can overlap */
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

/* Read byte from I/O address space */
uint8_t inb(int port)
{
	int ret;

	//asm volatile ("xorl %eax,%eax");
	//asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));

	return ret;
}

/* Write byte to I/O address space */
void outb(int port, uint8_t data)
{
	//asm volatile ("outb %%al,%%dx"::"a" (data), "d"(port));
}


static uint32_t rand_val;

void srand(uint32_t seed)
{
	rand_val = seed;
}

/* Return a pseudo random number */
uint32_t rand(void)
{
	rand_val = (rand_val * 1103515245 + 12345) >> 16;
	return rand_val;
}

void print_color(char *s, char *color){
	printstr(color);
	printstr(s);
	printstr(" \e[0m");
}

void println_color(char *s, char *color){
	printstr(color);
	printstr(s);
	printstr(" \e[0m\r\n");
}

void print_location(int x, int y){
	printstr("\e[");
	char temp1[32];
	itoa(y, temp1);
	printstr(temp1);
	printstr(";");
	char temp2[32];
	itoa(x, temp2);
	printstr(temp2);
	printstr("H");
}

void printint(int x, int y, int n)
{
	int i,j;
	unsigned char a[40];
	unsigned long port = PORT3f8;
	unsigned char temp;
	i=10000;
	print_location(x, y);
	printstr("                            ");
	print_location(x, y);

	while(i--);

	i = 0;
	do {
		a[i] = n % 10;
		n = n / 10;
		i++;
	}while(n);

	for (j=i-1;j>=0;j--) {
		i=100000;
		temp = '0' + a[j];
		while(i--);
		*(unsigned char*)port = temp;
	}
}

void cursor_up(int n){
	printstr("\e[");
	char temp[8];
	itoa(n, temp);
	printstr(temp);
	printstr("A");
}

void cursor_down(int n){
	printstr("\e[");
	char temp[8];
	itoa(n, temp);
	printstr(temp);
	printstr("B");
}

void cursor_right(int n){
	printstr("\e[");
	char temp[8];
	itoa(n, temp);
	printstr(temp);
	printstr("C");
}

void cursor_left(int n){
	printstr("\e[");
	char temp[8];
	itoa(n, temp);
	printstr(temp);
	printstr("D");
}


void delay1s(void){
	unsigned char h,i,j,k;
	for(h=5;h>0;h--)
		for(i=4;i>0;i--)
			for(j=116;j>0;j--)
				for(k=214;k>0;k--);
}

 void printnum2(unsigned long long n)
 {
   int i,j;
   unsigned char a[40];
   unsigned long port = PORT3f8;
   i=10000;
   while(i--);

   i = 0;
   do {
   a[i] = n % 16;
   n = n / 16;
   i++;
   }while(n);

  for (j=i-1;j>=0;j--) {
   if (a[j]>=10) {
     //while (((*(volatile unsigned char*)PORT3fd) & 0x20)==0);
      *(unsigned char*)port = 'a' + a[j] - 10;
    }else{
      //while (((*(volatile unsigned char*)PORT3fd) & 0x20)==0);
	*(unsigned char*)port = '0' + a[j];
   }
  }
  printstr("\r\n");
}

