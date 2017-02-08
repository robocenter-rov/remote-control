#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QUdpSocket>
#include "messages.h"
#include "joystick.h"

#define IP_ADDR "127.0.0.1" //"192.168.1.177"
#define PORT 8000 //8888

class Communicator : public QObject
{
    Q_OBJECT
public:
    Communicator();
    ~Communicator();
    template <typename T>
    void Communicator::sendMessage(T msg)
    {
        QByteArray buff;
        QDataStream ds(&buff, QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds << msg;
        _socket->writeDatagram(buff, QHostAddress(IP_ADDR), PORT);
    }
public slots:
    void readMessage();
private slots:
    void readAndSendJoySensors();
private:
    void udpSocketInit();
    QUdpSocket *_socket;
    Joystick *_joy;
    QTimer *_joyTimer;
};

#endif // COMMUNICATOR_H
