# Iridium9612
A simple library for transmitting satellite data

## Purpose
This software library allows you to use an Arduino to send arbitrary data
over a satellite connection. Imagine monitoring temperature at some very
remote point in the woods, or taking wind readings from a buoy in the middle
of the ocean. All this and more is possible using the Iridium9612 library.

## Usage
```
// create the satellite object
Iridium iridium;

// set the serial port to talk over
// has to be hardware serial, no SoftSerial
iridium.begin(&Serial1);

// turn the radio on
iridium.setRadio(true);

// read the current network time
unsigned long time = iridium.getTime();

// create our data packet
struct Transmission {
  unsigned long timestamp;
  float temperature;
};

Transmission tx;

// load data into the packet
tx.timestamp = time;
tx.temperature = getTemperature(); // <-- assuming you've written this function

// load the packet into the transmitter...
// details() is a macro from the EasyTransfer library that
// does some magic behind the scenes
iridium.loadBuffer(details(tx));

// send the transmission
int status = iridium.send();

// check for success
if(status<4) Serial.println("Message sent.");
else Serial.println("Error sending message.");
```
