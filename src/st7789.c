/**
 * --------------------------------------------------------------------------------------------+
 * @name        ST7789 1.69" LCD Driver
 * --------------------------------------------------------------------------------------------+
 *              Copyright (C) 2023 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        03.12.2023
 * @file        st7789.c
 * @version     1.0
 * @tested      AVR Atmega328
 *
 * @depend      st7789.h
 *
 * --------------------------------------------------------------------------------------------+
 * @descr       Version 1.0
 * --------------------------------------------------------------------------------------------+
 * @inspir
 */
#include "st7789.h"

/** @array Init command */
const uint8_t INIT_ST7789[] PROGMEM = {
  // NUMBER OF COMMANDS
  // ---------------------------------------
  4,                                                    // number of initializers
  // COMMANDS WITH DELAY AND ARGUMENTS
  // ---------------------------------------
  SWRESET, 0, 150,                                      // Software reset, no arguments, delay >120ms
  SLPOUT, 0, 150,                                       // Out of sleep mode, no arguments, delay >120ms
  COLMOD, 1, 0x55, 10,                                  // Set color mode, RGB565
  DISPON, 0, 200                                        // Display turn on
};

uint16_t cacheIndexRow = 0;                             // @var array cache memory char index row
uint16_t cacheIndexCol = 0;                             // @var array cache memory char index column

/**
 * @desc    Clear screen
 *
 * @param   struct st7789 *
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7789_ClearScreen (struct st7789 * lcd, uint16_t color)
{
  ST7789_Set_Window (lcd, 0, SIZE_X, 0, SIZE_Y);        // set whole window 
  ST7789_Send_Color_565 (lcd, color, 76800UL);          // draw individual pixels
}

/**
 * @desc    Draw line by Bresenham algoritm
 * @surce   https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 *
 * @param   struct st7789 *
 * @param   uint8_t x start position / 0 <= cols <= MAX_X-1
 * @param   uint8_t x end position   / 0 <= cols <= MAX_X-1
 * @param   uint8_t y start position / 0 <= rows <= MAX_Y-1
 * @param   uint8_t y end position   / 0 <= rows <= MAX_Y-1
 * @param   uint16_t color
 *
 * @return  void
 */
char ST7789_DrawLine (struct st7789 * lcd, uint16_t x1, uint16_t x2, uint8_t y1, uint8_t y2, uint16_t color)
{
  int16_t D;                                            // determinant
  int16_t delta_x, delta_y;                             // deltas
  int16_t trace_x = 1, trace_y = 1;                     // steps

  delta_x = x2 - x1;                                    // delta x
  delta_y = y2 - y1;                                    // delta y

  if (delta_x < 0) {                                    // check if x2 > x1
    delta_x = -delta_x;                                 // inversion delta x
    trace_x = -trace_x;                                 // inversion step x
  }

  if (delta_y < 0) {                                    // check if y2 > y1
    delta_y = -delta_y;                                 // inversion detla y
    trace_y = -trace_y;                                 // inversion step y
  }

  // Bresenham condition for m < 1 (dy < dx)
  // ---------------------------------------
  if (delta_y < delta_x) {
    D = (delta_y << 1) - delta_x;                       // calculate determinant
    ST7789_DrawPixel (lcd, x1, y1, color);              // draw first pixel
    while (x1 != x2) {                                  // check if x1 equal x2
      x1 += trace_x;                                    // update x1
      if (D >= 0) {                                     // check if determinant is positive
        y1 += trace_y;                                  // update y1
        D -= 2*delta_x;                                 // update determinant
      }
      D += 2*delta_y;                                   // update deteminant
      ST7789_DrawPixel (lcd, x1, y1, color);            // draw next pixel
    }
  // Bresenham condition for m > 1 (dy > dx)
  // ---------------------------------------
  } else {
    D = delta_y - (delta_x << 1);                       // calculate determinant
    ST7789_DrawPixel (lcd, x1, y1, color);              // draw first pixel
    while (y1 != y2) {                                  // check if y2 equal y1
      y1 += trace_y;                                    // update y1
      if (D <= 0) {                                     // check if determinant is positive
        x1 += trace_x;                                  // update y1
        D += 2*delta_y;                                 // update determinant
      }
      D -= 2*delta_x;                                   // update deteminant
      ST7789_DrawPixel (lcd, x1, y1, color);            // draw next pixel
    }
  }

  return ST77XX_SUCCESS;                                 // success return
}

/**
 * @desc    Fast Draw Line Horizontal
 *
 * @param   struct st7789 *
 * @param   uint16_t xs - start position
 * @param   uint16_t xe - end position
 * @param   uint8_t y - position
 * @param   uint16_t color
 *
 * @return void
 */
