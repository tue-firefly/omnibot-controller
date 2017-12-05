#define NR_MOTORS 3
// Motor pins
static const int PWM_PINS[NR_MOTORS] = {3, 5, 6};
static const int DIRECTION_PINS[NR_MOTORS] = {2, 9, 10};

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
        cmd->x = casted[0];
        cmd->y = casted[1];
        cmd->psi = casted[2];
        return true;
    }
    return false;
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
    Serial.println("Ready.");
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

void applyCommand(Command *cmd) {
    float v[NR_MOTORS];

    // TODO these values are probably not 100% correct
    v[0] = (1/2.0) * cmd->x - (1/3.0) * cmd->y - (1/3.0) * cmd->psi;
    v[0] = -v[0]; // Convention
    v[1] = (1/2.0) * cmd->x + (1/3.0) * cmd->y + (1/3.0) * cmd->psi;
    v[2] = 0       * cmd->x - (2/3.0) * cmd->y + (1/3.0) * cmd->psi;
    
    for(int i = 0; i < NR_MOTORS; i++) {
        setMotor(i, v[i]);
    }
}

void setMotor(int index, float velocity) {
    // Set turning direction
    bool positive = velocity > 0;
    digitalWrite(DIRECTION_PINS[index], positive ? HIGH : LOW);

    // Set PWM value
    int pwm = constrain(abs(velocity), 0, 255);
    analogWrite(PWM_PINS[index], pwm);
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

