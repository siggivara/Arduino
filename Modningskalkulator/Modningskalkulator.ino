#include <LiquidCrystal.h>
#include <Wire.h>
#include "Lib/RTC/RTClib.h"

// ###################### Thermistor section #################################
// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 50
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    
 
int samples[NUMSAMPLES];


// ######################### end thermistor section ##########################


// ######################## LCD section ######################################
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// ######################## end LCD section ##################################


// ######################## RTC section ######################################
RTC_DS1307 RTC;
// ######################## end RTC section ################################## 


// ######################## Other section ######################################
const int buttonPin = 13;
float lastButtonState = 0;
float MaturityMinutesWhenDone = 57600;
float NumberOfMaturityMinutesElapsed = 0.0;
DateTime LastUpdate;
int minutesPassed = 0;
int hoursPassed = 0;
// ######################## end Other section ################################## 

void setup(void) {
  // RTC
  Wire.begin();
  RTC.begin();
  // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
  LastUpdate = RTC.now();

  // LCD
  lcd.begin(16, 2);
  updateTemperature(getTemperature());
  updateMaturityOnLcd(RTC.now());
  
  // Other
  pinMode(buttonPin, INPUT);
}
 
void loop(void) {
  readButton();
  
  float temperature = getTemperature();
  updateTemperature(temperature);

  updateMaturityMinutes(temperature);

  updatePercentDone();

  updateTimePased();

  updateTimeLeft();
  
  //updateDisplayClock();
}

void updateTimePased(){
  hoursPassed = RTC.now().hour();
  minutesPassed = RTC.now().minute();
  lcd.setCursor(12, 0);
  lcd.print(hoursPassed);
  lcd.print("t");
  //lcd.print(minutesPassed);
  //lcd.print("m");
}

void updateTimeLeft() {
  int hoursLeft = (MaturityMinutesWhenDone - NumberOfMaturityMinutesElapsed)/60;
  int minutesLeft = MaturityMinutesWhenDone - NumberOfMaturityMinutesElapsed - (hoursLeft*60);
  lcd.setCursor(12, 1);
  lcd.print(hoursLeft);
  lcd.print("t");
  //lcd.print(minutesLeft);
  //lcd.print("m");
}

void updateTemperature(float temperature){
  
  lcd.setCursor(0, 0);
  lcd.print(temperature, 1);
  lcd.print("C");
}

void updateMaturityMinutes(float temperature) {
  DateTime currentTime = RTC.now();
  if(currentTime.minute() != LastUpdate.minute()) {
    NumberOfMaturityMinutesElapsed += temperature;
    updateMaturityOnLcd(currentTime);
  }
}

void updateMaturityOnLcd(DateTime currentTime) {
  lcd.setCursor(0, 1);
  lcd.print((NumberOfMaturityMinutesElapsed/60.0)/24.0, 1);
  lcd.print("DG");
  LastUpdate = currentTime;

  
}

void updatePercentDone(){
  lcd.setCursor(6, 1);
  lcd.print(NumberOfMaturityMinutesElapsed/MaturityMinutesWhenDone);
  lcd.print("%");
}


void updateDisplayClock() {
  DateTime now = RTC.now();
  lcd.setCursor(8, 0);
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();
  
  if(hour < 10) {
    lcd.print("0");
    lcd.print(hour);
  }
  else
    lcd.print(hour);
  lcd.print(":");
  if(minute < 10) {
    lcd.print("0");
    lcd.print(minute);
  }
  else
    lcd.print(minute);
  lcd.print(":");
  if(second < 10) {
    lcd.print("0");
    lcd.print(second);
  }
  else
    lcd.print(second);
}

void readButton() {
  // read the state of the pushbutton value:
  int buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH && lastButtonState == LOW) {
    NumberOfMaturityMinutesElapsed = 0.0;
    updateMaturityMinutes(getTemperature());
    hoursPassed = 0;
    minutesPassed = 0;
    lastButtonState = buttonState;
  } else {
    lastButtonState = buttonState;
  }
}


float getTemperature() {
  uint8_t i;
  float average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
 
  return steinhart;
}

