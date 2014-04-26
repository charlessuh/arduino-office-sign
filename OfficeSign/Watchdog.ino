/*
 * Based on: http://donalmorrissey.blogspot.com/2010/04/sleeping-arduino-part-5-wake-up-via.html
 */

void setupWatchdog() {
  // Setup watchdog timer with interrupt mode and 8s
  MCUSR &= ~(1 << WDRF);
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = 1 << WDIE | 1 << WDP0 | 1 << WDP3;
}

ISR(WDT_vect) {
  watchdogTripped = true;
}
