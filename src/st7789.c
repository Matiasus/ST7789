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
 * @depend      font.h
 * --------------------------------------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 * --------------------------------------------------------------------------------------------+
 * @inspir      
 */

#include "st7789.h"

/** @array Init command */
const uint8_t INIT_ST7789[] PROGMEM = {
  // number of initializers
  5,
  // ---------------------------------------
  // Software reset - no arguments,  delay
  0, 150, SWRESET,
  // Out of sleep mode, no arguments, delay
  0, 200, SLPOUT,  
  // Set color mode, 1 argument delay
  1,  10, COLMOD, 0x05,
  // D7  D6  D5  D4  D3  D2  D1  D0
  // MY  MX  MV  ML RGB  MH   -   -
  // ------------------------------
  // ------------------------------
  // MV  MX  MY -> {MV (row / column exchange) MX (column address order), MY (row address order)}
  // ------------------------------
  //  0   0   0 -> begin left-up corner, end right-down corner 
  //               left-right (normal view) 
  //  0   0   1 -> begin left-down corner, end right-up corner 
  //               left-right (Y-mirror)
  //  0   1   0 -> begin right-up corner, end left-down corner 
  //               right-left (X-mirror)
  //  0   1   1 -> begin right-down corner, end left-up corner
  //               right-left (X-mirror, Y-mirror)
  //  1   0   0 -> begin left-up corner, end right-down corner
  //               up-down (X-Y exchange)  
  //  1   0   1 -> begin left-down corner, end right-up corner
  //               down-up (X-Y exchange, Y-mirror)
  //  1   1   0 -> begin right-up corner, end left-down corner 
  //               up-down (X-Y exchange, X-mirror)  
  //  1   1   1 -> begin right-down corner, end left-up corner
  //               down-up (X-Y exchange, X-mirror, Y-mirror)
  // ------------------------------
  //  ML: vertical refresh order 
  //      0 -> refresh top to bottom 
  //      1 -> refresh bottom to top
  // ------------------------------
  // RGB: filter panel
  //      0 -> RGB 
  //      1 -> BGR        
  // ------------------------------ 
  //  MH: horizontal refresh order 
  //      0 -> refresh left to right 
  //      1 -> refresh right to left
  // 0xA0 = 1010 0000
  1,   0, MADCTL, 0xA0,
  // Main screen turn on
  0, 200, DISPON 
  // ---------------------------------------
};

uint16_t cacheMemIndexRow = 0;                           // @var array cache memory char index row
uint16_t cacheMemIndexCol = 0;                           // @var array cache memory char index column

/**
 * @desc    Hardware Reset
 *
 * @param   struct signal *
 *
 * @return  void
 */
void ST7789_Reset (struct signal * reset)
{
  SET_BIT (*(reset->port), reset->pin);                 // posible write: SET_BIT (*(*reset).port, reset->pin)
  SET_BIT (*(reset->ddr), reset->pin);                  // DDR as output
  _delay_ms(200);                                       // delay 200 ms
  CLR_BIT (*(reset->port), reset->pin);                 // Reset Low 
  _delay_ms(200);                                       // delay 200 ms
  SET_BIT (*(reset->port), reset->pin);                 // Reset High
}

/**
 * @desc    Init st7789 driver
 *
 * @param   struct st7789 *
 *
 * @return  void
 */
void ST7789_Init (struct st7789 * lcd)
{
  // SPI Init (cs, settings)
  // ----------------------------------------------------------------
  SPI_Init (SPI_SS, SPI_MASTER | SPI_MODE_0 | SPI_MSB_FIRST | SPI_FOSC_DIV_128);
  
  // DDR
  // --------------------------------------
  SET_BIT (*(lcd->cs->ddr), lcd->cs->pin);
  SET_BIT (*(lcd->bl->ddr), lcd->bl->pin);
  SET_BIT (*(lcd->dc->ddr), lcd->dc->pin);
  // PORT
  // --------------------------------------
  SET_BIT (*(lcd->cs->port), lcd->cs->pin);             // Chip Select H
  SET_BIT (*(lcd->bl->port), lcd->bl->pin);             // BackLigt ON
  
  ST7789_Reset (lcd->rs);                               // hardware reset
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
void ST7789_Send_Data (struct st7789 * lcd, uint8_t data)
{
  CLR_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip enable - active low
  SET_BIT (*(lcd->dc->port), lcd->dc->pin);             // data (active high)
  SPI_Transfer (data);                                  // transfer
  SET_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip disable - idle high
}
