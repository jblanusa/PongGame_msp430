/**
 * @file oled.h
 * @brief Deklaracija funkcija za interfejs sa OLED W Click plocicom
 * @author Jovan Blanusa, 47/2012 (jovan.blanusa@gmail.com)
 * @date 2016
 */
#ifndef OLED_H_
#define OLED_H_

#include <stdint.h>

/**
 * Sirina displeja u pikselima
 */
#define OLED_WIDTH             96

/**
 * Visina displeja podeljena sa 8
 */
#define OLED_BYTE_HEIGHT       5

/**
 * Broj bajtova potrebnih za predstavljanje slike
 */
#define IMAGE_SIZE 			   480

/**
 * Makroi komandi za SSD1306 kontroler koji kontrolise OLED
 */
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_SETSEGMENTREMAP     0xA1
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_COMSCANINC          0xC0
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_EXTERNALVCC         0x1
#define SSD1306_SWITCHCAPVCC        0x2
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A

/**
 * @brief Funkcija koja prosledjuje bajt kontroleru SSD1306 preko SPI magistrale
 * @param Podatak ili komanda koja se salje
 */
int SPI_B0_Write(uint8_t);

/**
 * @brief Slanje komande kontroleru za OLED
 * @param Komanda koja se salje
 */
void OLED_Command(uint8_t);

/**
 * @brief Slanje podatka kontroleru za OLED
 * @param Podatak koja se salje
 */
void OLED_Data(uint8_t);

/**
 * @brief Inicijalizacija OLED displeja
 */
void OLED_Initialize(void);

/**
 * @brief Postavljanje trenutne vrednosti reda
 * @param Trenutna vrednost reda
 */
void OLED_SetRow(uint8_t);

/**
 * @brief Postavljanje trenutne vrednosti kolone
 * @param Trenutna vrednost kolone
 */
void OLED_SetColumn(uint8_t);

/**
 * @brief Prosledjivanje slike na OLED displej
 * @param Slika koju zelimo da iscrtamo na displeju
 */
void OLED_PutPicture(const uint8_t *);

/**
 * @brief Podesavanje kontrasta OLED displeja
 * @param Vrednost kontrasta
 */
void OLED_SetContrast(uint8_t);

/**
 * @brief Brisanje ekrana
 */
void OLED_Clear(void);

#endif /* OLED_H_ */
