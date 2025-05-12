#include <Arduino.h>
#include <MFRC522.h>
#include <SD.h>

// Reset pin 27, SDA pin 21
#define RST_PIN 27 
#define SDA_PIN 21

MFRC522 mfrc522(SDA_PIN, RST_PIN); // Create MFRC522 instance
void rfidmatching(String rfid);

void setup()
{
  Serial.begin(115200); // Initiate a serial communication
  SPI.begin();        // Initiate  SPI bus
  
  mfrc522.PCD_Init(); // Initiate MFRC522
  Serial.println("Scan a RFID tag");
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  mfrc522.PCD_SoftPowerDown();

  if (SD.begin(5))
  {
    Serial.println("SD Card initialization DONE");
    // return;
  }
  Serial.println("SD Card initialized");

  File file = SD.open("/test.txt", "w");
  if (file)
  {
    Serial.println("File opened with success");
    file.println("13 F0 7D 0E,1");
    file.println("53 F0 A4 A9,2");
    file.println("03 50 0A 35,3");
    file.println("93 4A E6 26,4");

    file.close();
  }
  else
  {
    Serial.println("Error opening file");
  }

  File readFile = SD.open("/test.txt", "r");
  if (readFile)
  {
    Serial.println("Reading file:");
    while (readFile.available())
    {
      Serial.write(readFile.read());
    }
    readFile.close();
  }
  else
  {
    Serial.println("Error opening file for reading");
  }
  SD.end();

}

void loop()
{
  if (!mfrc522.PICC_IsNewCardPresent())
  { // If a new PICC placed to RFID reader continue
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial())
  { // If a new PICC placed to RFID reader continue
    return;
  }
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  { // Read 4 byte  card UID
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();                               // Change letters to uppercase
  Serial.println("UID tag : " + content.substring(1)); // Print UID on serial monitor
  mfrc522.PICC_HaltA();                                // Stop reading
  mfrc522.PCD_StopCrypto1();
  mfrc522.PCD_SoftPowerDown();
  rfidmatching(content.substring(1)); // Check if the card is registered
  // delay(1000);

}



void rfidmatching(String rfid)
{
  if(SD.begin(5))
  {
    Serial.println("SD Card initialization DONE");
    // return;
  }
  File readFile = SD.open("/test.txt", "r");
  if (readFile)
  {
    Serial.println("Reading file:");
    while (readFile.available())
    {
      String line = readFile.readStringUntil('\n');
      if (line.indexOf(rfid) >= 0) // Changed to >= 0 to match the start of the line
      {
        Serial.println("RFID Matched");

        int commaIndex = line.indexOf(',');
        if (commaIndex > 0)
        {
          String firstPart = line.substring(0, commaIndex);
          String secondPart = line.substring(commaIndex + 1);

          Serial.print("First Part: ");
          Serial.println(firstPart);
          Serial.print("Second Part: ");
          Serial.println(secondPart);
        }
        break;
      }
    }
    readFile.close();
  }
  else
  {
    Serial.println("Error opening file for reading");
  }
  SD.end();
}
