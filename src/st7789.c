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
  0, 150, SWRESET,                                      // Software reset, no arguments, delay >120ms
  0, 150, SLPOUT,                                       // Out of sleep mode, no arguments, delay >120ms
  1,  10, COLMOD, 0x55,                                 // Set color mode, 1 argument delay
  1,   0, MADCTL, 0xA0,                                 // Memory Data Access Control
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
                                                        // 0xA0 = 1010 0000
  0, 200, DISPON                                        // Main screen turn on
};

uint16_t cacheIndexRow = 0;                             // @var array cache memory char index row
uint16_t cacheIndexCol = 0;                             // @var array cache memory char index column

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
  SET_BIT (*(lcd->rs->ddr), lcd->rs->pin);              // DDR as output
  SET_BIT (*(lcd->cs->ddr), lcd->cs->pin);
  SET_BIT (*(lcd->bl->ddr), lcd->bl->pin);
  SET_BIT (*(lcd->dc->ddr), lcd->dc->pin);
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
  ST7789_Reset (lcd->rs);

  // INIT SEQUENCE
  // --------------------------------------
  ST7789_Init_Sequence (lcd, INIT_ST7789);
}

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
  uint8_t time;
  uint8_t command;
  uint8_t arguments;
  uint8_t loops = pgm_read_byte (list++);

  while (loops--) {
    arguments = pgm_read_byte (list++);                 // 1st arg - number of command arguments
    time = pgm_read_byte (list++);                      // 2nd arg - delay time
    command = pgm_read_byte (list++);                   // 3th arg - command

    ST7789_Send_Command (lcd, command);                 // send command
    while (arguments--) {
      ST7789_Send_Data (lcd, pgm_read_byte (list++));   // send argument
    }
    ST7789_DelayMs (time);                              // delay
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
void ST7789_Send_Data (struct st7789 * lcd, uint8_t data)
{
  CLR_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip enable - active low
  SET_BIT (*(lcd->dc->port), lcd->dc->pin);             // data (active high)
  SPI_Transfer (data);                                  // transfer
  SET_BIT (*(lcd->cs->port), lcd->cs->pin);             // chip disable - idle high
}
