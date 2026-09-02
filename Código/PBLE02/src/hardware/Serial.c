#include "../programa.h"
#include "../ctl/stateMachine.h"
#include "../ctl/event.h"

#define UART_SRB_SIZE 128
#define UART_RRB_SIZE 32

RINGBUFF_T txring, rxring;
uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

volatile int serial_alarme_L_pendente = -1;
volatile int serial_alarme_H_pendente = -1;

char lower[32];
int last = EV_NOEVENT;

static void Init_UART_PinMux(void)
{
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, IOCON_FUNC1 | IOCON_MODE_INACT);    /* PIO0_18 used for RXD */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, IOCON_FUNC1 | IOCON_MODE_INACT);    /* PIO0_19 used for TXD */
#elif (defined(BOARD_NXP_XPRESSO_11C24) || defined(BOARD_MCORE48_1125))
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_6, (IOCON_FUNC1 | IOCON_MODE_INACT));/* RXD */
    Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO1_7, (IOCON_FUNC1 | IOCON_MODE_INACT));/* TXD */
#else
#error "No Pin muxing defined for UART operation"
#endif
}

void UART_IRQHandler(void)
{
    Chip_UART_IRQRBHandler(LPC_USART, &rxring, &txring);
}

void iniciaSerial(void)
{
    Init_UART_PinMux();

    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART0);
    Chip_Clock_SetUARTClockDiv(1);

    Chip_UART_SetBaud(LPC_USART, 115200);
    Chip_UART_ConfigData(LPC_USART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
    Chip_UART_SetupFIFOS(LPC_USART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(LPC_USART);

//    RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
//    RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);
//
//    Chip_UART_IntEnable(LPC_USART, (UART_IER_RBRINT | UART_IER_RLSINT));
//    NVIC_SetPriority(UART0_IRQn, 1);
//    NVIC_EnableIRQ(UART0_IRQn);
}

void desligaSerial(void)
{
   // NVIC_DisableIRQ(UART0_IRQn);
   // Chip_UART_DeInit(LPC_USART);
}

uint8_t dadoRecebido(void)
{
    uint8_t key = 0;
    key = Chip_UART_ReadByte(LPC_USART);
    return key;
}

void enviaDado(uint8_t dado)
{
    Chip_UART_SendByte(LPC_USART, dado);
}

void serialLigaLED()
{
    static uint8_t key;

    key = dadoRecebido();

    if(key > 0)
    {
        switch(key) {
            case '0': toggleLED(0); break;
            case '1': toggleLED(1); break;
            case '2': toggleLED(2); break;
            case '3': toggleLED(3); break;
            default: break;
        }
        enviaDado(key);
    }
}

void serial_enviaString(const char *str)
{
    while (*str) {
        enviaDado(*str++);
    }
}

void serial_enviaInteiro(int val)
{
    char buffer[11];
    int i = 0;

    if (val == 0) {
        enviaDado('0');
        return;
    }

    if (val < 0) {
        enviaDado('-');
        val = -val;
    }

    while (val > 0) {
        buffer[i++] = (val % 10) + '0';
        val /= 10;
    }

    while (i > 0) {
        enviaDado(buffer[--i]);
    }
}

void serial_enviaInt2Dig(int val)
{
    if (val < 0) val = 0;
    enviaDado((val / 10) % 10 + '0');
    enviaDado((val % 10) + '0');
}

void serial_enviaInt4Dig(int val)
{
    if (val < 0) val = 0;
    enviaDado((val / 1000) % 10 + '0');
    enviaDado((val / 100)  % 10 + '0');
    enviaDado((val / 10)   % 10 + '0');
    enviaDado(val % 10 + '0');
}

static void interpretaComando(const char *buf, int len)
{
    int k;
    for (k = 0; k < len && k < 31; k++) {
        char c = buf[k];
        if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
        lower[k] = c;
    }
    lower[k] = '\0';

    int i, match;

    /* --- GET alarmelow --- */
    if (lower[0]=='g' && lower[1]=='e' && lower[2]=='t' &&
        lower[3]=='a' && lower[4]=='l' && lower[5]=='a' &&
        lower[6]=='r' && lower[7]=='m' && lower[8]=='e' &&
        lower[9]=='l' && lower[10]=='o' && lower[11]=='w' && lower[12]=='\0') {
        serial_enviaString("#alarmeLow=");
        serial_enviaInteiro(getAlarmLevel_L());
        serial_enviaString("#\r\n");
        return;
    }

    /* --- GET alarmehigh --- */
    if (lower[0]=='g' && lower[1]=='e' && lower[2]=='t' &&
        lower[3]=='a' && lower[4]=='l' && lower[5]=='a' &&
        lower[6]=='r' && lower[7]=='m' && lower[8]=='e' &&
        lower[9]=='h' && lower[10]=='i' && lower[11]=='g' &&
        lower[12]=='h' && lower[13]=='\0') {
        serial_enviaString("#alarmeHigh=");
        serial_enviaInteiro(getAlarmLevel_H());
        serial_enviaString("#\r\n");
        return;
    }

    /* --- GET time --- */
    if (lower[0]=='g' && lower[1]=='e' && lower[2]=='t' &&
        lower[3]=='t' && lower[4]=='i' && lower[5]=='m' &&
        lower[6]=='e' && lower[7]=='\0') {
        serial_enviaString("#time=");
        serial_enviaInt2Dig(getHours());
        serial_enviaString(":");
        serial_enviaInt2Dig(getMinutes());
        serial_enviaString(":");
        serial_enviaInt2Dig(getSeconds());
        serial_enviaString("#\r\n");
        return;
    }

    /* --- GET sensor --- */
    if (lower[0]=='g' && lower[1]=='e' && lower[2]=='t' &&
        lower[3]=='s' && lower[4]=='e' && lower[5]=='n' &&
        lower[6]=='s' && lower[7]=='o' && lower[8]=='r' && lower[9]=='\0') {
        float valor_raw = getSensorLevel();
        char tensao[4];
        floatParaString(3.3 * valor_raw / (1023 * GANHO_AMPOP), tensao);
        serial_enviaString("#sensor=");
        serial_enviaInt4Dig((int) valor_raw);
        serial_enviaString("/");
        serial_enviaString(tensao);
        serial_enviaString("V#\r\n");
        return;
    }

    /* --- SET alarmehighh -> alarme high = 400 --- */
    if (strcmp(lower, "alarmehighh") == 0) {
        setAlarmLevel_H(400);
        salvarAlarmesEEPROM();
        serial_enviaString("#alarmeHigh=");
        serial_enviaInteiro(getAlarmLevel_H());
        serial_enviaString("#\r\n");
        return;
    }

    /* --- SET alarmehighl -> alarme high = 350 --- */
    if (strcmp(lower, "alarmehighl") == 0) {
        setAlarmLevel_H(350);
        salvarAlarmesEEPROM();
        serial_enviaString("#alarmeHigh=");
        serial_enviaInteiro(getAlarmLevel_H());
        serial_enviaString("#\r\n");
        return;
    }

    /* --- SET alarmelowh -> alarme low = 300 --- */
    if (strcmp(lower, "alarmelowh") == 0) {
        setAlarmLevel_L(300);
        salvarAlarmesEEPROM();
        serial_enviaString("#alarmeLow=");
        serial_enviaInteiro(getAlarmLevel_L());
        serial_enviaString("#\r\n");
        return;
    }

    /* --- SET alarmelowl -> alarme low = 250 --- */
    if (strcmp(lower, "alarmelowl") == 0) {
        setAlarmLevel_L(250);
        salvarAlarmesEEPROM();
        serial_enviaString("#alarmeLow=");
        serial_enviaInteiro(getAlarmLevel_L());
        serial_enviaString("#\r\n");
        return;
    }

    i = 0; match = 1;
    {
        const char *pfx = "alarmelow:";
        while (pfx[i] != '\0') {
            if (lower[i] != pfx[i]) { match = 0; break; }
            i++;
        }
    }
    if (match) {
        int valor = 0;
        while (buf[i] >= '0' && buf[i] <= '9') {
            valor = (valor * 10) + (buf[i] - '0');
            i++;
        }
        serial_alarme_L_pendente = valor;
        serial_enviaString("#alarmelow:");
        serial_enviaInteiro(valor);
        serial_enviaString(" recebido#\r\n");
        return;
    }

    i = 0; match = 1;
    {
        const char *pfx = "alarmehigh:";
        while (pfx[i] != '\0') {
            if (lower[i] != pfx[i]) { match = 0; break; }
            i++;
        }
    }
    if (match) {
        int valor = 0;
        while (buf[i] >= '0' && buf[i] <= '9') {
            valor = (valor * 10) + (buf[i] - '0');
            i++;
        }
        serial_alarme_H_pendente = valor;
        serial_enviaString("#alarmehigh:");
        serial_enviaInteiro(valor);
        serial_enviaString(" recebido#\r\n");
        return;
    }

    {
        int horas = 0, minutos = 0, segundos = 0, parte = 0;
        i = 0;
        while (buf[i] != '\0') {
            if (buf[i] >= '0' && buf[i] <= '9') {
                if      (parte == 0) horas    = (horas    * 10) + (buf[i] - '0');
                else if (parte == 1) minutos  = (minutos  * 10) + (buf[i] - '0');
                else if (parte == 2) segundos = (segundos * 10) + (buf[i] - '0');
            } else if (buf[i] == ':') {
                parte++;
            }
            i++;
        }
        if (parte == 2 && horas < 24 && minutos < 60 && segundos < 60) {
            setRTCHoras(horas);
            setRTCMinutos(minutos);
            setRTCSegundos(segundos);
            serial_enviaString("#time set#\r\n");
            return;
        }
    }

    if(lower[1] != '\0'){
    	serial_enviaString("#comando invalido#\r\n");
    }
}

int eventoSerial(int ev){
	if(lower[0] == 'c'){
		switch(lower[1]){
			case 'a':
				ev = EV_LEFT;
			break;
			case 'd':
				ev =  EV_RIGHT;
			break;
			case 'w':
				ev = EV_UP;
			break;
			case 's':
				ev = EV_DOWN;
			break;
			case 'e':
				ev = EV_ENTER;
			break;
			case 'q':
				ev = EV_CANCEL;
			break;
		}
	}

	if(ev != last){
		last = ev;
		return ev;
	} else {
		return EV_NOEVENT;
	}
}

/*
  Deve ser chamada a cada iteração do loop principal.

  Protocolo unificado: todos os comandos são terminados com '\n'.
  O '(' e ')' do protocolo legado são ignorados — o conteúdo entre
  eles é acumulado normalmente e interpretado ao chegar o '\n'.

  Exemplos de uso no terminal:
    getAlarmelow\n
    getAlarmehigh\n
    gettime\n
    getsensor\n
    alarmelow:200\n
    alarmehigh:800\n
    14:30:00\n
 */
void processaComandoSerial(void)
{
    static char cmd_buf[32];
    static int  cmd_idx = 0;
    uint8_t byte_lido;

    while (Chip_UART_ReadLineStatus(LPC_USART) & UART_LSR_RDR)
    {
        byte_lido = Chip_UART_ReadByte(LPC_USART);
        enviaDado(byte_lido);
        if (byte_lido == '(' || byte_lido == ')' || byte_lido == ' ') {
            continue;
        }

        if (byte_lido == '\r' || byte_lido == '\n') {
            if (cmd_idx > 0) {
                cmd_buf[cmd_idx] = '\0';
                interpretaComando(cmd_buf, cmd_idx);
                cmd_idx = 0;
            }
            continue;
        }

        if (cmd_idx < (int)(sizeof(cmd_buf) - 1)) {
            cmd_buf[cmd_idx++] = byte_lido;
        } else {
            cmd_idx = 0;
        }
    }
}
