/*
  DS1307.cpp - Arduino library support for the DS1307 I2C Real-Time Clock
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  Copyright (C)2018 Spacial. All right reserved
  
  This library has been made to easily interface and use the DS1307 RTC with
  the Arduino without needing the Wire library.

  You can find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/
#include "DS1307.h"

#define REG_SEC		0
#define REG_MIN		1
#define REG_HOUR	2
#define REG_DOW		3
#define REG_DATE	4
#define REG_MON		5
#define REG_YEAR	6
#define REG_CON		7

/* Public */

Time::Time()
{
	this->year = 2010;
	this->mon  = 1;
	this->date = 1;
	this->hour = 0;
	this->min  = 0;
	this->sec  = 0;
	this->dow  = 5;
}

DS1307_RAM::DS1307_RAM()
{
	for (int i=0; i<56; i++)
		cell[i]=0;
}

DS1307::DS1307(uint8_t data_pin, uint8_t sclk_pin)
{
	_sda_pin = data_pin;
	_scl_pin = sclk_pin;

	pinMode(_scl_pin, OUTPUT);
}
Time DS1307::getTime()
{
	Time t;
	_burstRead();
	t.sec	= _decode(_burstArray[0]);
	t.min	= _decode(_burstArray[1]);
	t.hour	= _decodeH(_burstArray[2]);
	t.dow	= _burstArray[3];
	t.date	= _decode(_burstArray[4]);
	t.mon	= _decode(_burstArray[5]);
	t.year	= _decodeY(_burstArray[6])+2000;
	return t;
}

void DS1307::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
	if (((hour>=0) && (hour<24)) && ((min>=0) && (min<60)) && ((sec>=0) && (sec<60)))
	{
		_writeRegister(REG_HOUR, _encode(hour));
		_writeRegister(REG_MIN, _encode(min));
		_writeRegister(REG_SEC, _encode(sec));
	}
}

void DS1307::setDate(uint8_t date, uint8_t mon, uint16_t year)
{
	if (((date>0) && (date<=31)) && ((mon>0) && (mon<=12)) && ((year>=2000) && (year<3000)))
	{
		year -= 2000;
		_writeRegister(REG_YEAR, _encode(year));
		_writeRegister(REG_MON, _encode(mon));
		_writeRegister(REG_DATE, _encode(date));
	}
}

void DS1307::setDOW(uint8_t dow)
{
	if ((dow>0) && (dow<8))
		_writeRegister(REG_DOW, dow);
}

char *DS1307::getTimeStr(uint8_t format)
{
	char *output= (char*)"xxxxxxxx";
	Time t;
	t=getTime();
	if (t.hour<10)
		output[0]=48;
	else
		output[0]=char((t.hour / 10)+48);
	output[1]=char((t.hour % 10)+48);
	output[2]=58;
	if (t.min<10)
		output[3]=48;
	else
		output[3]=char((t.min / 10)+48);
	output[4]=char((t.min % 10)+48);
	output[5]=58;
	if (format==FORMAT_SHORT)
		output[5]=0;
	else
	{
	if (t.sec<10)
		output[6]=48;
	else
		output[6]=char((t.sec / 10)+48);
	output[7]=char((t.sec % 10)+48);
	output[8]=0;
	}
	return output;
}

