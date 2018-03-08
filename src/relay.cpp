#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library
#include <FastCRC.h>  //CRCcheck library

// Declaring definitions

#define DEBUG 1 // if 1, debug with Serial
#define RX_433 2 // Pin connecter to Transmitter
#define NBPARAM 3 // Number of int sent
#define MSGLEN NBPARAM*2 // Msg len is 4 = 2 signed int (2 bytes each)
#define PCKTLEN MSGLEN+3 // +1 for the lenght of the msgpacket +2 for CRC 16

// Declaring structs

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

// Declaring variables

byte buffer[PCKTLEN] = {}; // init unsigned bytes to be sent over
FastCRC16 CRC16;
intarray itempext, itempeau, ihumid, crc_local, crc_rx;

// Declaring functions

int bytes2int(byte arg[2]) { // Convert a 2byte array into int
  intarray result;
  result.part[0] = arg[0];
  result.part[1] = arg[1];
  return result.ints;
}

void splitpacket(byte msg[PCKTLEN], byte part1[2], byte part2[2], byte part3[3]) { // build array to be sent
  part1[0] = msg[1];
  part1[1] = msg[2];
  part2[0] = msg[3];
  part2[1] = msg[4];
  part3[0] = msg[5];
  part3[1] = msg[6];
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
    splitpacket(buffer, itempext.part, itempeau.part, ihumid.part);
    Serial.print("Température extérieure : ");
    Serial.println(float(itempext.ints)/100);
    Serial.print("Température eau : ");
    Serial.println(float(itempeau.ints)/100);
    Serial.print("Humidité : ");
    Serial.println(float(ihumid.ints)/100);
    Serial.println("#######################################");
    crc_rx.part[0]= buffer[PCKTLEN-2];
    crc_rx.part[1]= buffer[PCKTLEN-1];
    buffer[PCKTLEN-2] = 0;
    buffer[PCKTLEN-1] = 0;

    crc_local.ints = CRC16.ccitt(buffer, sizeof(buffer));
    Serial.print(crc_local.part[0],HEX);
    Serial.println(crc_local.part[1],HEX);
    Serial.print(crc_rx.part[0],HEX);
    Serial.println(crc_rx.part[1],HEX);
    if (crc_local.ints == crc_rx.ints) {
      Serial.println("CRC Check ok");
    }


    man.beginReceiveArray(PCKTLEN, buffer);
  }
}
