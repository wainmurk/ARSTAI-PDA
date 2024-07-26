
#define USER_SETUP_INFO "User_Setup"

#define ILI9341_DRIVER       // Define additional parameters below for this display


#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   17  // Chip select control pin
#define TFT_DC    25  // Data Command control pin
#define TFT_RST   16  // Reset pin (could connect to RST pin)

#define LOAD_GLCD

#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000