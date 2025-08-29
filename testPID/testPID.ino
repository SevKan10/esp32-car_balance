/*=============================== OLED ===============================*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/*=============================== KALMAN FILTER ===============================*/
#include <Kalman.h> 
#define RESTRICT_PITCH 

Kalman kalmanX; 
Kalman kalmanY;
/* IMU Data */
double accX, accY, accZ;
double gyroX, gyroY, gyroZ;
int16_t tempRaw;
double gyroXangle, gyroYangle; 
double compAngleX, compAngleY; 
double kalAngleX, kalAngleY; 

uint32_t timer;
uint8_t i2cData[14]; 

/*=============================== STEP  ===============================*/
// *** A4988 ***
//  MS1   MS2   MS3   Microstep resolution
//  Low   Low   Low       Full step = 1 bước = 1.8'
//  High  Low   Low       1/2 step
//  Low   High  Low       1/4 step
// ** High  High  Low       1/8 step **
//  High  High  High      1/16 step

#define EN    12
#define STEP1 25  // Chân xung
#define DIR1  26  // Chân DIR1 dùng để xác định chiều quay của step
#define STEP2 14  // Chân xung
#define DIR2  27  // Chân DIR1 dùng để xác định chiều quay của step

/*=============================== VARIABLE ===============================*/
float const Kp = 0.5; // Hằng số tỉ lệ
float const Ki = 0.0; // Hằng số tích phân
float const Kd = 0.5; // Hằng số vi phân (đạo hàm)
float integral = 0; // Tích phân, vì biến này sẽ cộng dồn nên ban đầu khai báo sẽ là 0
float derivative;   // Đạo hàm (vi phân)
double dt;           // delta t: khoảng thời gian giữa 2 lần lấy mẫu hay cục thể là giữa 2 lần void loop()
float output;       // kết quả PID = tổng các kết quả từ quá trình tính: u(t) = Kp*error + Ki*(integral+= error*dt) + Kd*(error - lastError)/dt  
float setPoint = -2.95, error, lastError = 0; // setPoint là điểm mà chiếc xe được cân bằng
unsigned long delayTime;

void setup() 
{

  Serial.begin(115200);

/*----------------------------- OLED -----------------------------*/
  Wire.begin(); //SDA,SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {Serial.println(F("NOT FOUND OLED"));while (true);}

  // display.clearDisplay();
  // display.setTextSize(2);
  // display.setTextColor(SSD1306_WHITE);

  // display.clearDisplay();
  // display.setCursor(23, 8);
  // display.println("WELCOME");
  // display.display();
  // delay(3000);

  // display.clearDisplay();
  // display.setCursor(23, 8);
  // display.println("LOADING.");
  // display.display();
  // delay(3000);
  // display.clearDisplay();

/*----------------------------- KALMAN FILTER -----------------------------*/
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
  /* Set kalman and gyro starting angle */
  while (i2cRead(0x3B, i2cData, 6));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif
  kalmanX.setAngle(roll); // Set starting angle
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;
  timer = micros(); // Set timer bằng từ lúc thiết bị khởi động để tính delta time

/*----------------------------- STEP MOTOR -----------------------------*/
  pinMode(EN, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(DIR2, OUTPUT);
  digitalWrite(EN, 0); // Set 0 = LOW, khởi động

}

void loop() 
{
/*----------------------------- KALMAN FILTER -----------------------------*/
 /* Update all the values */
  while (i2cRead(0x3B, i2cData, 14));
  accX = (int16_t)((i2cData[0] << 8) | i2cData[1]);
  accY = (int16_t)((i2cData[2] << 8) | i2cData[3]);
  accZ = (int16_t)((i2cData[4] << 8) | i2cData[5]);
  tempRaw = (int16_t)((i2cData[6] << 8) | i2cData[7]);
  gyroX = (int16_t)((i2cData[8] << 8) | i2cData[9]);
  gyroY = (int16_t)((i2cData[10] << 8) | i2cData[11]);
  gyroZ = (int16_t)((i2cData[12] << 8) | i2cData[13]);;

  double dt = (double)(micros() - timer) / 1000000; // Calculate delta time
  timer = micros();
#ifdef RESTRICT_PITCH // Eq. 25 and 26
  double roll  = atan2(accY, accZ) * RAD_TO_DEG;
  double pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
#else // Eq. 28 and 29
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
#endif

  double gyroXrate = gyroX / 131.0; // Convert to deg/s
  double gyroYrate = gyroY / 131.0; // Convert to deg/s

#ifdef RESTRICT_PITCH
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) 
  {
    kalmanX.setAngle(roll);
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  } 
  else kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate; // Invert rate, so it fits the restriced accelerometer reading
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);
#else
  // This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90))
  {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  } else kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
#endif

  // gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter, dữ liệu thô
  // gyroYangle += gyroYrate * dt;
  gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate, dữ liệu của kalman giúp giảm trôi
  gyroYangle += kalmanY.getRate() * dt;

  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll; // Calculate the angle using a Complimentary filter
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;

  if (millis() - delayTime > 2)
  {
    /* Print Data */
    Serial.print("kalAngle Y: "); Serial.print(kalAngleY); Serial.print("\t");
    delayTime = millis();
  }

/*----------------------------- DELTA TIME -----------------------------*/
  // 1s = 1000ms = 1000000mrs
  dt = (double)(micros() - timer) / 1000000.0; // Ép kiểu micros() về số thực, do micros() sẽ trả về dạng unsigned long, cần ép kiểu để chung kiểu với dt dùng tính trong PID
  timer = micros(); // Gán lại cho timer để chuẩn bị cho lần lặp tính dt tiếp theo
  
  updatePID(kalAngleY, dt);
  driveStepper(output);
}

