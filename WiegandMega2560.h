#ifndef _WIEGAND_H
#define _WIEGAND_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define D0PinA 19			// Arduino Pin 19 Hardware interrupt
#define D1PinA 18			// Arduino Pin 18 Hardware interrupt

#define D0PinB 21			// Arduino Pin 20 Hardware interrupt
#define D1PinB 20			// Arduino Pin 21 Hardware interrupt


class WIEGAND {

public:
	WIEGAND();
	void begin();
	bool available();
	unsigned long getCode();
	int getWiegandType();
	int getGateActive();
	
private:
	static void ReadD0A();
	static void ReadD1A();
	static void ReadD0B();
	static void ReadD1B();			
	static bool DoWiegandConversion ();
	static unsigned long GetCardId (unsigned long *codehigh, unsigned long *codelow, char bitlength);
	
	static unsigned long 	_sysTick;
	static unsigned long 	_lastWiegand;
	static int				_GateActive;	
	
	static unsigned long 	_cardTempHighA;
	static unsigned long 	_cardTempA;
	static int				_bitCountA;	
	static int				_wiegandTypeA;
	static unsigned long	_codeA;
	
	static unsigned long 	_cardTempHighB;
	static unsigned long 	_cardTempB;
	static int				_bitCountB;	
	static int				_wiegandTypeB;
	static unsigned long	_codeB;
};

#endif
