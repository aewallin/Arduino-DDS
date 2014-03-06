/*
    AD9912-based Direct Digital Synthesizer
    https://github.com/aewallin/Arduino-DDS
    
    Anders Wallin, 2014-02-06

 */

// include the library code:
#include <LiquidCrystal.h> //  http://www.arduino.cc/en/Tutorial/LiquidCrystal
#include "encoderlib.h" 
#include "ddslib.h"
#include "printf_wrapper.h"

// LCD pin definitions
const int lcd_RS  = 22; 
const int lcd_ENA = 23;
const int lcd_D4 = 24;
const int lcd_D5 = 25;
const int lcd_D6 = 26;
const int lcd_D7 = 27;
LiquidCrystal lcd(lcd_RS, lcd_ENA, lcd_D4, lcd_D5, lcd_D6, lcd_D7);

// Encoder pin definitions
const int buttonPin = 29;     
const int encBPin = 33;
const int encAPin = 31;
Encoder encoder(encAPin, encBPin);

// AD9912 SPI pin definitions
const int SPI_SDIO = 53;
const int SPI_CSB  = 51;
const int SPI_SCLK = 49;
AD9912::DDS dds(SPI_SDIO, SPI_CSB, SPI_SCLK);

// globals that hold program state
long int enc;          // encoder position
bool but;              // button state
int8_t ftw[6];         // 6-byte (48-bit) frequency tuning word
unsigned int ftwd[15]; // 15-digit number displayed on LCD

// program state
#define STATE_ADJ 0  // adjust a digit
#define STATE_MOV 1  // move between digits
char state; // state of program, either ADJ or MOV 
int pos;    // position of cursor on screen, from 0 to 15.

void setup() {
  Serial.begin (9600); 
  delay(1000);
  Serial.println("start");
  
  // initial state and position
  state = STATE_MOV;
  pos = 0;
  
  pinMode(buttonPin, INPUT);
  attachInterrupt(buttonPin, doButton, RISING); 
  attachInterrupt(encAPin, doEncoder, RISING);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(20, 2);
  //dds.reset();
  //dds.update_registers();
  dds.set_pd( AD9912::PD_HSTL | AD9912::PLL_PD); // no PLL!
  
  delay(10);

  dds.set_PLL_parameters(AD9912::VCO_AUTORANGE); // not required?
  
  delay(10);
  //dds.set_PLL_N( 0x1F ); // 2x( N +2 ) = 66x
  //delay(10);
  //serial_printf( "PLL = %d \n",dds.get_PLL_N() );
  dds.update_registers();
  //
  
  // Print a message to the LCD.
  encoder.setPosition(0);
  enc = encoder.getPosition();
  updateLCD();
  Serial.println("setup() done");
  
  // reset frequency to zero
  for (int n=0;n<15;n++) {
      ftwd[n]=0;
  }

}

void loop() {
  // wait for interrupts.
  // all functionality is in the intterrupt-handlers
}

// print frequency to terminal in decimal form
// only for debug
void serial_print_frequency() {
    char fstr[128]; 
    sprintf(fstr, "FTW %01u%01u%01u %01u%01u%01u %01u%01u%01u . %01u%01u%01u %01u%01u%01u\n",
    ftwd[14],ftwd[13],ftwd[12],
    ftwd[11],ftwd[10],ftwd[9],
    ftwd[8],ftwd[7],ftwd[6],
    ftwd[5],ftwd[4],ftwd[3],
    ftwd[2],ftwd[1],ftwd[0]);
    Serial.print( fstr );
  }

// serial output in hex format
// only for debug
void serial_print_frequency2() {
  serial_printf("ftw= %02x %02x %02x %02x %02x %02x\n",(char)ftw[5],(char)ftw[4],(char)ftw[3],(char)ftw[2],(char)ftw[1],(char)ftw[0] );
}

