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

void driveStepper(float pidOut) 
{
  // Giới hạn lại để không quá nhanh
  if (pidOut > 500) pidOut = 500;
  if (pidOut < -500) pidOut = -500;

  // Xác định chiều quay
  if (pidOut > 0) 
  {
    digitalWrite(DIR1, 0);
    digitalWrite(DIR2, 1);
  } 
  else if (pidOut < 0)  
  {
    digitalWrite(DIR1, 1);
    digitalWrite(DIR2, 0);
  }

}

