

void iolet_initial(void);
void iolet_clean(void);
void iolet_check_input(void);
void iolet_read_32(unsigned int address, unsigned int * data);
void iolet_read_16(unsigned int address, unsigned short * data);
void iolet_read_8(unsigned int address, unsigned char * data);
void iolet_write_32(unsigned int address, unsigned int data);
void iolet_write_16(unsigned int address, unsigned short data);
void iolet_write_8(unsigned int address, unsigned char data);
