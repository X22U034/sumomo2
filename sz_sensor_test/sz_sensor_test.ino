#include "lib/fast_digital_rw.hpp"  // 高速デジタル入出力ライブラリ

#define SL2 A5  // 対物センサー左
#define SR2 A2  // 対物センサー右

void setup() {
  Serial.begin(115200);  // シリアル通信開始
  pinMode(SL2, INPUT);
  pinMode(SR2, INPUT);
  pinMode(13,OUTPUT);

  Serial.println("=== SR2 / SL2 センサー動作チェック開始 ===");
  Serial.println("黒=1, 白=0 （HIGH/LOW表示）");
}

void loop() {
  bool sl = digitalRead(SL2);   // 左センサー読み取り
  bool sr = digitalRead(SR2);  // 右センサー読み取り
  digitalWrite(13, sl);
  Serial.print("SL2(左): ");
  Serial.print(sl);
  Serial.print("   SR2(右): ");
  Serial.println(sr);

}