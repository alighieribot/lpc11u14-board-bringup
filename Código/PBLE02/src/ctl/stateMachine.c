#include <stdint.h>
#include "var.h"
#include "stateMachine.h"
#include "event.h"
#include "output.h"
#include "../programa.h"
#include "../hardware/LED.h"
#include "../hardware/RTC.h"

static int aux[3];
static char* outputAux;
static uint8_t posCursor = 0;
uint8_t blink = 0;

static unsigned int ultimoSegundo = 0;

void iniciaSM(void) {
    setState(STATE_HORAS);
    varInit();
    iniciaLCD();
    iniciaADC();
    iniciaLED();
    LCD_clear();
	LCD_setCursor(0, 0);
	outputPrint(getState(), getLanguage());
	ultimoSegundo = millis_counter;
}

char* paraString(int entrada){
    static char saida[16];

    int divisor = 1;

    while (entrada / divisor >= 10){
        divisor *= 10;
    }

    int i = 0;

    while (divisor > 0){
        saida[i++] = '0' + entrada / divisor;
        entrada %= divisor;
        divisor /= 10;
    }

    saida[i] = '\0';

    return saida;
}

void smLoop(void) {
    unsigned char evento;

    setSensorLevel(leSensor());

    if (getSensorLevel() > getAlarmLevel_H()) {
        /* Acima do alarme H: acende LED 1, garante LED 0 apagado */
        ligaLED(1);
        desligaLED(0);
    } else if (getSensorLevel() < getAlarmLevel_L()) {
        /* Abaixo do alarme L: acende LED 0, garante LED 1 apagado */
        ligaLED(0);
        desligaLED(1);
    } else {
        desligaLED(0);
        desligaLED(1);
    }

    if (millis_counter - ultimoSegundo >= 1000) {
        ultimoSegundo += 1000;

        int s = getSeconds() + 1;
        int m = getMinutes();
        int h = getHours();
        ajustaTempo(&s, &m, &h);
    }

    //maquina de estados
    evento = eventRead();

    evento = eventoSerial(evento);

    if(evento == EV_ENTER){ // garantir que posCursor de horas esteja na posicao 0
        posCursor = 0;
    }

		switch (getState()) {
			//estados de exibição
			case STATE_HORAS:
				LCD_setCursor(1, 0);
				LCD_esconderCursor();
				LCD_int2Dig(getHours());
				LCD_caractere(':');
				LCD_int2Dig(getMinutes());
				LCD_caractere(':');
				LCD_int2Dig(getSeconds());

				if(evento == EV_ENTER){
					aux[0] = getHours();
					aux[1] = getMinutes();
					aux[2] = getSeconds();
					setState(STATE_MUDAR_HORAS);
				}
				if(evento == EV_DOWN)
					setState(STATE_ALARME);

				if(evento == EV_UP)
					setState(STATE_IDIOMA);
			break;

			case STATE_IDIOMA:
				if(evento == EV_ENTER){
					aux[0] = getLanguage();
					setState(STATE_MUDAR_IDIOMA);
				}

				if(evento == EV_UP)
					setState(STATE_SENSOR);

				if(evento == EV_DOWN)
					setState(STATE_HORAS);
			break;

			case STATE_MUDAR_IDIOMA:
				if(evento == EV_RIGHT || evento == EV_DOWN){
					int novoIdioma = (getLanguage() + 1) % NUM_IDIOMAS;
					setLanguage(novoIdioma);
				}

				if(evento == EV_LEFT || evento == EV_UP){
					int novoIdioma = (getLanguage() - 1 + NUM_IDIOMAS) % NUM_IDIOMAS;
					setLanguage(novoIdioma);
				}

				/* ENTER: confirma — idioma já está salvo em getLanguage() */
				if(evento == EV_ENTER){
					setState(STATE_IDIOMA);
					salvarIdiomaEEPROM();
				}

				/* CANCEL: restaura idioma original e volta sem salvar */
				if(evento == EV_CANCEL){
					setLanguage(aux[0]);
					setState(STATE_IDIOMA);
				}
			break;

			case STATE_ALARME:
				if(evento == EV_ENTER){
					//anotando valores caso usuario queira cancelar operacao
					aux[0] = getAlarmLevel_L();
					aux[1] = getAlarmLevel_H();
					setState(STATE_ALARME_H);
				}

				if(evento == EV_DOWN)
					setState(STATE_SENSOR);

				if(evento == EV_UP)
					setState(STATE_HORAS);

				LCD_esconderCursor();
				LCD_setCursor(1, 0);
				LCD_string("H: ");
				LCD_string(paraString(getAlarmLevel_H()));
				LCD_string(" L: ");
				LCD_string(paraString(getAlarmLevel_L()));
			break;

			case STATE_SENSOR: {
				/* Mostra a tensao lida no sensor (ex: "2.75V") na linha 1. */
				float tensao = getSensorLevel_V();
				int parteInteira = (int)tensao / 5.45 * 3.3;
				int parteDecimal = (int)((tensao - parteInteira) * 100 + 0.5f);
				if (parteDecimal >= 100) {
					parteDecimal -= 100;
					parteInteira++;
				}

				LCD_setCursor(1, 0);
				LCD_esconderCursor();
				LCD_string(paraString(parteInteira));
				LCD_caractere('.');
				if (parteDecimal < 10)
					LCD_caractere('0');
				LCD_string(paraString(parteDecimal));
				LCD_string("V   ");

				if(evento == EV_UP)
					setState(STATE_ALARME);

				if(evento == EV_DOWN)
					setState(STATE_IDIOMA);
			} break;

			case STATE_ALARME_L:
				//ir para segunda linha
				LCD_setCursor(1, 0);
				LCD_mostrarCursor();

				//Converter o numero para string
				outputAux = paraString(aux[0]);
				LCD_string(outputAux);

				//se ir para direita, variavel vai ser incrementada se tambem for menor que o H aux
				if(aux[0] < aux[1] && evento == EV_RIGHT){
					aux[0]++;
				}

				if(aux[0] > 0 && evento == EV_LEFT){
					aux[0]--;
				}

				if(evento == EV_UP){
					setAlarmLevel_L(aux[0]);
					setAlarmLevel_H(aux[1]);
					setState(STATE_ALARME_H);
				}

				//pode voltar ao STATE_ALARM quando confirmar operações, sem necessariamente ir para STATE_ALARM_L
				if(evento == EV_ENTER){
					setAlarmLevel_L(aux[0]);
					setAlarmLevel_H(aux[1]);
					setState(STATE_ALARME);
					salvarAlarmesEEPROM();
				}

				if(evento == EV_CANCEL)
					setState(STATE_ALARME);
			break;

			case STATE_ALARME_H:
				//ir para segunda linha
				LCD_setCursor(1, 0);
				LCD_mostrarCursor();

				//Converter o numero para string
				outputAux = paraString(aux[1]);
				LCD_string(outputAux);

				//se ir para esquerda, variavel vai ser incrementada se tambem for maior que o L aux
				if(aux[1] > aux[0] && evento == EV_LEFT){
					aux[1]--;
				}

				//supondo que acima de 1000 seja um valor muito absurdo
				if(aux[1] < 1000 && evento == EV_RIGHT){
					aux[1]++;
				}

				if(evento == EV_DOWN){
					setAlarmLevel_L(aux[0]);
					setAlarmLevel_H(aux[1]);
					setState(STATE_ALARME_L);
				}

				//pode voltar ao STATE_ALARM quando confirmar operações, sem necessariamente ir para STATE_ALARM_L
				if(evento == EV_ENTER){
					setAlarmLevel_L(aux[0]);
					setAlarmLevel_H(aux[1]);
					setState(STATE_ALARME);
					salvarAlarmesEEPROM();
				}

				if(evento == EV_CANCEL)
					setState(STATE_ALARME);

				break;

			case STATE_MUDAR_HORAS:
				//impressao das variaveis temporarias
				LCD_setCursor(1, 0);
				LCD_int2Dig(aux[0]);
				LCD_caractere(':');
				LCD_int2Dig(aux[1]);
				LCD_caractere(':');
				LCD_int2Dig(aux[2]);

				//blink
				blink++;
				if(blink > 3){
					LCD_setCursor(1, posCursor);
					LCD_caractere(' ');
					blink = 0;
				}

				LCD_setCursor(1, posCursor);

				//Se estiver em ':', pular posicao
				if(evento == EV_UP && posCursor > 0){
					if(posCursor == 3 || posCursor == 6)
						posCursor -= 2;
					else
						posCursor--;
				}

				//mesma coisa de cima só que da esquerda para direita
				if(evento == EV_DOWN && posCursor < 7){
					if(posCursor == 1 || posCursor == 4)
						posCursor += 2;
					else
						posCursor++;
				}

				switch(posCursor){
					//definir horas em aux[0]
					case 0:
						if(aux[0] + 10 < 24 && evento == EV_RIGHT)
							aux[0] += 10;
						if(aux[0] - 10 >= 0 && evento == EV_LEFT)
							aux[0] -= 10;
					break;
					case 1:
						if(aux[0] + 1 < 24 && evento == EV_RIGHT)
							aux[0]++;
						if(aux[0] - 1 >= 0 && evento == EV_LEFT)
							aux[0]--;
					break;

					//definir minutos em aux[1]
					case 3:
						if(aux[1] + 10 < 60 && evento == EV_RIGHT)
							aux[1] += 10;
						if(aux[1] - 10 >= 0 && evento == EV_LEFT)
							aux[1] -= 10;
					break;
					case 4:
						if(aux[1] + 1 < 60 && evento == EV_RIGHT)
							aux[1]++;
						if(aux[1] - 1 >= 0 && evento == EV_LEFT)
							aux[1]--;
					break;

					//definir segundos em aux[2]
					case 6:
						if(aux[2] + 10 < 60 && evento == EV_RIGHT)
							aux[2] += 10;
						if(aux[2] - 10 >= 0 && evento == EV_LEFT)
							aux[2] -= 10;
					break;
					case 7:
						if(aux[2] + 1 < 60 && evento == EV_RIGHT)
							aux[2]++;
						if(aux[2] - 1 >= 0 && evento == EV_LEFT)
							aux[2]--;
					break;

					default:
						//pass
					break;
				}

				if(evento == EV_ENTER){
					setHours(aux[0]);
					setMinutes(aux[1]);
					setSeconds(aux[2]);
					setState(STATE_HORAS);
					salvarHorarioEEPROM();
				}

				if(evento == EV_CANCEL)
					setState(STATE_HORAS);

				break;

			case STATE_OUT_OF_RANGE:
				if(getSensorLevel() <= getAlarmLevel_H() && getSensorLevel() >= getAlarmLevel_L()){
					setState(STATE_HORAS);
				}
				LCD_setCursor(1, 0);
				LCD_string("tese");

				//alarmes e tal, nn sei se vai ter mais alguma coisa
				break;
		}

		outputPrint(getState(), getLanguage());
}
