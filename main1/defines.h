#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Kalman.h>

/*Step Motor*/
#define EN   12
#define STEP1 25  
#define DIR1  33  
#define STEP2 14  
#define DIR2  27  

// Aliases cho motor.cpp
#define PIN_MOTOR_EN    EN
#define PIN_MOTOR1_STEP STEP1
#define PIN_MOTOR1_DIR  DIR1
#define PIN_MOTOR2_STEP STEP2
#define PIN_MOTOR2_DIR  DIR2

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
extern Adafruit_SSD1306 display;

#define RESTRICT_PITCH 

// Motor parameters
#define MICROSTEPPING 8         
#define MAX_ACCEL 7             
#define ZERO_SPEED 65535        
#define MAX_SPEED 400           

// Timer config
extern hw_timer_t * timer1;
extern hw_timer_t * timer2;