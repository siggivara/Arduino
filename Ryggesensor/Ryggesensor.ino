#include "Lib/NewPing/NewPing.h"
#include "Lib/TonePlayer/TonePlayer.h"

enum DistanceSensor {
  A,
  B
};

#define piezoPin 9
#define NOTE_F3  175

#define TRIGGER_PIN_A  12
#define ECHO_PIN_A     11
#define TRIGGER_PIN_B  7
#define ECHO_PIN_B     6
#define MAX_DISTANCE 100

TonePlayer tone1 (TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno)

NewPing sonarA(TRIGGER_PIN_A, ECHO_PIN_A, MAX_DISTANCE);
NewPing sonarB(TRIGGER_PIN_B, ECHO_PIN_B, MAX_DISTANCE);


void setup() {
  Serial.begin(115200);
  pinMode (piezoPin, OUTPUT);  // D9 on Uno, D11 on Mega 2560
}

void loop() {
  int distance = getShortestDistance();
  int noteDuration = distance * 10;
  playNote(piezoPin, NOTE_F3, noteDuration);
  delay(100);
}


void playNote(int buzzerPin, int note, int duration) {
  Serial.print("Duration: ");
  Serial.println(duration);
  tone1.tone (NOTE_F3);     // play 440 Hz
  delay(duration);          // wait half a second
  tone1.noTone ();      // stop playing
}


long getShortestDistance() {
  delay(50);
  int uS_A = sonarA.ping();
  delay(50);
  int uS_B = sonarB.ping();
  //long uS_A = 10000000;

  int uS = 0;
  
  if(uS_A < uS_B) {
    Serial.println("A is shorter");
    uS = uS_A;
  }
  else {
    Serial.println("B is shorter");
    uS = uS_B;
  }
  
  Serial.print("Ping: ");
  Serial.print(uS / US_ROUNDTRIP_CM);
  Serial.println("cm");
  return uS / US_ROUNDTRIP_CM;
}
