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
  5,                                                    // number of initializers
  // COMMANDS WITH DELAY AND ARGUMENTS
  // ---------------------------------------
  ST77XX_SWRESET, 0, 150,                               // Software reset, no arguments, delay >120ms
  ST77XX_SLPOUT, 0, 150,                                // Out of sleep mode, no arguments, delay >120ms
  ST77XX_COLMOD, 1, 0x55, 10,                           // Set color mode, RGB565
  ST77XX_INVON, 0, 150,                                 // Set invert color mode
  ST77XX_DISPON, 0, 200                                 // Display turn on
};

uint16_t cacheIndexRow = 0;                             // @var array cache memory char index row
uint16_t cacheIndexCol = 0;                             // @var array cache memory char index column

struct S_SCREEN Screen = {
  .x = MAX_X, 
  .y = MAX_Y
};

/**
 * +------------------------------------------------------------------------------------+
 * |== STATIC FUNCTIONS ================================================================|
 * +------------------------------------------------------------------------------------+
 */
/* Chip Select Active */
static inline void ST7789_CS_Active (struct st7789 * lcd) { CLR_BIT (*(lcd->cs->port), lcd->cs->pin); }
/* Chip Select Idle */
static inline void ST7789_CS_Idle (struct st7789 * lcd) { SET_BIT (*(lcd->cs->port), lcd->cs->pin); }

/* Command Active */
static inline void ST7789_DC_Command (struct st7789 * lcd) { CLR_BIT (*(lcd->dc->port), lcd->dc->pin); }
/* Data Active */
static inline void ST7789_DC_Data (struct st7789 * lcd) { SET_BIT (*(lcd->dc->port), lcd->dc->pin); }

/**
 * +------------------------------------------------------------------------------------+
 * |== PUBLIC FUNCTIONS ================================================================|
 * +------------------------------------------------------------------------------------+
 */

/**
 * @desc    Set text position x, y
 *
 * @param   uint8_t x - position
 * @param   uint8_t y - position
 *
 * @return  char
 */
char ST7789_SetPosition (uint8_t x, uint8_t y)
{
  if ((x > MAX_X) && (y > MAX_Y)) {
    return ST77XX_ERROR;                                // check if coordinates is out of range
  } 
  else if ((x > MAX_X) && (y <= MAX_Y)) {
    cacheIndexRow = y;                                  // set position y
    cacheIndexCol = 2;                                  // set position x
  } else {
    cacheIndexRow = y;                                  // set position y 
    cacheIndexCol = x;                                  // set position x
  }

  return ST77XX_SUCCESS;
}

/**
 * @desc    Draw String
 *
 * @param   struct st7789 *
 * @param   char * string 
 * @param   uint16_t color
 * @param   enum S_SIZE (X1, X2, X3)
 *
 * @return  uint8_t
 */
uint8_t ST7789_DrawString (struct st7789 * lcd, char * str, uint16_t color, enum S_SIZE size)
{
  uint8_t i = 0;
  //uint8_t delta_y = CHARS_ROWS_LEN + (size >> 4);

  while (str[i] != '\0') {
/*
    if (((cacheIndexCol + CHARS_COLS_LEN + (size & 0x0F)) > MAX_X)) {
      if ((cacheIndexRow + delta_y) > (MAX_Y - delta_y)) {
        return ST77XX_ERROR;
      } else {
        cacheIndexRow += delta_y;
        cacheIndexCol = 2;
      } 
    }
    */
    ST7789_DrawChar (lcd, str[i++], color, size);
  }

  return ST77XX_SUCCESS;
}

/**
 * @desc    Draw character
 *
 * @param   struct st7789 *
 * @param   char character
 * @param   uint16_t color
 *
 * @return  void
 */
