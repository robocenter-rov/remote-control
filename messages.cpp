#include "messages.h"
#include <QtCore>

QDataStream &operator <<(QDataStream &ds, const MotorsThrustMsg &msg)
{
    ds << msg._id
       << msg._motor1Thrust
       << msg._motor2Thrust
       << msg._motor3Thrust
       << msg._motor4Thrust
       << msg._motor5Thrust
       << msg._motor6Thrust;
    qDebug() << "id: " << msg._id
             << "\nmotor1Thrust" << msg._motor1Thrust
             << "\nmotor2Thrust" << msg._motor2Thrust
             << "\nmotor3Thrust" << msg._motor3Thrust
             << "\nmotor4Thrust" << msg._motor4Thrust
             << "\nmotor5Thrust" << msg._motor5Thrust
             << "\nmotor6Thrust" << msg._motor6Thrust;
    return ds;
}

MotorsThrustMsg::MotorsThrustMsg(const MotorsThrustMsg& other)
{
    this->_id = other._id;
    this->_motor1Thrust = other._motor1Thrust;
    this->_motor2Thrust = other._motor2Thrust;
    this->_motor3Thrust = other._motor3Thrust;
    this->_motor4Thrust = other._motor4Thrust;
    this->_motor5Thrust = other._motor5Thrust;
    this->_motor6Thrust = other._motor6Thrust;
}

MotorsThrustMsg::MotorsThrustMsg(
    int16_t motor1Thrust, int16_t motor2Thrust, int16_t motor3Thrust,
    int16_t motor4Thrust, int16_t motor5Thrust, int16_t motor6Thrust) :
    BaseMsg(1)
{
    _motor1Thrust = motor1Thrust;
    _motor2Thrust = motor2Thrust;
    _motor3Thrust = motor3Thrust;
    _motor4Thrust = motor4Thrust;
    _motor5Thrust = motor5Thrust;
    _motor6Thrust = motor6Thrust;
}
