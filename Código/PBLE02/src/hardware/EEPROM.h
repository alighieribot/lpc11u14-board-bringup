#ifndef E2PROM_H
#define E2PROM_H

#include "../programa.h"

#define EEPROM_ADDR 0x50

#define EEPROM_WP_PORT 0
#define EEPROM_WP_PIN  3

#define EEPROM_ADDR_MAGIC       0x00
#define EEPROM_ADDR_IDIOMA      0x01
#define EEPROM_ADDR_ALARME_H    0x02
#define EEPROM_ADDR_ALARME_L    0x04
#define EEPROM_ADDR_HORAS       0x06
#define EEPROM_ADDR_MINUTOS     0x07
#define EEPROM_ADDR_SEGUNDOS    0x08

#define EEPROM_MAGIC_VALUE      0xA5

void iniciaEEPROM(void);

int EEPROM_EscreverByte(uint16_t endereco_memoria, uint8_t dado);
int EEPROM_LerByte(uint16_t endereco_memoria, uint8_t *dado);

int EEPROM_EscreverInt(uint16_t endereco_memoria, int valor);
int EEPROM_LerInt(uint16_t endereco_memoria, int *valor);

#endif
