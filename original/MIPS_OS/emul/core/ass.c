#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "ass.h"
#include "string.h"
#include "cpu.h"

static char **split_args(char *args, char div, int copy);
static int get_reg_id(const char *string, unsigned int *id);
static int get_value(char *buffer, unsigned int *value);
static int get_label_address(struct LABEL_LIST *labels, char *name,
							 unsigned int *address);

int
ass_assemble(char *asmcode, unsigned int *code, struct LABEL_LIST *labels,
			 unsigned int address)
{
	char *temp;
	char **argv;
	int id;

	unsigned int rs = 0;
	unsigned int rt = 0;
	unsigned int rd = 0;
	unsigned int imm = 0;
	unsigned int offset = 0;
	unsigned int target = 0;
	unsigned int sa = 0;
	unsigned int inst;

	temp = asmcode;

	while (*temp && (!isspace(*temp)))
		temp++;

	if (isspace(*temp))
		*temp = ',';
	argv = split_args(asmcode, ',', 0);
	id = 0;
	temp = argv[0];
	argv++;
	while (argv[id])
		id++;

	if (strcmp(temp, "add") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x20);

	} else if (strcmp(temp, "addi") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rs);
		id -= get_value(argv[2], &imm);
		inst = INST_JOIN_IMM(8, rs, rt, imm);

	} else if (strcmp(temp, "addu") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x21);

	} else if (strcmp(temp, "addiu") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rs);
		id -= get_value(argv[2], &imm);
		inst = INST_JOIN_IMM(9, rs, rt, imm);

	} else if (strcmp(temp, "and") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x24);

	} else if (strcmp(temp, "andi") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rs);
		id -= get_value(argv[2], &imm);
		inst = INST_JOIN_IMM(0xc, rs, rt, imm);

	} else if (strcmp(temp, "div") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_reg_id(argv[1], &rt);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x1a);

	} else if (strcmp(temp, "divu") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_reg_id(argv[1], &rt);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x1b);

	} else if (strcmp(temp, "mult") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_reg_id(argv[1], &rt);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x18);

	} else if (strcmp(temp, "multu") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_reg_id(argv[1], &rt);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x19);

	} else if (strcmp(temp, "nor") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x27);

	} else if (strcmp(temp, "or") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x25);

	} else if (strcmp(temp, "ori") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rs);
		id -= get_value(argv[2], &imm);
		inst = INST_JOIN_IMM(0xd, rs, rt, imm);

	} else if (strcmp(temp, "sll") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rt);
		id -= get_value(argv[2], &sa);
		inst = INST_JOIN(0, 0, rt, rd, sa, 0);

	} else if (strcmp(temp, "sllv") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rt);
		id -= get_reg_id(argv[2], &rs);
		inst = INST_JOIN(0, rs, rt, rd, 0, 4);

	} else if (strcmp(temp, "sra") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rt);
		id -= get_value(argv[2], &sa);
		inst = INST_JOIN(0, 0, rt, rd, sa, 3);

	} else if (strcmp(temp, "srav") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rt);
		id -= get_reg_id(argv[2], &rs);
		inst = INST_JOIN(0, rs, rt, rd, 0, 7);

	} else if (strcmp(temp, "srl") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rt);
		id -= get_value(argv[2], &sa);
		inst = INST_JOIN(0, 0, rt, rd, sa, 2);

	} else if (strcmp(temp, "srlv") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rt);
		id -= get_reg_id(argv[2], &rs);
		inst = INST_JOIN(0, rs, rt, rd, 0, 6);

	} else if (strcmp(temp, "sub") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x22);

	} else if (strcmp(temp, "subu") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x23);

	} else if (strcmp(temp, "xor") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x26);

	} else if (strcmp(temp, "xori") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rs);
		id -= get_value(argv[2], &imm);
		inst = INST_JOIN_IMM(0xE, rs, rt, imm);

	} else if (strcmp(temp, "lui") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_value(argv[1], &imm);
		inst = INST_JOIN_IMM(0xF, 0, rt, imm);

	} else if (strcmp(temp, "slt") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x2A);

	} else if (strcmp(temp, "slti") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rs);
		id -= get_value(argv[2], &imm);
		inst = INST_JOIN_IMM(0xA, rs, rt, imm);

	} else if (strcmp(temp, "sltu") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		id -= get_reg_id(argv[2], &rt);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x2B);

	} else if (strcmp(temp, "sltiu") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rs);
		id -= get_value(argv[2], &imm);
		inst = INST_JOIN_IMM(0xB, rs, rt, imm);

	} else if (strcmp(temp, "beq") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_reg_id(argv[1], &rt);
		id -= get_label_address(labels, argv[2], &offset);
		inst = INST_JOIN_IMM(4, rs, rt, ((offset - address - 4) >> 2));

	} else if (strcmp(temp, "bgez") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_label_address(labels, argv[1], &offset);
		inst = INST_JOIN_IMM(1, rs, 1, ((offset - address - 4) >> 2));

/*
	} else if (strcmp(temp, "bgezal") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_label_address(labels, argv[1], &offset);
		inst = INST_JOIN_IMM(1, rs, 0x11, ((offset - address - 4) >> 2));

*/
	} else if (strcmp(temp, "bgtz") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_label_address(labels, argv[1], &offset);
		inst = INST_JOIN_IMM(7, rs, 0, ((offset - address - 4) >> 2));

	} else if (strcmp(temp, "blez") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_label_address(labels, argv[1], &offset);
		inst = INST_JOIN_IMM(6, rs, 0, ((offset - address - 4) >> 2));
/*
	} else if (strcmp(temp, "bltzal") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_label_address(labels, argv[1], &offset);
		inst = INST_JOIN_IMM(1, rs, 0x10, ((offset - address - 4) >> 2));
*/
	} else if (strcmp(temp, "bltz") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_label_address(labels, argv[1], &offset);
		inst = INST_JOIN_IMM(1, rs, 0, ((offset - address - 4) >> 2));

	} else if (strcmp(temp, "bne") == 0) {
		if (id != 3)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		id -= get_reg_id(argv[1], &rt);
		id -= get_label_address(labels, argv[2], &offset);
		inst = INST_JOIN_IMM(5, rs, rt, ((offset - address - 4) >> 2));

	} else if (strcmp(temp, "j") == 0) {
		if (id != 1)
			id = 100;
		id -= get_label_address(labels, argv[0], &target);
		inst = 0x08000000 | ((target >> 2) & 0x03FFFFFF);

	} else if (strcmp(temp, "jal") == 0) {
		if (id != 1)
			id = 100;
		id -= get_label_address(labels, argv[0], &target);
		inst = 0x0C000000 | ((target >> 2) & 0x03FFFFFF);

/*
	} else if (strcmp(temp, "jalr") == 0) {
		if (id == 1) {
			id -= get_reg_id(argv[0], &rs);
			inst = INST_JOIN(0, rs, 0, 31, 0, 9);
		} else if (id == 2) {
			id -= get_reg_id(argv[0], &rd);
			id -= get_reg_id(argv[1], &rs);
			inst = INST_JOIN(0, rs, 0, rd, 0, 9);
		} else {
			id = 100;
		}
*/
	} else if (strcmp(temp, "jr") == 0) {
		if (id != 1)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		inst = INST_JOIN(0, rs, 0, 0, 0, 8);

/*
	} else if (strcmp(temp, "lb") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);
		inst = INST_JOIN_IMM(0x20, rs, rt, offset);
*/
	} else if (strcmp(temp, "lbu") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);
		inst = INST_JOIN_IMM(0x24, rs, rt, offset);
