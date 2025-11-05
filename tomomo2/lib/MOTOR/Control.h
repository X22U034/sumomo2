// Created by Tomoki Sato
//=======================================================================
// モーター関連
//=======================================================================
#pragma once

// モーター制御用
typedef struct
{
	int8_t dir1; // 回転方向(正転|逆転)[1|-1]
	int8_t dir2; // 回転方向(正転|逆転)[1|-1]
	int limit;	 // duty最大値の制限[0~1000]
	int ramp;		 // 1msあたりに変化するduty増減値(duty/ms)
} MOTOR;
MOTOR M;

// 各モーターの設定値
typedef struct
{
	int m1; // duty[-1000~1000]
	int m2; // duty[-1000~1000]
} DUTY;
DUTY Duty; // 目標値
DUTY _d;	 // 制御用

/*
//モーターの回転許可()[0|1]
void SET_MOTOR_ENABLE(bool n)
{
	digitalWrite(L_EN1,n);
	digitalWrite(R_EN1,n);
	digitalWrite(L_EN2,n);
	digitalWrite(R_EN2,n);
}
*/

// 左右のモーターごとの回転方向を指定(左回転方向,右回転方向)[正転|逆転,正転|逆転][1|-1,1|-1]
/**
 * @brief 左右のモーターごとの回転方向を指定(正転|逆転,正転|逆転)[1|-1,1|-1]
 * @param dir1 左回転方向
 * @param dir2 右回転方向
 * @return なし
 *
 */
void SET_MOTOR_DIRECTION(int8_t dir1, int8_t dir2)
{
	if (dir1 * dir2 == -1 || dir1 * dir2 == 1)
	{
		M.dir1 = dir1;
		M.dir2 = dir2;
	}
}

// モーターの回転方向を反転させる(反転パターン)[0|1|2|3]
/**
 * @brief モーターの回転方向を反転させる(反転パターン)[0|1|2|3]
 * @param rev 反転パターン[反転なし|m1のみ|m2のみ|m1,m2反転]
 * @return なし
 *
 */
void SET_MOTOR_REVERSE(uint8_t rev)
{
	switch (rev)
	{
	case 1:
		SET_MOTOR_DIRECTION(-1, 1);
		break;
	case 2:
		SET_MOTOR_DIRECTION(1, -1);
		break;
	case 3:
		SET_MOTOR_DIRECTION(-1, -1);
		break;

	default:
		SET_MOTOR_DIRECTION(1, 1);
	}
}

// dutyの最大値(絶対値)の指定(duty)[100～1000]
/**
 * @brief dutyの最大値(絶対値)の指定(duty)[100～1000]
 * @param duty 最大値(絶対値)
 * @return なし
 *
 */
void SET_MOTOR_LIMIT(int duty)
{
#if defined(OUTPUT_FULL_DIGITAL)
	M.limit = 1000;

#else
	if (duty >= 100 && duty <= 1000)
		M.limit = duty;
#endif
}

// dutyの上限と下限を100%から減らす量を指定(減らす量)[0～100][%]
/**
 * @brief dutyの上限と下限を100%から減らす量を指定(減らす量)[0～100][%]
 * @param percent 減らす量[%]
 * @return なし
 *
 * @details
 * 例えば、100%から10%減らす場合は、10を指定する。
 * 100%から20%減らす場合は、20を指定する。
 *
 */
void SET_MOTOR_RECHARGE(uint8_t percent)
{
	SET_MOTOR_LIMIT(1000 - (percent * 10));
}

// [-M.limit~M.limit]入力dutyを制限値内に切り詰める(duty)[-1000~1000]
/**
 * @brief [-M.limit~M.limit] dutyを制限値内にする(duty比)[-1000~1000]
 * @param duty 入力duty
 * @return 制限値内に切り詰めたduty
 *
 */
int IN_LIMIT(int duty)
{
	if (duty >= M.limit)
		duty = M.limit;
	else if (duty <= -M.limit)
		duty = -M.limit;
	return duty;
}

