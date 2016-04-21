#include <env.h>
#include <pmap.h>
#include <printf.h>

// Trivial temporary clock interrupt handler,
// called from clock_interrupt in locore.S
/*void
clock(void)
{
	printf("*");
}*/


// The real clock interrupt handler,
// implementing round-robin scheduling
void
sched_yield(void)
{
	static int i=1;
	env_run(&envs[i^=1]);
}

