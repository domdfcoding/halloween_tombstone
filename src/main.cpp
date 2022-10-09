
// An adaption of the "UncannyEyes" sketch (see eye_functions.h tab)
// for the TFT_eSPI library, combined with the NeoPixel "stringtest" example.

// Configuration settings for the eye, eye style, chip selects and x/y offsets
// can be defined in the "config.h" file.

// Use as high a SPI clock rate as is supported by the display.
// 27MHz minimum, some diplays can be operated at higher clock rates in the
// range 40-80MHz.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip

// Which pins are connected to the two NeoPixel strips?
#define LED_PIN_A 28
#define LED_PIN_B 22

#define LED_COUNT 8 // How many NeoPixels are attached?
#define LED_BRIGHTNESS 150

#include <neopixels.h>

// Declare our NeoPixel strip objects:
Adafruit_NeoPixel strip_a(LED_COUNT, LED_PIN_A, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip_b(LED_COUNT, LED_PIN_B, NEO_GRB + NEO_KHZ800);
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

TFT_eSPI tft; // A single instance is used for 1 or 2 displays

// A pixel buffer is used during eye rendering
#define BUFFER_SIZE 1024 // 128 to 1024 seems optimum

#ifdef USE_DMA
#define BUFFERS 2 // 2 toggle buffers with DMA
#else
#define BUFFERS 1 // 1 buffer for no DMA
#endif

#include "config.h" // Eye configuration goes in this file

#include "eye_functions.h"

// 1st core setup for eyes
void setup() {
	Serial.begin(115200);
	// while (!Serial);
	Serial.println("Starting");

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
	// Enable backlight pin, initially off
	Serial.println("Backlight turned off");
	pinMode(DISPLAY_BACKLIGHT, OUTPUT);
	digitalWrite(DISPLAY_BACKLIGHT, LOW);
#endif

	// Initialise the eye(s), this will set all chip selects low for the
	// tft.init()
	initEyes();

	// Initialise TFT
	Serial.println("Initialising displays");
	tft.init();

#ifdef USE_DMA
	tft.initDMA();
#endif

	// Raise chip select(s) so that displays can be individually configured
	digitalWrite(TFT_CS, HIGH);
	digitalWrite(TFT_CS, LOW);
	tft.setRotation(TFT_ROTATION);
	tft.fillScreen(TFT_BLACK);
	digitalWrite(TFT_CS, HIGH);

#if defined(DISPLAY_BACKLIGHT) && (DISPLAY_BACKLIGHT >= 0)
	Serial.println("Backlight now on!");
	analogWrite(DISPLAY_BACKLIGHT, BACKLIGHT_MAX);
#endif

	startTime = millis(); // For frame-rate calculation
}

// 2nd core setup for NeoPixels
void setup1() {
	strip_a.begin();
	strip_b.begin();

	// Turn OFF all pixels
	strip_a.show();
	strip_b.show();

	strip_a.setBrightness(LED_BRIGHTNESS);
	strip_b.setBrightness(LED_BRIGHTNESS);
}

// Main loop for eyes
void loop() { updateEye(); }

// 2nd core loop for NeoPixels
void loop1() {
	// Seeming duplicates are for correct timing

	// Fill along the length of the strip in various colors...
	colorWipe(strip_a.Color(255, 0, 0), 50); // Red
	colorWipe(strip_b.Color(255, 0, 0), 50); // Red
	colorWipe(strip_a.Color(0, 255, 0), 50); // Green
	colorWipe(strip_b.Color(0, 255, 0), 50); // Green
	colorWipe(strip_a.Color(0, 0, 255), 50); // Blue
	colorWipe(strip_b.Color(0, 0, 255), 50); // Blue

	// Do a theater marquee effect in various colors...
	theaterChase(strip_a.Color(127, 127, 127), 50); // White, half brightness
	theaterChase(strip_b.Color(127, 127, 127), 50); // White, half brightness
	theaterChase(strip_a.Color(127, 0, 0), 50);		// Red, half brightness
	theaterChase(strip_b.Color(127, 0, 0), 50);		// Red, half brightness
	theaterChase(strip_a.Color(0, 0, 127), 50);		// Blue, half brightness
	theaterChase(strip_b.Color(0, 0, 127), 50);		// Blue, half brightness

	rainbow(10);			 // Flowing rainbow cycle along the whole strip
	theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}
