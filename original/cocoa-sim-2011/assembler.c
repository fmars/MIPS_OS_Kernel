/*
 * ass.c
 * by wuxb
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string.h"
#include "disass.h"

#define INST_MASK_31_26      0xFC000000
#define INST_MASK_25_21      0x03E00000
#define INST_MASK_20_16      0x001F0000
#define INST_MASK_15_11      0x0000F800
#define INST_MASK_10_6       0x000007C0
#define INST_MASK_5_0        0x0000003F
#define INST_MASK_IMM        0x0000FFFF

#define SEGMENT_DATA         0xbfa00000
#define SEGMENT_TEXT         0xbfc00000

#define INST_JOIN( f1 , f2 , f3 , f4 , f5 , f6 ) \
((( f1 << 26 )& INST_MASK_31_26 ) | \
(( f2 << 21 ) & INST_MASK_25_21 ) | \
(( f3 << 16 ) & INST_MASK_20_16 ) | \
(( f4 << 11 ) & INST_MASK_15_11 ) | \
(( f5 << 6 ) & INST_MASK_10_6 ) | ( f6 & INST_MASK_5_0 ))

#define INST_JOIN_IMM(f1,f2,f3,imm) \
(((f1 << 26)& INST_MASK_31_26 ) | \
((f2 << 21) & INST_MASK_25_21 ) | \
((f3 << 16) & INST_MASK_20_16 ) | \
(imm & INST_MASK_IMM))

struct LABEL_PAIR_LIST {
	unsigned int address;
	char name[16];
	struct LABEL_PAIR_LIST *next;
};

int ADDRESS_BIGENDIAN = 0;

static int count = SEGMENT_TEXT;
static FILE *out = NULL;
static FILE *outmap = NULL;
static struct LABEL_PAIR_LIST *lhead = NULL;
static struct LABEL_PAIR_LIST *ltemp = NULL;
static unsigned int segment_data = SEGMENT_DATA;
static unsigned int segment_text = SEGMENT_TEXT;

static unsigned int get_label_addr(char *label);
static void gen_label_address(char *buffer);
static int get_value(char *buffer);
static void process_line(char *buffer);
static unsigned int get_reg_id(const char *string);
static void inst_error(int count);

static unsigned int
get_reg_id(const char *string)
{

	int rt = 0;

	if (*(string - 1) != '$')
		printf("warning, need '$' [%08x]\n", count);
	if (strcmp(string, "zero") == 0) {
		return 0;
	} else if (strncmp(string, "at", 2) == 0) {
		return 1;
	} else if (strncmp(string, "v0", 2) == 0) {
		return 2;
	} else if (strncmp(string, "v1", 2) == 0) {
		return 3;
	} else if (strncmp(string, "a0", 2) == 0) {
		return 4;
	} else if (strncmp(string, "a1", 2) == 0) {
		return 5;
	} else if (strncmp(string, "a2", 2) == 0) {
		return 6;
	} else if (strncmp(string, "a3", 2) == 0) {
		return 7;
	} else if (strncmp(string, "t0", 2) == 0) {
		return 8;
	} else if (strncmp(string, "t1", 2) == 0) {
		return 9;
	} else if (strncmp(string, "t2", 2) == 0) {
		return 10;
	} else if (strncmp(string, "t3", 2) == 0) {
		return 11;
	} else if (strncmp(string, "t4", 2) == 0) {
		return 12;
	} else if (strncmp(string, "t5", 2) == 0) {
		return 13;
	} else if (strncmp(string, "t6", 2) == 0) {
		return 14;
	} else if (strncmp(string, "t7", 2) == 0) {
		return 15;
	} else if (strncmp(string, "s0", 2) == 0) {
		return 16;
	} else if (strncmp(string, "s1", 2) == 0) {
		return 17;
	} else if (strncmp(string, "s2", 2) == 0) {
		return 18;
	} else if (strncmp(string, "s3", 2) == 0) {
		return 19;
	} else if (strncmp(string, "s4", 2) == 0) {
		return 20;
	} else if (strncmp(string, "s5", 2) == 0) {
		return 21;
	} else if (strncmp(string, "s6", 2) == 0) {
		return 22;
	} else if (strncmp(string, "s7", 2) == 0) {
		return 23;
	} else if (strncmp(string, "t8", 2) == 0) {
		return 24;
	} else if (strncmp(string, "t9", 2) == 0) {
		return 25;
	} else if (strncmp(string, "k0", 2) == 0) {
		return 26;
	} else if (strncmp(string, "k1", 2) == 0) {
		return 27;
	} else if (strncmp(string, "gp", 2) == 0) {
		return 28;
	} else if (strncmp(string, "sp", 2) == 0) {
		return 29;
	} else if (strncmp(string, "fp", 2) == 0) {
		return 30;
	} else if (strncmp(string, "ra", 2) == 0) {
		return 31;
	} else {					/* try number */
		if (sscanf(string, "%d", &rt) != 1) {
			printf("! Bad register name : %s\n", string);
			exit(0);
		}
		return rt;
	}
}

