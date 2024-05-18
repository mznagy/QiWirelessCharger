#include <avr/io.h>
//#define F_CPU 11059200
#include <util/delay.h>
#include <avr/power.h>
#include <avr/interrupt.h>
# define HIGH 1
# define LOW 0
#define clock_div_1 11.059200


//********************
//the code is mapped to Atmega32


// Peripherals initialization:
void digitalWrite(uint8_t pin, uint8_t state) {
	if (state)
		PORTA |= (1<<PA3); // we need to map PB4->PA2 (ADC2),PORTB -> PORTA in Atmega32A
	else
		PORTA &= ~(1 << PA3);
}

void adc_init(void) {
	ADMUX = 1 << REFS0;
	ADMUX |= 3; // by this we are using ADC 3 in both attiny13 & atmega32
	// enabling the ADC and the PreScaller select.
	ADCSRA = (1 << ADEN) | (1 << ADPS2);
}

uint16_t adc_read(void) {
	ADCSRA |= 1 << ADSC; // starting conversion
	while (ADCSRA & (1 << ADSC)); // polling to stop the code if the bit is not set
	return ADC;
}


// Global variable to track the state all over the code:
volatile uint8_t bit_state = 0;

//
void tx_byte(uint8_t data) {
	bit_state ^= 1; // toggling the state as indicator that it's being used or not.
	//	if it's being used the digital writes won't be done. as it's already in action
	digitalWrite(2, bit_state);
	_delay_us(250);
	digitalWrite(2, bit_state);
	_delay_us(250);

	uint8_t parity = 0;
	for (int i = 0; i < 8; i++) {
		bit_state ^= 1;
		digitalWrite(2, bit_state);
		_delay_us(250);
		if (data & (1 << i)) {
			parity++;
			bit_state ^= 1;
		}
		digitalWrite(2, bit_state);
		_delay_us(250);
	}

	if (parity & 1) {
		parity = 0;
	} else
		parity = 1;

	bit_state ^= 1;
	digitalWrite(2, bit_state);
	_delay_us(250);

	if (parity) {
		bit_state ^= 1;
	}
	digitalWrite(2, bit_state);
	_delay_us(250);

	bit_state ^= 1;
	digitalWrite(2, bit_state);
	_delay_us(250);
	bit_state ^= 1;

	digitalWrite(2, bit_state);
	_delay_us(250);

}

void tx(uint8_t * data, int len) {
	uint8_t checksum = 0;
	//  static uint8_t state = 0;
	for (int i = 0; i < 15; i++) {
		digitalWrite(2, HIGH);
		_delay_us(250);
		digitalWrite(2, LOW);
		_delay_us(250);
	}
	bit_state = 0;
	for (int i = 0; i < len; i++) {
		tx_byte(data[i]);
		checksum ^= data[i];
	}
	tx_byte(checksum);
}

static uint16_t adcvb[2];

int main() {

	adc_init();
	DDRA |= 1 << PA2; // mapping PB4 to PA2

	volatile uint8_t state = 0;
	while (1) {

		uint16_t adcv = adc_read();
		if (adcv < 423) {
			state = 0;
		}

		switch (state) {
		case 0:
		{
			uint16_t adcv = adc_read();
			if (adcv > 423) {
				state = 1;
				_delay_ms(10);
			}
			break;
		}
		case 1:
		{
			uint8_t dt[] = {
					0x1,
					255
			};
			for (int i = 0; i < 20; i++) {
				tx(dt, 2);    //send ping response so that the transmitter identifies receiver.
				_delay_ms(10);
			}
			state = 2;
			break;
		}

		case 2:
		{
			//if(adcv > ((423*3/2))) {
			int8_t error = 0;
			uint16_t adcv = adc_read();
			int16_t temp_error = 0;
			adcvb[0] = adcvb[1];
			adcvb[1] = adcv;
			//if(abs(adcvb[0] - adcvb[1]) > 20)
			// temp_error = (int16_t)((423* 3) - adcv);
			//else
			temp_error = (int16_t)((423 * 2) - adcv);	//1.1v adc reference. 423 equals to 5V. (4.7/47K voltage divider)

			temp_error /= 5;
			if (temp_error > 127) temp_error = 127;
			if (temp_error < -128) temp_error = -128;
			error = (int8_t) temp_error;
			uint8_t dt[] = {
					0x3,
					(int8_t) error
			};
			tx(dt, 2);	//send error correction packet. 0x03 is error correction packet header. 1 BYTE payload, check WPC documents for more details.
			/*} else {
         uint8_t dt[] = {0x3,(int8_t)1};
         tx(dt,2);
        }*/
		} {
			uint8_t dt[] = {0x4, 0XFF};
			tx(dt, 2);	//received power indication packet. I am not sure if this is needed or not. Please read the WPC document
			//for more details. I jut implemented and it worked. But I am not sure if this is the proper way to do it.
		}
		//    _delay_ms(10);
		break;

		}
	}
}
