/**
 * @file init.c
 * @brief Implementacije funkcija za inicijalizaciju periferija mikrokontrolera
 * @author Jovan Blanusa, 47/2012 (jovan.blanusa@gmail.com)
 * @date 2016
 */
#include "init.h"
#include "oled.h"

/**
 * Konstanta koja definise ucestanost koju koristi Timer A
 */
#define OLED_REFRESH_FREQUENCY 1024


/**
 * @brief Inicijalizacija AD konvertora
 *
 * Funkcija koja konfigurise hardver AD konvertora.
 * Multipleksira odgovarajuce pinove tako da se koriste za AD konvertor.
 * Konfigurise AD konvertor da radi u rezimu Repeat Sequence of channels.
 * Koristi memorijske lokacije 0 i 1 za smestanje rezultata konverzije.
 */
void initADC(void)
{
	/* multipleksiranje pinova */
	P7SEL |= BIT6 + BIT7;

	/* podesavanje AD konvertora */
	ADC12CTL0 = ADC12ON + ADC12MSC;		/* koristi MSC */
	ADC12CTL1 = ADC12SHS_1 + ADC12CONSEQ_1 + ADC12SHP; // Proveriti shs i conseq
	ADC12MCTL0 = ADC12INCH_14;
	ADC12MCTL1 = ADC12INCH_9;
	ADC12IE |= ADC12IE0 + ADC12IE1;	/* dozvoli sve prekide */
	ADC12CTL0 |= ADC12ENC;		/* dozvoli konverziju */

}

/**
 * @brief Inicijalizacija tajmera A0
 *
 * Tajmer A0 se koristi za generisanje periodicnog prekida
 * na koji ce se osvezavati displej.
 */
void initTMRA(void)
{
    TA0CCTL0 = OUTMOD_4 + CCIE;		// outmod = toggle
    TA0CCR0 = OLED_REFRESH_FREQUENCY;
    TA0CTL = TASSEL_1 + MC_1;	// SMCLK, up mode
}

/**
 * @brief Inicijalizacija SPI B0
 *
 * SPI se koristi za komunikaciju sa OLED W click plocicom preko MikroBus magistrale.
 * Click plocica se nalazi na MikroBusu 1, pa je odgovarajuci SPI B0.
 */
void initSPIB(void)
{
    P3SEL |= BIT3 + BIT2 + BIT1;		 // funkcija pinova 3,2,1 porta 3 dodeljena SPI-u
    P3DIR |= BIT3 + BIT2 + BIT1;

    UCB0CTL1 = UCSWRST;     // Put state machine in reset
    UCB0CTL0 |= UCMST +     // MSP je master
    		  + UCSYNC		// Sinhroni mod
			  + UCMSB		// Prvo se salje MSB
			  + UCMODE_0	// UCMODE_0 -> 3pin SPI; UCMODE_1 ->  4pin SPI,Slave aktivan na UCxSTE = 1;  UCMODE_2 4pin SPI,Slave aktivan na UCxSTE = 0;
			  + UCCKPL;		// UCxCLK neaktivno stanje 0
	UCB0CTL1 |= UCSSEL_2;	// USCI Clock Source: SMCLK
    UCB0BR0 |= 0x01;
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;

}

/**
 * @brief Inicijalizacija hardvera potrebnog za komunikaciju preko MikroBusa sa Click plocicom
 *
 * Inicijalizuje se SPI B0 i uz to jos tri pina koji ce sluziti kao izlazni pinovi CS, DC i RST
 * i predstavljaju deo MikroBus interfejsa.
 */
void initMBUS1(void)
{
	initSPIB();
	P3DIR |= BIT0; // CS  bit za oled
	P4DIR |= BIT1; // DC  bit za oled
	P2DIR |= BIT0; // RST bit za oled

}

/**
 * @brief Inicijalizacija tastera 4
 *
 * Odgovarajuci pin porta 2 se konfigurise kao ulazni.
 */
void initBUTTON(void)
{
	P2IE |= BIT7;
}
