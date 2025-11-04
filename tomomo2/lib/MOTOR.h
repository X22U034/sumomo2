// Created by Tomoki Sato
#pragma once

// モーター
// --- ピン定義 ---
#define MZ_M1_PWM1 3	// 左モーター IN1
#define MZ_M1_PWM2 11 // 左モーター IN2
#define MZ_M2_PWM1 9	// 右モーター IN1
#define MZ_M2_PWM2 10 // 右モーター IN2

// PWM の最大値
#define MAX_DUTY 1000
#define PWM_MAX 255

typedef struct
{
	int duty1 = 0;
	int duty2 = 0;
	int ramp = 50;			// 1msあたりのduty増減値
	int recharge = 950; // 出力値のリミッター値(0~1000)
	int rev1 = 1;				// モーター反転(1:正転,-1:逆転)
	int rev2 = 1;
	int target1 = 0; // 各モータの目標値
	int target2 = 0;
} Motor;
Motor m;

/*
void SET_MOTOR_ENABLE(int n)
{
	digitalWrite(L_EN1,n);
	digitalWrite(R_EN1,n);
	digitalWrite(L_EN2,n);
	digitalWrite(R_EN2,n);
}
*/

// モーター初期設定
/**
 * @brief SETUP_MOTOR
 * モーター制御用ピンの初期設定を行う関数
 * OUTPUTモードに設定する
 * @return void
 */
void SETUP_MOTOR()
{
	pinMode(MZ_M1_PWM1, OUTPUT);
	pinMode(MZ_M1_PWM2, OUTPUT);
	pinMode(MZ_M2_PWM1, OUTPUT);
	pinMode(MZ_M2_PWM2, OUTPUT);
}

// モーターの駆動反転[1:正転,-1:反転]
/**
 * @brief SET_MOTOR_REVERSE
 * モーターの駆動反転を設定する関数
 * @param m1 モーター1の反転設定値(1 or -1)
 * @param m2 モーター2の反転設定値(1 or -1)
 * @return void
 * @note 例: SET_MOTOR_REVERSE(1, -1); // モーター1は正転、モーター2は反転
 */
void SET_MOTOR_REVERSE(int m1, int m2)
{
	if (m1 * m2 == -1 || m1 * m2 == 1) //[1 or -1]の検出
	{
		m.rev1 = m1;
		m.rev2 = m2;
	}
	else //[1,-1]以外の記述があったら
	{
		m.rev1 = 1;
		m.rev2 = 1;
	}
}

/**
 * @brief SET_RECHARGE
 * モーターのリミッター値を設定する関数
 * recharge値はduty比の最大値から百分率を引いた値で設定する
 * 例えば、リミッターを5%に設定する場合、recharge値は950となる
 * @param re リミッター値(0~100)
 * @return void
 * @note 範囲外の値が指定された場合、デフォルト値の950に設定される
 */
void SET_MOTOR_RECHARGE(int re)
{
	if (re < 0 || re > 100)
	{
		m.recharge = 1000 - 50;
	}
	else
	{
		m.recharge = 1000 - (re * 10);
	}
}

/**
 * @brief SET_MOTOR_RAMP
 * モーターのramp値を設定する関数
 * ramp値は1~200の範囲で設定可能
 * @param ramp 設定するramp値
 * @return void
 * @note 範囲外の値が指定された場合、デフォルト値の50に設定される
 * @note ramp値は1msあたりのduty増減値を表す
 * @note ramp値が小さいほど、モーターの加減速が緩やかになる
 * @note ramp値が大きいほど、モーターの加減速が急になる
 * @note 推奨値は20~100の範囲
 * @note 例: SET_MOTOR_RAMP(30); // ramp値を30に設定
 */
void SET_MOTOR_RAMP(int ramp)
{
	if (ramp < 1 || ramp > 200)
	{
		m.ramp = 50;
	}
	else
	{
		m.ramp = ramp;
	}
}

// 左右のモーターの出力duty比を取得
/**
 * @brief GET_M1PWMDUTY
 * モーター1の現在のPWM出力値を取得する関数
 * @return int モーター1のPWM出力値[-1000~1000]
 */
int GET_M1PWMDUTY()
{
	return m.target1;
}

/**
 * @brief GET_M2PWMDUTY
 * モーター2の現在のPWM出力値を取得する関数
 * @return int モーター2のPWM出力値[-1000~1000]
 */
int GET_M2PWMDUTY()
{
	return m.target2;
}

// MOTOR_BLUNT関数を改善（静的変数の追加）
/**
 * @brief MOTOR_BLUNT
 * モーターの出力を徐々に目標値へ変化させる関数
 * 1msごとに呼び出すことを想定している
 * 目標値と現在値の差に応じて、dutyをramp値ずつ増減させる
 * 目標値に達したら現在値を目標値に揃える
 * @return bool 値が更新された場合はtrue
 */
