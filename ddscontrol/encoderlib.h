#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "Arduino.h"

// modified version of http://playground.arduino.cc/Main/RotaryEncoders

class Encoder {
  /*  

    Rotary quadrature encoder.

    // ------------------------------------------------------------------------------------------------
    // Example usage :
    // ------------------------------------------------------------------------------------------------
        #include "Encoder.h"

        Encoder encoder(2, 4);

        void setup() { 
            attachInterrupt(0, doEncoder, CHANGE); 
            Serial.begin (115200);
            Serial.println("start");
        } 

        void loop(){
            // do some stuff here - the joy of interrupts is that they take care of themselves
        }

        void doEncoder(){
            encoder.update();
            Serial.println( encoder.getPosition() );
        }    
    // ------------------------------------------------------------------------------------------------
    // Example usage end
    // ------------------------------------------------------------------------------------------------
  */
public:

    // constructor : sets pins as inputs and turns on pullup resistors
    Encoder( int8_t PinA, int8_t PinB) : pin_a ( PinA), pin_b( PinB ) {
        // set pin a and b to be input 
        pinMode(pin_a, INPUT); 
        pinMode(pin_b, INPUT); 
        position = 0;
        // optionally turn on pullup resistors
        //digitalWrite(pin_a, HIGH);    
        //digitalWrite(pin_b, HIGH);                 
    };

    // call this from your interrupt function

    void update () {
        if (digitalRead(pin_a)) 
          if (digitalRead(pin_b)) {
              position++; 
              delta=1;
           } else {
             position--;
             delta = -1;
           }
        else if (digitalRead(pin_b)) {
            position--; 
            delta = -1;
          } else {
            position++;
            delta = +1;
          }
    };

    // returns current position
    long int getPosition () { return position; };
    int getDelta () { return delta; };
    // set the position value
    void setPosition ( const long int p) { position = p; };

private:
    long int position;
    int delta;
    int8_t pin_a;
    int8_t pin_b;
};

#endif // __ENCODER_H__
