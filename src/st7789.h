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
  #define ST77XX_SUCCESS        0
  #define ST77XX_ERROR          1

  // Command definition
  // -----------------------------------
  #define ST77XX_NOP            0x00  // This command is empty command.
  #define ST77XX_SWRESET        0x01  // The display module performs a software reset, registers are written with their SW reset default values

  #define ST77XX_SLPIN          0x10  // In this mode the DC/DC converter is stopped, internal oscillator is stopped, and panel scanning is stopped.
  #define ST77XX_SLPOUT         0x11  // In this mode the DC/DC converter is enable, internal display oscillator is started, and panel scanning is started
  #define ST77XX_PTLON          0x12  // This command turns on Partial mode. The partial mode window is described by the Partial Area command (30h)
  #define ST77XX_NORON          0x13  // This command turns the display to normal mode, Normal display mode on means partial mode off

  #define ST77XX_INVOFF         0x20  // This command is used to recover from display inversion mode
  #define ST77XX_INVON          0x21  // This command is used to recover from display inversion mode.
  #define ST77XX_DISPOFF        0x28  // In this mode, the output from Frame Memory is disabled and blank page inserted
  #define ST77XX_DISPON         0x29  // This command is used to recover from DISPLAY OFF mode.
  #define ST77XX_CASET          0x2A  // Column Address Set, XS, XE [15:0] < 239 (00Efh)): MV=”0”, XS, XE [15:0] < 319 (013Fh)): MV=”1”
  #define ST77XX_RASET          0x2B  // Row Address Set, 0 < YS [15:0] < YE [15:0] < 319 (013fh)): MV=”0”, 0 < YS [15:0] < YE [15:0] < 239 (00EFh)): MV=”1”
  #define ST77XX_RAMWR          0x2C  // Memory Write, This command is used to transfer data from MCU to frame memory.

  #define ST77XX_PTLAR          0x30  // Partial Area
  #define ST77XX_MADCTL         0x36  // Memory Data Access Control
  #define ST77XX_COLMOD         0x3A  // Interface Pixel Format
  #define ST77XX_TEOFF          0x34  // Tearing Effect Line OFF
  #define ST77XX_TEON           0x35  // Tearing Effect Line On, This command is used to turn ON the Tearing Effect output signal from the TE signal line
  #define ST77XX_VSCSAD         0x37  // Vertical Scroll Start Address of RAM
  #define ST77XX_IDMOFF         0x38  // Idle Mode Off
  #define ST77XX_IDMON          0x39  // Idle Mode On

  #define ST77XX_WRCTRLD        0x53  // Write CTRL Display

  // Colors
  // -----------------------------------
  #define BLACK                 0x0000
  #define WHITE                 0xFFFF
  #define RED                   0xFC00
  #define BLUE                  0x00CF

  // AREA definition
  // -----------------------------------
  #define MAX_X                 320                     // max columns / MV = 0 in MADCTL
  #define MAX_Y                 240                     // max rows / MV = 0 in MADCTL
  #define WINDOW_PIXELS         MAX_X * MAX_Y
  #define SIZE_X                MAX_X - 1               // columns max counter
  #define SIZE_Y                MAX_Y - 1               // rows max counter
  #define CHARS_COLS_LEN        5                       // number of columns for chars
  #define CHARS_ROWS_LEN        8                       // number of rows for chars

  // FUNCTION macros
  // -----------------------------------
  #define CLR_BIT(port, bit)   (((port) &= ~(1<<(bit))))
  #define SET_BIT(port, bit)   (((port) |=  (1<<(bit))))

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

  // Memory Data Access Control
  // D7  D6  D5  D4  D3  D2  D1  D0
  // MY  MX  MV  ML RGB  MH   -   -
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
  #define ST77XX_RGB            0x00                    // RGB Mode
  #define ST77XX_BGR            0x08                    // BGR Mode 
  #define ST77XX_NORMAL         0x00                    // Normal   
  #define ST77XX_Y_MIRROR       0x80                    // X-Mirror
  #define ST77XX_X_MIRROR       0x40                    // Y-Mirror
  #define ST77XX_XY_MIRROR      0xC0                    // X-Mirror, Y-Mirror
  #define ST77XX_XY_CHANGE      0x20                    // X-Y Exchange
  #define ST77XX_XY_CHANGE_Y    0xA0                    // X-Y Exchange Y-Mirror
  #define ST77XX_XY_CHANGE_X    0x60                    // X-Y Exchange X-Mirror
  #define ST77XX_XY_CHANGE_XY   0xD0                    // X-Y Exchange X-Mirror Y-Mirror

  /**
   * @desc    Clear screen
   *
   * @param   struct st7789 *
   * @param   uint16_t
   *
   * @return  void
   */
  void ST7789_ClearScreen (struct st7789 *, uint16_t);

  /**
   * @desc    Draw line by Bresenham algoritm
   * @surce   https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
   *
   * @param   struct st7789 *
   * @param   uint8_t x start position / 0 <= cols <= MAX_X-1
   * @param   uint16_t x end position   / 0 <= cols <= MAX_X-1
   * @param   uint8_t y start position / 0 <= rows <= MAX_Y-1
   * @param   uint8_t y end position   / 0 <= rows <= MAX_Y-1
   * @param   uint16_t color
   *
   * @return  void
   */
  char ST7789_DrawLine (struct st7789 *, uint16_t, uint16_t, uint8_t, uint8_t, uint16_t);

  /**
   * @desc    Fast Draw Line Horizontal
   *
   * @param   struct st7789 *
   * @param   uint8_t xs - start position
   * @param   uint8_t xe - end position
   * @param   uint8_t y - position
   * @param   uint16_t color
   *
   * @return void
   */
  void ST7789_FastLineHorizontal (struct st7789 *, uint16_t, uint16_t, uint8_t, uint16_t);

  /**
   * @desc    Fast Draw Line Vertical
   *
   * @param   struct st7789 *
   * @param   uint8_t x - position
   * @param   uint8_t ys - start position
   * @param   uint8_t ye - end position
   * @param   uint16_t color
   *
   * @return  void
   */
  void ST7789_FastLineVertical (struct st7789 *, uint16_t, uint8_t, uint8_t, uint16_t);

  /**
   * @desc    Draw Pixel
   *
   * @param   struct st7789 * lcd
   * @param   uint16_t x position / 0 <= cols <= MAX_X-1
   * @param   uint8_t y position / 0 <= rows <= MAX_Y-1
   * @param   uint16_t color
   *
   * @return  void
   */
  void ST7789_DrawPixel (struct st7789 *, uint16_t, uint8_t, uint16_t);

  /**
   * @desc    Init LCD
   *
   * @param   struct st7789 *
   * @param   uint8_t
   *
   * @return  void
   */
  void ST7789_Init (struct st7789 *, uint8_t);

  /**
   * @desc    RAM Content Show
   *
   * @param   struct st7789 * lcd
   *
   * @return  void
   */
  void ST7789_RAM_ContentShow (struct st7789 *);

  /**
   * @desc    RAM Content Hide
   *
   * @param   struct st7789 * lcd
   *
   * @return  void
   */
  void ST7735_RAM_ContentHide (struct st7789 *);

  /**
   * @desc    Inversion On
   *
   * @param   struct st7789 * lcd
   *
   * @return  void
   */
  void ST7735_InvertColorOn (struct st7789 *);

  /**
   * @desc    Inversion Off
   *
   * @param   struct st7789 * lcd
   *
   * @return  void
   */
  void ST7735_InvertColorOff (struct st7789 *);

  /**
   * @desc    Set Configuration LCD
   *
   * @param   struct st7789 * lcd
   * @param   uint8_t
   *
   * @return  void
   */
  void ST7789_SetConfiguration (struct st7789 *, uint8_t);

  /**
   * --------------------------------------------------------------------------------------------+
   * PRIVATE FUNCTIONS
   * --------------------------------------------------------------------------------------------+
   */

  /**
   * @desc    Set Window
   *
   * @param   struct st7789 * lcd
   * @param   uint16_t xs - start position
   * @param   uint16_t xe - end position
   * @param   uint8_t ys - start position
   * @param   uint8_t ye - end position
   *
   * @return  uint8_t
   */
  uint8_t ST7789_Set_Window (struct st7789 *, uint16_t, uint16_t, uint8_t, uint8_t);

  /**
   * @desc    Write Color Pixels
   *
   * @param   struct st7789 * lcd
   * @param   uint16_t color
   * @param   uint32_t counter
   *
   * @return  void
   */
  void ST7789_Send_Color_565 (struct st7789 *, uint16_t, uint32_t);

  /**
   * --------------------------------------------------------------------------------------------+
   * PRIMITIVE / PRIVATE FUNCTIONS
   * --------------------------------------------------------------------------------------------+
   */

  /**
   * @desc    Hardware Reset
   *
   * @param   struct signal *
   *
   * @return  void
   */
  void ST7789_Reset_HW (struct signal *);

  /**
   * @desc    Init sequence
   *
   * @param   struct st7789 *
   * @param   const uint8_t *
   *
   * @return  void
   */
  void ST7789_Init_Sequence (struct st7789 *, const uint8_t *);

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
  void ST7789_Send_Data_Byte (struct st7789 *, uint8_t);

  /**
   * @desc    16bits data send
   *
   * @param   struct st7789 *
   * @param   uint16_t
   *
   * @return  void
   */
  void ST7789_Send_Data_Word (struct st7789 *, uint16_t);

  /**
   * @desc    Delay
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void ST7789_Delay_ms (uint8_t);

#endif
