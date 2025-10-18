#ifndef EVENTSEQUENCE_H
#define EVENTSEQUENCE_H

#include <Arduino.h>
#include <config.h>
#include <MD_YX5300.h>
#include <SoftwareSerial.h>
#include <wall.h>

extern bool isRunning;

enum SequenceStates{
    idle,

    audio_on,

    wall_close,

    fog_on_1,
    fog_off_1,

    light_off,

    wall_open,

    wait_open
};

void resetSequence();
void startSequence();
void endSequence();

void stepSequence();
void runFSM(unsigned long time);

void configureMP3();

#endif