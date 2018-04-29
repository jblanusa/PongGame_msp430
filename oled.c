/**
 * @file oled.c
 * @brief Implementacije funkcija za interfejs sa OLED W Click plocicom
 * @author Jovan Blanusa, 47/2012 (jovan.blanusa@gmail.com)
 * @date 2016
 */
#include <msp430.h>
#include "oled.h"

/**
 * Postavljanje bita CS na MikroBus magistrali
 */
#define SET_CS      	P3OUT |=  BIT0

/**
 * Brisanje bita CS na MikroBus magistrali
 */
#define RESET_CS		P3OUT &= ~BIT0

/**
 * Postavljanje bita DC na MikroBus magistrali
 */
#define SET_DC			P4OUT |=  BIT1

/**
 * Brisanje bita DC na MikroBus magistrali
 */
#define RESET_DC		P4OUT &= ~BIT1

/**
 * Postavljanje bita RST na MikroBus magistrali
 */
#define SET_RST			P2OUT |=  BIT0

/**
 * Brisanje bita RST na MikroBus magistrali
 */
#define RESET_RST		P2OUT &= ~BIT0

/**
 * @brief Funkcija koja prosledjuje bajt kontroleru SSD1306 preko SPI magistrale
 * @param Podatak ili komanda koja se salje
 *
 * Funkcija prosledjuje podatak na SPI magistralu tako sto u predajni registar
 * upise zeljeni podatak. Takodje je potrebno sacekati da se taj podatak posalje
 * testiranjem BUSY bita iz statusnog registra.
 */
int SPI_B0_Write(uint8_t data)
{
	while(UCB0STAT & UCBUSY);
	UCB0TXBUF = data;
	while(UCB0STAT & UCBUSY);
	return 0;
}

/**
 * @brief Slanje komande kontroleru za OLED
 * @param Komanda koja se salje
 *
 * Da bi se poslala komanda, potrebno je resetovati CS signal, jer je aktivan na
 * niskom logickom nivou. Takodje je potrebno resetovati DC signal da bi se naznacilo
 * da je u pitanju komanda.
 */
void OLED_Command(uint8_t temp)
{
  RESET_CS;
  RESET_DC;
  SPI_B0_Write(temp);
  SET_CS;
}

/**
 * @brief Slanje podatka kontroleru za OLED
 * @param Podatak koja se salje
 *
 * Da bi se poslalo podatak, potrebno je resetovati CS signal, jer je aktivan na
 * niskom logickom nivou. Takodje je potrebno setovati DC signal da bi se naznacilo
 * da je u pitanju podatak.
 */
void OLED_Data(uint8_t temp)
{
  RESET_CS;
  SET_DC;
  SPI_B0_Write(temp);
  SET_CS;
}

/**
 * @brief Inicijalizacija OLED displeja
 *
 * Da bi se inicijalizovao OLED displej potrebno je proslediti
 * niz odredjenih komandi i da se ispostuju odredjena vremena
 * koja predstavljaju koliko dugo mora RST signal biti aktivan
 * odnosno neaktivan.
 */
