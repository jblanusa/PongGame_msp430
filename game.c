/**
 * @file game.c
 * @brief Implementacija funkcija koji se koriste za realizaciju Pong igre
 * @author Jovan Blanusa, 47/2012 (jovan.blanusa@gmail.com)
 * @date 2016
 */
#include <math.h>

#include "game.h"
#include "lut.h"
#include "oled.h"

/**
 * Velicina loptice
 */
#define BALL_SIZE 3

/**
 * Maska koja se koristi za iscrtavanje loptice
 */
#define BALL_MASK 7

/**
 * Velicina igraca
 */
#define PLANK_SIZE 8

/**
 * Red u kome se ispisuje rezultat prvog igraca
 */
#define SCORE1_ROW 0

/**
 * Red u kome se ispisuje rezultat drugog igraca
 */
#define SCORE2_ROW 0

/**
 * Kolona u kome se ispisuje rezultat prvog igraca
 */
#define SCORE1_COL 39

/**
 * Kolona u kome se ispisuje rezultat drugog igraca
 */
#define SCORE2_COL 52

/**
 * Razmak izmedju dve cifre u rezultatu
 */
#define NUM_OFFSET  6

/**
 * Broj kolona koje zauzima jedna cifra
 */
#define NUM_OF_COLS 5

/**
 * Maksimalan pomeraj loptice po Y osi
 */
#define MAX_Y_STEP 3

/**
 * Pomeraj loptice po X osi
 */
#define DEF_X_STEP 4 // 3

/**
 * Vrednost koja definise posle koliko se generise nova loptica posle postizanja poena
 */
#define IDLE_WAIT 10

/**
 * Rezultati prvog i drugog igraca
 */
static unsigned int score1 = 0, score2 = 0;

/**
 * X i Y koordinata loptice
 */
static int xpos, ypos;

/**
 * Vrednosti koje definisu polozaj igraca
 */
static int bpos1 = 15, bpos2 = 15;

/**
 * Broj koraka koje loptica pravi posle svakog frejma
 */
static int xstep = DEF_X_STEP, ystep = 1;

/**
 * Trenutni frejm koji se iscrtava
 */
uint8_t playground[OLED_WIDTH * OLED_BYTE_HEIGHT];

/**
 * Koliko se jos ceka do generisanja nove loptice
 */
static int idle_cnt = 0;

/**
 * Indikator koji odredjuje da li je potrebno da se generise nova loptica
 */
static uint8_t new_ball = 1;

/**
 * Iterator kroz nizove
 */
static int i;

/**
 * @brief Generator slucajnih brojeva
 *
 * Generator pseudoslucajnih brojeva koji radi na principu
 * Linearnog kongruentnog generatora:
 *
 * seed = (a * seed + c) % m;
 *
 * gde koeficijenti a, c i m moraju da zadovoljavaju odredjene uslove.
 * }
 */
unsigned int random()
{
    static unsigned int nSeed = 5323;
    nSeed = (8253729 * nSeed + 2396403);

    return nSeed  % 32767;
}


/**
 * @brief Funkcija koja osvezava ekran na prekid tajmera
 * @param Polozaj prvog igraca
 * @param Polozaj drugog igraca
 *
 * Osnovni tok funkcije izgleda:
 * 	- Izbrisi prethodni polozaj loptice
 * 	- Izbrisi prethodni polozaj igraca
 * 	- Odredi novi polozaj loptice i igraca
 * 	- Iscrtati igrace
 * 	- Ispisati rezultat
 * 	- Iscrtati lopticu
 *
 * U slucaju pocetka igre, ili nove loptice, potrebno je
 * postaviti lopticu na nasumicnu visinu sredine terena,
 * i nasumicno odrediti na koju ce stranu da ide, kao i
 * koliki ce da bude korak po Y osi. Korak po X osi je
 * konstantan.
 */
