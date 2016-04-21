/*
 * vm.c
 * by wuxb
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "address.h"
#include "memory.h"
#include "flash.h"
#include "vm.h"
#include "cp0.h"
#include "timer.h"
#include "icu.h"

extern int cpu_big_endian;

static int cycle_count = 1000;

static void reset_all(int argc, char **argv);

static void
reset_all(int argc, char **argv)
{
	int opt;
	unsigned int memory_mb = 0x08000000;
	char *flash_path = NULL;
	int writeback = 0;
	
	while ((opt = getopt(argc, argv, "m:f:e:c:w")) != -1) {
		switch (opt) {
		case 'm':				// mem
			sscanf(optarg, "%d", &memory_mb);
			break;
		case 'f':				// flash
			flash_path = malloc(strlen(optarg) + 1);
			strcpy(flash_path, optarg);
			break;
		case 'e':				// endian
			if (strcmp(optarg, "big") == 0)
				cpu_big_endian = 1;
			break;
		case 'c':				// cycles
			sscanf(optarg, "%d", &cycle_count);
			break;
		case 'w':
			writeback = 1;
			break;
		}
	}
	cpu_reset();
	cp0_reset();
	icu_reset();
	timer_reset();
	flash_load(flash_path, writeback);
	memory_init(memory_mb);
	iolet_initial();
}

int
main(int argc, char **argv)
{
	reset_all(argc, argv);

	while (cycle_count > 16) {
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();

		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		execute_cycle();
		cycle_count -= 16;
	}
	while (cycle_count) {
		execute_cycle();
		cycle_count--;
	}
	putchar('\n');
	iolet_clean();
	flash_unload();
	return 0;
}
