uint8_t mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x94, 0x6F };

// If IP_ADDRESS is bit defined, DHCP will be used.
/*#define IP_ADDRESS IPAddress(10, 0, 112, 251)
#define DNS_ADDRESS IPAddress(153, 32, 14, 247)
#define GATEWAY_ADDRESS IPAddress(10, 0, 112, 1)
#define SUBNET_MASK IPAddress(255, 255, 252, 0)*/

// Define time server, either as hostname or an IP address
//#define TIME_SERVER "time.nist.gov"
#define TIME_SERVER IPAddress(153, 32, 14, 247)

// If defined, will sleep
#define SLEEP 1

// If defined, the sign will turn off when sign location equals locationOnDay for today.
//#define SIGN_LOCATION "San Jose"

//US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, dowSunday, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, dowSunday, Nov, 2, -480};
Timezone timezone(usPDT, usPST);

// Location data
#define repeat5(x) x, x, x, x, x
char locationOnDay[5][21] = { "San Francisco", "San Jose", "San Francisco", "San Jose", "San Francisco" };
char propositionOnDay[5][3] = { repeat5("in") };

int startOfDayInMinutes[5] = { repeat5(11 * 60) };
bool dayStarted[5] = {false, false, false, false, false};
