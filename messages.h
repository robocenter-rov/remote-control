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

    struct AxesValue {
        AxesValue(int16_t a1, int16_t a2, int16_t a3, int16_t a4 = 0, int16_t a5 = 0);
        int16_t axis0;
        int16_t axis1;
        int16_t axis2;
        int16_t axis3;
        int16_t axis4;
    };
} // Cmd namespace

namespace Msg {
    // TO DO
} // Msg namespace

#endif // MESSAGES_H
