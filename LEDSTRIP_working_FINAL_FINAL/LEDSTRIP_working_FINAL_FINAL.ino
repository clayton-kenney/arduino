/* dots
 *
 * By: John Burroughs
 *
 * Modified by: Andrew Tuline
 * Edited by Clayton K
 * Date: October 2019
 * 
 * Similar to dots by John Burroughs, but uses the FastLED beatsin8() function instead.
 *
 */

#include <FastLED.h>  //bring in fast LED library

//FIXED GLOBAL VARS
#define AUDIO_PIN A1  //Pin for the envelope of the sound detector
#define LED_DT 7  //Define led data pin in
#define COLOR_ORDER RGB  //define color order
#define LED_TYPE NEOPIXEL  //define type of led
#define NUM_LEDS 30  //num of leds in strip
#define POT_PIN A5   //define pot input
#define POT_PIN2 A3  //define blue pot

//define changeable global vars
uint8_t max_bright = 200;  //Define max brightness

//define audio vars
uint8_t musicVal = 0;              //define incoming music value for viz
uint8_t musicMin = 0;              //define music min, hard coded zero for now
uint8_t musicMax = 0;              //define starting point for music max
uint16_t potPrev = 0;              //define pot value read from pin
uint16_t potCurrent = 0;           //value converted into color

uint16_t pot2 = 128;        //define pot 2 values


// Define variables used by the sequences.
int   thisdelay =   60;                                       // A delay value for the sequence(s)
//uint8_t   count =   0;                                        // Count up to 255 and then reverts to 0
uint8_t fadeval = 250;                                        // Trail behind the LED's. Lower => faster fade.

uint8_t bpm = 10;

struct CRGB leds[NUM_LEDS];

void setup() {
  delay(2000); //for safe startup
  Serial.begin(9600); //read music input
  LEDS.addLeds<LED_TYPE, LED_DT>(leds, NUM_LEDS);       // Use this for WS2812B
  
  FastLED.setBrightness(max_bright);  //pass FastLED max brightness var
  set_max_power_in_volts_and_milliamps(5, 500);  //moderate power output using built in library
  potCurrent = analogRead(POT_PIN); //records current pot val for color changing
}
float gHue = 0;         //set hue floating value, will conver to int in loop
float gHueDelta = .05; //increment hue for slow fade

void loop() {
  musicVal = analogRead(AUDIO_PIN); // records the current volume from analog pin
  FastLED.setBrightness(pot2);
  if (musicVal > musicMax) musicMax = musicVal; //resets max vol to account for volume increases
  FastLED.show();         //start FastLED
  pot_change();
  dot_beat();  //begin dot_beat program
  //gHue += gHueDelta; // compute new hue value, automatically in range
  //potPrev = (int) gHue;
  
} //setup()

void dot_beat() {
  bpm = musicVal/15;
  uint8_t inner = beatsin8(bpm, NUM_LEDS/4, NUM_LEDS/4*3);    // Move 1/4 to 3/4
  uint8_t outer = beatsin8(bpm, 0, NUM_LEDS-1);               // Move entire length
  uint8_t middle = beatsin8(bpm, NUM_LEDS/3, NUM_LEDS/3*2);   // Move 1/3 to 2/3
  
  leds[middle] = CHSV((potPrev - 75), 255, 255);
  leds[inner] = CHSV((potPrev -50), 255, 255);
  leds[outer] = CHSV(potPrev, 255, 255);
  fadeval = pot2;  //modify fadeval(num of light still on) with pot 2
  nscale8(leds,NUM_LEDS,fadeval);                             // Fade the entire array. Or for just a few LED's, use  nscale8(&leds[2], 5, fadeval);

} // dot_beat()

void pot_change() {
  unsigned long currentMillis = millis();  //declare counting variable
  static unsigned long startTime = 0;   //set start to zero, one
  static int index = 0; 
  // Once each second, check the position of the pot
  if (currentMillis - startTime >= 500) {   //define interval period
    startTime = currentMillis;
    
    potCurrent = analogRead(POT_PIN); //records current pot val for color changing
    if (potCurrent != potPrev && potCurrent != 1023) {
      potPrev = potCurrent;
      potPrev = map(potPrev, 0, 1025, 255, 0);
      }
  pot2 = analogRead(POT_PIN2); //read serial pot 2 (blue) 
  pot2 = map(pot2, 1023, 0, 0, 255); //adjust fadeval var
  }
}  // pot_change()
