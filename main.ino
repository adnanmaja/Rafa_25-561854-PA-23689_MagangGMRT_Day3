// Rafa Attamimi Adnan Atmaja
// 561854

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

Servo servo1, servo2, servo3, servo4, servo5;
Adafruit_MPU6050 mpu;

#define SERVO1_PIN 18
#define SERVO2_PIN 19
#define SERVO3_PIN 25
#define SERVO4_PIN 26
#define SERVO5_PIN 27
#define PIR_PIN 13

const int pwm = 1500;

float previousYaw = 0;
unsigned long yawStopTime = 0;
bool yawIsIdle = false;
int previousPir = LOW;

void setup() {
    Serial.begin(115200);
  
    Wire.begin(21, 22);

    pinMode(PIR_PIN, INPUT);

    servo1.setPeriodHertz(50);
    servo1.attach(SERVO1_PIN, 1000, 2000);
    servo1.writeMicroseconds(pwm);
    servo2.setPeriodHertz(50);
    servo2.attach(SERVO2_PIN, 1000, 2000);
    servo2.writeMicroseconds(pwm);
    servo3.setPeriodHertz(50);
    servo3.attach(SERVO3_PIN, 1000, 2000);
    servo3.writeMicroseconds(pwm);
    servo4.setPeriodHertz(50);
    servo4.attach(SERVO4_PIN, 1000, 2000);
    servo4.writeMicroseconds(pwm);
    servo5.setPeriodHertz(50);
    servo5.attach(SERVO5_PIN, 1000, 2000);
    servo5.writeMicroseconds(pwm);

    if (!mpu.begin()) {
        while (true) delay(10);
    }
}

void loop() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float rollAngle  = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
    float pitchAngle = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI;
    
    static float yawAngle = 0;
    static unsigned long previousTime = 0;
    unsigned long now = millis();
    float deltaTime = (now - previousTime) / 1000.0; // detik
    previousTime = now;
    yawAngle += g.gyro.z * 180.0 / PI * deltaTime;

    roll(rollAngle);
    pitch(pitchAngle);
    yaw(yawAngle);
    handleExternalMotion();

    int rollUs  = 1500 + (int)(-rollAngle * 500.0 / 90.0);
    int pitchUs = 1500 + (int)(pitchAngle * 500.0 / 90.0);
    Serial.print("Roll=");
    Serial.print(rollAngle);
    Serial.print(" Pitch=");
    Serial.print(pitchAngle);
    Serial.print(" PWM S1=");
    Serial.print(rollUs);
    Serial.print(" S2=");
    Serial.print(rollUs);
    Serial.print(" S3=");
    Serial.print(pitchUs);
    Serial.print(" S4=");
    Serial.println(pitchUs);

    delay(100);
}

void roll(float degree){
    int us = 1500 + (-degree * 500 / 90);
    servo1.writeMicroseconds(us);
    servo2.writeMicroseconds(us);
}

void pitch(float degree){
    int us = 1500 + (degree * 500 / 90);
    servo3.writeMicroseconds(us);
    servo4.writeMicroseconds(us);
}

void yaw(float degree){

    if (fabs(degree - previousYaw) > 0.5){
        int us = 1500 + (degree * 500 / 90);
        servo5.writeMicroseconds(us);
        yawIsIdle = false;
    }else{
        if (!yawIsIdle) {
            yawIsIdle = true;
            yawStopTime = millis();
        }
            if (millis() - yawStopTime >= 1000) {
            servo5.writeMicroseconds(1500);
        }
    }
      previousYaw = degree;
}

void handleExternalMotion() {
  int currentPir = digitalRead(PIR_PIN);

  if (currentPir == HIGH && previousPir == LOW) {

    servo1.writeMicroseconds(1750);
    servo2.writeMicroseconds(1750);
    servo3.writeMicroseconds(1750);
    servo4.writeMicroseconds(1750);
    servo5.writeMicroseconds(1750);

    delay(1000);

    servo1.writeMicroseconds(1500);
    servo2.writeMicroseconds(1500);
    servo3.writeMicroseconds(1500);
    servo4.writeMicroseconds(1500);
    servo5.writeMicroseconds(1500);

    delay(1000);
  }
  previousPir = currentPir;
}