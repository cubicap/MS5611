#pragma once

#include <Arduino.h>
#include <Wire.h>

//Change these two values to change measurement resolution (value must be one of 0, 2, 4, 6, 8)
#define MS5611_PRESSURE_OSR				4		//0, 2, 4, 6, 8
#define MS5611_TEMPERATURE_OSR			4		//0, 2, 4, 6, 8

class MS5611 {
	public:
		MS5611();
		void begin();
		void reset();
		bool update();
		int32_t getTemperature();
		int32_t getPressure();
		void readCalibration();
		void getCalibration(uint16_t *);
	private:
		uint32_t getRawTemperature();
		uint32_t getRawPressure();
		void sendCommand(uint8_t);
		uint32_t readADC();
		uint16_t readPROM(uint16_t);
		int32_t pressure;
		int32_t temperature;
		int32_t dT;
		uint16_t calibration[6];
};
