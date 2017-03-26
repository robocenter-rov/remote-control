#include "robocamera.h"
#include <QCameraInfo>

RoboCamera::RoboCamera(QGraphicsScene *scene)
{
    sceneInit(scene);
}

RoboCamera::RoboCamera(QGraphicsView *gv, QObject *gv_parent, QGraphicsScene *scene, unsigned int idx)
{
    sceneInit(scene);

    if (QCamera::availableDevices().count() < idx + 1) {
        qDebug() << "Unavaible video device\n";
        return;
    }
    _camera = new QCamera(QCameraInfo::availableCameras().at(idx));
    _imageCapture = new QCameraImageCapture(_camera);
    _imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    _camera->setCaptureMode(QCamera::CaptureStillImage);
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
    delete _imageCapture;
    _scene = nullptr;
    _videoWidget = nullptr;
    _camera = nullptr;
    _imageCapture = nullptr;
}

QGraphicsScene *RoboCamera::getScene() const
{
    return this->_scene;
}

void RoboCamera::sceneInit(QGraphicsScene *scene)
{
    if (scene == nullptr) _scene = new QGraphicsScene();
    else _scene = scene;
    addVideoWidget();
}

void RoboCamera::imageCapture()
{
    _camera->searchAndLock();
    char buf[10];
    itoa(_imgId++, (char *)buf, 10);
    QString path(QCoreApplication::applicationDirPath());
    _imageCapture->capture(path);
    _lastSavedImg = _videoWidget->grab().toImage();
    _camera->unlock();
}

QImage RoboCamera::getLastSavedImage()
{
    return _lastSavedImg;
}

void RoboCamera::addVideoWidget()
{
    if (_videoWidget == nullptr) _videoWidget = new QVideoWidget();
    _scene->addWidget(_videoWidget);
}

QVideoWidget *RoboCamera::getVideoWidget() const
{
    return _videoWidget;
}
