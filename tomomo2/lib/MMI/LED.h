// Created by Tomoki Sato
//=======================================================================
// LED
//=======================================================================
#pragma once

volatile bool LED_state;

// 内蔵LEDの点灯状態を指定する(点灯|消灯)[1|0]
/**
 * @brief 内蔵LEDの点灯状態を指定する(点灯|消灯)[1|0]
 * @param state 点灯状態
 * @return なし
 *
*/
void SET_LED(bool state)
{
	LED_state = state;
	digitalWrite(LED_BUILTIN, LED_state);
}

// LEDの点灯状態を反転させる
/**
 * @brief LEDの点灯状態を反転させる
 * @param なし
 * @return なし
 *
*/
void REVERSE_LED()
{
	digitalWrite(LED_BUILTIN, LED_state = !LED_state);
}

// LEDの点灯状態を反転させる
void SWITCH_LED()
{
	REVERSE_LED();
}

// LEDの点灯状態を反転させる
void TOGGLE_LED()
{
	REVERSE_LED();
}
