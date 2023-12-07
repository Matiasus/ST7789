# C Library for ST7789 1.69 TFT LCD display

## ST7789 Description
Detailed information are described in [Datasheet ST7789V2](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/unit/lcd/ST7789V2_SPEC_V1.0.pdf). Initialization is the same as at ST7735 driver.

## Library
C library is aimed for driving 1.69 TFT LCD display 320x240 with ST7789 driver through SPI's Atmega328P / Atmega8.

### Versions
- **1.0** - the first publication, init sequence and basic functions like draw line, clear screen.

## Hardware connection MICROCONTROLLER -> LCD
Library was tested and proved with **_ST7789 1.69″ TFT Display_** and Arduino UNO R3. The hardware configuration was chosen for simplicity. Arduino have not installed bootloader it was raw Atmega328P microcontroller. Communication with PC was done through SPI port interface of Arduino UNO R3.
| PORT LCD | PORT ATMEGA328P | ARDUINO UNO R3 | Description |
| :--- | :--- |  :--- | :--- |
| UCC | +5V | +5V | Supply Voltage +5V |
| GND | GND | GND | Ground |
| DIN | PB3 | D11 | SPI Serial data in / MOSI |
| CLK | PB5 | D13 | SPI Serial clock signal |
| CS | PB2 | D10 | SPI Chip Select |
| DC | PD7 | D7 | SPI data / command; 1=Data, 0=Command |
| RST | PB0 | D8 | Chip Reset Signal |
| BL | PB1 | D9 | Backlight control |

### Picture of hardware connection
<img src="img/hw_connection.png" alt="Hardware connection" width="600">

### Usage
Prior defined for MCU Atmega8 / Atmega328P.

### Tested
Library was tested and proved on a **_ST7789 1.69″ TFT Display_** with **_Atmega328P_**.
  
## Demonstration

## Links
- [Datasheet ST7789](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/unit/lcd/ST7789V2_SPEC_V1.0.pdf)
- [Hardware connection](https://www.elecom.sk/sk/1-69inch-lcd-display-module-240-280-resolution-spi-interface-ips-262k-colors.html?cnid=95cb7bd85f6306d486d3e6f45598f&)
