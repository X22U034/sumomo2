// Maker Sumo の NOTE 定義をコピー (CytronMakerSumo.h から)
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 92
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3733
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978





// Minizade Robot Controller Board と Maker Sumo の連携アダプター
// Maker Sumo ライブラリの関数名を模倣し、Minizade のピンアサインで動作するように実装します。

#include <Arduino.h>

// Minizade Robot Controller Board のピンアサイン
#define MINIZADE_MOTOR_L_PWM1 5   // 左モーターPWM1
#define MINIZADE_MOTOR_L_PWM2 6   // 左モーターPWM2
#define MINIZADE_MOTOR_R_PWM1 10  // 右モーターPWM1
#define MINIZADE_MOTOR_R_PWM2 9   // 右モーターPWM2
#define MINIZADE_BUZZER 7         // ブザー
#define MINIZADE_START_MODULE 13  // スタートモジュール
#define MINIZADE_IR_RECEIVER 4    // 赤外線受信機

// Maker Sumo のモーターサイド定義を模倣
#define MOTOR_L 0
#define MOTOR_R 1

class MinizadeMakerSumoAdapter
{
public:
  MinizadeMakerSumoAdapter();
  void begin(void);
  void stop(void);
  void setMotorSpeed(int side, int speed);
  void moveForward(int speed);
  void moveBackward(int speed);
  void turnLeft(int speed);
  void turnRight(int speed);
  void playMelody(const int *pitch, const int *duration, int length);

  // Maker Sumo のセンサーやスイッチ機能は、Minizade のピンに合わせて調整が必要です。
  // ここでは基本的なモーター制御とブザーのみを実装します。
  // センサーやDIPスイッチ、バッテリーセンスはMinizadeのピンに直接アクセスするか、
  // 外部センサーを接続して別途コードを記述してください。
  int readDipSwitch(void) { return 0; } // 仮実装
  float readBatteryVoltage(void) { return 0.0; } // 仮実装
  void calibrateEdgeSensors(void) {} // 仮実装
  int readEdgeSensorThreshold(int side) { return 0; } // 仮実装
  bool isEdgeDetected(int side) { return false; } // 仮実装
};

MinizadeMakerSumoAdapter MakerSumo; // Maker Sumo ライブラリのインスタンス名を模倣

MinizadeMakerSumoAdapter::MinizadeMakerSumoAdapter()
{
  // コンストラクタ
}

void MinizadeMakerSumoAdapter::begin(void)
{
  pinMode(MINIZADE_MOTOR_L_PWM1, OUTPUT);
  pinMode(MINIZADE_MOTOR_L_PWM2, OUTPUT);
  pinMode(MINIZADE_MOTOR_R_PWM1, OUTPUT);
  pinMode(MINIZADE_MOTOR_R_PWM2, OUTPUT);
  pinMode(MINIZADE_BUZZER, OUTPUT);
  pinMode(MINIZADE_START_MODULE, INPUT_PULLUP); // スタートモジュールはプルアップ入力
  pinMode(MINIZADE_IR_RECEIVER, INPUT); // 赤外線受信機は入力

  // 初期状態ではモーターを停止
  stop();
}

void MinizadeMakerSumoAdapter::stop(void)
{
  digitalWrite(MINIZADE_MOTOR_L_PWM1, LOW);
  digitalWrite(MINIZADE_MOTOR_L_PWM2, LOW);
  digitalWrite(MINIZADE_MOTOR_R_PWM1, LOW);
  digitalWrite(MINIZADE_MOTOR_R_PWM2, LOW);
}

