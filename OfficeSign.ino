#include <Wire.h>
#include <LiquidTWI2.h>         
#include <Ethernet.h>
#include <Time.h>
#include <Timezone.h>
#include "types.h"

#define repeat5(x) x, x, x, x, x

//-- BEGIN CONFIGURATION

#define MAC_ADDRESS { 0x90, 0xA2, 0xDA, 0x0D, 0x94, 0x6F }

// If defined, will use static configuration
/*#define IP_ADDRESS IPAddress(10, 0, 112, 251)
#define DNS_ADDRESS IPAddress(153, 32, 14, 247)
#define GATEWAY_ADDRESS IPAddress(10, 0, 112, 1)
#define SUBNET_MASK IPAddress(255, 255, 252, 0)*/

// Define time server, either as hostname or an IP address
//#define TIME_SERVER "time.nist.gov"
#define TIME_SERVER IPAddress(153, 32, 14, 247)

// If defined, will sleep
#define SLEEP 1

// If defined, tPhe sign will turn off when sign location equals locationOnDay for today.
//#define SIGN_LOCATION "San Jose"

//US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, dowSunday, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, dowSunday, Nov, 2, -480};
Timezone timezone(usPDT, usPST);

//-- END CONFIGURATION

char locationOnDay[5][21] = { "San Francisco", "San Jose", "San Francisco", "San Jose", "San Francisco" };
char propositionOnDay[5][3] = { repeat5("in") };
int startOfDayInMinutes[5] = { repeat5(11 * 60) };

uint8_t mac[] = MAC_ADDRESS;

#define MODE_ON 1
#define MODE_OFF 0

volatile bool watchdogTripped = true;

int mode;
// Connect via i2c, default address #0 (A0-A2 not jumpered)
// DAT (SDA) to A4
// CLK (SCL) to A5
LiquidTWI2 lcd(0);

// Turn off LED pin 9 on Arduino Ethernet
#define PIN_LED 9

void setup() {
  setupPower();

  // Turn off LED pin
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  lcd.setMCPType(LTI_TYPE_MCP23008); 
  lcd.begin(20, 4);
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