static void
inst_error(int count)
{
	printf("Bad inst at: %08x\n", count);
	exit(1);
}

static unsigned int
get_label_addr(char *label)
{
	struct LABEL_PAIR_LIST *temp = lhead;
	while (temp) {
		if (strcmp(temp->name, label) == 0) {
			return temp->address;
		}
		temp = temp->next;
	}
	printf("error ! label [%s] not find!!\n", label);
	exit(0);
	return 0;
}

static void
gen_label_address(char *buffer)
{
	char *chr;
	if (!buffer[0]) {
		return;
	}
	chr = strchr(buffer, ':');
	if (chr) {
		/* increase list. */
		if (ltemp == NULL) {
			ltemp = (struct LABEL_PAIR_LIST *)
				malloc(sizeof (struct LABEL_PAIR_LIST));
			lhead = ltemp;
		} else {
			ltemp->next = (struct LABEL_PAIR_LIST *)
				malloc(sizeof (struct LABEL_PAIR_LIST));
			ltemp = ltemp->next;
		}
		ltemp->next = NULL;
		ltemp->address = count;
		*chr = 0;
		strcpy(ltemp->name, buffer);

		chr++;
		if (*chr) {
			count++;
			while (*chr) {
				if ((*chr) == ',') {
					count += 4;
				}
				chr++;
			}
		}
	} else {
		/* char and not have ':', it is a instruction. */
		count += 4;
	}
}

static int
get_value(char *buffer)
{
	int value = 0;
	if (buffer[0] == 'x') {		/* hex number */
		sscanf(buffer + 1, "%x", &value);
	} else if (buffer[0] == '-') {
		sscanf(buffer + 1, "%d", &value);
		value = -value;
	} else {
		if (sscanf(buffer, "%d", &value) == 0) {
			printf("need value\n");
			exit(1);
		}
	}
	return value;
}

