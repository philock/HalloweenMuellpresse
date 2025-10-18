#include <eventSequence.h>

SoftwareSerial mp3Serial(PIN_UART_RX, PIN_UART_TX);
MD_YX5300 mp3(mp3Serial);
Wall wall;

SequenceStates seqState;
bool isRunning = false;
unsigned long sequenceStartTime;

void resetSequence(){
    isRunning = false;
    seqState = SequenceStates::idle;

    // Turn all effects off
    wall.reset();
    digitalWrite(PIN_230V, HIGH);
    digitalWrite(PIN_FOG, HIGH);
    mp3.playStop();
}

void startSequence(){
    Serial.println("INFO: Sequence started");

    isRunning = true;
    sequenceStartTime = millis();
    seqState = SequenceStates::audio_on;
    
    activeLED.blink();
}

void endSequence(){
    Serial.println("INFO: Sequence ended");

    // Initialize FSM state
    isRunning = false;
    seqState = SequenceStates::idle;

    activeLED.on();
}

void stepSequence(){
    mp3.check();
    wall.poll();

    if(isRunning) runFSM(millis() - sequenceStartTime);
}

void runFSM(unsigned long time){
    switch (seqState){
    case SequenceStates::idle:
        isRunning = false;
        break;

    case SequenceStates::audio_on:
        if(time > 500){
            mp3.playStart();
            seqState = SequenceStates::wall_close;
        }
        break;
    
    case SequenceStates::wall_close:
        if(time > 2000){
            wall.close();
            digitalWrite(PIN_230V, LOW);
            seqState = SequenceStates::fog_on_1;
        }
        break;

    case SequenceStates::fog_on_1:
        if(time > 4000){
            digitalWrite(PIN_FOG, LOW);
            seqState = SequenceStates::fog_off_1;
        }
        break;

    case SequenceStates::fog_off_1:
        if(time > 5000){
            digitalWrite(PIN_FOG, HIGH);
            seqState = SequenceStates::light_off;
        }
        break;

    case SequenceStates::light_off:
        if(time > 15000){
            digitalWrite(PIN_230V, HIGH);
            seqState = SequenceStates::wall_open;
        }
        break;
        
    case SequenceStates::wall_open:
        if(time > 25000){
            wall.open();
            seqState = SequenceStates::wait_open;
        }

    case SequenceStates::wait_open:
        if(time > 27000 && !wall.isRunning()){
            endSequence();
        }

    default:
        break;
    }
}

void configureMP3(){
    mp3Serial.begin(MD_YX5300::SERIAL_BPS);
    mp3.begin();
    mp3.setSynchronous(true);
    mp3.playFolderRepeat(1);
    mp3.volume(mp3.volumeMax());

}