void ST7789_FastLineHorizontal (struct st7789 * lcd, uint16_t xs, uint16_t xe, uint8_t y, uint16_t color)
{
  if (xs > xe) {                                        // check if start is > as end
    uint8_t temp = xs;                                  // temporary safe
    xe = xs;                                            // start change for end
    xs = temp;                                          // end change for start
  }
  ST7789_Set_Window (lcd, xs, xe, y, y);                // set window
  ST7789_Send_Color_565 (lcd, color, xe - xs);          // draw pixel by 565 mode
}

/**
 * @desc    Fast Draw Line Vertical
 *
 * @param   struct st7789 *
 * @param   uint16_t x - position
 * @param   uint8_t ys - start position
 * @param   uint8_t ye - end position
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7789_FastLineVertical (struct st7789 * lcd, uint16_t x, uint8_t ys, uint8_t ye, uint16_t color)
{
  if (ys > ye) {                                        // check if start is > as end
    uint8_t temp = ys;                                  // temporary safe
    ye = ys;                                            // start change for end
    ys = temp;                                          // end change for start
  }
  ST7789_Set_Window (lcd, x, x, ys, ye);                // set window
  ST7789_Send_Color_565 (lcd, color, ye - ys);          // draw pixel by 565 mode
}

/**
 * @desc    Draw pixel
 *
 * @param   struct st7789 * lcd
 * @param   uint16_t x position / 0 <= cols <= MAX_X-1
 * @param   uint8_t y position / 0 <= rows <= MAX_Y-1
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7789_DrawPixel (struct st7789 * lcd, uint16_t x, uint8_t y, uint16_t color)
{
  ST7789_Set_Window (lcd, x, x, y, y);                  // set window
  ST7789_Send_Color_565 (lcd, color, 1);                // draw pixel by 565 mode
}

/**
 * @desc    RAM Content Show
 *
 * @param   struct st7789 * lcd
 *
 * @return  void
 */
void ST7789_RAM_ContentShow (struct st7789 * lcd)
{
  ST7789_Send_Command (lcd, ST77XX_DISPON);             // display content on
}

/**
 * @desc    RAM Content Hide
 *
 * @param   struct st7789 * lcd
 *
 * @return  void
 */
void ST7735_RAM_ContentHide (struct st7789 * lcd)
{
  ST7789_Send_Command (lcd, ST77XX_DISPOFF);            // display content off
}

/**
 * @desc    Inversion On
 *
 * @param   struct st7789 * lcd
 *
 * @return  void
 */
void ST7735_InvertColorOn (struct st7789 * lcd)
{
  ST7789_Send_Command (lcd, ST77XX_INVON);              // inversion on
}

/**
 * @desc    Inversion Off
 *
 * @param   struct st7789 * lcd
 *
 * @return  void
 */
void ST7735_InvertColorOff (struct st7789 * lcd)
{
  ST7789_Send_Command (lcd, ST77XX_INVOFF);             // inversion off
}

/**
 * @desc    Set Configuration LCD
 *
 * @param   struct st7789 * lcd
 * @param   uint8_t
 *
 * @return  void
 */
void ST7789_SetConfiguration (struct st7789 * lcd, uint8_t configuration)
{
  ST7789_Send_Command (lcd, ST77XX_MADCTL);             // Memory Data Access Control
  ST7789_Send_Data_Byte (lcd, configuration);           // set configuration like rotation, refresh,... 
}

/**
 * @desc    Init st7789 driver
 *
 * @param   struct st7789 *
 * @param   uint8_t
 *
 * @return  void
 */
void ST7789_Init (struct st7789 * lcd, uint8_t configuration)
{
  // SPI Init (cs, settings)
  // ----------------------------------------------------------------
  SPI_Init (SPI_MASTER | SPI_MODE_0 | SPI_MSB_FIRST | SPI_FOSC_DIV_4);

  // DDR
  // --------------------------------------
  SET_BIT (*(lcd->rs->ddr), lcd->rs->pin);              // Reset
  SET_BIT (*(lcd->cs->ddr), lcd->cs->pin);              // Chip Select
  SET_BIT (*(lcd->bl->ddr), lcd->bl->pin);              // BackLight
  SET_BIT (*(lcd->dc->ddr), lcd->dc->pin);              // Data/Command
  // PORT
  // --------------------------------------
  SET_BIT (*(lcd->rs->port), lcd->rs->pin);             // Reset hold H
  SET_BIT (*(lcd->cs->port), lcd->cs->pin);             // Chip Select H
  SET_BIT (*(lcd->bl->port), lcd->bl->pin);             // BackLigt ON

  // POWER UP
  // --------------------------------------
  _delay_ms (10);                                       // power up time delay +/- no limit

  // HW RESET
  // --------------------------------------
  ST7789_Reset_HW (lcd->rs);

  // INIT SEQUENCE
  // --------------------------------------
  ST7789_Init_Sequence (lcd, INIT_ST7789);
  
  // SET CONFIGURATION
  // --------------------------------------
  ST7789_SetConfiguration (lcd, configuration);
}

/**
 * --------------------------------------------------------------------------------------------+
 * PRIVATE FUNCTIONS
 * --------------------------------------------------------------------------------------------+
 */

