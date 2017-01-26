#include "robocamera.h"

RoboCamera::RoboCamera()
{
    sceneInit();
}

RoboCamera::RoboCamera(QGraphicsView *gv, QObject *gv_parent, const QByteArray &deviceName = "")
{
    /* DO : use deviceName */

    sceneInit();

    _camera = new QCamera();
    _camera->setCaptureMode(QCamera::CaptureVideo);
    _camera->setViewfinder(_videoWidget);
    _camera->start();

    gv->setScene(_scene);
    gv->viewport()->installEventFilter(gv_parent);
    gv->show();
}

RoboCamera::~RoboCamera()
{
    delete _scene;
    delete _videoWidget;
    delete _camera;
    _scene = nullptr;
    _videoWidget = nullptr;
    _camera = nullptr;
}

QGraphicsScene *RoboCamera::getScene() const
{
    return this->_scene;
}

void RoboCamera::sceneInit()
{
    _scene = new QGraphicsScene();
    _videoWidget = new QVideoWidget();
    _scene->addWidget(_videoWidget);
}