char *DS1307::getDateStr(uint8_t slformat, uint8_t eformat, char divider)
{
	char *output= (char*)"xxxxxxxxxx";
	int yr, offset;
	Time t;
	t=getTime();
	switch (eformat)
	{
		case FORMAT_LITTLEENDIAN:
			if (t.date<10)
				output[0]=48;
			else
				output[0]=char((t.date / 10)+48);
			output[1]=char((t.date % 10)+48);
			output[2]=divider;
			if (t.mon<10)
				output[3]=48;
			else
				output[3]=char((t.mon / 10)+48);
			output[4]=char((t.mon % 10)+48);
			output[5]=divider;
			if (slformat==FORMAT_SHORT)
			{
				yr=t.year-2000;
				if (yr<10)
					output[6]=48;
				else
					output[6]=char((yr / 10)+48);
				output[7]=char((yr % 10)+48);
				output[8]=0;
			}
			else
			{
				yr=t.year;
				output[6]=char((yr / 1000)+48);
				output[7]=char(((yr % 1000) / 100)+48);
				output[8]=char(((yr % 100) / 10)+48);
				output[9]=char((yr % 10)+48);
				output[10]=0;
			}
			break;
		case FORMAT_BIGENDIAN:
			if (slformat==FORMAT_SHORT)
				offset=0;
			else
				offset=2;
			if (slformat==FORMAT_SHORT)
			{
				yr=t.year-2000;
				if (yr<10)
					output[0]=48;
				else
					output[0]=char((yr / 10)+48);
				output[1]=char((yr % 10)+48);
				output[2]=divider;
			}
			else
			{
				yr=t.year;
				output[0]=char((yr / 1000)+48);
				output[1]=char(((yr % 1000) / 100)+48);
				output[2]=char(((yr % 100) / 10)+48);
				output[3]=char((yr % 10)+48);
				output[4]=divider;
			}
			if (t.mon<10)
				output[3+offset]=48;
			else
				output[3+offset]=char((t.mon / 10)+48);
			output[4+offset]=char((t.mon % 10)+48);
			output[5+offset]=divider;
			if (t.date<10)
				output[6+offset]=48;
			else
				output[6+offset]=char((t.date / 10)+48);
			output[7+offset]=char((t.date % 10)+48);
			output[8+offset]=0;
			break;
		case FORMAT_MIDDLEENDIAN:
			if (t.mon<10)
				output[0]=48;
			else
				output[0]=char((t.mon / 10)+48);
			output[1]=char((t.mon % 10)+48);
			output[2]=divider;
			if (t.date<10)
				output[3]=48;
			else
				output[3]=char((t.date / 10)+48);
			output[4]=char((t.date % 10)+48);
			output[5]=divider;
			if (slformat==FORMAT_SHORT)
			{
				yr=t.year-2000;
				if (yr<10)
					output[6]=48;
				else
					output[6]=char((yr / 10)+48);
				output[7]=char((yr % 10)+48);
				output[8]=0;
			}
			else
			{
				yr=t.year;
				output[6]=char((yr / 1000)+48);
				output[7]=char(((yr % 1000) / 100)+48);
				output[8]=char(((yr % 100) / 10)+48);
				output[9]=char((yr % 10)+48);
				output[10]=0;
			}
			break;
	}
	return output;
}

char *DS1307::getDOWStr(uint8_t format)
{
	char *output= (char*)"xxxxxxxxx";
	Time t;
	t=getTime();
	switch (t.dow)
	{
		case MONDAY:
			output=(char*)"Monday";
			break;
		case TUESDAY:
			output=(char*)"Tuesday";
			break;
		case WEDNESDAY:
			output=(char*)"Wednesday";
			break;
		case THURSDAY:
			output=(char*)"Thursday";
			break;
		case FRIDAY:
			output=(char*)"Friday";
			break;
		case SATURDAY:
			output=(char*)"Saturday";
			break;
		case SUNDAY:
			output=(char*)"Sunday";
			break;
	}     
	if (format==FORMAT_SHORT)
		output[3]=0;
	return output;
}

char *DS1307::getMonthStr(uint8_t format)
{
	char *output= (char*)"xxxxxxxxx";
	Time t;
	t=getTime();
	switch (t.mon)
	{
		case 1:
			output=(char*)"January";
			break;
		case 2:
			output=(char*)"February";
			break;
		case 3:
			output=(char*)"March";
			break;
		case 4:
			output=(char*)"April";
			break;
		case 5:
			output=(char*)"May";
			break;
		case 6:
			output=(char*)"June";
			break;
		case 7:
			output=(char*)"July";
			break;
		case 8:
			output=(char*)"August";
			break;
		case 9:
			output=(char*)"September";
			break;
		case 10:
			output=(char*)"October";
			break;
		case 11:
			output=(char*)"November";
			break;
		case 12:
			output=(char*)"December";
			break;
	}     
	if (format==FORMAT_SHORT)
		output[3]=0;
	return output;
}

void DS1307::halt(bool enable)
{
  uint8_t _reg = _readRegister(REG_SEC);
  _reg &= ~(1 << 7);
  _reg |= (enable << 7);
  _writeRegister(REG_SEC, _reg);
}

void DS1307::setOutput(bool enable)
{
  uint8_t _reg = _readRegister(REG_CON);
  _reg &= ~(1 << 7);
  _reg |= (enable << 7);
  _writeRegister(REG_CON, _reg);
}


void DS1307::enableSQW(bool enable)
{
  uint8_t _reg = _readRegister(REG_CON);
  _reg &= ~(1 << 4);
  _reg |= (enable << 4);
  _writeRegister(REG_CON, _reg);
}

void DS1307::setSQWRate(int rate)
{
  uint8_t _reg = _readRegister(REG_CON);
  _reg &= ~(3);
  _reg |= (rate);
  _writeRegister(REG_CON, _reg);
}

/* Private */

void	DS1307::_sendStart(byte addr)
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, LOW);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, addr);
}

