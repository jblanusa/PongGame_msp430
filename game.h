/**
 * @file game.h
 * @brief Deklaracija funkcija koji se koriste za realizaciju Pong igre
 * @author Jovan Blanusa, 47/2012 (jovan.blanusa@gmail.com)
 * @date 2016
 */
#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

/**
 * Maksimalna vrednost koju AD konvertor moze da generise
 */
#define MAX_ADC_VAL 4095

/**
 * @brief Funkcija koja osvezava ekran na prekid tajmera
 * @param Polozaj prvog igraca
 * @param Polozaj drugog igraca
 */
void RefreshScreen(unsigned int adc1, unsigned int adc2, uint8_t reset);

/**
 * @brief Iscrtavanje igraca
 */
void DrawBoard();

/**
 * @brief Brisanje igraca
 */
void RemoveBoard();

/**
 * @brief Iscrtavanje loptice
 */
void DrawBall();

/**
 * @brief  Brisanje loptice
 */
void RemoveBall();

/**
 * @brief Ispisivanje rezultata
 */
void WriteResult();

/**
 * @brief Odredjivanje sledeceg polozaja loptice
 */
void NextState();

/**
 * @brief Ponovno iscrtavanje sredista terena
 */
void RedrawMiddle();

#endif /* GAME_H_ */
