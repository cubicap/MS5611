#include "MS5611.h"

#ifndef MS5611_PRESSURE_OSR
	#define MS5611_PRESSURE_OSR			4
#endif

#ifndef MS5611_TEMPERATURE_OSR
	#define MS5611_TEMPERATURE_OSR		4
#endif

#if MS5611_PRESSURE_OSR == 0
	#define MS5611_PRESSURE_DELAY		1
#elif MS5611_PRESSURE_OSR == 2
	#define MS5611_PRESSURE_DELAY		2
#elif MS5611_PRESSURE_OSR == 4
	#define MS5611_PRESSURE_DELAY		3
#elif MS5611_PRESSURE_OSR == 6
	#define MS5611_PRESSURE_DELAY		5
#elif MS5611_PRESSURE_OSR == 8
	#define MS5611_PRESSURE_DELAY		9
#else
	#define MS5611_PRESSURE_DELAY		MS5611_PRESSURE_OSR + 1
#endif

#if MS5611_TEMPERATURE_OSR == 0
	#define MS5611_TEMPERATURE_DELAY		1
#elif MS5611_TEMPERATURE_OSR == 2
	#define MS5611_TEMPERATURE_DELAY		2
#elif MS5611_TEMPERATURE_OSR == 4
	#define MS5611_TEMPERATURE_DELAY		3
#elif MS5611_TEMPERATURE_OSR == 6
	#define MS5611_TEMPERATURE_DELAY		5
#elif MS5611_TEMPERATURE_OSR == 8
	#define MS5611_TEMPERATURE_DELAY		9
#else
	#define MS5611_TEMPERATURE_DELAY		MS5611_TEMPERATURE_OSR + 1
#endif
		
#define MS5611_CMD_RESET 				0x1E
#define MS5611_CMD_ADC_READ				0x00
#define MS5611_CMD_CONV_D1		 		0x40
#define MS5611_CMD_CONV_D2 				0x50
#define MS5611_CMD_PROM_READ			0xA0
#define MS5611_NUM_BYTES_ADC			3
#define MS5611_NUM_BYTES_PROM			2
#define MS5611_ADDRESS					0x77

MS5611::MS5611() {
	temperature = 0;
	dT = 0;
	pressure = 0;
}

void MS5611::begin() {
	Wire.begin();
	reset();
	delay(50);
	readCalibration();
	
}

bool MS5611::update() {
	uint32_t D2 = getRawTemperature();
	dT = D2 - ((int32_t)calibration[4] * 256);
	temperature = 2000 + ((int64_t) dT * calibration[5]) / 8388608;
	
	uint32_t D1 = getRawPressure();
	int64_t OFF = (int64_t) calibration[1] * 65536 + (int64_t) calibration[3] * dT / 128;
	int64_t SENS = (int64_t) calibration[0] * 32768 + (int64_t) calibration[2] * dT / 256;
	pressure = (D1 * SENS / 2097152 - OFF) / 32768;
	
	return true;
}

uint32_t MS5611::getRawPressure() {
	sendCommand(MS5611_CMD_CONV_D1 | MS5611_PRESSURE_OSR);
	delay(MS5611_PRESSURE_DELAY);
	return readADC();
}

uint32_t MS5611::getRawTemperature() {
	sendCommand(MS5611_CMD_CONV_D2 | MS5611_TEMPERATURE_OSR);
	delay(MS5611_TEMPERATURE_DELAY);
	return readADC();
}

int32_t	MS5611::getPressure() {
	return pressure;
}

int32_t MS5611::getTemperature() {
	return temperature;
}

void MS5611::readCalibration() {
	for (uint8_t i = 0; i < 6; i++) {
		calibration[i] = readPROM(i + 1);
	}
}

void MS5611::getCalibration(uint16_t *C) {
	for (uint8_t i = 0; i < 6; i++) {
		C[i] = calibration[i];
	}
	return;
}

void MS5611::sendCommand(uint8_t cmd) {
	Wire.beginTransmission(MS5611_ADDRESS);
	Wire.write(cmd);
	Wire.endTransmission();
}

uint32_t MS5611::readADC() {
	sendCommand(MS5611_CMD_ADC_READ);
	Wire.beginTransmission(MS5611_ADDRESS);
	Wire.requestFrom(MS5611_ADDRESS, MS5611_NUM_BYTES_ADC);
	uint32_t data = 0;
	if (Wire.available() < MS5611_NUM_BYTES_ADC) {
		Wire.endTransmission();
		return 0;
	}

	for (int8_t i = MS5611_NUM_BYTES_ADC - 1; i >= 0; i--) {
		data |= ((uint32_t)Wire.read() << (8 * i));
	}

	Wire.endTransmission();
	return data;
}

uint16_t MS5611::readPROM(uint16_t address) {
	sendCommand(MS5611_CMD_PROM_READ | address<<1);

	Wire.beginTransmission(MS5611_ADDRESS);
	Wire.requestFrom(MS5611_ADDRESS, MS5611_NUM_BYTES_PROM);
	uint16_t data = 0;
	if (Wire.available() < MS5611_NUM_BYTES_PROM) {
		Wire.endTransmission();
		return 0;
	}
	for (int8_t i = MS5611_NUM_BYTES_PROM - 1; i >= 0; i--) {
		data |= ((uint16_t)Wire.read() << (8 * i));
	}
	Wire.endTransmission();
	return data;
}

void MS5611::reset() {
	sendCommand(MS5611_CMD_RESET);
	delay(3);
}
