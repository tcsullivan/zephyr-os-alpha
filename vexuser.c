#include <stdlib.h>
#include <math.h>

#define DIGI_CFG_DIGI_OUT(p) { kVexDigital_##p, kVexSensorDigitalOutput, kVexConfigOutput, 0 }
#define DIGI_CFG_DIGI_IN(p)  { kVexDigital_##p, kVexSensorDigitalInput,  kVexConfigInput,  0 }

#define MOTOR_CFG_MOT(p, t, r)   p, kVexMotor##t, kVexMotor##r
#define MOTOR_CFG_NOIME          kVexSensorNone, 0
#define MOTOR_CFG_IME(c)         kVexSensorIME,  kImeChannel_##c

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

// motor ports
#define mDriveFrontLeft		kVexMotor_6
#define mDriveFrontRight	kVexMotor_9
#define mDriveBackLeft		kVexMotor_7
#define mDriveBackRight		kVexMotor_8
#define mLiftLowRight		kVexMotor_5
#define mLiftHighRight		kVexMotor_4
#define mLiftLowLeft		kVexMotor_3
#define mLiftHighLeft		kVexMotor_2
#define mPickupThingy		kVexMotor_10
#define mClawThingy			kVexMotor_1

// Digi IO configuration
static vexDigiCfg dConfig[] = {
};

static vexMotorCfg mConfig[] = {
	{ MOTOR_CFG_MOT(mClawThingy,      393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveFrontLeft,  393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveFrontRight, 393T, Reversed), MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveBackLeft,   393T, Reversed), MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveBackRight,  393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mLiftLowRight,    393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mLiftHighRight,   393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mLiftLowLeft,     393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mLiftHighLeft,    393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mPickupThingy,    393T, Normal),   MOTOR_CFG_NOIME }
};

void vexUserSetup(void)
{
	vexDigitalConfigure(dConfig, DIG_CONFIG_SIZE(dConfig));
	vexMotorConfigure(mConfig, MOT_CONFIG_SIZE(mConfig));
}

void vexUserInit(void)
{}

msg_t vexAutonomous(void* arg)
{
    (void)arg;

    vexTaskRegister("auton");

	vexMotorSet(mClawThingy, -127);
	vexMotorSet(mPickupThingy, -64);
	vexSleep(300);
	vexMotorSet(mClawThingy, 0);
	vexMotorSet(mPickupThingy, 0);

	vexMotorSet(mDriveFrontLeft,  127);
	vexMotorSet(mDriveFrontRight, 127);
	vexMotorSet(mDriveBackLeft,   127);
	vexMotorSet(mDriveBackRight,  127);
	vexSleep(3000);
	vexMotorSet(mDriveFrontLeft,  -30);
	vexMotorSet(mDriveFrontRight, -30);
	vexMotorSet(mDriveBackLeft,   -30);
	vexMotorSet(mDriveBackRight,  -30);
	vexSleep(1000);
	vexMotorSet(mDriveFrontLeft,  0);
	vexMotorSet(mDriveFrontRight, 0);
	vexMotorSet(mDriveBackLeft,   0);
	vexMotorSet(mDriveBackRight,  0);

    while(1)
        vexSleep(25);

    return (msg_t)0;
}

typedef struct {
	int x, y;
} __attribute__ ((packed)) joy_t;

int doubleButton(int btnp, int btnn, int speed);

msg_t vexOperator(void* arg)
{
	static joy_t joyLeft, joyRight;

	(void)arg;

	vexTaskRegister("operator");

	while(!chThdShouldTerminate()) {

		// control update
		joyLeft  = (joy_t) {vexControllerGet(Ch4), vexControllerGet(Ch3)};
		joyRight = (joy_t) {vexControllerGet(Ch1), vexControllerGet(Ch2)};

		// drive motors
		vexMotorSet(mDriveFrontLeft,  joyLeft.y + joyLeft.x);
		vexMotorSet(mDriveFrontRight, joyLeft.y - joyLeft.x);
		vexMotorSet(mDriveBackLeft,   joyLeft.y + joyLeft.x);
		vexMotorSet(mDriveBackRight,  joyLeft.y - joyLeft.x);

		// lift motors
		vexMotorSet(mLiftLowRight,  joyRight.y);
		vexMotorSet(mLiftHighRight, joyRight.y);
		vexMotorSet(mLiftLowLeft,   joyRight.y);
		vexMotorSet(mLiftHighLeft,  joyRight.y);

		// lift thingy
		vexMotorSet(mPickupThingy, doubleButton(Btn5U, Btn5D, 64));

		// claw thingy
		vexMotorSet(mClawThingy, doubleButton(Btn6U, Btn6D, 127));

		vexSleep(25);
	}

	return (msg_t)0;
}


int doubleButton(int btnp, int btnn, int speed)
{
	return (vexControllerGet(btnp) ? speed : (vexControllerGet(btnn) ? -speed : 0));
}
