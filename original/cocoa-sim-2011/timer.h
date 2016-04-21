// time control.
// ns..

// second : 1,000,000,000 ns

#define TIMER_OFFSET_0_CTRL    0
#define TIMER_OFFSET_0_PRESET  4
#define TIMER_OFFSET_0_COUNT   8
#define TIMER_OFFSET_1_CTRL    12
#define TIMER_OFFSET_1_PRESET  16
#define TIMER_OFFSET_1_COUNT   20
#define TIMER_OFFSET_2_CTRL    24
#define TIMER_OFFSET_2_PRESET  28
#define TIMER_OFFSET_2_COUNT   32



#define TC_TIME_INST_COMMON    50
#define TC_TIME_PULSE          80
#define TC_TIME_TICK           500


#define TC_CTRL_MASK         0x00000007
#define TC_CTRL_MODE_MASK    0x00000006
#define TC_CTRL_ENABLE_MASK  0x00000001


struct RTC {
	unsigned int ctrl;
	unsigned int preset;
	unsigned int count;
	unsigned int out;
};

void timer_reset(void);
void timer_time_pass(unsigned int ns);
void timer_read_32(unsigned int address, unsigned int * data);
void timer_read_16(unsigned int address, unsigned short * data);
void timer_read_8(unsigned int address, unsigned char * data);
void timer_write_32(unsigned int address, unsigned int data);
void timer_write_16(unsigned int address, unsigned short data);
void timer_write_8(unsigned int address, unsigned char data);
void show_timer_regs(void);

