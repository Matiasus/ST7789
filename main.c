/** 
 * --------------------------------------------------------------------------------------------+ 
 * @name        Main example st7789 LCD driver
 * --------------------------------------------------------------------------------------------+ 
 *              Copyright (C) 2023 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        03.12.2023
 * @file        main.c
 * @version     1.0
 * @tested      AVR Atmega328
 *
 * @depend      st7789.h
 * --------------------------------------------------------------------------------------------+
 * @descr       Version 1.0
 *
 */
#include "src/st7789.h"

/** @var Screen definition */
extern struct S_SCREEN Screen;

/**
 * @desc    Main function
 *
 * @param   void
 *
 * @return  int
 */
int main (void)
{
  uint16_t i;
  // LCD - init struct
  // ----------------------------------------------------------
  struct signal cs = { .ddr = &DDRB, .port = &PORTB, .pin = 2 };          // Chip Select
  struct signal bl = { .ddr = &DDRB, .port = &PORTB, .pin = 1 };          // Back Light
  struct signal rs = { .ddr = &DDRB, .port = &PORTB, .pin = 0 };          // Reset
  struct signal dc = { .ddr = &DDRD, .port = &PORTD, .pin = 7 };          // Data / Command

  struct st7789 lcd = { .cs = &cs, .bl = &bl, .dc = &dc, .rs = &rs };     // LCD struct

  // LCD INIT
  // ----------------------------------------------------------
  ST7789_Init (&lcd, ST77XX_ROTATE_270 | ST77XX_RGB);

  // DRAWING
  // ----------------------------------------------------------
  ST7789_ClearScreen (&lcd, WHITE);
  for (i=0; i<Screen.height; i=i+5) {
    ST7789_DrawLine (&lcd, 0, Screen.width, 0, i, RED);
  }
  for (i=0; i<Screen.height; i=i+5) {
    ST7789_DrawLine (&lcd, 0, Screen.width, i, 0, BLUE);
  }
  for (i=0; i<30; i++) {
    ST7789_FastLineHorizontal (&lcd, 0, Screen.width, i, BLACK);
  }
  ST7789_SetPosition (75, 5);
  ST7789_DrawString (&lcd, "ST7789V2 DRIVER", WHITE, X3);

  // EXIT
  // ----------------------------------------------------------
  return 0;
}