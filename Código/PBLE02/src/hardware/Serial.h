#ifndef SERIAL_H
#define SERIAL_H
    #include "../programa.h"
    #define UART_SRB_SIZE 128
    #define UART_RRB_SIZE 32

    extern RINGBUFF_T txring, rxring;
    extern uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

    extern volatile int serial_alarme_L_pendente;
    extern volatile int serial_alarme_H_pendente;

    void UART_IRQHandler(void);
    void iniciaSerial(void);
    void desligaSerial(void);
    uint8_t dadoRecebido(void);
    void enviaDado(uint8_t dado);
    void serialLigaLED(void);
    void processaComandoSerial(void);

    void serial_enviaString(const char*);
    void serial_enviaInteiro(int);
    void serial_enviaInt2Dig(int);
    void serial_enviaInt4Dig(int);

    int eventoSerial(int ev);
#endif