void RefreshScreen(unsigned int adc1, unsigned int adc2, uint8_t reset)
{
	// Sluzi za pravljenje pauze posle kraja igrice
	if(idle_cnt>0){
		idle_cnt--;
	}
	else
	{
		if(new_ball)
		{
			// Pozadina se ponovo ucitava
			int i,j;
			for(i = 0; i < OLED_BYTE_HEIGHT; i++)
				for(j = 0; j < OLED_WIDTH; j++)
					playground[i * OLED_WIDTH + j] = background[i * OLED_WIDTH + j];

			xpos = OLED_WIDTH / 2;

			//Nasumicna y koordinata lopte, izbegavamo preklapanje sa zidovima
			unsigned int rnd = random();
			ypos = (rnd | 0x3F) % (8 * OLED_BYTE_HEIGHT - (BALL_SIZE>>1)*2) + (BALL_SIZE>>1);
			xstep = DEF_X_STEP * (rnd & 0x40 ? 1 : -1);
			ystep = (rnd >> 7) % MAX_Y_STEP + 1;

/*			if(reset)
			{
				score1 = score2 = 0;
			}*/
			new_ball = 0;
		}
		else
		{
			// Ako nije reset, brisemo prethodne pozicije lopti
			RemoveBall();
			RemoveBoard();
			RedrawMiddle();
		}

		// Odredjujemo sledecu poziciju lopte, i rezultat
		bpos1 = adc1; bpos2 = adc2;
		NextState();

		DrawBoard();
		WriteResult();
		DrawBall();

		//Slanje slike na OLED
		OLED_PutPicture(playground);
	}
}

/**
 * @brief Iscrtavanje igraca
 *
 * Iscrtava oba igraca na osnovu vrednosti koje su prosledjene funkciji
 * RefreshScreen i koje predstavljaju skalirane vrednosti AD konvertora.
 * Igraci se iscrtavaju tako sto se odredjeni biti u matrici koja predstavlja
 * trenutni frejm postavljaju na 1.
 */
void DrawBoard()
{
	int pos1 = bpos1, pos2 = bpos2;
	int row = pos1 / 8, offs = pos1 % 8;
	playground[row * OLED_WIDTH + 1] |= 0xFF << offs;
	playground[row * OLED_WIDTH + 2] |= 0xFF << offs;
	playground[(row + 1)* OLED_WIDTH + 1] |= 0xFF >> (8 - offs);
	playground[(row + 1)* OLED_WIDTH + 2] |= 0xFF >> (8 - offs);

	row = pos2 / 8, offs = pos2 % 8;
	playground[row * OLED_WIDTH + (OLED_WIDTH - 2)] |= 0xFF << offs;
	playground[row * OLED_WIDTH + (OLED_WIDTH - 3)] |= 0xFF << offs;
	playground[(row + 1)* OLED_WIDTH + (OLED_WIDTH - 2)] |= 0xFF >> (8 - offs);
	playground[(row + 1)* OLED_WIDTH + (OLED_WIDTH - 3)] |= 0xFF >> (8 - offs);
}

/**
 * @brief Ponovno iscrtavanje sredista terena
 *
 * Cilj ove funkcije je da ponovo iscrta isprekidanu liniju na sredini
 * terena za slucaj da je funkcija RemoveBall izbrisala neki njen deo
 * dok je brisala lopticu.
 */
void RedrawMiddle()
{
	for(i = 0; i < OLED_BYTE_HEIGHT; i++)
	{
		playground[i * OLED_WIDTH + (OLED_WIDTH>>1)] |= background[i * OLED_WIDTH + (OLED_WIDTH>>1)];
		playground[i * OLED_WIDTH + (OLED_WIDTH>>1) - 1] |= background[i * OLED_WIDTH + (OLED_WIDTH>>1) - 1];
	}
}

/**
 * @brief Brisanje igraca
 *
 * Brisanje oba igraca postavljanjem vrednosti odredjenih bita u
 * matrici trenutnog frejma na 0.
 */
