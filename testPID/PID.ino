void updatePID(float angle, double deltaTime)
{ 
  error = setPoint - angle;
  integral+= error*deltaTime; // Tính tích phân
  derivative = (error - lastError)/deltaTime; // Tính vi phân
  output =  Kp*error + Ki*integral + Kd*derivative; // PID
  lastError = error;

  // Giới hạn output để tránh quá lớn hoặc quá nhỏ
  if (output > 500) output = 500;
  if (output < -500) output = -500;
  Serial.print("PID: "); Serial.println(output);
  // controlStep(output);
  // driveStepper(output);
}