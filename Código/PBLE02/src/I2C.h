#ifndef I2C_H
#define I2C_H
	#include "programa.h"

	#define I2C_TIMEOUT -1

	void iniciaI2C();
	int I2C_Transmitir(unsigned char endereco, unsigned char *valor, unsigned char qtd);
	int I2C_Receber(unsigned char endereco, unsigned char *valor, unsigned char qtd);
#endif