void RemoveBoard()
{
	int pos1 = bpos1, pos2 = bpos2;
	int row = pos1 / 8, offs = pos1 % 8;
	playground[row * OLED_WIDTH + 1] &= ~( 0xFF << offs );
	playground[row * OLED_WIDTH + 2] &= ~( 0xFF << offs) ;
	playground[(row + 1)* OLED_WIDTH + 1] &= ~( 0xFF >> (8 - offs) );
	playground[(row + 1)* OLED_WIDTH + 2] &= ~( 0xFF >> (8 - offs) );

	row = pos2 / 8, offs = pos2 % 8;
	playground[row * OLED_WIDTH + (OLED_WIDTH - 2)] &= ~( 0xFF << offs );
	playground[row * OLED_WIDTH + (OLED_WIDTH - 3)] &= ~( 0xFF << offs );
	playground[(row + 1)* OLED_WIDTH + (OLED_WIDTH - 2)] &= ~( 0xFF >> (8 - offs) );
	playground[(row + 1)* OLED_WIDTH + (OLED_WIDTH - 3)] &= ~( 0xFF >> (8 - offs) );
}

/**
 * @brief Iscrtavanje loptice
 *
 * Loptica se iscrtava na osnovu trenutne pozicije loptice dobijene
 * iz funkcije NextState. Iscrtava se tako sto se odredjeni biti u
 * matrici trenutnog frejma postavljaju na 1.
 */
void DrawBall()
{
	int row = ypos / 8, offs = ypos % 8;
	for(i = xpos - (BALL_SIZE>>1); i <= xpos + (BALL_SIZE>>1); i++)
	{
	    int shift = offs-(BALL_SIZE>>1);
		playground[row * OLED_WIDTH + i] |= shift > 0 ? BALL_MASK << shift : BALL_MASK >> (-shift);
		if(offs < (BALL_SIZE>>1))
		{
			playground[(row - 1) * OLED_WIDTH + i] |= BALL_MASK << offs + 8 - (BALL_SIZE>>1);
		}
		else if(7 - offs < (BALL_SIZE>>1))
		{
			playground[(row + 1) * OLED_WIDTH + i] |= BALL_MASK >> BALL_SIZE - (offs + (BALL_SIZE>>1) - 7);
		}
	}
}

/**
 * @brief Brisanje loptice
 *
 * Loptica sebrise na osnovu prethodne pozicije. Brise se tako sto se odredjeni biti u
 * matrici trenutnog frejma postavljaju na 0.
 */
void RemoveBall()
{
	int row = ypos / 8, offs = ypos % 8;
	for(i = xpos - (BALL_SIZE>>1); i <= xpos + (BALL_SIZE>>1); i++)
	{
	    int shift = offs-(BALL_SIZE>>1);
		playground[row * OLED_WIDTH + i] &= ~( (shift > 0) ? (BALL_MASK << shift) : (BALL_MASK >> (-shift)) );
		if(offs < (BALL_SIZE>>1))
		{
			playground[(row - 1) * OLED_WIDTH + i] &= ~( BALL_MASK << offs + 8 - (BALL_SIZE>>1) );
		}
		else if(7 - offs < (BALL_SIZE>>1))
		{
			playground[(row + 1) * OLED_WIDTH + i] &= ~( BALL_MASK >> BALL_SIZE - (offs + (BALL_SIZE>>1) - 7) );
		}
	}
}

/**
 * @brief Ispisivanje rezultata
 *
 * Rezultat se ispisuje koristeci Look-up tabelu datu u fajlu lut.h.
 * Look-up tabela sadrzi podatke potrebne za ispisivanje cifara na
 * OLED displej.
 */
void WriteResult()
{
	if(score1 < 10)
	{
		for(i = 0; i < NUM_OF_COLS; i++)
		{
			playground[SCORE1_ROW * OLED_WIDTH + SCORE1_COL + i] = lut[score1 * NUM_OF_COLS + i];
		}
	}
	else
	{
		for(i = 0; i < NUM_OF_COLS; i++)
		{
			playground[SCORE1_ROW * OLED_WIDTH + SCORE1_COL + i - NUM_OFFSET] = lut[((score1 % 100)/10) * NUM_OF_COLS + i];
			playground[SCORE1_ROW * OLED_WIDTH + SCORE1_COL + i] = lut[score1 % 10 * NUM_OF_COLS + i];
		}
	}

	if(score2 < 10){
		for(i = 0; i < NUM_OF_COLS; i++)
		{
			playground[SCORE2_ROW * OLED_WIDTH + SCORE2_COL + i] = lut[score2 * NUM_OF_COLS + i];
		}
	}
	else
	{
		for(i = 0; i < NUM_OF_COLS; i++)
		{
			playground[SCORE2_ROW * OLED_WIDTH + SCORE2_COL + i] = lut[((score2 % 100)/10) * NUM_OF_COLS + i];
			playground[SCORE2_ROW * OLED_WIDTH + SCORE2_COL + i + NUM_OFFSET] = lut[score2 % 10 * NUM_OF_COLS + i];
		}
	}
}

