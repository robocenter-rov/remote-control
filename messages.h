#ifndef MESSAGES_H
#define MESSAGES_H

#include <cstdint>

enum CoordinateSystem { Global, Local };
enum Axis { AxisX, AxisY, AxisZ };

// TO DO: Add constructions, and send methods
namespace Cmd {
    /* Low-level commands */
    /**********************/
    struct SetDigitalValue
    {
        int id;
        int pin;
        bool value;
    };

    struct SetAnalogValue
    {
        int id;
        int pin;
        int value;
    };

    /* Mid-level commands */
    /**********************/
    struct SetMotorsSpeed
    {
        float motor1speed;
        float motor2speed;
        float motor3speed;
        float motor4speed;
        float motor5speed;
        float motor6speed;
    };

    /* High-level commands */
    /***********************/
    struct SetDepth
    {
        CoordinateSystem coordSystem;
        float value;
    };

    struct SetAngleSpeed
    {
        CoordinateSystem coord_system;
        Axis axis;
        float value;
    };

    struct SetPosition
    {
        CoordinateSystem coordSystem;
        float x, y, z;
    };

    struct Rotate
    {
        CoordinateSystem coordSystem;
        Axis axis;
        float angle;    // NOT SOLVED! value in degrees or radians?
    };

} // Cmd namespace

namespace Msg {
    // TO DO
} // Msg namespace

#endif // MESSAGES_H
