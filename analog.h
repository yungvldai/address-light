#ifndef ANALOG_H
#define ANALOG_H

#include "avr/io.h"

void adc_init(void) {
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX |= (1 << REFS0);
}

unsigned int adc_convert(unsigned int adc_in) {
	ADMUX &= 0b11111000;
	ADMUX |= adc_in;
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADSC)));
	return (unsigned int) ADC;
}

#endif