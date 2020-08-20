# massduino_boards

This is an unofficial Arduino board support package for [MassDuino](http://www.inhaos.com/products.php?cPath=136)
boards produced by [INHAOS](http://www.inhaos.com/), based on official package V46r2 available [here](http://www.inhaos.com/download.php).

Besides packaging, this BSP includes a few trivial changes to add C++11 support, which is needed to compile
the latest Arduino libraries.

# Useful information

MassDuino topic on  [arduino.cc](https://forum.arduino.cc/index.php?topic=255512.0)

Key differences between MD-328D and ATmega328P:

 - Most MD-328D input pins have weak internal pullups (sourcing roughly 30-40 uA). Configuring a pin as `INPUT_PULLUP` increases the sourced current to 0.4-0.5 mA.
 
 - Sketches which require precise clock on MD-328D (notably, those using UART) must call `sysClock(EXT_OSC)` before using features relying on exact timing. This also means XTAL pins (D22 and D27) cannot be used as general-purpose pins.
 