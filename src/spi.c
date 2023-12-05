/**
 * ---------------------------------------------------------------+
 * @brief       SPI (Master Mode)
 * ---------------------------------------------------------------+
 *              Copyright (C) 2022 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        21.09.2023
 * @file        spi.c
 * @version     1.0
 * @test        AVR Atmega328p
 *
 * @depend
 * ---------------------------------------------------------------+
 * @interface   SPI master mode
 * @pins        SCLK, MOSI, MISO, CS (SS)
 *
 * @sources
 */

// INCLUDE libraries
#include "spi.h"

/**
 * @desc    SPI Init
 *
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  void
 */
void SPI_Init (uint8_t settings)
{
  // SPI PORT Init
  // ----------------------------------------------------------------
  SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK);
  SPI_DDR &= ~(1 << SPI_MISO);
  SPI_PORT |= (1 << SPI_MISO);

  // SPI init
  // ----------------------------------------------------------------
  SPI_SPCR = settings | (1 << SPE);
  SPI_SPSR |= (1 << SPI2X);
}

/**
 * @desc    SPI Send & Receive Byte
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SPI_Transfer (uint8_t data)
{
  SPI_SPDR = data;
  while(!(SPI_SPSR & (1<<SPIF))) 
  ;
  return SPI_SPDR;
}
