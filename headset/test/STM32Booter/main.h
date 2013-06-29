#ifndef MAIN_H_
#define MAIN_H_

#include "arm_math.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4_discovery.h"

void _tick();
void msleep(long time);
void waitButton();

#endif
