// LoRa TX_TCP
// Example sketch showing how to create a simple messaging client (transmitter)
// of LoRa technology with the RH_RF95 class
// It is designed to work with the other example LoRa_TCP_RX

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 868.0 or other frequency, must match RX's freq!
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

  Serial.println("Arduino LoRa TX_TCP Test!");

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

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

unsigned long initialWait = 30000; //30 seconds
unsigned long initialTime;
unsigned long finalTime;
unsigned long transmissionTime;//time of test (final - initial)
int16_t packetnum = 1;  // packet counter, we increment per xmission
boolean flagInitial = false;
boolean flagReceived = false;
int16_t retriesCounter = 0;
int16_t lostCounter = 0;

void loop()
{
  if(flagInitial==false){
    Serial.print("Wait ");
    Serial.print(initialWait/1000);
    Serial.println(" seconds"); 
    Serial.println("------------------------- ");
    Serial.println("");
    delay(initialWait);
  }
  flagInitial=true;
  initialTime=millis();
  for(;packetnum<=100;)
    {//number of packets of the test
      do{
        char radiopacket[20] = "message number     ";
        itoa(packetnum++, radiopacket+15, 10);
        Serial.print("Sending "); Serial.println(radiopacket);
        radiopacket[19] = 0;
        rf95.send((uint8_t *)radiopacket, 20);        
        rf95.waitPacketSent();
        //Now wait for a reply
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.waitAvailableTimeout(500))
        {//timeout of 500 ms to receive a reply
         if (rf95.recv(buf, &len))
         {//if we receive a reply --> ok
            Serial.println((char*)buf);
            Serial.println("------------------------- ");
            Serial.println("");
            flagReceived = true; 
          }
          else
          {//if we dont receive a reply --> we tried to forward five messages
            if(retriesCounter<=4){               
              retriesCounter++;
              Serial.print("Reception failed, retry number"); Serial.println(retriesCounter);
              Serial.println("   *****   ");
              packetnum--;
            }else{//after five retries --> packet lost        
              retriesCounter = 0;
              Serial.println("Reception failed. Lost packet");
              Serial.println("-------------------------");
              lostCounter++;
              flagReceived = true;
            }
          }
        }
        else
        {//if we dont receive a reply after the 500 ms of timeout --> we tried to forward five messages
          if(retriesCounter<=4){               
            retriesCounter++;// = retriesCounter+1;
            Serial.println("No reply, the receiver is on?");
            Serial.println("   *****   ");
            packetnum--;
          }else{//after five retries --> packet lost         
            retriesCounter = 0;
            Serial.println("Reception failed. Lost packet, receiver is on?");
            lostCounter++;
            flagReceived = true;
          }
        } 
      }while(flagReceived==false );
  delay(100);
  }
  finalTime = millis();
  transmissionTime = finalTime - initialTime;
  Serial.println("/////////////////////////////////////////////////////");
  Serial.print("Number of packets lost: ");Serial.println(lostCounter);
  Serial.println("-----------------------------------------------------");
  Serial.print("    Transmission time TCP: ");
  Serial.print(transmissionTime);
  Serial.println(" miliseconds (ms)");
  Serial.println("-----------------------------------------------------");
  Serial.println("/////////////////////////////////////////////////////");
  
  //reset
  packetnum = 0;  // packet counter, we increment per xmission
  flagReceived = false;
  retriesCounter = 0;
  lostCounter = 0;

  delay(60000);//time to relaunch test  
}
