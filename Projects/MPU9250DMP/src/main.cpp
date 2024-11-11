#include "Arduino.h"
#include "MPU9250.h"

MPU9250 mpu;

void print_roll_pitch_yaw();
void print_calibration();

void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(2000);

    if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            delay(5000);
        }
    }

    // calibrate anytime you want to
    Serial.println("Accel Gyro calibration will start in 5sec.");
    Serial.println("Please leave the device still on the flat plane.");
    mpu.verbose(true);
    delay(5000);
    mpu.calibrateAccelGyro();

    Serial.println("Mag calibration will start in 5sec.");
    Serial.println("Please Wave device in a figure eight until done.");
    delay(5000);
    mpu.calibrateMag();

    print_calibration();
    mpu.verbose(false);
}
int count =0;
float roll=0.0;
float pitch=0.0;
float yaw=0.0;
float rollInit=0.0;
float pitchInit=0.0;
float yawInit=0.0;
float lowPassWeight = 0.01;

void loop() {
    if (mpu.update()) {
        static uint32_t prev_ms = millis();
        
        roll = roll *(1.0-lowPassWeight) + mpu.getRoll() * lowPassWeight;
        pitch = pitch *(1.0-lowPassWeight) + mpu.getPitch() * lowPassWeight;
        yaw = yaw *(1.0-lowPassWeight) + mpu.getYaw() * lowPassWeight;
        
        if (millis() > prev_ms + 25) {
          count++;
          if (count == 0)
            {
              rollInit=roll;
              pitchInit=pitch;
              yawInit=yaw;
            } 
            print_roll_pitch_yaw();
            prev_ms = millis();
        }
    }
}

void print_roll_pitch_yaw() {
    Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(yaw - yawInit, 1);
    Serial.print(", ");
    Serial.print(pitch - pitchInit, 1);
    Serial.print(", ");
    Serial.println(roll - rollInit, 1);
}

void print_calibration() {
    Serial.println("< calibration parameters >");
    Serial.println("accel bias [g]: ");
    Serial.print(mpu.getAccBiasX() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasY() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getAccBiasZ() * 1000.f / (float)MPU9250::CALIB_ACCEL_SENSITIVITY);
    Serial.println();
    Serial.println("gyro bias [deg/s]: ");
    Serial.print(mpu.getGyroBiasX() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasY() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.print(", ");
    Serial.print(mpu.getGyroBiasZ() / (float)MPU9250::CALIB_GYRO_SENSITIVITY);
    Serial.println();
    Serial.println("mag bias [mG]: ");
    Serial.print(mpu.getMagBiasX());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasY());
    Serial.print(", ");
    Serial.print(mpu.getMagBiasZ());
    Serial.println();
    Serial.println("mag scale []: ");
    Serial.print(mpu.getMagScaleX());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleY());
    Serial.print(", ");
    Serial.print(mpu.getMagScaleZ());
    Serial.println();
}