#define ENA   14          // Enable/speed motors FWD          GPIO14(D5)
#define ENB   12          // Enable/speed motors Steer        GPIO12(D6)
#define IN_1  15          // L298N in1 motors Forward         GPIO15(D8)
#define IN_2  13          // L298N in2 motors Back            GPIO13(D7)
#define IN_3  2           // L298N in3 motors Left            GPIO2(D4)
#define IN_4  0           // L298N in4 motors Right           GPIO0(D3)

#include <WiFi.h>

const char* ssid = "J_WICK";
const char* password = "bitemebitemebiteme";
const int serverPort = 12345;

int fwd = 0;
int steer = 0;

WiFiServer server(serverPort);

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  server.begin();

  // Print ESP32's IP address to Serial Monitor
  Serial.println("ESP32 IP Address: " + WiFi.localIP().toString());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available() >= 4) { // assuming 4 bytes for the integer
        int receivedInteger;
        uint8_t bytes[4];
        
        // Read the bytes in reverse order
        for (int i = 3; i >= 0; i--) {
          bytes[i] = client.read();
        }
        
        // Convert bytes to integer
        receivedInteger = *((int*)bytes);
        weedOut(receivedInteger);
        
        //Serial Plotter
        Serial.print("Forward:");
        Serial.print(String(fwd));
        Serial.print(',');
        Serial.print("Steer:");
        Serial.print(String(steer));
        Serial.println();   

        move();
        // Serial.print("DADF");

      }
    }
    client.stop();
  }
}

void weedOut(int receivedInteger) {
  if (receivedInteger >= 100 && receivedInteger <= 300) {
    fwd = receivedInteger - 200;
  } else if (receivedInteger >= 400 && receivedInteger <= 600) {
    steer = receivedInteger - 500;
  }
}


void move() {
  int fwdMapped = map(abs(fwd), 0, 100, 400, 1023);
  int steerMapped = map(abs(steer), 0, 100, 400, 1023);

  if(fwd > 0){
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, fwdMapped);
    Serial.println("forward " + String(fwdMapped));
  } else {
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, fwdMapped);
    Serial.println("back " + String(fwdMapped));
  }

  if(steer > 0){
    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENA, steerMapped);
    Serial.println("left " + String(steerMapped));
  } else {
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENA, steerMapped);
    Serial.println("right " + String(steerMapped));
  }

}

void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}
