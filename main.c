/**
 * @file main.c
 * @brief IRS Projekat - Pong igrica
 * @author Jovan Blanusa, 47/2012 (jovan.blanusa@gmail.com)
 * @date 2016
 *
 * Implementirana je Pong igrica za dva igraca.
 *  _________________________________
 * 	|			 12 ' 4				|
 * 	|	| 			'				|
 * 	|	|		o	'			|	|
 * 	|				'			|	|
 * 	|				'				|
 *  |_______________'_______________|
 *
 *  Prikaz se vrsi na Click ploci Oled W:
 *   http://www.mikroe.com/click/oled-w/
 *
 *  Igraci se upravljaju pomocu potenciometara povezanih na AD konvertore
 */
#include <msp430.h> 
#include <stdint.h>

#include "init.h"
#include "game.h"
#include "oled.h"

/**
 * Indikator koji postavlja tajmer u prekidu i signalizira programu
 * da treba da osvezi ekran.
 */
volatile uint8_t TimerFlag = 0;

/**
 * Indikator koji se postavlja pritiskom tastera S4 i po pritisku
 * igra pocinje.
 */
volatile uint8_t ResetGame = 0;	//Igra se resetuje na pocetku

/**
 * Vrednosti potenciometara koji predstavljaju polozaj igraca.
 * Ucitavaju se u prekidnoj rutini AD konvertora
 */
volatile unsigned int adc1val = MAX_ADC_VAL/2, adc2val = MAX_ADC_VAL/2;

extern const uint8_t start_screen[];

/*
 * @brief Glavna funkcija
 *
 * Funkcija inicijalizuje hardver pa potom u beskonacnoj petlji
 * proverava da li je tajmer postavio svoj indikator. Ako jeste
 * osvezava ekran pozivom funkcije RefreshScreen.
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    initADC();
	initTMRA();
	initMBUS1();
	initBUTTON();
	OLED_Initialize();
    __bis_SR_register(GIE);		// globalna dozvola maskirajucih prekida
    OLED_PutPicture(start_screen);

    while(1)
    {
    	while(!ResetGame);
//    	ResetGame = 0;
    	if(TimerFlag){
    		RefreshScreen(adc1val >> 7, adc2val >> 7, ResetGame);  //Maksimalno 32 polozaja plocice
    		TimerFlag = 0;
    	}
    	ResetGame = 1;
    }
}

/**
 * @brief Prekidna rutina tastera S4
 *
 * U prekidnoj rutini se postavlja fleg koji oznacava da treba poceti
 * igru, pa se potom brise indikator prekida iz Interrupt Flag registra.
 */
#pragma vector=PORT2_VECTOR
__interrupt void port2handler(void)
{
	ResetGame = 1;		// registruje se prekid tastera
	P2IFG &= ~BIT7;				// brisanje flega
}

/**
 * @brief Prekidna rutina TajmerA0
 *
 * Zadatak prekidne rutine je samo da postavi indikator da je tajmer
 * izmerio odredjeno vreme i da je vreme da se prikaze novi frejm na
 * Oled W.
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	TimerFlag = 1;
}
