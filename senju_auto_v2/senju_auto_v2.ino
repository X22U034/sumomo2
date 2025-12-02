#define L1 3    // 左モーター IN1
#define L2 11   // 左モーター IN2
#define R1 9    // 右モーター IN1
#define R2 10   // 右モーター IN2　
#define OSR A2  // 対物センサー右
#define OSL A3  // 対物センサー左
#define FSR A4  // 床センサー右
#define FSL A5  // 床センサー左
#define ST 4    // スタートモジュール

#define MAX_DUTY 900
#define PWM_MAX 255

// タイマー制御用変数
unsigned long start_ms = 0;
unsigned long now_ms = 0;  // 現在時刻取得

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

void stopMove() {
  set_duty(0, 0);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(OSR, INPUT);
  pinMode(OSL, INPUT);
  pinMode(FSR, INPUT);
  pinMode(FSL, INPUT);

  // ----------------------------------------
  // 電源ON → 5秒待機
  // ----------------------------------------
  //stopMove();        // 念のためモーターを停止
  //delay(5000);       // ここで5秒待つ
}


void loop() {
  // 現在時刻更新
  now_ms = millis();

  // スタート信号読み込み
  int st_module = digitalRead(ST);

  //ゴー信号受信
  if (st_module == 1) {
    // ------------------------------------------------
    // ① 白線センサー（最優先）
    // ------------------------------------------------
    int fsrVal = analogRead(FSR);
    int fslVal = analogRead(FSL);
    bool lineRight = (fsrVal <= 736);
    bool lineLeft = (fslVal <= 736);

    if (lineLeft || lineRight) {
      // 停止
      set_duty(0, 0);
      delay(25);

      // バック
      set_duty(-500, -500);
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


    // ------------------------------------------------
    // ② 対物センサー処理（反応中はループ）
    // ------------------------------------------------
    int osr = !digitalRead(OSR);  // 対物センサー右
    int osl = !digitalRead(OSL);  // 対物センサー左

    // --- 両方反応 → 加速 ---
    if (osr && osl) {
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
    else if (osl) {
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
      else if (osr) {
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


    // ------------------------------------------------
    // デバッグ
    // ------------------------------------------------
    Serial.print("OSR: ");
    Serial.print(osr);
    Serial.print(" | OSL: ");
    Serial.print(osl);
    Serial.print(" | FSR: ");
    Serial.print(fsrVal);
    Serial.print(" | FSL: ");
    Serial.print(fslVal);
    Serial.println();
  } else {  //レディーもしくはストップ信号受信時
    stopMove();
    delay(200);  // チャタリング防止
    return;
  }
}
