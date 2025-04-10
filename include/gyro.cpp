#include <Simple_MPU6050.h>

Simple_MPU6050 mpu;
#define MPU6050_DEFAULT_ADDRESS 0x68
static unsigned long last_read_time;
float y,p,r;
void get_rot(int16_t *gyro, int16_t *accel, int32_t *quat);

void setup() {
  mpu.begin();
  mpu.Set_DMP_Output_Rate_Hz(10);
  mpu.CalibrateMPU();
  mpu.load_DMP_Image();
  mpu.on_FIFO(get_rot);
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

// a function called whenever update_gyro() is called and gyro data is available
void get_rot(int16_t *gyro, int16_t *accel, int32_t *quat) {
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

void update_gyro() {
  // if 99 millis have passed since last read and data is available
  // then data will update and last_read_time variable will reset
  if ((millis() - last_read_time) >= (99))
    if (mpu.dmp_read_fifo(0)) last_read_time = millis();
}