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


void configureMP3(){
    mp3Serial.begin(MD_YX5300::SERIAL_BPS);
    Serial.begin(115200);
    mp3.begin();
    mp3.setSynchronous(true);
    mp3.playFolderRepeat(1);
    mp3.volume(mp3.volumeMax());

}

void audioPlay(){
    mp3.playStart();
}

void audioStop(){
    mp3.playStop();
}

void setup() {
    // Relay outputs
    pinMode(PIN_230V, OUTPUT);
    pinMode(PIN_FOG, OUTPUT);
    digitalWrite(PIN_230V, HIGH);
    digitalWrite(PIN_FOG, HIGH);

    // Indicator LEDs
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    configureMP3();
    Serial.begin(115200);
}

void loop() {
    mp3.check();
    wall.poll();

    if(switch_1.read()){ // Left position
        Serial.println("INFO: System reset");
        wall.reset();
        delay(1500);
    }

    if(!wall.isRunning() || wall.isError()){
        if(button.read() && !switch_1.read() && !switch_2.read()) wall.close(); // Middle position
        if(button.read() && switch_2.read()) wall.open(); // Right position
    }
    
}
