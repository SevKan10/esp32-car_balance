#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long Time;
int kalAngleX = 90;
int kalAngleY = 181;
void setup() 
{
  Serial.begin(9600);
  Wire.begin(); //SDA,SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {Serial.println(F("NOT FOUND OLED"));while (true);}

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
  display.setCursor(0, 0);
  display.print("X: ");
  display.print(kalAngleX);
  display.display();

  display.setCursor(0, 30);
  display.print("Y: ");
  display.print(kalAngleY);
  display.display();
}