/*
	} else if (strcmp(temp, "lh") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);
		inst = INST_JOIN_IMM(0x21, rs, rt, offset);
*/

	} else if (strcmp(temp, "lhu") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);
		inst = INST_JOIN_IMM(0x25, rs, rt, offset);

	} else if (strcmp(temp, "lw") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);
		inst = INST_JOIN_IMM(0x23, rs, rt, offset);

	} else if (strcmp(temp, "sb") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);
		inst = INST_JOIN_IMM(0x28, rs, rt, offset);

	} else if (strcmp(temp, "sh") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);
		inst = INST_JOIN_IMM(0x29, rs, rt, offset);

	} else if (strcmp(temp, "sw") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		if (argv[1][0] == 'x') {
			sscanf(argv[1] + 1, "%x", &offset);
		} else {
			sscanf(argv[1], "%d", &offset);
		}
		id -= get_reg_id(strchr(argv[1], '$') + 1, &rs);

		inst = INST_JOIN_IMM(0x2B, rs, rt, offset);
	} else if (strcmp(temp, "break") == 0) {
		inst = INST_JOIN(0, 0, 0, 0, 0, 0xD);
	} else if (strcmp(temp, "syscall") == 0) {
		inst = INST_JOIN(0, 0, 0, 0, 0, 0xC);
	} else if (strcmp(temp, "mfhi") == 0) {
		if (id != 1)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		inst = INST_JOIN(0, 0, 0, rd, 0, 0x10);

	} else if (strcmp(temp, "mflo") == 0) {
		if (id != 1)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		inst = INST_JOIN(0, 0, 0, rd, 0, 0x12);

/*
	} else if (strcmp(temp, "mthi") == 0) {
		if (id != 1)
			id = 100;
		id -= get_reg_id(argv[0], &rs);
		inst = INST_JOIN(0, rs, 0, 0, 0, 0x11);

	} else if (strcmp(temp, "mtlo") == 0) {

		if (id != 1) {

			id = 100;
		}
		id -= get_reg_id(argv[0], &rs);
		inst = INST_JOIN(0, rs, 0, 0, 0, 0x13);
*/
	} else if (strcmp(temp, "mfc0") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rd);
		inst = INST_JOIN(0x10, 0, rt, rd, 0, 0);

	} else if (strcmp(temp, "mtc0") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rt);
		id -= get_reg_id(argv[1], &rd);
		inst = INST_JOIN(0x10, 4, rt, rd, 0, 0);

	} else if (strcmp(temp, "eretx") == 0) {
		inst = 0x42000018;
	} else if (strcmp(temp, "b") == 0) {
		if (id != 1)
			id = 100;
		id -= get_label_address(labels, argv[0], &offset);
		inst = INST_JOIN_IMM(4, 0, 0, ((offset - address - 4) >> 2));

	} else if (strcmp(temp, "bal") == 0) {
		if (id != 1)
			id = 100;
		id -= get_label_address(labels, argv[0], &offset);
		inst = INST_JOIN_IMM(1, 0, 0x11, ((offset - address - 4) >> 2));

	} else if (strcmp(temp, "nop") == 0) {
		inst = 0;
	} else if (strcmp(temp, "move") == 0) {
		if (id != 2)
			id = 100;
		id -= get_reg_id(argv[0], &rd);
		id -= get_reg_id(argv[1], &rs);
		inst = INST_JOIN(0, rs, 0, rd, 0, 0x25);
	} else {
		id = 100;
	}
	if (id == 0)
		*code = inst;
	return id;
}

