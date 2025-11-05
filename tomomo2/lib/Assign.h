// Created by Tomoki Sato
//=======================================================================
// 基板側 固定ピンアサイン
//=======================================================================
#pragma once

// 完全デジタル入出力方式を使用する場合はコメントアウトを外す
// #define OUTPUT_FULL_DIGITAL

// 基板固有ピンアサイン判定
//------------------------------------------------------
// Arduino nanoかどうか
#if defined(__AVR_ATmega328P__)
#define IS_ARDUINO_NANO 1
#else
#define IS_ARDUINO_NANO 0
#endif
// Arduino nano minizade基板かどうか
#if defined(__AVR_ATmega328P__) && defined(MINIZADE)
#define IS_MINIZADE 1
#else
#define IS_MINIZADE 0
#endif

// ラズパイピコかどうか
#if defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_ARCH_MBED)
#define IS_RP_PICO 1
#else
#define IS_RP_PICO 0
#endif

// RAK11300(RP2040-Zero)基板かどうか
#if defined(ARDUINO_RAKWIRELESS_RAK11300)
#define IS_RAK11300 1
#else
#define IS_RAK11300 0
#endif

// 基板固有ピンアサイン
//----------------------------------------------------------------------
#if IS_ARDUINO_NANO && !IS_MINIZADE // Arduino nano 標準基板
#define ARM_SOLENOID 13
#define BUZZER A5
#elif IS_MINIZADE // Arduino nano minizade基板
#define ARM_SOLENOID 22
#define BUZZER 13
#elif IS_RAK11300 // RP2040-Zero基板
#define ARM_SOLENOID 9
#define BUZZER 15
#ifndef LED_BUILTIN // LEDピン定義が無い場合
#define LED_BUILTIN 16
#endif
#elif IS_RP_PICO // ラズパイピコ
#define ARM_SOLENOID 22
#define BUZZER 28
#ifndef LED_BUILTIN
#define LED_BUILTIN 16
#endif
#endif

// モータードライバ
//----------------------------------------------------------------------
// OUT_DIR_4
// BTS7960, minizade, SD3mini基板など
// モーター1つでduty指定と回転方向[0,1|1,0]を各2ピンで指定する
#if IS_MINIZADE
#define MD1_M1_OUT1 3	 // m1duty正転
#define MD1_M1_OUT2 11 // m1duty逆転
#define MD1_M2_OUT1 9	 // m2duty正転
#define MD1_M2_OUT2 10 // m2duty逆転
#elif IS_RAK11300
#define MD1_M1_OUT1 26 // m1duty正転
#define MD1_M1_OUT2 27 // m1duty逆転
#define MD1_M2_OUT1 28 // m2duty正転
#define MD1_M2_OUT2 29 // m2duty逆転
#else
#define MD1_M1_OUT1 5	 // m1duty正転
#define MD1_M1_OUT2 6	 // m1duty逆転
#define MD1_M2_OUT1 9	 // m2duty正転
#define MD1_M2_OUT2 10 // m2duty逆転
#endif

// OUT2_DIR2
// 寺井さん基板など
//  モーター1つでduty指定が各1ピン、回転方向[0|1]を各1ピンで指定する
#define MD2_M1_OUT 5	// m1duty
#define MD2_M1_DIR 6	// m1回転方向
#define MD2_M2_OUT 9	// m2duty
#define MD2_M2_DIR 10 // m2回転方向

// OUT2_DIR4
// 緑のヒートシンク基板など
// モーター1つでduty指定が各1ピン、回転方向[0,1|1,0]を各2ピンで指定する
#define MD3_M1_DIR1 5 // m1正転
#define MD3_M1_DIR2 6 // m1逆転
#define MD3_M2_DIR1 7 // m2正転
#define MD3_M2_DIR2 8 // m2逆転
#define MD3_M1_OUT 9	// m1duty
#define MD3_M2_OUT 10 // m2duty

// DIPスイッチ
//----------------------------------------------------------------------
#define DIP1 18 // 最下位ビット
#define DIP2 19
#define DIP3 20
#define DIP4 21 // 最上位ビット
