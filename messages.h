#ifndef MESSAGES_H
#define MESSAGES_H

#include <cstdint>
#include <QDataStream>

class BaseMsg
{
public:
    unsigned char _id;
    BaseMsg() {}
    BaseMsg(unsigned char id) { _id = id; }
};

class MotorsThrustMsg : public BaseMsg
{
public:
    MotorsThrustMsg() { _id = 1; }
    MotorsThrustMsg(const MotorsThrustMsg& other);
    MotorsThrustMsg(
        int16_t motor1Thrust, int16_t motor2Thrust, int16_t motor3Thrust,
        int16_t motor4Thrust, int16_t motor5Thrust, int16_t motor6Thrust);
    friend QDataStream &operator <<(QDataStream &ds, const MotorsThrustMsg &msg);
private:
    int16_t _motor1Thrust;
    int16_t _motor2Thrust;
    int16_t _motor3Thrust;
    int16_t _motor4Thrust;
    int16_t _motor5Thrust;
    int16_t _motor6Thrust;
};

struct SetFlashLightState
{
    unsigned char id = 5;
    int16_t worker_id;
    uint16_t tag;
    char state;
};

struct StartBluetoothReading
{
    unsigned char id = 3;
    int16_t worker_id;
    uint16_t tag;
};

struct GetWorkedState
{
    unsigned char id = 11;
    uint16_t worked_id;
};

struct SetManipulatorPosition
{
    unsigned char id = 2;
    uint16_t hand_pos;
    uint16_t arm_pos;
};

struct I2CScan
{
    unsigned char id = 17;
    int16_t worker_id;
    uint16_t tag;
};

#endif // MESSAGES_H
