#include "lib/SD3mini.h" // SD3miniライブラリ

// --- ピン定義 ---
#define FSL = 4;	// 床センサー左
#define SL2 = A5; // 対物センサー左
#define SR1 = A4; // クロス対物センサー右
#define SL1 = A3; // クロス対物センサー左
#define SR2 = A2; // 対物センサー右
#define FSR = A0; // 床センサー右

// --- グローバル変数 ---
unsigned long actionStart = 0;		// アクション開始時刻
unsigned long actionDuration = 0; // アクション時間
bool inAction = false;						// アクション中フラグ

// スタートルーチン管理
bool startDone = false;
int startStep = 0;
unsigned long startStepStart = 0;

// --- スタートルーチン（ノンブロッキング版） ---
void runStartRoutine()
{
	unsigned long now = millis();

	switch (startStep)
	{
	case 0: // 5秒待機
		set_duty(0, 0);
		if (now - startStepStart >= 5000)
		{
			startStep = 1;
			startStepStart = now;
		}
		break;

	case 1: // 斜め右を向く (300ms)
		set_duty(1000, -1000);
		if (now - startStepStart >= 50)
		{
			startStep = 2;
			startStepStart = now;
		}
		break;

	case 2: // 前進 (500ms)
		set_duty(1000, 1000);
		if (now - startStepStart >= 100)
		{
			startStep = 3;
			startStepStart = now;
		}
		break;

	case 3: // 左旋回 (400ms)
		set_duty(-1000, 1000);
		if (now - startStepStart >= 100)
		{
			startStep = 4;
			startStepStart = now;
		}
		break;

	case 4: // 完了
		set_duty(0, 0);
		startDone = true;
		break;
	}
}

void setup()
{
	// ピンモード設定
	pinMode(SL1, INPUT);
	pinMode(SR1, INPUT);
	pinMode(SL2, INPUT);
	pinMode(SR2, INPUT);
	pinMode(FSR, INPUT);
	pinMode(FSL, INPUT);
	startStepStart = millis(); // スタートルーチン開始時刻

	SETUP_MOTOR();					 // モーター初期設定
	SET_MOTOR_RAMP(5);			 // モーターの加減速設定
	SET_MOTOR_RECHARGE(5);	 // モーターリミッター設定
	SET_MOTOR_REVERSE(1, 1); // モーター反転設定 変更なし
}

void loop()
{
	unsigned long now = millis();

	// --- スタートルーチン中なら進行 ---
	if (!startDone)
	{
		runStartRoutine();
		return;
	}

	// --- アクション中は継続処理 ---
	if (inAction)
	{
		if (now - actionStart >= actionDuration)
		{
			inAction = false; // アクション終了
		}
		else
		{
			return; // アクション中は他の処理を無視
		}
	}

	if (!dr(FSL))
	{
		set_duty(1000, -1000); // 白線検知 → 右へ逃げる
		actionStart = now;
		actionDuration = 75;
		inAction = true;
	}
	else if (!dr(FSR))
	{
		set_duty(-1000, 1000); // 白線検知 → 左へ逃げる
		actionStart = now;
		actionDuration = 75;
		inAction = true;
	}
	else if (!dr(SL1) && !dr(SR1))
	{
		set_duty(1000, 1000); // 相手検知 → 突進
	}
	else if (!dr(SL1))
	{
		set_duty(-100, 1000); // 相手左検知 → 左旋回
	}
	else if (!dr(SR1))
	{
		set_duty(1000, -100); // 相手右検知 → 右旋回
	}
	else if (!dr(SL2))
	{
		set_duty(-1000, 1000); // 相手左検知 → 左旋回
	}
	else if (!dr(SR2))
	{
		set_duty(1000, -1000); // 相手右検知 → 右旋回
	}
	else
	{
		set_duty(300, 300); // 通常前進
	}
}
