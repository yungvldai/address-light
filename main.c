#include <avr/io.h>
#include <util/delay.h>

#define cl() PORTC &= ~(1<<1)
#define ch() PORTC |= (1<<1)

#define runtime 1

void set_l(void) {
	ch();
	asm("nop"); //microdelay
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	cl();
}

void set_h(void) {
	ch();
	asm("nop"); //microdelay
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	cl();
}

void set_array(unsigned long int* array, unsigned int len) {
	unsigned long int bit_order;
	unsigned int j, i;
	for (j = 0; j < len; j++) { 
		bit_order = 0x1000000;
		for (i = 0; i < 24; i++) {
			bit_order >>= 1;	
			if ((array[j] & bit_order) == 0) {
				set_l(); 			
			} else {
				set_h();
			} 
		}
	}
}

unsigned long int rgb(unsigned long int r, unsigned long int g, unsigned long int b) {
	return (g << 16) | (r << 8) | (b);
}

int main(void) {
	while (runtime) {
		
	}
	return 0;
}