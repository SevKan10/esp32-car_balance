void updatePID(float angle, double deltaTime)
{ 
  if (deltaTime <= 0 || deltaTime > 1.0) 
  {
    Serial.print("BAD DELTATIME: ");
    Serial.println(deltaTime, 6);
    return;
  }
  
  error = setPoint - angle;
  
  float P = Kp * error;
  
  integral += error * deltaTime;
  if (integral > integralMax) integral = integralMax;
  if (integral < integralMin) integral = integralMin;
  float I = Ki * integral;
  
  derivative = (error - lastError) / deltaTime;
  float D = Kd * derivative;
  
  output = P + I + D;
  
  // Giới hạn output cho motor speed
  if (output > MAX_SPEED) output = MAX_SPEED;
  if (output < -MAX_SPEED) output = -MAX_SPEED;
  
  // Safety stop
  if (abs(angle) > 30) 
  {
    output = 0;
    digitalWrite(EN, HIGH);  // Sử dụng EN thay vì PIN_MOTOR_EN
    Serial.println("SAFETY STOP");
    return;
  } 
  else {digitalWrite(EN, LOW);}
  
  lastError = error;
}