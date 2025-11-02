#include <Arduino.h>
#include <config.h>
#include <input.h>
#include <eventSequence.h>

//Input trigger(PIN_TRIG, false, true);
Input switch_1(PIN_SWITCH_1, false, true);
Input switch_2(PIN_SWITCH_2, false, true);
Input button(PIN_BUTTON, false, true);

LED errorLED(PIN_LED_RED);
LED activeLED(PIN_LED_GREEN);

enum SystemState{
    STOP,
    AUTO,
    MANUAL
};
SystemState sysState;

void readSwitchState(){
    unsigned int switch_state = ((int)switch_1.read() << 1) | (int)switch_2.read();

    if(switch_state == 0b10 && sysState != SystemState::STOP){ // left position
        Serial.println("INFO: System reset and stopped");
        resetSequence();
        sysState = SystemState::STOP;

    }
    else if(switch_state == 0b00 && sysState != SystemState::MANUAL){ // middle position
        Serial.println("INFO: System set to manual operation");
        sysState = SystemState::MANUAL;

    }
    else if(switch_state == 0b01 && sysState != SystemState::AUTO){ // right position
        Serial.println("INFO: System set to automatic operation");
        sysState = SystemState::AUTO;
    }
}

void setup() {
    // Relay outputs
    pinMode(PIN_230V, OUTPUT);
    pinMode(PIN_FOG, OUTPUT);
    digitalWrite(PIN_230V, HIGH);
    digitalWrite(PIN_FOG, HIGH);

    configureMP3();

    Serial.begin(115200);

    activeLED.on();
    errorLED.off();

    //trigger.setLongpressTime(1000);
    //trigger.setDebounceTime(200);
    pinMode(PIN_TRIG, INPUT);
}


int numTrigs = 0;
void loop() {
    readSwitchState();
    errorLED.poll();
    activeLED.poll();

    int val = analogRead(PIN_TRIG);
    Serial.println(val);
    if(val < 900) numTrigs ++;
    else numTrigs = 0;
   
    switch(sysState){
    case SystemState::STOP:
        if(isRunning) isRunning = false;
        break;

    case SystemState::AUTO:
        if(numTrigs > 100 && !isRunning) {
            startSequence();
            numTrigs = 0;
        }
        break;

    case SystemState::MANUAL:
        if(button.read() && !isRunning) startSequence();
        break;
    }

    stepSequence();

    //audioTest();
}