// convert binary 48-bit frequency tuning word to decimal representation
// for displaying the frequency in decimal on the LCD
//
void frequency_to_decimal() {
  const int Nd = 18; // number of decimal digits to compute
  const int Nb = 8; // number of bytes
  
  unsigned int ftwd2[Nd]; // new decimal digits computed here
    for (int n=0;n<Nd;n++) {
      ftwd2[n]=0; // zero the result
    }

    for (int m = 0; m<Nd; m++) { // loop through decimal digit numbers
      for (int n =0 ; n<6; n++) { // loop through the binary ftw
        ftwd2[m] += (char)(ftw[n]) * AD9912::FTW660[n][m]; // m:th digit of decimal number to be computed
      }
    }


    
    // now each digit might be larger than 9, so do carry
    for (int m = 0; m<(Nd-1); m++) { // there should be no carry on the last digit!
        if (ftwd2[m] > 9) {
          unsigned int carry = (unsigned int)( ftwd2[m] / 10 );
          unsigned int modulus = (ftwd2[m] % 10);
          ftwd2[m+1] += carry; // the carry
          ftwd2[m] = modulus;  // single digit remainder
        }
    }
    
    // now we round to 15 digits, displayed on screen
    for (int m = 2; m<(Nd-1); m++) { // there should be no carry on the last digit!
      if (m==3) { //digit that determines rounding
          if (ftwd2[m] >= 5)
              ftwd2[m+1] +=1;
          ftwd2[m] = 0; // zero the rounding-digit
      } else {
        if (ftwd2[m] > 9) {
          unsigned int carry = (unsigned int)( ftwd2[m] / 10 );
          unsigned int modulus = (ftwd2[m] % 10);
          ftwd2[m+1] += carry; // the carry
          ftwd2[m] = modulus;  // single digit remainder
        }
      }
    }
    
    // we are done, so assign to the global variable.
    for (int m=0; m<15;m++) {
      ftwd[m]=ftwd2[m+3];
    }
    // when updateLCD() is next called, it will display the correct frequency 
}

// subtract 10^(8-d) from the current frequency
// see add_digit() for more documentation
void sub_digit(char d) { 
  const unsigned int Nbytes = 14; // precision for binary computation
  int fnew[Nbytes];
  for (int n=0;n<Nbytes;n++) {
    fnew[n]=0;
    if (n>7)
      fnew[n]+=(char)ftw[n-8]; // initialize to current ftw
  }
  //serial_printf("sub fnew 1= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

  // now do addition
  for (int n=0;n<Nbytes;n++) {
    fnew[n] -= AD9912::F660[d][n];
  }
  //serial_printf("sub fnew 2= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

  //serial_printf("fnew 1= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

  // now do carry, since fnew[n] might be <0
  for (int n = 0; n<(Nbytes-1); n++) { // there should be no carry on the 6:th digit!
      if (fnew[n] < 0) {
        int carry = -1+ (int)( fnew[n] / 0x100 );
        int modulus = ( (fnew[n]-carry*0x100) % 0x100);
        //serial_printf("%03d %03d %03d\n", fnew[n], carry, modulus);
        fnew[n+1] += carry; // the carry
        fnew[n] = modulus; // single digit remains
      }
  }
  //serial_printf("sub fnew 3= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

  //serial_printf("fnew 1= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

    // now we round to 6 bytes, written to DDS
    for (int m = 2; m<(Nbytes-1); m++) { // there should be no carry on the last digit!
      if (m==7) { // 
          if (fnew[m] >= 0x80)
              fnew[m+1] +=1;
      } else {
        if (fnew[m] > 0xFF) {
          unsigned int carry = (unsigned int)( fnew[m] / 0x100 );
          unsigned int modulus = (fnew[m] % 0x100);
          fnew[m+1] += carry; // the carry
          fnew[m] = modulus; // single digit remains
        }
      }
    }
    
  // ready, assign to ftw
    for (int n=0;n<6;n++) {
      ftw[n] = fnew[n+8]; 
    }
   //serial_print_frequency2();
}

