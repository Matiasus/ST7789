/**
 * ---------------------------------------------------------------+
 * @brief       SPI (Master Mode)
 * ---------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        21.09.2022
 * @file        spi.h
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend      avr/io.h
 * ---------------------------------------------------------------+
 * @interface   SPI master mode
 * @pins        SCLK, MOSI, MISO, CS (SS)
 *
 * @sources
 */

#ifndef __SPI_H__
#define __SPI_H__

  // includes
  #include <avr/io.h>

  // atmega328p
  #if defined(__AVR_ATmega328P__)

    #define SPI_DDR           DDRB
    #define SPI_PORT          PORTB
    #define SPI_SCK           PINB5     // or SCLK
    #define SPI_MISO          PINB4     // or DO
    #define SPI_MOSI          PINB3     // or SDA, SDI, DI
    #define SPI_SS            PINB2     // or CS

    // SPI registers
    #define SPI_SPSR          SPSR
    #define SPI_SPCR          SPCR
    #define SPI_SPDR          SPDR

  #endif

  // macros
  #define CS_ENABLE()         SPI_PORT &= ~(1 << SPI_SS)
  #define CS_DISABLE()        SPI_PORT |= (1 << SPI_SS)

  // SPI init definitions
  #define SPI_MASTER          0x10
  #define SPI_SLAVE           0x00
  #define SPI_MODE_0          0x00
  #define SPI_MODE_1          0x04
  #define SPI_MODE_2          0x08
  #define SPI_MODE_3          0x0C
  #define SPI_MSB_FIRST       0x00
  #define SPI_LSB_FIRST       0x20
  #define SPI_FOSC_DIV_4      0x00
  #define SPI_FOSC_DIV_16     0x01
  #define SPI_FOSC_DIV_64     0x02
  #define SPI_FOSC_DIV_128    0x03

  /**
   * @desc    SPI Init
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  void
   */
  void SPI_Init (uint8_t, uint8_t);

  /**
   * @desc    SPI Write Byte
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SPI_Transfer (uint8_t);
  
#endif
