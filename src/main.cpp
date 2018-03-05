#include <Arduino.h>
#include <Manchester.h>  //Initialising 433 wireless library


#define DEBUG 1 // if 1, debug with Serial
#define RX_433 2 // Pin connecter to Transmitter


byte msgpacket[] = {1,2,3,4}; // init unsigned bytes to be sent over

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
  man.beginReceive();

  if (DEBUG) {  // Sending over Serial to make sure it works
    Serial.begin(9600);
    Serial.println("Initialising debug mode...");
    Serial.println("Received packets : ");
    }
  }

void loop() {
  if (DEBUG) {
  if (man.receiveComplete()) {
      byte m = man.getMessage();
      man.beginReceive(); //start listening for next message right after you retrieve the message
      Serial.println(m,HEX);
}
}
}
