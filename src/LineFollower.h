#ifndef LINEFOLLOWER_H
#define LINEFOLLOWER_H

#include <Arduino.h>
#include <Wire.h>
#include "TwoWDController.h"

class LineFollower {
  private:
    uint8_t i2cAddr;
    TwoWDController* robot; // Pointer kepada controller motor
    
    // Pembolehubah PID & Parameter
    float Kp;
    float Kd;
    float Ki;
    float baseSpeed;
    float turnSpeed;
    float lastError;
    const long ALIGN_TICKS = 2252; // Jarak 120mm

  public:
    int L2, L1, C, R1, R2;
    int sumValue;

    // Constructor
    LineFollower(uint8_t addr, TwoWDController* motorCtrl);

    // Fungsi Asas
    bool begin();
    void readSensors();
    
    // Fungsi Misi (High-Level API)
    void followLineToJunction(int targetJunctions);
    void turnLeft90();
    void turnRight90();
    void turn180();
};

#endif