char ST7789_DrawChar (struct st7789 * lcd, char character, uint16_t color, enum S_SIZE size)
{
  uint8_t letter, idxCol, idxRow;                       // variables
  
  if ((character < 0x20) &&
      (character > 0x7f)) { 
    return ST77XX_ERROR;                                // out of range
  }
  
  idxCol = CHARS_COLS_LEN;                              // last column of character array - 5 columns 
  idxRow = CHARS_ROWS_LEN;                              // last row of character array - 8 rows / bits

  ST7789_CS_Active (lcd);                               // chip enable - active low

  // --------------------------------------
  // SIZE X1 - normal font 1x high, 1x wide
  // --------------------------------------
  if (size == X1) { 
    while (idxCol--) {
      letter = pgm_read_byte (&FONTS[character - 32][idxCol]);
      while (idxRow--) {
        if (letter & (1 << idxRow)) {
          ST7789_Set_Window (lcd, cacheIndexCol+idxCol, cacheIndexCol+idxCol, cacheIndexRow+idxRow, cacheIndexRow+idxRow);
          ST7789_Send_Color_565 (lcd, color, 1);
        }
      }
      idxRow = CHARS_ROWS_LEN;
    }
    cacheIndexCol += CHARS_COLS_LEN + 1;
  // --------------------------------------
  // SIZE X2 - font 2x higher, normal wide
  // --------------------------------------
  } else if (size == X2) {
    while (idxCol--) {
      letter = pgm_read_byte (&FONTS[character - 32][idxCol]);
      while (idxRow--) {
        if (letter & (1 << idxRow)) {
          ST7789_Set_Window (lcd, cacheIndexCol+idxCol, cacheIndexCol+idxCol, cacheIndexRow+(idxRow<<1), cacheIndexRow+(idxRow<<1)+1);
          ST7789_Send_Color_565 (lcd, color, 2);
        }
      }
      idxRow = CHARS_ROWS_LEN;
    }
    cacheIndexCol += CHARS_COLS_LEN + 1;

  // --------------------------------------
  // SIZE X3 - font 2x higher, 2x wider
  // --------------------------------------
  } else if (size == X3) {
    while (idxCol--) {
      letter = pgm_read_byte (&FONTS[character - 32][idxCol]);
      while (idxRow--) {
        if (letter & (1 << idxRow)) {
          ST7789_Set_Window (lcd, cacheIndexCol+(idxCol<<1), cacheIndexCol+(idxCol<<1)+1, cacheIndexRow+(idxRow<<1), cacheIndexRow+(idxRow<<1)+1);
          ST7789_Send_Color_565 (lcd, color, 4);
        }
      }
      idxRow = CHARS_ROWS_LEN;
    }
    cacheIndexCol += CHARS_COLS_LEN + CHARS_COLS_LEN + 1;
  }

  ST7789_CS_Idle (lcd);                                 // chip disable - idle high

  return ST77XX_SUCCESS;
}

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
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_Set_Window (lcd, 0, Screen.x, 0, Screen.y);    // set window
  ST7789_Send_Color_565 (lcd, color, WINDOW_PIXELS);    // draw pixel by 565 mode
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high
}

/**
 * @desc    Draw line by Bresenham algoritm
 * @surce   https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 *
 * @param   struct st7789 *
 * @param   uint16_t x start position / 0 <= cols <= MAX_X-1
 * @param   uint16_t x end position   / 0 <= cols <= MAX_X-1
 * @param   uint16_t y start position / 0 <= rows <= MAX_Y-1
 * @param   uint16_t y end position   / 0 <= rows <= MAX_Y-1
 * @param   uint16_t color
 *
 * @return  void
 */
char ST7789_DrawLine (struct st7789 * lcd, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, uint16_t color)
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
  ST7789_CS_Active (lcd);                               // chip enable - active low
  if (delta_y < delta_x) {
    D = (delta_y << 1) - delta_x;                       // calculate determinant
    ST7789_Set_Window (lcd, x1, x1, y1, y1);            // set window
    ST7789_Send_Color_565 (lcd, color, 1);              // draw pixel by 565 mode
    while (x1 != x2) {                                  // check if x1 equal x2
      x1 += trace_x;                                    // update x1
      if (D >= 0) {                                     // check if determinant is positive
        y1 += trace_y;                                  // update y1
        D -= 2*delta_x;                                 // update determinant
      }
      D += 2*delta_y;                                   // update deteminant
      ST7789_Set_Window (lcd, x1, x1, y1, y1);          // set window
      ST7789_Send_Color_565 (lcd, color, 1);            // draw pixel by 565 mode
    }
  // Bresenham condition for m > 1 (dy > dx)
  // ---------------------------------------
  } else {
    D = delta_y - (delta_x << 1);                       // calculate determinant
    ST7789_Set_Window (lcd, x1, x1, y1, y1);            // set window
    ST7789_Send_Color_565 (lcd, color, 1);              // draw pixel by 565 mode
    while (y1 != y2) {                                  // check if y2 equal y1
      y1 += trace_y;                                    // update y1
      if (D <= 0) {                                     // check if determinant is positive
        x1 += trace_x;                                  // update y1
        D += 2*delta_y;                                 // update determinant
      }
      D -= 2*delta_x;                                   // update deteminant
      ST7789_Set_Window (lcd, x1, x1, y1, y1);          // set window
      ST7789_Send_Color_565 (lcd, color, 1);            // draw pixel by 565 mode
    }
  }
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high

  return ST77XX_SUCCESS;                                // success return
}

/**
 * @desc    Fast Draw Line Horizontal
 *
 * @param   struct st7789 *
 * @param   uint16_t xs - start position
 * @param   uint16_t xe - end position
 * @param   uint16_t y - position
 * @param   uint16_t color
 *
 * @return void
 */
void ST7789_FastLineHorizontal (struct st7789 * lcd, uint16_t xs, uint16_t xe, uint16_t y, uint16_t color)
{
  if (xs > xe) {                                        // check if start is > as end
    uint8_t temp = xs;                                  // temporary safe
    xe = xs;                                            // start change for end
    xs = temp;                                          // end change for start
  }
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_Set_Window (lcd, xs, xe, y, y);                // set window
  ST7789_Send_Color_565 (lcd, color, xe - xs);          // draw pixel by 565 mode
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high
}

