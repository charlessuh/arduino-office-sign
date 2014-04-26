/**
 * Code based on: http://learn.adafruit.com/character-lcds/rgb-backlit-lcds
 */

#define PIN_RED 3
#define PIN_GREEN 5
#define PIN_BLUE 6
#define STEP 1

int brightness = 255;

uint8_t r = 0, g = 0, b = 0;

void setupBacklight() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  setBacklight(255, 0, 255);
}

void resetBacklight() {
  r = g = b = 0;
  fadeBacklight();
}

void fadeBacklight() {
#ifndef SLEEP
  static unsigned long previousMillis;
  static unsigned long currentMillis;

  currentMillis = millis();

  if(currentMillis - previousMillis > 8000) {
    previousMillis = currentMillis;
#endif

    if(r < 255) {
      setBacklight(r, 0, 255 - r);
      r += STEP;
    } 
    else {
      if(g < 255) {
        setBacklight(255 - g, g, 0);
        g += STEP;
      } else {
        if(b < 255) {
          setBacklight(0, 255 - b, b);
          b += STEP;
        } else {
          resetBacklight();
        }
      }
    }

#ifndef SLEEP
  }
#endif
}

void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest!
  r = map(r, 0, 255, 0, 100);
  g = map(g, 0, 255, 0, 150);

  if(brightness < 255) {
    r = map(r, 0, 255, 0, brightness);
    g = map(g, 0, 255, 0, brightness);
    b = map(b, 0, 255, 0, brightness);
  }

  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);

  analogWrite(PIN_RED, r);
  analogWrite(PIN_GREEN, g);
  analogWrite(PIN_BLUE, b);
}