void	DS1307::_sendStop()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_sda_pin, HIGH);
	pinMode(_sda_pin, INPUT);
}

void	DS1307::_sendNack()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, HIGH);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void	DS1307::_sendAck()
{
	pinMode(_sda_pin, OUTPUT);
	digitalWrite(_scl_pin, LOW);
	digitalWrite(_sda_pin, LOW);
	digitalWrite(_scl_pin, HIGH);
	digitalWrite(_scl_pin, LOW);
	pinMode(_sda_pin, INPUT);
}

void	DS1307::_waitForAck()
{
	pinMode(_sda_pin, INPUT);
	digitalWrite(_scl_pin, HIGH);
	while (_sda_pin==LOW) {}
	digitalWrite(_scl_pin, LOW);
}

uint8_t DS1307::_readByte()
{
	pinMode(_sda_pin, INPUT);

	uint8_t value = 0;
	uint8_t currentBit = 0;

	for (int i = 0; i < 8; ++i)
	{
		digitalWrite(_scl_pin, HIGH);
		currentBit = digitalRead(_sda_pin);
		value |= (currentBit << 7-i);
		delayMicroseconds(1);
		digitalWrite(_scl_pin, LOW);
	}
	return value;
}
void DS1307::_writeByte(uint8_t value)
{
	pinMode(_sda_pin, OUTPUT);
	shiftOut(_sda_pin, _scl_pin, MSBFIRST, value);
}

uint8_t DS1307::_readRegister(uint8_t reg)
{
	uint8_t	readValue=0;

	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(reg);
	_waitForAck();
	_sendStop();
	_sendStart(DS1307_ADDR_R);
	_waitForAck();
	readValue = _readByte();
	_sendNack();
	_sendStop();
	return readValue;
}

void DS1307::_writeRegister(uint8_t reg, uint8_t value)
{
	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(reg);
	_waitForAck();
	_writeByte(value);
	_waitForAck();
	_sendStop();
}

void DS1307::_burstRead()
{
	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(0);
	_waitForAck();
	_sendStop();
	_sendStart(DS1307_ADDR_R);
	_waitForAck();

	for (int i=0; i<8; i++)
	{
		_burstArray[i] = _readByte();
		if (i<7)
			_sendAck();
		else
			_sendNack();
	}
	_sendStop();
}

uint8_t	DS1307::_decode(uint8_t value)
{
	uint8_t decoded = value & 127;
	decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
	return decoded;
}

uint8_t DS1307::_decodeH(uint8_t value)
{
  if (value & 128)
    value = (value & 15) + (12 * ((value & 32) >> 5));
  else
    value = (value & 15) + (10 * ((value & 48) >> 4));
  return value;
}

uint8_t	DS1307::_decodeY(uint8_t value)
{
	uint8_t decoded = (value & 15) + 10 * ((value & (15 << 4)) >> 4);
	return decoded;
}

uint8_t DS1307::_encode(uint8_t value)
{
	uint8_t encoded = ((value / 10) << 4) + (value % 10);
	return encoded;
}

void DS1307::writeBuffer(DS1307_RAM r)
{
	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(8);
	_waitForAck();

	for (int i=0; i<56; i++)
	{
		_writeByte(r.cell[i]);
		_waitForAck();
	}

	_sendStop();
}

DS1307_RAM DS1307::readBuffer()
{
	DS1307_RAM r;

	_sendStart(DS1307_ADDR_W);
	_waitForAck();
	_writeByte(8);
	_waitForAck();
	_sendStop();
	_sendStart(DS1307_ADDR_R);
	_waitForAck();

	for (int i=0; i<56; i++)
	{
		r.cell[i] = _readByte();
		if (i<55)
			_sendAck();
		else
			_sendNack();
	}
	_sendStop();

	return r;
}

void DS1307::poke(uint8_t addr, uint8_t value)
{
	if ((addr >=0) && (addr<=55))
	{
		addr += 8;
		_sendStart(DS1307_ADDR_W);
		_waitForAck();
		_writeByte(addr);
		_waitForAck();
		_writeByte(value);
		_waitForAck();
		_sendStop();
	}
}

uint8_t DS1307::peek(uint8_t addr)
{
	if ((addr >=0) && (addr<=55))
	{
		uint8_t readValue;

		addr += 8;
		_sendStart(DS1307_ADDR_W);
		_waitForAck();
		_writeByte(addr);
		_waitForAck();
		_sendStop();
		_sendStart(DS1307_ADDR_R);
		_waitForAck();
		readValue = _readByte();
		_sendNack();
		_sendStop();

		return readValue;
	}
	else
		return 0;
}

