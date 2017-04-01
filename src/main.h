#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "switches.h"
#include "color.h"

#define LED_ON      PORTB |=  _BV(PINB7)
#define LED_OFF     PORTB &= ~_BV(PINB7) 
#define LED_TOGGLE  PINB  |=  _BV(PINB7)

#define COMPONENTS	2;
#define NAME_LENGTH 12;
#define MAX_ENTITIES 10;


typedef char Name[12];
typedef int(*IntFunc)(void);

typedef struct
{
	int16_t noOptions;
	int16_t optionNo;
	char ** optionNames;
	char * windowName;
	IntFunc * functions;
} window;

typedef struct
{
	float pos[2];
	float vel[2];
	int16_t colour;
} entity;


int redraw();
int loop();

enum tile {
	test,
	player,
	gui_left,
	gui_middle,
	gui_right,
	stone,
	water,
	sand,
	tiles,
};

uint16_t tile_data[tiles][400] = {
	{ 
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x07e4,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4, 0x07e4
	},
	{
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xcd50,0x0000,0xacf0,0xcd50,0xcd50,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xac6b,0xac6b,0xdd2c,0xac6b,0xac6b,0xac6b,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xdd2c,0xac6b,0xdd2c,0xac6b,0xdd2c,0xdd2c,0xdd2c,0xdd2c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xac6b,0xdd2c,0xfe53,0xdd2c,0xac6b,0xac6b,0xdd2c,0xac6b,0xac6b,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0xac6b,0xdd2c,0xfe53,0xfe73,0xfe53,0xdd2c,0xe54c,0xac6b,0xdd2c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xcceb,0xfe53,0x5aeb,0xfe53,0xfe53,0x5aeb,0xfe53,0xcceb,0xc571,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xfe53,0xfe53,0xfe53,0xfe53,0xfe53,0xfe53,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xfe53,0xfe53,0xfe53,0xfe53,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0xffff,0xfaed,0xfaed,0xffff,0x94b2,0xfe53,0xfe53,0x94b2,0xd6ba,0xfaed,0xfaed,0xd6ba,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0xffff,0xfaec,0xfb2d,0xffff,0x94b2,0x94b2,0xd6db,0xfb2d,0xf2ec,0xd6bb,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0xfe53,0xfe76,0xfb2d,0xfb2d,0xec52,0xec52,0xfb2d,0xfaed,0xfef8,0xfe53,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0xfe53,0xfe53,0xfaed,0xfaed,0xfb2d,0xfb2d,0xfaed,0xfaed,0xfe53,0xfe53,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0xfb2d,0xfaed,0xfb2d,0xfb2d,0xfb2d,0xfaed,0xfaed,0xfaed,0xfaed,0xfb2d,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0xfe53,0xfe53,0xdd2c,0xdd2c,0xffff,0xd6ba,0xdd2c,0xc4cb,0xfe53,0xfe53,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xdd2c,0xfb2d,0xfaed,0xdd2c,0xd6ba,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xce14,0xec6c,0xd3eb,0xce14,0xd6ba,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xd6ba,0x0000,0x0000,0xd6ba,0xd6ba,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xfccd,0xdc4d,0x0000,0x0000,0xdc4d,0xfccd,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xfb2d,0xfaed,0x0000,0x0000,0xfaed,0xfb2d,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xfccd,0xfccd,0xfaed,0x0000,0x0000,0xfaed,0xfccd,0xfccd,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
	},
	{
		0x0000,0x0000,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,
		0x0000,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,
		0x0000,0x0000,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c
	},
	{
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c 
	},
	{
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0x0000,0x0000,
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0xe73c,0xe73c,
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0x0000,
		0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0xe73c,0x0000,0xe73c,0xe73c,0x0000,0x0000 
	},
	{
		0x4208,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0xc638,0x4208,0x8410,0x8410,0x8410,
		0x4208,0x8410,0x8410,0x4208,0x4208,0x4208,0x4208,0x8410,0x4208,0x4208,0x4208,0x4208,0x4208,0x4208,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,
		0x4208,0x8410,0x4208,0x8410,0x8410,0xc638,0x4208,0x8410,0x4208,0x8410,0x8410,0xc638,0xc638,0x4208,0x4208,0x4208,0x4208,0x4208,0x4208,0x4208,
		0xc638,0x4208,0x8410,0x8410,0x8410,0xc638,0xc638,0x4208,0x4208,0x8410,0x8410,0x8410,0xc638,0x4208,0xc638,0x4208,0x8410,0xc638,0x4208,0x8410,
		0xc638,0xc638,0x4208,0x8410,0x8410,0x8410,0x4208,0x8410,0x8410,0x4208,0x4208,0x8410,0x4208,0x8410,0x8410,0x4208,0x8410,0x8410,0x4208,0x8410,
		0x8410,0x8410,0x4208,0x8410,0x4208,0x4208,0x4208,0x8410,0x4208,0x8410,0xc638,0x4208,0x4208,0x8410,0x8410,0x4208,0x4208,0x8410,0x4208,0x8410,
		0x8410,0x8410,0x4208,0x4208,0x8410,0xc638,0xc638,0x4208,0x4208,0x8410,0xc638,0xc638,0x8410,0x4208,0x4208,0xc638,0xc638,0x4208,0xc638,0x4208,
		0x4208,0x4208,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,0x8410,0x8410,0xc638,0x4208,0x8410,0xc638,
		0xc638,0x4208,0x8410,0x4208,0x8410,0x8410,0x4208,0x8410,0xc638,0x4208,0x4208,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0x4208,0x8410,0x8410,
		0x8410,0x4208,0x8410,0x4208,0x4208,0x4208,0x4208,0x8410,0xc638,0xc638,0xc638,0x4208,0x4208,0x8410,0x8410,0x8410,0x4208,0xc638,0x4208,0x8410,
		0x8410,0x4208,0x4208,0x4208,0xc638,0xc638,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,0xc638,0x4208,0x4208,0x4208,0x8410,0xc638,0x4208,0x8410,
		0x4208,0x4208,0x4208,0x8410,0x8410,0xc638,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,0x8410,0xc638,0xc638,0x4208,0x8410,0x8410,0xc638,0x4208,
		0x8410,0x8410,0xc638,0x4208,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0x4208,
		0x4208,0x8410,0x4208,0xc638,0x4208,0x4208,0xc638,0x4208,0x8410,0x8410,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,0x4208,0x4208,0x4208,0x8410,
		0xc638,0x4208,0x8410,0x8410,0x4208,0x8410,0x8410,0xc638,0x4208,0x4208,0x8410,0x4208,0x4208,0x8410,0x4208,0x8410,0xc638,0xc638,0x4208,0x8410,
		0x8410,0xc638,0x4208,0x8410,0x4208,0x4208,0x8410,0x8410,0x8410,0x4208,0x8410,0xc638,0xc638,0x4208,0x8410,0x8410,0x8410,0xc638,0xc638,0x4208,
		0x8410,0x8410,0x4208,0x4208,0xc638,0x4208,0x4208,0x4208,0x4208,0x4208,0x8410,0x8410,0xc638,0xc638,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,
		0x4208,0x4208,0xc638,0x4208,0x8410,0xc638,0x4208,0x8410,0x8410,0xc638,0x4208,0x8410,0x8410,0xc638,0x4208,0x8410,0x8410,0x8410,0x8410,0x4208,
		0x4208,0x8410,0xc638,0x4208,0x8410,0x4208,0x4208,0x4208,0x8410,0x4208,0x4208,0x8410,0x8410,0x8410,0x4208,0x4208,0x4208,0x8410,0x4208,0x4208,
		0x4208,0x8410,0x8410,0xc638,0x4208,0x8410,0x8410,0xc638,0x4208,0x8410,0xc638,0x4208,0x4208,0x4208,0x8410,0xc638,0x4208,0x4208,0x8410,0xc638 
	},
	{

		0x8fff,0x469f,0x3e7f,0x3e7f,0x3e7f,0x8fff,0x357f,0x2d3f,0x2d5f,0x3e7f,0x8fff,0x8fff,0x5e9f,0x2d3f,0x2d3f,0x2d3f,0x2d3f,0x2d3f,0x2d5f,0x3e7f,
		0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x8fff,0x357f,0x2d3f,0x357f,0x8fff,0x3e7f,0x2d5f,0x5e9f,0x7fbf,0x2d3f,0x2d3f,0x87df,0xc7bd,0xf77b,0xf77b,
		0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3dbf,0x773f,0x777f,0x7f7f,0xdf9c,0x2d7f,0x2d3f,0x3dff,0x56df,0x777f,0x777f,0x4edf,0x56bf,0x76be,0xf77b,
		0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x2d3f,0x5ebf,0x8f5e,0xb7bd,0xaf1d,0x2d3f,0x2d3f,0x461f,0x5eff,0x66df,0x6f5f,0x3e7f,0x3e7f,0x4e9f,0xcfbd,
		0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x2d3f,0x2d3f,0x8e5d,0xe75b,0x3e7f,0x2d3f,0x2d3f,0x5e9f,0x7fbf,0x2d3f,0x3e7f,0x3e7f,0x3e7f,0x469f,0x8fff,
		0x2d3f,0x3e3f,0x3e7f,0x3e5f,0x2d3f,0x2d3f,0x7fbf,0x5e9f,0x357f,0x8fff,0xf77b,0xf77b,0xbfbd,0x7fbf,0x2d3f,0x3e7f,0x3e7f,0x35bf,0x359f,0x8fff,
		0x8fff,0x359f,0x2d3f,0x2d3f,0x2d3f,0x8fff,0x357f,0x35df,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x35df,0x3d7f,0xf77b,0x2d3f,0x2d3f,0x2d3f,0x2d3f,0x2d3f,
		0x8fff,0x359f,0x2d3f,0x355f,0xcefc,0x3ddf,0x3e1f,0x3e3f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x35df,0x357f,0xa7de,0x2d3f,0x2d3f,0x2d3f,0x2d3f,0x2d3f,
		0x8fff,0x7e5e,0x761e,0x7e1e,0xaf1d,0x2d3f,0x3e3f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x35ff,0x2d9f,0x357f,0x6f5f,0x4e3f,0x2d3f,0x2d3f,0x2d3f,0x2d3f,
		0x8fff,0xe79b,0xf77b,0xe77b,0x3e7f,0x2d3f,0x3e3f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x2d5f,0x2d3f,0x2d5f,0x3e7f,0x8fff,0x355f,0x2d3f,0x2d3f,0x2d3f,
		0x2d3f,0x2d3f,0x2d3f,0x3d5f,0xf77b,0x8fff,0x469f,0x3e7f,0x3e7f,0x3e7f,0x2d3f,0x2d3f,0x35df,0x469f,0x8fff,0xf77b,0x3d5f,0x5ebf,0x97ff,0xf77b,
		0x3e7f,0x3e7f,0x35df,0x2d3f,0x2d3f,0x3e7f,0x87df,0x5e9f,0x2d3f,0x2d3f,0x2d3f,0xe75b,0xbfbd,0x7fbf,0x2d3f,0x2d3f,0xe75b,0x96fd,0x3e7f,0x3e7f,
		0x3e7f,0x3e7f,0x35df,0x2d3f,0x2d3f,0x2d7f,0x4e9f,0x671f,0x6f3f,0x2d3f,0xc6fc,0x6ebe,0x563f,0x3dbf,0x2d3f,0x2d3f,0x55be,0x6ebf,0x777f,0x3e7f,
		0x3e7f,0x3e7f,0x35df,0x2d3f,0x2d3f,0x2d3f,0x3e3f,0x56df,0x6f3f,0x761e,0xaf1c,0x461f,0x2d9f,0x2d3f,0x2d3f,0x359f,0x359f,0x5edf,0x87df,0x3e7f,
		0x3e7f,0x3e7f,0x35df,0x2d3f,0x2d3f,0x2d3f,0x3e3f,0x3e7f,0x4e7f,0xf77b,0x3e7f,0x2d5f,0x2d3f,0x2d3f,0x2d3f,0x3e7f,0x3e7f,0x673f,0x87df,0x3e7f,
		0x3e7f,0x3e7f,0x35df,0x2d3f,0x2d3f,0x2d3f,0x3e3f,0x96fd,0xe75b,0x2d3f,0x8fff,0x357f,0x2d3f,0x2d5f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x469f,0x8fff,
		0x8fff,0x469f,0x35df,0x2d3f,0x2d3f,0x3e7f,0x87df,0x5e9f,0x2d3f,0x2d3f,0xf77b,0x3d5f,0x35df,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e3f,0x2d3f,
		0xdf9c,0x45bf,0x2d5f,0x2d3f,0x361f,0x779f,0x461f,0x3dff,0x361f,0x361f,0xa7de,0x357f,0x35df,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e7f,0x3e3f,0x2d3f,
		0xf77b,0x863d,0x661e,0x4e3f,0x5eff,0x66ff,0x355f,0x35df,0x3e7f,0x3e7f,0x6f7f,0x565f,0x461f,0x35ff,0x3e7f,0x3e7f,0x3e7f,0x3e3f,0x35df,0x2d3f,
		0xf77b,0xf77b,0xc7bd,0x8fff,0x8fff,0x2d3f,0x2d3f,0x35df,0x3e7f,0x3e7f,0x3e7f,0x87df,0x5e9f,0x2d5f,0x3e7f,0x3e7f,0x3e7f,0x35bf,0x2d3f,0x2d3f
	},
	{
		0xfe0f,0xf62f,0xfe0f,0xfe0f,0xf5cf,0xf58e,0xfdef,0xf62f,0xf60f,0xfe2f,0xfe2f,0xfe50,0xfe70,0xfed1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe90,0xfe30,
		0xfef1,0xfef1,0xfef1,0xfed1,0xfe90,0xfe50,0xfe50,0xf5cf,0xfe2f,0xfe50,0xf5ce,0xfe50,0xfe50,0xfe50,0xfe90,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,
		0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe70,0xfe2f,0xf5ef,0xfe2f,0xf62f,0xf5ef,0xf60f,0xfe50,0xfe50,0xfe50,0xfed1,0xfef1,0xfef1,0xfef1,
		0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfed1,0xfe50,0xf58e,0xfe2f,0xfe50,0xfe2f,0xf60f,0xfe2f,0xfe50,0xfe50,0xfe70,0xfed1,0xfef1,
		0xfe50,0xfe70,0xfe50,0xfe70,0xfed1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe50,0xf62f,0xf5ce,0xf5ef,0xfe2f,0xf62f,0xfe2f,0xfe2f,0xfe50,0xfe50,0xfe50,
		0xfe2f,0xfe2f,0xfe2f,0xfe2f,0xfe50,0xfe90,0xfef1,0xfef1,0xfef1,0xfeb1,0xfe70,0xf650,0xfe30,0xfe50,0xf60f,0xfe2f,0xf62f,0xfe2f,0xfe30,0xfe2f,
		0xfe2f,0xf60f,0xf5cf,0xf56e,0xfe2f,0xfe50,0xfe70,0xfed1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfed1,0xfe70,0xf5ae,0xf5ef,0xfe50,0xfe50,
		0xf5ef,0xfe50,0xfe2f,0xf5ef,0xf56e,0xf5cf,0xfe50,0xfe70,0xfed1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe70,0xf5ef,0xf5ef,0xf5ef,
		0xf56e,0xfe50,0xfe50,0xfe50,0xfe2f,0xf56e,0xfe50,0xfe50,0xfe50,0xfe90,0xfed1,0xfed1,0xfed1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe90,0xf5cf,0xf56e,
		0xf56e,0xf56e,0xf5ef,0xfe30,0xfe2f,0xfe2f,0xf5ef,0xf5ae,0xf60f,0xfe50,0xfe50,0xfe50,0xfe50,0xfe50,0xfeb0,0xfef1,0xfef1,0xfef1,0xfe90,0xf5ae,
		0xf5ae,0xf56e,0xf56e,0xed8e,0xf58e,0xf60f,0xf62f,0xf5ef,0xf5ce,0xf5cf,0xf5ae,0xf5ef,0xfe2f,0xfe50,0xfe50,0xfeb0,0xfef1,0xfef1,0xfef1,0xf650,
		0xfed1,0xfe30,0xf5cf,0xf5cf,0xf5ef,0xf5ef,0xf5ae,0xf5cf,0xfe2f,0xfe50,0xfe2f,0xf5ef,0xf56e,0xf58e,0xfe2f,0xfe50,0xfef1,0xfef1,0xfef1,0xfef1,
		0xfef1,0xfef1,0xfed1,0xfed1,0xfed1,0xfed1,0xfed1,0xfe90,0xf60f,0xf5cf,0xf5ef,0xfe2f,0xf5ef,0xf58e,0xf5ae,0xfe2f,0xfe70,0xfed0,0xfef1,0xfef1,
		0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe50,0xf5cf,0xf5ef,0xfe50,0xfe50,0xf56e,0xf5ae,0xfe50,0xfe50,0xfed1,0xfef1,
		0xfed1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe90,0xfe2f,0xf5cf,0xfe30,0xfe2f,0xf62f,0xf58e,0xf62f,0xfe50,0xfe70,
		0xfe50,0xfe70,0xfe70,0xfe50,0xfe50,0xfe50,0xfe90,0xfef1,0xfef1,0xfef1,0xfef1,0xfe90,0xf5cf,0xed8e,0xf5ef,0xfe30,0xf60f,0xf58e,0xf62f,0xfe2f,
		0xfe30,0xfe50,0xfe50,0xfe50,0xfe50,0xfe50,0xfe50,0xfe70,0xfeb0,0xfef1,0xfef1,0xfef1,0xfef1,0xfe70,0xf56e,0xed6e,0xfe2f,0xfe2f,0xf60f,0xf5ef,
		0xf62f,0xfe2f,0xf5ef,0xf60f,0xf5ef,0xf5cf,0xf5ef,0xf62f,0xfe50,0xfed0,0xfef1,0xfef1,0xfef1,0xfef1,0xf670,0xf58e,0xf56e,0xf5cf,0xf5ef,0xfe2f,
		0xf5ef,0xfe2f,0xfe30,0xfe50,0xfe2f,0xf5ef,0xf58e,0xf5ae,0xfe50,0xfe50,0xfeb1,0xfef1,0xfef1,0xfef1,0xfef1,0xfed1,0xf60f,0xf5cf,0xf56e,0xf58e,
		0xf56e,0xed6e,0xed8e,0xf5cf,0xf5ae,0xf5ef,0xf62f,0xfe2f,0xf5ce,0xfe2f,0xfe50,0xfe70,0xfed1,0xfef1,0xfef1,0xfef1,0xfef1,0xfe90,0xfe2f,0xf56e
	}
};
	
