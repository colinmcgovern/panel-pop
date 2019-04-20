//
// Created by axel on 9/8/18.
//

#include "InputConfig.h"

InputConfig::InputConfig(InputEvent *up,
                         InputEvent *down,
                         InputEvent *left,
                         InputEvent *right,
                         InputEvent *swap,
                         InputEvent *raiseStack) :
        _up(up),
        _down(down),
        _left(left),
        _right(right),
        _swap(swap),
        _raiseStack(raiseStack),
        _currentState(NONE, false, false),
        _prevState(NONE, false, false) {}

void InputConfig::updateState() {
    _prevState = _currentState;
    _currentState = InputState::getCurrentState(*this);
}

Direction InputConfig::getDownDirection() {
    if (_prevState._direction != _currentState._direction) {
        return _currentState._direction;
    }
    return NONE;
}

bool InputConfig::raiseStackDown() {
    return !_prevState._raiseStack && _currentState._raiseStack;
}

bool InputConfig::swapDown() {
    return !_prevState._swap && _currentState._swap;
}