static void
process_line(char *const buffer)
{
	char buf_op[32] = { 0 };
	char buf_src[4][32] = { {0} };
	int rs = 0;
	int rt = 0;
	int rd = 0;
	int imm = 0;
	int offset = 0;
	int target = 0;
	int sa = 0;
	int i = 0;
	int j = 0;
	int id = 0;
	char *chr = NULL;
	int flag = 0;
	unsigned int data = 0;
	char buf_data[32] = { 0 };
	unsigned int inst = -1;

	if (buffer[0] == 0)
		return;

	/* skip first blank. */
	chr = strchr(buffer, ':');

	if (!chr) {					/* inst.., split opcode and oprands */
		while (buffer[i]) {
			if (buffer[i] == ' ')
				break;
			buf_op[i] = buffer[i];
			i++;
		}
		while (buffer[i] == ' ')
			i++;

		j = 0;
		flag = 0;
		while (buffer[i]) {
			if (buffer[i] == ',') {
				if (flag)
					id++;
				flag = 0;
				if (id == 4)
					break;
				j = 0;
				i++;
				continue;
			}
			if (buffer[i] != ' ') {
				flag++;
				buf_src[id][j] = buffer[i];
				j++;
			}
			i++;
		}
		if (flag)
			id++;

	} else {					/* has ':', try get dec number. */
		chr++;
		i = 0;
		fprintf(outmap, "LABEL[%08x] %s\n", count, buffer);
		if (!(*chr)) {			/* no data */
			return;
		}
		while (*chr) {
			switch (*chr) {
			case ' ':
				break;
			case ',':
				buf_data[i] = 0;
				data = atoi(buf_data);
				printf("DATA [%08x] %x\n", count, data);

				fprintf(outmap, "DATA [%08x] : %08x\n", count, data);
				fwrite(&data, 4, 1, out);
				count += 4;
				i = 0;
				break;
			default:
				buf_data[i] = (*chr);
				i++;
				break;
			}
			chr++;
		}
		buf_data[i] = 0;
		data = atoi(buf_data);
		printf("DATA [%08x] %x\n", count, data);
		fprintf(outmap, "DATA [%08x] : %08x\n", count, data);
		fwrite(&data, 4, 1, out);
		count += 4;
		return;
	}

	if (strcmp(buf_op, "add") == 0) {
		//printf("gen add\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x20);

	} else if (strcmp(buf_op, "addi") == 0) {
		//printf("gen addi\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		imm = get_value(&buf_src[2][0]);
		inst = INST_JOIN_IMM(8, rs, rt, imm);

	} else if (strcmp(buf_op, "addu") == 0) {
		//printf("gen addu\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x21);

	} else if (strcmp(buf_op, "addiu") == 0) {
		//printf("gen addiu\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		imm = get_value(&buf_src[2][0]);
		inst = INST_JOIN_IMM(9, rs, rt, imm);

	} else if (strcmp(buf_op, "and") == 0) {
		//printf("gen and\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x24);

	} else if (strcmp(buf_op, "andi") == 0) {
		//printf("gen andi\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		imm = get_value(&buf_src[2][0]);
		inst = INST_JOIN_IMM(0xc, rs, rt, imm);

	} else if (strcmp(buf_op, "div") == 0) {
		//printf("gen div\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x1a);

	} else if (strcmp(buf_op, "divu") == 0) {
		//printf("gen divu\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x1b);

	} else if (strcmp(buf_op, "mult") == 0) {
		//printf("gen mult\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x18);

	} else if (strcmp(buf_op, "multu") == 0) {
		//printf("gen multu\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		inst = INST_JOIN(0, rs, rt, 0, 0, 0x19);

	} else if (strcmp(buf_op, "nor") == 0) {
		//printf("gen nor\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x27);

	} else if (strcmp(buf_op, "or") == 0) {
		//printf("gen or\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x25);

	} else if (strcmp(buf_op, "ori") == 0) {
		//printf("gen ori\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		imm = get_value(&buf_src[2][0]);
		inst = INST_JOIN_IMM(0xd, rs, rt, imm);

	} else if (strcmp(buf_op, "sll") == 0) {
		//printf("gen sll\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		sa = get_value(&buf_src[2][0]);
		inst = INST_JOIN(0, 0, rt, rd, sa, 0);

	} else if (strcmp(buf_op, "sllv") == 0) {
		//printf("gen sllv\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		rs = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 4);

	} else if (strcmp(buf_op, "sra") == 0) {
		//printf("gen sra\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		sa = get_value(&buf_src[2][0]);
		inst = INST_JOIN(0, 0, rt, rd, sa, 3);

	} else if (strcmp(buf_op, "srav") == 0) {
		//printf("gen srav\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		rs = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 7);

	} else if (strcmp(buf_op, "srl") == 0) {
		//printf("gen srl\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		sa = get_value(&buf_src[2][0]);
		inst = INST_JOIN(0, 0, rt, rd, sa, 2);

	} else if (strcmp(buf_op, "srlv") == 0) {
		//printf("gen srlv\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		rs = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 6);

	} else if (strcmp(buf_op, "sub") == 0) {
		//printf("gen sub\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x22);

	} else if (strcmp(buf_op, "subu") == 0) {
		//printf("gen subu\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x23);

	} else if (strcmp(buf_op, "xor") == 0) {
		//printf("gen xor\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x26);

	} else if (strcmp(buf_op, "xori") == 0) {
		//printf("gen xori\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		imm = get_value(&buf_src[2][0]);
		inst = INST_JOIN_IMM(0xE, rs, rt, imm);

	} else if (strcmp(buf_op, "lui") == 0) {
		//printf("gen lui\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		imm = get_value(&buf_src[1][0]);
		inst = INST_JOIN_IMM(0xF, 0, rt, imm);

	} else if (strcmp(buf_op, "slt") == 0) {
		//printf("gen slt\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x2A);

	} else if (strcmp(buf_op, "slti") == 0) {
		//printf("gen slti\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		imm = get_value(&buf_src[2][0]);
		inst = INST_JOIN_IMM(0xA, rs, rt, imm);

	} else if (strcmp(buf_op, "sltu") == 0) {
		//printf("gen sltu\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		rt = get_reg_id(&buf_src[2][1]);
		inst = INST_JOIN(0, rs, rt, rd, 0, 0x2B);

	} else if (strcmp(buf_op, "sltiu") == 0) {
		//printf("gen sltiu\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		imm = get_value(&buf_src[2][0]);
		inst = INST_JOIN_IMM(0xB, rs, rt, imm);

	} else if (strcmp(buf_op, "beq") == 0) {
		//printf("gen beq\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		offset = get_label_addr(buf_src[2]);
		inst = INST_JOIN_IMM(4, rs, rt, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "bgez") == 0) {
		//printf("gen bgez\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		offset = get_label_addr(buf_src[1]);
		inst = INST_JOIN_IMM(1, rs, 1, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "bgezal") == 0) {
		//printf("gen bgezal\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		offset = get_label_addr(buf_src[1]);
		inst = INST_JOIN_IMM(1, rs, 0x11, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "bgtz") == 0) {
		//inst_error("gen bgtz\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		offset = get_label_addr(buf_src[1]);
		inst = INST_JOIN_IMM(7, rs, 0, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "blez") == 0) {
		//printf("gen blez\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		offset = get_label_addr(buf_src[1]);
		inst = INST_JOIN_IMM(6, rs, 0, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "bltzal") == 0) {
		//printf("gen bltzal\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		offset = get_label_addr(buf_src[1]);
		inst = INST_JOIN_IMM(1, rs, 0x10, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "bltz") == 0) {
		//printf("gen bltz\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		offset = get_label_addr(buf_src[1]);
		inst = INST_JOIN_IMM(1, rs, 0, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "bne") == 0) {
		//printf("gen bne\n");
		if (id != 3) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		rt = get_reg_id(&buf_src[1][1]);
		offset = get_label_addr(buf_src[2]);
		inst = INST_JOIN_IMM(5, rs, rt, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "j") == 0) {
		//printf("gen j\n");
		if (id != 1) {
			inst_error(count);
			return;
		}
		target = get_label_addr(buf_src[0]);
		inst = 0x08000000 | ((target >> 2) & 0x03FFFFFF);

	} else if (strcmp(buf_op, "jal") == 0) {
		//printf("gen jal\n");
		if (id != 1) {
			inst_error(count);
			return;
		}
		target = get_label_addr(buf_src[0]);
		inst = 0x0C000000 | ((target >> 2) & 0x03FFFFFF);

	} else if (strcmp(buf_op, "jalr") == 0) {
		//printf("gen jalr\n");
		if (id == 1) {
			rs = get_reg_id(&buf_src[0][1]);
			inst = INST_JOIN(0, rs, 0, 31, 0, 9);
		} else if (id == 2) {
			rd = get_reg_id(&buf_src[0][1]);
			rs = get_reg_id(&buf_src[1][1]);
			inst = INST_JOIN(0, rs, 0, rd, 0, 9);
		} else {
			inst_error(count);
			return;
		}
	} else if (strcmp(buf_op, "jr") == 0) {
		//printf("gen jr\n");
		if (id != 1) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		inst = INST_JOIN(0, rs, 0, 0, 0, 8);

	} else if (strcmp(buf_op, "lb") == 0) {
		//printf("gen lb\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);
		inst = INST_JOIN_IMM(0x20, rs, rt, offset);

	} else if (strcmp(buf_op, "lbu") == 0) {
		//printf("gen lbu\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);
		inst = INST_JOIN_IMM(0x24, rs, rt, offset);

	} else if (strcmp(buf_op, "lh") == 0) {
		//printf("gen lh\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);
		inst = INST_JOIN_IMM(0x21, rs, rt, offset);

	} else if (strcmp(buf_op, "lhu") == 0) {
		//printf("gen lhu\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);
		inst = INST_JOIN_IMM(0x25, rs, rt, offset);

	} else if (strcmp(buf_op, "lw") == 0) {
		//printf("gen lw\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);
		inst = INST_JOIN_IMM(0x23, rs, rt, offset);

	} else if (strcmp(buf_op, "sb") == 0) {
		//printf("gen sb\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);
		inst = INST_JOIN_IMM(0x28, rs, rt, offset);

	} else if (strcmp(buf_op, "sh") == 0) {
		//printf("gen sh\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);
		inst = INST_JOIN_IMM(0x29, rs, rt, offset);

	} else if (strcmp(buf_op, "sw") == 0) {
		//printf("gen sw\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		if (buf_src[1][0] == 'x') {
			sscanf(buf_src[1] + 1, "%x", &offset);
		} else {
			sscanf(buf_src[1], "%d", &offset);
		}
		rs = get_reg_id(strchr(buf_src[1], '$') + 1);

		inst = INST_JOIN_IMM(0x2B, rs, rt, offset);
	} else if (strcmp(buf_op, "break") == 0) {
		//printf("gen break\n");
		inst = INST_JOIN(0, 0, 0, 0, 0, 0xD);

	} else if (strcmp(buf_op, "syscall") == 0) {
		//printf("gen syscall\n");
		inst = INST_JOIN(0, 0, 0, 0, 0, 0xC);

	} else if (strcmp(buf_op, "mfhi") == 0) {
		//printf("gen mfhi\n");
		if (id != 1) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		inst = INST_JOIN(0, 0, 0, rd, 0, 0x10);

	} else if (strcmp(buf_op, "mflo") == 0) {
		//printf("gen mflo\n");
		if (id != 1) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		inst = INST_JOIN(0, 0, 0, rd, 0, 0x12);

	} else if (strcmp(buf_op, "mthi") == 0) {
		//printf("gen mthi\n");
		if (id != 1) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		inst = INST_JOIN(0, rs, 0, 0, 0, 0x11);

	} else if (strcmp(buf_op, "mtlo") == 0) {
		//printf("gen mtlo\n");
		if (id != 1) {
			inst_error(count);
			return;
		}
		rs = get_reg_id(&buf_src[0][1]);
		inst = INST_JOIN(0, rs, 0, 0, 0, 0x13);

	} else if (strcmp(buf_op, "mfc0") == 0) {
		//printf("gen mfc0\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rd = get_reg_id(&buf_src[1][1]);
		inst = INST_JOIN(0x10, 0, rt, rd, 0, 0);

	} else if (strcmp(buf_op, "mtc0") == 0) {
		//printf("gen mtc0\n");
		if (id != 2) {
			inst_error(count);
			return;
		}
		rt = get_reg_id(&buf_src[0][1]);
		rd = get_reg_id(&buf_src[1][1]);
		inst = INST_JOIN(0x10, 4, rt, rd, 0, 0);

	} else if (strcmp(buf_op, "eretx") == 0) {
		//printf("gen eretx\n");
		inst = 0x42000018;
	} else if (strcmp(buf_op, "b") == 0) {
		if (id != 1) {
			inst_error(count);
			return;
		}
		offset = get_label_addr(buf_src[0]);
		inst = INST_JOIN_IMM(4, 0, 0, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "bal") == 0) {
		if (id != 1) {
			inst_error(count);
			return;
		}
		offset = get_label_addr(buf_src[0]);
		inst = INST_JOIN_IMM(1, 0, 0x11, ((offset - count - 4) >> 2));

	} else if (strcmp(buf_op, "nop") == 0) {
		inst = 0;

	} else if (strcmp(buf_op, "move") == 0) {
		if (id != 2) {
			inst_error(count);
			return;
		}
		rd = get_reg_id(&buf_src[0][1]);
		rs = get_reg_id(&buf_src[1][1]);
		inst = INST_JOIN(0, rs, 0, rd, 0, 0x25);

//      }else if (strcmp(buf_op, "") == 0) {

	} else {
		printf("not inst\n");
		return;
	}

	printf("CODE [%08x] : %08x\n", count, inst);
	disassemble(inst);
	fprintf(outmap, "CODE [%08x] : %08x ;%s\n", count, inst, buffer);

	/* only in binary, endian has its meaning */
	if (ADDRESS_BIGENDIAN) {
		inst = ((inst & 0x000000FF) << 24) |
			((inst & 0x0000FF00) << 8) |
			((inst & 0x00FF0000) >> 8) | ((inst & 0xFF000000) >> 24);
	}
	fwrite(&inst, 4, 1, out);
	count += 4;
}

