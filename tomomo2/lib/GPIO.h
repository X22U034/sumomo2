// Created by Tomoki Sato
//=======================================================================
// デジタル入出力
//=======================================================================
#pragma once

//  pwm周波数を指定(周波数)[15000~60000][hz]
/**
 * @brief pwm周波数を指定(周波数)[15000~60000][hz]
 * @param Hz
 * @return void
 */
void SET_PWM_FREQ(uint16_t Hz)
{
#if IS_ARDUINO_NANO && !defined(OUTPUT_FULL_DIGITAL)
	// if (Hz > 245)
	// AVR_SET_PWM9_PWM10FREQ(Hz);

#elif IS_RP_PICO
	if (Hz > 2045)
		analogWriteFreq(Hz);

#endif
}

//[0|1] 入力状態を取得する(ピン番号)
/**
 * @brief [0|1] 入力状態を取得する(ピン番号)
 * @param uint8_t pin
 * @return bool
 * 		LOW : 0
 * 		HIGH : 1
 */
bool GET_GPIO_IN(uint8_t pin)
{
#if IS_ARDUINO_NANO
	return dr(pin);

#elif IS_RP_PICO
	return gpio_get(pin);

#else
	return digitalRead(pin);

#endif
}

// LOWかHIGHの電圧を出力(ピン番号,LOW|HIGH)[0|1]
/**
 * @brief LOWかHIGHの電圧を出力(ピン番号,LOW|HIGH)[0|1]
 * @param uint8_t pin
 * @param bool val
 * @return void
 */
void SET_GPIO_OUT(uint8_t pin, bool val)
{
#if IS_ARDUINO_NANO
	// fastestDigitalWrite(pin, val);
	dw(pin, val);

#elif IS_RP_PICO
	gpio_put(pin, val);

#else
	digitalWrite(pin, val);

#endif
}

//[1000~2000] パルス幅をマイクロ秒で取得(ピン番号)
/**
 * @brief [1000~2000] パルス幅をマイクロ秒で取得(ピン番号)
 * @param uint8_t pin
 * @return int
 */
int GET_GPIO_PULSE(uint8_t pin)
{
#if IS_ARDUINO_NANO
	int pulse = pulseIn(pin, HIGH, 30000);

#elif IS_RP_PICO
	int pulse = pulseIn(pin, HIGH, 30000);
	// int pulse = pulseRead(pin);

#else
	int pulse = pulseIn(pin, HIGH, 30000);

#endif
	return pulse;
	return (pulse > 0) ? pulse : 1500;
}

// 指定したピンからアナログ値を出力(ピン番号,アナログ値)[pin,0~255]
/**
 * @brief 指定したピンからアナログ値を出力(ピン番号,アナログ値)[pin,0~255]
 * @param uint8_t pin
 * @param uint8_t val
 * @return void
 */
void SET_GPIO_PWM(uint8_t pin, uint8_t val)
{
// 完全デジタル入出力方式の場合
#if defined(OUTPUT_FULL_DIGITAL)
	SET_GPIO_OUT(pin, (val > 0) ? 1 : 0);

#elif IS_ARDUINO_NANO
	// if (pin == 9)
	// 	AVR_SET_PWM9DUTY(val);
	// else if (pin == 10)
	// 	AVR_SET_PWM10DUTY(val);
	// else
	analogWrite(pin, val);

#elif IS_RP_PICO
	analogWrite(pin, val);

#else
	analogWrite(pin, val);

#endif
	// Serial.print(val), Serial.print(" ");
}
