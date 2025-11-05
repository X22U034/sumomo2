// Created by Tomoki Sato
//=======================================================================
// 音
//=======================================================================
#pragma once

// 音階周波数リスト
const float SCALE_FREQ[12] = {
		195.998, // 0 g
		207.652, // 1 g#
		220.000, // 2 a
		233.082, // 3 a#
		246.942, // 4 b
		261.626, // 5 c
		277.183, // 6 c#
		293.665, // 7 d
		311.127, // 8 d#
		329.628, // 9 e
		349.228, // 10 f
		369.994	 // 11 f#
};

// SET_BUZZER関数用の音階(ド~次のド)
//////////////////////// 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13
uint8_t BZ_SCALE[14] = {29, 31, 33, 34, 36, 38, 40, 41, 43, 45, 46, 48, 50, 52};

// ラズパイpico用のtone関数(鳴らすピン,周波数,鳴らす時間)[ピン番号,Hz,ms]
/**
 * @brief ラズパイpico用のtone関数
 *
 * @param pin 鳴らすピン番号
 * @param freq 周波数(Hz)
 * @param duration 鳴らす時間(ms)
 * @return void
 *
 */
void TONE(uint8_t pin, float freq, float duration)
{
	float t = 1000000.0 / freq;												// 1周期の時間
	unsigned int t_2 = t / 2.0;												// 1/2周期の時間
	unsigned long cycle = freq * (duration / 1000.0); // 指定時間分に必要なパルス周期

	do // 指定周期になるまで出力周期をくりかえす
	{
		SET_GPIO_OUT(pin, 1);
		delayMicroseconds(t_2);
		SET_GPIO_OUT(pin, 0);
		delayMicroseconds(t_2);
	} while (--cycle > 0); // ダウンカウンタ
}

// ブザーを鳴らす(音階番号,鳴らす時間)[0~87,ms]
/**
 * @brief ブザーを鳴らす
 *
 * @param scale 音階番号(0~87)
 * @param len 鳴らす時間(ms)
 * @return void
 *
 */
void SET_BUZZER(uint8_t scale, uint16_t len)
{
	if (scale > 87)
		scale = 87;

	// 音階番号から周波数を計算
	float freq = SCALE_FREQ[scale % 12] * (float)(1 << (scale / 12));
	// 1オクターブ上げると
	// 周波数が2倍になる,
	// 音階番号が12増える,
	// 音階番号が12で割り切れる
	// 音階番号/12の商がオクターブ数
	// 音階番号%12の余りが音階番号

#if defined(__AVR_ATmega328P__)
	tone(BUZZER, freq, len);
	delay(len);
#else
	TONE(BUZZER, freq, len);
#endif
}