/*
 * Simple counter program, to check the functionality of yield().
 * Each process prints their invocation counter on a separate line
 */

#include "common.h"
#include "syslib.h"
#include "util.h"

static void print_counter(int done);
static int rec(int n);

void _start(void)
{
    int i;

    for (i = 0; i <= 100; i++) {
		print_location(0,7);
        printstr("Did you know that 1 + ... + ");
        printint(28,7, i);
		print_location(31,7);
        printstr(" = ");
        printint(34, 7, rec(i));
        print_counter(FALSE);
		delay1s();
        yield();
    }
    print_counter(TRUE);
    exit();
}

static void print_counter(int done)
{
    static int counter = 0;

	print_location(0,8);
    printstr("Process 2 (Math)      : ");
    if (done) {
		print_location(25,8);
        printstr("Exited");
    } else {
        printint(25,8, counter++);
    }
}

/* calculate 1 + ... + n */
static int rec(int n)
{
    if (n % 37 == 0) {
        yield();
    }
    if (n == 0) {
        return 0;
    } else {
        return n + rec(n - 1);
    }
}
