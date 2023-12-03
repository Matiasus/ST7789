/** 
 * --------------------------------------------------------------------------------------------+ 
 * @name        ST7789 1.69" LCD Driver
 * --------------------------------------------------------------------------------------------+ 
 *              Copyright (C) 2023 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        03.12.2023
 * @file        st7789.h
 * @version     1.0
 * @tested      AVR Atmega328
 *
 * @depend      font.h
 * --------------------------------------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 * --------------------------------------------------------------------------------------------+
 * @inspir      
 */

#include <avr/pgmspace.h>

#ifndef __ST7789_H__
#define __ST7789_H__

  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include <util/delay.h>
  #include "spi.h"
  #include "font.h"

  // Success / Error
  // -----------------------------------
  #define ST7789_SUCCESS        0
  #define ST7789_ERROR          1

  // PORT/PIN definition
  // -----------------------------------
  #define PORT                  PORTB
  #define DDR                   DDRB 
  #define ST7789_MOSI           3 // SDA
  #define ST7789_MISO           4
  #define ST7789_SCK            5 // SCL

  // Command definition
  // -----------------------------------
  #define DELAY                 0x80
  
  #define NOP                   0x00
  #define SWRESET               0x01
  #define RDDID                 0x04
  #define RDDST                 0x09

  #define SLPIN                 0x10
  #define SLPOUT                0x11
  #define PTLON                 0x12
  #define NORON                 0x13

  #define INVOFF                0x20
  #define INVON                 0x21
  #define DISPOFF               0x28
  #define DISPON                0x29
  #define RAMRD                 0x2E
  #define CASET                 0x2A
  #define RASET                 0x2B
  #define RAMWR                 0x2C

  #define PTLAR                 0x30
  #define MADCTL                0x36
  #define COLMOD                0x3A

  #define FRMCTR1               0xB1
  #define FRMCTR2               0xB2
  #define FRMCTR3               0xB3
  #define INVCTR                0xB4
  #define DISSET5               0xB6

  #define PWCTR1                0xC0
  #define PWCTR2                0xC1
  #define PWCTR3                0xC2
  #define PWCTR4                0xC3
  #define PWCTR5                0xC4
  #define VMCTR1                0xC5

  #define RDID1                 0xDA
  #define RDID2                 0xDB
  #define RDID3                 0xDC
  #define RDID4                 0xDD

  #define GMCTRP1               0xE0
  #define GMCTRN1               0xE1

  #define PWCTR6                0xFC

  // Colors
  // -----------------------------------
  #define BLACK                 0x0000
  #define WHITE                 0xFFFF
  #define RED                   0xF000

  // AREA definition
  // -----------------------------------
  #define MAX_X                 161                     // max columns / MV = 0 in MADCTL
  #define MAX_Y                 130                     // max rows / MV = 0 in MADCTL
  #define SIZE_X                MAX_X - 1               // columns max counter
  #define SIZE_Y                MAX_Y - 1               // rows max counter
  #define CACHE_SIZE_MEM        (MAX_X * MAX_Y)         // whole pixels
  #define CHARS_COLS_LEN        5                       // number of columns for chars
  #define CHARS_ROWS_LEN        8                       // number of rows for chars

  // FUNCTION macros
  // -----------------------------------
  #define CLR_BIT(port, bit)                            ( ((port) &= ~(1 << (bit))) )
  #define SET_BIT(port, bit)                            ( ((port) |= (1 << (bit))) )
  #define IS_BIT_CLR(port, bit)                         ( IS_BIT_SET(port, bit) ? 0 : 1 )
  #define IS_BIT_SET(port, bit)                         ( ((port) & (1 << (bit))) ? 1 : 0 )
  #define WAIT_UNTIL_BIT_IS_SET(port, bit)              { while (IS_BIT_CLR(port, bit)); }

  extern const uint8_t INIT_ST7789[];                   // @const Command list ST7789B 
  
  /** @enum Font sizes */
  enum Size {
    X1 = 0x00,                                          // 1x high & 1x wide size
    X2 = 0x80,                                          // 2x high & 1x wide size
    X3 = 0x81                                           // 2x high & 2x wider size
  };

  /** @struct Signal */
  struct signal {
    volatile uint8_t * ddr;                             // ddr
    volatile uint8_t * port;                            // port
    uint8_t pin;                                        // pin
  };

  /** @struct Lcd */
  struct st7789 {
    struct signal * cs;                                 // Chip Select
    struct signal * bl;                                 // Back Light
    struct signal * dc;                                 // Data / Command
    struct signal * rs;                                 // Reset
  };

  /**
   * @desc    Init st7789 driver
   *
   * @param   struct st7789 *
   *
   * @return  void
   */
  void ST7789_Init (struct st7789 *);

  /**
   * @desc    Hardware Reset
   *
   * @param   struct signal *
   *
   * @return  void
   */
  void ST7789_Reset (struct signal *);

  /**
   * @desc    Command send
   *
   * @param   struct st7789 *
   * @param   uint8_t
   *
   * @return  void
   */
  void ST7789_Send_Command (struct st7789 *, uint8_t);

  /**
   * @desc    8bits data send
   *
   * @param   struct st7789 *
   * @param   uint8_t
   *
   * @return  void
   */
  void ST7789_Send_Data (struct st7789 *, uint8_t);

  /**
   * @desc    Delay
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void ST7789_DelayMs (uint8_t);

#endif
