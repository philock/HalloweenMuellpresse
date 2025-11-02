#ifndef MOTORINTERFACE_H
#define MOTORINTERFACE_H

#include <Arduino.h>
#include <config.h>
#include <input.h>

enum MotorInterfaceState{
    CLOSE_TRIGGER,
    CLOSE_WAIT_FOR_ACK_START,
    CLOSE_WAIT_FOR_ACK_RELEASE,

    OPEN_TRIGGER,
    OPEN_WAIT_FOR_ACK_START,
    OPEN_WAIT_FOR_ACK_RELEASE,

    RECOVER_TRIGGER,

    SUCCESS, // Also idle state
    FAIL
};

class MotorInterface{
    public:
        MotorInterface();

        // Called repeatedly on every loop
        void poll();

        // Trigger wall opening sequence
        void triggerOpen();

        // Trigger wall closing sequence
        void triggerClose();

        // Trigger recovery sequence after wall was blocked. Not acknowledged because red LED is blinking constantly.
        void triggerRecover();

        // Returns true when trigger was sucessful
        bool isSuccess();

        // Returns true when trigger has failed
        bool isFail();

        // Retuns state of green LED on motor
        bool isGreen();

        // Retuns state of red LED on motor
        bool isRed();

        // Resets interface, stopping all trigger processes
        void reset();

        // Do not use acknowledgements from the motor LEDs, only blind transmissions
        void unancknowledgedMode(bool unack = false);

    private:
        const unsigned int _N_retry = 2; // Number of trigger retries
        const int _trig_duration = 200; // Duration of transmitter button press
        const int _timeout = 1000; // Timerout duration after trigger press
        bool _unancknowledgedMode = false;

        MotorInterfaceState _state = MotorInterfaceState::SUCCESS;

        unsigned long _t_trig_start;
        unsigned long _t_ack;

        unsigned int _nack_counter; // Counts number of not-acknowledged transmission, up to _N_retry

        Input _stateGreen;
        Input _stateRed;

        // Handles failed acknowledgements by either trying again or going to fail-state after too _N_retry attempts
        void close_handle_nack();
        void open_handle_nack();

        // State functions
        void state_close_trigger();
        void state_close_wait_for_ack_start();
        void state_close_wait_for_ack_release();
        void state_open_trigger();
        void state_open_wait_for_ack_start();
        void state_open_wait_for_ack_release();

        void state_recover();
};

#endif