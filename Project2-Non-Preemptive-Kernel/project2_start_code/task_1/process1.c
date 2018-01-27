/*
 * Simple counter program, to check the functionality of yield().
 * Each process prints their invocation counter on a separate line
 */

#include "common.h"
#include "syslib.h"
#include "util.h"


#define ANSI_COLOR_RED     "\e[31m"
#define ANSI_COLOR_GREEN   "\e[32m"
#define ANSI_COLOR_YELLOW  "\e[33m"
#define ANSI_COLOR_BLUE    "\e[34m"
#define ANSI_COLOR_MAGENTA "\e[35m"
#define ANSI_COLOR_CYAN    "\e[36m"
#define ANSI_COLOR_RESET   "\e[0m

#define BLANK   "                    "
static char plane1[]={"     ___       _    "};
static char plane2[]={" | __\\_\\_o____/_| "};
static char plane3[]={" <[___\\_\\_-----<  "};
static char plane4[]={" |  o'              "};

void _start(void)
{
	while(1){
		volatile static int i, j= 22;
		for(i = 60; i > 0; ){
			/* clear */
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_YELLOW);
			j=j+1;
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_RED);
			j=j+1;
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_GREEN);
			j=j+1;
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_BLUE);
			j=j+1;
			delay1s();
			j = j-4;
			i = i -1;

			yield();
			/* print */
			print_location(i, j);
			print_color(plane1, ANSI_COLOR_YELLOW);
			j=j+1;
			print_location(i, j);
			print_color(plane2, ANSI_COLOR_RED);
			j=j+1;
			print_location(i, j);
			print_color(plane3, ANSI_COLOR_GREEN);
			j=j+1;
			print_location(i, j);
			print_color(plane4, ANSI_COLOR_BLUE);
			j=j+1;
			delay1s();
			j=j-4;

		}
	}
}
