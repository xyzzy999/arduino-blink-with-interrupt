#include <WProgram.h>
#include "arduino.h"

/*
 * Schieberegister test
 */

int main(void)
{
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

int t_delay = 20,
		c = 0;

struct s_pin_out {
	uint8_t data[8], ps, clock, ds;
} pin_out =
		{
					{ 2, 3, 4, 5, 6, 7, 8, 9 }, 11, 12, 13
		},
		*pout = &pin_out;

struct s_pin_inp {
	uint8_t q6, q7, q8;
} pin_inp =
		{ 53, 51, 49 },
		*pinp = &pin_inp;

void wbyte(uint8_t b)
{
	uint8_t i;

	// Serial.print((int)b);
	// Serial.print(" = ");
	for (i = 0; i < 8; i++, b >>= 1) {
		// (b & 0x01) ? Serial.print("1 ") : Serial.print("0 ");
		digitalWrite(pout->data[i], b & 0x01);
	}
	// Serial.print("\n");
}

int readq()
{
	int res = 0;

	res |= digitalRead(pinp->q8);
	res <<= 1;
	res |= digitalRead(pinp->q7);
	res <<= 1;
	res |= digitalRead(pinp->q6);

	return res;
}

void pulse(uint8_t pin, uint8_t level = LOW, int t = 50000)
{
	digitalWrite(pin, !level);
	delayMicroseconds(t);
	//delay(t);
	digitalWrite(pin, level);
	delayMicroseconds(t);
	//delay(t);
}

void setup()
{
	uint8_t *p;

	for (p = (uint8_t *) pout; p < (uint8_t *) &pout[1]; p++)
		pinMode(*p, OUTPUT);
	for (p = (uint8_t *) pinp; p < (uint8_t *) &pinp[1]; p++)
		pinMode(*p, INPUT);
	Serial.begin(115200);
	delay(2000);
	Serial.println("Start :");
}

void loop()
{
	uint8_t i,
			soll,
			q678;

	if ( ! (c &= 0xff) ) Serial.print("Write Byte = 0");

	wbyte(c);		// send data to ic = mc14021
	pulse(pout->ps, LOW);	// read data into ic

	for (i = 0; i < 8; i++) {
		q678 = readq();		// read q6, q7, q8 from ic
		soll = (i < 5) ? c >> (5 - i) : c << (i - 5);
		soll &= 0x07;
		if (soll != q678) {
			Serial.print("clock[");
			Serial.print((int)i);
			Serial.print("] = ");

			Serial.print((int)q678);
			Serial.print(" : ");
			Serial.print((int)soll);
			Serial.println(" error");
			// (soll == q678) ? Serial.println(" ok") : Serial.println(" error");
		}
		pulse(pout->clock); // shift data in IC
	}
	c++;
}