/**
 * @brief Odredjivanje sledeceg polozaja loptice
 *
 * Funkcija odredjuje sledeci polozaj loptice na osnovu njenog
 * trenutnog polozaja i vrednosti koraka po X i Y osi. Ako je
 * loptica blizu gornjeg ili donjeg zida potrebno je azurirati
 * novu poziciju tako da izgleda kao da se loptica odbila. U
 * tom slucaju se menja i vrednost koraka po Y osi.
 *
 * Takodje je potrebno odrediti da li je igrac sprecio lopticu
 * da prodje, i da li je osvojio poen. Takodje, vrse se azuriranja
 * koraka po X i Y osi ako je loptica udarila u igraca. Ako je igrac
 * izgubio poen, signalizira se da je potrebno generisati novu lopticu.
 */
void NextState()
{
// Azuriranje X koordinate
	// Ako ce loptica udariti u desni zid
	if(xpos + xstep >= (OLED_WIDTH - (BALL_SIZE>>1)) - 2)
	{
		// Odredjivanje rastojanja loptice od centra daske
		//int dist =  bpos1 - (8*OLED_BYTE_HEIGHT - (PLANK_SIZE>>1));
		int dist =  bpos2 - ypos + (PLANK_SIZE>>1);
		//score2 = dist;
		dist = dist > 0 ? dist : dist - 1;

		//Nije pogodjena daska
		if(abs(dist) > (PLANK_SIZE>>1) + (BALL_SIZE>>1) + 1)
		{
			idle_cnt = IDLE_WAIT;
			new_ball = 1;
			score1++;
		}
		else
		{
			xstep = -xstep;
			if(abs(dist) > MAX_Y_STEP)
				ystep = dist > 0 ? -MAX_Y_STEP : MAX_Y_STEP;
			else
				ystep = -dist;
		}
	}
	// Ako ce udariti u levi zid
	else if(xpos + xstep < (BALL_SIZE>>1) + 2 )
	{
		// Odredjivanje rastojanja loptice od centra daske
		//int dist =  bpos1 - (8*OLED_BYTE_HEIGHT - (PLANK_SIZE>>1));
		int dist =  bpos1 - ypos + (PLANK_SIZE>>1);
		dist = dist > 0 ? dist : dist - 1;

		//Nije pogodjena daska
		if(abs(dist) > (PLANK_SIZE>>1) + (BALL_SIZE>>1) + 1)
		{
			idle_cnt = IDLE_WAIT;
			new_ball = 1;
			score2++;
		}
		else
		{
			xstep = -xstep;
			if(abs(dist) > MAX_Y_STEP)
				ystep = dist > 0 ? -MAX_Y_STEP : MAX_Y_STEP;
			else
				ystep = -dist;
		}
	}
	else
	{
		xpos += xstep;
	}

// Azuriranje Y koordinate
	// Ako ce loptica udariti u donji zid
	if(ypos + ystep >= (8*OLED_BYTE_HEIGHT - (BALL_SIZE>>1)))
	{
		ypos = 2*(8*OLED_BYTE_HEIGHT - 1) - (ypos + ystep);
		ystep = -ystep;
	}
	// Ako ce udariti u gornji zid
	else if(ypos + ystep < (BALL_SIZE>>1) )
	{
		ypos  = -(ypos + ystep);
		ystep = -ystep;
	}
	else
	{
		ypos += ystep;
	}

}
