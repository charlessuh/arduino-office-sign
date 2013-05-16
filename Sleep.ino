#include <avr/sleep.h>
#include <avr/power.h>

unsigned int sleep_count = 0;

void setupPower() {
  power_adc_disable();
  power_timer1_disable();
  
  #ifndef DEBUG
  power_usart0_disable();
  #endif
  
  // Disable ACD
  ACSR |= 1 << ACD;
}

void enterSleep() {
  static int sleep_mode;
  
  // When backlight is on, we need
  // timer0 and timer2 for the 3 PWM pins.
  if(mode == MODE_ON)
    sleep_mode = SLEEP_MODE_IDLE;
  else
    sleep_mode = SLEEP_MODE_PWR_DOWN;
  
  set_sleep_mode(sleep_mode);
  sleep_enable();

  if(sleep_mode == SLEEP_MODE_IDLE) {
    power_twi_disable();
    power_spi_disable();
  }
  
  sleep_mode();
  sleep_disable(); 

  if(sleep_mode == SLEEP_MODE_IDLE) {
    power_twi_enable();
    power_spi_enable();
  } else {
    if(sleep_count % 3 == 0)
      adjustTime(9);
    else
      adjustTime(8);
    sleep_count++;
  }
}

