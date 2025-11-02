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

    fog_on_2,
    fog_off_2,

    light_off,

    wall_open,

    wait_open
};

// Turn off all effects and drive wall open
void resetSequence();

// Run effect sequence
void startSequence();

// Called at end of sequence
void endSequence();

// Call continuously in main loop
void stepSequence();

// Handles state transitions to the next effect event
void runFSM(unsigned long time);

void configureMP3();

void audioTest();

#endif