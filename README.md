# omnibot-controller
Simple controller for Omnibot that applies velocities read over serial.

## Serial protocol
Communication runs at 115200 baud.

Send three floats (4 bytes each, litte-endian) `v_x`, `v_y`, and `v_psi`. 

**Nota bene**: No headers or terminators are used, the user must guarantee that exactly 12 bytes are sent per message, or trailing bytes will mess up new messages.
