#include "var.h"
#include "../util.h"
#include "../hardware/RTC.h"
#include "../hardware/EEPROM.h"

static char state;
static char language;
static int time;
static unsigned int alarmLevel_H;
static unsigned int alarmLevel_L;
static unsigned int sensorLevel;
static float sensorLevel_V;

static unsigned int seconds;
static unsigned int minutes;
static unsigned int hours;

static void usarValoresPadrao(void) {
    language     = 0;
    alarmLevel_H = 350;
    alarmLevel_L = 250;
    hours        = 22;
    minutes      = 58;
    seconds      = 0;
}

static void carregarConfigEEPROM(void) {
    uint8_t magic = 0xFF;
    int aH, aL;

    /* Se a leitura falhar (EEPROM ausente/desconectada), usa padrao
     * e nao tenta gravar (evita ficar preso tentando de novo). */
    if (EEPROM_LerByte(EEPROM_ADDR_MAGIC, &magic) != I2C_OK) {
        usarValoresPadrao();
        return;
    }

    if (magic != EEPROM_MAGIC_VALUE) {
        /* Primeira vez ligando (ou memoria em branco): grava os padroes */
        usarValoresPadrao();

        EEPROM_EscreverByte(EEPROM_ADDR_IDIOMA, (uint8_t)language);
        EEPROM_EscreverInt (EEPROM_ADDR_ALARME_H, (int)alarmLevel_H);
        EEPROM_EscreverInt (EEPROM_ADDR_ALARME_L, (int)alarmLevel_L);
        EEPROM_EscreverByte(EEPROM_ADDR_HORAS,    (uint8_t)hours);
        EEPROM_EscreverByte(EEPROM_ADDR_MINUTOS,  (uint8_t)minutes);
        EEPROM_EscreverByte(EEPROM_ADDR_SEGUNDOS, (uint8_t)seconds);
        EEPROM_EscreverByte(EEPROM_ADDR_MAGIC, EEPROM_MAGIC_VALUE);
    } else {
        /* Ja existe configuracao salva: recupera (com fallback se falhar) */
        uint8_t idiomaLido, horasLidas, minutosLidos, segundosLidos;

        if (EEPROM_LerByte(EEPROM_ADDR_IDIOMA, &idiomaLido) == I2C_OK)
            language = (char)(idiomaLido % 2);

        if (EEPROM_LerInt(EEPROM_ADDR_ALARME_H, &aH) == I2C_OK)
            alarmLevel_H = (unsigned int)aH;

        if (EEPROM_LerInt(EEPROM_ADDR_ALARME_L, &aL) == I2C_OK)
            alarmLevel_L = (unsigned int)aL;

        if (EEPROM_LerByte(EEPROM_ADDR_HORAS, &horasLidas) == I2C_OK)
            hours = horasLidas;

        if (EEPROM_LerByte(EEPROM_ADDR_MINUTOS, &minutosLidos) == I2C_OK)
            minutes = minutosLidos;

        if (EEPROM_LerByte(EEPROM_ADDR_SEGUNDOS, &segundosLidos) == I2C_OK)
            seconds = segundosLidos;
    }
}

void varInit(void) {
    iniciaEEPROM();

    time = 1000;
    sensorLevel = 300;

    carregarConfigEEPROM();
}

/* ... restante identico ao anterior (getters/setters, ajustaTempo) ... */

char getState(void) { return state; }
void setState(char newState) { state = newState; }

int getTime(void) { return time; }
void setTime(int newTime) { time = newTime; }

int getAlarmLevel_H(void) { return alarmLevel_H; }
void setAlarmLevel_H(int newAlarmLevel) { alarmLevel_H = newAlarmLevel; }

int getAlarmLevel_L(void) { return alarmLevel_L; }
void setAlarmLevel_L(int newAlarmLevel) { alarmLevel_L = newAlarmLevel; }

int getSensorLevel(void) { return sensorLevel; }
void setSensorLevel(int newSensorLevel) {
	sensorLevel_V = 3.3 * newSensorLevel / (1023.0 * GANHO_AMPOP);
	sensorLevel = newSensorLevel;
}
float getSensorLevel_V(void) { return sensorLevel_V; }

char getLanguage(void){ return language; }
void setLanguage(char newLanguage){ language = newLanguage % 2; }

int getSeconds(void) { return seconds; }
void setSeconds(int newSeconds) { seconds = newSeconds; }

int getMinutes(void) { return minutes; }
void setMinutes(int newMinutes) { minutes = newMinutes; }

int getHours(void) { return hours; }
void setHours(int newHours) { hours = newHours; }

void ajustaTempo(int* s, int* m, int* h)
{
    while (*s >= 60) { *s -= 60; (*m)++; }
    while (*s < 0)   { *s += 60; (*m)--; }
    while (*m >= 60) { *m -= 60; (*h)++; }
    while (*m < 0)   { *m += 60; (*h)--; }
    while (*h >= 24) *h -= 24;
    while (*h < 0)   *h += 24;

    setSeconds(*s);
    setMinutes(*m);
    setHours(*h);
}

void salvarIdiomaEEPROM(void) {
    EEPROM_EscreverByte(EEPROM_ADDR_IDIOMA, (uint8_t)language);
}

void salvarAlarmesEEPROM(void) {
    EEPROM_EscreverInt(EEPROM_ADDR_ALARME_H, (int)alarmLevel_H);
    EEPROM_EscreverInt(EEPROM_ADDR_ALARME_L, (int)alarmLevel_L);
}

void salvarHorarioEEPROM(void) {
    EEPROM_EscreverByte(EEPROM_ADDR_HORAS,    (uint8_t)hours);
    EEPROM_EscreverByte(EEPROM_ADDR_MINUTOS,  (uint8_t)minutes);
    EEPROM_EscreverByte(EEPROM_ADDR_SEGUNDOS, (uint8_t)seconds);
}
