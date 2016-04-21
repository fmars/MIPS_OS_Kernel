/*
 * debug.h
 * me Debug the virtual machine in instruction level.
 * by wuxb
 */

#define DEBUG_BREAK      "break"
#define DEBUG_CONTINUE   "continue"
#define DEBUG_ABORT      "abort"
#define DEBUG_STEP       "step"
#define DEBUG_PRINT      "print"
#define DEBUG_EDIT       "edit"


struct BP_LIST{
	struct BP_LIST * next;
	unsigned int break_addr;
};


