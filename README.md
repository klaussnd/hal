# Hardware abstraction layer with common interface for AVR and Linux

This is a hardware abstraction layer written in C++ which aims at providing a common API for basic hardware functions for both [AVR](https://en.wikipedia.org/wiki/AVR_microcontrollers) microcontrollers and Linux based single-board computers like the Raspberry Pi or Beagle Bone. The aim is to have an API which is as similar as possible on both platforms, such that code on top of the API like sensor drivers can be re-used for both platforms.

The project uses C++17 and thus requires a C++17 compatible compiler. On AVR, you might need to [obtain a compatible toolchain](https://github.com/modm-io/avr-gcc), since the avr-gcc shipped with common Linux distributions is too old. For AVR, the [standard C++ library provided by modm](https://github.com/modm-io/avr-libstdcpp.git) and an [AVR CMake toolchain file](https://github.com/mkleemann/cmake-avr.git) are used as submodules.

The code is supposed to be used directly in projects, i.e. it is not intended to be compiled as a library. The reason is the code must be compiled for the exact microcontroller and hardware configuration at hand. As far as possible, all configuration is done at compile time. Therefore a general pre-compiled library is not applicable. Also, due to space limitations in microcontrollers, only the bare minimum of required code should be compiled in.

## Supported hardware

The following low-level hardware devices are currently supported:

* Both platforms:
    * i2c
    * usart
* AVR:
    * Abstraction for timers
    * Abstraction for I/O pin names
    * PWM
* Linux:
    * [CAN](https://en.wikipedia.org/wiki/CAN_bus) with an interface compatible to [avr-canlib](https://github.com/klaussnd/avr-can-lib)

The following sensors are currently supported:

* [Si1145 light sensor](doc/sensor-si1145.md) ([datasheet](https://www.silabs.com/documents/public/data-sheets/Si1145-46-47.pdf))
* VEML6075 UV sensor ([datasheet](https://cdn.sparkfun.com/assets/3/c/3/2/f/veml6075.pdf), [application note](https://cdn.sparkfun.com/assets/3/9/d/4/1/designingveml6075.pdf))
* HYT939 temperature and humidity sensor ([datasheet](https://www.ist-ag.com/sites/default/files/hyt939p.pdf))
* Si7021 temperature and humidity sensor ([datasheet](https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf))
* MH-Z19 CO2 sensor ([datasheet](https://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf))

# Tests and examples

In `tests`, there are test firmware for AVR (`avr`), test programs for Linux (`linux`), unit tests for testable components (`unit`), as well as scripts for the evaluation of test data acquisitions (`script`). The test firmware and apps can be used as examples on how to use the hardware abstraction layer.

# License

The code is licensed under the terms of the [GNU Affero General Public License](https://www.gnu.org/licenses/agpl-3.0.en.html).
