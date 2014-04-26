void setupLCD() { 
  lcd.setMCPType(LTI_TYPE_MCP23008); 
  lcd.begin(20, 4);
}

void loopLCD() {
  
  time_t t = now();
  int dow = weekday(t);
  
  if (dow > dowSunday && dow < dowSaturday) {
    
    if(mode == MODE_OFF) {
      
       int h = hour(t);
       if(h >= 6 && h < 18) {
         updateLCD(dow - dowMonday, false);
         modeOn();
       }
         
    } else if(mode == MODE_ON) {
      
      dow -= dowMonday;
  
      char* location = locationOnDay[dow];
      bool started = dayStarted[dow];
      
      bool shouldStartDay = !started && elapsedSecsToday(t) / 60 >= startOfDayInMinutes[dow];
   
   
      #ifdef SIGN_LOCATION
      if(strncmp(location, SIGN_LOCATION, sizeof(SIGN_LOCATION)) == 0) {
        
        // Shut off at start of day, since will be in office
        if(shouldStartDay) {
          dayStarted[dow] = true;
          modeOff();
        }
        
      } else {
      #endif
      
        if(shouldStartDay) {
          dayStarted[dow] = true;
          updateLCD(dow, true);
        }
        
        if(hour(t) >= 18)
          modeOff();
          
      #ifdef SIGN_LOCATION
      }
      #endif
    }
  }
}

void updateLCD() {
  int dow = weekday() - dowMonday;
  updateLCD(dow, dayStarted[dow]);
}

void updateLCD(int dow, bool started) {
  
  char* location = locationOnDay[dow];
  char* proposition = propositionOnDay[dow];
  
  lcd.clear();
  
  const char* line;
  if(started)
    line = "Charles is";
  else
    line = "Charles will be";
  
  char buffer[21];
  char *bufferPtr = buffer;
  
  int n = sizeof(buffer) - 1;
  while(*line && n--)
    *bufferPtr++ = *line++;
   
  if(*proposition) { 
    if(n--)
      *bufferPtr++ = ' ';
    
    while(*proposition && n--)
      *bufferPtr++ = *proposition++;
  }
  
  *bufferPtr++ = '\0';
  printLine(buffer);
    
  lcd.setCursor(0, 1);
  printLine(location);
     
  lcd.setCursor(0, 2);
  printLine("today");
}

void printLine(char *input) {
  char swap[21];
  
  int inputLength = strlen(input);
  
  int leftPadding = (20 - inputLength) / 2;
  if(leftPadding < 0)
    leftPadding = 0;
    
  int outputLength = inputLength + leftPadding;
  if(outputLength > 21)
    outputLength = 21;
  
  int i;
  for(i = 0; i < leftPadding; i++)
    swap[i] = ' ';
  for(i = leftPadding; i < outputLength; i++)
    swap[i] = input[i - leftPadding];
    
   swap[outputLength] = '\0';
   
   lcd.print(swap);
}
