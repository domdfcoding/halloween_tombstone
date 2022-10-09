// Adapted from the TFT_eSPI Animated Eyes example.
// https://github.com/Bodmer/TFT_eSPI/tree/master/examples/Generic/Animated_Eyes_1
// License: https://github.com/Bodmer/TFT_eSPI/blob/master/license.txt
//--------------------------------------------------------------------------

//
// Code adapted by Bodmer as an example for TFT_eSPI, this runs on any
// TFT_eSPI compatible processor so ignore the technical limitations
// detailed in the original header below. Assorted changes have been
// made including removal of the display mirror kludge.

//--------------------------------------------------------------------------
// Uncanny eyes for Adafruit 1.5" OLED (product #1431) or 1.44" TFT LCD
// (#2088).  Works on PJRC Teensy 3.x and on Adafruit M0 and M4 boards
// (Feather, Metro, etc.).  This code uses features specific to these
// boards and WILL NOT work on normal Arduino or other boards!
//
// SEE FILE "config.h" FOR MOST CONFIGURATION (graphics, pins, display type,
// etc).  Probably won't need to edit THIS file unless you're doing some
// extremely custom modifications.
//
// Adafruit invests time and resources providing this open source code,
// please support Adafruit and open-source hardware by purchasing products
// from Adafruit!
//
// Written by Phil Burgess / Paint Your Dragon for Adafruit Industries.
// MIT license.  SPI FIFO insight from Paul Stoffregen's ILI9341_t3 library.
// Inspired by David Boccabella's (Marcwolf) hybrid servo/OLED eye concept.
//--------------------------------------------------------------------------

// A simple state machine is used to control eye blinks/winks:
#define NOBLINK 0 // Not currently engaged in a blink
#define ENBLINK 1 // Eyelid is currently closing
#define DEBLINK 2 // Eyelid is currently opening
typedef struct {
	uint8_t state;		// NOBLINK/ENBLINK/DEBLINK
	uint32_t duration;	// Duration of blink state (micros)
	uint32_t startTime; // Time (micros) of last state change
} eyeBlink;

// Autonomous iris motion uses a fractal behavior to similate both the major
// reaction of the eye plus the continuous smaller adjustments that occur.
uint16_t oldIris = (IRIS_MIN + IRIS_MAX) / 2, newIris;
eyeBlink eye_blink_info;

uint32_t startTime; // For FPS indicator

uint16_t pbuffer[BUFFERS][BUFFER_SIZE]; // Pixel rendering buffer
bool dmaBuf = 0;						// DMA buffer selection

// Initialise eyes ---------------------------------------------------------
void initEyes(void) {
	// Initialise eye objects based on config.h:
	Serial.println("Initialise eye objects");

	eye_blink_info.state = NOBLINK;

	pinMode(TFT_CS, OUTPUT);
	digitalWrite(TFT_CS, LOW);

	// Also set up an individual eye-wink pin if defined:
	if (WINK_PIN >= 0)
		pinMode(WINK_PIN, INPUT_PULLUP);
}

void split( // Subdivides motion path into two sub-paths w/randimization
	int16_t startValue, // Iris scale value (IRIS_MIN to IRIS_MAX) at start
	int16_t endValue,	// Iris scale value at end
	uint32_t startTime, // micros() at start
	int32_t duration,	// Start-to-end time, in microseconds
	int16_t range);

// UPDATE EYE --------------------------------------------------------------
void updateEye(void) {
	newIris = random(IRIS_MIN, IRIS_MAX);
	split(oldIris, newIris, micros(), 10000000L, IRIS_MAX - IRIS_MIN);
	oldIris = newIris;
}

