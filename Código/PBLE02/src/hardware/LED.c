/*
 *  Created on: 5 de jul. de 2025
 *      Author: Osmar Bruno
 */

#include "../programa.h"

void iniciaLED(void);
void ligaLED(int);
void desligaLED(int);
void toggleLED(int);

void iniciaLED(void)
{
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);
    LPC_IOCON->PIO0[11] = 0x01;
    LPC_IOCON->PIO0[12] = 0x01;

	Chip_GPIO_SetPinDIR(LPC_GPIO, 0, 12, 1); //LED 0
	Chip_GPIO_SetPinDIR(LPC_GPIO, 0, 11, 1);  //LED 1
//    Chip_GPIO_SetPinDIR(LPC_GPIO, 0, 12, 1);  //LED 2
//	Chip_GPIO_SetPinDIR(LPC_GPIO, 1, 28, 1); //LED 3


	Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 0);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, 0);
//	Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 0);
//	Chip_GPIO_SetPinState(LPC_GPIO, 1, 28, 0);
}

void ligaLED(int led)
{
    switch(led) {
        case 0:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 1);
            break;
        case 1:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, 1);
            break;
        default:
            break;
    }
}

void desligaLED(int led)
{
    switch(led) {
        case 0:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, 0);
            break;
        case 1:
            Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, 0);
            break;
        default:
            break;
    }
}

void defineLED(int led, int state)
{
	switch(led) {
		case 0:
			Chip_GPIO_SetPinState(LPC_GPIO, 0, 12, state);
			break;
		case 1:
			Chip_GPIO_SetPinState(LPC_GPIO, 0, 11, state);
			break;
		default:
			break;
	}
}

void toggleLED(int led)
{
    switch(led) {
        case 0:
            Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 12);
            break;
        case 1:
            Chip_GPIO_SetPinToggle(LPC_GPIO, 0, 11);
            break;
        default:
            break;
    }
}
