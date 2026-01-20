#define ML1 3    // 左モーター IN1
#define ML2 11   // 左モーター IN2
#define MR1 9    // 右モーター IN1
#define MR2 10   // 右モーター IN2

#define SL1 A3  // 対物センサー 前方左
#define SR1 A2  // 対物センサー 前方右
#define SL2 4  // 対物センサー 斜め左
#define SR2 5  // 対物センサー 斜め右

#define BTN 6 // ボタン
#define BUZZER 13

#define FSR A4  // 床センサー右
#define FSL A5  // 床センサー左
//#define ST A7    // スタートモジュール 4 => A7

#define MAX_DUTY 900
#define PWM_MAX 242 // 255 * 0.9

// タイマー制御用変数
unsigned long start_ms = 0;
unsigned long now_ms = 0;  // 現在時刻取得

void set_duty(int L, int R) {
  if (L >= 0) {
    analogWrite(ML1, map(L, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(ML2, 0);
  } else {
    analogWrite(ML1, 0);
    analogWrite(ML2, map(-L, 0, MAX_DUTY, 0, PWM_MAX));
  }

  if (R >= 0) {
    analogWrite(MR1, map(R, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(MR2, 0);
  } else {
    analogWrite(MR1, 0);
    analogWrite(MR2, map(-R, 0, MAX_DUTY, 0, PWM_MAX));
  }
}

void sensorCheck() {
  int sl1 = !digitalRead(SL1);
  int sr1 = !digitalRead(SR1);
  int sl2 = digitalRead(SL2);
  int sr2 = digitalRead(SR2);
  int fsr = analogRead(FSR);
  int fsl = analogRead(FSL);

  Serial.print("SL1:");
  Serial.print(sl1);
  Serial.print(" SR1:");
  Serial.print(sr1);

  Serial.print(" SL2:");
  Serial.print(sl2);
  Serial.print(" SR2:");
  Serial.print(sr2);

  Serial.print(" FSR:");
  Serial.print(fsr);
  Serial.print(" FSL:");
  Serial.print(fsl);
  Serial.println();

  // --- どれか反応したら鳴らす ---
  if (sl1 || sr1 || sl2 || sr2 || fsr <= 736 || fsl <= 736) {
    digitalWrite(BUZZER, HIGH);
    delay(1);
    digitalWrite(BUZZER, LOW);
  } else {
    digitalWrite(BUZZER, LOW);
  }
  delay(100);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(ML1, OUTPUT);
  pinMode(ML2, OUTPUT);
  pinMode(MR1, OUTPUT);
  pinMode(MR2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(SL1, INPUT_PULLUP);
  pinMode(SR1, INPUT_PULLUP);
  pinMode(SL2, INPUT_PULLUP);
  pinMode(SR2, INPUT_PULLUP);
 
  pinMode(BTN, INPUT_PULLUP);

  pinMode(FSR, INPUT_PULLUP);
  pinMode(FSL, INPUT_PULLUP);

  set_duty(0, 0); // 念のためモーターを停止

  while (digitalRead(BTN)) {
    sensorCheck();
  }
  delay(5000); // ここで5秒待つ
}

void loop() {
  // 現在時刻更新
  now_ms = millis();

  // スタート信号読み込み
  // int st_module = 1;

  // ゴー信号受信
  // if (st_module == 1) {
    // ① 白線センサー（最優先）
    int fsrVal = analogRead(FSR);
    int fslVal = analogRead(FSL);
    bool lineRight = (fsrVal <= 736);
    bool lineLeft = (fslVal <= 736);

    if (lineLeft || lineRight) {
      // 停止
      set_duty(0, 0);
      delay(25);

      // バック 
      set_duty(-400, -400);
      delay(100);
      set_duty(-900, -900);  // 最終duty
      delay(200);

      if (lineLeft) {
        // 右旋回
        set_duty(-500, -900);
        delay(25);
        set_duty(0, -900);
        delay(25);
        set_duty(500, -900);
        delay(25);
        set_duty(900, -900);  // 最終duty
        delay(25);

        set_duty(900, -500);
        delay(25);
        set_duty(900, 0);
        delay(25);
        set_duty(900, 500);
        delay(25);
      } else if (lineRight) {
        // ←旋回
        set_duty(-900, -500);
        delay(25);
        set_duty(-900, 0);
        delay(25);
        set_duty(-900, 500);
        delay(25);
        set_duty(-900, 900);  // 最終duty
        delay(25);

        set_duty(-500, 900);
        delay(25);
        set_duty(0, 900);
        delay(25);
        set_duty(500, 900);
        delay(25);
      }
      // 直進
      set_duty(500, 500);
      delay(25);
    }

    // ② 対物センサー処理（反応中はループ）
    int SR1 = !digitalRead(SR1);  // 対物センサー右
    int SL1 = !digitalRead(SL1);  // 対物センサー左

    // --- 両方反応 → 加速 ---
    if (SR1 && SL1) {
      // タイマー開始
      if (start_ms == 0) {
        start_ms = now_ms;
      }

      if (now_ms - start_ms <= 50) {
        set_duty(500, 500);
      } else {
        set_duty(900, 900);  // 最終duty
      }
    }
    // --- 左反応 → 左旋回 ---
    else if (SL1) {
      // タイマー開始
      if (start_ms == 0) {
        start_ms = now_ms;
      }

      if (now_ms - start_ms <= 100) {
        set_duty(0, 250);
      }
      else if (now_ms - start_ms <= 200) {
        set_duty(-250, 500);
      } 
      else {
        set_duty(-500, 750);  // 最終duty
      }
    }
      // --- 右反応 → 右旋回 ---
      else if (SR1) {
        // タイマー開始
        if (start_ms == 0) {
          start_ms = now_ms;
        }
  
        if (now_ms - start_ms <= 100) {
          set_duty(250, 0);
        }
        else if (now_ms - start_ms <= 200) {
          set_duty(500, -250);
        } 
        else {
          set_duty(750, -500);  // 最終duty
        }
      
    } else {
      // ③ 通常直進（敵なし）
      set_duty(150, 150);
      // センサー未検知時はタイマーリセット
      start_ms = 0;
    }


    // デバッグ
    Serial.print("SR1: ");
    Serial.print(SR1);
    Serial.print(" | SL1: ");
    Serial.print(SL1);
    Serial.print(" | FSR: ");
    Serial.print(fsrVal);
    Serial.print(" | FSL: ");
    Serial.print(fslVal);
    Serial.println();
  // } 
}
