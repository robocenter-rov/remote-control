#ifndef MESSAGES_H
#define MESSAGES_H

#include <cstdint>
#include <QDataStream>

struct ThrustVector{
    int16_t x, y, z, ty, tz;
};

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
    MotorsThrustMsg() : BaseMsg(1) {}
    MotorsThrustMsg(const MotorsThrustMsg& other);
    MotorsThrustMsg(int16_t axes0, int16_t axes1, int16_t axes2, int16_t axes3, int16_t axes4);
    friend QDataStream &operator <<(QDataStream &ds, const MotorsThrustMsg &msg);
private:
    ThrustVector _pos;
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
