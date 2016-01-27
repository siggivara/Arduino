#include "Lib/NewPing/NewPing.h"
#include "Lib/TonePlayer/TonePlayer.h"

#define piezoPin 9
#define NOTE_F3  175

#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define MAX_DISTANCE 100

TonePlayer tone1 (TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno)
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);


void setup() {
  Serial.begin(115200);
  pinMode (piezoPin, OUTPUT);  // D9 on Uno, D11 on Mega 2560
}

void loop() {
  int distance = getDistance();
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


long getDistance() {
  delay(50);
  int uS = sonar.ping();
  Serial.print("Ping: ");
  Serial.print(uS / US_ROUNDTRIP_CM);
  Serial.println("cm");
  return uS / US_ROUNDTRIP_CM;
}
