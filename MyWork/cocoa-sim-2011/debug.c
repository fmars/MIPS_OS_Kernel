/*
 * debug.c
 * debug the vm.
 * by wuxb.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "debug.h"
#include "cpu.h"
#include "cp0.h"
#include "address.h"
#include "flash.h"
#include "timer.h"
#include "icu.h"
#include "memory.h"
#include "disass.h"
#include "ass.h"
#include "string.h"

#define WELCOME_INFO \
"Welcome to wuxb local debuger !\n\
\tcommand:[bdcsxrqhemvl], TYPE 'h' for help.\n"
/* abcde   h   lm    qrs   v x   */
/* abcdefg hijklmn opqrst uvwxyz */

#define HELP_INFO_B \
">> b : show breakpoints\n\
   b <hex address> : set breakpoint at <hex address>."

#define HELP_INFO_D \
">> d <hex address> : delete the breakpoint in <hex address>."

#define HELP_INFO_C \
">> c <dec integer> : continue execute at most <dec integer> instructions."

#define HELP_INFO_S \
">> s : step (in) one instruction."

#define HELP_INFO_X \
">> x : reset simulator."

#define HELP_INFO_R \
">> r : show registers of CPU, CP0, ICU, TIMER."

#define HELP_INFO_Q \
">> q : quit simulator."

#define HELP_INFO_H \
">> h : see this infomation."

#define HELP_INFO_E \
">> e [bl] : b(big) or l(little) endian."

#define HELP_INFO_M \
">> m <dec number> : set memory size (MB)."

#define HELP_INFO_V \
">> v : show emulator variables."

#define HELP_INFO_L \
">> l : list disassembly * 16.\n\
   l <hex address> : list from <hex address>."

#define HELP_INFO_A \
">> a <asm code> : write one asmcode @ cpu_pc.\n\
   A [<hex address>] : write code from cpu_pc or <hex address>."

extern unsigned int cpu_pc;
extern int cpu_big_endian;
extern unsigned int memory_size;
extern unsigned int flash_size;

static char* preset_flash_path = NULL;
static unsigned int preset_memory_size = 0x20000000u;
static int preset_flash_writeback = 0;
static struct BP_LIST *bp_head = NULL;

static void reset_all(void);
static void clean_all(void);
static void process_continue(int max_exec);
static void process_inst(void);
static void process_command(char *buffer);
static int find_break_point(unsigned int bpc);
static void add_break_point(unsigned int bpc);
static void delete_break_point(unsigned int bpc);
static void print_break_points(void);
static void assembly_loop(unsigned int address);
static void show_help(void);

static void
reset_all()
{
	cpu_reset();
	cp0_reset();
	icu_reset();
	timer_reset();
	flash_load(preset_flash_path, preset_flash_writeback);
	show_cpu_regs();
	memory_init(preset_memory_size);	/* 128MB. */
}

static void
clean_all()
{
	memory_free();
	flash_unload();
	delete_break_point(0);
}

static void
process_continue(int max_exec)
{
	printf("\n");
	while (max_exec) {
		execute_cycle();
		//process_inst();
		if (find_break_point(cpu_pc)) {
			break;
		}
		max_exec--;
	}
	printf("\n");
	putchar(10);
	//show_cpu_written_reg();
	show_cpu_regs();

}

static void
process_inst(void)
{
	execute_cycle();
	putchar(10);
	//show_cpu_written_reg();
	show_cpu_regs();
}

