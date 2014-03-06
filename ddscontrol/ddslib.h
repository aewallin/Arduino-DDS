#ifndef __DDSLIB_H__
#define __DDSLIB_H__

#include "Arduino.h"
#include "spibitbang.h"
#include "ad9912.h"

namespace AD9912 {

class DDS {
  /*  
    experimental DDS control
    AW 2014-01-02
  */
public:
      DDS( int8_t SDIO, int8_t CSB, int8_t SCLK)  {
        spi = new SPIBitBang(SDIO, CSB, SCLK);
      };
      ~DDS() {
        delete spi;
      }
      
      // read the part ID
      // should return 0x1902 = 6402 (decimal)
      int read_id() {
        int instr = instruction( READ, BYTES_TWO, PART_ID);
        int id;
        spi->write16read16(instr, id);
        return id;
      }
      void reset() {
            int instr = instruction( WRITE, BYTES_ONE, RESET);
            spi->write24( instr, 255 );
        }
      void update_registers() {
        int instr = instruction( WRITE, BYTES_ONE, SERIAL_OPTS_1);
        spi->write24( instr, 255 );
      }
      void register_read_mode(int8_t mode) {
        int instr = instruction( WRITE, BYTES_ONE, SERIAL_OPTS_0);
        spi->write24( instr, mode );
      }
      
      // fixed divide-by-2 and 2-offset:
      // set to zero, corresponds to N=4;
      void set_PLL_N(int8_t n) {
        int instr = instruction( WRITE, BYTES_ONE, PLL_N);
        spi->write24( instr, n );
      }
      int get_PLL_N() {
        int instr = instruction( READ, BYTES_ONE, PLL_N);
        int n;
        spi->write16read8( instr, n );
        return n;
      }
      void set_PLL_parameters(int8_t par) {
        int instr = instruction( WRITE, BYTES_ONE,PLL_PARAMETERS);
        spi->write24( instr, par );
      }
        
      void set_pd(int8_t mode) {
        int instr = instruction( WRITE, BYTES_ONE, PD_ENA);
        spi->write24( instr, mode );
      }
      
      void set_frequency(int8_t* f) {
        ftw[0]=f[0];
        ftw[1]=f[1];
        ftw[2]=f[2];
        ftw[3]=f[3];
        ftw[4]=f[4];
        ftw[5]=f[5];
        write_frequency();
      }
      void write_frequency() {
        int instr = instruction( WRITE, BYTES_STREAM, FTW5);
        spi->write16write48(instr, ftw);
        //delay(1);
        update_registers();
      }
      // read frequency tuning word
      // initial state is set by S1-S4 pins on the AD9912
      // s1 s2 s3 s4 ftw[5] ftw[4]  Fout, assuming 1GHz sysclock
      // 1  1  1  1      27   d0   155.51758 MHz
      // 0  1  1  1      1f   75   122.87903
      // 1  0  1  1      17   97    92.14783
      // 0  0  1  1      13   e8    77.75879
      // 1  1  0  1      0f   ba    61.43188
      // 0  1  0  1      0d   45    51.83411
      // 1  0  0  1      09   f4    38.87939
      // 1  0  0  0      00   00     0
      int read_frequency() {
        int instr = instruction( READ, BYTES_STREAM, FTW5);
        spi->write16read48(instr, ftw);
      }
      void get_frequency(int8_t* f) {
        read_frequency();
        f[0]=ftw[0];
        f[1]=ftw[1];
        f[2]=ftw[2];
        f[3]=ftw[3];
        f[4]=ftw[4];
        f[5]=ftw[5];
      }
      int instruction(int readwrite, int bytes, int address) {
        return readwrite | bytes | address;
      }
      
private:
    SPIBitBang* spi;
    int8_t ftw[6];
};

} // end namespace
#endif 
