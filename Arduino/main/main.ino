// Ard.Pin - 328.pin - Device Pin    -   Device
// 00  RXD    PD0       USB               USB Cable
// 01  TXD    PD1       USB               USB Cable
// 02  INT0   PD2       PIR               PIR
// 03~ INT1   PD3       DFPlayer-Busy     DFMini Busy (16)
// 04         PD4       
// 05~        PD5       
// 06~        PD6       NeoPixel          NeoPixel(in) 
// 07         PD7       NeoPixel          NeoPixel(in)
// 08         PB0       
// 09~        PB1          
// 10~        PB2       RX                DFMini TX
// 11~        PB3       TX                DFMini RX
// 12         PB4       
// 13         PB5       
// A0         PC0       
// A1         PC1       
// A2         PC2       
// A3         PC3       
// A4         PC4       
// A5         PC5

/**
Required Arduino Libraries:
 *    . Adafruit_NeoPixel by Adafruit                 
 *    . DFRobotDFPlayerMini by DFRobot
 *    . SoftwareSerial 
 *    . Wire
*/

#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "SchedulerVTimer.h"
#include "DFRobotDFPlayerMini.h"
#include "defs.h"
#include <Wire.h>

// Object creation
/**> Object referring to the right eye NeoPixel */
Adafruit_NeoPixel pixelsRight(NUMPIXELS, NEOPIXELPINRIGHT, NEO_GRB + NEO_KHZ800);
/**> Object referring to the left eye NeoPixel */
Adafruit_NeoPixel pixelsLeft(NUMPIXELS, NEOPIXELPINLEFT, NEO_GRB + NEO_KHZ800);
/**> SoftwareSerial object used by DFPlayerMini */
SoftwareSerial dfMiniSerial(RXPIN, TXPIN);
/**> DFPlayerMini object */
DFRobotDFPlayerMini myDFPlayer;

// Function signatures
void toggleEyes(void);
void resetColor(void);
void playNextSong(void);
void allowPinInterrupt(uint8_t source);
void stopPinInterrupt(uint8_t source);

// Global variables
/**> Tell whether the LED eyes are on or off */
volatile bool eyesOn = false;
volatile bool playing = false;
/**> Value for white in NeoPixel's library */
volatile const uint32_t WHITE = pixelsRight.Color(0, 0, 0);
/**> Value for red in NeoPixel's library */
volatile const uint32_t RED = pixelsRight.Color(255, 0, 0);
/**> Value for color absence in NeoPixel's library */
volatile const uint32_t OFF = pixelsRight.Color(0, 0, 0);
/**> Current eyes color */
volatile uint32_t currentColor = WHITE;
/**> Counter for the next song to be played */
volatile uint8_t nextSong = 1;
volatile uint8_t currentByte = 0;
char* strings[] = {"temperature: ", "humidity: "};
volatile int8_t data[2] = {0, 0};

/**
 * Setup function in Arduino's programming model,
 * runs only once at program start
 */
void setup() {
  // Initialize SWSerial for DF Mini Player
  dfMiniSerial.begin(9600);
  Serial.begin(9600);
  // Initialize DFPMini Player
  myDFPlayer.begin(dfMiniSerial, true, false);
  // Setup both vtimer and app tasks 
  setupTasks(vtimer, toggleEyes);
  setupTasks(app, playNextSong);
  // Start the virtual timer module
  initSchedulerVTTimer();

  // Allows an interrupt on the PIR sensor pin (unused)
  allowPinInterrupt(PIN2);

  // Join I2C bus with address 0x20
  Wire.begin(0x20);               
  Wire.onReceive(receiveEvent);  

  // Initialize NeoPixel LEDs
  pixelsRight.begin(); 
  pixelsLeft.begin();

  // Determines the initial NeoPixel color
  for (int i = 0; i < NUMPIXELS; i++)
    pixelsRight.setPixelColor(i, currentColor);
  for (int i = 0; i < NUMPIXELS; i++)
    pixelsLeft.setPixelColor(i, currentColor);

  // Start a timer to blink the eyes every 200ms (currently, eye color is none)
  startVTimer(toggleEyes, 200, UNUSED);
}

/**
 * Loop function in Arduino's programming model,
 * runs while program is active
 */
void loop() {
  // Busca tasks para executar 
  procTasks();
}

/**
 * Blinks the LED eyes. Toggles state.
 * The 200ms timer is restarted.
 */
