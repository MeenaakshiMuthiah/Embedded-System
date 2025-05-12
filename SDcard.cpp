#include <Arduino.h>
#include <SD.h>

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor (optional for ESP32)

  // Initialize SD card (CS pin = 5)
  if (!SD.begin(5)) {
    Serial.println("SD Card initialization failed!");
    return; // Stop if SD card fails
  }
  Serial.println("SD Card initialized.");

  // Open file for writing (and create if it doesn't exist)
  File writeFile = SD.open("/test.txt", FILE_WRITE);
  if (!writeFile) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Write data to file
  for (int i = 0; i < 10; i++) {
    writeFile.println("meenaakshi"); // Write 10 lines
  }
  writeFile.close();
  Serial.println("Data written to test.txt");

  // Open file for reading
  File readFile = SD.open("/test.txt", FILE_READ);
  if (!readFile) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Read and print file content
  Serial.println("File Content:");
  while (readFile.available()) {
    Serial.write(readFile.read());
  }
  readFile.close();
}

void loop() {
  // Nothing here (runs once due to setup())
}
