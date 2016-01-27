
/*

Play tones with hardware timers
Author: Nick Gammon
Date:   1 April 2013

  */

#include <Arduino.h>

class TonePlayer
 {
 // addresses of output ports - NULL if not applicable
 volatile byte * const timerRegA_;
 volatile byte * const timerRegB_;
 volatile byte * const timerOCRH_;
 volatile byte * const timerOCRL_;
 volatile byte * const timerTCNTH_;
 volatile byte * const timerTCNTL_;
 
 public:
   // constructor
   TonePlayer (
         // ports
         volatile byte & timerRegA, 
         volatile byte & timerRegB, 
         volatile byte & timerOCRH,
         volatile byte & timerOCRL, 
         volatile byte & timerTCNTH, 
         volatile byte & timerTCNTL)
      : 
        timerRegA_  (&timerRegA), 
        timerRegB_  (&timerRegB),
        timerOCRH_  (&timerOCRH), 
        timerOCRL_  (&timerOCRL), 
        timerTCNTH_ (&timerTCNTH), 
        timerTCNTL_ (&timerTCNTH)
 { }
   
   void tone (const unsigned int Hz);
   void noTone ();
   
 };  // end of TonePlayer
