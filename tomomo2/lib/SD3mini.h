// Created by Tomoki Sato
#pragma once

#ifdef DEBUG_MODE
#define __DEBUG__ 1
#else
#define __DEBUG__ 0
#endif

#include "fast_digital_rw.hpp"

#include "Assign.h"
#include "Config.h"
#include "GPIO.h"

#include "MMI/Buzzer.h"
#include "MMI/LED.h"
#include "MMI/Switch.h"
#include "MMI/MUSIC/Score.h"
#include "MMI/MUSIC/Player.h"

#include "MOTOR/Driver.h"
#include "MOTOR/Control.h"
