#ifndef TECLADO_H_
#define TECLADO_H_

#include "../programa.h"

// Mapeamento exato extraído do datasheet do LPC11U1x
#define PORTA_BTN_CIMA   1
#define PINO_BTN_CIMA    13  // Pino 36: PIO1_13

#define PORTA_BTN_MEIO   0
#define PINO_BTN_MEIO    14  // Pino 35: PIO0_14 (Partilhado com TRST)

#define PORTA_BTN_BAIXO  0
#define PINO_BTN_BAIXO   13  // Pino 34: PIO0_13 (Partilhado com TDO)

// Inicializa os pinos e configura o IOCON
void iniciaTeclado(void);

// Funções de leitura (Retornam 1 se pressionado, 0 se solto)
int lerBotaoCima(void);
int lerBotaoMeio(void);
int lerBotaoBaixo(void);

#endif
