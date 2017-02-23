#include "joystick.h"
#include <QtCore>

Joystick::Joystick():
    _joy(0)
{
    joyInit();
}

Joystick::~Joystick()
{
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
        _joy = SDL_JoystickOpen(0);
    }

    if(_joy) {
        qDebug() << "Opened Joystick 0";
        qDebug() << "Name: " <<  SDL_JoystickName(0);
        qDebug() << "Number of Axes: " << SDL_JoystickNumAxes(_joy);
        qDebug() << "Number of Buttons: " << SDL_JoystickNumButtons(_joy);
        qDebug() << "Number of Balls: " << SDL_JoystickNumBalls(_joy);
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

MotorsThrustMsg Joystick::getMotorsThrust()
{
    SDL_JoystickUpdate();

    return MotorsThrustMsg(
        SDL_JoystickGetAxis(_joy, 0),
        SDL_JoystickGetAxis(_joy, 1),
        SDL_JoystickGetAxis(_joy, 2),
        SDL_JoystickGetAxis(_joy, 3),
        SDL_JoystickGetAxis(_joy, 4));
}
