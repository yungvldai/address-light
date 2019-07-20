#include "avr/io.h"
#include "util/delay.h"

#define set_l() PORTC &= ~(1 << 0) 
#define set_h() PORTC |= (1 << 0) 

#include "analog.h"

#define LEDS 5

unsigned long int color_map[60] = {
	0xff0000, 0xff1a00, 0xff3300, 0xff4d00, 0xff6600,
	0xff8000, 0xff9900, 0xffb300, 0xffcc00, 0xffe600,
	0xffff00, 0xe6ff00, 0xccff00, 0xb3ff00, 0x99ff00,
	0x80ff00, 0x66ff00, 0x4dff00, 0x33ff00, 0x1aff00,
	0xff00, 0xff1a, 0xff33, 0xff4d, 0xff66, 0xff80,
	0xff99, 0xffb3, 0xffcc, 0xffe6, 0xffff, 0xe6ff,
	0xccff, 0xb3ff, 0x99ff, 0x80ff, 0x66ff, 0x4dff,
	0x33ff, 0x1aff, 0xff, 0x1a00ff, 0x3300ff, 0x4d00ff,
	0x6600ff, 0x8000ff, 0x9900ff, 0xb300ff, 0xcc00ff,
	0xe600ff, 0xff00ff, 0xff00e6, 0xff00cc, 0xff00b3,
	0xff0099, 0xff0080, 0xff0066, 0xff004d, 0xff0033,
	0xff001a
};

unsigned int speed[10] = {
	1000, 600, 500, 400, 300, 200, 100, 50, 10, 11
};

#define RAINBOW 1
#define STROBE 2
#define CLEARWHITE 3
#define STROBEWHITE 4

void low(void) {
	set_h();
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	set_l();
}

void high(void) {
	set_h();
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
	asm("nop");
	set_l(); 
}


void set_one_color(unsigned long int color, unsigned int len) {
	_delay_ms(10);
	unsigned long int bit_order;
	unsigned int j,i;
	for (j = 0; j < len; j++) {
		bit_order = 0x1000000;
		for (i = 0;i < 24;i++){
			bit_order >>= 1;	
			if (((color) & bit_order) == 0) {
				low();			
			} else {
				high();
			} 
		}
	}
	_delay_ms(10);
}

void set_array(unsigned long int* colors, unsigned int len) {
	_delay_ms(10);
	unsigned long int bit_order;
	unsigned int j,i;
	for (j = 0; j < len; j++) {
		bit_order = 0x1000000;
		for (i = 0;i < 24;i++){
			bit_order >>= 1;	
			if (((colors[j]) & bit_order) == 0) {
				low();			
			} else {
				high();
			} 
		}
	}
	_delay_ms(10);
}

unsigned long int rgb(unsigned long int r, unsigned long int g, unsigned long int b) {
	return (g << 16) | (r << 8) | b; 
}

unsigned long int reorder(unsigned long int grb) {
	unsigned int g = (grb >> 8) & 0b11111111;
	unsigned int r = (grb >> 16) & 0b11111111;
	unsigned int b = grb & 0b11111111;
	return rgb(r, g, b);
}

void run_light(unsigned long int* array, unsigned int len) {
	unsigned long int color = array[0];
	int i;
	for(i = 0; i < (len - 1); i++) {
		array[i] = array[i + 1];
	}
	array[len - 1] = color;
}

unsigned long int apply_brightness(unsigned long int color, unsigned int brightness) {
	unsigned int r = (color >> 8) & 0b11111111;
	unsigned int g = (color >> 16) & 0b11111111;
	unsigned int b = color & 0b11111111;
	r = r * brightness / 100;
	g = g * brightness / 100;
	b = b * brightness / 100;
	return rgb(r, g, b);
}

int main(void) {
	DDRC = 0b100111;
	DDRD = 0b00000000;
	
	adc_init();
	
	unsigned int reo_3, reo_4, brightness, mode = 1;
	
	while(1) {
	
		_delay_ms(1);
		
		if (PIND & 1) {
			mode++;
			if (mode == 5) {
				mode = 1;
			}
			_delay_ms(300);
		}
		
		if (mode == RAINBOW) {
			reo_3 = adc_convert(3);
			reo_4 = adc_convert(4);
			brightness = reo_4 * 50 / 1023;
		
			set_one_color(
				reorder(
					apply_brightness(
						color_map[reo_3 * 59 / 1023],
					brightness)
				),
			LEDS);
		} 
		if (mode == STROBE) {
			reo_3 = adc_convert(3);
			reo_4 = adc_convert(4);
			set_one_color(
				reorder(
					color_map[reo_3 * 59 / 1023]
				),
			LEDS);
			_delay_ms(speed[reo_4 / (1023 / 9)]);
			set_one_color(0, LEDS);
			_delay_ms(speed[reo_4 / (1023 / 9)]);
		} 
		if (mode == CLEARWHITE) {
			reo_4 = adc_convert(4);
			brightness = reo_4 * 50 / 1023;
			set_one_color(
				apply_brightness(
					0xffffff, brightness
				),
			LEDS);
		}
		if (mode == STROBEWHITE) {
			reo_3 = adc_convert(3);
			reo_4 = adc_convert(4);
			brightness = reo_4 * 50 / 1023;
			set_one_color(
				apply_brightness(
					0xffffff, brightness
				),
			LEDS);
			_delay_ms(speed[reo_3 / (1023 / 9)]);
			set_one_color(0, LEDS);
			_delay_ms(speed[reo_3 / (1023 / 9)]);
		} 
	}

	return 0;
}