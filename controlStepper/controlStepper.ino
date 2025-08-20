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
#define STEP2 14  // Chân xung
#define DIR2  27  // Chân DIR1 dùng để xác định chiều quay của step

int R = 2;
int D = 400;
String inputString = "";
bool stringComplete = false;

void setup() 
{
  Serial.begin(9600);
  Serial.println("R,D (Ex: 200,500):");

  pinMode(EN, OUTPUT);
  pinMode(STEP1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(STEP2, OUTPUT);
  pinMode(DIR2, OUTPUT);

  digitalWrite(EN, 0); // Set 0 = LOW, khởi động

}

void loop() 
{
  // Doc du lieu tu Serial
  while (Serial.available()) 
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {stringComplete = true;}
  }

  // Xu ly khi chuoi da hoan chinh
  if (stringComplete) 
  {
    inputString.trim();
    int commaIndex = inputString.indexOf(',');

    if (commaIndex != -1) 
    {
      String sR = inputString.substring(0, commaIndex); // Từ vị trí đầu tới ngay trước index
      String sD = inputString.substring(commaIndex + 1); // Từ ngay index + 1 đơn vị

      R = sR.toInt();
      D = sD.toInt();
    } 
    else {Serial.println("FAILED R,D");}

    inputString = "";
    stringComplete = false;
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

