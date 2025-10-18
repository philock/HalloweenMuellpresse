#include <wall.h>

Wall::Wall(){
    if(_interface.isRed()) _state = WallState::OPEN;
    if(_interface.isGreen()) _state = WallState::CLOSED;
}

void Wall::poll(){
    switch(_state){
    case WallState::OPEN:
        break;

    case WallState::WAIT_CLOSING:
        if(_interface.isSuccess()){
            _t_start = millis();
            _state = WallState::CLOSING;
        }
        else if (_interface.isFail()){
            _state = WallState::OPEN;
        }
        break;

    case WallState::CLOSING:
        if(_interface.isGreen() && millis() > _t_start + _drive_duration){
            Serial.println("INFO: Wall closed");
            _state = WallState::CLOSED;
        }
        else if(!_interface.isRed()){ // Wall was blocked, red led starts blinking
            Serial.println("ERROR: Wall is blocked");
            errorLED.errCode(wallBlocked, 3);
            _state = WallState::ERROR;
        }

        break;

    case WallState::CLOSED:
        break;

    case WallState::WAIT_OPENING:
        if(_interface.isSuccess()){
            _t_start = millis();
            _state = WallState::OPENING;
        }
        else if (_interface.isFail()){
            _state = WallState::CLOSED;
        }
        break;

    case WallState::OPENING:
        if(_interface.isRed() && millis() > _t_start + _drive_duration){
            Serial.println("INFO: Wall opened");
            _state = WallState::OPEN;
        }
        break;
    
    case WallState::ERROR:
        break;

    default:
        break;
    }

    _interface.poll();
}

bool Wall::isRunning(){
    return (_state == WallState::WAIT_OPENING || _state == WallState::WAIT_CLOSING || _state == WallState::OPENING || _state == WallState::CLOSING);
}

bool Wall::isError(){
    return _state == WallState::ERROR;
}

void Wall::open(){
    if(_state == WallState::ERROR){
        Serial.println("INFO: Wall open requested to leave error-state.");
        _interface.triggerRecover();
        errorLED.off();
    }
    else{
        Serial.println("INFO: Wall open trigger.");
        _interface.triggerOpen();
    }

    _state = WallState::WAIT_OPENING;
}

void Wall::close(){
    if(_state == WallState::ERROR){
        Serial.println("INFO: Wall cannot start closing while in error-state.");    
        return;
    } 

    _interface.triggerClose();
    Serial.println("INFO: Wall close trigger.");
    _state = WallState::WAIT_CLOSING;
}

void Wall::reset(){
    if(_interface.isGreen()) {
        Serial.println("INFO: Wall: Reset to state closed");
        _state = WallState::CLOSED;
    }
    else{
        Serial.println("INFO: Wall: Reset to state open");
        _state = WallState::OPEN;
    }

    errorLED.off();
    _interface.reset();
}