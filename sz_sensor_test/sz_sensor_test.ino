#include "lib/fast_digital_rw.hpp" // 高速デジタル入出力ライブラリ

void setup()
{
	Serial.begin(9600); // シリアル通信開始
	// MOTOR ピン3, 11, 9, 10, 13, A6 以外をINPUT_PULLUPに設定

	for (uint8_t i = 0; i <= A7; i++)
	{
		switch (i)
		{
		case 3:
		case 9:
		case 10:
		case 11:
		case 13:
		case A6:
			break;

		default:
			pinMode(i, INPUT_PULLUP);
		}
	}

	pinMode(13, OUTPUT);
	pinMode(A6, OUTPUT);

	Serial.println("センサー動作チェック");
}

void loop()
{
	// 1~A6の状態を一覧表示
	for (uint8_t i = 1; i <= A6; i++)
	{
		Serial.print(i);
		Serial.print(":");
		Serial.print(dr(i));
		Serial.print("  ");
	}
	Serial.println();
}