#define GPIOB_BASE 0x48000400
#define GPIOA_BASE 0x48000000
#define RCC_BASE 0x40021000
#define TIM3 0x40000400
#define TIM7 0x40001400

#define RCC_AHBENR (*(unsigned int *)(RCC_BASE + 0x14))
#define GPIOB_MODER (*(unsigned int *)(GPIOB_BASE + 0x00))
#define GPIOB_ODR (*(unsigned int *)(GPIOB_BASE + 0x14))
#define GPIOB_IDR (*(unsigned int *)(GPIOB_BASE + 0x10))
#define GPIOB_PUPDR (*(unsigned int *)(GPIOB_BASE + 0x0C))

#define GPIOA_MODER (*(unsigned int *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR (*(unsigned int *)(GPIOA_BASE + 0x14))
#define GPIOA_IDR (*(unsigned int *)(GPIOA_BASE + 0x10))

#define RCC_APB1ENR (*(unsigned int *)(RCC_BASE + 0x1C))
#define TIM3_CR1 (*(unsigned int *)(TIM3 + 0x00))
#define TIM3_EGR (*(unsigned int *)(TIM3 + 0x14))
#define TIM3_PSC (*(unsigned int *)(TIM3 + 0x28))
#define TIM3_CNT (*(unsigned int *)(TIM3 + 0x24))
#define TIM3_CCR1 (*(unsigned int *)(TIM3 + 0x34))
#define TIM3_SR (*(unsigned int *)(TIM3 + 0x10))
#define TIM3_CCMR1 (*(unsigned int *)(TIM3 + 0x18))

#define RCC_APB1RSTR (*(unsigned int *)(RCC_BASE + 0x10))//reset register tim7
#define TIM7_PSC (*(unsigned int *)(TIM7 + 0x28))
#define TIM7_CNT (*(unsigned int *)(TIM7 + 0x24))
#define TIM7_SR (*(unsigned int *)(TIM7 + 0x10))
#define TIM7_CR1 (*(unsigned int *)(TIM7 + 0x00))
#define TIM7_EGR (*(unsigned int *)(TIM7 + 0x14))

void shieldConfig() {

	//setting clock
	RCC_AHBENR |= (1 << 18) | (1 << 17); // clock B is located in bit 18 and clock A in bit 17
	//setting pin 3 to GPO ( 01 ) LSB
	GPIOB_MODER |= (1 << (3 * 2)) | (1 << (4 * 2)) | (1 << (5 * 2)) | (1 << (6 * 2)); // due to only bit 6, 8, 10 and 12 needing to be High only this one

	// leaving pins 11, 12 and 13 untouched for now (22,23, 24,25, 26,27)

	//setting pin 15 to pull-up (
	GPIOB_PUPDR |= (1 << 8 * 2); // due to only bit  needing to be High only this one

	GPIOA_MODER |= (1 << (5 * 2)) | (1 << (6 * 2)); //5 is clock 6 is DIO
}

void timerConfig() {
	RCC_APB1ENR |= (10001 << 1); // Enable TIM3 and TIM7 clock
	TIM3_PSC = 7; // Set prescaler TIM3
	TIM7_PSC = 7999; // Set prescaler TIM7
	TIM3_EGR |= (1 << 0);
	TIM7_EGR |= (1 << 0);
	TIM3_CR1 |= (1 << 0);
	TIM7_CR1 |= (1 << 0);
}

void ledWrite(int num, int on) {
	int ledsArray[] = { 3, 5, 4, 6 };
	if (on == 1) {
		GPIOB_ODR |= (1 << ledsArray[num]); // turn on led corresponding to location num in array
	} else {
		GPIOB_ODR &= ~(1 << ledsArray[num]); // turn off led corresponding to location num in array
	}
}

void timerDelay(int milliseconds) {
	TIM3_CCR1 = milliseconds;
	TIM3_CNT = 0;
	while ((TIM3_SR & (1 << 1)) == 0)
	{

	}
	TIM3_SR &= ~(1 << 1); // RESET SR
}

void loopLicht()
{
	for(int i=0; i < 4; i++)
	{
		ledWrite(i,1);
		timerDelay(50000);
		ledWrite(i,0);
	}
}

void clock(int value)
{
	if(value == 0)
	{
		GPIOA_ODR &= ~(1 << 5);
	}
	else
	{
		GPIOA_ODR |= (1 << 5);
	}
	timerDelay(1);
}

void dio(int value)
{
	if(value == 1)
	{
		GPIOA_ODR &= ~(1 << 6);
	}
	else
	{
		GPIOA_ODR |= (1 << 6);
	}
}

void start()
{
	clock(1);
	dio(1);
	timerDelay(1);
	dio(0);
	timerDelay(1);
	clock(0);
	timerDelay(1);
}

void bitx(char state)
{
	clock(1);
	if(state == 1)
	{
		dio(1);
	}
	else
	{
		dio(0);
	}
	timerDelay(1);
	clock(1);
	timerDelay(1);
	clock(0);
	timerDelay(1);
}

void stop()
{
	clock(0);
	dio(0);
	timerDelay(1);
	clock(1);
	timerDelay(1);
	dio(1);
	timerDelay(1);
}

void addressdisplay()
{
	for(int ad = 0; ad <= 8; ad++)
	{
		if(ad < 6 || ad == 8)
		{
			bitx(0);
		}
		else
		{
			bitx(1);
		}
	}
}

void writeDisplay()
{
	for(int data = 0; data < 4; data++)
	{
		for(int byte = 0; byte <= 8; byte++)
		{
			if(byte >= 7)
			{
				bitx(0);
			}
			else
			{
				bitx(1);
			}
		}
	}
	stop();
}

void segmentConfig()
{
	start();
	for(int counter = 0; counter <= 8; counter++) //data command
	{
		if(counter != 6)
		{
			bitx(0);
		}
		else
		{
			bitx(1);
		}
	}
	for(int dc = 0; dc <= 8; dc++)
	{
		if(dc < 4 || dc == 7)//display control command
		{
			bitx(1);
		}
		else
		{
			bitx(0);
		}
	}
	addressdisplay();
	writeDisplay();
}

void sevensegment(int seconds)
{
	if(seconds < 10)
	{
		//addressdisplay();
	}
}

int main() {
	shieldConfig();
	timerConfig();
	segmentConfig();

	while(1)
	{
		sevensegment(1);
		loopLicht();
	}

	return 0;
}