/**
 * @desc    Fast Draw Line Vertical
 *
 * @param   struct st7789 *
 * @param   uint16_t x - position
 * @param   uint16_t ys - start position
 * @param   uint16_t ye - end position
 * @param   uint16_t color
 *
 * @return  void
 */
void ST7789_FastLineVertical (struct st7789 * lcd, uint16_t x, uint16_t ys, uint16_t ye, uint16_t color)
{
  if (ys > ye) {                                        // check if start is > as end
    uint8_t temp = ys;                                  // temporary safe
    ye = ys;                                            // start change for end
    ys = temp;                                          // end change for start
  }
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_Set_Window (lcd, x, x, ys, ye);                // set window
  ST7789_Send_Color_565 (lcd, color, ye - ys);          // draw pixel by 565 mode
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high
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
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_Set_Window (lcd, x, x, y, y);                  // set window
  ST7789_Send_Color_565 (lcd, color, 1);                // draw pixel by 565 mode
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high
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
void ST7789_RAM_ContentHide (struct st7789 * lcd)
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
void ST7789_InvertColorOn (struct st7789 * lcd)
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
void ST7789_InvertColorOff (struct st7789 * lcd)
{
  ST7789_Send_Command (lcd, ST77XX_INVOFF);             // inversion off
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
 * @desc    Set Configuration LCD
 *
 * @param   struct st7789 * lcd
 * @param   uint8_t
 *
 * @return  void
 */
void ST7789_SetConfiguration (struct st7789 * lcd, uint8_t configuration)
{
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_DC_Command (lcd);                              // command (active low)
  SPI_Transfer (ST77XX_MADCTL);                         // command
  ST7789_DC_Data (lcd);                                 // data (active high)
  SPI_Transfer (configuration);                         // set configuration like rotation, refresh,...
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high

  if (((0xF0 & configuration) == ST77XX_ROTATE_90) ||
      ((0xF0 & configuration) == ST77XX_ROTATE_270)) {
    Screen.x = MAX_Y;
    Screen.y = MAX_X;
  }
}

/**
 * @desc    Set window
 *
 * @param   struct st7789 * lcd
 * @param   uint16_t xs - start position
 * @param   uint16_t xe - end position
 * @param   uint16_t ys - start position
 * @param   uint16_t ye - end position
 *
 * @return  uint8_t
 */
uint8_t ST7789_Set_Window (struct st7789 * lcd, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye)
{
  if ((xs > xe) || (xe > Screen.x) ||
      (ys > ye) || (ye > Screen.y)) {
    return ST77XX_ERROR;                                // out of range
  }

  // CASET
  // --------------------------------------
  ST7789_DC_Command (lcd);                              // command (active low)
  SPI_Transfer (ST77XX_CASET);                          // command
  
  ST7789_DC_Data (lcd);                                 // data (active high)
  SPI_Transfer ((uint8_t) (xs >> 8));                   // transfer High Byte
  SPI_Transfer ((uint8_t) xs);                          // transfer low Byte
  SPI_Transfer ((uint8_t) (xe >> 8));                   // transfer High Byte
  SPI_Transfer ((uint8_t) xe);                          // transfer low Byte
  
  // RASET
  // --------------------------------------
  ST7789_DC_Command (lcd);                              // command (active low)
  SPI_Transfer (ST77XX_RASET);                          // command
  
  ST7789_DC_Data (lcd);                                 // data (active high)
  SPI_Transfer ((uint8_t) (ys >> 8));                   // transfer High Byte
  SPI_Transfer ((uint8_t) ys);                          // transfer low Byte
  SPI_Transfer ((uint8_t) (ye >> 8));                   // transfer High Byte
  SPI_Transfer ((uint8_t) ye);                          // transfer low Byte

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
  // RAMWR
  // --------------------------------------
  ST7789_DC_Command (lcd);                              // command (active low)
  SPI_Transfer (ST77XX_RAMWR);                          // command
  
  ST7789_DC_Data (lcd);                                 // data (active high)
  while (count--) {
    SPI_Transfer ((uint8_t) (color >> 8));              // transfer High Byte
    SPI_Transfer ((uint8_t) color);                     // transfer low Byte
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
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_DC_Command (lcd);                              // command (active low)
  SPI_Transfer (data);                                  // transfer
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high
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
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_DC_Data (lcd);                                 // data (active high)
  SPI_Transfer (data);                                  // transfer
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high
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
  ST7789_CS_Active (lcd);                               // chip enable - active low
  ST7789_DC_Data (lcd);                                 // data (active high)
  SPI_Transfer ((uint8_t) (data >> 8));                 // transfer High Byte
  SPI_Transfer ((uint8_t) data);                        // transfer low Byte
  ST7789_CS_Idle (lcd);                                 // chip disable - idle high
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
