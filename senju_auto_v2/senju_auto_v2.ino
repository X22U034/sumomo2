// #define debug_mode

#include <Arduino.h>

/* ==== PIN ==== */
#define MDL1 3
#define MDL2 11
#define MDR1 9
#define MDR2 10

#define SL1 A3
#define SR1 A2
#define SL2 4
#define SR2 5

#define BUZZER 13

#define FSR A4
#define FSL A5

#define PROPO2 A0     // trigger (RC PWM input)
#define ST_MODULE A1  // start module

/* ==== PARAM ==== */
#define MAX_DUTY 900
#define MAX_PWM 255
#define MOTOR_RAMP 3
#define LINE_TH 777
#define PROPO_TRG_TH 1750

#define DIR_L -1
#define DIR_R 1

/* ==== AVOID STATE ==== */
bool line_active = false;
int line_dir = 0;

/* ==== SENSOR STATE ==== */
int sl1, sr1, sl2, sr2;
int fsl, fsr;
int lineL, lineR;
int propo_trg;
int propo;
int st_module;

/* ==== MOTOR STATE ==== */
int m1_current = 0, m2_current = 0;
int m1_target = 0, m2_target = 0;

/* ==== PROPO (PCINT) ==== */
volatile uint16_t propo_us = 0;           // 最新パルス幅[us]
volatile uint32_t propo_rise_us = 0;      // 立ち上がり時刻[micros]
volatile uint32_t propo_last_update = 0;  // 更新時刻[micros]

// Port C (A0-A5) のピン変化割り込み
ISR(PCINT1_vect) {
  // A0 = PC0 = PCINT8
  if (PINC & _BV(0)) {  // HIGH: rising
    propo_rise_us = micros();
  } else {  // LOW: falling
    uint32_t w = micros() - propo_rise_us;
    // 一般的なRCは1000-2000us。上限だけ軽くガード
    if (w <= 3000) {
      propo_us = (uint16_t)w;
      propo_last_update = micros();
    }
  }
}

/* ==== SENSOR ==== */
void read_sensors() {
  sl1 = !digitalRead(SL1);
  sr1 = !digitalRead(SR1);
  sl2 = digitalRead(SL2);
  sr2 = digitalRead(SR2);

  // fsr = analogRead(FSR);
  // fsl = analogRead(FSL);
  // lineL = (fsl <= LINE_TH);
  // lineR = (fsr <= LINE_TH);
  lineL = 0;
  lineR = 0;

  // ---- PROPO: 割り込みで取った値をコピー（ノンブロッキング）----
  uint16_t p;
  uint32_t last;
  noInterrupts();
  p = propo_us;
  last = propo_last_update;
  interrupts();

  propo_trg = (int)p;

  // フェイルセーフ：一定時間更新が無ければOFF扱い
  if ((uint32_t)(micros() - last) > 60000UL) {  // 60ms以上更新なし
    propo = 0;
  } else {
    propo = (propo_trg >= PROPO_TRG_TH);
  }

  st_module = digitalRead(ST_MODULE);
}

/* ==== MOTOR ==== */
void motor_ramp() {
  static unsigned long ramp_ms = 0;
  if (millis() - ramp_ms < 1) return;
  ramp_ms = millis();

  if (m1_current < m1_target) m1_current += MOTOR_RAMP;
  if (m1_current > m1_target) m1_current -= MOTOR_RAMP;
  if (m2_current < m2_target) m2_current += MOTOR_RAMP;
  if (m2_current > m2_target) m2_current -= MOTOR_RAMP;

  m1_current = constrain(m1_current, -MAX_DUTY, MAX_DUTY);
  m2_current = constrain(m2_current, -MAX_DUTY, MAX_DUTY);
}

void motor_out() {
  if (m1_current >= 0) {
    analogWrite(MDL1, map(m1_current, 0, MAX_DUTY, 0, MAX_PWM));
    analogWrite(MDL2, 0);
  } else {
    analogWrite(MDL1, 0);
    analogWrite(MDL2, map(-m1_current, 0, MAX_DUTY, 0, MAX_PWM));
  }

  if (m2_current >= 0) {
    analogWrite(MDR1, map(m2_current, 0, MAX_DUTY, 0, MAX_PWM));
    analogWrite(MDR2, 0);
  } else {
    analogWrite(MDR1, 0);
    analogWrite(MDR2, map(-m2_current, 0, MAX_DUTY, 0, MAX_PWM));
  }
}

