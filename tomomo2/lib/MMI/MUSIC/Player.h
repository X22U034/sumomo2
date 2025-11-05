// Created by Tomoki Sato
//=======================================================================
// ブザー演奏
//=======================================================================
#pragma once

#define SCALE score[note][0]			 // 音の高さ
#define ON_TIME score[note][1] * 10	 // 音の長さ
#define OFF_TIME score[note][2] * 10 // 待ち時間

// 楽譜データ通りに演奏する(効果音名,配列の縦の要素数)
void PLAY_MUSIC(uint8_t score[][3], uint8_t score_length)
{
	uint8_t note = 0; //

	delay(250);
	SET_LED(1);
	do // 必ず一度は音を鳴らす
	{
		if (ON_TIME == 0) // 音の長さが未記入
		{
			delay(OFF_TIME); // 休符
		}
		else
		{
			SET_BUZZER(SCALE, ON_TIME); // ブザーを鳴らす
			delay(OFF_TIME - ON_TIME);	 // 次の音までの余白
		}
	} while (++note < score_length);
	SET_LED(0);
}


// 指定番号で効果音を呼び出す(番号)
/**
 * @brief 指定番号で効果音を呼び出す
 * 
 * @param select 
 * @return void
 * 
*/
void SOUND(uint8_t select)
{
	switch (select)
	{
	case 1:
		PLAY_MUSIC(BATTERY_CHARGER_SE1, 2);
		break;
	case 2:
		PLAY_MUSIC(BATTERY_CHARGER_SE2, 3);
		break;
	case 3:
		PLAY_MUSIC(MARIO_FIREBALL, 3);
		break;
	case 4:
		PLAY_MUSIC(MARIO_POWER_UP, 27);
		break;
	default:
		PLAY_MUSIC(GAME_BOY_START, 2);
	}
}
