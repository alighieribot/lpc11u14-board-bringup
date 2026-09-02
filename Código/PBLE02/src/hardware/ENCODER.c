/*
 * encoder.c
 */

#include "encoder.h"

// Mapeamento dos pinos conforme o esquemático
#define ENC_PORT_A 0
#define ENC_PIN_A  2

#define ENC_PORT_B 1
#define ENC_PIN_B  26

#define ENC_PORT_SW 1
#define ENC_PIN_SW 27

/*
 * Aqui usamos os 2 bits (A,B) como um estado de 0 a 3 e uma tabela
 * com todas as transições possíveis entre estados consecutivos. Cada
 * transição válida vale +1 ou -1 (um "quarter-step"); transições
 * impossíveis (ruído/bounce) valem 0. Um clique físico do encoder
 * equivale a 4 quarter-steps, então só reportamos um movimento quando
 * o encoder volta ao estado de repouso (detent), com A=1 e B=1 (nível
 * alto pelos pull-ups quando o encoder não está sendo girado).
 *
 * indice = (estado_anterior << 2) | estado_atual
 */
static const int8_t tabelaQuadratura[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

static uint8_t estadoAnterior = 0;  // ultimo estado (A<<1)|B
static int8_t  acumulador     = 0;  // soma dos quarter-steps desde o ultimo detent

void iniciaEncoder(void) {
    // 1. Habilita o clock para o bloco GPIO (redundante com o do LED, mas seguro)
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);

    // 2. Configura os pinos como GPIO (FUNC0) e HABILITA os resistores de Pull-Up
    // O Pull-Up garante nível lógico ALTO quando o encoder estiver "aberto"
    Chip_IOCON_PinMuxSet(LPC_IOCON, ENC_PORT_A, ENC_PIN_A, (IOCON_FUNC0 | IOCON_MODE_PULLUP));
    Chip_IOCON_PinMuxSet(LPC_IOCON, ENC_PORT_B, ENC_PIN_B, (IOCON_FUNC0 | IOCON_MODE_PULLUP));
    Chip_IOCON_PinMuxSet(LPC_IOCON, ENC_PORT_SW, ENC_PIN_SW, (IOCON_FUNC0 | IOCON_MODE_PULLUP));

    // 3. Define a direção dos pinos como Entrada (Input = 0)
    Chip_GPIO_SetPinDIR(LPC_GPIO, ENC_PORT_A, ENC_PIN_A, 0);
    Chip_GPIO_SetPinDIR(LPC_GPIO, ENC_PORT_B, ENC_PIN_B, 0);
    Chip_GPIO_SetPinDIR(LPC_GPIO, ENC_PORT_SW, ENC_PIN_SW, 0);

    // 4. Salva o estado inicial (A,B) para referência da máquina de estados
    uint8_t A = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_A, ENC_PIN_A);
    uint8_t B = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_B, ENC_PIN_B);
    estadoAnterior = (A << 1) | B;
    acumulador = 0;
}

int leEncoder(void){
    uint8_t A = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_A, ENC_PIN_A);
    uint8_t B = Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_B, ENC_PIN_B);
    uint8_t estadoAtual = (A << 1) | B;

    if (estadoAtual == estadoAnterior) {
        return 0; // nada mudou desde a ultima leitura
    }

    uint8_t indice = (uint8_t)((estadoAnterior << 2) | estadoAtual);
    acumulador += tabelaQuadratura[indice];
    estadoAnterior = estadoAtual;

    int movimento = 0;

    // Só reporta o clique quando o encoder volta ao repouso (A=1,B=1).
    //
    // Idealmente um clique fisico gera 4 quarter-steps (+4 ou -4), mas
    // como a leitura aqui e por polling (o loop principal tambem faz
    // LCD, serial, etc.), e facil perder alguma leitura intermediaria.
    // Por isso usamos apenas o SINAL do acumulado (nao o valor exato):
    // qualquer saldo positivo/negativo ao voltar ao repouso ja conta
    // como um clique no sentido correspondente. Isso torna a leitura
    // tolerante a passos perdidos, sem reintroduzir o bug de "sempre
    // o mesmo sentido" (transicoes invalidas continuam valendo 0).
    if (estadoAtual == 3) {
        if (acumulador > 0)
            movimento = 1;
        else if (acumulador < 0)
            movimento = -1;

        acumulador = 0; // sempre zera ao passar pelo detent
    }

    return movimento;
}

int leBotaoEncoder(void) {
    // Como usamos Pull-Up, o botão pressionado joga o sinal para 0 (GND)
    if (Chip_GPIO_GetPinState(LPC_GPIO, ENC_PORT_SW, ENC_PIN_SW) == 0) {
        return 1; // Pressionado
    }
    return 0; // Solto
}