void set_duty(int m1duty, int m2duty) {
  m1_target = constrain(m1duty, -MAX_DUTY, MAX_DUTY);
  m2_target = constrain(m2duty, -MAX_DUTY, MAX_DUTY);
  motor_ramp();
  motor_out();
}

/* ==== LINE AVOID ==== */
bool back_turn(int dir) {
  static unsigned long start_ms = 0;
  unsigned long now_ms = millis();

  if (start_ms == 0) start_ms = now_ms;

  unsigned long time = now_ms - start_ms;

  if (time < 150) {
    set_duty(-300, -300);
  } else if (time < 300) {
    if (dir == DIR_L) set_duty(-300, 300);
    else set_duty(300, -300);
  } else {
    start_ms = 0;
    return false;
  }
  return true;
}

/* ==== SHORT DASH ==== */
bool gaga() {
  static unsigned long start_ms = 0;
  unsigned long now = millis();

  if (start_ms == 0) start_ms = now;

  unsigned long time = now - start_ms;

  if (time < 60) {
    set_duty(600, 600);
  } else if (time < 1000) {
    set_duty(0, 0);
  } else {
    start_ms = 0;
    return false;
  }
  return true;
}

void debug_print() {
#ifdef debug_mode
  Serial.print("sl1:");
  Serial.print(sl1);
  Serial.print(" sr1:");
  Serial.print(sr1);
  Serial.print(" sl2:");
  Serial.print(sl2);
  Serial.print(" sr2:");
  Serial.print(sr2);
  Serial.print(" FSL:");
  Serial.print(fsl);
  Serial.print(" FSR:");
  Serial.print(fsr);
  Serial.print(" lineL:");
  Serial.print(lineL);
  Serial.print(" lineR:");
  Serial.print(lineR);
  Serial.print(" propo_trg:");
  Serial.print(propo_trg);
  Serial.print(" propo:");
  Serial.print(propo);
  Serial.print(" st_module:");
  Serial.print(st_module);
  Serial.println();
#endif
}

void sensor_check() {
  if (sl1 || sr1 || sl2 || sr2 || lineL || lineR || propo || st_module) {
    digitalWrite(BUZZER, HIGH);
    delay(1);
    digitalWrite(BUZZER, LOW);
  }
}

/* ==== SETUP ==== */
void setup() {
#ifdef debug_mode
  Serial.begin(2000000);
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

  pinMode(ST_MODULE, INPUT_PULLUP);
  pinMode(PROPO2, INPUT);

  // ---- PCINT enable for A0 (PC0 = PCINT8) ----
  PCICR |= _BV(PCIE1);    // Port C のピン変化割り込み許可
  PCMSK1 |= _BV(PCINT8);  // A0 を監視対象に

  set_duty(0, 0);

  // ==== セーフティ解除（プロポON待ち）====
  while (!propo) {
    read_sensors();
    sensor_check();
    debug_print();
  }

  digitalWrite(BUZZER, HIGH);
  delay(50);
  digitalWrite(BUZZER, LOW);
  delay(50);
  digitalWrite(BUZZER, HIGH);
  delay(50);
  digitalWrite(BUZZER, LOW);
  delay(50);

  // プロポON後、スタートモジュール待ち
  while (!st_module) {
    read_sensors();
  }
}

/* ==== LOOP ==== */
void loop() {
  static bool mode_attack = false;
  read_sensors();
  debug_print();

  // 安全停止：プロポOFF または スタート未投入なら停止
  if (propo || !st_module) {
    while (1){
    set_duty(0, 0);
    }
  }

  /* ==== LINE AVOID ==== */
  if (!line_active) {
    if (lineL) {
      line_active = true;
      line_dir = DIR_R;
    } else if (lineR) {
      line_active = true;
      line_dir = DIR_L;
    }
  }

  if (line_active) {
    if (back_turn(line_dir)) return;
    line_active = false;
  }

  if (sl1 && sr1) set_duty(1000, 1000);
  else if (sl1) {
    set_duty(-300, 600);
    mode_attack = true;
  } else if (sr1) {
    set_duty(600, -300);
    mode_attack = true;
  } else if (sl2) set_duty(-600, 600);
  else if (sr2) set_duty(600, -600);
  else {
    if (mode_attack)
      set_duty(600, 600);
    else
      gaga();
  }
}
