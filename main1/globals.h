#include "defines.h"

/*Kalman Filter*/
extern Kalman kalmanX; 
extern Kalman kalmanY;

/* IMU Data */
extern double accX, accY, accZ;
extern double gyroX, gyroY, gyroZ;
extern int16_t tempRaw;
extern double gyroXangle, gyroYangle; 
extern double compAngleX, compAngleY; 
extern double kalAngleX, kalAngleY; 

extern uint32_t timer;
extern uint8_t i2cData[14]; 

/*Variable*/
extern float const Kp;
extern float const Ki;
extern float const Kd;
extern float integral; 
extern float derivative;   
extern double dt;           
extern float output;       
extern float setPoint, error, lastError; 
extern unsigned long delayTime;
extern float integralMax;    
extern float integralMin;
extern float outputMax;      
extern float outputMin;
extern float lastOutput;  // THÊM MỚI

extern unsigned long lastStepTime;
extern unsigned long stepPeriod;  
extern bool motorEnabled;
extern bool motorDirection;       

// Motor control variables
extern int16_t speed_M1;
extern int16_t speed_M2;
extern int8_t dir_M1;
extern int8_t dir_M2;

// Function declarations
void setMotorSpeedM1(int16_t tspeed);
void setMotorSpeedM2(int16_t tspeed);
void setupMotors();
void controlMotorsFromPID(float pidOutput);
uint8_t i2cWrite(uint8_t registerAddress, uint8_t data, bool sendStop);
uint8_t i2cWrite(uint8_t registerAddress, uint8_t *data, uint8_t length, bool sendStop);
uint8_t i2cRead(uint8_t registerAddress, uint8_t *data, uint8_t nbytes);
void updatePID(float angle, double deltaTime);