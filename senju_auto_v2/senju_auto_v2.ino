#define debug_mode

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

#define BTN 6  // push button
#define BUZZER 13

#define FSR A4
#define FSL A5

#define PROPO_TRG A0  // trigger
#define ST_MODULE A1  // start module

/* ==== PARAM ==== */
#define MAX_DUTY 900
#define MAX_PWM 255
#define MOTOR_RAMP 30
#define LINE_TH 777
// #define ST_MOD_TH 512

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
int st_module;
// int sensor_state[15]=0;

/* ==== MOTOR STATE ==== */
int m1_current = 0, m2_current = 0;
int m1_target = 0, m2_target = 0;

/* ==== SENSOR ==== */
void read_sensors() {
  sl1 = !digitalRead(SL1);
  sr1 = !digitalRead(SR1);
  sl2 = digitalRead(SL2);
  sr2 = digitalRead(SR2);

  fsr = analogRead(FSR);
  fsl = analogRead(FSL);
  lineL = (fsl <= LINE_TH);
  lineR = (fsr <= LINE_TH);

  propo_trg = pulseIn(PROPO_TRG, HIGH, 30000);
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
  // 下限上限に収める
  m1_target = constrain(m1duty, -MAX_DUTY, MAX_DUTY);
  m2_target = constrain(m2duty, -MAX_DUTY, MAX_DUTY);
  motor_ramp();
  motor_out();
}

/* ==== LINE AVOID ==== */
bool back_turn(int dir) {
  static unsigned long start_ms = 0;
  unsigned long now_ms = millis();

  if (start_ms == 0) {
    start_ms = now_ms;
  }

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

  if (start_ms == 0) {
    start_ms = now;
  }

  unsigned long time = now - start_ms;

  if (time < 20) {
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
  Serial.print(" st_module:");
  Serial.print(st_module);
  Serial.println();
#endif
}

void sensor_check() {
  read_sensors();
  debug_print();

  if (sl1 || sr1 || sl2 || sr2 || lineL || lineR) {
    digitalWrite(BUZZER, HIGH);
    delay(1);
    digitalWrite(BUZZER, LOW);
  }
  delay(200);
}

/* ==== SETUP ==== */
void setup() {
#ifdef debug_mode
  Serial.begin(9600);
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
  pinMode(ST_MODULE, INPUT_PULLUP);
  pinMode(PROPO_TRG, INPUT_PULLUP);


  set_duty(0, 0);

  // セーフティ解除
  while (digitalRead(BTN)) {
    sensor_check();
  }
  delay(5000);
}

/* ==== LOOP ==== */
void loop() {
  read_sensors();
  debug_print();

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
    line_active = false;  // 完走したら解除
  }

  if (sl1 && sr1) set_duty(1000, 1000);
  else if (sl1) set_duty(0, 300);
  else if (sr1) set_duty(300, 0);
  else if (sl2) set_duty(-300, 300);
  else if (sr2) set_duty(300, -300);
  else gaga();
}
