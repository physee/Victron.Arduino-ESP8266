# Victron.Arduino-ESP8266
Code to read the VE.Direct-Protocol from serial into a value array. Uses a non-blocking read loop and does checksum verification before adding the data. Extra care has been taken to not used `readByteUntil()` or any other blocking serial command that can mess with background services, especially on the ESP8266.

Extend this code with MQTT or any other protocol or service to send and/or extract the data.

## Config
At the moment only the MPPT 75/10 is configured in the `config.h`.

## Usage
Make sure the RX and TX of the VE.Direct-Protocol are connected to the corresponding pins in the setup, `victronSerial`. On the NodeMCU, pins D7/D8 are used.

Every second the MPPT sends out data, this is put into the `value` array. As per code the `PrintValues()` function loops of the array and prints the values and keys. 

The values can also be used with the macros defined, eg. `value[VPV]` or `value[ERR]`.

Note that the values are stored as chars, so convert to suitable types with functions like: `atof()` or `atoi()` etc. 