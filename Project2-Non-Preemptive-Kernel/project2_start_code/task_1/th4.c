#include "common.h"
#include "scheduler.h"
#include "util.h"
#include "lock.h"

lock_t lock;

int len = 0;
void thread_print(char * toPrint) {
	print_str(len, 0, toPrint);
	len++;

}

static int test_line = 10;
void thread11(void)
{
	lock_init(&lock);
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("Hlock initialized by thread 1! ");
		
	lock_acquire(&lock);
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("lock acquired by thread 1! yielding... ");
		
	do_yield();
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 1 in context! ");
		
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("Hthread 1 releasing lock ");
		
	lock_release(&lock);
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 1 exiting ");
		
	do_exit();
}


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

void thread12(void)
{
	int i, j= 22;
	while(1){

		for(i = 60; i > 0; i = i -1){
			/* print */
			print_location(i, j);
			print_color(plane1, ANSI_COLOR_YELLOW);
			j++;
			print_location(i, j);
			print_color(plane2, ANSI_COLOR_RED);
			j++;
			print_location(i, j);
			print_color(plane3, ANSI_COLOR_GREEN);
			j++;
			print_location(i, j);
			print_color(plane4, ANSI_COLOR_BLUE);
			j++;
			delay1s();
			j=j-4;

			do_yield();
			/* clear */
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_YELLOW);
			j++;
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_RED);
			j++;
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_GREEN);
			j++;
			print_location(i, j);
			print_color(BLANK, ANSI_COLOR_BLUE);
			j++;
			delay1s();
			j = j-4;
		}
	}
}

void thread13(void)
{
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 3 in context! yielding... ");
		//printstr("thread 3 in context! yielding...\r\n");
		
	do_yield();
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 3 acquiring lock! ");
		//printstr("thread 3 acquiring lock!\r\n");
		
	lock_acquire(&lock);
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 3 in context! releasing lock!!");
		//printstr("thread 3 in context! releasing lock!!\r\n");
		
	lock_release(&lock);
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 3 exiting ");
		//printstr("thread 3 exiting\r\n");
		
	do_exit();
}

void thread14(void)
{
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 4 in context! acquiring lock... ");
		//printstr("thread 4 in context! acquiring lock...\r\n");
		
	lock_acquire(&lock);
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 4 in context! releasing lock... ");
		//printstr("thread 4 in context! releasing lock...\r\n");
		
	lock_release(&lock);
		//thread_print("thread 4 exiting!");
		print_location(0, test_line);
		test_line = test_line +1;
		printstr("thread 4 exiting! ");
		
	do_exit();
}
