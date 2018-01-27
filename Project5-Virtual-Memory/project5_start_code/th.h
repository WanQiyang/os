/* th.h Best viewed with tabs set to 4 spaces. */

#ifndef TH_H
#define TH_H

// Prototypes
void clock_thread(void);

/* Threads to test the condition variables and locks */
void thread2(void);
void thread3(void);
/* Threads to test semaphores */
void num(void);
void scroll_th(void);
void caps(void);

/* Threads to test barriers */
void barrier1(void);
void barrier2(void);
void barrier3(void);

#endif
