// Created by Tomoki Sato
//=======================================================================
// 設定項目
//=======================================================================
#pragma once

// ユーザー設定
typedef struct
{
	uint8_t robot;					// ロボットタイプ[RC|AT]
	uint8_t controller = 0; // 使用するコントローラー[NRC|MC8|...]
	uint8_t motor_driver = 0;
} Config;
Config Cfg;

// ロボットのタイプ
enum robot_type
{
	RC, // ラジコン型
	AT	// 自立型
};

// 使用するコントローラー
enum controller_type
{
	FS_NRC, // 富士ソフト 新ラジ
	KO_MC8	// 近藤科学 MC8
};

// モータードライバの制御方式(dutyピンの数_方向指定ピンの数)
enum motor_driver_type
{
	OUT_DIR_4 = 1, // BTS7960 // モーター1つでduty指定と回転方向[0,1|1,0]を2ピンで指定する
	OUT2_DIR2,		 // 寺井さん基板 // モーター1つでduty指定が1ピン、回転方向[0|1]を1ピンで指定する
	OUT2_DIR4			 // 緑のヒートシンク基板 // モーター1つでduty指定が1ピン、回転方向[0,1|1,0]を2ピンで指定する
};

// ロボットタイプを指定(RC|AT)[0|1][ラ自立はRCを指定]
/**
 * @brief ロボットタイプを指定(RC|AT)[0|1][ラ自立はRCを指定]
 * 		RC : ラジコン型
 * 		AT : 自立型
 * @param type
 * 		RC : 0
 * 		AT : 1
 * @return void
 */
void SET_ROBOT_TYPE(bool type)
{
	Cfg.robot = type;
}

// [0|1] 指定したロボットタイプを取得
/**
 * @brief [0|1] 指定したロボットタイプを取得
 * 		RC : 0
 * 		AT : 1
 * @return bool
 * 		RC : 0
 * 		AT : 1
 * @note ラジコン型は0、自立型は1を返す
 */
bool GET_ROBOT_TYPE()
{
	return Cfg.robot;
}

// 使用するコントローラーを指定(NRC|MC8|...)[0|1|...]
/**
 * @brief 使用するコントローラーを指定(NRC|MC8|...)[0|1|...]
 * 		FS_NRC : 富士ソフト 新ラジ
 * 		KO_MC8 : 近藤科学 MC8
 * @param type
 * 		FS_NRC : 0
 * 		KO_MC8 : 1
 * @return void
 * @note 0:富士ソフト 新ラジ 1:近藤科学 MC8
 * @note 他のコントローラーを使用する場合は、ここに追加する
 */
void SET_CONTROLLER_TYPE(uint8_t type)
{
	Cfg.controller = type;
}

//[0|1|...] 指定したコントローラーを取得
/**
 * @brief [0|1|...] 指定したコントローラーを取得
 * 		FS_NRC : 富士ソフト 新ラジ
 * 		KO_MC8 : 近藤科学 MC8
 * @return uint8_t
 * 		FS_NRC : 0
 * 		KO_MC8 : 1
 * @note 0:富士ソフト 新ラジ 1:近藤科学 MC8
 *
 */
uint8_t GET_CONTROLLER_TYPE()
{
	return Cfg.controller;
}

// 使用するモータードライバを指定(OUT_DIR_4|OUT2_DIR2|OUT2_DIR4|...)[0|1|2|...]
/**
 * @brief 使用するモータードライバを指定(OUT_DIR_4|OUT2_DIR2|OUT2_DIR4|...)[0|1|2|...]
 * 		OUT_DIR_4 : BTS7960 // モーター1つでduty指定と回転方向[0,1|1,0]を2ピンで指定する
 * 		OUT2_DIR2 : 寺井さん基板 // モーター1つでduty指定が1ピン、回転方向[0|1]を1ピンで指定する
 * 		OUT2_DIR4 : 緑のヒートシンク基板 // モーター1つでduty指定が1ピン、回転方向[0,1|1,0]を2ピンで指定する
 * @param type
 * 		OUT_DIR_4 : 0
 * 		OUT2_DIR2 : 1
 * 		OUT2_DIR4 : 2
 * @return void
 * @note 0:BTS7960 1:寺井さん基板 2:緑のヒートシンク基板
 * @note 他のモータードライバを使用する場合は、ここに追加する
 * @note モータードライバのピン設定は、ここで行う
 */
void SET_MD_TYPE(uint8_t type)
{
	Cfg.motor_driver = type;

	switch (type)
	{
	case OUT_DIR_4:
		pinMode(MD1_M1_OUT1, OUTPUT);
		pinMode(MD1_M1_OUT2, OUTPUT);
		pinMode(MD1_M2_OUT1, OUTPUT);
		pinMode(MD1_M2_OUT2, OUTPUT);
		break;

	case OUT2_DIR2:
		pinMode(MD2_M1_OUT, OUTPUT);
		pinMode(MD2_M1_DIR, OUTPUT);
		pinMode(MD2_M2_OUT, OUTPUT);
		pinMode(MD2_M2_DIR, OUTPUT);
		break;

	case OUT2_DIR4:
		pinMode(MD3_M1_OUT, OUTPUT);
		pinMode(MD3_M2_OUT, OUTPUT);
		pinMode(MD3_M1_DIR1, OUTPUT);
		pinMode(MD3_M1_DIR2, OUTPUT);
		pinMode(MD3_M2_DIR1, OUTPUT);
		pinMode(MD3_M2_DIR2, OUTPUT);
		break;

	default: // 何してる！と警告メッセージ
		Serial.print(F("Error : motor driver type is not valid.\n"));
		Serial.print(F("        Please reconfigure SET_MD_TYPE.\n"));
		Serial.print(F("        SET_MD_TYPE(OUT_DIR_4)\n"));
		Serial.print(F("        or SET_MD_TYPE(OUT2_DIR2)\n"));
		Serial.print(F("        or SET_MD_TYPE(OUT2_DIR4)\n\n"));
		while (1)
			;
	}
}

