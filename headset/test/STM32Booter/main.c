/* Includes */
#include "main.h"

#define N 1024
#define TIMES 5000

static volatile unsigned long long millis = 0LL;

float first[N], second[N], result[N];

void doMathSlow();
void doMathFast();

int main(void) {
	// Tick every 1ms
	SysTick_Config(168000);
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDOn(LED3);
	while (1) {
		msleep(250L);
		STM_EVAL_LEDToggle(LED3);
		STM_EVAL_LEDToggle(LED4);
		doMathFast();
		STM_EVAL_LEDToggle(LED4);
		STM_EVAL_LEDToggle(LED5);
		doMathSlow();
		STM_EVAL_LEDToggle(LED5);
		STM_EVAL_LEDToggle(LED6);
		msleep(250L);
		STM_EVAL_LEDToggle(LED6);
		STM_EVAL_LEDToggle(LED3);
	}
	return 0;
}

void doMathSlow() {
	unsigned int i, j;
	for (i = 0; i < TIMES; i++) {
		for (j = 0; j < N; j++)
			result[j] = first[j] + second[j];
	}
}

void doMathFast() {
	unsigned int i;
	for (i = 0; i < TIMES; i++)
		arm_add_f32(first, second, result, N);
}

void _tick() {
	millis++;
}

void msleep(long time) {
	unsigned long long target = millis + time;
	while (millis < target);
}

void waitButton() {
	while (STM_EVAL_PBGetState(BUTTON_USER) == Bit_RESET);
	msleep(100L);
	while (STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET);
	msleep(100L);
}