/* convert  "$a $b, $c, ...." to  "$a", "$b", "$c"... */
static char **
split_args(char *args, char div, int copy)
{
	char **argv;
	int argc = 0;
	char *temp;

	argv = (char **) malloc(sizeof (char *) * 32);

	/* get rid of comment. */
	temp = strchr(args, '#');
	if (temp)
		*temp = 0;
	temp = args;

	argv[0] = temp;
	argc++;

	while (*temp) {
		if (*temp == div) {
			*temp = 0;
			temp++;
			argv[argc] = temp;
			argc++;
		} else {
			temp++;
		}
	}

	argv[argc] = NULL;

	for (argc = 0; argv[argc]; argc++) {
		argv[argc] = trim(argv[argc]);
		if (copy) {
			temp = malloc(strlen(argv[argc]) + 1);
			strcpy(temp, argv[argc]);
			argv[argc] = temp;
		}
	}
	return argv;
}

static int
get_reg_id(const char *string, unsigned int *id)
{
	int flag = 1;

	if (*(string) != '$')
		flag = 0;

	string++;
	if (strcmp(string, "zero") == 0)
		*id = 0;
	else if (strncmp(string, "at", 2) == 0)
		*id = 1;
	else if (strncmp(string, "v0", 2) == 0)
		*id = 2;
	else if (strncmp(string, "v1", 2) == 0)
		*id = 3;
	else if (strncmp(string, "a0", 2) == 0)
		*id = 4;
	else if (strncmp(string, "a1", 2) == 0)
		*id = 5;
	else if (strncmp(string, "a2", 2) == 0)
		*id = 6;
	else if (strncmp(string, "a3", 2) == 0)
		*id = 7;
	else if (strncmp(string, "t0", 2) == 0)
		*id = 8;
	else if (strncmp(string, "t1", 2) == 0)
		*id = 9;
	else if (strncmp(string, "t2", 2) == 0)
		*id = 10;
	else if (strncmp(string, "t3", 2) == 0)
		*id = 11;
	else if (strncmp(string, "t4", 2) == 0)
		*id = 12;
	else if (strncmp(string, "t5", 2) == 0)
		*id = 13;
	else if (strncmp(string, "t6", 2) == 0)
		*id = 14;
	else if (strncmp(string, "t7", 2) == 0)
		*id = 15;
	else if (strncmp(string, "s0", 2) == 0)
		*id = 16;
	else if (strncmp(string, "s1", 2) == 0)
		*id = 17;
	else if (strncmp(string, "s2", 2) == 0)
		*id = 18;
	else if (strncmp(string, "s3", 2) == 0)
		*id = 19;
	else if (strncmp(string, "s4", 2) == 0)
		*id = 20;
	else if (strncmp(string, "s5", 2) == 0)
		*id = 21;
	else if (strncmp(string, "s6", 2) == 0)
		*id = 22;
	else if (strncmp(string, "s7", 2) == 0)
		*id = 23;
	else if (strncmp(string, "t8", 2) == 0)
		*id = 24;
	else if (strncmp(string, "t9", 2) == 0)
		*id = 25;
	else if (strncmp(string, "k0", 2) == 0)
		*id = 26;
	else if (strncmp(string, "k1", 2) == 0)
		*id = 27;
	else if (strncmp(string, "gp", 2) == 0)
		*id = 28;
	else if (strncmp(string, "sp", 2) == 0)
		*id = 29;
	else if (strncmp(string, "fp", 2) == 0)
		*id = 30;
	else if (strncmp(string, "ra", 2) == 0)
		*id = 31;
	else
		flag = sscanf(string, "%d", id);

	return flag;
}

static int
get_value(char *buffer, unsigned int *value)
{
	int flag = 0;
	if (buffer[0] == '0') {
		buffer++;
		if (buffer[0] == 'x') {	/* 0x{0123456789abcdef} */
			flag = sscanf(buffer + 1, "%x", value);
		} else {				/* 0{01234567} */
			flag = sscanf(buffer, "%o", value);
		}
	} else {					/* [-]{01234567890} */
		flag = sscanf(buffer, "%d", value);
	}
	return flag;
}

static int
get_label_address(struct LABEL_LIST *labels, char *name, unsigned int *address)
{
	if (get_value(name, address))
		return 1;

	while (labels) {
		if (strcmp(labels->name, name) == 0) {
			*address = labels->address;
			return 1;
		}
		labels = labels->next;
	}
	return 0;
}

/*
int
main(){
	char in[123];
	char ** argv;
	unsigned int code;
	
	int t;
	while(*fgets(in, 123, stdin) != '^'){
		ass_assemble(in,&code,0,0);
		printf("%x\n", code);
		free(argv);
	}
	return 0;
}*/
