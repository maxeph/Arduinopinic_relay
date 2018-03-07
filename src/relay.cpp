#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library


#define DEBUG 1 // if 1, debug with Serial
#define RX_433 2 // Pin connecter to Transmitter
#define MSGLEN 4
#define PCKTLEN MSGLEN+1


byte buffer[PCKTLEN] = {}; // init unsigned bytes to be sent over

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

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

intarray itempext, itempeau;

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
    for(uint8_t i=1; i<receivedSize; i++) {
      Serial.println(buffer[i]);
      }
      splitpacket(buffer, itempext.part, itempeau.part);
      Serial.print("Température extérieure : ");
      Serial.println(float(itempext.ints)/100);
      Serial.print("Température eau : ");
      Serial.println(float(itempeau.ints)/100);
      Serial.println("#######################################");

    man.beginReceiveArray(PCKTLEN, buffer);
  }
}