/**
 * @desc    Set window
 *
 * @param   struct st7789 * lcd
 * @param   uint16_t xs - start position
 * @param   uint16_t xe - end position
 * @param   uint8_t ys - start position
 * @param   uint8_t ye - end position
 *
 * @return  uint8_t
 */
uint8_t ST7789_Set_Window (struct st7789 * lcd, uint16_t xs, uint16_t xe, uint8_t ys, uint8_t ye)
{
  if ((xs > xe) || (xe > SIZE_X) ||
      (ys > ye) || (ys > SIZE_Y)) {
    return ST77XX_ERROR;                                // out of range
  }

  ST7789_Send_Command (lcd, ST77XX_CASET);              // column address set
  ST7789_Send_Data_Word (lcd, 0x0000 | xs);             // send start x position
  ST7789_Send_Data_Word (lcd, 0x0000 | xe);             // send end x position

  ST7789_Send_Command (lcd, ST77XX_RASET);              // row address set
  ST7789_Send_Data_Word (lcd, 0x0000 | ys);             // send start y position
  ST7789_Send_Data_Word (lcd, 0x0000 | ye);             // send end y position

  return ST77XX_SUCCESS;                                // success
}

/**
 * @desc    Write Color Pixels
 *
 * @param   struct st7789 * lcd
 * @param   uint16_t color
 * @param   uint32_t counter
 *
 * @return  void
 */
void ST7789_Send_Color_565 (struct st7789 * lcd, uint16_t color, uint32_t count)
{
  ST7789_Send_Command (lcd, ST77XX_RAMWR);              // access to RAM
  while (count--) {
    ST7789_Send_Data_Word (lcd, color);                 // write color
  }
}

/**
 * --------------------------------------------------------------------------------------------+
 * PRIMITIVE / PRIVATE FUNCTIONS
 * --------------------------------------------------------------------------------------------+
 */

/**
 * @desc    Hardware Reset Sequence
 *
 *              | >10us | >120ms|
 *          ----        --------
 *              \______/
 *
 * @param   struct signal *
 *
 * @return  void
 */
void ST7789_Reset_HW (struct signal * reset)
{
  CLR_BIT (*(reset->port), reset->pin);                 // Reset Impulse
  _delay_us (100);                                      // >10us
  SET_BIT (*(reset->port), reset->pin);                 //
  _delay_ms (120);                                      // >120 ms
}

/**
 * @desc    Init sequence
 *
 * @param   struct st7789 *
 * @param   const uint8_t *
 *
 * @return  void
 */
void ST7789_Init_Sequence (struct st7789 * lcd, const uint8_t * list)
{
  uint8_t arguments;
  uint8_t loops = pgm_read_byte (list++);

  while (loops--) {
    // COMMAND
    // ------------------------------------
    ST7789_Send_Command (lcd, pgm_read_byte (list++));
    // ARGUMENTS
    // ------------------------------------
    arguments = pgm_read_byte (list++);
    while (arguments--) {
      ST7789_Send_Data_Byte (lcd, pgm_read_byte (list++));
    }
    // DELAY
    // ------------------------------------
    ST7789_Delay_ms (pgm_read_byte (list++));
  }
}

/**
 * @desc    Command send
 *
 * @param   struct st7789 *
 * @param   uint8_t
 *
 * @return  void
 */
void ST7789_Send_Command (struct st7789 * lcd, uint8_t data)
{
  CLR_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip enable - active low
  CLR_BIT (*(lcd->dc->port), lcd->dc->pin);             // command (active low)
  SPI_Transfer (data);                                  // transfer
  SET_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip disable - idle high
}

/**
 * @desc    8bits data send
 *
 * @param   struct st7789 *
 * @param   uint8_t
 *
 * @return  void
 */
void ST7789_Send_Data_Byte (struct st7789 * lcd, uint8_t data)
{
  CLR_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip enable - active low
  SET_BIT (*(lcd->dc->port), lcd->dc->pin);             // data (active high)
  SPI_Transfer (data);                                  // transfer
  SET_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip disable - idle high
}

/**
 * @desc    16bits data send
 *
 * @param   struct st7789 *
 * @param   uint16_t
 *
 * @return  void
 */
void ST7789_Send_Data_Word (struct st7789 * lcd, uint16_t data)
{
  CLR_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip enable - active low
  SET_BIT (*(lcd->dc->port), lcd->dc->pin);             // data (active high)
  SPI_Transfer ((uint8_t) (data >> 8));                 // transfer High Byte
  SPI_Transfer ((uint8_t) data);                        // transfer low Byte
  SET_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip disable - idle high
}

/**
 * @desc    Delay
 *
 * @param   uint8_t time in milliseconds / max 256ms
 *
 * @return  void
 */
void ST7789_Delay_ms (uint8_t time)
{
  while (time--) {
    _delay_ms(1);                                         // 1ms delay
  }
}
