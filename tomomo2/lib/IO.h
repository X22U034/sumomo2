// Created by Tomoki Sato
/*使用ピン設定*/
#pragma once

// Arduino nano と Raspberry Pi pico ピンアサイン互換
// A0 == 14
// A1 == 15
// A2 == 16
// A3 == 17
// A4 == 18
// A5 == 19
// A6 == 20
// A7 == 21

#ifdef DEBUG_MODE
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define RC 0
#define AT 1
#define TR_NRC 0 // 富士ソフト 新ラジ(New RC)
#define TR_MC8 1 // 近藤科学 MC-8

/* setup関数内で記述しておくこと*/
bool ROBOT_TYPE = RC;
bool CONTROLLER = TR_NRC;
byte GEAR_RATIO = 7;			 // 車体のギヤ比(四捨五入)
byte STICK_DEAD_ZONE = 10; // スティック不感帯(5~95%)
/*例
void setup()
{
	ROBOT_TYPE = RC;
	CONTROLLER = TR_NRC;
	GEAR_RATIO = 7;				 //車体のギヤ比(四捨五入)
	STICK_RESPONSE_ANGLE = 5; //スティックの反応角度
}
*/
/* setup関数内で記述しておくこと*/

/*入力ポート*/
#define L20 2
#define R20 3
#define L45 4
// 5
// 6
#define R45 7
#define FRONT 8
// 9
// 10
#define L_LINE 11
#define R_LINE 12
// 13
#define STICK1 14		 // ラジコン型 操作スティック
#define STICK2 15		 // ラジコン型 操作スティック
#define BTN1 16			 // 自立型 選手側セーフティ解除、停止、プログラム選択
#define BTN2 17			 // 自立型 選手側セーフティ解除、停止、プログラム選択
#define ST_MODULE 18 // 自立型 スタート・ストップモジュール
// 19

/*出力ポート*/
#define ARM_COIL 13 // アーム用ソレノイド
#define BUZZER 7		// ブザーピン
// BTS7960
#define BTS7960_PWM_L1 5
#define BTS7960_PWM_R1 6
#define BTS7960_PWM_L2 9
#define BTS7960_PWM_R2 10
// BTS7960駆動許可(ポート未定義)
#define L_EN1
#define R_EN1
#define L_EN2
#define R_EN2
// Pololu(死亡)
#define P36v20_PWM_H1 5
#define P36v20_DIR_1 6
#define P36v20_PWM_H2 9
#define P36v20_DIR_2 10

// 戦術(フラグ処理に使用)
enum Motions
{
	STRAIGHT = 1,
	CURVE_L = 2,
	CURVE_R = 3,
	BACK,
	TURN_L,
	TURN_R,
	Q_TURN,
	KUNOJI_L,
	KUNOJI_R,
	Q_KNJ_L,
	Q_KNJ_R,
	ATTACK,
	STAY,
	RETRY
};

typedef struct
{
	int hold; // 動作保持
	int rise; // 立ち上がり検出
} FLAG;
FLAG Flag; // Flagとして使う

unsigned long S_Time = 0; // 経過時間を一時保存
// start time

int T_count = 0;
int Tactics = 0;					// 戦術選択
int Port_state[22] = {0}; // GPIOポート

/*
void CFG_ROBOT_TYPE(bool robot_type)
{
	myCFG.ROBOT_TYPE = robot_type;
}

void CFG_CONTROLLER(bool controller)
{
	myCFG.CONTROLLER = controller;
}
void CFG_RC(int gear_ratio, int controller)
*/