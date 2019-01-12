#include <Manchester.h>  //Initialising 433 wireless library
#include <Crc16.h>  // CRCcheck library
#include <Wire.h> // I2C library

// Declaring definitions

#define DEBUG 1 // if 1, debug with Serial
#define RX_433 2 // Pin connecter to Transmitter
#define NBPARAM 3 // Number of int sent
#define MSGLEN NBPARAM*2 // Msg len is 4 = 2 signed int (2 bytes each)
#define PCKTLEN MSGLEN+3 // +1 for the lenght of the msgpacket +2 for CRC 16
#define I2C_ADDR 0x24 // Slave adress

// Declaring structs

union intarray { // shared memory for int and byte array to get its bytes
  int ints;
  byte part[2];
};

// Declaring variables

byte radio_rx[PCKTLEN] = {}; // init unsigned bytes to be receive
byte i2C_tx[PCKTLEN] = {}; // init unsigned bytes to be sent
intarray itempext, itempeau, ihumid, crc_local, crc_rx;
int nloop = 1; // counting n° of receipts
int ncrcok = 0;
float successrate = 0;

// Declaring functions

int bytes2int(byte arg[2]) { // Convert a 2byte array into int
  intarray result;
  result.part[0] = arg[0];
  result.part[1] = arg[1];
  return result.ints;
}

int getcrc(byte msg[PCKTLEN]) { // get 16bit CRC
  Crc16 crc; // init CRC16 object
  for (uint8_t i = 0; i < PCKTLEN - 2; i++) {
    crc.updateCrc(msg[i]);
  }
  return crc.getCrc();
}

void splitpacket(byte msg[PCKTLEN], byte part1[2], byte part2[2], byte part3[3]) { // Split array in variables
  part1[0] = msg[1];
  part1[1] = msg[2];
  part2[0] = msg[3];
  part2[1] = msg[4];
  part3[0] = msg[5];
  part3[1] = msg[6];
}

// Declaring handler

void receiveData(int byteCount) {

  while (Wire.available()) {
    Wire.read();
    Serial.println("Request from pi...");
  }
}

// callback for sending data
void PiRequete() {
  Wire.write( i2C_tx, sizeof(i2C_tx) );
}

void setup() {
  man.setupReceive(RX_433, MAN_600); // Init 433 connection
  man.beginReceiveArray(PCKTLEN, radio_rx); // begin listening
  Wire.begin(I2C_ADDR); // begin I2C connection as slave
  Wire.onRequest(PiRequete);
  Wire.onReceive(receiveData);
  Serial.begin(9600); // start serial for output
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
    receivedSize = radio_rx[0]; // Storing the length of the packet received

    if (DEBUG) {
      Serial.print("########## PACKET N° "); // Showing raw data receivedSize
      Serial.print(nloop);
      Serial.println(" #################");
      Serial.print("RX PCKT LEN : ");
      Serial.print(radio_rx[0],HEX);
      for(uint8_t i=1;i<receivedSize-2;i++) {
        if (i % 2 != 0) {
          Serial.print(" Msg n°");
          Serial.print((i/2)+1);
          Serial.print(" : ");
          Serial.print(radio_rx[i],HEX);
          Serial.print(" ");
        }
        else {
          Serial.print(radio_rx[i],HEX);
        }
      }
    }

    crc_rx.part[0]= radio_rx[PCKTLEN-2]; // Received 2bytes CRC
    crc_rx.part[1]= radio_rx[PCKTLEN-1];
    radio_rx[PCKTLEN-2] = 0;
    radio_rx[PCKTLEN-1] = 0;
    crc_local.ints = getcrc(radio_rx); // Calculating own CRC
    radio_rx[PCKTLEN-2] = crc_rx.part[0];
    radio_rx[PCKTLEN-1] = crc_rx.part[1];

    if (DEBUG) {  // Showing CRC
      Serial.println("");
      Serial.print("CRC : ");
      Serial.print(crc_rx.part[0],HEX);
      Serial.print(crc_rx.part[1],HEX);
      Serial.print(" Local CRC : ");
      Serial.print(crc_local.part[0],HEX);
      Serial.print(crc_local.part[1],HEX);
      if (crc_local.ints == crc_rx.ints) {
        Serial.println(" CRC Check ok");
      }
      else{
        Serial.println(" ERROR CRC!!!");
      }
    }


    if (crc_local.ints == crc_rx.ints) { // if and only if crc is ok, the array which will be sent is updated
      i2C_tx[0] = PCKTLEN;
      for(uint8_t i=1;i<PCKTLEN;i++) {
        i2C_tx[i] = radio_rx[i];


    }
    ncrcok++;
  }

    if (DEBUG) {  // Showing Final values
      splitpacket(radio_rx, itempext.part, itempeau.part, ihumid.part); // Splitting packet in various variables
      Serial.print("Temp ext : ");
      Serial.print(float(itempext.ints)/100);
      Serial.print(" Temp eau : ");
      Serial.print(float(itempeau.ints)/100);
      Serial.print(" Humid : ");
      Serial.println(float(ihumid.ints)/100);
      Serial.print("CRC OK (%) : ");
      successrate = float(ncrcok) / float(nloop) * 100;
      Serial.println(successrate);
      nloop++;
    }

    man.beginReceiveArray(PCKTLEN, radio_rx);
  }
}
