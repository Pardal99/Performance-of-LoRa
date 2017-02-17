// LoRa UDP_TX
// Example sketch showing how to create a simple messaging client (transmitter)
// of LoRa technology with the RH_RF95 class
// It is designed to work with the other example LoRa_UDP_RX

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 868.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 1;  // packet counter
boolean flagInitial = false;
unsigned long initialWait = 30000; //30 seconds
unsigned long initialTime;
unsigned long finalTime;
unsigned long transmissionTime;//time of test (final - initial)


void loop()
{
  if(flagInitial==false){
    Serial.print("Wait ");
    Serial.print(initialWait/1000);
    Serial.println(" seconds"); 
    delay(initialWait);
  }
  flagInitial=true;
  initialTime=millis();
  
  for(;packetnum<=100;)
  {//Maximun number of packets
    Serial.println("Send menssage to receiver");
    char radiopacket[20] = "Message number     ";
    itoa(packetnum++, radiopacket+15, 10);
    Serial.print("Sending "); Serial.println(radiopacket);
    radiopacket[19] = 0;
    rf95.send((uint8_t *)radiopacket, 20);
    delay(100);
    rf95.waitPacketSent();
    delay(100);
  } 
    finalTime = millis();
    transmissionTime = finalTime - initialTime;
    Serial.println("-----------------------------------------------------");
    Serial.print("    Transmission time UDP: ");
    Serial.print(transmissionTime);
    Serial.println(" miliseconds (ms)");
    Serial.println("-----------------------------------------------------");
    packetnum = 1;
    delay(50000);
}
