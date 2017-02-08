#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QtGlobal>
#ifdef Q_OS_WIN32
    #include <SDL.h>
#endif
#ifdef Q_OS_WIN64
    #include <SDL.h>
#endif
#ifdef Q_OS_LINUX
    #include <SDL2/SDL.h>
#endif
#undef main
#include <QTimer>
#include <QObject>
#include "messages.h"

class Joystick : public QObject
{
    Q_OBJECT
public:
    Joystick();
    ~Joystick();
    void close();
    void update();
    MotorsThrustMsg getMotorsThrust();
private:
    void joyInit();
    SDL_Joystick *_joy;
};

#endif // JOYSTICK_H
