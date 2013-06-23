/*
Iridium9612.cpp - Library for transmitting data
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


#ifndef Iridium_cpp
#define Iridium_cpp

#include "Iridium9612.h"

// Iridium epoch
#define IRIDIUM_EPOCH 1173325821

void Iridium::begin(HardwareSerial *theSerial)
{
  _serial = theSerial;
  // open serial communications
  _serial->begin(IRIDIUM_DATA_RATE);
  // set timeout
  setTimeout(30);
  // turn echo off
  _cmd("ATE0");
  waitForString("OK");
  // ensure verbose mode
  _cmd("ATV1");
  waitForString("OK");
  // initialize clock
  setTime(IRIDIUM_EPOCH);
  // debug is off by default
  _dbg=0;
}

boolean Iridium::loadBuffer(uint8_t * b,uint8_t len)
{
  // send the AT command
  // AT+SBDWB=<length><CRLF>
  _send("AT+SBDWB=");
  _serial->println(len,DEC);
  // wait for the READY response
  waitForString("READY");
  
  checksum sum;
  sum.sum = 0;
  //5+5
  for(int i=0;i<len;i++){
    _serial->write(b[i]);
    sum.sum+=b[i];
  }
  // the iridium is backwards to the Arduino
  // need to send in reverse order
  // google "endianness" if you're interested
  // and also read Gulliver's Travels
  _serial->write(sum.b[1]);
  _serial->write(sum.b[0]);
}

int Iridium::conductSBDSession(){
  // send the AT command
  // AT+SBDIX<CRLF>
  _serial->println("AT+SBDIX");
  waitForString("SBDIX: ");
  
  return _readInt();
}

int Iridium::send(){
  return conductSBDSession();
}

int Iridium::getSignalStrength(){
  _cmd("AT+CSQ");
  waitForString("CSQ:");
  return _readInt();
}

void Iridium::setDebugLevel(int i){
  _dbg = i;
}

void Iridium::setTimeout(int t)
{
  _timeout = t;
//  _serial->setTimeout(t);
}

void Iridium::setRadio(boolean b){
  if(b) _cmd("AT*R1");
  else  _cmd("AT*R0");
 // _flushSerial();
}

void Iridium::waitForString(String s)
{
  tm_start = now();
  int matches = 0;
  int bytes=100;
  while(matches<s.length() && bytes>0)
  {
    if((tm_start+_timeout)<now()){
      debug("Timed out waiting for \""+s+"\"");
      return;
    }
    if(_serial->available()){
      char c = _serial->read();
      if(_dbg>1) dbgSerial->write(c);
      if(c==s.charAt(matches)){
        matches++;
      }else matches = 0;
      bytes--;
    }
  }
}

int Iridium::_readInt()
{
  tm_start = now();
  int ret = 0;
  while(true)
  {
    if((tm_start+_timeout)<now()) return ret;
    if(!_serial->available()) continue;
    char c = _serial->read();
    if(c<'0'||c>'9') return ret;
    ret*=10;
    ret+=(int)(c-'0');
  }
  return ret;
}

unsigned long Iridium::_readHex()
{
  tm_start = now();
  unsigned long ret = 0;
  int q=1;
  while(true)
  {
    if((tm_start+_timeout)<now()) return ret;
    if(!_serial->available()) continue;
    char c = _serial->read();
    int i=0;
    if(c>='0'&&c<='9') i=(int)(c-'0');
    else if(c>='a'&&c<='f') i=(int)(c-'a')+10;
    else if(c>='A'&&c<='F') i=(int)(c-'A')+10;
    else break;
    ret=ret<<4;
    ret+=i;
  }
  return ret;
}

void Iridium::enableDebug(HardwareSerial * s)
{
  dbgSerial = s;
  _dbg = 2;
}

void Iridium::disableDebug()
{
  _dbg = 0;
}

unsigned long Iridium::getTime()
{
  unsigned long irTime = _rawTime();
  unsigned long unixTime = (unsigned long)(9*(irTime/100) + IRIDIUM_EPOCH);
  if(irTime>0){
    setTime(unixTime);
    //debug(hour(unixTime)*10000+minute(unixTime)*100+second(unixTime));
  }
  else debug("Could not update time from network");
  _flushSerial();
  return now();
}

void Iridium::debug(String s){
  debug(s,1);
}

void Iridium::debug(String s, int i){
  if(_dbg>=i) dbgSerial->println(s);
}

unsigned long Iridium::_rawTime()
{
  _cmd("AT-MSSTM");
  waitForString("MSSTM: ");
  return _readHex();
}

void Iridium::attention()
{
  _cmd("AT");
  waitForString("OK");
}

void Iridium::_flushSerial()
{
  while(_serial->available()) _serial->read();
}

void Iridium::_cmd(String s)
{
  debug(s,2);
  _serial->println(s);
}

void Iridium::_send(String s)
{
  debug(s,2);
  _serial->print(s);
}

#endif
