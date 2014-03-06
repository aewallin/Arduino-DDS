#ifndef __SPIBITBANG_H__
#define __SPIBITBANG_H__

#include "Arduino.h"

class SPIBitBang {
  /*  
    experimental bit-banged SPI
    try to follow SPI api http://arduino.cc/en/Reference/SPI
    * 
    * read/write bytes over SPI.
  */
public:
    SPIBitBang( int8_t pinSDIO, int8_t pinCSB, int8_t pinSCLK) : SDIO (pinSDIO), CSB(pinCSB), SCLK (pinSCLK) {
        pinMode(SDIO, OUTPUT); 
        pinMode(CSB, OUTPUT);
        digitalWrite( CSB, HIGH); // deactivate
        pinMode(SCLK, OUTPUT); 
        digitalWrite( SCLK, LOW);
    };
    // write one byte
    void write8(int8_t data) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          shiftOut( SDIO, SCLK, MSBFIRST, data);
          digitalWrite( CSB, HIGH);
    };
    // write two bytes
    void write16(int data) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          shiftOut( SDIO, SCLK, MSBFIRST, (data >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, data); // low byte
          digitalWrite( CSB, HIGH);
    };
    // write three bytes:
    // two-byte instruction
    // one-byte data
    void write24(int instruction, int8_t data) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          shiftOut( SDIO, SCLK, MSBFIRST, (instruction >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, instruction); // low byte
          shiftOut( SDIO, SCLK, MSBFIRST, data);
          digitalWrite( CSB, HIGH);
    };
    // write four bytes
    void write32(int data1, int data2) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          
          shiftOut( SDIO, SCLK, MSBFIRST, (data1 >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, data1); // low byte
          shiftOut( SDIO, SCLK, MSBFIRST, (data2 >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, data2); // low byte
          
          digitalWrite( CSB, HIGH);
    };
    /*
    int read16() {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          
          pinMode(SDIO, INPUT);          
          int8_t msb = shiftIn( SDIO, SCLK, MSBFIRST);
          int8_t lsb = shiftIn( SDIO, SCLK, MSBFIRST);
          pinMode(SDIO, OUTPUT);          

          digitalWrite( CSB, HIGH);
          return word(msb,lsb);
    };*/
    // write two bytes
    // then read one byte
    void write16read16(int instruction, int &data) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          shiftOut( SDIO, SCLK, MSBFIRST, (instruction >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, instruction); // low byte
          
          pinMode(SDIO, INPUT);          
          int8_t msb = shiftIn( SDIO, SCLK, MSBFIRST);
          int8_t lsb = shiftIn( SDIO, SCLK, MSBFIRST);
          
          digitalWrite( CSB, HIGH);
          pinMode(SDIO, OUTPUT);  
          data = word(msb,lsb);   
    };
    void write16read8(int instruction, int &data) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          shiftOut( SDIO, SCLK, MSBFIRST, (instruction >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, instruction); // low byte
          
          pinMode(SDIO, INPUT);          
          int8_t msb = shiftIn( SDIO, SCLK, MSBFIRST);
         
          digitalWrite( CSB, HIGH);
          pinMode(SDIO, OUTPUT);  
          data = msb;   
    };
    
    void write16read48(int instruction, int8_t* data) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          shiftOut( SDIO, SCLK, MSBFIRST, (instruction >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, instruction); // low byte
          
          pinMode(SDIO, INPUT);          
          data[5] = shiftIn( SDIO, SCLK, MSBFIRST);
          data[4] = shiftIn( SDIO, SCLK, MSBFIRST);
          data[3] = shiftIn( SDIO, SCLK, MSBFIRST);
          data[2] = shiftIn( SDIO, SCLK, MSBFIRST);
          data[1] = shiftIn( SDIO, SCLK, MSBFIRST);
          data[0] = shiftIn( SDIO, SCLK, MSBFIRST);
          
          digitalWrite( CSB, HIGH);
          pinMode(SDIO, OUTPUT);  
    };
    void write16write48(int instruction, int8_t* data) {
          digitalWrite( SCLK, LOW); // ensure clock pin LOW when we start
          digitalWrite( CSB, LOW);
          
          shiftOut( SDIO, SCLK, MSBFIRST, (instruction >> 8)); // high byte
          shiftOut( SDIO, SCLK, MSBFIRST, instruction); // low byte          
          shiftOut( SDIO, SCLK, MSBFIRST, data[5] );
          shiftOut( SDIO, SCLK, MSBFIRST, data[4] );
          shiftOut( SDIO, SCLK, MSBFIRST, data[3] );
          shiftOut( SDIO, SCLK, MSBFIRST, data[2] );
          shiftOut( SDIO, SCLK, MSBFIRST, data[1] );
          shiftOut( SDIO, SCLK, MSBFIRST, data[0] );
          
          digitalWrite( CSB, HIGH);
    };
private:
    int8_t SDIO; // serial data in/out pin. OUTPUT or INPUT
    int8_t CSB;  // chip-select pink. OUTPUT
    int8_t SCLK; // serial clock pin. OUTPUT
};

#endif 