// EYE-RENDERING FUNCTION --------------------------------------------------
void drawEye(  // Renders one eye.  Inputs must be pre-clipped & valid.
			   // Use native 32 bit variables where possible as this is 10%
			   // faster!
	uint8_t e, // Eye array index; 0 or 1 for left/right
	uint32_t iScale,  // Scale factor for iris
	uint32_t scleraX, // First pixel X offset into sclera image
	uint32_t scleraY, // First pixel Y offset into sclera image
	uint32_t uT,	  // Upper eyelid threshold value
	uint32_t lT) {	  // Lower eyelid threshold value

	uint32_t screenX, screenY, scleraXsave;
	int32_t irisX, irisY;
	uint32_t p, a;
	uint32_t d;

	uint32_t pixels = 0;

	// Set up raw pixel dump to entire screen.  Although such writes can wrap
	// around automatically from end of rect back to beginning, the region is
	// reset on each frame here in case of an SPI glitch.
	digitalWrite(TFT_CS, LOW);
	tft.startWrite();
	tft.setAddrWindow(EYE_XPOSITION, EYE_YPOSITION, 128, 128);

	// Now just issue raw 16-bit values for every pixel...

	scleraXsave = scleraX; // Save initial X value to reset on each line
	irisY = scleraY - (SCLERA_HEIGHT - IRIS_HEIGHT) / 2;

	// Eyelid image is left<>right swapped for two displays
	uint16_t lidX = 0;
	uint16_t dlidX = -1;
	if (e)
		dlidX = 1;
	for (screenY = 0; screenY < SCREEN_HEIGHT; screenY++, scleraY++, irisY++) {
		scleraX = scleraXsave;
		irisX = scleraXsave - (SCLERA_WIDTH - IRIS_WIDTH) / 2;
		if (e)
			lidX = 0;
		else
			lidX = SCREEN_WIDTH - 1;
		for (screenX = 0; screenX < SCREEN_WIDTH;
			 screenX++, scleraX++, irisX++, lidX += dlidX) {
			if ((pgm_read_byte(lower + screenY * SCREEN_WIDTH + lidX) <= lT) ||
				(pgm_read_byte(upper + screenY * SCREEN_WIDTH + lidX) <=
				 uT)) { // Covered by eyelid
				p = 0;
			} else if ((irisY < 0) || (irisY >= IRIS_HEIGHT) || (irisX < 0) ||
					   (irisX >= IRIS_WIDTH)) { // In sclera
				p = pgm_read_word(sclera + scleraY * SCLERA_WIDTH + scleraX);
			} else { // Maybe iris...
				p = pgm_read_word(polar + irisY * IRIS_WIDTH +
								  irisX);				   // Polar angle/dist
				d = (iScale * (p & 0x7F)) / 128;		   // Distance (Y)
				if (d < IRIS_MAP_HEIGHT) {				   // Within iris area
					a = (IRIS_MAP_WIDTH * (p >> 7)) / 512; // Angle (X)
					p = pgm_read_word(iris + d * IRIS_MAP_WIDTH +
									  a); // Pixel = iris
				} else {				  // Not in iris
					p = pgm_read_word(sclera + scleraY * SCLERA_WIDTH +
									  scleraX); // Pixel = sclera
				}
			}
			*(&pbuffer[dmaBuf][0] + pixels++) = (p >> 8 | p << 8);

			if (pixels >= BUFFER_SIZE) {
				yield();
#ifdef USE_DMA
				tft.pushPixelsDMA(&pbuffer[dmaBuf][0], pixels);
				dmaBuf = !dmaBuf;
#else
				tft.pushPixels(pbuffer, pixels);
#endif
				pixels = 0;
			}
		}
	}

	if (pixels) {
#ifdef USE_DMA
		tft.pushPixelsDMA(&pbuffer[dmaBuf][0], pixels);
#else
		tft.pushPixels(pbuffer, pixels);
#endif
	}
	tft.endWrite();
	digitalWrite(TFT_CS, HIGH);
}

// EYE ANIMATION -----------------------------------------------------------

const uint8_t ease[] = { // Ease in/out curve for eye movements 3*t^2-2*t^3
	0,	 0,	  0,   0,	0,	 0,	  0,   1,
	1,	 1,	  1,   1,	2,	 2,	  2,   3, // T
	3,	 3,	  4,   4,	4,	 5,	  5,   6,
	6,	 7,	  7,   8,	9,	 9,	  10,  10, // h
	11,	 12,  12,  13,	14,	 15,  15,  16,
	17,	 18,  18,  19,	20,	 21,  22,  23, // x
	24,	 25,  26,  27,	27,	 28,  29,  30,
	31,	 33,  34,  35,	36,	 37,  38,  39, // 2
	40,	 41,  42,  44,	45,	 46,  47,  48,
	50,	 51,  52,  53,	54,	 56,  57,  58, // A
	60,	 61,  62,  63,	65,	 66,  67,  69,
	70,	 72,  73,  74,	76,	 77,  78,  80, // l
	81,	 83,  84,  85,	87,	 88,  90,  91,
	93,	 94,  96,  97,	98,	 100, 101, 103, // e
	104, 106, 107, 109, 110, 112, 113, 115,
	116, 118, 119, 121, 122, 124, 125, 127, // c
	128, 130, 131, 133, 134, 136, 137, 139,
	140, 142, 143, 145, 146, 148, 149, 151, // J
	152, 154, 155, 157, 158, 159, 161, 162,
	164, 165, 167, 168, 170, 171, 172, 174, // a
	175, 177, 178, 179, 181, 182, 183, 185,
	186, 188, 189, 190, 192, 193, 194, 195, // c
	197, 198, 199, 201, 202, 203, 204, 205,
	207, 208, 209, 210, 211, 213, 214, 215, // o
	216, 217, 218, 219, 220, 221, 222, 224,
	225, 226, 227, 228, 228, 229, 230, 231, // b
	232, 233, 234, 235, 236, 237, 237, 238,
	239, 240, 240, 241, 242, 243, 243, 244, // s
	245, 245, 246, 246, 247, 248, 248, 249,
	249, 250, 250, 251, 251, 251, 252, 252, // o
	252, 253, 253, 253, 254, 254, 254, 254,
	254, 255, 255, 255, 255, 255, 255, 255}; // n

