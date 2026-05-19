#ifndef LINEFOLLOWER_H
#define LINEFOLLOWER_H

#include <Arduino.h>
#include <Wire.h>
#include "TwoWDController.h"

// --- TAMBAHAN BARU: Definisi Indeks Penderia ---
#define SENSOR_L2 0
#define SENSOR_L1 1
#define SENSOR_C  2
#define SENSOR_R1 3
#define SENSOR_R2 4
// -----------------------------------------------

class LineFollower {
  private:
    uint8_t i2cAddr;
    TwoWDController* robot; 
    
    float Kp, Kd, Ki;
    float baseSpeed, turnSpeed;
    float lastError;
    const long ALIGN_TICKS = 2252; 

  public:
    int L2, L1, C, R1, R2;
    int sumValue;

    LineFollower(uint8_t addr, TwoWDController* motorCtrl);

    bool begin();
    void readSensors();
    
    // --- TAMBAHAN BARU: Kaedah Get Sensor ---
    int getSensorState(int sensorIndex);
    // ----------------------------------------
    
    void followLineToJunction(int targetJunctions);
    void turnLeft90();
    void turnRight90();
    void turn180();
};

#endif