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

#define BTN 6
#define BUZZER 13

#define FSR A4
#define FSL A5

/* ==== PARAM ==== */
#define MAX_DUTY 900
#define PWM_MAX 242
#define MOTOR_RAMP 30
#define LINE_TH 767

#define L -1
#define R  1

/* ==== SENSOR STATE ==== */
int sl1, sr1, sl2, sr2;
int fsr, fsl;
int lineL, lineR;

/* ==== MOTOR STATE ==== */
int curL = 0, curR = 0;
int tgtL = 0, tgtR = 0;
unsigned long ramp_ms = 0;

/* ==== AVOID STATE ==== */
int TURN_DIR = 0;
unsigned long start_ms = 0;

/* ==== SENSOR ==== */
void read_sensors() {
  sl1 = !digitalRead(SL1);
  sr1 = !digitalRead(SR1);
  sl2 =  digitalRead(SL2);
  sr2 =  digitalRead(SR2);

  fsr = analogRead(FSR);
  fsl = analogRead(FSL);

  lineL = (fsl <= LINE_TH);
  lineR = (fsr <= LINE_TH);
}

/* ==== MOTOR ==== */
void motor_ramp() {
  if (millis() - ramp_ms < 1) return;
  ramp_ms = millis();

  if (curL < tgtL) curL += MOTOR_RAMP;
  if (curL > tgtL) curL -= MOTOR_RAMP;
  if (curR < tgtR) curR += MOTOR_RAMP;
  if (curR > tgtR) curR -= MOTOR_RAMP;

  curL = constrain(curL, -MAX_DUTY, MAX_DUTY);
  curR = constrain(curR, -MAX_DUTY, MAX_DUTY);
}

void motor_out() {
  if (curL >= 0) {
    analogWrite(MDL1, map(curL, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(MDL2, 0);
  } else {
    analogWrite(MDL1, 0);
    analogWrite(MDL2, map(-curL, 0, MAX_DUTY, 0, PWM_MAX));
  }

  if (curR >= 0) {
    analogWrite(MDR1, map(curR, 0, MAX_DUTY, 0, PWM_MAX));
    analogWrite(MDR2, 0);
  } else {
    analogWrite(MDR1, 0);
    analogWrite(MDR2, map(-curR, 0, MAX_DUTY, 0, PWM_MAX));
  }
}

void set_duty(int m1duty, int m2duty) {
  tgtL = constrain(m1duty, -MAX_DUTY, MAX_DUTY);
  tgtR = constrain(m2duty, -MAX_DUTY, MAX_DUTY);
  motor_ramp();
  motor_out();
}

/* ==== LINE AVOID ==== */
bool BACK_TURN(int dir) {
  unsigned long now = millis();

  if (TURN_DIR == 0) {
    TURN_DIR = dir;
    start_ms = now;
  }

  unsigned long t = now - start_ms;

  if (t < 100) {
    set_duty(-800, -800);
  } else if (t < 300) {
    if (TURN_DIR == L) set_duty(-800, 800);
    else               set_duty(800, -800);
  } else {
    TURN_DIR = 0;
    return false;
  }
  return true;
}

void debug_print() {
#ifdef debug_mode
  Serial.print("sl1:"); Serial.print(sl1);
  Serial.print(" sr1:"); Serial.print(sr1);
  Serial.print(" sl2:"); Serial.print(sl2);
  Serial.print(" sr2:"); Serial.print(sr2);
  Serial.print(" FSL:"); Serial.print(fsl);
  Serial.print(" FSR:"); Serial.print(fsr);
  Serial.print(" lineL:"); Serial.print(lineL);
  Serial.print(" lineR:"); Serial.print(lineR);
  Serial.println();
#endif
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

  set_duty(0, 0);

  while (digitalRead(BTN)) {
    read_sensors();
    debug_print();

    if (sl1 || sr1 || sl2 || sr2 || lineL || lineR) {
      digitalWrite(BUZZER, HIGH);
      delay(1);
      digitalWrite(BUZZER, LOW);
    }
    delay(100);
  }

  delay(5000);
}

/* ==== LOOP ==== */
void loop() {
  read_sensors();
  debug_print();

  if (lineL && BACK_TURN(L)) return;
  if (lineR && BACK_TURN(R)) return;

  if (sl1 && sr1)      set_duty(900, 900);
  else if (sl1)        set_duty(250, 500);
  else if (sr1)        set_duty(500, 250);
  else if (sl2)        set_duty(-500, 500);
  else if (sr2)        set_duty(500, -500);
  else                 set_duty(250, 250);
}
