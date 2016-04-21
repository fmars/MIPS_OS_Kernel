#define SYS_putchar   0
#define SYS_printf    1
#define SYS_exit      2

//#define putchar	syscall_putchar
//#define printf 	syscall_printf
//#define exit 	syscall_exit

   
extern void syscall_putchar(char ch);
extern void syscall_printf(char *fmt);
extern void syscall_exit();
