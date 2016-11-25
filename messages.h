#ifndef MESSAGES_H
#define MESSAGES_H

enum coordinate_system_t { GLOBAL, LOCAL };
enum axis_t { AXIS_X, AXIS_Y, AXIS_Z };

// TO DO: Add constructions, and send methods
namespace Cmd {
    /* Low-level commands */
    /**********************/
    struct SetDigitalValue {
        int id;
        int pin;
        bool value;
    };

    struct SetAnalogValue {
        int id;
        int pin;
        int value;
    };

    /* Mid-level commands */
    /**********************/
    struct SetMotorsSpeed {
        float motor1speed;
        float motor2speed;
        float motor3speed;
        float motor4speed;
        float motor5speed;
        float motor6speed;
    };

    /* High-level commands */
    /***********************/
    struct SetDepth {
        coordinate_system_t coord_system;
        float value;
    };

    struct SetAngleSpeed {
        coordinate_system_t coord_system;
        axis_t axis;
        float value;
    };

    struct SetPosition {
        coordinate_system_t coord_system;
        float x, y, z;
    };

    struct Rotate {
        coordinate_system_t coord_system;
        axis_t axis;
        float angle;    // NOT SOLVED! value in degrees or radians?
    };
}

namespace Msg {
    // TO DO
}

#endif // MESSAGES_H