//[0|1|2|...] 指定したモータードライバを取得
/**
 * @brief [0|1|2|...] 指定したモータードライバを取得
 * 		OUT_DIR_4 : BTS7960 // モーター1つでduty指定と回転方向[0,1|1,0]を2ピンで指定する
 * 		OUT2_DIR2 : 寺井さん基板 // モーター1つでduty指定が1ピン、回転方向[0|1]を1ピンで指定する
 * 		OUT2_DIR4 : 緑のヒートシンク基板 // モーター1つでduty指定が1ピン、回転方向[0,1|1,0]を2ピンで指定する
 * @return uint8_t
 * 		OUT_DIR_4 : 0
 * 		OUT2_DIR2 : 1
 * 		OUT2_DIR4 : 2
 * @note 0:BTS7960 1:寺井さん基板 2:緑のヒートシンク基板
 */
uint8_t GET_MD_TYPE()
{
	return Cfg.motor_driver;
}

// GPIO アクセス管理
//----------------------------------------------------------------------
// GPIOの番号最大値
// 例：Arduino nanoの場合、GPIO_MAXは21
#if defined(__AVR_ATmega328P__)
#define GPIO_MAX 21 // Arduino nano

// 例：ラズパイピコの場合、GPIO_MAXは28
#elif defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_ARCH_MBED)
#define GPIO_MAX 28 // ラズパイピコ

#else
#define GPIO_MAX 30

#endif

//[0|1] 指定したピンがモータードライバで使用されているかどうか判定(ピン番号)
/**
 * @brief [0|1] 指定したピンがモータードライバで使用されているかどうか判定(ピン番号)
 * @param uint8_t pin
 * @return bool
 * 		使用されている : 1
 * 		使用されていない : 0
 */
bool OUTPUT_PIN(uint8_t pin)
{
	if (pin == ARM_SOLENOID || pin == BUZZER)
		return 1;

	if (GET_MD_TYPE() == OUT_DIR_4)
	{
		switch (pin)
		{
		case MD1_M1_OUT1:
		case MD1_M1_OUT2:
		case MD1_M2_OUT1:
		case MD1_M2_OUT2:
			return 1;
			break;
		}
	}
	else if (GET_MD_TYPE() == OUT2_DIR2)
	{
		switch (pin)
		{
		case MD2_M1_OUT:
		case MD2_M1_DIR:
		case MD2_M2_OUT:
		case MD2_M2_DIR:
			return 1;
			break;
		}
	}
	else if (GET_MD_TYPE() == OUT2_DIR4)
	{
		switch (pin)
		{
		case MD3_M1_OUT:
		case MD3_M2_OUT:
		case MD3_M1_DIR1:
		case MD3_M1_DIR2:
		case MD3_M2_DIR1:
		case MD3_M2_DIR2:
			return 1;
			break;
		}
	}
	return 0;
}

//[0|1] 指定したピンが取得不可かどうか判定(ピン番号)
/**
 * @brief [0|1] 指定したピンが取得不可かどうか判定(ピン番号)
 * @param uint8_t pin
 * @return bool
 * 		取得不可 : 1
 * 		取得可能 : 0
 */
bool CANNOT_GET(uint8_t pin)
{
	if (pin > GPIO_MAX)
		return 1;

	if (OUTPUT_PIN(pin))
		return 1;

	// 取得不可リスト
	switch (pin)
	{
		// arduino nanoの通信用ピン
#if IS_ARDUINO_NANO

#elif IS_RP_PICO
	case 23: // 割当不可 23(SWDIO)
	case 24: // 割当不可 24(SWCLK)
	case 25: // 割当不可 25(SWIO)
		return 1;
		break;
#endif

	default:
		break;
	}

	return 0;
}

// 並列処理用ピン管理
bool multi_flag[GPIO_MAX + 1] = {false};

// 並列処理で使用するピンを設定(ピン番号)
/**
 * @brief 並列処理で使用するピンを設定(ピン番号)
 * @param uint8_t i
 * @return void
 */
void SET_MULTI_PORT(uint8_t i)
{
	if (i > GPIO_MAX)
		return;

	multi_flag[i] = true;
}

// 並列処理用のピンかどうか判定(ピン番号)
/**
 * @brief 並列処理用のピンかどうか判定(ピン番号)
 * @param uint8_t i
 * @return bool
 * 		並列処理用のピン : 1
 * 		並列処理用のピン以外 : 0
 */
bool MULTI_PORT(uint8_t i)
{
	if (i > GPIO_MAX)
		return false;
	else
		return multi_flag[i];
}