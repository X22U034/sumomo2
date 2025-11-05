// Created by Tomoki Sato
//=======================================================================
// DIPスイッチ
//=======================================================================
#pragma once

//[0~15] DIPスイッチの番号[0~15]を取得
/**
 * @brief DIPスイッチの番号[0~15]を取得
 * @param なし
 * @return DIPスイッチの番号[0~15]
 *
 */
uint8_t GET_DIPSWITCH()
{
	uint8_t num = 0;
	num += GET_GPIO_IN(DIP1) << 0;
	num += GET_GPIO_IN(DIP2) << 1;
	num += GET_GPIO_IN(DIP3) << 2;
	num += GET_GPIO_IN(DIP4) << 3;
	return num;
}