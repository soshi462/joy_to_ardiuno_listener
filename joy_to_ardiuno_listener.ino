#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ---- 定数の設定 ----
#define MAX_SIGNAL 1700  // 最大のPWM信号幅（マイクロ秒単位）
#define MIN_SIGNAL 1300  // 最小のPWM信号幅（マイクロ秒単位）
#define STOP_SIGNAL 1500 // 停止時のPWM信号幅（マイクロ秒単位）
#define NUM_CHANNELS 4   // 使用するモーターの数（メカナムホイール用）

// Adafruit PWM Servo Driverのインスタンスを作成
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// 各モーターのチャネル定義
const int motorChannels[NUM_CHANNELS] = {0, 1, 2, 3}; // 前左、前右、後左、後右

void setup() {
  Serial.begin(115200); // シリアル通信のボーレート修正

  pwm.begin();
  pwm.setPWMFreq(50); // PCA9685の周波数設定

  for (int i = 0; i < NUM_CHANNELS; i++) {
    int pulse = map(STOP_SIGNAL, MIN_SIGNAL, MAX_SIGNAL, 102, 512);
    pwm.setPWM(motorChannels[i], 0, pulse);
  }

  pinMode(13, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    int commaIndex = input.indexOf(',');
    if (commaIndex > 0) {
      int x = input.substring(0, commaIndex).toInt();
      int y = input.substring(commaIndex + 1).toInt();

      if (x >= -100 && x <= 100 && y >= -100 && y <= 100) {
        controlMotors(x, y);
      } else {
        Serial.println("Invalid input. x and y must be between -100 and 100.");
      }
    } else {
      Serial.println("Invalid input format. Use x,y (e.g., 50,50).");
    }
  }
}

void controlMotors(int x, int y) {
  int frontLeftSpeed = y + x;
  int frontRightSpeed = -y + x;
  int rearLeftSpeed = y - x;
  int rearRightSpeed = -y - x;

  setMotorSpeed(0, frontLeftSpeed);
  setMotorSpeed(1, frontRightSpeed);
  setMotorSpeed(2, rearLeftSpeed);
  setMotorSpeed(3, rearRightSpeed);

  digitalWrite(13, x >= 0 ? HIGH : LOW);

  Serial.print("Front Left: ");
  Serial.print(frontLeftSpeed);
  Serial.print(", Front Right: ");
  Serial.print(frontRightSpeed);
  Serial.print(", Rear Left: ");
  Serial.print(rearLeftSpeed);
  Serial.print(", Rear Right: ");
  Serial.println(rearRightSpeed);
}

void setMotorSpeed(int motor, int speed) {
  speed = constrain(speed, -100, 100);
  int pulseWidth = map(speed, -100, 100, MIN_SIGNAL, MAX_SIGNAL);
  int pulse = map(pulseWidth, MIN_SIGNAL, MAX_SIGNAL, 102, 512);
  pwm.setPWM(motorChannels[motor], 0, pulse);
}
