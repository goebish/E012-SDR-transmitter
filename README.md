# E012 SDR transmitter

Software defined radio transmitter for EAchine E012 mini quadcopter

[![Youtube video](https://img.youtube.com/vi/rZcHcuowGJU/0.jpg)](https://www.youtube.com/watch?v=rZcHcuowGJU)

### Usage:

- Open and run gfsk_tx.grc in gnuradio companion
- Build and run e012_transmitter.exe
- Connect a battery to the quadcopter
- Connect a gamepad / joystick, press start button to send bind packets
- Right analog trigger controls throttle
- Right stick X axis controls roll
- Right stick Y axis controls pitch
- Left stick X axis controls yaw

### Building e012_transmitter.exe:

Building the executable requires Qt 5.7 or + for gamepad support

- with Visual Studio 2015 + Qt integration addin: use e012_transmitter.sln 
- with qmake or Qt creator: use e012_transmitter.pro

This has been tested with gnuradio 3.7.11 under Windows and Linux with a BladeRF X40 SDR and wireless XBox 360 gamepad
