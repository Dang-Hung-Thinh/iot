#include <WiFi.h> 
#include <PubSubClient.h>


const int trigPin=5;
const int echoPin=18;
const char* ssid = "P813";     // Your WiFi SSID
const char* password = "dddddddd"; // Your WiFi Password
const char* mqtt_server = "test.mosquitto.org";  // MQTT broker server //broker.hivemq.com

WiFiClient espClient;
PubSubClient clientmqtt(espClient);
unsigned long lastMsg = 0;

#define SOUND_SPEED 0.034
long duration;
float distanceCm;
float h_of_lake;
void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA);  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("");
  Serial.println("Connected to WiFi.");
}

void reconnect() {
  while (!clientmqtt.connected()) {
    Serial.println("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);  // Create a unique client ID
    if (clientmqtt.connect(clientId.c_str())) {
      Serial.println("MQTT connected.");
      clientmqtt.publish("topic/Publish", "Welcome");  // Send a welcome message to the topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(clientmqtt.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);  // Retry after 5 seconds if connection fails
    }
  }
}



void setup() {
  
  Serial.begin(115200);
  setup_wifi();  // Connect to WiFi
  clientmqtt.setServer(mqtt_server, 1883);  // Set the MQTT server and port
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
}

void loop() {
  if (!clientmqtt.connected()) {
    reconnect();  
  }
  clientmqtt.loop();  // Keep the MQTT connection alive

  unsigned long now = millis();
  if (now - lastMsg > 10000) {  // Publish every 2 seconds
    lastMsg = now;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH, 23500);  // Timeout to avoid lockups

  // Nếu duration là 0, nghĩa là không nhận được tín hiệu Echo
  if (duration > 0) {
    // Calculate the distance in cm
    distanceCm = duration * SOUND_SPEED / 2;
    
    h_of_lake=50-distanceCm;

    // Print the water level to the Serial Monitor
    Serial.print("heigh of lake (cm): ");
    Serial.println(distanceCm);
    
  } else {
    Serial.println("No echo detected!");
  }

    
    String data = String(h_of_lake);
    clientmqtt.publish("topic/sen2", data.c_str());
  }
}