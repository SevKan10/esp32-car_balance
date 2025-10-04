#include "defines.h"
#include "globals.h"

// ĐỊNH NGHĨA CÁC BIẾN THỰC TẾ
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Kalman kalmanX; 
Kalman kalmanY;

double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;
double gyroXangle, gyroYangle; 
double compAngleX, compAngleY; 
double kalAngleX, kalAngleY; 

uint32_t timer;
uint8_t i2cData[14]; 

// **PID PARAMETERS ĐÃ CHỈNH**
float const Kp = 10.0;    // Giảm từ 8.0 → 3.0
float const Ki = 0.1;    // Tắt Ki
float const Kd = 2.0;   // Giảm từ 2.0 → 1.0

float integral = 0; 
float derivative;   
double dt;           
float output;       
float setPoint = 0.0, error, lastError = 0;  // Đổi setPoint từ 0.8 → 0.0
unsigned long delayTime;
float integralMax = 100.0;    
float integralMin = -100.0;
float outputMax = 255.0;      
float outputMin = -255.0;
float lastOutput = 0;  // THÊM MỚI

unsigned long lastStepTime = 0;
unsigned long stepPeriod = 2000;  
bool motorEnabled = false;
bool motorDirection = true;       

int16_t speed_M1 = 0;
int16_t speed_M2 = 0;
int8_t dir_M1 = 0;
int8_t dir_M2 = 0;

hw_timer_t * timer1 = NULL;
hw_timer_t * timer2 = NULL;

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("NOT FOUND OLED"));
    while (true);
  }

  /*----------------------------- KALMAN FILTER SETUP -----------------------------*/
#if ARDUINO >= 157
  Wire.setClock(400000UL); 
#else
  TWBR = ((F_CPU / 400000UL) - 16) / 2; 
#endif

  i2cData[0] = 7; 
  i2cData[1] = 0x00; 
  i2cData[2] = 0x00; 
  i2cData[3] = 0x00; 
  while (i2cWrite(0x19, i2cData, 4, false)); 
  while (i2cWrite(0x6B, 0x01, true)); 
  while (i2cRead(0x75, i2cData, 1));
  if (i2cData[0] != 0x68) { 
    Serial.print(F("Error reading sensor"));
    while (1);
  }
  delay(100); 

  /* Set kalman starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);

#ifdef RESTRICT_PITCH
  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  kalmanX.setAngle(roll);
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;
  timer = micros();

  /*----------------------------- MOTOR SETUP -----------------------------*/
  setupMotors();
  
  /*----------------------------- DISPLAY SETUP -----------------------------*/
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(23, 8);
  display.println("READY!");
  display.display();
  delay(3000);
  display.clearDisplay();
  
  Serial.println("=== BALANCE ROBOT STARTED ===");
  Serial.println("Kp: " + String(Kp) + " | Ki: " + String(Ki) + " | Kd: " + String(Kd));
  Serial.println("SetPoint: " + String(setPoint));
}

void loop() 
{
    /*----------------------------- DELTA TIME -----------------------------*/
    double dt = (double)(micros() - timer) / 1000000.0;
    timer = micros();
    
    if (dt < 0.001) dt = 0.001;
    if (dt > 0.1) dt = 0.1;

    /*----------------------------- KALMAN FILTER -----------------------------*/
    while (i2cRead(0x3B, i2cData, 14));
    accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
    accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
    accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
    tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
    gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
    gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
    gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);

#ifdef RESTRICT_PITCH
    double roll  = atan2(accY, accZ) * RAD_TO_DEG;
    double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else
    double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
    double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

    double gyroXrate = gyroX / 131.0;
    double gyroYrate = gyroY / 131.0;

#ifdef RESTRICT_PITCH
    if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
        kalmanX.setAngle(roll);
        compAngleX = roll;
        kalAngleX = roll;
        gyroXangle = roll;
    } else {
        kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt);
    }

    if (abs(kalAngleX) > 90)
        gyroYrate = -gyroYrate;
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
    if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
        kalmanY.setAngle(pitch);
        compAngleY = pitch;
        kalAngleY = pitch;
        gyroYangle = pitch;
    } else {
        kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
    }

    if (abs(kalAngleY) > 90)
        gyroXrate = -gyroXrate;
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt);
#endif

    gyroXangle += kalmanX.getRate() * dt;
    gyroYangle += kalmanY.getRate() * dt;

    compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll;
    compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

    if (gyroXangle < -180 || gyroXangle > 180)
        gyroXangle = kalAngleX;
    if (gyroYangle < -180 || gyroYangle > 180)
        gyroYangle = kalAngleY;

    /*----------------------------- PID & MOTOR CONTROL -----------------------------*/
    updatePID(kalAngleY, dt);
    controlMotorsFromPID(output);

    /*----------------------------- DISPLAY -----------------------------*/
    if (millis() - delayTime > 200) {  // Giảm frequency display
        display.setCursor(0, 10);
        display.print("Ang: ");
        display.print(kalAngleY, 1);
        display.setCursor(0, 30);
        display.print("PID: ");
        display.print(output, 0);
        display.setCursor(0, 50);
        display.print("M1: ");
        display.print(speed_M1);
        display.display();
        display.clearDisplay();
        delayTime = millis();
    }
}