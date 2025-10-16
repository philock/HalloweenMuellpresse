#include <Arduino.h>
#include <config.h>
#include <MD_YX5300.h>
#include <SoftwareSerial.h>

SoftwareSerial mp3Serial(PIN_UART_RX, PIN_UART_TX);
MD_YX5300 mp3(mp3Serial);

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
    pinMode(PIN_WALL_OPEN, OUTPUT);
    pinMode(PIN_WALL_CLOSE, OUTPUT);
    pinMode(PIN_230V, OUTPUT);
    pinMode(PIN_FOG, OUTPUT);
    digitalWrite(PIN_WALL_OPEN, HIGH);
    digitalWrite(PIN_WALL_CLOSE, HIGH);
    digitalWrite(PIN_230V, HIGH);
    digitalWrite(PIN_FOG, HIGH);

    // Indicator LEDs
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);

    // Inputs
    pinMode(PIN_MOTOR_STATE_GREEN, INPUT);
    pinMode(PIN_MOTOR_STATE_RED, INPUT);
    pinMode(PIN_TRIG, INPUT_PULLUP);
    pinMode(PIN_SWITCH_1, INPUT_PULLUP);
    pinMode(PIN_SWITCH_2, INPUT_PULLUP);
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    configureMP3();
    Serial.begin(115200);
}

void loop() {
    mp3.check();
    audioPlay();
    delay(1000);

    digitalWrite(PIN_WALL_OPEN, LOW);
    delay(100);
    digitalWrite(PIN_WALL_CLOSE, LOW);
    delay(100);
    digitalWrite(PIN_230V, LOW);
    delay(100);
    digitalWrite(PIN_FOG, LOW);
    delay(100);
    digitalWrite(PIN_WALL_OPEN, HIGH);
    delay(100);
    digitalWrite(PIN_WALL_CLOSE, HIGH);
    delay(100);
    digitalWrite(PIN_230V, HIGH);
    delay(100);
    digitalWrite(PIN_FOG, HIGH);

    if(!digitalRead(PIN_SWITCH_1)) digitalWrite(PIN_LED_GREEN, HIGH);
    else digitalWrite(PIN_LED_GREEN, LOW);

    if(!digitalRead(PIN_SWITCH_2)) digitalWrite(PIN_LED_RED, HIGH);
    else digitalWrite(PIN_LED_RED, LOW);

    if(!digitalRead(PIN_BUTTON)) Serial.println("Button pushed");

    if(!digitalRead(PIN_TRIG)) Serial.println("Trigger");

    if(digitalRead(PIN_MOTOR_STATE_GREEN)) Serial.println("Motor green");
    if(digitalRead(PIN_MOTOR_STATE_RED)) Serial.println("Motor red");

}
