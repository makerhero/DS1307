/*
  DS1307.h - Arduino library support for the DS1307 I2C Real-Time Clock
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
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
#ifndef DS1307_h
#define DS1307_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define DS1307_ADDR_R	209
#define DS1307_ADDR_W	208

#define FORMAT_SHORT	1
#define FORMAT_LONG		2

#define FORMAT_LITTLEENDIAN	1
#define FORMAT_BIGENDIAN	2
#define FORMAT_MIDDLEENDIAN	3

#define MONDAY		1
#define TUESDAY		2
#define WEDNESDAY	3
#define THURSDAY	4
#define FRIDAY		5
#define SATURDAY	6
#define SUNDAY		7

#define SQW_RATE_1		0
#define SQW_RATE_4K		1
#define SQW_RATE_8K		2
#define SQW_RATE_32K	3

class Time
{
public:
	uint8_t		hour;
	uint8_t		min;
	uint8_t		sec;
	uint8_t		date;
	uint8_t		mon;
	uint16_t	year;
	uint8_t		dow;

		Time();
};

class DS1307_RAM
{
public:
	byte	cell[56];

		DS1307_RAM();
};

class DS1307
{
public:
		DS1307(uint8_t data_pin, uint8_t sclk_pin);
	Time	getTime();
	void	setTime(uint8_t hour, uint8_t min, uint8_t sec);
	void	setDate(uint8_t date, uint8_t mon, uint16_t year);
	void	setDOW(uint8_t dow);

	char	*getTimeStr(uint8_t format=FORMAT_LONG);
	char	*getDateStr(uint8_t slformat=FORMAT_LONG, uint8_t eformat=FORMAT_LITTLEENDIAN, char divider='.');
	char	*getDOWStr(uint8_t format=FORMAT_LONG);
	char	*getMonthStr(uint8_t format=FORMAT_LONG);

	void	halt(bool value);
	void	setOutput(bool enable);
	void	enableSQW(bool enable);
	void	setSQWRate(int rate);

	void		writeBuffer(DS1307_RAM r);
	DS1307_RAM	readBuffer();
	void		poke(uint8_t addr, uint8_t value);
	uint8_t		peek(uint8_t addr);

private:
	uint8_t _scl_pin;
	uint8_t _sda_pin;
	uint8_t _burstArray[8];

	void	_sendStart(byte addr);
	void	_sendStop();
	void	_sendAck();
	void	_sendNack();
	void	_waitForAck();
	uint8_t	_readByte();
	void	_writeByte(uint8_t value);
	void	_burstRead();
	uint8_t	_readRegister(uint8_t reg);
	void 	_writeRegister(uint8_t reg, uint8_t value);
	uint8_t	_decode(uint8_t value);
	uint8_t	_decodeH(uint8_t value);
	uint8_t	_decodeY(uint8_t value);
	uint8_t	_encode(uint8_t vaule);
};
#endif
