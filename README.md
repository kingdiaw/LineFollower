# LineFollower Library for ESP32

Library aras tinggi (*high-level*) untuk memudahkan kawalan pergerakan robot *line follower* berasaskan rangkaian grid pada platform ESP32. Library ini direka khusus untuk mengawal pergerakan robot secara berurutan (*sequential / blocking*) bagi memudahkan pembinaan laluan kompleks tanpa memerlukan logik *State Machine* yang rumit di dalam sketch utama.

Library ini bergantung sepenuhnya kepada library pembantu **TwoWDController** untuk kawalan motor berasaskan unit halaju meter per saat (m/s).

---

## 🚀 Ciri-Ciri Utama

- **API Berurutan (Blocking API):** Membolehkan anda menyusun arahan navigasi seperti blok Lego (Contoh: Jalan 2 simpang ➡️ Belok Kiri ➡️ Jalan 1 simpang).
- **Sokongan Dual PCF8574:** Menggunakan dua buah cip pengembang I/O pada bas I2C yang sama secara harmoni (0x20 untuk motor, 0x21 untuk sensor).
- **Logik Direct I2C (Wire):** Membaca data penderia secara terus melalui operasi bitwise tanpa bergantung kepada library PCF8574 luaran, menghapuskan isu konflik *multiple instances*.
- **Kawalan Kalis Inersia (Early Brake):** Menggunakan strategi rujukan sensor `R1` / `L1` untuk memotong kuasa motor lebih awal bagi mengatasi momentum lajak fizikal robot semasa membelok 90 darjah.
- **Tuning Stabil P-Only:** Dinala khas untuk sensor digital bagi mengelakkan kesan sentakan kasar (*Derivative Kick*).

---

## 🛠️ Konfigurasi Perkakasan (Hardware Setup)

Sistem ini berkongsi bas I2C standard ESP32 pada pin **SDA (GPIO 21)** dan **SCL (GPIO 22)**.

### 1. Modul Driver Motor (Alamat I2C: `0x20`)
*Dikawal secara dalaman oleh `TwoWDController`*
- **P2:** Motor Kiri IN1
- **P3:** Motor Kiri IN2
- **P0:** Motor Kanan IN1
- **P1:** Motor Kanan IN2

### 2. Modul Sensor Garisan (Alamat I2C: `0x21`)
*Dikawal secara langsung oleh `LineFollower` (Aktif LOW: Hitam = 0, Putih = 1)*
- **P0:** Sensor Kiri Luar (L2) — *Pemberat: -20*
- **P1:** Sensor Kiri Dalam (L1) — *Pemberat: -10*
- **P2:** Sensor Tengah (C) — *Pemberat: 0*
- **P3:** Sensor Kanan Dalam (R1) — *Pemberat: 10*
- **P4:** Sensor Kanan Luar (R2) — *Pemberat: 20*

### 3. Sambungan GPIO ESP32 Langsung
- **GPIO 4 & 2:** Enable Kiri & Kanan (PWM Speed)
- **GPIO 18 & 19:** Encoder Motor Kiri (Channel A & B)
- **GPIO 25 & 13:** Encoder Motor Kanan (Channel A & B)

---

## 📦 Cara Pemasangan (Installation)

1. Muat turun (*Download*) repositori ini dalam bentuk fail ZIP.
2. Ekstrak dan namakan semula folder tersebut kepada `LineFollower`.
3. Pindahkan folder `LineFollower` ke dalam direktori library Arduino anda:
   `Documents/Arduino/libraries/`
4. Pastikan library `TwoWDController` juga dipasang di dalam direktori yang sama.
5. Mulakan semula (Restart) Arduino IDE anda.

---

## 📖 Panduan API (API Reference)

### `LineFollower(uint8_t addr, TwoWDController* motorCtrl)`
*Constructor* untuk mengisytiharkan objek pembaca sensor. Menerima alamat I2C sensor dan rujukan (*pointer*) objek pembantu motor.

```

```text
README.md successfully generated.

```cpp
LineFollower lineRobot(0x21, &robot);

```

### `bool begin()`

