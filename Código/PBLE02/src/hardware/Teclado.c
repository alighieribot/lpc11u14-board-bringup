#include "teclado.h"
#include "../programa.h"

void iniciaTeclado(void){
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_BTN_CIMA, PINO_BTN_CIMA, (IOCON_FUNC0 | IOCON_MODE_PULLUP));
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_BTN_MEIO, PINO_BTN_MEIO, 0x91);
    Chip_IOCON_PinMuxSet(LPC_IOCON, PORTA_BTN_BAIXO, PINO_BTN_BAIXO, 0x91);

    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_BTN_CIMA, PINO_BTN_CIMA, 0);
    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_BTN_MEIO, PINO_BTN_MEIO, 0);
    Chip_GPIO_SetPinDIR(LPC_GPIO, PORTA_BTN_BAIXO, PINO_BTN_BAIXO, 0);
}

int lerBotaoCima(void){
    return !Chip_GPIO_GetPinState(LPC_GPIO, PORTA_BTN_CIMA, PINO_BTN_CIMA);
}

int lerBotaoMeio(void){
    return !Chip_GPIO_GetPinState(LPC_GPIO, PORTA_BTN_MEIO, PINO_BTN_MEIO);
}

int lerBotaoBaixo(void){
    return !Chip_GPIO_GetPinState(LPC_GPIO, PORTA_BTN_BAIXO, PINO_BTN_BAIXO);
}
