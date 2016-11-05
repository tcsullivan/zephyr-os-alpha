#include <unistd.h>
#include <array>

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header

#include <motor.h>
#include <control.h>

//static WORKING_AREA(waVexIME, 512);
//static msg_t vexIME(void *);

// Digi IO configuration
static vexDigiCfg dConfig[] = {
};

static vexMotorCfg mConfig[] = {
	{ MOTOR_CFG_MOT(mClawThingy,      393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveFrontLeft,  393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveFrontRight, 393T, Reversed), MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveBackLeft,   393T, Reversed), MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mDriveBackRight,  393T, Normal),   MOTOR_CFG_NOIME },
	{ MOTOR_CFG_MOT(mLiftLowRight,    393T, Normal),   MOTOR_CFG_IME(iLiftLowRight)  },
	{ MOTOR_CFG_MOT(mLiftHighRight,   393T, Normal),   MOTOR_CFG_IME(iLiftHighRight) },
	{ MOTOR_CFG_MOT(mLiftLowLeft,     393T, Normal),   MOTOR_CFG_IME(iLiftLowLeft)   },
	{ MOTOR_CFG_MOT(mLiftHighLeft,    393T, Normal),   MOTOR_CFG_IME(iLiftHighLeft)  },
	{ MOTOR_CFG_MOT(mPickupThingy,    393T, Normal),   MOTOR_CFG_NOIME }
};

void vexUserSetup(void)
{
	vexDigitalConfigure(dConfig, DIG_CONFIG_SIZE(dConfig));
	vexMotorConfigure(mConfig, MOT_CONFIG_SIZE(mConfig));
}

void vexUserInit(void)
{}

#define AIRCR_ADDR		0xE000ED0C
#define VECTKEY			0x05FA
#define SYSRESETREQ		(1<<2)
#define VECTRESET		(1<<0)

void softwareReset(void){
	uint32_t AIRCR = *((uint32_t *)AIRCR_ADDR);
	AIRCR = (AIRCR & 0xFFFF) | (VECTKEY << 16) | SYSRESETREQ | VECTRESET;
	*((volatile uint32_t *)0xE000ED0C) = AIRCR;
	asm("DSB");
	while(1);
}

msg_t vexAutonomous(void* arg)
{
    (void)arg;

    vexTaskRegister("auton");

	vexMotorSet(mClawThingy, -127);
	vexMotorSet(mPickupThingy, -64);
	vexSleep(300);
	vexMotorSet(mClawThingy, 0);
	vexMotorSet(mPickupThingy, 0);

	vexMotorSet(mDriveFrontLeft,  -127);
	vexMotorSet(mDriveFrontRight, -127);
	vexMotorSet(mDriveBackLeft,   -127);
	vexMotorSet(mDriveBackRight,  -127);
	vexSleep(3000);
	vexMotorSet(mDriveFrontLeft,  30);
	vexMotorSet(mDriveFrontRight, 30);
	vexMotorSet(mDriveBackLeft,   30);
	vexMotorSet(mDriveBackRight,  30);
	vexSleep(1000);
	vexMotorSet(mDriveFrontLeft,  0);
	vexMotorSet(mDriveFrontRight, 0);
	vexMotorSet(mDriveBackLeft,   0);
	vexMotorSet(mDriveBackRight,  0);

    while(1)
        vexSleep(25);

    return (msg_t)0;
}

int doubleButton(int btnp, int btnn, int speed);

msg_t vexOperator(void* arg)
{
	Controller joyMain (1);

	(void)arg;

	vexTaskRegister("operator");

	//chThdCreateStatic(waVexIME, sizeof(waVexIME), NORMALPRIO - 1, vexIME, nullptr);

	while(!chThdShouldTerminate()) {

		// control update
		joyMain.update();

		// drive motors
		int dx = joyMain->Ch4, dy = -joyMain->Ch3;
		vexMotorSet(mDriveFrontLeft,  dy - dx);
		vexMotorSet(mDriveFrontRight, dy + dx);
		vexMotorSet(mDriveBackLeft,   dy - dx);
		vexMotorSet(mDriveBackRight,  dy + dx);

		// lift motors
#ifndef NEW_LIFT
		int ly = joyMain->Ch2;
		vexMotorSet(mLiftLowRight,  ly);
		vexMotorSet(mLiftHighRight, ly);
		vexMotorSet(mLiftLowLeft,   ly);
		vexMotorSet(mLiftHighLeft,  ly);
#else
		if (joyMain->Btn8U)
			motorCountInc();
		else if (joyMain->Btn7D)
			motorCountDec();
#endif // NEW_LIFT

		// lift thingy
		vexMotorSet(mPickupThingy, doubleButton(joyMain->Btn5U, joyMain->Btn5D, 64));

		// claw thingy
		vexMotorSet(mClawThingy, doubleButton(joyMain->Btn6U, joyMain->Btn6D, 127));

		if (joyMain->Btn8R)
			softwareReset();

		vexSleep(25);
	}

	return (msg_t)0;
}


int doubleButton(int btnp, int btnn, int speed)
{
	return (btnp ? speed : (btnn ? -speed : 0));
}

extern "C" {
	void _exit(int code) {
		(void)code;

		vexLcdPrintf(0, 0, "PANIC: exit(%d)", code);
		vexLcdPrintf(0, 1, "Halting...");

		while(1);
	}
	void _kill(pid_t pid) {
		(void)pid;
		// no way to kill here
	}
	pid_t _getpid(void) {
		// no pids here
		return 0;
	}
}

/*using CountTuple = std::tuple<tVexMotor, tVexImeChannels, int32_t>;

static std::array<CountTuple, 4> MotorCounts = {
	std::make_tuple(mLiftLowLeft,   iLiftLowLeft,   0),
	std::make_tuple(mLiftHighLeft,  iLiftHighLeft,  0),
	std::make_tuple(mLiftLowRight,  iLiftLowRight,  0),
	std::make_tuple(mLiftHighRight, iLiftHighRight, 0)
};

void
motorCountInc(void)
{
	for (auto &c : MotorCounts)
		std::get<2>(c) += 10;
}

void
motorCountDec(void)
{
	for (auto &c : MotorCounts)
		std::get<2>(c) -= 10;
}

static msg_t
vexIME(void *arg)
{
	(void)arg;

	vexTaskRegister("uime");

	while (1) {
		for (auto &c : MotorCounts) {
			auto count = vexImeGetPtr(std::get<1>(c))->count;
			auto comp = std::get<2>(c);

			if (count > comp)
				vexMotorSet(vexMotorGet(std::get<0>(c)) - 2);
			else if(count < comp)
				vexMotorSet(vexMotorGet(std::get<0>(c)) + 2);
		}

		vexSleep(100);
	}
}*/
