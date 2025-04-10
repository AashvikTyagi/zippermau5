// just checking something here, dw

#define EN_1B 7
#define EN_1A 6

volatile int pulse_count = 0;
uint32_t last_time = 0;

void setup() {
    Serial.begin(115200);
    pinMode(EN_1A, INPUT_PULLUP);
    pinMode(EN_1B, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(EN_1A), ISR_EN1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(EN_1B), ISR_EN1, CHANGE);
}

void loop() {
    uint32_t current_time = millis();
    if (current_time - last_time >= 500) {
        Serial.print((pulse_count / 2.0) * 60.0);
        Serial.println(" rpm");
        pulse_count = 0;
        last_time = current_time;
    }
}

void ISR_EN1() {
    pulse_count++;
}