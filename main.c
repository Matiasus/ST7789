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

/**
 * @desc    Main function
 *
 * @param   void
 *
 * @return  int
 */
int main (void)
{
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

  for (uint8_t i = 0; i<240; i=i+5) {
    ST7789_DrawLine (&lcd, 0, 320, 0, i, RED);
  }
  for (uint8_t i = 0; i<240; i=i+5) {
    ST7789_DrawLine (&lcd, 0, 320, i, 0, BLUE);
  }
  for (uint8_t i = 0; i<30; i++) {
    ST7789_FastLineHorizontal (&lcd, 0, 320, i, BLACK);
  }

  ST7789_SetPosition (90, 5);
  ST7789_DrawString (&lcd, "ST7789 DRIVER", WHITE, X3);

  // EXIT
  // ----------------------------------------------------------
  return 0;
}