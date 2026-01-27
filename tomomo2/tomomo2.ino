#include "CytronMakerSumo.h"

#define EDGE_L_THRESHOLD 357
#define EDGE_R_THRESHOLD 344

void setup() {
  Serial.begin(9600);
  MakerSumo.begin();

  Serial.println("=== Full Sensor Test ===");
  Serial.println("Opponent sensors: LOW = detected");
  Serial.println("Edge sensors: value < threshold = WHITE LINE");
  delay(1000);
}

void loop() {

  // --- Opponent sensors ---
  int fc = digitalRead(OPP_FC);
  int fl = digitalRead(OPP_FL);
  int fr = digitalRead(OPP_FR);
  int l  = digitalRead(OPP_L);
  int r  = digitalRead(OPP_R);

  // --- Edge sensors ---
  int edgeL = analogRead(EDGE_L);
  int edgeR = analogRead(EDGE_R);

  Serial.print("OPP  FC:");
  Serial.print(fc);
  Serial.print(" FL:");
  Serial.print(fl);
  Serial.print(" FR:");
  Serial.print(fr);
  Serial.print(" L:");
  Serial.print(l);
  Serial.print(" R:");
  Serial.print(r);

  Serial.print("  ||  EDGE  L:");
  Serial.print(edgeL);
  Serial.print(edgeL < EDGE_L_THRESHOLD ? " (WHITE)" : " (BLACK)");

  Serial.print("  R:");
  Serial.print(edgeR);
  Serial.print(edgeR < EDGE_R_THRESHOLD ? " (WHITE)" : " (BLACK)");

  Serial.println();

  delay(120);
}