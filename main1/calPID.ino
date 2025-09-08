void updatePID(float angle, double deltaTime) 
{ 
  if (deltaTime <= 0 || deltaTime > 1.0) {
    Serial.print("⚠️ BAD DELTATIME: ");
    Serial.println(deltaTime, 6);
    return;
  }
  
  error = setPoint - angle;
  
  // **THÊM DEADZONE** - Tránh rung nhỏ
  if (abs(error) < 0.3) {
    error = 0;
    integral = 0;  // Reset integral khi trong deadzone
  }
  
  float P = Kp * error;
  
  integral += error * deltaTime;
  if (integral > integralMax) integral = integralMax;
  if (integral < integralMin) integral = integralMin;
  float I = Ki * integral;
  
  derivative = (error - lastError) / deltaTime;
  float D = Kd * derivative;
  
  output = P + I + D;
  
  // **SMOOTH OUTPUT** - Tránh thay đổi đột ngột
  float outputChange = output - lastOutput;
  if (abs(outputChange) > 50) {
    if (outputChange > 0) {
      output = lastOutput + 50;
    } else {
      output = lastOutput - 50;
    }
  }
  lastOutput = output;
  
  // Giới hạn output cho motor speed
  if (output > MAX_SPEED) output = MAX_SPEED;
  if (output < -MAX_SPEED) output = -MAX_SPEED;
  
  // Safety stop
  if (abs(angle) > 30) {
    output = 0;
    digitalWrite(EN, HIGH);
    Serial.println("⚠️ SAFETY STOP - Angle too large!");
    return;
  } else {
    digitalWrite(EN, LOW);
  }
  
  // **DEBUG CHI TIẾT**
  Serial.print("Ang:"); Serial.print(angle, 2);
  Serial.print(" | Err:"); Serial.print(error, 2);
  Serial.print(" | P:"); Serial.print(P, 1);
  Serial.print(" | D:"); Serial.print(D, 1);
  Serial.print(" | PID:"); Serial.print(output, 1);
  
  if (abs(error) < 0.5) {
    Serial.print(" | ✅BALANCED");
  } else if (error > 0) {
    Serial.print(" | ↰LEAN_LEFT→PUSH_RIGHT");
  } else {
    Serial.print(" | ↱LEAN_RIGHT→PUSH_LEFT");
  }
  Serial.println();
  
  lastError = error;
}