void toggleEyes(void) {
  if (eyesOn) { 
    for (int i = 0; i < NUMPIXELS; i++)
      pixelsRight.setPixelColor(i, OFF);
    for (int i = 0; i < NUMPIXELS; i++)
      pixelsLeft.setPixelColor(i, OFF);
    pixelsRight.show();
    pixelsLeft.show();
    eyesOn = false;
  } else {
    for (int i = 0; i < NUMPIXELS; i++)
      pixelsRight.setPixelColor(i, currentColor);
    for (int i = 0; i < NUMPIXELS; i++)
      pixelsLeft.setPixelColor(i, currentColor);
    pixelsRight.show();
    pixelsLeft.show();
    eyesOn = true;
  }
  // Restarts timer
  startVTimer(toggleEyes, 200, UNUSED);
}

/**
 * Change eye color to white for the next eye update
 */
void resetColor(void) {
  currentColor = WHITE;
  for (int i = 0; i < NUMPIXELS; i++)
    pixelsRight.setPixelColor(i, currentColor);
  for (int i = 0; i < NUMPIXELS; i++)
    pixelsLeft.setPixelColor(i, currentColor);
}

/** 
 * Play DFMiniPlayer's next song. Checks the next song to be executed
 * and changes a flag to tell it's currently playing. Allows interrupts
 * on pin 3 (BUSY pin), which should be generated when the song is over
 */
void playNextSong(void) {
  myDFPlayer.play(nextSong);
  nextSong++;
  if (nextSong > NUMSONGS)
    nextSong = 1;
  // Allows an interrupt on busy pin (goes to HIGH when song ends)
  allowPinInterrupt(PIN3);
}

/**
 * Deals with an INT0 interrupt (pin 2). Changes eye color to red, stops interrups on 
 * PIR sensor and queues task to play next song.
 */
ISR (INT0_vect) {
  // Change eye color to red
  currentColor = RED;
  for (int i = 0; i < NUMPIXELS; i++)
    pixelsRight.setPixelColor(i, currentColor);
  for (int i = 0; i < NUMPIXELS; i++)
    pixelsLeft.setPixelColor(i, currentColor);
  // Changes take effect
  pixelsLeft.show();
  pixelsRight.show();
  // Eyes now on
  eyesOn = true;
  // Stops any PIR sensor interrupt
  stopPinInterrupt(PIN2);
  // Plays next song next time loop is executed
  postTask(app, playNextSong, UNUSED);
}


/**
 * Deals with an INT0 interrupt (pin 3). Resets eye color to default after song ends.
 */lor
ISR (INT1_vect) {
  resetColor();
  playing = false;
  // Stops busy pin interrupts
  stopPinInterrupt(PIN3);
  // Allows PIR interrupts
  allowPinInterrupt(PIN2);
}

/**
 * Allows INT0 or INT1 interrupts.
 * @param source interrupt source, can be 0 or 1
 */
void allowPinInterrupt(uint8_t source) {
  // Interrupt on rising
  if (!source) {
    EICRA |= (1 << ISC01) | (1 << ISC00);
    EIMSK |= (1 << INT0);
  } else {
    EICRA |= (1 << ISC11) | (1 << ISC10);
    EIMSK |= (1 << INT1);
  }
}

/**
 * Stops INT0 or INT1 interrupts.
 * @param source interrupt source, can be 0 or 1
 */
void stopPinInterrupt(uint8_t source) {
  if (!source)
    EIMSK = clearBit(EIMSK, INT0);
  else
    EIMSK = clearBit(EIMSK, INT1);
}

/** 
 * Verifies if an event occurred and sends it through I2C. 
 */
void receiveEvent(int howMany) {
  Serial.print(strings[currentByte]);
  while (Wire.available() > 0) {
    int8_t byte = Wire.read();
    Serial.println(byte);
    if (byte == 1 && !playing){
      // Muda cor dos olhos para vermelho
      currentColor = RED;
      for (int i = 0; i < NUMPIXELS; i++)
        pixelsRight.setPixelColor(i, currentColor);
      for (int i = 0; i < NUMPIXELS; i++)
        pixelsLeft.setPixelColor(i, currentColor);
      // Faz mudanças tomarem efeito
      pixelsLeft.show();
      pixelsRight.show();
      // Olhos agora estão ligados
      eyesOn = true;
      playing = true;
      postTask(app, playNextSong, UNUSED);
    }
    else{
      data[currentByte] = byte;
      Serial.println(data[currentByte]);
      currentByte = !currentByte;
    }
  }
}
