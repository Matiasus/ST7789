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
  #define ST7789_MOSI           3     // SDA
  #define ST7789_MISO           4
  #define ST7789_SCK            5     // SCL

  // Command definition
  // -----------------------------------
  #define NOP                   0x00  // This command is empty command.
  #define SWRESET               0x01  // The display module performs a software reset, registers are written with their SW reset default values

  #define SLPIN                 0x10  // In this mode the DC/DC converter is stopped, internal oscillator is stopped, and panel scanning is stopped.
  #define SLPOUT                0x11  // In this mode the DC/DC converter is enable, internal display oscillator is started, and panel scanning is started
  #define PTLON                 0x12  // This command turns on Partial mode. The partial mode window is described by the Partial Area command (30h)
  #define NORON                 0x13  // This command turns the display to normal mode, Normal display mode on means partial mode off

  #define INVOFF                0x20  // This command is used to recover from display inversion mode
  #define INVON                 0x21  // This command is used to recover from display inversion mode.
  #define DISPOFF               0x28  // In this mode, the output from Frame Memory is disabled and blank page inserted
  #define DISPON                0x29  // This command is used to recover from DISPLAY OFF mode.
  #define CASET                 0x2A  // Column Address Set, XS, XE [15:0] < 239 (00Efh)): MV=”0”, XS, XE [15:0] < 319 (013Fh)): MV=”1”
  #define RASET                 0x2B  // Row Address Set, 0 < YS [15:0] < YE [15:0] < 319 (013fh)): MV=”0”, 0 < YS [15:0] < YE [15:0] < 239 (00EFh)): MV=”1”
  #define RAMWR                 0x2C  // Memory Write, This command is used to transfer data from MCU to frame memory.

  #define PTLAR                 0x30  // Partial Area
  #define MADCTL                0x36  // Memory Data Access Control
  #define COLMOD                0x3A  // Interface Pixel Format
  #define TEOFF                 0x34  // Tearing Effect Line OFF
  #define TEON                  0x35  // Tearing Effect Line On, This command is used to turn ON the Tearing Effect output signal from the TE signal line
  #define VSCSAD                0x37  // Vertical Scroll Start Address of RAM
  #define IDMOFF                0x38  // Idle Mode Off
  #define IDMON                 0x39  // Idle Mode On

  #define WRCTRLD               0x53  // Write CTRL Display

  // Colors
  // -----------------------------------
  #define BLACK                 0x0000
  #define WHITE                 0xFFFF
  #define RED                   0xF000

  // AREA definition
  // -----------------------------------
  #define MAX_X                 280                     // max columns / MV = 0 in MADCTL
  #define MAX_Y                 240                     // max rows / MV = 0 in MADCTL
  #define SIZE_X                MAX_X - 1               // columns max counter
  #define SIZE_Y                MAX_Y - 1               // rows max counter
  #define CACHE_SIZE_MEM        (MAX_X * MAX_Y)         // whole pixels
  #define CHARS_COLS_LEN        5                       // number of columns for chars
  #define CHARS_ROWS_LEN        8                       // number of rows for chars

  // FUNCTION macros
  // -----------------------------------
  #define CLR_BIT(port, bit)                            ( ((port) &= ~(1 << (bit))) )
  #define SET_BIT(port, bit)                            ( ((port) |= (1 << (bit))) )
  #define IS_BIT_SET(port, bit)                         ( ((port) & (1 << (bit))) ? 1 : 0 )
  #define IS_BIT_CLR(port, bit)                         ( IS_BIT_SET(port, bit) ? 0 : 1 )
  #define WAIT_IF_BIT_SET(port, bit)                    { while (IS_BIT_CLR(port, bit)); }

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
   * @desc    Data send - byte
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
