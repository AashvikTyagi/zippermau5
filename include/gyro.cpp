#include <Simple_MPU6050.h>

Simple_MPU6050 mpu;
#define MPU6050_DEFAULT_ADDRESS 0x68
static unsigned long lastReadTime;
float y,p,r;
void getRot(int16_t *gyro, int16_t *accel, int32_t *quat);

void setup() {
  mpu.begin();
  mpu.Set_DMP_Output_Rate_Hz(10);
  mpu.CalibrateMPU();
  mpu.load_DMP_Image();
  mpu.on_FIFO(getRot);
  Serial.begin(115200);
}

void loop() {
  Serial.print(y);
  Serial.print('-');
  Serial.print(p);
  Serial.print('-');
  Serial.println(r);
  delay(100);
}

// a function called whenever updateGyro is called and gyro data is available
void getRot(int16_t *gyro, int16_t *accel, int32_t *quat) {
  Quaternion q;
  VectorFloat gravity;
  float ypr[3] = { 0, 0, 0 };
  float xyz[3] = { 0, 0, 0 };
  mpu.GetQuaternion(&q, quat);
  mpu.GetGravity(&gravity, &q);
  mpu.GetYawPitchRoll(ypr, &q, &gravity);
  mpu.ConvertToDegrees(ypr, xyz);
  y = xyz[0];
  p = xyz[1];
  r = xyz[2];
}

void updateGyro() {
  // if 99 millis have passed since last read and data is available
  // then data will update and lastReadTime variable will reset
  if ((millis() - lastReadTime) >= (99))
    if (mpu.dmp_read_fifo(0)) lastReadTime = millis();
}