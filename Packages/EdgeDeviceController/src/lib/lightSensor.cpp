#include <lib/lightSensor.h>

float LightSensor::lastReading = 0;

float LightSensor::read() {
    return LightSensor::lastReading = mapFloat(analogRead(LIGHT_SENSOR_PIN), 0, 800, 0, 100);
}
