#include <Arduino.h>
#include <config.h>
#include <input.h>
#include <eventSequence.h>

Input trigger(PIN_TRIG, false, true);
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
    else if(switch_state == 0b00 && sysState != SystemState::AUTO){ // middle position
        Serial.println("INFO: System set to automatic operation");
        sysState = SystemState::AUTO;

    }
    else if(switch_state == 0b01 && sysState != SystemState::MANUAL){ // right position
        Serial.println("INFO: System set to manual operation");
        sysState = SystemState::MANUAL;
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
}

void loop() {
    readSwitchState();
    errorLED.poll();
    activeLED.poll();
   
    switch(sysState){
    case SystemState::STOP:
        if(isRunning) isRunning = false;
        break;

    case SystemState::AUTO:
        if(trigger.read() && !isRunning) startSequence();
        break;

    case SystemState::MANUAL:
        if(button.read() && !isRunning) startSequence();
        break;
    }

    stepSequence();

    //audioTest();
}
