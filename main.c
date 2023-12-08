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
  struct signal dc = { .ddr = &DDRB, .port = &PORTB, .pin = 0 };          // Data / Command
  struct signal rs = { .ddr = &DDRD, .port = &PORTD, .pin = 7 };          // Reset

  struct st7789 lcd = { .cs = &cs, .bl = &bl, .dc = &dc, .rs = &rs };     // LCD struct

  // LCD
  // ----------------------------------------------------------
  ST7789_Init (&lcd, ST77XX_NORMAL | ST77XX_RGB);

  // EXIT
  // ----------------------------------------------------------
  return 0;
}
