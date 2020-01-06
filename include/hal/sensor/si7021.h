// SI7021 temperature and humidity sensor
#pragma once

bool si7021Init();
/// @return Temperature in decicelsius
uint16_t si7021Temperature();
/// @return Relative humidity in percent
uint16_t si7021Humidity();
