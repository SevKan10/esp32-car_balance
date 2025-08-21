// *** A4988 ***
//  MS1   MS2   MS3   Microstep resolution
//  Low   Low   Low       Full step = 1 bước = 1.8'
//  High  Low   Low       1/2 step
//  Low   High  Low       1/4 step
// ** High  High  Low       1/8 step **
//  High  High  High      1/16 step

#define EN    12
#define STEP1 32  // Chân xung
#define DIR1  25  // Chân DIR1 dùng để xác định chiều quay của step
#define STEP2 14  // Chân xung
#define DIR2  27  // Chân DIR1 dùng để xác định chiều quay của step

int R = 2;
int D = 400;
String inputParameter ="";

void setup() 
{
  Serial.begin(9600);
  Serial.println("Fomat R,D (Ex: 200,500):");

  pinMode(EN, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(DIR2, OUTPUT);

  digitalWrite(EN, 0); // Set 0 = LOW, khởi động

}

void loop() 
{
  if (Serial.available())
  {
    int commandIndex;
    inputParameter = Serial.readStringUntil('\n');
    commandIndex = inputParameter.indexOf(',');

    if (commandIndex > 0) {R = inputParameter.substring(0, commandIndex).toInt(); D = inputParameter.substring(commandIndex + 1).toInt();}
    else {Serial.println("FAILED R,D");}
  }

  digitalWrite(DIR1, 0);
  digitalWrite(DIR2, 0);
  controlStep(R, D);

  delay(1000);

  digitalWrite(DIR1, 1);
  digitalWrite(DIR2, 1);
  controlStep(R, D);

  delay(1000);

}

