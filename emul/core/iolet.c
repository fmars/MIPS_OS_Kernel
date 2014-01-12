#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include "icu.h"

static unsigned int iolet_data;
static sem_t buf_full;// full
static sem_t buf_empty; // empty
static pthread_t th_in;

static void *
thread_listen_in(void * pointer)
{
	int c;
	do{
		printf("listen\n");
		c = getchar();
		if(sem_wait(&buf_empty) != 0)
			pthread_exit(NULL);
		iolet_data = (unsigned int)c;
		sem_post(&buf_full);
	}while(1);
}

void
iolet_initial(void)
{
	sem_init(&buf_full, 0, 0);
	sem_init(&buf_empty, 0, 1);
	pthread_create(&th_in, NULL, thread_listen_in, NULL);
}

void
iolet_clean(void)
{
	sem_destroy(&buf_full);
	sem_destroy(&buf_empty);
	pthread_cancel(th_in);
}

void
iolet_check_input(void)
{
	if(sem_trywait(&buf_full) == 0){
		signal_interrupt(ICU_IRQ_IOLET);
	}
}

void 
iolet_read_32(unsigned int address, unsigned int * data)
{
	int sval;
	
	*data = (unsigned int)iolet_data;
	
	sem_getvalue(&buf_empty, &sval);
	if(sval == 0)
		sem_post(&buf_empty);
	
	return;
}

void
iolet_read_16(unsigned int address, unsigned short * data)
{
	int sval;
	
	*data = (unsigned short)iolet_data;
	
	sem_getvalue(&buf_empty, &sval);
	if(sval == 0)
		sem_post(&buf_empty);
	
	return;
}

void
iolet_read_8(unsigned int address, unsigned char * data)
{
	int sval;
	
	*data = (unsigned char)iolet_data;
	
	sem_getvalue(&buf_empty, &sval);
	if(sval == 0)
		sem_post(&buf_empty);
	
	return;
}

void
iolet_write_32(unsigned int address, unsigned int data)
{
	printf("%u", data);
}

void
iolet_write_16(unsigned int address, unsigned short data)
{
	printf("%u", data);
}

void
iolet_write_8(unsigned int address, unsigned char data)
{
	printf("%c", data);
}

