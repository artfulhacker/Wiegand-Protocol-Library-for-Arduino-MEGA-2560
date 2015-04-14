#include "WiegandMega2560.h"

unsigned long WIEGAND::_sysTick=0;
unsigned long WIEGAND::_lastWiegand=0;
int			  WIEGAND::_GateActive=0;    // 1 = Active A   --   2 = Active B

unsigned long WIEGAND::_cardTempHighA=0;
unsigned long WIEGAND::_cardTempA=0;
unsigned long WIEGAND::_codeA=0;
int 		  WIEGAND::_bitCountA=0;	
int			  WIEGAND::_wiegandTypeA=0;

unsigned long WIEGAND::_cardTempHighB=0;
unsigned long WIEGAND::_cardTempB=0;
unsigned long WIEGAND::_codeB=0;
int 		  WIEGAND::_bitCountB=0;	
int			  WIEGAND::_wiegandTypeB=0;


WIEGAND::WIEGAND()
{
}

unsigned long WIEGAND::getCode()
{
  switch (_GateActive) {
    case 1:
		return _codeA;
      break;
    case 2:
		return _codeB;
      break;
    default: 
		return 0;
  }
}

int WIEGAND::getWiegandType()
{
  switch (_GateActive) {
    case 1:
		return _wiegandTypeA;
      break;
    case 2:
		return _wiegandTypeB;
      break;
    default: 
		return 0;
  }
	
}

int WIEGAND::getGateActive()
{
	return _GateActive;
}

bool WIEGAND::available()
{
	return DoWiegandConversion();
}

void WIEGAND::begin()
{
	_sysTick=millis();
	_lastWiegand = 0;
	
	_cardTempHighA = 0;
	_cardTempA = 0;
	_codeA = 0;
	_wiegandTypeA = 0;
	_bitCountA = 0;  
	
	
	_cardTempHighB = 0;
	_cardTempB = 0;
	_codeB = 0;
	_wiegandTypeB = 0;
	_bitCountB = 0;  

	pinMode(D0PinA, INPUT);					// Set D0 pin as input
	pinMode(D1PinA, INPUT);					// Set D1 pin as input
	attachInterrupt(4, ReadD0A, FALLING);	// Hardware interrupt - high to low pulse
	attachInterrupt(5, ReadD1A, FALLING);	// Hardware interrupt - high to low pulse
		
	pinMode(D0PinB, INPUT);					// Set D0 pin as input
	pinMode(D1PinB, INPUT);					// Set D1 pin as input
	attachInterrupt(2, ReadD0B, FALLING);	// Hardware interrupt - high to low pulse
	attachInterrupt(3, ReadD1B, FALLING);	// Hardware interrupt - high to low pulse
}

void WIEGAND::ReadD0A ()
{
	_bitCountA++;				// Increament bit count for Interrupt connected to D0
	if (_bitCountA>31)			// If bit count more than 31, process high bits
	{
		_cardTempHighA |= ((0x80000000 & _cardTempA)>>31);	//	shift value to high bits
		_cardTempHighA <<= 1;
		_cardTempA <<=1;
	}
	else
	{
		_cardTempA <<= 1;		// D0 represent binary 0, so just left shift card data
	}
	_lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}

void WIEGAND::ReadD1A()
{
	_bitCountA ++;				// Increment bit count for Interrupt connected to D1
	if (_bitCountA>31)			// If bit count more than 31, process high bits
	{
		_cardTempHighA |= ((0x80000000 & _cardTempA)>>31);	// shift value to high bits
		_cardTempHighA <<= 1;
		_cardTempA |= 1;
		_cardTempA <<=1;
	}
	else
	{
		_cardTempA |= 1;			// D1 represent binary 1, so OR card data with 1 then
		_cardTempA <<= 1;		// left shift card data
	}
	_lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}


void WIEGAND::ReadD0B ()
{
	_bitCountB++;				// Increament bit count for Interrupt connected to D0
	if (_bitCountB>31)			// If bit count more than 31, process high bits
	{
		_cardTempHighB |= ((0x80000000 & _cardTempB)>>31);	//	shift value to high bits
		_cardTempHighB <<= 1;
		_cardTempB <<=1;
	}
	else
	{
		_cardTempB <<= 1;		// D0 represent binary 0, so just left shift card data
	}
	_lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}

void WIEGAND::ReadD1B()
{
	_bitCountB ++;				// Increment bit count for Interrupt connected to D1
	if (_bitCountB>31)			// If bit count more than 31, process high bits
	{
		_cardTempHighB |= ((0x80000000 & _cardTempB)>>31);	// shift value to high bits
		_cardTempHighB <<= 1;
		_cardTempB |= 1;
		_cardTempB <<=1;
	}
	else
	{
		_cardTempB |= 1;			// D1 represent binary 1, so OR card data with 1 then
		_cardTempB <<= 1;		// left shift card data
	}
	_lastWiegand = _sysTick;	// Keep track of last wiegand bit received
}

unsigned long WIEGAND::GetCardId (unsigned long *codehigh, unsigned long *codelow, char bitlength)
{
	unsigned long cardID=0;

	if (bitlength==26)								// EM tag
		cardID = (*codelow & 0x1FFFFFE) >>1;

	if (bitlength==34)								// Mifare 
	{
		*codehigh = *codehigh & 0x03;				// only need the 2 LSB of the codehigh
		*codehigh <<= 30;							// shift 2 LSB to MSB		
		*codelow >>=1;
		cardID = *codehigh | *codelow;
	}
	return cardID;
}

bool WIEGAND::DoWiegandConversion ()
{
	unsigned long cardIDA;
	unsigned long cardIDB;
	
	_sysTick=millis();
	if ((_sysTick - _lastWiegand) > 25)								// if no more signal coming through after 25ms
	{
		if ((_bitCountA==26) || (_bitCountA==34)) 	// bitCount for keypress=8, Wiegand 26=26, Wiegand 34=34
		{
			_cardTempA >>= 1;			// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (_bitCountA>32)			// bit count more than 32 bits, shift high bits right to make adjustment
				_cardTempHighA >>= 1;	

			cardIDA = GetCardId (&_cardTempHighA, &_cardTempA, _bitCountA);
			_wiegandTypeA=_bitCountA;
			_bitCountA=0;
			_cardTempA=0;
			_cardTempHighA=0;
			_GateActive=1;
			_codeA=cardIDA;
			return true;				
		}
		else if ((_bitCountB==26) || (_bitCountB==34)) 	// bitCount for keypress=8, Wiegand 26=26, Wiegand 34=34
		{
			_cardTempB >>= 1;			// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (_bitCountB>32)			// bit count more than 32 bits, shift high bits right to make adjustment
				_cardTempHighB >>= 1;	

			cardIDB = GetCardId (&_cardTempHighB, &_cardTempB, _bitCountB);
			_wiegandTypeB=_bitCountB;
			_bitCountB=0;
			_cardTempB=0;
			_cardTempHighB=0;
			_GateActive=1;
			_codeB=cardIDB;
			return true;				
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			_lastWiegand=_sysTick;
			_bitCountA=0;			
			_cardTempA=0;
			_cardTempHighA=0;
			_bitCountB=0;			
			_cardTempB=0;
			_cardTempHighB=0;
			_GateActive=0;
			return false;
		}	
	}
	else
		return false;
}