uint8_t room_data[2][2][176] = { 
	{
		{
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, 
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, 
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, 
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, 
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, 
			water, water, sand, sand, sand, sand, water, water, water, water, water, sand, sand, sand, sand, sand,
			water, water, sand, water, water, sand, water, water, water, water, water, sand, water, water, water, water,
			water, water, sand, water, water, sand, sand, sand, sand, sand, sand, sand, water, water, water, water,
			water, sand, sand, water, water, water, water, water, water, water, water, sand, water, water, water, water,
			water, sand, water, water, water, water, water, water, water, water, water, sand, water, water, water, water
		},
		{
			water, sand, water, water, water, water, water, water, water, water, water, sand, water, water, water, water,
			water, sand, water, water, water, water, water, water, water, sand, sand, sand, water, water, water, water,
			water, sand, sand, water, water, water, water, water, sand, sand, sand, sand, sand, sand, sand, water,
			water, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water,
			water, sand, sand, sand, stone, stone, stone, stone, sand, sand, sand, sand, sand, sand, sand, sand,
			water, sand, stone, stone, stone, stone, stone, stone, stone, stone, sand, sand, sand, sand, sand, sand,
			water, sand, sand, stone, stone, stone, stone, stone, stone, stone, stone, stone, sand, sand, sand, sand,
			water, sand, sand, sand, sand, sand, sand, sand, stone, stone, stone, stone, sand, sand, sand, sand,
			water, water, water, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand,
			water, water, water, sand, sand, water, sand, sand, water, sand, sand, sand, sand, sand, sand, sand,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water
		}
	},
	{
		{
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water,
			sand, sand, sand, sand, sand, sand, sand, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, sand, water, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, sand, sand, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, sand, water, water, water, water, water, water, water, water,
			water, water, water, water, water, water, water, sand, water, water, water, water, water, water, water, water
		},
		{
			water, water, water, water, water, water, water, sand, water, water, water, water, water, water, water, water,
			water, water, water, water, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water, water,
			water, water, water, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water,
			water, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water,
			sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water,
			sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water,
			sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water,
			sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water,
			sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water, water,
			sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, sand, water, water, water,
			water, water, water, water, water, water, water, water, water, water, water, water, water, water, water, water
		}
	} 
};