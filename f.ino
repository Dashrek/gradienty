#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#include <OneWire.h>
#include <DS18B20.h>
#define ONEWIRE_PIN 2
float maxi(float mala[18]){
  float a=-273.15;

  for (int i=0;i<18;i++){
    if (mala[i]>a) a=mala[i];
  }
  return a;
  }
float mini(float mala[18]){
  float a=1000000.0;

  for (int i=0;i<18;i++){
    if (mala[i]<a) a=mala[i];
  }
  return a;
  }
float srednia(float mala[18]){
  float a=0;

  for (int i=0;i<18;i++){
     a+=mala[i];
  }
  a=(a-mini(mala)-maxi(mala))/16;
  return a;
  }
// Adres czujnika
byte address[8] = {0x28,0xFF,0x77,0x29,0xA2,0x17,0x4,0xCD};

OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);
float mala[18]={0.0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0};
short int numer=0;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4]={0x71,0xCE,0x69,0X08};

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  sensors.begin();
  rfid.PCD_Init(); // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);


}

void loop() {

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] == nuidPICC[0] ||
    rfid.uid.uidByte[1] == nuidPICC[1] ||
    rfid.uid.uidByte[2] == nuidPICC[2] ||
    rfid.uid.uidByte[3] == nuidPICC[3] ) {
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    for(int i=0; i<18;i++){

    float temperature = sensors.readTemperature(address);
    mala[numer]=temperature;
    if (numer==17){
      Serial.print(srednia(mala));
      Serial.println(F(" 'C"));
      numer=0;
      }else numer++;
    sensors.request(address);
    delay(100);
  }
  }
  else Serial.println(F("Not this Card."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}