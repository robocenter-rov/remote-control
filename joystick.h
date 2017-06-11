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

#define BUTTON_COUNT 14

class Joystick : public QObject
{
    Q_OBJECT
public:
    Joystick();
    ~Joystick();
    void close();
    void update();
    void handleEvent();
    float axesAt(int idx);
    bool atBtn(int idx);
    bool atHat(int idx);
    bool btnStateChanged(int idx);
    bool btnDoubleClicked(int idx);
signals:
    void joyButtonEvent();
private:
    void joyInit();
    int _hatState;
    bool _btnDoubleState[BUTTON_COUNT];
    bool btnState[BUTTON_COUNT];
    bool _btnStateChanged[BUTTON_COUNT];
    int _btnStateCountTrue;
    SDL_Joystick *_joy;
    int _btnStateCountFalse[BUTTON_COUNT];
};

#endif // JOYSTICK_H