static void
process_command(char *buffer)
{
	char *arg;
	static unsigned int address = 0xbfc00000;
	int count;
	unsigned int temp;
	unsigned int inst;

	arg = buffer + 1;

	while (arg && *arg == ' ') {
		arg++;
	}

	switch (buffer[0]) {

		/* break <address> */
	case 'b':
		if (arg == NULL || *arg == 0) {
			print_break_points();
		} else {
			/* hex number only. */
			if (sscanf(arg, "%x", &address))
				add_break_point(address);
		}
		break;

		/* delete breakpoint */
	case 'd':
		if (arg == NULL || *arg == 0) {
			printf("Which break point do you want to delete?\n");
		} else {
			if (sscanf(arg, "%x", &address))
				delete_break_point(address);
		}
		break;

		/* continue [inst to stop] */
	case 'c':
		count = 256;
		/* default run 100 insts */
		if (arg && (*arg != 0)) {
			sscanf(arg, "%d", &count);
		}
		process_continue(count);
		address = cpu_pc & 0xFFFFFFFC;
		break;

		/* step */
	case 's':
		process_inst();
		address = cpu_pc & 0xFFFFFFFC;
		break;

		/* reset */
	case 'x':
		reset_all();
		address = cpu_pc & 0xFFFFFFFC;
		break;

		/* registers */
	case 'r':
		show_cpu_regs();
		show_cp0_regs();
		show_icu_regs();
		show_timer_regs();
		break;

		/* quit. */
	case 'q':
		clean_all();
		putchar('\n');
		
		exit(0);
		break;

		/* help infomation. */
	case 'h':
		show_help();
		break;

		/* endian. */
	case 'e':
		if (buffer[2] == 'b')
			cpu_big_endian = 1;
		else if (buffer[2] == 'l')
			cpu_big_endian = 0;
		else
			printf("?\n");
		break;

		/* memory size. */
	case 'm':
		if (sscanf(arg, "%d", &address)) {
			memory_init(address << 20);
		}
		break;

		/* ... */
	case 'v':
		printf("memory size: %d MB\n", memory_size >> 20);
		printf("flash size: %d MB\n", flash_size >> 20);
		break;

		/* list 16 * disass */
	case 'l':
		if (arg && (*arg != 0)) {
			sscanf(arg, "%x", &address);
		}

		for (count = 0; count < 16; count++) {
			if (address_read_32(address + (count << 2), &temp)) {
				printf("%08x Bad address.\n", address + (count << 2));
			} else {
				printf("[%08x][%08x] ", address + (count << 2), temp);
				disassemble(temp);
			}
		}
		address += 64;
		break;

		/* Assemble one instruction at cpu_pc. */
	case 'a':
		if (!*arg)
			break;
		if (isdigit(*arg)) {
			if (*arg == '0') {
				arg++;
				if (*arg == 'x') {
					temp = sscanf(arg + 1, "%x", &inst);
				} else {
					temp = sscanf(arg, "%o", &inst);
				}
			} else {
				temp = sscanf(arg, "%d", &inst);
			}

			if (temp == 0 || address_write_32(cpu_pc, inst))
				printf("error write\n");

		} else if (ass_assemble(trim(arg), &inst, NULL, cpu_pc) == 0) {
			if (address_write_32(cpu_pc, inst))
				printf("error write\n");
		} else {
			printf("error ass\n");
		}
		break;

		/* begin assemble at given address or cpu_pc. */
	case 'A':
		if (sscanf(arg, "%x", &address)) {
			assembly_loop(address);
		} else {
			assembly_loop(cpu_pc);
		}
		break;

	default:
		show_help();
		break;
	}
	return;
}

static int
find_break_point(unsigned int bpc)
{
	struct BP_LIST *temp = bp_head;
	bpc &= 0xfffffffc;
	while (temp) {
		if (bpc == (temp->break_addr))
			return 1;
		temp = temp->next;
	}
	return 0;
}

static void
add_break_point(unsigned int bpc)
{
	struct BP_LIST *temp;
	bpc &= 0xfffffffc;
	temp = (struct BP_LIST *) malloc(sizeof (struct BP_LIST));
	temp->next = bp_head;
	temp->break_addr = bpc;
	bp_head = temp;
}

