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
DateTime StartTime;
DateTime LastMinuteUpdate;
DateTime LastHourUpdate;
int HoursPassed = -1;  // -1 to make initial update show as 0;
// ######################## end Other section ################################## 

void setup(void) {
  // RTC
  Wire.begin();
  RTC.begin();
  // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(__DATE__, __TIME__));
  StartTime = LastMinuteUpdate = LastHourUpdate = RTC.now();

  // LCD
  lcd.begin(16, 2);
  updateTemperature(getTemperature());
  updateMaturityOnLcd(StartTime);
  updatePercentDone();
  updateTimePassed(StartTime);
  updateTimeLeft(getTemperature(), 0);
  
  // Other
  pinMode(buttonPin, INPUT);
}
 
void loop(void) {
  readButtonState();

  DateTime now = RTC.now();
  if(LastHourUpdate.hour() != now.hour())
    updateTimePassed(now);
  
  float temperature = getTemperature();
  updateTemperature(temperature);

  updateMaturityMinutes(temperature, now);

  updatePercentDone();

  float percentageDone = NumberOfMaturityMinutesElapsed / MaturityMinutesWhenDone;
  updateTimeLeft(temperature, percentageDone);
}

void updateTimePassed(DateTime now){
  HoursPassed++;
    
  if(HoursPassed < 10)
    lcd.setCursor(15, 0);
  else if(HoursPassed < 100)
    lcd.setCursor(14, 0);
  else
    lcd.setCursor(13, 0);
    
  lcd.print(HoursPassed);
  LastHourUpdate = now;
}

void updateTimeLeft(float temperature, float percentageDone) {
  int hoursLeft = (MaturityMinutesWhenDone - NumberOfMaturityMinutesElapsed) / temperature / 60;
  if(hoursLeft < 10)
    lcd.setCursor(15, 1);
  else if(hoursLeft < 100)
    lcd.setCursor(14, 1);
  else
    lcd.setCursor(13, 1);
    
  lcd.print(hoursLeft);
}

void updateTemperature(float temperature){
  lcd.setCursor(0, 0);
  lcd.print(temperature, 1);
  lcd.print("C");
}

void updateMaturityMinutes(float temperature, DateTime now) {
  if(now.minute() != LastMinuteUpdate.minute()) {
    NumberOfMaturityMinutesElapsed += temperature;
    updateMaturityOnLcd(now);
  }
}

void updateMaturityOnLcd(DateTime currentTime) {
  lcd.setCursor(0, 1);
  lcd.print((NumberOfMaturityMinutesElapsed/60.0)/24.0, 1);
  lcd.print("D");
  LastMinuteUpdate = currentTime;
}

void updatePercentDone(){
  float percentageDone = (NumberOfMaturityMinutesElapsed/MaturityMinutesWhenDone) * 100;
  lcd.setCursor(6, 1);
  lcd.print(percentageDone, 1);
  lcd.print("%");
}

void readButtonState() {
  // read the state of the pushbutton value:
  int buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    StartTime = LastMinuteUpdate = LastHourUpdate = RTC.now();
    HoursPassed = -1;
    NumberOfMaturityMinutesElapsed = 0;
    updateTimePassed(StartTime);
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

