// Created by Tomoki Sato
//=======================================================================
// モータードライバ
//=======================================================================
#pragma once

// モータードライバの出力状態を更新する(出力値1,出力値2)[-255~255,-255~255]
void UPDATE_MD1(int m1duty, int m2duty)
{
	if (m1duty > 0)
	{
		SET_GPIO_PWM(MD1_M1_OUT1, m1duty);
		SET_GPIO_PWM(MD1_M1_OUT2, 0);
	}
	else if (m1duty < 0)
	{
		SET_GPIO_PWM(MD1_M1_OUT1, 0);
		SET_GPIO_PWM(MD1_M1_OUT2, -m1duty);
	}
	else
	{
		SET_GPIO_PWM(MD1_M1_OUT1, 0);
		SET_GPIO_PWM(MD1_M1_OUT2, 0);
	}

	if (m2duty > 0)
	{
		SET_GPIO_PWM(MD1_M2_OUT1, m2duty);
		SET_GPIO_PWM(MD1_M2_OUT2, 0);
	}
	else if (m2duty < 0)
	{
		SET_GPIO_PWM(MD1_M2_OUT1, 0);
		SET_GPIO_PWM(MD1_M2_OUT2, -m2duty);
	}
	else
	{
		SET_GPIO_PWM(MD1_M2_OUT1, 0);
		SET_GPIO_PWM(MD1_M2_OUT2, 0);
	}
}

// モータードライバの出力状態を更新する(出力値1,出力値2)[-255~255,-255~255]
void UPDATE_MD2(int m1duty, int m2duty)
{
	if (m1duty > 0)
	{
		SET_GPIO_OUT(MD2_M1_DIR, 0);
		SET_GPIO_PWM(MD2_M1_OUT, m1duty);
	}
	else if (m1duty < 0)
	{
		SET_GPIO_OUT(MD2_M1_DIR, 1);
		SET_GPIO_PWM(MD2_M1_OUT, -m1duty);
	}
	else
	{
		SET_GPIO_OUT(MD2_M1_DIR, 0);
		SET_GPIO_PWM(MD2_M1_OUT, 0);
	}

	if (m2duty > 0)
	{
		SET_GPIO_OUT(MD2_M2_DIR, 0);
		SET_GPIO_PWM(MD2_M2_OUT, m2duty);
	}
	else if (m2duty < 0)
	{
		SET_GPIO_OUT(MD2_M2_DIR, 1);
		SET_GPIO_PWM(MD2_M2_OUT, -m2duty);
	}
	else
	{
		SET_GPIO_OUT(MD2_M2_DIR, 0);
		SET_GPIO_PWM(MD2_M2_OUT, 0);
	}
}

// モータードライバの出力状態を更新する(出力値1,出力値2)[-255~255,-255~255]
void UPDATE_MD3(int m1duty, int m2duty)
{
	if (m1duty > 0)
	{
		SET_GPIO_PWM(MD3_M1_OUT, m1duty);
		SET_GPIO_OUT(MD3_M1_DIR1, 1);
		SET_GPIO_OUT(MD3_M1_DIR2, 0);
	}
	else if (m1duty < 0)
	{
		SET_GPIO_PWM(MD3_M1_OUT, -m1duty);
		SET_GPIO_OUT(MD3_M1_DIR1, 0);
		SET_GPIO_OUT(MD3_M1_DIR2, 1);
	}
	else
	{
		SET_GPIO_PWM(MD3_M1_OUT, 0);
		SET_GPIO_OUT(MD3_M1_DIR1, 0);
		SET_GPIO_OUT(MD3_M1_DIR2, 0);
	}

	if (m2duty > 0)
	{
		SET_GPIO_PWM(MD3_M2_OUT, m2duty);
		SET_GPIO_OUT(MD3_M2_DIR1, 1);
		SET_GPIO_OUT(MD3_M2_DIR2, 0);
	}
	else if (m2duty < 0)
	{
		SET_GPIO_PWM(MD3_M2_OUT, -m2duty);
		SET_GPIO_OUT(MD3_M2_DIR1, 0);
		SET_GPIO_OUT(MD3_M2_DIR2, 1);
	}
	else
	{
		SET_GPIO_PWM(MD3_M2_OUT, 0);
		SET_GPIO_OUT(MD3_M2_DIR1, 0);
		SET_GPIO_OUT(MD3_M2_DIR2, 0);
	}
}