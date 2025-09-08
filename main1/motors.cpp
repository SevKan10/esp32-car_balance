#include "defines.h"
#include "globals.h"

// Interrupt handlers cho timer
void IRAM_ATTR onTimer1() {digitalWrite(PIN_MOTOR1_STEP, !digitalRead(PIN_MOTOR1_STEP));}

void IRAM_ATTR onTimer2() {digitalWrite(PIN_MOTOR2_STEP, !digitalRead(PIN_MOTOR2_STEP));}

void setupMotors() 
{
  // Setup pins
  pinMode(PIN_MOTOR_EN, OUTPUT);
  pinMode(PIN_MOTOR1_DIR, OUTPUT);
  pinMode(PIN_MOTOR1_STEP, OUTPUT);
  pinMode(PIN_MOTOR2_DIR, OUTPUT);
  pinMode(PIN_MOTOR2_STEP, OUTPUT);
  
  digitalWrite(PIN_MOTOR_EN, LOW);  // Enable motors
  
  // Setup timers (ESP32 v3.0 API)
  timer1 = timerBegin(1000000);  // 1MHz frequency
  timerAttachInterrupt(timer1, &onTimer1);
  timerAlarm(timer1, ZERO_SPEED, true, 0);
  
  timer2 = timerBegin(1000000);  // 1MHz frequency  
  timerAttachInterrupt(timer2, &onTimer2);
  timerAlarm(timer2, ZERO_SPEED, true, 0);
}

void setMotorSpeedM1(int16_t tspeed) 
{
  long timer_period;
  int16_t speed;

  // WE LIMIT MAX ACCELERATION
  if ((speed_M1 - tspeed) > MAX_ACCEL)
    speed_M1 -= MAX_ACCEL;
  else if ((speed_M1 - tspeed) < -MAX_ACCEL)
    speed_M1 += MAX_ACCEL;
  else
    speed_M1 = tspeed;

  // Convert control speed to motor speed (1/8 microstepping)
  speed = speed_M1 * 25;

  if (speed == 0) 
  {
    timer_period = ZERO_SPEED;
    dir_M1 = 0;
    timerStop(timer1);
  }
  else if (speed > 0) 
  {
    timer_period = 1000000 / speed; // 1MHz timer
    dir_M1 = 1;
    digitalWrite(PIN_MOTOR1_DIR, HIGH);
    timerAlarm(timer1, timer_period, true, 0);
    timerStart(timer1);
  }
  else 
  {
    timer_period = 1000000 / -speed;
    dir_M1 = -1;
    digitalWrite(PIN_MOTOR1_DIR, LOW);
    timerAlarm(timer1, timer_period, true, 0);
    timerStart(timer1);
  }
  
  if (timer_period > ZERO_SPEED)
    timer_period = ZERO_SPEED;
}

void setMotorSpeedM2(int16_t tspeed) 
{
  long timer_period;
  int16_t speed;

  // WE LIMIT MAX ACCELERATION  
  if ((speed_M2 - tspeed) > MAX_ACCEL)
    speed_M2 -= MAX_ACCEL;
  else if ((speed_M2 - tspeed) < -MAX_ACCEL)
    speed_M2 += MAX_ACCEL;
  else
    speed_M2 = tspeed;

  speed = speed_M2 * 25;

  if (speed == 0) 
  {
    timer_period = ZERO_SPEED;
    dir_M2 = 0;
    timerStop(timer2);
  }
  else if (speed > 0) 
  {
    timer_period = 1000000 / speed;
    dir_M2 = 1;
    // CHÚ Ý: Motor 2 ngược hướng do lắp đối xứng
    digitalWrite(PIN_MOTOR2_DIR, LOW);   // Ngược với Motor 1
    timerAlarm(timer2, timer_period, true, 0);
    timerStart(timer2);
  }
  else 
  {
    timer_period = 1000000 / -speed;
    dir_M2 = -1;
    digitalWrite(PIN_MOTOR2_DIR, HIGH);  // Ngược với Motor 1
    timerAlarm(timer2, timer_period, true, 0);
    timerStart(timer2);
  }
  
  if (timer_period > ZERO_SPEED)
    timer_period = ZERO_SPEED;
}

// Hàm điều khiển từ PID
void controlMotorsFromPID(float pidOutput) 
{
  // Chuyển đổi PID output thành motor speed
  int16_t motorSpeed = -(int16_t)pidOutput;
  
  // Giới hạn tốc độ
  if (motorSpeed > MAX_SPEED) motorSpeed = MAX_SPEED;
  if (motorSpeed < -MAX_SPEED) motorSpeed = -MAX_SPEED;
  
  // Set speed cho cả 2 motor (cùng tốc độ để đi thẳng)
  setMotorSpeedM1(motorSpeed);
  setMotorSpeedM2(motorSpeed);
  
  Serial.print("PID: "); Serial.print(pidOutput);
  Serial.print(" | M1: "); Serial.print(speed_M1);
  Serial.print(" | M2: "); Serial.println(speed_M2);
}