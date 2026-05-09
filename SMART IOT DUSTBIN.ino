#include <ESP8266WiFi.h> 
#include <ThingSpeak.h> 
#include <Servo.h> 
// -------- WiFi -------- 
const char* ssid = "ELAYARAJA"; 
const char* password = "12345678"; 
// -------- ThingSpeak -------- 
unsigned long channelID = 3249995; 
const char* writeAPIKey = "55EJK4XSUSHL70QX"; 
WiFiClient client; 
// -------- Pins -------- 
#define TRIG_PIN D5 
#define ECHO_PIN D6 
#define SERVO_PIN D2 
#define GAS_PIN A0 
#define RAIN_PIN D1 
#define BUZZER_PIN D7 
Servo dustbinServo; 
// -------- Variables -------- 
long duration; 
int distance; 
int gasValue; 
int rainValue; 
void setup() { 
Serial.begin(9600); 
pinMode(TRIG_PIN, OUTPUT); 
pinMode(ECHO_PIN, INPUT); 
pinMode(RAIN_PIN, INPUT); 
pinMode(BUZZER_PIN, OUTPUT); 
dustbinServo.attach(SERVO_PIN); 
50  
dustbinServo.write(0);   // Lid closed 
digitalWrite(BUZZER_PIN, LOW); 
// WiFi connection 
WiFi.begin(ssid, password); 
Serial.print("Connecting to WiFi"); 
while (WiFi.status() != WL_CONNECTED) { 
delay(500); 
Serial.print("."); 
} 
Serial.println("\nWiFi Connected"); 
ThingSpeak.begin(client); 
} 
void loop() { 
// -------- Ultrasonic Distance -------- 
digitalWrite(TRIG_PIN, LOW); 
delayMicroseconds(2); 
digitalWrite(TRIG_PIN, HIGH); 
delayMicroseconds(10); 
digitalWrite(TRIG_PIN, LOW); 
duration = pulseIn(ECHO_PIN, HIGH); 
distance = duration * 0.034 / 2; 
// -------- Gas Sensor -------- 
gasValue = analogRead(GAS_PIN); 
// -------- Rain Sensor -------- 
rainValue = digitalRead(RAIN_PIN);  // 1 = rain detected 
// -------- Gas ? Buzzer -------- 
if (gasValue > 600) { 
digitalWrite(BUZZER_PIN, HIGH); 
} else { 
digitalWrite(BUZZER_PIN, LOW); 
} 
// -------- Distance + Rain ? Servo -------- 
if (rainValue == 0 && distance > 0 && distance < 20) { 
dustbinServo.write(90);   // Open lid 
delay(3000); 
51  
dustbinServo.write(0);    // Close lid 
} else { 
dustbinServo.write(0);    // Force closed if rain 
} 
// -------- Serial Monitor -------- 
Serial.println("Distance: " + String(distance) + " cm"); 
Serial.println("Gas: " + String(gasValue)); 
Serial.println("Rain: " + String(rainValue)); 
// -------- ThingSpeak Upload -------- 
ThingSpeak.setField(1, distance); 
ThingSpeak.setField(2, gasValue); 
ThingSpeak.setField(3, rainValue); 
int status = ThingSpeak.writeFields(channelID, writeAPIKey); 
if (status == 200) { 
Serial.println("Data uploaded to ThingSpeak"); 
} else { 
Serial.println("Upload error"); 
} 
delay(15000);   // ThingSpeak minimum delay 
}  