#ifdef AUTOBLINK
uint32_t timeOfLastBlink = 0L, timeToNextBlink = 0L;
#endif

// Process motion for a single frame of left or right eye
void frame(uint16_t iScale) // Iris scale (0-1023)
{
	static uint32_t frames = 0; // Used in frame rate calculation
	int16_t eyeX, eyeY;
	uint32_t t = micros(); // Time at start of function

	if (!(++frames & 255)) { // Every 256 frames...
		float elapsed = (millis() - startTime) / 1000.0;
		if (elapsed)
			Serial.println((uint16_t)(frames / elapsed)); // Print FPS
	}

	// Autonomous X/Y eye motion
	// Periodically initiates motion to a new random point, random speed,
	// holds there for random period until next motion.

	static bool eyeInMotion = false;
	static int16_t eyeOldX = 512, eyeOldY = 512, eyeNewX = 512, eyeNewY = 512;
	static uint32_t eyeMoveStartTime = 0L;
	static int32_t eyeMoveDuration = 0L;

	int32_t dt = t - eyeMoveStartTime; // uS elapsed since last eye event
	if (eyeInMotion) {				   // Currently moving?
		if (dt >= eyeMoveDuration) {   // Time up?  Destination reached.
			eyeInMotion = false;	   // Stop moving
			eyeMoveDuration = random(3000000); // 0-3 sec stop
			eyeMoveStartTime = t;			   // Save initial time of stop
			eyeX = eyeOldX = eyeNewX;		   // Save position
			eyeY = eyeOldY = eyeNewY;
		} else { // Move time's not yet fully elapsed -- interpolate position
			int16_t e = ease[255 * dt / eyeMoveDuration] + 1;	// Ease curve
			eyeX = eyeOldX + (((eyeNewX - eyeOldX) * e) / 256); // Interp X
			eyeY = eyeOldY + (((eyeNewY - eyeOldY) * e) / 256); // and Y
		}
	} else { // Eye stopped
		eyeX = eyeOldX;
		eyeY = eyeOldY;
		if (dt > eyeMoveDuration) { // Time up?  Begin new move.
			int16_t dx, dy;
			uint32_t d;
			do { // Pick new dest in circle
				eyeNewX = random(1024);
				eyeNewY = random(1024);
				dx = (eyeNewX * 2) - 1023;
				dy = (eyeNewY * 2) - 1023;
			} while ((d = (dx * dx + dy * dy)) > (1023 * 1023)); // Keep trying
			eyeMoveDuration = random(72000, 144000); // ~1/14 - ~1/7 sec
			eyeMoveStartTime = t; // Save initial time of move
			eyeInMotion = true;	  // Start move on next frame
		}
	}

	// Blinking
#ifdef AUTOBLINK
	// Similar to the autonomous eye movement above -- blink start times
	// and durations are random (within ranges).
	if ((t - timeOfLastBlink) >= timeToNextBlink) { // Start new blink?
		timeOfLastBlink = t;
		uint32_t blinkDuration = random(36000, 72000); // ~1/28 - ~1/14 sec
		// Set up durations for both eyes (if not already winking)
		if (eye_blink_info.state == NOBLINK) {
			eye_blink_info.state = ENBLINK;
			eye_blink_info.startTime = t;
			eye_blink_info.duration = blinkDuration;
		}
		timeToNextBlink = blinkDuration * 3 + random(4000000);
	}
#endif

	if (eye_blink_info.state) { // Eye currently blinking?
		// Check if current blink state time has elapsed
		if ((t - eye_blink_info.startTime) >= eye_blink_info.duration) {
			// Yes -- increment blink state, unless...
			if ((eye_blink_info.state == ENBLINK) &&
				( // Enblinking and...

					((WINK_PIN >= 0) && digitalRead(WINK_PIN) == LOW))) {
				// Don't advance state yet -- eye is held closed instead
			} else { // No buttons, or other state...
				if (++eye_blink_info.state > DEBLINK) { // Deblinking finished?
					eye_blink_info.state = NOBLINK;		// No longer blinking
				} else { // Advancing from ENBLINK to DEBLINK mode
					eye_blink_info.duration *=
						2; // DEBLINK is 1/2 ENBLINK speed
					eye_blink_info.startTime = t;
				}
			}
		}
	} else { // Not currently blinking...check buttons!

		if ((WINK_PIN >= 0) && (digitalRead(WINK_PIN) == LOW)) { // Wink!
			eye_blink_info.state = ENBLINK;
			eye_blink_info.startTime = t;
			eye_blink_info.duration = random(45000, 90000);
		}
	}

	// Process motion, blinking and iris scale into renderable values

	// Scale eye X/Y positions (0-1023) to pixel units used by drawEye()
	eyeX = map(eyeX, 0, 1023, 0, SCLERA_WIDTH - 128);
	eyeY = map(eyeY, 0, 1023, 0, SCLERA_HEIGHT - 128);

	// Horizontal position is offset so that eyes are very slightly crossed
	// to appear fixated (converged) at a conversational distance.  Number
	// here was extracted from my posterior and not mathematically based.
	// I suppose one could get all clever with a range sensor, but for now...

	if (eyeX > (SCLERA_WIDTH - 128))
		eyeX = (SCLERA_WIDTH - 128);

	// Eyelids are rendered using a brightness threshold image.  This same
	// map can be used to simplify another problem: making the upper eyelid
	// track the pupil (eyes tend to open only as much as needed -- e.g. look
	// down and the upper eyelid drops).  Just sample a point in the upper
	// lid map slightly above the pupil to determine the rendering threshold.
	static uint8_t uThreshold = 128;
	uint8_t lThreshold, n;
#ifdef TRACKING
	int16_t sampleX = SCLERA_WIDTH / 2 - (eyeX / 2), // Reduce X influence
		sampleY = SCLERA_HEIGHT / 2 - (eyeY + IRIS_HEIGHT / 4);
	// Eyelid is slightly asymmetrical, so two readings are taken, averaged
	if (sampleY < 0)
		n = 0;
	else
		n = (pgm_read_byte(upper + sampleY * SCREEN_WIDTH + sampleX) +
			 pgm_read_byte(upper + sampleY * SCREEN_WIDTH +
						   (SCREEN_WIDTH - 1 - sampleX))) /
			2;
	uThreshold = (uThreshold * 3 + n) / 4; // Filter/soften motion
	// Lower eyelid doesn't track the same way, but seems to be pulled upward
	// by tension from the upper lid.
	lThreshold = 254 - uThreshold;
#else // No tracking -- eyelids full open unless blink modifies them
	uThreshold = lThreshold = 0;
#endif

	// The upper/lower thresholds are then scaled relative to the current
	// blink position so that blinks work together with pupil tracking.
	if (eye_blink_info.state) { // Eye currently blinking?
		uint32_t s = (t - eye_blink_info.startTime);
		if (s >= eye_blink_info.duration)
			s = 255; // At or past blink end
		else
			s = 255 * s / eye_blink_info.duration; // Mid-blink
		s = (eye_blink_info.state == DEBLINK) ? 1 + s : 256 - s;
		n = (uThreshold * s + 254 * (257 - s)) / 256;
		lThreshold = (lThreshold * s + 254 * (257 - s)) / 256;
	} else {
		n = uThreshold;
	}

	// Pass all the derived values to the eye-rendering function:
	drawEye(0, iScale, eyeX, eyeY, n, lThreshold);
}