bool MOTOR_BLUNT()
{
	static unsigned long last_time = 0;
	bool updated = false;

	unsigned long current_time = millis();
	if (current_time - last_time >= 1) // 1ms以上経過した
	{
		// 更新時間を記録
		last_time = current_time;

		if (m.duty1 < m.target1)
		{
			m.duty1 += m.ramp;
			if (m.duty1 > m.target1) // dutyを加算して超えた場合
				m.duty1 = m.target1;	 // 目標値と揃える
			updated = true;
		}
		else if (m.duty1 > m.target1)
		{
			m.duty1 -= m.ramp;
			if (m.duty1 < m.target1)
				m.duty1 = m.target1;
			updated = true;
		}

		if (m.duty2 < m.target2)
		{
			m.duty2 += m.ramp;
			if (m.duty2 > m.target2)
				m.duty2 = m.target2;
			updated = true;
		}
		else if (m.duty2 > m.target2)
		{
			m.duty2 -= m.ramp;
			if (m.duty2 < m.target2)
				m.duty2 = m.target2;
			updated = true;
		}
	}

	return updated;
}

/**
 * @brief SET_M1PWMDUTY
 * モーター1のPWM出力を設定する関数
 * @param m1 PWM出力値[-1000~1000]
 * @return void
 */
void SET_M1PWMDUTY(int m1)
{
	static int d1 = 0;

	if (m1 > m.recharge)
		m1 = m.recharge;
	else if (m1 < -m.recharge)
		m1 = -m.recharge;

	m.target1 = m1;
	// MOTOR_BLUNT();

	d1 = map(m.duty1, -1000, 1000, -255, 255) * m.rev1;
	if (d1 > 0)
	{
		analogWrite(BTS7960_PWM_L1, d1);
		analogWrite(BTS7960_PWM_R1, 0);
	}
	else if (d1 < 0)
	{
		analogWrite(BTS7960_PWM_L1, 0);
		analogWrite(BTS7960_PWM_R1, -d1);
	}
	else
	{
		analogWrite(BTS7960_PWM_L1, 0);
		analogWrite(BTS7960_PWM_R1, 0);
	}
}

/**
 * @brief SET_M2PWMDUTY
 * モーター2のPWM出力を設定する関数
 * @param m2 PWM出力値[-1000~1000]
 * @return void
 */
void SET_M2PWMDUTY(int m2)
{
	static int d2 = 0;

	if (m2 > m.recharge)
		m2 = m.recharge;
	else if (m2 < -m.recharge)
		m2 = -m.recharge;

	m.target2 = m2;
	// MOTOR_BLUNT();

	d2 = map(m.duty2, -1000, 1000, -255, 255) * m.rev2;
	if (d2 > 0)
	{
		analogWrite(BTS7960_PWM_L2, d2);
		analogWrite(BTS7960_PWM_R2, 0);
	}
	else if (d2 < 0)
	{
		analogWrite(BTS7960_PWM_L2, 0);
		analogWrite(BTS7960_PWM_R2, -d2);
	}
	else
	{
		analogWrite(BTS7960_PWM_L2, 0);
		analogWrite(BTS7960_PWM_R2, 0);
	}
}

// 左右のモーターへ出力duty比[-1000~1000]を指定
/**
 * @brief SET_DUTY
 * モーターの出力duty比を設定する関数
 * @param m1 左モーターのduty比[-1000~1000]
 * @param m2 右モーターのduty比[-1000~1000]
 * @return void
 * @note duty比は-1000から1000の範囲で指定可能
 * @note 正の値は前進、負の値は後退を表す
 * @note 例: SET_DUTY(500, -500); // 左モーター前進、右モーター後退
 */
void SET_DUTY(int m1, int m2)
{
	SET_M1PWMDUTY(m1);
	SET_M2PWMDUTY(m2);
	MOTOR_BLUNT();
}

/**
 * @brief set_duty
 * モーターの出力duty比を設定する関数（滑らかな加減速機能付き）
 * @param m1duty 左モーターのduty比[-1000~1000]
 * @param m2duty 右モーターのduty比[-1000~1000]
 * @return void
 * @note duty比は-1000から1000の範囲で指定可能
 * @note 正の値は前進、負の値は後退を表す
 * @note 実際のモーター出力は徐々に変化する
 */
void set_duty(int m1duty, int m2duty)
{
	// リミッター処理
	if (m1duty > m.recharge)
		m1duty = m.recharge;
	else if (m1duty < -m.recharge)
		m1duty = -m.recharge;

	if (m2duty > m.recharge)
		m2duty = m.recharge;
	else if (m2duty < -m.recharge)
		m2duty = -m.recharge;

	// 反転設定を適用して目標値を設定
	m.target1 = m1duty * m.rev1;
	m.target2 = m2duty * m.rev2;

	// 滑らかな変化を適用
	if (MOTOR_BLUNT())
	{
		// 値が更新された場合のみモーター出力を変更
		// モーター1の出力設定
		if (m.duty1 >= 0)
		{
			analogWrite(MZ_M1_PWM1, map(m.duty1, 0, MAX_DUTY, 0, PWM_MAX));
			analogWrite(MZ_M1_PWM2, 0);
		}
		else
		{
			analogWrite(MZ_M1_PWM1, 0);
			analogWrite(MZ_M1_PWM2, map(-m.duty1, 0, MAX_DUTY, 0, PWM_MAX));
		}

		// モーター2の出力設定
		if (m.duty2 >= 0)
		{
			analogWrite(MZ_M2_PWM1, map(m.duty2, 0, MAX_DUTY, 0, PWM_MAX));
			analogWrite(MZ_M2_PWM2, 0);
		}
		else
		{
			analogWrite(MZ_M2_PWM1, 0);
			analogWrite(MZ_M2_PWM2, map(-m.duty2, 0, MAX_DUTY, 0, PWM_MAX));
		}
	}
}