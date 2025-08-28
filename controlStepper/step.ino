// Do khi ở full step 1 bước là 1.8' = 1 xung, => xoay 1 vòng 360' = 200 xung
// Nhưng chân đã được set trên phần cứng là 1/8 step => 1 xung bây giờ sẽ quay được = 1.8*(1/8) = 0.225'; => để quay 1 vòng 360' cần 1600 xung.

void controlStep(int round, int delay)
{
  long pluse = (round * 360) / 0.225;
  for (long i = 0; i < pluse; i++)
  {
    digitalWrite(STEP1, 1); // Cuộn dây trên của step
    digitalWrite(STEP2, 1); // Cuộn dây trên của step
    delayMicroseconds(delay);
    digitalWrite(STEP1, 0); // Cuộn dây dưới của step
    digitalWrite(STEP2, 0); // Cuộn dây dưới của step
    delayMicroseconds(delay);
  }

}