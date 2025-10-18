#include <motorInterface.h>

MotorInterface::MotorInterface():
_stateGreen(PIN_MOTOR_STATE_GREEN, true, false),
_stateRed(PIN_MOTOR_STATE_RED, true, false)
{
    pinMode(PIN_WALL_OPEN, OUTPUT);
    pinMode(PIN_WALL_CLOSE, OUTPUT);

    reset();

    _stateRed.setDebounceTime(10);
    _stateGreen.setDebounceTime(10);
}

void MotorInterface::poll(){
    switch (_state){
    case MotorInterfaceState::CLOSE_TRIGGER:
        state_close_trigger();
        break;
    
    case MotorInterfaceState::CLOSE_WAIT_FOR_ACK_START:
        state_close_wait_for_ack_start();
        break;

    case MotorInterfaceState::CLOSE_WAIT_FOR_ACK_RELEASE:
        state_close_wait_for_ack_release();
        break;

    case MotorInterfaceState::OPEN_TRIGGER:
        state_open_trigger();
        break;
    
    case MotorInterfaceState::OPEN_WAIT_FOR_ACK_START:
        state_open_wait_for_ack_start();
        break;

    case MotorInterfaceState::OPEN_WAIT_FOR_ACK_RELEASE:
        state_open_wait_for_ack_release();
        break;

    case MotorInterfaceState::RECOVER_TRIGGER:
        state_recover();
        break;

    case MotorInterfaceState::SUCCESS:
        break;

    case MotorInterfaceState::FAIL:
        break;

    default:
        break;
    }
}

void MotorInterface::state_close_trigger(){
    if(millis() > _t_trig_start + _trig_duration){
        digitalWrite(PIN_WALL_CLOSE, HIGH);
        _t_ack = millis();
        _state = MotorInterfaceState::CLOSE_WAIT_FOR_ACK_START;
    }
}

void MotorInterface::state_close_wait_for_ack_start(){
    // Wait for acknowledgement start by LED turning off
    if(!_stateRed.read() && !_stateGreen.read()){
        _t_ack = millis();
        _state = MotorInterfaceState::CLOSE_WAIT_FOR_ACK_RELEASE;

    }
    else if(millis() > _t_ack + _timeout){ // Timeout, too much time between trigger and LED turning off
        close_handle_nack();
        Serial.println("ERROR: Motor interface, state close_wait_start: Timeout while waiting red LED to turn off.");
    }
}

void MotorInterface::state_close_wait_for_ack_release(){
    // Wait for LED to turn back on, indicating sucessful acknowledgement
    if(_stateRed.read()){
        unsigned long blink_time  = millis() - _t_ack;
        Serial.print("INFO: Motor interface, state close_wait_release: blink_time = ");
        Serial.println(blink_time);

        // Check if LED blink had the correct duration
        if(blink_time > 100 && blink_time < 500){
            _state = MotorInterfaceState::SUCCESS;
        }
        else{
            // Error, invalid ack, too long or too short
            close_handle_nack();
            Serial.println("ERROR: Motor interface, state close_wait_release: Invalid acknowledgement time.");
        }
    }
    else if(_stateGreen.read()){
        unsigned long blink_time  = millis() - _t_ack;
        Serial.print("INFO: Motor interface, state close_wait_release: Wall already closed, nothing to do. blink_time = ");
        Serial.println(blink_time);
        _state = MotorInterfaceState::SUCCESS;
    }
    else if(millis() > _t_ack + _timeout){
        // Error, LED did not turn back on in time
        close_handle_nack();
        Serial.println("ERROR: Motor interface, state close_wait_release: Timeout while waiting red LED to turn back on.");
    }
}

void MotorInterface::state_open_trigger(){
    if(millis() > _t_trig_start + _trig_duration){
        digitalWrite(PIN_WALL_OPEN, HIGH);
        _t_ack = millis();
        _state = MotorInterfaceState::OPEN_WAIT_FOR_ACK_START;
    }
}

