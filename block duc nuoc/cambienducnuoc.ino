#include <WiFi.h> 
#include <PubSubClient.h>


const int sensorPin =34;
const char* ssid = "P813";     // Your WiFi SSID
const char* password = "dddddddd"; // Your WiFi Password
const char* mqtt_server = "test.mosquitto.org";  // MQTT broker server //broker.hivemq.com

WiFiClient espClient;
PubSubClient clientmqtt(espClient);
unsigned long lastMsg = 0;



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
  pinMode(sensorPin,INPUT);
}

void loop() {
  if (!clientmqtt.connected()) {
    reconnect();  
  }
  clientmqtt.loop();  // Keep the MQTT connection alive

  unsigned long now = millis();
  if (now - lastMsg > 15000) {  // Publish every 2 seconds
    lastMsg = now;

  int sensorValue = analogRead(sensorPin);  // Đọc giá trị từ cảm biến (0 - 4095)
  
  // Chuyển đổi giá trị ADC sang tỷ lệ 0-100% để dễ hiểu
  float turbidityPercent = sensorValue*100/2200; // 100% là nước trong, 0% là rất đục, 2200 là ngưỡng giá trị cao nhất


  // Hiển thị kết quả lên Serial Monitor
  Serial.print("Giá trị cảm biến (ADC): ");
  Serial.print(sensorValue);
  Serial.print(" - Mức độ đục: ");
  Serial.print(turbidityPercent);
  Serial.println("%");


    
    String data = String(sensorValue);
    clientmqtt.publish("topic/sen3", data.c_str());
  }
}