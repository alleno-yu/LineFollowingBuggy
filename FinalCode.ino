int r=1;
int pressurePin = A0;
int isObstaclePin = 5;
int force = 0;
int isObstacle = HIGH; // HIGH MEANS NO OBSTACLE
bool Detection();
bool IsNewCardCorrect();
void Lock();
void Unlock();
void LOCK_UNLOCK();
void brakelight();
unsigned char len = 0;
unsigned char buf[8];

#define LED_G 4 //define green LED pin
#define Control 21 //define brake light control pin
#define Ground 12 //define ground
#include <SPI.h>
#include <MFRC522.h>
#include <mcp_can.h>
#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
const int SPI_CS_PIN = 17;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup() 
{

  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(isObstaclePin, INPUT);
  pinMode(pressurePin, INPUT);
  digitalWrite(LED_G, LOW);
  pinMode(Control, OUTPUT);
  pinMode(Ground, OUTPUT);
  digitalWrite(Ground,LOW);
      while (CAN_OK != CAN.begin(CAN_100KBPS))              // init can bus : baudrate = 100kbps
    {
        delay(100);
    }
}

void loop(){
  if(r==0){
  if(Detection() == false){
    for(int i=0; i<20; i++){
      LOCK_UNLOCK();
      brakelight();
      if(Detection() == true) {
          LOCK_UNLOCK();
        return;
      }
      else delay(500);
    }
    if(Detection() == false){
      Serial.println("0");
      digitalWrite(LED_G, LOW);
      r=1;
      LOCK_UNLOCK();
      brakelight();
      return;
    }
    else return;
   }
  else{
  LOCK_UNLOCK();
  brakelight();
  }
}
else{
  Serial.println("Message : 1");
          LOCK_UNLOCK();
          brakelight();
}
}


bool Detection(){
     force = analogRead(pressurePin);
     isObstacle = digitalRead(isObstaclePin);
     if (force > 500 || isObstacle == LOW) return true;
     else return false; 
 }
bool IsNewCardCorrect() {
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return false;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
  return false;
  }
  
//  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
//  Serial.println();
  content.toUpperCase();
  
  if (content.substring(1) == "04 5D 53 6A 5E 43 80") 
  {
//    Serial.println("Message : Authorized Acess");
//    delay(2000);
//    Serial.println();
    return true;
  }
  else {
//    Serial.println("Message : No Acess");
//    delay(2000);
//    Serial.println();
    return false;
  }
}
  void Lock(){
    digitalWrite(LED_G, LOW);
    delay(10);
  }

  
  void Unlock(){
    digitalWrite(LED_G, HIGH);
    delay(10);
  }

  
  void LOCK_UNLOCK(){
              if (r==1){
              if (IsNewCardCorrect()){
              Unlock();
              delay(200);
              r=0;
              }
              else return;
              }
         else{
              if (IsNewCardCorrect()){
              Lock();
              delay(200);
              r=1;
              }
              else return;
              }
  }





void brakelight(){
      unsigned char len = 0;
    unsigned char buf[8];

    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned int canId = CAN.getCanId();
        
        if(canId == 20 && buf[2]==1){
                     digitalWrite(Control,HIGH);
        }
        else if(canId == 20 && buf[2]==0){
            digitalWrite(Control,LOW);
        }
    }
}

  
