#include "programa.h"

//hardware
#include "hardware/LCD.h"
#include "hardware/RTC.h"
#include "hardware/Teclado.h"
#include "hardware/ENCODER.h"
#include "hardware/LED.h"
#include "hardware/Serial.h"

//ctl
#include "ctl/output.h"
#include "ctl/event.h"
#include "ctl/var.h"
#include "ctl/stateMachine.h"
#include "ctl/timer.h"


const uint32_t OscRateIn = 20000000;
volatile uint32_t millis_counter = 0;

int main(void){
	//clock
	iniciaTimer();
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);

	iniciaI2C();

	//hardware
	outputInit();
	iniciaLED();
	iniciaSerial();

	//software
	eventInit();
	iniciaSM();

	LCD_esconderCursor();
	LCD_clear();

	while(1){
		smLoop();
		processaComandoSerial();
	}
}
