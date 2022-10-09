// GRAPHICS SETTINGS (appearance of eye) -----------------------------------

// This option enables a a simpler "football-shaped" eye that's left/right
// symmetrical. The default shape includes the caruncle, creating distinct
// left/right eyes.

#define SYMMETRICAL_EYELID

// Enable ONE of these #includes -- HUGE graphics tables for various eyes:
#include "data/defaultEye.h" // Standard human-ish hazel eye -OR-
// #include "data/dragonEye.h"     // Slit pupil fiery dragon/demon eye -OR-
//#include "data/noScleraEye.h"   // Large iris, no sclera -OR-
// #include "data/goatEye.h"       // Horizontal pupil goat/Krampus eye -OR-
// #include "data/newtEye.h"       // Eye of newt -OR-
//#include "data/terminatorEye.h" // Git to da choppah!
//#include "data/catEye.h"        // Cartoonish cat (flat "2D" colors)
//#include "data/owlEye.h"        // Minerva the owl (DISABLE TRACKING)
//#include "data/naugaEye.h"      // Nauga googly eye (DISABLE TRACKING)
//#include "data/doeEye.h"        // Cartoon deer eye (DISABLE TRACKING)

// DISPLAY HARDWARE SETTINGS (screen type & connections) -------------------
#define TFT_ROTATION 0	 // TFT rotation
#define EYE_XPOSITION 0	 // x shift for eye
#define EYE_YPOSITION 13 // y shift for eye

#define DISPLAY_BACKLIGHT -1 // Pin for backlight control (-1 for none)
#define BACKLIGHT_MAX 255

// Eye motion settings -----------------------------------------------------

#define BLINK_PIN -1 // Pin for manual blink button (BOTH eyes)
#define WINK_PIN -1	 // Left wink pin (set to -1 for no pin)

#define TRACKING  // If defined, eyelid tracks pupil
#define AUTOBLINK // If defined, eyes also blink autonomously

#if !defined(IRIS_MIN) // Each eye might have its own MIN/MAX
#define IRIS_MIN 90	   // Iris size (0-1023) in brightest light
#endif
#if !defined(IRIS_MAX)
#define IRIS_MAX 130 // Iris size (0-1023) in darkest light
#endif
