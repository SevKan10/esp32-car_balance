// *** A4988 ***
//  MS1   MS2   MS3   Microstep resolution
//  Low   Low   Low       Full step = 1 bước = 1.8'
//  High  Low   Low       1/2 step
//  Low   High  Low       1/4 step
// ** High  High  Low       1/8 step **
//  High  High  High      1/16 step

#define EN    12
#define STEP1 32  // Chân xung
#define DIR1  35  // Chân DIR1 dùng để xác định chiều quay của step
// #define STEP2 14  // Chân xung
// #define DIR2  27  // Chân DIR1 dùng để xác định chiều quay của step
void setup() 
{
  Serial.begin(9600);

  pinMode(EN, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  // pinMode(STEP2, OUTPUT);
  // pinMode(DIR2, OUTPUT);

  digitalWrite(EN, 0); // Set 0 = LOW, khởi động

}

void loop() 
{
  digitalWrite(DIR1, 0);
  // digitalWrite(DIR2, 0);
  controlStep(2, 400);

  delay(1000);

  digitalWrite(DIR1, 1);
  // digitalWrite(DIR2, 1);
  controlStep(1, 1000);

  delay(1000);
}
