Arduino-DDS
===========

Direct Digital Syntehsizer based on AD9912 DDS, ADF4350 PLL, and Arduino Due.

Hardware
--------

* Arduino Due with Ethernet shield
* AD9912 DDS evaluation kit
* ADF4350 PLL evaluation kit
* 20x2 character LCD
* Rotary encoder with pushbutton, for user interaction

Software
--------
*ddscontrol/ has the Arduino Due code. 
  * ddscontrol.ino   Main program 
  * ad9912.h         AD9912 DDS registers and register-values
  * ddslib.h         AD9912 specific funtions
  * spibitbang.h     SPI control over Arduino GPIO pins
  * encoderlib.h     Rotary encoder interface
  * printf_wrapper.h serial output
  
* fcalc.py is a python script for generating frequency-word tables used in ad9912.h
