#include <WiFi.h>
#include "DHTesp.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

#define DHT_PIN 15
#define BUZZER 12
const int servoPin = 18;

#define LDR 32

float theta_offset = 30;
double control_fac = 0.75;
float light;
float theta;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

char tempAr[6];
char lightAr[6];

Servo servo;

DHTesp dhtSensor;

bool isScheduledON = false;
unsigned long scheduledOnTime;

void setup() {
  Serial.begin(115200);
  SetupWifi();
  SetupMqtt();

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  servo.attach(servoPin);

  timeClient.begin();
  timeClient.setTimeOffset(5.5 * 3600);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(LDR,INPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    connectToBroker();
  }

  mqttClient.loop();

  updateTemperature();
  updateLightIntensity();
  Serial.println(tempAr);
  Serial.println(lightAr);

  mqttClient.publish("ENTC-ADMIN-TEMP", tempAr);
  mqttClient.publish("ENTC-ADMIN-LIGHT", lightAr);

  checkSchedule();
  servoController();

  delay(1000);
}

void buzzerOn(bool on) {
  if (on) {
    tone(BUZZER, 256);
  } else {
    noTone(BUZZER);
  }
}

void connectToBroker() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32-12345645454")) {
      Serial.println("connected");
      mqttClient.subscribe("ENTC-ADMIN-MAIN-ON-OFF");
      mqttClient.subscribe("ENTC-ADMIN-SCH-ON");
      mqttClient.subscribe("ENTC-ADMIN-THETA-OFFSET");
      mqttClient.subscribe("ENTC-ADMIN-GAMMA");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void SetupMqtt() {
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receiveCallback);
}

void receiveCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char payloadCharAr[length];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadCharAr[i] = (char)payload[i];
  }
  Serial.println("]");

  if (strcmp(topic, "ENTC-ADMIN-MAIN-ON-OFF") == 0) {
    buzzerOn(payloadCharAr[0] == '1');
  } else if (strcmp(topic, "ENTC-ADMIN-SCH-ON") == 0) {
    if (payloadCharAr[0] == 'N') {
      isScheduledON = false;
    } else {
      isScheduledON = true;
      scheduledOnTime = atol(payloadCharAr);
    }
  } else if (strcmp(topic, "ENTC-ADMIN-THETA-OFFSET") == 0) {
    theta_offset = atof(payloadCharAr);
    Serial.println(theta_offset);
  } else if (strcmp(topic, "ENTC-ADMIN-GAMMA") == 0) {
    control_fac = atof(payloadCharAr);
    Serial.println(control_fac);
  }
}

void SetupWifi() {
  Serial.println();
  Serial.print("Connecting to Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void updateTemperature() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  String(data.temperature, 2).toCharArray(tempAr, 6);
}

void checkSchedule() {
  if (isScheduledON) {
    unsigned long currentTime = getTime();
    if (currentTime > scheduledOnTime) {
      buzzerOn(true);
      isScheduledON = false;
      Serial.println("Scheduled ON");
    }
  }
}

unsigned long getTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}

void updateLightIntensity() {
  light = analogRead(LDR);
  String(light/4095, 2).toCharArray(lightAr, 6);
}

void servoController() {
  theta = theta_offset + (180 - theta_offset) * (light/4095 )* (control_fac);
  servo.write(theta);
  Serial.println(theta);
}
