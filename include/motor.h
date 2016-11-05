#ifndef MOTOR_H_
#define MOTOR_H_

#include "ch.h"
#include "hal.h"
#include "vex.h"

typedef enum {
	/* 1 */  mClawThingy = kVexMotor_1,
	/* 2 */  mLiftHighLeft,
	/* 3 */  mLiftLowLeft,
	/* 4 */  mLiftHighRight,
	/* 5 */  mLiftLowRight,
	/* 6 */  mDriveFrontLeft,
	/* 7 */  mDriveBackLeft,
	/* 8 */  mDriveBackRight,
	/* 9 */  mDriveFrontRight,
	/* 10 */ mPickupThingy,
} motor_port_t;

constexpr const tVexImeChannels iLiftLowLeft   = kImeChannel_1;
constexpr const tVexImeChannels iLiftHighLeft  = kImeChannel_2;
constexpr const tVexImeChannels iLiftLowRight  = kImeChannel_3;
constexpr const tVexImeChannels iLiftHighRight = kImeChannel_4;

#define DIGI_CFG_DIGI_OUT(p) { kVexDigital_##p, kVexSensorDigitalOutput, kVexConfigOutput, 0 }
#define DIGI_CFG_DIGI_IN(p)  { kVexDigital_##p, kVexSensorDigitalInput,  kVexConfigInput,  0 }

#define MOTOR_CFG_MOT(p, t, r)   (tVexMotor)p, kVexMotor##t, kVexMotor##r
#define MOTOR_CFG_NOIME          kVexSensorNone, 0
#define MOTOR_CFG_IME(c)         kVexSensorIME,  c

#endif // MOTOR_H_
