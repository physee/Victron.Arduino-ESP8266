/*
    Victron.Arduino-ESP8266
    A:Pim Rutgers
    E:pim@physee.eu

    Code to grab data from the VE.Direct-Protocol on Arduino / ESP8266.
    Tested on NodeMCU v1.0

    The fields of the serial commands are configured in "config.h"

*/
#include <SoftwareSerial.h>
#include "config.h"

// Serial variables
#define rxPin D7
#define txPin D8                                    // TX Not used
SoftwareSerial victronSerial(rxPin, txPin);         // RX, TX Using Software Serial so we can use the hardware serial to check the ouput
                                                    // via the USB serial provided by the NodeMCU.
char receivedChars[buffsize];                       // an array to store the received data
char tempChars[buffsize];                           // an array to manipulate the received data
char recv_label[num_keywords][label_bytes]  = {0};  // {0} tells the compiler to initalize it with 0. 
char recv_value[num_keywords][value_bytes]  = {0};  // That does not mean it is filled with 0's
char value[num_keywords][value_bytes]       = {0};  // The array that holds the verified data
static byte blockindex = 0;
bool new_data = false;
bool blockend = false;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(19200);
    victronSerial.begin(19200);
}

void loop() {
    // Receive information on Serial from MPPT
    RecvWithEndMarker();
    HandleNewData();

    // Just print the values every second,
    // Add your own code here to use the data. 
    // Make sure to not used delay(X)s of bigger than 50ms,
    // so make use of the same principle used in PrintEverySecond() 
    // or use some sort of Alarm/Timer Library
    PrintEverySecond();
}

// Serial Handling
// ---
// This block handles the serial reception of the data in a 
// non blocking way. It checks the Serial line for characters and 
// parses them in fields. If a block of data is send, which always ends
// with "Checksum" field, the whole block is checked and if deemed correct
// copied to the 'value' array. 

void RecvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (victronSerial.available() > 0 && new_data == false) {
        rc = victronSerial.read();
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= buffsize) {
                ndx = buffsize - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            new_data = true;
        }
        yield();
    }
}

void HandleNewData() {
    // We have gotten a field of data 
    if (new_data == true) {
        //Copy it to the temp array because parseData will alter it.
        strcpy(tempChars, receivedChars);
        ParseData();
        new_data = false;
    }
}

void ParseData() {
    char * strtokIndx; // this is used by strtok() as an index
    strtokIndx = strtok(tempChars,"\t");      // get the first part - the label
    // The last field of a block is always the Checksum
    if (strcmp(strtokIndx, "Checksum") == 0) {
        blockend = true;
    }
    strcpy(recv_label[blockindex], strtokIndx); // copy it to label
    
    // Now get the value
    strtokIndx = strtok(NULL, "\r");    // This continues where the previous call left off until '/r'.
    if (strtokIndx != NULL) {           // We need to check here if we don't receive NULL.
        strcpy(recv_value[blockindex], strtokIndx);
    }
    blockindex++;

    if (blockend) {
        // We got a whole block into the received data.
        // Check if the data received is not corrupted.
        // Sum off all received bytes should be 0;
        byte checksum = 0;
        for (int x = 0; x < blockindex; x++) {
            // Loop over the labels and value gotten and add them.
            // Using a byte so the the % 256 is integrated. 
            char *v = recv_value[x];
            char *l = recv_label[x];
            while (*v) {
                checksum += *v;
                v++;
            }
            while (*l) {
                checksum+= *l;
                l++;
            }
            // Because we strip the new line(10), the carriage return(13) and 
            // the horizontal tab(9) we add them here again.  
            checksum += 32;
        }
        // Checksum should be 0, so if !0 we have correct data.
        if (!checksum) {
            // Since we are getting blocks that are part of a 
            // keyword chain, but are not certain where it starts
            // we look for the corresponding label. This loop has a trick
            // that will start searching for the next label at the start of the last
            // hit, which should optimize it. 
            int start = 0;
            for (int i = 0; i < blockindex; i++) {
              for (int j = start; (j - start) < num_keywords; j++) {
                if (strcmp(recv_label[i], keywords[j % num_keywords]) == 0) {
                  // found the label, copy it to the value array
                  strcpy(value[j], recv_value[i]);
                  start = (j + 1) % num_keywords; // start searching the next one at this hit +1
                  break;
                }
              }
            }
        }
        // Reset the block index, and make sure we clear blockend.
        blockindex = 0;
        blockend = false;
    }
}

void PrintEverySecond() {
    static unsigned long prev_millis;
    if (millis() - prev_millis > 1000) {
        PrintValues();
        prev_millis = millis();
    }
}


void PrintValues() {
    for (int i = 0; i < num_keywords; i++){
        Serial.print(keywords[i]);
        Serial.print(",");
        Serial.println(value[i]);
    }
}