void MotorInterface::state_open_wait_for_ack_start(){
    // Wait for acknowledgement start by LED turning off
    if(!_stateGreen.read() && !_stateRed.read()){
        _t_ack = millis();
        _state = MotorInterfaceState::OPEN_WAIT_FOR_ACK_RELEASE;

    }
    else if(millis() > _t_ack + _timeout){ // Timeout, too much time between trigger and LED turning off
        open_handle_nack();
        Serial.println("ERROR: Motor interface, state open_wait_start: Timeout while waiting green LED to turn off.");
    }
}

void MotorInterface::state_open_wait_for_ack_release(){
    // Wait for red LED to turn on, indicating sucessful acknowledgement
    if(_stateRed.read()){
        unsigned long blink_time  = millis() - _t_ack;
        Serial.print("INFO: Motor interface, state open_wait_release: blink_time = ");
        Serial.println(blink_time);

        // Check if LED blink had the correct duration
        if(blink_time > 100 && blink_time < 500){
            _state = MotorInterfaceState::SUCCESS;
        }
        else{
            // Error, invalid ack, too long or too short
            open_handle_nack();
            Serial.println("ERROR: Motor interface, state open_wait_release: Invalid acknowledgement time.");
        }
    }
    else if(millis() > _t_ack + _timeout){
        // Error, LED did not turn back on in time
        open_handle_nack();
        Serial.println("ERROR: Motor interface, state open_wait_release: Timeout while waiting red LED to turn on.");
    }
}

void MotorInterface::state_recover(){
    if(millis() > _t_trig_start + _trig_duration){
        digitalWrite(PIN_WALL_OPEN, HIGH);
        _state = MotorInterfaceState::SUCCESS;
    }
}

void MotorInterface::close_handle_nack(){
    _nack_counter ++;

    if(_nack_counter > _N_retry){ // Too many failed transmissions
        _state = MotorInterfaceState::FAIL; 

    }
    else{ // Try to transmit again
        digitalWrite(PIN_WALL_CLOSE, LOW);
        _t_trig_start = millis();
        _state = MotorInterfaceState::CLOSE_TRIGGER;
    }
}

void MotorInterface::open_handle_nack(){
    _nack_counter ++;

    if(_nack_counter > _N_retry){ // Too many failed transmissions
        _state = MotorInterfaceState::FAIL; 

    }
    else{ // Try to transmit again
        digitalWrite(PIN_WALL_OPEN, LOW);
        _t_trig_start = millis();
        _state = MotorInterfaceState::OPEN_TRIGGER;
    }
}

void MotorInterface::triggerOpen(){
    if(_state != SUCCESS) return;

    // Door can be in the closed position (green) or half-way position (red) to initiate opening sequence
    // if(!_stateGreen.read() && !_stateRed.read()) return;

    digitalWrite(PIN_WALL_OPEN, LOW);
    _t_trig_start = millis();
    _state = MotorInterfaceState::OPEN_TRIGGER;
    _nack_counter = 0;
}

void MotorInterface::triggerClose(){
    if(_state != SUCCESS) return;

    // Door can be in the open position (red) or half-way position (also red) to initiate closing sequence
    // if(!_stateGreen.read()) return;

    digitalWrite(PIN_WALL_CLOSE, LOW);
    _t_trig_start = millis();
    _state = MotorInterfaceState::CLOSE_TRIGGER;
    _nack_counter = 0;
}

void MotorInterface::triggerRecover(){
    digitalWrite(PIN_WALL_OPEN, LOW);
    _t_trig_start = millis();
    _state = MotorInterfaceState::RECOVER_TRIGGER;
    _nack_counter = 0;
}

bool MotorInterface::isSuccess(){
    return _state == MotorInterfaceState::SUCCESS;
}

bool MotorInterface::isFail(){
    return _state == MotorInterfaceState::FAIL;
}

bool MotorInterface::isGreen(){
    return _stateGreen.read();
}

bool MotorInterface::isRed(){
    return _stateRed.read();
}

void MotorInterface::reset(){
    // Disable both relay channels
    digitalWrite(PIN_WALL_OPEN, HIGH);
    digitalWrite(PIN_WALL_CLOSE, HIGH);

    // reset nack-counter and go back to idle state (SUCCESS)
    _nack_counter = 0;
    _state = MotorInterfaceState::SUCCESS;
}
