#define L1 3   // 左モーター IN1
#define L2 11  // 左モーター IN2
#define R1 9   // 右モーター IN1
#define R2 10  // 右モーター IN2　
#define OSR A2 // 対物センサー右
#define OSL A3 // 対物センサー左
#define FSR A4 // 床センサー右
#define FSL A5  // 床センサー左
#define ST 4  // スタートモジュール

#define MAX_DUTY 900
#define PWM_MAX  255

void set_duty(int L, int R) {
  if (L >= 0) {
    analogWrite(L1, map(L, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(L2, 0);
  } else {
    analogWrite(L1, 0);
    analogWrite(L2, map(-L, 0, MAX_DUTY, 0, PWM_MAX));
  }

  if (R >= 0) {
    analogWrite(R1, map(R, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(R2, 0);
  } else {
    analogWrite(R1, 0);
    analogWrite(R2, map(-R, 0, MAX_DUTY, 0, PWM_MAX));
  }
}

void stopMove() { set_duty(0, 0); }

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(L1, OUTPUT); pinMode(L2, OUTPUT);
  pinMode(R1, OUTPUT); pinMode(R2, OUTPUT);
  pinMode(OSR, INPUT); pinMode(OSL, INPUT);
  pinMode(FSR, INPUT); pinMode(FSL, INPUT);

  // ----------------------------------------
  // 電源ON → 5秒待機
  // ----------------------------------------
  stopMove();        // 念のためモーターを停止
  delay(5000);       // ここで5秒待つ

}


void loop() {

  // ------------------------------------------------
  // ① 白線センサー（最優先）
  // ------------------------------------------------
  int fsrVal = analogRead(FSR);
  int fslVal = analogRead(FSL);
  bool lineRight = (fsrVal <= 739);
  bool lineLeft  = (fslVal <= 739);

  if (lineRight || lineLeft) {

    // // バック
    // set_duty(-900, -900);
    // delay(200);

    // 右旋回
    set_duty(300, -300);
    delay(100); 
  }

  // ------------------------------------------------
  // ② 対物センサー処理（反応中はループ）
  // ------------------------------------------------
  int osr = digitalRead(OSR);
  int osl = digitalRead(OSL);

  if (osr == LOW || osl == LOW) {

    // --- 両方反応 → 加速 ---
    if (osr == LOW && osl == LOW) {
      set_duty(500, 500);
    }
    // --- 右反応 → 右旋回 → 加速 ---
    else if (osr == LOW) {
      set_duty(500, 0);  // 右旋回
      delay(25);
      set_duty(500, 500);   // 突進
    }
    // --- 左反応 → 左旋回 → 加速 ---
    else if (osl == LOW) {
      set_duty(0, 500);  // 左旋回
      delay(25);
      set_duty(500, 500);   // 突進
    }
}
  else {
      // ------------------------------------------------
      // ③ 通常直進（敵なし）
      // ------------------------------------------------
      set_duty(150, 150);
  }


  // ------------------------------------------------
  // デバッグ
  // ------------------------------------------------
  Serial.print("OSR: "); Serial.print(osr);
  Serial.print(" | OSL: "); Serial.print(osl);
  Serial.print(" | FSR: "); Serial.print(fsrVal);
  Serial.print(" | FSL: "); Serial.print(fslVal);
  Serial.println();
}


