#ifndef CONTROL_H_
#define CONTROL_H_

#include <stdint.h>

#include "ch.h"
#include "hal.h"
#include "vex.h"

typedef enum {
	BUTTON_UP,
	BUTTON_RELEASE,
	BUTTON_DOWN
} button_t;

struct joydata_t {
	int Ch1 :8;
	int Ch2 :8;
	int Ch3 :8;
	int Ch4 :8;

	struct accel_t {
		int y :8;
		int x :8;
		int z :8;
	} __attribute__ ((packed)) accel;

	char Btn5D	:1;
	char Btn5U  :1;
	char Btn6D  :1;
	char Btn6U  :1;

	char Reserved :4;

	char Btn8D	:1;
	char Btn8L  :1;
	char Btn8U  :1;
	char Btn8R  :1;
	char Btn7D	:1;
	char Btn7L  :1;
	char Btn7U  :1;
	char Btn7R  :1;

	char Reserved2[2];

} __attribute__ ((packed));

struct Controller {
private:
	int index;
	struct joydata_t* data;

public:
	Controller(int idx = 1)
		: index(idx), data(nullptr) {}
	~Controller(void) {}

	void update(void) {
		data = reinterpret_cast<struct joydata_t*>(vexSpiGetJoystickDataPtr(index));
	  	data->Ch1 = vexControllerGet(Ch1);//( data->Ch1 == 0xFF ) ? 127 : data->Ch1 - 127;
	  	data->Ch2 = vexControllerGet(Ch2);//-(( data->Ch2 == 0xFF ) ? 127 : data->Ch2 - 127);
	  	data->Ch3 = vexControllerGet(Ch3);//-(( data->Ch3 == 0xFF ) ? 127 : data->Ch3 - 127);
	  	data->Ch4 = vexControllerGet(Ch4);//( data->Ch4 == 0xFF ) ? 127 : data->Ch4 - 127;
	}
	
	inline const struct joydata_t* operator->(void) const 
	{ return data; }

} __attribute__ ((packed));

#endif // CONTROL_H_
