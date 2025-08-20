#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BAT 34  // chân check BAT ESP32

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() 
{
  Serial.begin(9600);
  Wire.begin(23,22); // SDA,SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {Serial.println(F("NOT FOUND OLED"));while (true);}

  pinMode(BAT, INPUT);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  display.clearDisplay();
  display.setCursor(23, 8);
  display.println("WELCOME");
  display.display();
  delay(3000);
  display.clearDisplay();
}

void loop() 
{
  float  vOut = (analogRead(BAT) * 3.3) / 4095.0; // mức tối đa của chân GPIO của ESP là 3.3V ~ 4095
  float vIn = vOut / (758.6 / (2064 + 758.6));
  int percentBattery = (vIn / 12.6) * 100; // 12.6 là mức no pin của 18560 mỗi viên 4.2x3

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(String(percentBattery) + " %");
  display.display();
}
