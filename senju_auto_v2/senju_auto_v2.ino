// #define debug_mode

#define MDL1 3   // 左モーター IN1
#define MDL2 11  // 左モーター IN2
#define MDR1 9   // 右モーター IN1
#define MDR2 10  // 右モーター IN2

#define SL1 A3  // 対物センサー 前方左
#define SR1 A2  // 対物センサー 前方右
#define SL2 4   // 対物センサー 斜め左
#define SR2 5   // 対物センサー 斜め右

#define BTN 6  // ボタン
#define BUZZER 13

#define FSR A4  // 床センサー右
#define FSL A5  // 床センサー左
//#define ST A7    // スタートモジュール 4 => A7

#define MAX_DUTY 900
#define PWM_MAX 242  // 255 * 0.9
#define MOTOR_RAMP 50

#define L -1
#define R 1
int TURN_DIR = 0;

// タイマー制御用変数
unsigned long start_ms = 0;
unsigned long now_ms = 0;  // 現在時刻取得

bool BACK_TURN(int dir) {
  unsigned long now_ms = millis();

  if (TURN_DIR == 0) {
    TURN_DIR = dir;
    start_ms = now_ms;
  }

  unsigned long t = now_ms - start_ms;
  if (t < 100) {
    set_duty(-900, -900);  // BACK
  } else if (t < 250) {
    if (TURN_DIR < 0)
      set_duty(-900, 900);  // TURN LEFT
    else
      set_duty(900, -900);  // TURN RIGHT
  } else {
    TURN_DIR = 0;  // 終了
    return false;
  }

  return true;  // 実行中
}

int curL = 0, curR = 0;  // current
int tgtL = 0, tgtR = 0;  // target
unsigned long ramp_ms = 0;

void motor_ramp() {
  if (millis() - ramp_ms < 1) return;  // 1ms周期
  ramp_ms = millis();

  // 左
  if (curL < tgtL) {
    curL += MOTOR_RAMP;
    if (curL > tgtL) curL = tgtL;
  } else if (curL > tgtL) {
    curL -= MOTOR_RAMP;
    if (curL < tgtL) curL = tgtL;
  }

  // 右
  if (curR < tgtR) {
    curR += MOTOR_RAMP;
    if (curR > tgtR) curR = tgtR;
  } else if (curR > tgtR) {
    curR -= MOTOR_RAMP;
    if (curR < tgtR) curR = tgtR;
  }
}

void set_duty(int m1, int m2) {
  tgtL = constrain(m1, -MAX_DUTY, MAX_DUTY);
  tgtR = constrain(m2, -MAX_DUTY, MAX_DUTY);

  motor_ramp();  // 内部でcurを更新

  // --- 左モーター出力 ---
  if (curL >= 0) {
    analogWrite(MDL1, map(curL, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(MDL2, 0);
  } else {
    analogWrite(MDL1, 0);
    analogWrite(MDL2, map(-curL, 0, MAX_DUTY, 0, PWM_MAX));
  }

  // --- 右モーター出力 ---
  if (curR >= 0) {
    analogWrite(MDR1, map(curR, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(MDR2, 0);
  } else {
    analogWrite(MDR1, 0);
    analogWrite(MDR2, map(-curR, 0, MAX_DUTY, 0, PWM_MAX));
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
#ifdef debug_mode
  Serial.begin(9600);
  while (!Serial) {}
#endif

  pinMode(MDL1, OUTPUT);
  pinMode(MDL2, OUTPUT);
  pinMode(MDR1, OUTPUT);
  pinMode(MDR2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(SL1, INPUT_PULLUP);
  pinMode(SR1, INPUT_PULLUP);
  pinMode(SL2, INPUT_PULLUP);
  pinMode(SR2, INPUT_PULLUP);

  pinMode(BTN, INPUT_PULLUP);

  pinMode(FSR, INPUT_PULLUP);
  pinMode(FSL, INPUT_PULLUP);

  set_duty(0, 0);  // 念のためモーターを停止

  while (digitalRead(BTN)) {
    sensorCheck();
  }
  delay(5000);  // ここで5秒待つ
}

void loop() {
  // 現在時刻更新
  now_ms = millis();

  // スタート信号読み込み
  // int st_module = 1;

  // ゴー信号受信
  // if (st_module == 1) {

  int sl1 = !digitalRead(SL1);
  int sr1 = !digitalRead(SR1);
  int sl2 = digitalRead(SL2);
  int sr2 = digitalRead(SR2);
  int fsr = analogRead(FSR);
  int fsl = analogRead(FSL);
  bool lineR = (fsr <= 736);
  bool lineL = (fsl <= 736);

  if (lineL) {
    if (BACK_TURN(L)) return;
  } else if (lineR) {
    if (BACK_TURN(R)) return;
  }

  if (sl1 && sr1) {
    set_duty(900, 900);
  } else if (sl1) {
    set_duty(250, 500);
  } else if (sr1) {
    set_duty(500, 250);
  } else if (sl2) {
    set_duty(-500, 500);
  } else if (sr2) {
    set_duty(500, -500);
  } else {
    set_duty(150, 150);
    start_ms = 0;
  }

  // デバッグ
  Serial.print(" sl1:");
  Serial.print(sl1);
  Serial.print(" sr1:");
  Serial.print(sr1);
  Serial.print(" sl2:");
  Serial.print(SL2);
  Serial.print(" sr2:");
  Serial.print(SR2);
  Serial.print(" FSL:");
  Serial.print(fsl);
  Serial.print(" FSR:");
  Serial.print(fsr);
  Serial.println();
  // }
}
