#include "EEPROM.h"

void iniciaEEPROM(void) {
    Chip_GPIO_SetPinDIR(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 1);
    Chip_GPIO_SetPinState(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 1);
}

int EEPROM_EscreverByte(uint16_t endereco_memoria, uint8_t dado) {
    uint8_t buffer_i2c[3];
    int status;

    buffer_i2c[0] = (endereco_memoria >> 8) & 0xFF;
    buffer_i2c[1] = endereco_memoria & 0xFF;
    buffer_i2c[2] = dado;

    Chip_GPIO_SetPinState(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 0);

    status = I2C_Transmitir(EEPROM_ADDR, buffer_i2c, 3);

    if (status == I2C_OK) {
        delayMS(5); // tempo obrigatorio de gravacao fisica no silicio
    }

    Chip_GPIO_SetPinState(LPC_GPIO, EEPROM_WP_PORT, EEPROM_WP_PIN, 1);

    return status;
}

int EEPROM_LerByte(uint16_t endereco_memoria, uint8_t *dado) {
    uint8_t buffer_endereco[2];
    int status;

    buffer_endereco[0] = (endereco_memoria >> 8) & 0xFF;
    buffer_endereco[1] = endereco_memoria & 0xFF;

    status = I2C_Transmitir(EEPROM_ADDR, buffer_endereco, 2);
    if (status != I2C_OK) return status;

    return I2C_Receber(EEPROM_ADDR, dado, 1);
}

int EEPROM_EscreverInt(uint16_t endereco_memoria, int valor) {
    int status = EEPROM_EscreverByte(endereco_memoria, (uint8_t)((valor >> 8) & 0xFF));
    if (status != I2C_OK) return status;
    return EEPROM_EscreverByte(endereco_memoria + 1, (uint8_t)(valor & 0xFF));
}

int EEPROM_LerInt(uint16_t endereco_memoria, int *valor) {
    uint8_t msb, lsb;
    int status;

    status = EEPROM_LerByte(endereco_memoria, &msb);
    if (status != I2C_OK) return status;

    status = EEPROM_LerByte(endereco_memoria + 1, &lsb);
    if (status != I2C_OK) return status;

    *valor = (int)((msb << 8) | lsb);
    return I2C_OK;
}
