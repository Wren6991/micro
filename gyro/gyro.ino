#define GYRO_ID 0x68
#define ACCEL_OFFS 0x3b
#define GYRO_OFFS 0x43
#define PWR_OFFS 0x6B
#define ACCEL_CONFIG_OFFS 0x1C
#define GYRO_CONFIG_OFFS 0x1B

const float gyrolpfactor = 0.98;

float lpx = 0, lpy = 0, lpz = 0;
float angx = 0, angy = 0, angz = 0;
float prevangratex = 0, prevangratey = 0, prevangratez = 0;
float lpangratex = 0, lpangratey = 0, lpangratez = 0;
unsigned long lasttime = 0;
const int sampletime = 10;

#include <Wire.h>

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  Wire.beginTransmission(GYRO_ID);
  Wire.write(PWR_OFFS);
  Wire.write(0);
  Wire.endTransmission(true);
  Wire.beginTransmission(GYRO_ID);
  Wire.write(ACCEL_CONFIG_OFFS);
  Wire.write(0x08);    // +- 4g
  Wire.endTransmission(true);
  lasttime = millis();
}

void loop()
{
  if (millis() - lasttime < sampletime)
    return;
  lasttime = millis();
  char data[6];
  Wire.beginTransmission(GYRO_ID);
  Wire.write(GYRO_OFFS);
  Wire.endTransmission();
  Wire.requestFrom(GYRO_ID, 6);
  for (int i = 0; i < 6; i++)
    data[i] = Wire.read();
  int16_t angratex = 0;
  int16_t angratey = 0;
  int16_t angratez = 0;
  angratex |= data[0] << 8 | data[1];
  angratey |= data[2] << 8 | data[3];
  angratez |= data[4] << 8 | data[5];
  float fangratex = angratex * 250.f / 32768;
  float fangratey = angratey * 250.f / 32768;
  float fangratez = angratez * 250.f / 32768;
  lpangratex = lpangratex * gyrolpfactor + fangratex * (1.f - gyrolpfactor);
  lpangratey = lpangratey * gyrolpfactor + fangratey * (1.f - gyrolpfactor);
  lpangratez = lpangratez * gyrolpfactor + fangratez * (1.f - gyrolpfactor);
  //if (abs(fangratex) > 1.f)
    angx += (prevangratex + fangratex - lpangratex * 2.f) * (0.5f * sampletime / 1000.f);
  //if (abs(fangratey) > 1.f)
    angy += (prevangratey + fangratey - lpangratey * 2.f) * (0.5f * sampletime / 1000.f);
  //if (abs(fangratez) > 1.f)
    angz += (prevangratez + fangratez - lpangratez * 2.f) * (0.5f * sampletime / 1000.f);
  prevangratex = fangratex;
  prevangratey = fangratey;
  prevangratez = fangratez;
  
  Wire.beginTransmission(GYRO_ID);
  Wire.write(ACCEL_OFFS);
  Wire.endTransmission();
  Wire.requestFrom(GYRO_ID, 6);
  for (int i = 0; i < 6; i++)
    data[i] = Wire.read();
  int16_t accelx = 0;
  int16_t accely = 0;
  int16_t accelz = 0;
  accelx |= data[0] << 8 | data[1];
  accely |= data[2] << 8 | data[3];
  accelz |= data[4] << 8 | data[5];
  lpx = lpx * 0.9f + accelx / 8192.f * 0.1f;
  lpy = lpy * 0.9f + accely / 8192.f * 0.1f;
  lpz = lpz * 0.9f + accelz / 8192.f * 0.1f;
  Serial.print("Gyro:          ");
  Serial.print(angx);
  Serial.print(",\t");
  Serial.print(angy);
  Serial.print(",\t");
  Serial.println(angz);/*
  Serial.print("Accelerometer: ");
  Serial.print(lpx * 10);
  Serial.print(",\t");
  Serial.print(lpy * 10);
  Serial.print(",\t");
  Serial.println(lpz * 10);*/
}
