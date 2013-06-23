/*
Iridium9612.h - Library for transmitting data
using the Quake 9612 satellite transciever.
Might also work with Iridium 960x models.
Created by Ryan A. Muller, May 23, 2012.
  
This file is part of the Iridium9612 library.

The Iridium9612 library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The Iridium9612 library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with the Iridium9612 library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef details
#define details(name) (byte*)&name, sizeof(name)
#endif

#ifndef Iridium_h
#define Iridium_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "HardwareSerial.h"
#include "IridTime.h"

// baud rate for interactions with the Iridium
#define IRIDIUM_DATA_RATE 19200

union checksum
{
  byte b[2];
  unsigned int sum;
};

class Iridium
{
  public:
  
    // start communications with the Iridium
    void begin(HardwareSerial *theSerial);
    
    // load binary data into the unit
    boolean loadBuffer(uint8_t *,uint8_t);
    
    // send any data that's in the buffer and return
    // the raw response code
    int conductSBDSession();
    
    // send data and return true on success
    int send();
    
    // set the maximum time to wait before timeout
    void setTimeout(int);
    
    // turn the radio on or off
    void setRadio(boolean);
    
    // send the AT command
    void attention();
    
    // get the current time in Unix time
    unsigned long getTime();
    
    // turn on debugging
    void enableDebug(HardwareSerial *);
    
    // turn off debugging
    void disableDebug();
    
    // print a string if debug is on
    void debug(String);
    
    // print a string if debug is above a given level
    void debug(String,int);
    
    // set the debug level
    void setDebugLevel(int);
    
    // signal strength
    int getSignalStrength();

  private:
    
    // serial port used to communicate
    HardwareSerial *_serial;
    
    // max timeout before a function fails, seconds
    int _timeout;
    
    // function start
    unsigned long tm_start;
    
    // has this device been shut down?
    boolean _shutdown;
    
    // wait for a string from the unit
    void waitForString(String s);
    
    // read a base 10 integer from ASCII input
    int _readInt();
    
    // flush the serial buffer
    void _flushSerial();
    
    // read a base 16 integer from ASCII input
    unsigned long _readHex();
    
    // send a command followed by crlf
    void _cmd(String);
    
    // send any string with no crlf
    void _send(String);
    
    // get raw Iridium time
    unsigned long _rawTime();
    
    // debug level
    // 0: no debugging
    // 1: debug messages
    // 2: echo Iridium responses
    int _dbg;
    HardwareSerial *dbgSerial;
};

#endif
