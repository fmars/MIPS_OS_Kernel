#define SEGMENT_DATA         0xbfa00000
#define SEGMENT_TEXT         0xbfc00000


struct LABEL_LIST {
	unsigned int address;
	char name[16];
	struct LABEL_LIST *next;
};

int ass_assemble(char *asmcode, unsigned int *code, struct LABEL_LIST *labels, unsigned int address);



