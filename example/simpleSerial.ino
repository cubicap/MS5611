#include <Arduino.h>
#include <MS5611.h>

MS5611 baro = MS5611();

void setup() {
    Serial.begin(9600);
    baro.begin();
    
    Serial.println("Calibration data");
    uint16_t calibration[6];
    baro.getCalibration(calibration);
    for(int i = 0; i < 6; i++) {
        Serial.print("C");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(calibration[i]);
    }
}

void loop() {
    baro.update();
    Serial.print("Pressure: ");
    Serial.println(baro.getPressure());
    Serial.print("Temperature: ");
    Serial.println(baro.getTemperature());

    delay(1000);
}