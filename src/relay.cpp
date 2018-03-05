#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library


#define DEBUG 1 // if 1, debug with Serial
#define RX_433 2 // Pin connecter to Transmitter


byte buffer[4] = {6,7,8,9}; // init unsigned bytes to be sent over

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

void setup() {

  man.setupReceive(RX_433, MAN_600);
  man.beginReceiveArray(4, buffer);

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
    for(uint8_t i=1; i<receivedSize; i++)
        Serial.println(buffer[i]);

    man.beginReceiveArray(4, buffer);
  }
}
