#include "messages.h"
#include <QtCore>

QDataStream &operator <<(QDataStream &ds, const MotorsThrustMsg &msg)
{
    ds << msg._id
       << msg._pos.x
       << msg._pos.y
       << msg._pos.z
       << msg._pos.ty
       << msg._pos.tz;
    qDebug() << "id  : " << msg._id
             << "\nx  : " << msg._pos.x
             << "\ny  : " << msg._pos.y
             << "\nz  : " << msg._pos.z
             << "\nty : " << msg._pos.ty
             << "\ntz : " << msg._pos.tz
             << "\n";
    return ds;
}

MotorsThrustMsg::MotorsThrustMsg(const MotorsThrustMsg& other)
{
    _id = other._id;
    _pos.x = other._pos.x;
    _pos.y = other._pos.y;
    _pos.z = other._pos.z;
    _pos.ty = other._pos.ty;
    _pos.tz = other._pos.tz;
}

MotorsThrustMsg::MotorsThrustMsg(int16_t axes0, int16_t axes1, int16_t axes2, int16_t axes3, int16_t axes4) :
    BaseMsg(12)
{
    int eps = 4000;
    axes1 = (abs(axes1) < eps) ? 0 : axes1;
    axes0 = (abs(axes0) < eps) ? 0 : axes0;
    axes4 = (abs(axes4) < eps) ? 0 : axes4;

    double dist = sqrt(pow(axes1, 2) + pow(axes0, 2) + pow(axes4, 2));

    if (dist > INT16_MAX) {
        _pos.x = axes1/dist * INT16_MAX;
        _pos.y = axes0/dist * INT16_MAX;
        _pos.z = axes4/dist * INT16_MAX;
    } else {
        _pos.x = axes1;
        _pos.y = axes0;
        _pos.z = axes4;
    }

    _pos.ty = 0; // pitch
    _pos.tz = axes3; // heading
}
