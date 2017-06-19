/* Surely you will remove the processor conditionals and this comment
 *    appropriately depending on whether or not you use C++. */
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
		VGA_COLOR_BLACK = 0,
		VGA_COLOR_BLUE = 1,
		VGA_COLOR_GREEN = 2,
		VGA_COLOR_CYAN = 3,
		VGA_COLOR_RED = 4,
		VGA_COLOR_MAGENTA = 5,
		VGA_COLOR_BROWN = 6,
		VGA_COLOR_LIGHT_GREY = 7,
		VGA_COLOR_DARK_GREY = 8,
		VGA_COLOR_LIGHT_BLUE = 9,
		VGA_COLOR_LIGHT_GREEN = 10,
		VGA_COLOR_LIGHT_CYAN = 11,
		VGA_COLOR_LIGHT_RED = 12,
		VGA_COLOR_LIGHT_MAGENTA = 13,
		VGA_COLOR_LIGHT_BROWN = 14,
		VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
		return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
		return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) {
		size_t len = 0;
		while (str[len])
			len++;
		return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 20;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_lineshift(){
	for(size_t followPointer=0,indexPointer=1;indexPointer<VGA_HEIGHT;indexPointer++,followPointer++){
		for(size_t index=0;index<VGA_WIDTH;index++){
			terminal_buffer[followPointer*VGA_WIDTH+index]=terminal_buffer[indexPointer*VGA_WIDTH+index];
			if(indexPointer==VGA_HEIGHT-1)
				terminal_buffer[indexPointer*VGA_WIDTH+index]=0;
		}
	
	}

	/*uint8_t bg=((terminal_color&(15<<4)))%15;
	uint8_t fg=((terminal_color&15)+1)%15;
	terminal_setcolor(vga_entry_color(fg,bg));*/
}
 
void terminal_putchar(char c) {
	if(c=='\n'){
		terminal_column=VGA_WIDTH-1;
		/*uint8_t bg=((terminal_color&(15<<4)))%15;
		uint8_t fg=((terminal_color&15)+1)%15;
		terminal_setcolor(vga_entry_color(fg,bg));*/
	}
	else
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT){
			terminal_lineshift();
			terminal_row--;
		}
	}
}
 
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}
static inline void io_wait(void)
{
    /* TODO: This is probably fragile. */
    asm volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}
static inline uint8_t inb(uint16_t _port)
{
	uint8_t result;
	asm volatile("inb %1, %0" : "=a" (result) : "d" (_port));
	return result;
}
char getScancode()
{
	char c=0;
	//terminal_writestring("\ngetScancode called");
	do {
		if(inb(0x60)!=c)
		{	
			//terminal_writestring(&c);
			c=inb(0x60); 
			//terminal_writestring(&c);
			if(c>0){
				//terminal_writestring("\nReceived greater than zero");
				io_wait();
				return c;
			}
		}
	}while(1);
}
char getchar()
{
	return getScancode();
	//return 's';
}
 
 static inline bool are_interrupts_enabled()
{
    unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void) {
	/* Initialize terminal interface */
	terminal_initialize();
		 
	/* Newline support is left as an exercise. */
	terminal_writestring("Hello, kernel World!\nkarna banu ashalagari");
	while(1){terminal_putchar(getchar());io_wait();}
	/*if(are_interrupts_enabled()==0)
		terminal_writestring("\nInterrupts disabled");
	else {
		terminal_writestring("Interrupts enabled: ");
		terminal_putchar(are_interrupts_enabled());
	}*/
}
