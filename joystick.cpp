#include "joystick.h"
#include <QtCore>

Joystick::Joystick():
    _joy(0)
{
    joyInit();
}

Joystick::~Joystick()
{
    close();
    delete _joy;
}

void Joystick::joyInit()
{
    if (SDL_Init(SDL_INIT_JOYSTICK) == 0) {
        qDebug() << "SDL_INIT_JOYSTICK initialization was successful";
    } else {
        qDebug() << "SDL_INIT_JOYSTICK initialization failed";
        qDebug() << "Error: " << SDL_GetError();
    }

    qDebug() << "NumJoysticks = " << SDL_NumJoysticks();
    if (SDL_NumJoysticks() > 0) {
        SDL_JoystickEventState(SDL_ENABLE);
        _joy = SDL_JoystickOpen(0);
    }

    if(_joy) {
        qDebug() << "Opened Joystick 0";
        qDebug() << "Name: " <<  SDL_JoystickName(0);
        qDebug() << "Number of Axes: " << SDL_JoystickNumAxes(_joy);
        qDebug() << "Number of Buttons: " << SDL_JoystickNumButtons(_joy);
        qDebug() << "Number of Balls: " << SDL_JoystickNumBalls(_joy);
        qDebug() << "Number of Hat: " << SDL_JoystickNumHats(_joy);
    } else {
        qDebug() << "Couldn't open Joystick 0\n";
    }
}

void Joystick::close()
{
    SDL_JoystickClose(_joy);
}

void Joystick::update()
{
    SDL_JoystickUpdate();
}

float Joystick::axesAt(int idx)
{
    return SDL_JoystickGetAxis(_joy, idx)/double(INT16_MAX);
}

void Joystick::handleEvent()
{
    SDL_JoystickUpdate();
    _hatState = SDL_JoystickGetHat(_joy, 0);
    for (int i = 0; i < BUTTON_COUNT; i++) {
        bool prevState = btnState[i];
        btnState[i] = SDL_JoystickGetButton(_joy, i);
        _btnStateChanged[i] = !(prevState == btnState[i]);
        _btnDoubleState[i] = prevState && btnState[i];
    }
    emit joyButtonEvent();
}

bool Joystick::atBtn(int idx)
{
    return btnState[idx];
}

bool Joystick::btnStateChanged(int idx)
{
    return _btnStateChanged[idx];
}

bool Joystick::atHat(int idx)
{
    return _hatState == idx;
}

bool Joystick::btnDoubleClicked(int idx)
{
    return _btnDoubleState[idx];
}
