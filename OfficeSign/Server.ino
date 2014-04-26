/**
 * Based on: http://arduino.cc/en/Tutorial/WebServer
 */
 
#include <Ethernet.h>
#include <stdlib.h>

EthernetServer server(80);
EthernetClient client;

void setupServer() {
  server.begin();
}

bool loopServer() {
  
  if(client = server.available()) {
   
    request_method method = getRequestMethod();
    
    bool newLine = false;
    while(char c = client.read()) {
  
      // Two newlines in a row
      if(c == '\n' && newLine) {
        if(method == POST) {
          readParams();
          client.println(F("HTTP/1.1 302 Found"));
          client.println(F("Location: /"));
        } else if(method == GET) {
          printPage(); 
        }  
        break;
      }
      
      if (c == '\n') {
        newLine = true;
      } else if (c != '\r') {
        newLine = false;
      }
    }

    delay(1);
    client.stop();
    
    return method == POST;
  }
  
  return false;
}

void printPage() {
  // send a standard http response header
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println(F("Connnection: close"));
  client.println();
  
  client.println(F("<!DOCTYPE HTML><html>"));
  client.println(F("<head><style>"));
  client.println(F("body { font-family: sans-serif; font-size: 13px; padding: 0 15px; }"));
  client.println(F("fieldset { border: solid 1px #ccc; border-radius: 5px; padding: 10px; margin: 15px 0; }"));
  client.println(F("</style></head>"));
  client.println(F("<body><form method=\"post\">"));
  
  time_t t = now();
  
  for(int i = 0; i < 5; i++) {
    client.println(F("<fieldset>"));
    
    client.print(F("<legend>"));
    if(i + dowMonday < weekday(t))
      client.print(F("Next "));
     
    client.print(dayStr(i + dowMonday));
    client.println(F("</legend>"));
    
    client.print(F("<select name=\"proposition[]\">"));
    printOption("(none)", "", propositionOnDay[i] == '\0');
    printOption("in", "in", strncmp(propositionOnDay[i], "in", 2) == 0);
    printOption("at", "at", strncmp(propositionOnDay[i], "at", 2) == 0);
    printOption("on", "on", strncmp(propositionOnDay[i], "on", 2) == 0);
    client.println(F("</select>"));
    
    client.print(F("<input name=\"location[]\" maxlength=\"20\" value=\""));
    client.print(locationOnDay[i]);
    client.print(F("\" tabindex=\""));
    client.print(i + 1);
    client.println(F("\">"));
    
    client.print(F("<input name=\"time[]\" maxlength=\"5\" value=\""));
    client.print(startOfDayInMinutes[i] / 60);
    client.print(F(":"));
    if(startOfDayInMinutes[i] % 60 < 9)
      client.print(F("0"));
    client.print(startOfDayInMinutes[i] % 60);
    client.println(F("\">"));
    
    client.println(F("</fieldset>"));
  }
  client.println(F("<input type=\"submit\" value=\"Save\"></form>"));
  
  // Print time
  client.print(F("<p>The current date/time is "));
  client.print(dayStr(weekday(t)));
  client.print(" ");
  client.print(monthShortStr(month(t)));
  client.print(" ");
  client.print(day(t));
  client.print(" ");
  client.print(year());
  client.print(" ");  
  client.print(hourFormat12(t));
  client.print(":");
  if(minute(t) < 10)
    client.print("0");
  client.print(minute(t));
  client.print(":");
  if(second(t) < 10)
    client.print("0");
  client.print(second(t));
  client.print(isAM() ? F("AM") : F("PM"));
  client.println(F("</p>"));
  
  client.println(F("</body></html>"));
}

void printOption(const char* name, const char* value, bool selected) {
  client.print(F("<option value=\""));
  client.print(value);
  client.print(F("\""));
  if(selected)
    client.print(F(" selected=\"selected\""));
  client.print(F(">"));
  client.print(name);
  client.print(F("</option>"));
}

void readParams() {
  
  char time[6];
    
  for(int i = 0; i < 5; i++) {
    readParam("proposition[]", propositionOnDay[i], sizeof(propositionOnDay[i]));
    readParam("location[]", locationOnDay[i], sizeof(locationOnDay[i]));
    readParam("time[]", time, sizeof(time));
    
    int hour = atoi(time);
    int minute = atoi(time + 3);
    startOfDayInMinutes[i] = hour * 60 + minute;
  }
}


void readParam(const char* name, char* value, unsigned int n) {
  
  char c;
  
  while((c = readChar()) && *name++ == c);
  
  if(c == '=') {
    
    while((c = readChar()) && n-- && c != '&') 
      *value++ = c;
  }
  
  while(n--)
    *value++ = '\0';
}

char readChar() {
  char c = client.read();

  if(c == '+') {
    c = ' ';
  } else if(c == '%') {
    char c1 = client.read();
    char c2 = client.read();
    if(c1 == -1 || c2 == -1) {
       c = 0;
    } else {
      char ch[3] = {c1, c2, '\0'};
      char* endPtr;
      c = strtol(ch, &endPtr, 16);
    }
  }
  
  if(c == -1)
    c = 0;
  
  return c;
}

request_method getRequestMethod() {

  char line[4];
  for(int i = 0; i < 4; i++)
    line[i] = client.read();
  
  if(strncmp(line, "GET", 3) == 0)
    return GET;
  else if(strncmp(line, "POST", 4) == 0)
    return POST;
    
  return INVALID;
}
