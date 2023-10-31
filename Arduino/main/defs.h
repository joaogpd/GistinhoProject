#ifndef DEFS_H
#define DEFS_H

/**> NeoPixel LED right eye pin */
#define NEOPIXELPINRIGHT 6 
/**> NeoPixel LED left eye pin */
#define NEOPIXELPINLEFT 7
/**> PIR sensor pin (unused) */
#define PIRPIN 2
/**> DFMiniPlayer TX pin (for the Arduino, RX) */
#define RXPIN 10
/**> DFMiniPlayer RX pin (for the Arduino, TX) */
#define TXPIN 11
/**> Interrupt value referring to pin 2 (INT0) */
#define PIN2 0
/**> Interrupt value referring to pin 3 (INT1) */
#define PIN3 1
/**> Number of NeoPixel LEDs in each pin */
#define NUMPIXELS 2
/**> Amount of songs from the SD card that will be played */
#define NUMSONGS 15
/**> Value used to ignore the unused parameter from SchedulerVTimer */
#define UNUSED 0
/**> Clears a given bit in a register */
#define clearBit(reg, bit) (reg &= ~(1 << bit))
/**> Used for debug purposes */
#define REDLEDPIN 9
#define GREENLEDPIN 8
#define YELLOWLEDPIN 1

#endif