// increase the frequency by 10^(8-d) Hz
// d is the position of the cursor on the screen
// when d=0 this adds 100 MHz to the freqyency
// when d=1      add   10 MHz
// ..
// when d=13     add   0.000 01 Hz 
void add_digit(char d) { // d in [0, 14]
  const unsigned int Nbytes = 14; // precision for binary computation
  unsigned int fnew[Nbytes];      // new frequency word calculated here
  for (int n=0;n<Nbytes;n++) {
    fnew[n]=0; // reset the extra low bytes
    if (n>7)
      fnew[n]+=(char)ftw[n-8]; // initialize the higher bytes to the current ftw
  }
  //serial_printf("fnew 1= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

  // now do addition
  for (int n=0;n<Nbytes;n++) {
    fnew[n] += AD9912::F660[d][n]; // ad9912.h has the tuning-word table
  }
  //serial_printf("fnew 1= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

  // now do carry, since fnew[n] might be > 0xFF
    for (int n = 0; n<(Nbytes-1); n++) { // there should be no carry on the 6:th digit!
        if (fnew[n] > 0xFF) {
          unsigned int carry = (unsigned int)( fnew[n] / 0x100 );
          unsigned int modulus = (fnew[n] % 0x100);
          fnew[n+1] += carry; // the carry
          fnew[n] = modulus; // single digit remains
        }
    }
  //serial_printf("fnew 1= %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d\n",fnew[13],fnew[12],fnew[11],fnew[10],fnew[9],fnew[8],fnew[7],fnew[6],fnew[5],fnew[4],fnew[3],fnew[2],fnew[1],fnew[0] );

    // now we round to 6 bytes, written to DDS
    for (int m = 2; m<(Nbytes-1); m++) { // there should be no carry on the last digit!
      if (m==7) { // 
          if (fnew[m] >= 0x80)
              fnew[m+1] +=1;
      } else {
        if (fnew[m] > 0xFF) {
          unsigned int carry = (unsigned int)( fnew[m] / 0x100 );
          unsigned int modulus = (fnew[m] % 0x100);
          fnew[m+1] += carry; // the carry
          fnew[m] = modulus; // single digit remains
        }
      }
    }
    
  // ready, assign to ftw
    for (int n=0;n<6;n++) {
      ftw[n] = fnew[n+8]; 
    }
   //serial_print_frequency2();
}

// update the LCD screen
void updateLCD() {
  lcd.clear();
  
  dds.get_frequency(ftw);
  
  char fstr[40]; // frequency on first row of LCD
  frequency_to_decimal();
  //serial_print_frequency();
  //serial_printf( "PLL = %d \n",dds.get_PLL_N() );
  
  // first row.
  // 15-digit number, with spaces and decimal point, e.g:
  // "123 456 789.012 345"  19 characters in total
  sprintf(fstr, "%01d%01d%01d %01d%01d%01d %01d%01d%01d.%01d%01d%01d %01d%01d%01d",
    (char)ftwd[14],(char)ftwd[13],(char)ftwd[12],
    (char)ftwd[11],(char)ftwd[10],(char)ftwd[9],
    (char)ftwd[8],(char)ftwd[7],(char)ftwd[6],
    (char)ftwd[5],(char)ftwd[4],(char)ftwd[3],
    (char)ftwd[2],(char)ftwd[1],(char)ftwd[0]);
  lcd.print(fstr);
  
  // second row
  // adjust cur, so that we are not under a space or the decimal point
  int cur=pos;
  if (pos>2 && pos<6)
    cur= pos+1;
  else if (pos>5 && pos<9)
    cur=pos+2;
  else if (pos>8 && pos<12)
    cur=pos+3;
  else if (pos>11)
    cur=pos+4;
  // indicate either MOV or ADJ mode
  lcd.setCursor(cur, 1); 
  if (state == STATE_MOV)
    lcd.print("^");
  else if (state == STATE_ADJ)
    lcd.print("#");

}


// interrupt handler for button-press
void doButton(){
    but = digitalRead( buttonPin );
    
    // flip state
    if (state == STATE_ADJ )
      state = STATE_MOV;
    else if (state == STATE_MOV)
      state = STATE_ADJ;

    updateLCD();
}

// move cursor position
void pos_move(int delta) {
  if (delta==1)
    pos++;
  else if (delta==-1)
    pos--;
    
  if (pos<0) // minimum position is 0
    pos=0;
  else if (pos>13) // maximum position is 13
    pos=13;
}

// interrupt handler for encoder
void doEncoder() {
    encoder.update();
    enc = encoder.getPosition();
    int delta = encoder.getDelta(); // either +1 or -1, depending on cw or ccw turn of encoder
    
    //serial_printf("delta = %d \n", delta);
    if (state == STATE_MOV) {
      pos_move(delta); // in the MOV state we move the cursor
    } else if (state == STATE_ADJ) {
        // in the ADJ state we adjust the frequency
        if (delta==1)
          add_digit(pos);
        else if (delta==-1)
          sub_digit(pos);
        
        // after adjust, write frequency to AD9912
        dds.set_frequency(ftw);
    }
    
    updateLCD();
}
