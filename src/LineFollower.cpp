#include "LineFollower.h"

// Constructor
LineFollower::LineFollower(uint8_t addr, TwoWDController* motorCtrl) {
  this->i2cAddr = addr;
  this->robot = motorCtrl;
  
  this->Kp = 0.003;
  this->Kd = 0.0;
  this->Ki = 0.0;
  this->baseSpeed = 0.15;
  this->turnSpeed = 0.12; 
  this->lastError = 0;
}

// Memulakan Bas I2C Penderia
bool LineFollower::begin() {
  Wire.beginTransmission(i2cAddr);
  Wire.write(0xFF); 
  return (Wire.endTransmission() == 0);
}

// Membaca Data Penderia
void LineFollower::readSensors() {
  Wire.requestFrom((int)i2cAddr, 1);
  if (Wire.available()) {
    uint8_t rawData = Wire.read();
    L2 = !(rawData & 0x01);         
    L1 = !((rawData >> 1) & 0x01);  
    C  = !((rawData >> 2) & 0x01);  
    R1 = !((rawData >> 3) & 0x01);  
    R2 = !((rawData >> 4) & 0x01);  
    sumValue = L2 + L1 + C + R1 + R2;
  }
}

// FUNGSI 1: Ikut Garisan
void LineFollower::followLineToJunction(int targetJunctions) {
  int count = 0;
  unsigned long cooldown = 0;

  Serial.print("Gerak: "); Serial.println(targetJunctions);

  while (count < targetJunctions) {
    unsigned long currentMillis = millis();
    readSensors();

    if (currentMillis - cooldown > 500) {
      if ((L2 == 1 && R2 == 1) || (L2 == 1 && L1 == 1 && C == 1) || (R2 == 1 && R1 == 1 && C == 1)) {
        count++;
        cooldown = currentMillis;
        Serial.print("Simpang: "); Serial.println(count);

        if (count >= targetJunctions) break; 
      }
    }

    float error = 0;
    if (sumValue != 0) {
      error = ((-20.0 * L2) + (-10.0 * L1) + (0 * C) + (10.0 * R1) + (20.0 * R2)) / sumValue;
      lastError = error; 
    } else {
      error = (lastError < 0) ? -30.0 : ((lastError > 0) ? 30.0 : 0);
    }

    float turnAmount = (Kp * error) + (Kd * (error - lastError));
    float leftSpeed = constrain(baseSpeed + turnAmount, 0.0, 0.4);
    float rightSpeed = constrain(baseSpeed - turnAmount, 0.0, 0.4);

    robot->drive(leftSpeed, rightSpeed);
    delay(10); 
  }

  long startEnc = robot->getLeftPosition();
  robot->drive(0.15, 0.15); 
  
  while (abs(robot->getLeftPosition() - startEnc) < ALIGN_TICKS) {
    delay(5);
  }
  
  robot->stop();
  delay(300); 
}

// FUNGSI 2A: Kiri
void LineFollower::turnLeft90() {
  Serial.println("Kiri...");
  robot->drive(-turnSpeed, turnSpeed); 
  delay(300); 
  while (true) {
    readSensors();
    if (L1 == 1) { robot->stop(); break; }
    delay(5);
  }
  delay(300);
}

// FUNGSI 2B: Kanan
void LineFollower::turnRight90() {
  Serial.println("Kanan...");
  robot->drive(turnSpeed, -turnSpeed); 
  delay(300); 
  while (true) {
    readSensors();
    if (R1 == 1) { robot->stop(); break; }
    delay(5);
  }
  delay(300);
}

// FUNGSI 2C: U-Turn
void LineFollower::turn180() {
  Serial.println("U-Turn...");
  robot->drive(turnSpeed, -turnSpeed); 
  delay(700); 
  while (true) {
    readSensors();
    if (R1 == 1) { robot->stop(); break; }
    delay(5);
  }
  delay(300);
}