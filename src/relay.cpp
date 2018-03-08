#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library
#include <FastCRC.h>  //CRCcheck library

// Declaring definitions

#define DEBUG 1 // if 1, debug with Serial
#define RX_433 2 // Pin connecter to Transmitter
#define MSGLEN 4 // Msg len is 4 = 2 signed int (2 bytes each)
#define PCKTLEN MSGLEN+3 // +1 for the lenght of the msgpacket +2 for CRC 16

// Declaring structs

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

// Declaring variables

byte buffer[PCKTLEN] = {}; // init unsigned bytes to be sent over
FastCRC16 CRC16;
intarray itempext, itempeau, crcc, crcc_r;

// Declaring functions

int bytes2int(byte arg[2]) { // Convert a 2byte array into int
  intarray result;
  result.part[0] = arg[0];
  result.part[1] = arg[1];
  return result.ints;
}

void splitpacket(byte msg[PCKTLEN], byte part1[2], byte part2[2]) { // build array to be sent
  part1[0] = msg[1];
  part1[1] = msg[2];
  part2[0] = msg[3];
  part2[1] = msg[4];
}

void setup() {

  man.setupReceive(RX_433, MAN_600);
  man.beginReceiveArray(PCKTLEN, buffer);

  if (DEBUG) {  // Sending over Serial to make sure it works
    Serial.begin(9600);
    Serial.println("Initialising debug mode...");
    Serial.println("Received packets : ");
  }
}

void loop() {
  if (man.receiveComplete())
  {
    uint8_t receivedSize = 0;

    //do something with the data in 'buffer' here before you start receiving to the same buffer again
    receivedSize = buffer[0];
    Serial.println("Raw data : ");
    for(uint8_t i=0; i<receivedSize; i++) {
      Serial.println(buffer[i],HEX);
    }
    splitpacket(buffer, itempext.part, itempeau.part);
    Serial.print("Température extérieure : ");
    Serial.println(float(itempext.ints)/100);
    Serial.print("Température eau : ");
    Serial.println(float(itempeau.ints)/100);
    Serial.println("#######################################");
    crcc_r.part[0]= buffer[5];
    crcc_r.part[1]= buffer[6];
    buffer[5] = 0;
    buffer[6] = 0;

    crcc.ints = CRC16.ccitt(buffer, sizeof(buffer));
    Serial.print(crcc.part[0],HEX);
    Serial.println(crcc.part[1],HEX);
    Serial.print(crcc_r.part[0],HEX);
    Serial.println(crcc_r.part[1],HEX);
    if (crcc.ints == crcc_r.ints) {
      Serial.println("CRC Check ok");
    }


    man.beginReceiveArray(PCKTLEN, buffer);
  }
}