// 1msあたりのduty変化量を指定(duty増減値)[1~1000][duty/ms]
/**
 * @brief 1msあたりのduty変化量を指定(duty増減値)[1~1000][duty/ms]
 * @param duty_ms duty増減値[duty/ms]
 * @return なし
 *
 */
void SET_MOTOR_RAMP(int duty_ms)
{
#if defined(OUTPUT_FULL_DIGITAL)
	M.ramp = 1000;
#else
	if (duty_ms >= 1 && duty_ms <= 1000)
		M.ramp = duty_ms;
#endif
}

//[str DUTY] モーターへの指令を鈍らす(dutyの目標値)
/**
 * @brief モーターへの指令を鈍らす(dutyの目標値)
 * @param target dutyの目標値
 * @return str DUTY 計算後のduty
 *
 */
DUTY MOTOR_RAMP(DUTY target)
{
	static uint32_t time;

	if (millis() - time >= 1)
	{
		time = millis();

		if (_d.m1 < target.m1)
		{
			_d.m1 += M.ramp;
			if (_d.m1 > target.m1) // target.m1を加算して超えた場合
				_d.m1 = target.m1;	 // 目標値と揃える
		}
		else if (_d.m1 > target.m1)
		{
			_d.m1 -= M.ramp;
			if (_d.m1 < target.m1)
				_d.m1 = target.m1;
		}

		if (_d.m2 < target.m2)
		{
			_d.m2 += M.ramp;
			if (_d.m2 > target.m2) // target.m2を加算して超えた場合
				_d.m2 = target.m2;	 // 目標値と揃える
		}
		else if (_d.m2 > target.m2)
		{
			_d.m2 -= M.ramp;
			if (_d.m2 < target.m2)
				_d.m2 = target.m2;
		}
	}
	return _d;
}

//[-1000~1000] 左のモーターのdutyを取得
int GET_M1_DUTY()
{
	return Duty.m1;
}

// [-1000~1000] 右のモーターのdutyを取得
int GET_M2_DUTY()
{
	return Duty.m2;
}

// dutyをanalogWrite用に変換(duty)[-1000~1000]
/**
 * @brief dutyをanalogWrite用に変換(duty)[-1000~1000]
 * @param duty duty
 * @return dutyをanalogWrite用に変換した値[-255~255]
 *
 */
int CONVERT_DUTY(int duty)
{
	return map(duty, -1000, 1000, -255, 255);
}

/**
 * @brief モーターの出力状態を指定(出力値1,出力値2)[-1000~1000,-1000~1000]
 * @param m1duty 出力値1
 * @param m2duty 出力値2
 * @return なし
 *
 */
void SET_DUTY(int m1duty, int m2duty)
{
	Duty.m1 = IN_LIMIT(m1duty);
	Duty.m2 = IN_LIMIT(m2duty);

	_d = MOTOR_RAMP(Duty);

	static int d1, d2;
	d1 = CONVERT_DUTY(_d.m1) * M.dir1;
	d2 = CONVERT_DUTY(_d.m2) * M.dir2;

	switch (GET_MD_TYPE())
	{
	case OUT_DIR_4:
		UPDATE_MD1(d1, d2);
		break;
	case OUT2_DIR2:
		UPDATE_MD2(d1, d2);
		break;
	case OUT2_DIR4:
		UPDATE_MD3(d1, d2);
		break;

	default:
		Serial.print(F("Error : Motor driver is not set.\n"));
		Serial.print(F("        Please add SET_MD_TYPE.\n"));
		Serial.print(F("        SET_MD_TYPE(OUT_DIR_4)\n"));
		Serial.print(F("        or SET_MD_TYPE(OUT2_DIR2)\n"));
		Serial.print(F("        or SET_MD_TYPE(OUT2_DIR4)\n\n"));

		while (1)
			SET_BUZZER(70, 1000);
	}
}