Memulakan komunikasi I2C dengan cip PCF8574 sensor dan menetapkan pin sebagai input. Mengembalikan nilai `true` jika berjaya.

```cpp
lineRobot.begin();

```

### `void followLineToJunction(int targetJunctions)`

Mengarah robot mengekori garisan menggunakan algoritma PID (P-Only) dan mengira jumlah persimpangan. Robot akan bergerak ke hadapan sejauh 120mm (*alignment fasa*) secara automatik pada simpang sasaran sebelum berhenti tepat di atas titik tengah persimpangan.

```cpp
lineRobot.followLineToJunction(2); // Jalan dan berhenti di simpang ke-2

```

### `void turnLeft90()`

Memusingkan robot ke kiri sebanyak 90 darjah pada paksinya (*pivot turn*). Menggunakan sensor `L1` sebagai mod hentian awal bagi memotong kesan inersia lajak.

```cpp
lineRobot.turnLeft90();

```

### `void turnRight90()`

Memusingkan robot ke kanan sebanyak 90 darjah pada paksinya. Menggunakan sensor `R1` sebagai mod hentian awal.

```cpp
lineRobot.turnRight90();

```

### `void turn180()`

Membuat pusingan U (*U-Turn*) 180 darjah mengikut arah jam sehingga menemui semula garisan lurus asal.

```cpp
lineRobot.turn180();

```

---

## 📝 Contoh Kod Penggunaan (Example)

Fail contoh ini boleh didapati terus di Arduino IDE melalui menu `File -> Examples -> LineFollower -> BasicRouting`.

```cpp
#include <Wire.h>
#include <TwoWDController.h>
#include <LineFollower.h>

// Ambil perhatian: pastikan parameter fizikal robot sepadan
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
  
  // Sediakan parameter fizikal untuk TwoWDController (Diameter, Resolusi, Interval, Gear)
  robot.setRobotParameters(0.067, 3950, 0.033, 1);
  robot.begin();
  robot.startAutoUpdate(0.033); 
  
  Serial.println("Sistem Sedia. Memulakan misi dalam 2 saat...");
  delay(2000); 

  // ===================================================
  // JUJUKAN MISI UTAMA (SEQUENTIAL ROUTING)
  // ===================================================
  
  // 1. Ikut garisan dan berhenti tepat pada simpang ke-2
  lineRobot.followLineToJunction(2);
  
  // 2. Belok kiri 90 darjah tepat di atas gandar simpang
  lineRobot.turnLeft90();
  
  // 3. Sambung gerak lurus untuk 1 simpang lagi
  lineRobot.followLineToJunction(1);
  
  // 4. Buat pusingan U-turn
  lineRobot.turn180();

  Serial.println("Misi Selesai. Robot berhenti sepenuhnya.");
}

void loop() {
  // Kekal kosong kerana semua arahan navigasi telah diselesaikan secara berurutan
}

```

---

## ⚙️ Nota Talaan & Konfigurasi (*Tuning Notes*)

Jika anda ingin mengubah tetapan prestasi dinamik robot, anda boleh menyunting pembolehubah di dalam *constructor* fail `LineFollower.cpp`:

* **`Kp = 0.003`:** Nilai optimum kawalan Proportional. Jika robot terkeluar dari garisan semasa kelajuan ditambah, naikkan nilai ini sedikit demi sedikit (Contoh: `0.004`).
* **`Kd = 0.0`:** Kekalkan pada `0.0` untuk penderia digital bagi menghalang kesan sentakan melompat (*jittery*).
* **`baseSpeed = 0.15` (m/s):** Kelajuan linear pergerakan terus.
* **`turnSpeed = 0.12` (m/s):** Kelajuan putaran paksi semasa membelok bagi memastikan impak inersia kekal rendah.
* **`ALIGN_TICKS = 2252`:** Mewakili jarak fizikal pergerakan alignment sebanyak **120mm** (telah dikurangkan 20mm daripada jarak fizikal asal 140mm bagi mengimbangi lajak brek roda).

---

### 📄 Lesen

Projek ini dilesenkan di bawah Lesen MIT. Sila gunakan untuk tujuan pertandingan, komuniti, dan pembelajaran robotik!
"""


