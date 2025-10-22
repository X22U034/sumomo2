#include "lib/fast_digital_rw.hpp"  // 高速デジタル入出力ライブラリ

#define SL2 A5  // 対物センサー左
#define SR2 A2  // 対物センサー右

void setup() {
  Serial.begin(9600);  // シリアル通信開始
  pinMode(SL2, INPUT);
  pinMode(SR2, INPUT);

  Serial.println("=== SR2 / SL2 センサー動作チェック開始 ===");
  Serial.println("黒=1, 白=0 （HIGH/LOW表示）");
}

void loop() {
  int leftSensor = dr(SL2);   // 左センサー読み取り
  int rightSensor = dr(SR2);  // 右センサー読み取り

  Serial.print("SL2(左): ");
  Serial.print(leftSensor);
  Serial.print("   SR2(右): ");
  Serial.println(rightSensor);

  delay(200);  // 0.2秒ごとに更新
}