void OLED_Initialize()
{
	RESET_RST;
	__delay_cycles(1048576);
	SET_RST;
	__delay_cycles(1048576);
    OLED_Command(SSD1306_DISPLAYOFF);             //0xAE  Set OLED Display Off
    OLED_Command(SSD1306_SETDISPLAYCLOCKDIV);     //0xD5  Set Display Clock Divide Ratio/Oscillator Frequency
    OLED_Command(0x80);
    OLED_Command(SSD1306_SETMULTIPLEX);           //0xA8  Set Multiplex Ratio
    OLED_Command(39);

    OLED_Command(SSD1306_SETSEGMENTREMAP);        //0xA1  Set Segment Remap Inv
    OLED_Command(SSD1306_COMSCANDEC);             //0xC8  Set COM Output Scan Inv
    OLED_Command(SSD1306_SETSTARTLINE);           //0x40  Set Display Start Line

    OLED_Command(SSD1306_SETDISPLAYOFFSET);       //0xD3  Set Display Offset
    OLED_Command(0x00);
    OLED_Command(SSD1306_CHARGEPUMP);             //0x8D  Set Charge Pump
    OLED_Command(0x14);                           //0x14  Enable Charge Pump
    OLED_Command(SSD1306_SETCOMPINS);             //0xDA  Set COM Pins Hardware Configuration
    OLED_Command(0x12);
    OLED_Command(SSD1306_SETCONTRAST);            //0x81   Set Contrast Control
    OLED_Command(0xAF);
    OLED_Command(SSD1306_SETPRECHARGE);           //0xD9   Set Pre-Charge Period
    OLED_Command(0x25);
    OLED_Command(SSD1306_SETVCOMDETECT);          //0xDB   Set VCOMH Deselect Level
    OLED_Command(0x20);
    OLED_Command(SSD1306_DISPLAYALLON_RESUME);    //0xA4   Set Entire Display On/Off
    OLED_Command(SSD1306_NORMALDISPLAY);          //0xA6   Set Normal/Inverse Display
    OLED_Command(SSD1306_DISPLAYON);              //0xAF   Set OLED Display On
}

/**
 * @brief Postavljanje trenutne vrednosti reda
 * @param Trenutna vrednost reda
 *
 * Postoji 5 redova od po 8 piksela u koriscenom OLED displeju.
 * Komanda koja se prosledjuje displeju potrebno je da se sastoji
 * od prva 4 bita  '1010' i potom 4 bita koji predstavljaju broj
 * reda.
 */
void OLED_SetRow(uint8_t add)
{
    add = 0xB0 | add; // Moguce kolone B0, B1, B2, B3, B4
    OLED_Command(add);
}

/**
 * @brief Postavljanje trenutne vrednosti kolone
 * @param Trenutna vrednost kolone
 *
 * Kontroler SSD1306 sluzi za konfigurisanje OLED displeja
 * dimenzija 128 x 64 pa je potrebno izvrsiti odredjene modifikacije
 * da bi se ispravno postavila trenunta kolona.
 */
void OLED_SetColumn(uint8_t add)
{
    add += 32;	// 0 - 95 -> 32 - 127
    OLED_Command((SSD1306_SETHIGHCOLUMN | (add >> 4))); 	// SET_HIGH_COLUMN
    OLED_Command((0x0F & add));        						// SET LOW_COLUMN
}

/**
 * @brief Prosledjivanje slike na OLED displej
 * @param Slika koju zelimo da iscrtamo na displeju
 *
 * Slika se iscrtava tako sto se sukcesivno postavljaju vrednosti reda
 * i kolone i slanje bajta kao podatak na OLED koji predstavlja 8 piksela.
 */
void OLED_PutPicture(const uint8_t *pic)
{
    unsigned char i,j;
    for(i = 0; i < OLED_BYTE_HEIGHT; i++) // 5*8=40 redova
    {
        OLED_SetRow(i);
        OLED_SetColumn(0);

        for(j = 0; j < OLED_WIDTH; j++)  // 96 kolona piksela
        {
            OLED_Data(*pic++);
        }
    }
}

/**
 * @brief Brisanje ekrana
 *
 * Displej se brise tako sto se ustvari ispise matrica sa
 * svim vrednostima 0.
 */
void OLED_Clear(void)
{
	uint8_t pic[IMAGE_SIZE] = {0};
	OLED_PutPicture(pic);
}

/**
 * @brief Podesavanje kontrasta OLED displeja
 * @param Vrednost kontrasta
 *
 * Kontrast se podesava pomocu dve sukcesivne komande,
 * koje se salju SSD1306 kontroleru.
 */
void OLED_SetContrast(uint8_t temp)
{
    OLED_Command(SSD1306_SETCONTRAST);
    OLED_Command(temp);                  // contrast step 1 to 256
}
