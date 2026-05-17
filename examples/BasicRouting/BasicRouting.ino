/*
  BasicRouting.ino
  Contoh penggunaan library LineFollower untuk navigasi grid.
  
  Misi:
  1. Jalan lurus melepasi 1 simpang.
  2. Berhenti di simpang ke-2.
  3. Belok Kiri 90 darjah.
*/

#include <Wire.h>
#include <TwoWDController.h>
#include <LineFollower.h>

TwoWDController robot(0x20, 4, 2, P2, P3, P0, P1, 18, 19, 25, 13);
LineFollower lineRobot(0x21, &robot); 

void setup() {
  Serial.begin(115200);
  Wire.begin(); 
  delay(500); 
  
  if (!lineRobot.begin()) {
    Serial.println("Ralat: Penderia gagal dihubungi!");
    while (1) delay(100); 
  }
  
  robot.setRobotParameters(0.067, 3950, 0.033, 1);
  robot.begin();
  robot.startAutoUpdate(0.033); 
  
  Serial.println("Sistem Sedia. Memulakan misi dalam 2 saat...");
  delay(2000); 

  // --- Arahan Misi Berurutan ---
  lineRobot.followLineToJunction(2);
  lineRobot.turnLeft90();
  
  Serial.println("Misi Berjaya. Robot dimatikan.");
}

void loop() {
  // Kosong - Misi telah selesai di setup()
}