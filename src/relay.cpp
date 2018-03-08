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
int nloop = 1; // counting n° of receipts

// Declaring functions

int bytes2int(byte arg[2]) { // Convert a 2byte array into int
  intarray result;
  result.part[0] = arg[0];
  result.part[1] = arg[1];
  return result.ints;
}

void splitpacket(byte msg[PCKTLEN], byte part1[2], byte part2[2], byte part3[3]) { // Split array in variables
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
  if (man.receiveComplete()) // When receipt
  {
    uint8_t receivedSize = 0;
    receivedSize = buffer[0]; // Storing the length of the packet received

    if (DEBUG) {
      Serial.print("########## PACKET N° "); // Showing raw data receivedSize
      Serial.print(nloop);
      Serial.println(" #################");
      Serial.println("Raw data : ");
      Serial.print("Packet length : ");
      Serial.println(buffer[0],HEX);
      for(uint8_t i=1;i<receivedSize-2;i++) {
        if (i % 2 != 0) {
          Serial.print("Msg n°");
          Serial.print((i/2)+1);
          Serial.print(" : ");
          Serial.print(buffer[i],HEX);
          Serial.print(" ");
        }
        else {
          Serial.println(buffer[i],HEX);
        }
      }
    }

    crc_rx.part[0]= buffer[PCKTLEN-2]; // Received 2bytes CRC
    crc_rx.part[1]= buffer[PCKTLEN-1];
    buffer[PCKTLEN-2] = 0;
    buffer[PCKTLEN-1] = 0;
    crc_local.ints = CRC16.ccitt(buffer, sizeof(buffer)); // Calculating own CRC

    if (DEBUG) {  // Showing CRC
      Serial.print("Received CRC : ");
      Serial.print(crc_rx.part[0],HEX);
      Serial.println(crc_rx.part[1],HEX);
      Serial.print("Local CRC : ");
      Serial.print(crc_local.part[0],HEX);
      Serial.println(crc_local.part[1],HEX);
      if (crc_local.ints == crc_rx.ints) {
        Serial.println("CRC Check ok");
      }
      else{
        Serial.println("ERROR CRC!!!");
      }
    }


    splitpacket(buffer, itempext.part, itempeau.part, ihumid.part); // Splitting packet in various variables

    if (DEBUG) {  // Showing Final values
      Serial.print("Température extérieure : ");
      Serial.println(float(itempext.ints)/100);
      Serial.print("Température eau : ");
      Serial.println(float(itempeau.ints)/100);
      Serial.print("Humidité : ");
      Serial.println(float(ihumid.ints)/100);
      nloop++;
    }

    man.beginReceiveArray(PCKTLEN, buffer);
  }
}
