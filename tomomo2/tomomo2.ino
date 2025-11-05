#define DEBUG_MODE		 // デバッグモード有効化
#define MINIZADE
#include "lib/SD3mini.h" // SD3miniライブラリ

// --- ピン定義 ---
#define BTN1 6 // プッシュボタン1
#define FSL 4  // 床センサー左
#define FSR A4 // 床センサー右

#define SL1 A3 // 対物センサー左
#define SR1 A2 // 対物センサー右
#define SR2 A1 // クロス対物センサー右
#define SL2 A0 // クロス対物センサー左

// --- グローバル変数 ---
unsigned long action_start = 0;	   // アクション開始時刻
unsigned long action_duration = 0; // アクション時間
bool in_action = false;			   // アクション中フラグ

int action_step = 0; // 現在のステップ番号
int action_total_steps = 0;

// スタートルーチン管理
bool start_done = false;
int motion_step = 0;
unsigned long timer_start = 0;

// 間欠運転用（追加）
const unsigned long INTERVAL_MS = 2000; // 2秒間隔
const unsigned long PULSE_MS = 100;		// 100msだけ動作
unsigned long intermittentStart = 0;

void setup()
{
#if __DEBUG__
	Serial.println("Debug Mode Enabled");
	Serial.begin(2000000);
	// Serial接続待機
	while (!Serial)
		;
#endif

	// ピンモード設定
	pinMode(SL1, INPUT_PULLUP);
	pinMode(SR1, INPUT_PULLUP);
	pinMode(SL2, INPUT_PULLUP);
	pinMode(SR2, INPUT_PULLUP);
	pinMode(FSR, INPUT_PULLUP);
	pinMode(FSL, INPUT_PULLUP);
	pinMode(BTN1, INPUT_PULLUP);
	// プッシュボタン押されて起動したら待機

	timer_start = millis(); // スタートルーチン開始時刻

	SET_MOTOR_RAMP(50);	   // モーターの加減速設定
	SET_MOTOR_RECHARGE(5); // モーターリミッター設定
	SET_MOTOR_REVERSE(0);  // モーター反転設定 変更なし
	SET_MD_TYPE(OUT_DIR_4);

	while (dr(BTN1))
	{
		sensor_check(); // センサー値の取得
	}

	// プッシュボタン押されたら5秒待機
	if (!dr(BTN1))
		delay(5000);
}

void sensor_check()
{
	// 対物センサー、床センサーの値を取得して表示
	Serial.print("SL1: ");
	Serial.print(dr(SL1));
	Serial.print(" SR1: ");
	Serial.print(dr(SR1));
	Serial.print(" SL2: ");
	Serial.print(dr(SL2));
	Serial.print(" SR2: ");
	Serial.print(dr(SR2));
	Serial.print(" FSR: ");
	Serial.print(dr(FSR));
	Serial.print(" FSL: ");
	Serial.print(dr(FSL));
	Serial.println();
}

// --- 複合アクション開始（シンプル版） ---
void start_compound_action(int steps)
{
	action_step = 0;
	action_total_steps = steps;
	action_start = millis();
	in_action = true;
}

void loop()
{
	unsigned long now_ms = millis();

	// --- アクション中は継続処理 ---
	if (in_action)
	{
		if (now_ms - action_start >= action_duration)
		{
			action_step++;

			// 全ステップ完了チェック
			if (action_step >= action_total_steps)
			{
				in_action = false;
				SET_DUTY(0, 0);
				return;
			}

			// 次のステップ開始
			action_start = now_ms;
		}
		else
		{
			return; // アクション中は他の処理を無視
		}
	}

	if (!dr(FSL))
	{
		// 白線左検知 → 後退してから右旋回（2ステップ）
		start_compound_action(2);

		if (action_step == 0)
		{
			SET_DUTY(-1000, -1000); // 後退
			action_duration = 100;
		}
		else if (action_step == 1)
		{
			SET_DUTY(1000, -1000); // 右旋回
			action_duration = 75;
		}
	}
	else if (!dr(FSR))
	{
		// 白線右検知 → 後退してから左旋回（2ステップ）
		start_compound_action(2);

		if (action_step == 0)
		{
			SET_DUTY(-1000, -1000); // 後退
			action_duration = 100;
		}
		else if (action_step == 1)
		{
			SET_DUTY(-1000, 1000); // 左旋回
			action_duration = 75;
		}
	}
	else if (!dr(SL1) && !dr(SR1))
	{
		SET_DUTY(1000, 1000); // 相手検知 → 突進
	}
	else if (!dr(SL1))
	{
		SET_DUTY(-100, 1000); // 相手左検知 → 左旋回
	}
	else if (!dr(SR1))
	{
		SET_DUTY(1000, -100); // 相手右検知 → 右旋回
	}
	else if (dr(SL2))
	{
		SET_DUTY(-1000, 1000); // 相手左検知 → 左旋回
	}
	else if (dr(SR2))
	{
		SET_DUTY(1000, -1000); // 相手右検知 → 右旋回
	}
	else
	{
		// 間欠運転: 2秒ごとに交互に片輪を100msだけduty=600で回す
		unsigned long elapsed = now_ms - intermittentStart;
		unsigned long phase = (elapsed / INTERVAL_MS) % 2; // 0 = left, 1 = right
		unsigned long inPhase = elapsed % INTERVAL_MS;

		if (inPhase < PULSE_MS)
		{
			if (phase == 0)
			{
				SET_DUTY(600, 0); // 左タイヤのみ動かす
			}
			else
			{
				SET_DUTY(0, 600); // 右タイヤのみ動かす
			}
		}
		else
		{
			SET_DUTY(0, 0); // 間欠以外は停止（必要なら低速前進に変更）
		}
	}
#if __DEBUG__
	Serial.print("M1 Duty: ");
	Serial.print(GET_M1_DUTY());
	Serial.print(" M2 Duty: ");
	Serial.print(GET_M2_DUTY());
	Serial.println();
#endif
}
