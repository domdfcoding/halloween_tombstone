// Copy to User_Setups/pico.h after any changes
//
//
//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches
//   should run without the need to make any more changes for a particular
//   hardware setup! Note that some sketches are designed for a particular TFT
//   pixel width/height

// User defined information reported by "Read_User_Setup" test & diagnostics example
#define USER_SETUP_INFO "tft_eyes_st7745"

// Define to disable all #warnings in library (can be put in
// User_Setup_Select.h)
//#define DISABLE_ALL_LIBRARY_WARNINGS

// ##################################################################################
//
// Section 1. Call up the right driver file and any options for it
//
// ##################################################################################

// Only define one driver, the other ones must be commented out
//#define ILI9341_DRIVER       // Generic driver for common displays
//#define ILI9341_2_DRIVER     // Alternative ILI9341 driver, see
//https://github.com/Bodmer/TFT_eSPI/issues/1172
#define ST7735_DRIVER // Define additional parameters below for this display
//#define ILI9163_DRIVER     // Define additional parameters below for this
//display #define S6D02A1_DRIVER #define HX8357D_DRIVER #define ILI9481_DRIVER
//#define ILI9486_DRIVER
//#define ILI9488_DRIVER     // WARNING: Do not connect ILI9488 display SDO to
//MISO if other devices share the SPI bus (TFT SDO does NOT tristate when CS is
//high) #define ST7789_DRIVER      // Full configuration option, define
//additional parameters below for this display #define ST7789_2_DRIVER    //
//Minimal configuration option, define additional parameters below for this
//display #define R61581_DRIVER #define RM68140_DRIVER #define ST7796_DRIVER
//#define SSD1351_DRIVER
//#define SSD1963_480_DRIVER
//#define SSD1963_800_DRIVER
//#define SSD1963_800ALT_DRIVER
//#define ILI9225_DRIVER
//#define GC9A01_DRIVER

// Some displays support SPI reads via the MISO pin, other displays have a
// single bi-directional SDA pin and the library will try to read this via the
// MOSI line. To use the SDA line for reading data from the TFT uncomment the
// following line:

// #define TFT_SDA_READ      // This option is for ESP32 ONLY, tested with
// ST7789 and GC9A01 display only

// For ST7735, ST7789 and ILI9341 ONLY, define the colour order IF the blue and
// red are swapped on your display Try ONE option at a time to find the correct
// colour order for your display

//  #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
//  #define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

// For M5Stack ESP32 module with integrated ILI9341 display ONLY, remove // in
// line below

// #define M5STACK

// For ST7789, ST7735, ILI9163 and GC9A01 ONLY, define the pixel width and
// height in portrait orientation #define TFT_WIDTH  80
#define TFT_WIDTH 128
// #define TFT_WIDTH  172 // ST7789 172 x 320
// #define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
#define TFT_HEIGHT 160
// #define TFT_HEIGHT 128
// #define TFT_HEIGHT 240 // ST7789 240 x 240
// #define TFT_HEIGHT 320 // ST7789 240 x 320
// #define TFT_HEIGHT 240 // GC9A01 240 x 240

// For ST7735 ONLY, define the type of display, originally this was based on the
// colour of the tab on the screen protector film but this is not always true,
// so try out the different options below if the screen does not display
// graphics correctly, e.g. colours wrong, mirror images, or stray pixels at the
// edges. Comment out ALL BUT ONE of these options for a ST7735 display driver,
// save this this User_Setup file, then rebuild and upload the sketch to the
// board again:

// #define ST7735_INITB
// #define ST7735_GREENTAB
// #define ST7735_GREENTAB2
// #define ST7735_GREENTAB3
// #define ST7735_GREENTAB128    // For 128 x 128 display
// #define ST7735_GREENTAB160x80 // For 160 x 80 display (BGR, inverted, 26
// offset)
#define ST7735_REDTAB
// #define ST7735_BLACKTAB
// #define ST7735_REDTAB160x80   // For 160 x 80 display with 24 pixel offset

// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.

// #define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF

// ##################################################################################
//
// Section 2. Define the pins that are used to interface with the display here
//
// ##################################################################################

// If a backlight control signal is available then define the TFT_BL pin in
// Section 2 below. The backlight will be turned ON when tft.begin() is called,
// but the library needs to know if the LEDs are ON with the pin HIGH or LOW. If
// the LEDs are to be driven with a PWM signal or turned OFF/ON then this must
// be handled by the user sketch. e.g. with digitalWrite(TFT_BL, LOW);

// #define TFT_BL   32            // LED back-light control pin
// #define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

// The TFT RESET pin can be connected to the MCU RST pin or 3.3V to free up a
// control pin
//
// The DC (Data Command) pin may be labelled AO or RS (Register Select)
//
// With some displays such as the ILI9341 the TFT CS pin can be connected to GND
// if no more SPI devices (e.g. an SD Card) are connected, in this case comment
// out the #define TFT_CS line below so it is NOT defined. Other displays such
// at the ST7735 require the TFT CS pin to be toggled during setup, so in these
// cases the TFT_CS line must be defined and connected.
//

// ###### EDIT THE PIN NUMBERS IN THE LINES FOLLOWING TO SUIT YOUR SETUP ######

#define TFT_CS 17  // Chip select control pin
#define TFT_DC 20  // Data Command control pin
#define TFT_RST 21 // Reset pin
//#define TFT_RST  -1    // Set TFT_RST to -1 if the display RESET is connected
//to NodeMCU RST or 3.3V

//#define TFT_BL PIN_D1  // LED back-light (only for ST7789 with backlight
//control pin)

#define TFT_MISO 16
#define TFT_MOSI 19
#define TFT_SCLK 18

// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################

// ##################################################################################
//
// Section 4. Other options
//
// ##################################################################################

// For RP2040 processor and SPI displays, uncomment the following line to use
// the PIO interface.
//#define RP2040_PIO_SPI // Leave commented out to use standard RP2040 SPI port
//interface

// For the RP2040 processor define the SPI port channel used (default 0 if
// undefined)
//#define TFT_SPI_PORT 1 // Set to 0 if SPI0 pins are used, or 1 if spi1 pins
//used

// Define the SPI clock frequency, this affects the graphics rendering speed.
// Too fast and the TFT driver will not keep up and display corruption appears.
// With an ILI9341 display 40MHz works OK, 80MHz sometimes fails
// With a ST7735 display more than 27MHz may not work (spurious pixels and
// lines) With an ILI9163 display 27 MHz works OK.

// #define SPI_FREQUENCY   1000000
// #define SPI_FREQUENCY   5000000
// #define SPI_FREQUENCY  10000000
// #define SPI_FREQUENCY  20000000
#define SPI_FREQUENCY 27000000
// #define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  55000000
// #define SPI_FREQUENCY  80000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY 20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY 2500000

// Comment out the following #define if "SPI Transactions" do not need to be
// supported. When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!

// Transaction support is needed to work with SD library but not needed with
// TFT_SdFat Transaction support is required if other SPI devices are connected.

// Transactions are automatically enabled by the library for an ESP32 (to use
// HAL mutex) so changing it here has no effect

// #define SUPPORT_TRANSACTIONS
