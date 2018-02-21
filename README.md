# omnibot-controller
Simple controller for Omnibot that applies velocities read over serial.

## Build and deploy
You can build and deploy the arduino script using the provided `Makefile`. For this you need to have the [Arduino Makefile](https://github.com/sudar/Arduino-Makefile) installed on your machine. Connect the arduino via USB and run `sudo make burn` in the `controller/` directory. Alternatively you can also use the Arduino IDE, but beware of the pin settings!

## Pin settings
The Firefly project uses three arduinos, each of which are wired in a different way to the actuators. For this reason the Arduino script has three sets of pin configurations embedded, and the selected configuration is determined using a `#define`. The provided `Makefile` will automatically read the serial of the connected Arduino, and set the appropriate `#define`. If however you use the Arduino IDE, pin configuration 1 will always be selected.

## Serial protocol
Communication runs at 115200 baud.

Send three floats (4 bytes each, litte-endian) `v_x`, `v_y`, and `v_psi`. 

**Nota bene**: No headers or terminators are used, the user must guarantee that exactly 12 bytes are sent per message, or trailing bytes will mess up new messages.
