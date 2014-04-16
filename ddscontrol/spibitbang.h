#ifndef __SPIBITBANG_H__
#define __SPIBITBANG_H__

#include "Arduino.h"

class SPIBitBang {
  /*  
    experimental bit-banged SPI
    AW 2014
    
    native SPI api is at http://arduino.cc/en/Reference/SPI
 
    pinSDIO is a combined MOSI/MISO data pin
    pinCSB  is the chip select pin
    pinSCLK is the serial clock pin

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
    // the two-byte write is called "instruction", because it tells the AD9912 what to do
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
    // used for reading the 6-byte FTW of the AD9912 DDS
    // writes results to *data, supplied by the user, which must have space for the 6 bytes.
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
    // this is used for setting the FTW (frequency tuning word) on the AD9912 DDS
    // the FTW is 48 bits, or 6 bytes. We assume the *data pointer contains these bytes
    // there is no error checking, so bad things will happen if *data is not the right type
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

