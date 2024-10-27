#include <mcp_can.h>
#include <SPI.h>
 
MCP_CAN CAN0(10);     // Set CS to pin 10
 
#define sensor_01_ECHO_PIN    3
#define sensor_01_TRIGGER_PIN 4
#define sensor_02_ECHO_PIN    5
#define sensor_02_TRIGGER_PIN 6
#define sensor_03_ECHO_PIN    7
#define sensor_03_TRIGGER_PIN 8
 
#define NUMBER_OF_SENSORS 3

//define the pin number for the buzzer
#define buzzerPin 				 9

int min_dist=2;
int max_dist=200;

struct sensor_data {
  int echo_pin;
  int trig_pin;
  int measured_distance_cm;
};
 
sensor_data sensor[NUMBER_OF_SENSORS];
 



void setup() {
  Serial.begin(115200);

 
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
  } else {
    Serial.println("Error Initializing MCP2515...");
  }
 
  CAN0.setMode(MCP_NORMAL);  // Set CAN to normal mode
 
  // Set up sensor pins
  sensor[0].echo_pin = sensor_01_ECHO_PIN;
  sensor[0].trig_pin = sensor_01_TRIGGER_PIN;
  sensor[1].echo_pin = sensor_02_ECHO_PIN;
  sensor[1].trig_pin = sensor_02_TRIGGER_PIN;
  sensor[2].echo_pin = sensor_03_ECHO_PIN;
  sensor[2].trig_pin = sensor_03_TRIGGER_PIN;
 
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    pinMode(sensor[i].echo_pin, INPUT);
    pinMode(sensor[i].trig_pin, OUTPUT);
  }
  
}
 
void loop() {
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    digitalWrite(sensor[i].trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(sensor[i].trig_pin, LOW);
 
    // Measure distance in cm
    sensor[i].measured_distance_cm = pulseIn(sensor[i].echo_pin, HIGH);
    sensor[i].measured_distance_cm = round(sensor[i].measured_distance_cm * 0.0343 / 2.0);
 
    // Prepare the data to send via CAN (2 bytes)
    byte data[2];
    data[0] = sensor[i].measured_distance_cm >> 8;  // MSB
    data[1] = sensor[i].measured_distance_cm & 0xFF; // LSB
   
    // Send data via CAN (ID = 0x100 + i, 2 bytes)
    byte sndStat = CAN0.sendMsgBuf(0x100 + i, 0, 2, data);

    // Print sensor distance
    Serial.print("Sensor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(sensor[i].measured_distance_cm);
    Serial.println(" cm");
  }

  // // Read sensor distances
  // int sensor1_dist = sensor[0].measured_distance_cm;
  // int sensor2_dist = sensor[1].measured_distance_cm;
  // int sensor3_dist = sensor[2].measured_distance_cm;

  // // Normalize the distance values
  // float normalized1 = normalizedDistance(sensor1_dist);
  // float normalized2 = normalizedDistance(sensor2_dist);
  // float normalized3 = normalizedDistance(sensor3_dist);

  // // Combine the normalized values (average here, can be weighted if necessary)
  // float combinedValue = (normalized1 + normalized2 + normalized3) / 3.0;

  // // Calculate tone frequency and beep interval based on combined value
  // int buzzerFrequency = calculateTone(combinedValue);
  // unsigned long interval = calculateInterval(combinedValue);

  // // Non-blocking buzzer control using millis()
  // static unsigned long previousMillis = 0;
  // unsigned long currentMillis = millis();

  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;

  //   // Toggle the buzzer
  //   if (digitalRead(buzzerPin) == LOW) {
  //     tone(buzzerPin, buzzerFrequency);  // Play tone based on combined sensor value
  //   } else {
  //     noTone(buzzerPin);  // Stop the buzzer
  //   }
  // }

    delay(400);
  
}


// float normalizedDistance(int sensorValue) {
//   // Normalize the sensor distance to a 0-1 range
//   return (float)(sensorValue - min_dist) / (max_dist - min_dist);
// }

// unsigned long calculateInterval(float combinedValue) {
//   // Map combined value to a beep interval (100ms to 1000ms)
//   return map(combinedValue * 100, 0, 100, 100, 1000);
// }

// int calculateTone(float combinedValue) {
//   // Map combined value to a buzzer frequency (1000Hz to 2000Hz)
//   return map(combinedValue * 100, 0, 100, 1000, 500);
// }