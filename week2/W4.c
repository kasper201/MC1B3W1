#define GPIOB_BASE 0x48000400
#define RCC_BASE 0x40021000
#define TIM3 0x40000400

#define RCC_AHBENR (*(unsigned int *)(RCC_BASE + 0x14))
#define GPIOB_MODER (*(unsigned int *)(GPIOB_BASE + 0x00))
#define GPIOB_ODR (*(unsigned int *)(GPIOB_BASE + 0x14))
#define GPIOB_IDR (*(unsigned int *)(GPIOB_BASE + 0x10))
#define GPIOB_PUPDR (*(unsigned int *)(GPIOB_BASE + 0x0C))

#define RCC_APB1ENR (*(unsigned int *)(RCC_BASE + 0x1C))
#define TIM6_CR1 (*(unsigned int *)(TIM6 + 0x00))
#define TIM6_EGR (*(unsigned int *)(TIM6 + 0x14))
#define TIM6_PSC (*(unsigned int *)(TIM6 + 0x28))
#define TIM6_CNT (*(unsigned int *)(TIM6 + 0x24))

#define TIM3_CR1 (*(unsigned int *)(TIM3 + 0x00))
#define TIM3_EGR (*(unsigned int *)(TIM3 + 0x14))
#define TIM3_PSC (*(unsigned int *)(TIM3 + 0x28))
#define TIM3_CNT (*(unsigned int *)(TIM3 + 0x24))
#define TIM3_CCR1 (*(unsigned int *)(TIM3 + 0x34))
#define TIM3_SR (*(unsigned int *)(TIM3 + 0x10))
#define TIM3_CCMR1 (*(unsigned int *)(TIM3 + 0x18))

//TIM3 global interrupt address is 0x0000 0080

void shieldConfig() {

	//setting clock
	RCC_AHBENR |= (1 << 18) | (1 << 17); // clock B is located in bit 18
	//setting pin 3 to GPO ( 01 ) LSB
	GPIOB_MODER |= (1 << (3 * 2)) | (1 << (4 * 2)) | (1 << (5 * 2))
					| (1 << (6 * 2)); // due to only bit 6, 8, 10 and 12 needing to be High only this one

	// leaving pins 11, 12 and 13 untouched for now (22,23, 24,25, 26,27)

	//setting pin 15 to pull-up (
	GPIOB_PUPDR |= (1 << 8 * 2); // due to only bit  needing to be High only this one

}

void timerConfig() {
	RCC_APB1ENR |= (1 << 1); // Enable TIM3 clock
	TIM3_PSC = 7999; // Set prescaler TIM3
	TIM3_EGR |= (1 << 0);
	TIM3_CR1 |= (1 << 0);
}

void ledWrite(int num, int on) {
	int ledsArray[] = { 3, 5, 4, 6 };
	if (on == 1) {
		GPIOB_ODR |= (1 << ledsArray[num]); // turn on led corresponding to location num in array
	} else {
		GPIOB_ODR &= ~(1 << ledsArray[num]); // turn off led corresponding to location num in array
	}
}

void timerDelay(unsigned int milliseconds) {
	TIM3_CCR1 = milliseconds;
	TIM3_CNT = 0;
	while ((TIM3_SR & (1 << 1)) == 0) {
	}
	TIM3_SR &= ~(1 << 1); // RESET SR
}

void loopLicht()
{
	for(int i=0; i < 4; i++)
	{
		ledWrite(i,1);
		timerDelay(500);
		ledWrite(i,0);
	}
}
int main() {
	shieldConfig();
	timerConfig();

	while(1){
		loopLicht();
	}

	return 0;
}