void MinizadeMakerSumoAdapter::setMotorSpeed(int side, int speed)
{
  // speed: -255 (後退) から 255 (前進)
  // Minizade のモーター制御ピンに合わせて調整
  if (side == MOTOR_L) {
    if (speed > 0) { // 前進
      analogWrite(MINIZADE_MOTOR_L_PWM1, speed);
      digitalWrite(MINIZADE_MOTOR_L_PWM2, LOW);
    } else if (speed < 0) { // 後退
      analogWrite(MINIZADE_MOTOR_L_PWM2, abs(speed));
      digitalWrite(MINIZADE_MOTOR_L_PWM1, LOW);
    } else { // 停止
      digitalWrite(MINIZADE_MOTOR_L_PWM1, LOW);
      digitalWrite(MINIZADE_MOTOR_L_PWM2, LOW);
    }
  } else if (side == MOTOR_R) {
    if (speed > 0) { // 前進
      analogWrite(MINIZADE_MOTOR_R_PWM1, speed);
      digitalWrite(MINIZADE_MOTOR_R_PWM2, LOW);
    } else if (speed < 0) { // 後退
      analogWrite(MINIZADE_MOTOR_R_PWM2, abs(speed));
      digitalWrite(MINIZADE_MOTOR_R_PWM1, LOW);
    } else { // 停止
      digitalWrite(MINIZADE_MOTOR_R_PWM1, LOW);
      digitalWrite(MINIZADE_MOTOR_R_PWM2, LOW);
    }
  }
}

void MinizadeMakerSumoAdapter::moveForward(int speed)
{
  setMotorSpeed(MOTOR_L, speed);
  setMotorSpeed(MOTOR_R, speed);
}

void MinizadeMakerSumoAdapter::moveBackward(int speed)
{
  setMotorSpeed(MOTOR_L, -speed);
  setMotorSpeed(MOTOR_R, -speed);
}

void MinizadeMakerSumoAdapter::turnLeft(int speed)
{
  setMotorSpeed(MOTOR_L, -speed); // 左モーターを後退
  setMotorSpeed(MOTOR_R, speed);  // 右モーターを前進
}

void MinizadeMakerSumoAdapter::turnRight(int speed)
{
  setMotorSpeed(MOTOR_L, speed);  // 左モーターを前進
  setMotorSpeed(MOTOR_R, -speed); // 右モーターを後退
}

// Maker Sumo の playMelody 関数を模倣
// Minizade のブザーピン (D7) を使用
void MinizadeMakerSumoAdapter::playMelody(const int *pitch, const int *duration, int length)
{
  for (int thisNote = 0; thisNote < length; thisNote++) {
    // note duration in ms:
    int noteDuration = 1000 / duration[thisNote];
    tone(MINIZADE_BUZZER, pitch[thisNote], noteDuration);

    // a little pause between notes takes up the rest of the noteDuration:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    noTone(MINIZADE_BUZZER);
  }
}

// ここからユーザーが記述する Arduino スケッチの例
void setup()
{
  MakerSumo.begin();
  Serial.begin(9600);

  // スタートボタンが押されるまで待機 (Minizade のスタートモジュール D13 を使用)
  Serial.println("Waiting for start module...");
  while (digitalRead(MINIZADE_START_MODULE) == HIGH) {
    delay(10);
  }
  Serial.println("Start!");

  // 例としてメロディを再生
  int melodyPitch[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
  int melodyDuration[] = {4, 8, 8, 4, 4, 4, 4, 4};
  MakerSumo.playMelody(melodyPitch, melodyDuration, 8);

  delay(1000);
}

void loop()
{
  // 前進
  Serial.println("Moving Forward");
  MakerSumo.moveForward(150);
  delay(2000);

  // 停止
  Serial.println("Stopping");
  MakerSumo.stop();
  delay(1000);

  // 左折
  Serial.println("Turning Left");
  MakerSumo.turnLeft(100);
  delay(1500);

  // 停止
  Serial.println("Stopping");
  MakerSumo.stop();
  delay(1000);

  // 後退
  Serial.println("Moving Backward");
  MakerSumo.moveBackward(150);
  delay(2000);

  // 停止
  Serial.println("Stopping");
  MakerSumo.stop();
  delay(1000);

  // 右折
  Serial.println("Turning Right");
  MakerSumo.turnRight(100);
  delay(1500);

  // 停止
  Serial.println("Stopping");
  MakerSumo.stop();
  delay(1000);

  // ループを繰り返す前に少し待機
  delay(3000);
}



