#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Adafruit_Fingerprint.h>

// Pin declarations
#define Relay 4
#define Buzzer 15
#define REDLED 12
#define REDLED1 25
#define GREENLED 13
#define ORANGELED 14
#define GREENLED1 33

const char *ssid = "V2031";
const char *password = "*********";
void SensorFingerBegin();
AsyncWebServer server(80);
HardwareSerial serialPort(2); // Use UART2
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialPort);

void initializePinMode() {
  pinMode(Relay, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(GREENLED, OUTPUT);
  pinMode(REDLED1, OUTPUT);
  pinMode(GREENLED1, OUTPUT);
  pinMode(ORANGELED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void setup() {
  Serial.begin(115200);
  initializePinMode();

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    return;
  }
  Serial.println("SPIFFS mounted successfully");

  WiFi.mode(WIFI_AP_STA);
  digitalWrite(ORANGELED, HIGH);
  WiFi.begin(ssid, password);
  
  int count = 0;
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    count++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
    digitalWrite(GREENLED, HIGH);
  } else {
    Serial.println("Failed to connect to WiFi");
    digitalWrite(ORANGELED, HIGH);
  }

  WiFi.softAP("fire-AP", "12345678");
  SensorFingerBegin();

  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS");
    while (1) delay(1000);
  }
  Serial.println("MDNS responder started");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<h1>Working</h1>");
  });

  server.begin();
  Serial.println("HTTP server started");
  Serial.println(WiFi.localIP());
}

void loop() {
  static unsigned long lastCheckTime = 0;
  if (millis() - lastCheckTime >= 200) {
    lastCheckTime = millis();
    Serial.println("Place finger on the sensor...");
    if (finger.getImage() == FINGERPRINT_OK) {
      Serial.println("Fingerprint detected!");
      int p = finger.image2Tz();
      if (p == FINGERPRINT_OK) {
        Serial.println("Fingerprint converted!");
        p = finger.fingerSearch();
        if (p == FINGERPRINT_OK) {
          Serial.println("Fingerprint recognized!");
          digitalWrite(GREENLED1, HIGH);
          digitalWrite(GREENLED, HIGH);
          digitalWrite(Buzzer, HIGH);
          delay(2000);
          digitalWrite(Buzzer, LOW);
          digitalWrite(GREENLED, LOW);
          digitalWrite(GREENLED1, LOW);
        } else {
          Serial.println("Fingerprint not recognized.");
          digitalWrite(Buzzer, HIGH);
          digitalWrite(REDLED, HIGH);
          delay(300);
          digitalWrite(Buzzer, LOW);
          delay(1700);
          digitalWrite(REDLED, LOW);
        }
      }
    }
  }
}

void SensorFingerBegin() {
  finger.begin(57600);
  if (!finger.verifyPassword()) {
    Serial.println("Did not find fingerprint sensor :(");
    return;
  }
  Serial.println("Found fingerprint sensor!");
  finger.getParameters();
  Serial.print("Capacity: "); Serial.println(finger.capacity);
  Serial.print("Security level: "); Serial.println(finger.security_level);
}

uint8_t getNextFreeID() {
  for (uint8_t id = 2; id <= 255; id++) {
    if (finger.loadModel(id) != FINGERPRINT_OK) {
      Serial.print("Next free ID: ");
      Serial.println(id);
      return id;
    }
  }
  Serial.println("No free ID available");
  return 0xFF;
}
