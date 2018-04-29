/**
 * @file init.h
 * @brief Deklaracija funkcija za inicijalizaciju periferija mikrokontrolera
 * @author Jovan Blanusa, 47/2012 (jovan.blanusa@gmail.com)
 * @date 2016
 */

#ifndef INIT_H_
#define INIT_H_

#include <msp430.h>
#include <stdint.h>

/**
 * @brief Inicijalizacija AD konvertora
 */
void initADC(void);

/**
 * @brief Inicijalizacija Tajmera A
 */
void initTMRA(void);

/**
 * @brief Inicijalizacija SPI B0 hardvera
 */
void initSPIB(void);

/**
 * @brief Inicijalizacija hardvera potrebnog za komunikaciju preko MikroBusa sa Click plocicom
 */
void initMBUS1(void);

/**
 * @brief Inicijalizacija tastera
 */
void initBUTTON(void);

#endif /* INIT_H_ */
