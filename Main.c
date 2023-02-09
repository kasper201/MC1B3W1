/*
 * main.c

 *
 *  Created on: 4 Feb 2023
 *      Author: Kasper Janssen
 */

#define GPIOB_BASE 0x48000400

#define RCC_AHBENR (*(unsigned int *)(0x40021000 + 0x14))
#define GPIOB_MODER (*(unsigned int *)(GPIOB_BASE + 0x00))
#define GPIOB_ODR (*(unsigned int *)(GPIOB_BASE + 0x14))
#define GPIOB_IDR (*(unsigned int *)(GPIOB_BASE + 0x10))
#define GPIOB_PUPDR (*(unsigned int *)(GPIOB_BASE + 0x0C))

void shieldConfig() {

	//setting clock
	RCC_AHBENR |= (1 << 18) | (1 << 17); // clock B is located in bit 18
	//setting pin 3 to GPO ( 01 ) LSB
	GPIOB_MODER |= (1 << (3 * 2)) | (1 << (4 * 2)) | (1 << (5 * 2)) | (1 << (6 * 2)); // due to only bit 6, 8, 10 and 12 needing to be High only this one

	// leaving pins 11, 12 and 13 untouched for now (22,23, 24,25, 26,27)
	//setting pin 15 to pull-up (
	GPIOB_PUPDR |= (1 << 8 * 2); // due to only bit  needing to be High only this one

}

void ledWrite(int num, int on) {
	int ledsArray[] = { 3, 5, 4, 6 };
	if (on == 1) {
		GPIOB_ODR |= (1 << ledsArray[num]); // turn on led corresponding to location num in array
	} else {
		GPIOB_ODR &= ~(1 << ledsArray[num]); // turn off led corresponding to location num in array
	}
}

int buttonRead() {
	return (~(GPIOB_IDR >> 8)) & 1; //
}

void delay() { // delay is wacky and you should not ever do it like this but i'm lazy
	int i;
	for (i = 0; i < 10000; i++) {
		asm("nop");
	}
}
int main() {
	shieldConfig();
	int buffer = 0;
	int stateloop = 1;
	int i = 0;

	while (1) {
		ledWrite(i, 1);
		for(int counter = 0; counter < 40; counter++)
		{
			delay();
			int buttonPressed = buttonRead();
			if( buttonPressed != buffer)
			{
				buffer = buttonPressed;
				if (buttonPressed == 1)
				{
					stateloop = -stateloop;
				}
			}
			delay();
		}
		ledWrite(i, 0);
		if (stateloop == 1) {
			i += 1;
			if (i > 3)
				i = 0;
		} else {
			i -= 1;
			if (i < 0)
				i = 3;
		}
	}
	return 0;
}
