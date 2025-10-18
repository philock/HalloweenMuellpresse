#ifndef WALL_H
#define WALL_H

#include <Arduino.h>
#include <config.h>
#include <motorInterface.h>

enum WallState{
    OPEN,
    WAIT_OPENING,
    OPENING,
    CLOSED,
    WAIT_CLOSING,
    CLOSING,
    ERROR
};

class Wall{
    public:
        Wall();

        // Called repeatedly on every loop
        void poll();

        // Start the opening or closing sequences
        void close();
        void open();

        // Returns true when wall is waiting for transmission of trigger or while driving
        bool isRunning();

        // Returns true when wall was blocked
        bool isError();

        void reset();

    private:
        MotorInterface _interface;

        WallState _state;

        unsigned long _t_start;
        const unsigned long _drive_duration = 5000;


};

#endif