static void
delete_break_point(unsigned int bpc)
{
	struct BP_LIST *temp;
	struct BP_LIST *tempd;
	temp = bp_head;
	bpc &= 0xfffffffc;
	while (temp) {
		/* 0 means delete all breakpoints. */
		if (bpc == 0 || temp->break_addr == bpc) {
			tempd = temp;
			temp = temp->next;
			free(tempd);
			if (tempd == bp_head)
				bp_head = NULL;
		} else {
			temp = temp->next;
		}
	}
}

static void
print_break_points()
{
	struct BP_LIST *temp;
	temp = bp_head;
	while (temp) {
		printf("Break point at : %x\n", temp->break_addr);
		temp = temp->next;
	}
}

static void
assembly_loop(unsigned int address)
{
	char buffer[128];
	int temp;
	unsigned int inst;

	buffer[127] = 0;
	for (;;) {
		printf("+XASS@[%08x] : ", address);
		fgets(buffer, 127, stdin);
		temp = strlen(buffer) - 1;
		if (temp >= 0 && buffer[temp] == '\n') {
			buffer[temp] = 0;
		}

		if (buffer[0] == 0) {
			break;
		}

		if (isdigit(buffer[0])) {
			if (buffer[0] == '0') {
				if (buffer[1] == 'x') {
					temp = sscanf(buffer + 2, "%x", &inst);
				} else {
					temp = sscanf(buffer + 1, "%o", &inst);
				}
			} else {
				temp = sscanf(buffer, "%d", &inst);
			}

			if (temp == 0 || address_write_32(address, inst)) {
				printf("error write\n");
				break;
			} else {
				address += 4;
			}
		} else if (ass_assemble(trim(buffer), &inst, NULL, address) == 0) {
			if (address_write_32(address, inst)) {
				printf("error ass\n");
				break;
			} else {
				address += 4;
			}
		}
	}
}

static void
show_help()
{
	puts(HELP_INFO_B);
	puts(HELP_INFO_D);
	puts(HELP_INFO_C);
	puts(HELP_INFO_S);
	puts(HELP_INFO_X);
	puts(HELP_INFO_R);
	puts(HELP_INFO_Q);
	puts(HELP_INFO_H);
	puts(HELP_INFO_E);
	puts(HELP_INFO_M);
	puts(HELP_INFO_V);
	puts(HELP_INFO_L);
	puts(HELP_INFO_A);
}

int
main(int argc, char **argv)
{
	char buffer1[128] = { 0 };
	char buffer2[128] = { 0 };
	char *buffer[2] = { buffer1, buffer2 };
	int bid = 0;
	int t1;
	int opt;

	while ((opt = getopt(argc, argv, "m:f:e:w")) != -1) {
		switch (opt) {
		case 'm':				// mem
			sscanf(optarg, "%d", &preset_memory_size);
			break;
		case 'f':				// flash
			preset_flash_path = malloc(strlen(optarg) + 1);
			strcpy(preset_flash_path, optarg);
			break;
		case 'e':				// endian
			if (strcmp(optarg, "big") == 0)
				cpu_big_endian = 1;
			break;
		case 'w':
			preset_flash_writeback = 1;
			break;
		}
	}

	printf(WELCOME_INFO);
	reset_all();
	//printf("flash: %x\n", flash_size);
	if (!flash_ok()) {
		exit(0);
	}
	/* main loop of debuger. */
	for (;;) {
		printf("+---XDB >> ");
		if (!fgets(buffer[bid], 127, stdin))
			return 0;
		t1 = strlen(buffer[bid]) - 1;
		if (t1 >= 0 && buffer[bid][t1] == '\n') {
			buffer[bid][t1] = 0;
		}

		if (buffer[bid][0] == 0) {
			/* empty line, process last operation. */
			process_command(buffer[1 - bid]);
		} else {
			/* new command. */
			process_command(buffer[bid]);
			bid = 1 - bid;
		}
	}
	return 0;
}