// AUTONOMOUS IRIS SCALING
// Autonomous iris motion uses a fractal behavior to similate both the major
// reaction of the eye plus the continuous smaller adjustments that occur.

void split( // Subdivides motion path into two sub-paths w/randimization
	int16_t startValue, // Iris scale value (IRIS_MIN to IRIS_MAX) at start
	int16_t endValue,	// Iris scale value at end
	uint32_t startTime, // micros() at start
	int32_t duration,	// Start-to-end time, in microseconds
	int16_t range) {	// Allowable scale value variance when subdividing

	if (range >= 8) {  // Limit subdvision count, because recursion
		range /= 2;	   // Split range & time in half for subdivision,
		duration /= 2; // then pick random center point within range:
		int16_t midValue = (startValue + endValue - range) / 2 + random(range);
		uint32_t midTime = startTime + duration;
		split(startValue, midValue, startTime, duration, range); // First half
		split(midValue, endValue, midTime, duration, range);	 // Second half
	} else {		// No more subdivisons, do iris motion...
		int32_t dt; // Time (micros) since start of motion
		int16_t v;	// Interim value
		while ((dt = (micros() - startTime)) < duration) {
			v = startValue + (((endValue - startValue) * dt) / duration);
			if (v < IRIS_MIN)
				v = IRIS_MIN; // Clip just in case
			else if (v > IRIS_MAX)
				v = IRIS_MAX;
			frame(v); // Draw frame w/interim iris scale value
		}
	}
}
