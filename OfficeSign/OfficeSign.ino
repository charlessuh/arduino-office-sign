#include <Wire.h>
#include <LiquidTWI2.h>         
#include <Ethernet.h>
#include <Time.h>
#include <Timezone.h>

#include "types.h"
#include "config.h"

#define MODE_ON 1
#define MODE_OFF 0

// Connect via i2c, default address #0 (A0-A2 not jumpered)
// On Uno: DAT (SDA) to A4, CLK (SCL) to A5
LiquidTWI2 lcd(0);

volatile bool watchdogTripped = true;

int mode;

// Turn off LED pin 9 on Arduino Ethernet
#define PIN_LED 9

void setup() {
  setupPower();

  // Turn off LED pin
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  setupLCD();
  setupBacklight();

  printLine("Initializing..");
  
  #ifdef IP_ADDRESS
    Ethernet.begin(mac, IP_ADDRESS, DNS_ADDRESS, GATEWAY_ADDRESS, SUBNET_MASK);
  #else
    // Use DHCP if no IP Address specified
    while(Ethernet.begin(mac) == 0) {
      lcd.clear();
      printLine("No Ethernet cable.");
    }
  #endif

  // int hr,int min,int sec,int dy, int mnth, int yr
  //setTime(10, 59, 30, 1, 3, 2013);
  setupNTP();

  setupServer();
  setupWatchdog();

  modeOff();
}


void loop() {

#ifdef SLEEP
  if(watchdogTripped) {
#endif

    if(mode == MODE_ON)
      fadeBacklight();

    if(loopServer())
      updateLCD();
    else
      loopLCD();

    #ifndef IP_ADDRESS
    Ethernet.maintain();
    #endif

#ifdef SLEEP
    // Re-enter sleep
    watchdogTripped = false;
    enterSleep();
  }
#endif
}

void modeOn() {
  lcd.setBacklight(HIGH);
  resetBacklight();

  lcd.display();

  mode = MODE_ON;
}

void modeOff() {
  lcd.setBacklight(LOW);
  setBacklight(0, 0, 0);

  lcd.noDisplay();

  mode = MODE_OFF;
}