int
main(int argc, char *argv[])
{
	FILE *input;
	FILE *out_d;
	FILE *out_t;

	char buffer[256] = { 0 };
	char *pbuffer;

	char *sname = NULL;

	if (argc <= 1) {
		printf("ass -- the mips assembly compiler(r).by Wuxb!\n");
		printf("to compile xx.s :\"ass xx.s [-tbfc00000] [-dbfa00000] [-b]\",\n\
			output `xx_data`, `xx_text` and `xx_map`\n");
	}

	/* read argv */
	while (--argc) {
		argv++;
		if ((*argv)[0] == '-') {
			switch ((*argv)[1]) {
			case 't':
				sscanf((*argv) + 2, "%x", &segment_text);
				break;
			case 'd':
				sscanf((*argv) + 2, "%x", &segment_data);
				break;
			case 'b':
				/* big endian */
				ADDRESS_BIGENDIAN = 1;
				break;
			}
		} else {
			sname = *argv;
		}
	}

	if (sname == NULL) {
		printf("please set the source file name.\n");
		return 0;
	}

	/* open source file */
	input = fopen(sname, "r");
	if (!input) {
		printf("file not found!\n");
		return 0;
	}

	/* create out_data */
	*strrchr(sname, '.') = 0;
	strcat(sname, "_data");
	out_d = fopen(sname, "wb");
	if (!out_d) {
		printf("cannot out_d\n");
		exit(0);
	}

	/* create out_text */
	*strrchr(sname, '_') = 0;
	strcat(sname, "_text");
	out_t = fopen(sname, "wb");
	if (!out_t) {
		printf("cannot out_t\n");
		exit(0);
	}

	/* create out_map */
	*strrchr(sname, '_') = 0;
	strcat(sname, "_map");
	outmap = fopen(sname, "w");
	if (!outmap) {
		printf("cannot outmap\n");
		exit(0);
	}

	/* first pass, generate label. */
	while (!feof(input)) {
		fgets(buffer, 255, input);
		pbuffer = trim(buffer);

		if (pbuffer[0] == '.') {
			if (strncmp(pbuffer, ".data", 5) == 0) {
				count = segment_data;
			} else if (strncmp(pbuffer, ".text", 5) == 0) {
				count = segment_text;
			} else {
				printf("? segment ?\n");
			}
			continue;
		} else if (pbuffer[0] == '#') {
			continue;
		}

		gen_label_address(pbuffer);
	}

	/* reset ... */
	out = out_t;
	count = SEGMENT_TEXT;
	rewind(input);

	/* second pass, generate code. */
	while (!feof(input)) {
		memset(buffer, 0, 256);
		fgets(buffer, 255, input);
		pbuffer = trim(buffer);

		if (pbuffer[0] == '.') {
			if (strncmp(pbuffer, ".data", 5) == 0) {
				count = segment_data;
				out = out_d;
			} else if (strncmp(pbuffer, ".text", 5) == 0) {
				count = segment_text;
				out = out_t;
			}
			continue;
		} else if (pbuffer[0] == '#') {
			continue;
		}
		process_line(pbuffer);
	}
	exit(0);
}
