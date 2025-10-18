#include <Arduino.h>
#include <config.h>
#include <MD_YX5300.h>
#include <SoftwareSerial.h>
#include <input.h>
#include <wall.h>

SoftwareSerial mp3Serial(PIN_UART_RX, PIN_UART_TX);
MD_YX5300 mp3(mp3Serial);
Input trigger(PIN_TRIG, false, true);
Input switch_1(PIN_SWITCH_1, false, true);
Input switch_2(PIN_SWITCH_2, false, true);
Input button(PIN_BUTTON, false, true);

LED errorLED(PIN_LED_RED);
LED activeLED(PIN_LED_GREEN);

Wall wall;

bool isRunning = false;
unsigned long sequenceStartTime;

enum SystemState{
    STOP,
    AUTO,
    MANUAL
};
SystemState state;

struct Event {
    unsigned long triggerTime;
    bool triggered;
    void (*action)();
};

void configureMP3(){
    mp3Serial.begin(MD_YX5300::SERIAL_BPS);
    mp3.begin();
    mp3.setSynchronous(true);
    mp3.playFolderRepeat(1);
    mp3.volume(mp3.volumeMax());

}

void readSwitchState(){
    unsigned int switch_state = ((int)switch_1.read() << 1) | (int)switch_2.read();

    if(switch_state == 0b10 && state != SystemState::STOP){ // left position
        Serial.println("INFO: System reset and stopped");
        wall.reset();
        state = SystemState::STOP;

    }
    else if(switch_state == 0b00 && state != SystemState::AUTO){ // middle position
        Serial.println("INFO: System set to automatic operation");
        state = SystemState::AUTO;

    }
    else if(switch_state == 0b01 && state != SystemState::MANUAL){ // right position
        Serial.println("INFO: System set to manual operation");
        state = SystemState::MANUAL;
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
}

// ##################################################################
// Event functions
// ##################################################################

void eventAudioPlay(){
    mp3.playStart();
}

void eventAudioStop(){
    mp3.playStop();
}

void eventWallClose(){
    wall.close();
    digitalWrite(PIN_230V, LOW);
}

void eventFogOn(){
    digitalWrite(PIN_FOG, LOW);
}

void eventFogOff(){
    digitalWrite(PIN_FOG, HIGH);
}

void eventLightOff(){
    digitalWrite(PIN_230V, HIGH);
}

void eventResetSequence(){
    isRunning = false;
    for (int i = 0; i < numEvents; i++) events[i].triggered = true;
}

Event events[] = {
    {0000, false, eventAudioPlay},
    {2000, false, eventWallClose},
    {5000, false, eventFogOn},
    {6000, false, eventFogOff},
    {8000, false, eventAudioStop},
    {10000, false, eventLightOff}
};
const int numEvents = sizeof(events) / sizeof(events[0]);

void eventSequencer(){
    unsigned long time = millis() - sequenceStartTime;

    // Run all events in sequence at appropriate time
    for (int i = 0; i < numEvents; i++) {
        if (!events[i].triggered && time >= events[i].triggerTime) {
            events[i].action();
            events[i].triggered = true;
        }
    }
}

void loop() {
    mp3.check();
    wall.poll();

    readSwitchState();
   
    switch(state){
    case SystemState::STOP:
        if(isRunning) isRunning = false;
        break;

    case SystemState::AUTO:
        if(trigger.read() && !isRunning){
            isRunning = true;
            sequenceStartTime = millis();
        }
        break;

    case SystemState::MANUAL:
        if(button.read() && !isRunning){
            isRunning = true;
            sequenceStartTime = millis();
        }
        break;
    }

    if(isRunning) eventSequencer();
}
