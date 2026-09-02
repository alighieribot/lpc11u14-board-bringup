#include <stdint.h>
#include "output.h"
#include "../programa.h"

#define NUM_IDIOMAS 2

static const char * const msgs[STATE_FIM][NUM_IDIOMAS] = {
    /* STATE_ALARME       */ {"Alarme atual:   ", "Current alarm:  "},
    /* STATE_ALARME_L     */ {"Alterar alarme L", "Change alarm  L "},
    /* STATE_ALARME_H     */ {"Alterar alarme H", "Change alarm  H "},
    /* STATE_HORAS        */ {"Tempo atual     ", "Current time    "},
    /* STATE_MUDAR_HORAS  */ {"Alterar tempo   ", "Change time     "},
    /* STATE_IDIOMA       */ {"Idioma:         ", "Language:       "},
    /* STATE_MUDAR_IDIOMA */ {"Alterar idioma  ", "Change language "},
    /* STATE_SENSOR       */ {"Tensao no sensor", "Sensor voltage  "},
    /* STATE_OUT_OF_RANGE */ {"CUIDADO!        ", "WARNING!        "},
};

/* Nomes dos idiomas exibidos na linha 1 dos estados de idioma.
 * Índice = valor retornado por getLanguage(). */
static const char * const nomes_idioma[NUM_IDIOMAS] = {
    "Portugues       ",
    "English         ",
};

void outputInit(void) {
    /* iniciaLCD() é chamado em iniciaSM(); não duplicar aqui */
}

void outputPrint(int numTela, int idioma) {

    if (numTela < 0 || numTela >= STATE_FIM) return;

    LCD_setCursor(0, 0);
    LCD_string(msgs[numTela][idioma]);

    switch (numTela) {

        case STATE_IDIOMA:
            LCD_setCursor(1, 0);
            LCD_esconderCursor();
            LCD_string(nomes_idioma[idioma]);
            break;

        case STATE_MUDAR_IDIOMA:
            LCD_setCursor(1, 0);
            LCD_mostrarCursor();
            LCD_string(nomes_idioma[idioma]);
            break;

        default:
            break;
    }
}
