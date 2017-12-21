/**
 * Low-level controller for omnibot.
 * Applies velocities (x, y, psi) read over serial to the three motors.
 *
 * Maintainer: Daan de Graaf
**/

#define NR_MOTORS 3
// Motor pins

#ifndef PINS_1
#ifndef PINS_2
#ifndef PINS_3

#define PINS_3

#endif
#endif
#endif

#ifdef PINS_1
static const int PWM_PINS[NR_MOTORS] = {3, 5, 6};
static const int DIRECTION_PINS[NR_MOTORS] = {2, 4, 7};
#define MOTOR_GAIN 1
#endif

#ifdef PINS_2
static const int PWM_PINS[NR_MOTORS] = {3, 5, 6};
static const int DIRECTION_PINS[NR_MOTORS] = {2, 9, 10};
#define MOTOR_GAIN 1
#endif

#ifdef PINS_3
static const int PWM_PINS[NR_MOTORS] = {3, 6, 9};
static const int DIRECTION_PINS[NR_MOTORS] = {2, 5, 8};
// Robot three's motor drivers have one additional pin per motor that needs to be the inverse of the direction pin
static const int DIRECTION_PINS_INV[NR_MOTORS] = {4, 7, 10};
// TODO probably not very accurate
#define MOTOR_GAIN 4
#endif


// Button to run test sequence
#define DEBUG_BUTTON_PIN A0

struct Command {
    float x;
    float y;
    float psi;
};

/**
 * Reads a new command from Serial, 
 * and modifies the Command passed as a parameter with the new values.
 * 
 * Returns: whether or not a new command was received.
 * If no new command is received, `cmd` is not modified
 */
bool getCommand(Command *cmd) {
    // Need to receive 3 floats
    int bufferSize = NR_MOTORS * sizeof(float);
    char buffer[bufferSize];
    // Only read when all data for one command is available
    if(Serial.available() >= bufferSize) {
        Serial.readBytes(buffer, bufferSize);
        float* casted = (float*) buffer;
        // N.B. signs of x and y are flipped for convention
        cmd->x = -casted[0];
        cmd->y = -casted[1];
        cmd->psi = casted[2];
        return true;
    }
    return false;
}

void setMotor(int index, float velocity) {
    // Set turning direction
    bool positive = velocity > 0;
    digitalWrite(DIRECTION_PINS[index], positive ? HIGH : LOW);
    #ifdef PINS_3
    digitalWrite(DIRECTION_PINS_INV[index], positive ? LOW : HIGH);
    #endif

    // Set PWM value
    int pwm = constrain(abs(velocity * MOTOR_GAIN), 0, 255);
    analogWrite(PWM_PINS[index], pwm);
}

void applyCommand(Command *cmd) {
    float v[NR_MOTORS];

    // TODO these values are probably not 100% correct
    v[0] = (1/2.0) * cmd->x - (1/3.0) * cmd->y - (1/3.0) * cmd->psi;
    v[0] = -v[0]; // Convention
    v[1] = (1/2.0) * cmd->x + (1/3.0) * cmd->y + (1/3.0) * cmd->psi;
    v[2] = 0       * cmd->x - (2/3.0) * cmd->y + (1/3.0) * cmd->psi;

    float m = 0;
    for(float val : v) {
        m = max(m, abs(val));
    }
    if (m > 255) {
        for(int i = 0; i < NR_MOTORS; i++) {
            v[i] /= m / 255;
        }
    }
    
    for(int i = 0; i < NR_MOTORS; i++) {
        setMotor(i, v[i]);
    }
}

void runTestSequence() {
    Command cmd;
    cmd.x = 0;
    cmd.y = 0;
    cmd.psi = 0;
    applyCommand(&cmd);

    delay(2000);

    cmd.x = 200.0;
    applyCommand(&cmd);
    delay(2000);    
    cmd.x = 0;
    applyCommand(&cmd);
    delay(200);


    cmd.y = 200.0;
    applyCommand(&cmd);
    delay(2000);
    cmd.y = 0;
    applyCommand(&cmd);
    delay(200);

    cmd.psi = 200;
    applyCommand(&cmd);
    delay(6700);
    cmd.psi = 0;
    applyCommand(&cmd);
    delay(200);
}

void setup() {
    // For communication with the Raspberry Pi
    Serial.begin(115200);

    // Setup button as input (PULLUP to use internal resistor)
    pinMode(DEBUG_BUTTON_PIN, INPUT_PULLUP);

    // Set motor controller communication pins as outputs
    for(int i = 0; i < NR_MOTORS; i++){
        pinMode(DIRECTION_PINS[i], OUTPUT);
        analogWrite(PWM_PINS[i], 0);
    }
    #ifdef PINS_3
    for(int i = 0; i < NR_MOTORS; i++){
        pinMode(DIRECTION_PINS_INV[i], OUTPUT);
    }
    #endif
    Serial.println("Ready.");

    //runTestSequence();
}

void loop() {
    Command cmd;
    if(getCommand(&cmd)) {
        applyCommand(&cmd);   
    }
    int v = digitalRead(DEBUG_BUTTON_PIN);
    digitalWrite(13, v);
    if(v == LOW) {
        Serial.println("Running test sequence");
        runTestSequence();
        Serial.println("Done");
    }
}

