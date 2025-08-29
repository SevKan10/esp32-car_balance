// void controlStep(float out)
// {
//   if (out > 0) 
//   {
//     digitalWrite(DIR1, 0);
//     digitalWrite(DIR2, 1);
//   } 
//   else if (out < 0) 
//   {
//     digitalWrite(DIR1, 1);
//     digitalWrite(DIR2, 0);
//   }
//   else if (out == 0) {digitalWrite(STEP1, 0); digitalWrite(STEP2, 0);}
//   for (int i = 0; i < output; i++)
//   {
//     digitalWrite(STEP1, 1); // Cuộn dây trên của step
//     digitalWrite(STEP2, 1); // Cuộn dây trên của step
//     delayMicroseconds(400);
//     digitalWrite(STEP1, 0); // Cuộn dây dưới của step
//     digitalWrite(STEP2, 0); // Cuộn dây dưới của step
//     delayMicroseconds(400);
//   }
// }

void driveStepper(float pidOut) {
  // Giới hạn lại để không quá nhanh
  if (pidOut > 500) pidOut = 500;
  if (pidOut < -500) pidOut = -500;

  // Xác định chiều quay
  if (pidOut > 0) {
    digitalWrite(DIR1, 0);
    digitalWrite(DIR2, 1);
  } else {
    digitalWrite(DIR1, 1);
    digitalWrite(DIR2, 0);
  }

  // Chuyển đổi PID -> tần số (steps/s)
  float freq = fabs(pidOut) * 3;  // hằng số 3 là hệ số G, bạn chỉnh thử nghiệm
  if (freq < 50) return;          // dưới 50 steps/s thì bỏ qua để tránh rung

  // Tính chu kỳ xung
  unsigned long period = 1000000.0 / freq; // microseconds per step

  // Phát 1 xung STEP
  digitalWrite(STEP1, HIGH);
  digitalWrite(STEP2, HIGH);
  delayMicroseconds(100);  // xung tối thiểu 2us cho A4988
  digitalWrite(STEP1, LOW);
  digitalWrite(STEP2, LOW);
  delayMicroseconds(period - 100);
}

