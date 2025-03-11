#include "Arduino.h"
#include "MPU9250.h"

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
enum MsgType {info=0, error=1, imu=2 };
#define msgDelimiter '|'
#define msgTokenInfo "I"
#define msgTokenError "E"
#define msgTokenImu "M"
#define msgTokenInvalid "x"

void postMessage(MsgType mType, String message);
void postInfo(String message);
void postError(String message);
void postValues(float pitch, float roll,float yaw);


MPU9250 mpu;

void print_roll_pitch_yaw();
void print_calibration();

void setup() {
    Serial.begin(115200);
    SerialBT.begin("Hobbywerkerei"); //Name des ESP32
    postInfo("ESP32 ist bereit...");
    
    Wire.begin();
    delay(2000);

    if (!mpu.setup(0x68)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
            postError("MPU connection failed.");
            delay(2000);
            postError("Check I2C connection.");
            delay(2000);
        }
    }

    // calibrate anytime you want to
    postInfo("Gyro calibration in 5sec.");
    
    mpu.verbose(true);
    delay(5000);
    postInfo("Calibration...");
    mpu.calibrateAccelGyro();
    postInfo("OK");
    delay(2000);
    postInfo("Mag calibration will start in 5sec");
    delay(5000);
    postInfo(" Wave device figure 8");
    mpu.calibrateMag();
    postInfo("OK");
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
        
        if (millis() > prev_ms + 10) {
          count++;
          if (count < 100)
          {
            postInfo("Init, please wait...");
            rollInit=roll;
            pitchInit=pitch;
            yawInit=yaw;
          } 
          else
          {
            postValues(roll, pitch, yaw);
          }
          prev_ms = millis();
        }
    }
    delay(1);
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

String messageTypeToString(MsgType mType)
{
  switch(mType)
  {
    case MsgType::info:{ return msgTokenInfo; }
    case MsgType::error: {return msgTokenError; }
    case MsgType::imu: {return msgTokenImu;}
    default: 
      return msgTokenInvalid;
  }
}


String  value2Message(float pitch, float roll,float yaw)
{
  return String(pitch,1) + msgDelimiter+ String(roll,1) + msgDelimiter+ String(yaw,1);
}

void postValues(float pitch, float roll,float yaw)
{
  String msg = value2Message( pitch, roll, yaw);
  postMessage(MsgType::imu, msg);
}

void postError(String message)
{
  postMessage(MsgType::error,  message);
}

void postInfo(String message)
{
  postMessage(MsgType::info,  message);
}

void postMessage(MsgType mType, String message)
{

  String msg =  messageTypeToString(mType) + msgDelimiter + message;
  if (Serial.available()) {
    Serial.println(msg);
  }

  if (SerialBT.isReady()) {
   SerialBT.println(msg);
  }
}