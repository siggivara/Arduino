#include "TonePlayer.h"

TonePlayer tone1 (TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno)

void setup() {
  // put your setup code here, to run once:
  pinMode (9, OUTPUT);  // D9 on Uno, D11 on Mega 2560
}

void loop() {  
  tone1.tone (440);     // play 440 Hz
  delay (500);          // wait half a second
  tone1.noTone ();      // stop playing
}
