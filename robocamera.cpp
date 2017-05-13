#include "robocamera.h"
#include <QCameraInfo>

RoboCamera::RoboCamera(QGraphicsScene *scene)
{
    sceneInit(scene);
}

RoboCamera::RoboCamera(unsigned int idx, QGraphicsView *gv, QObject *gv_parent, QGraphicsScene *scene, bool showInfo)
{
    _idx = idx;
    _showInfo = showInfo;
    sceneInit(scene);
/*qDebug() << QCamera::availableDevices().count() << idx;
    if (QCamera::availableDevices().count() < idx + 1) {
        qDebug() << "Unavaible video device\n";
        return;
    }*/
    _camera = new QCamera(QCameraInfo::availableCameras().at(0));
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
    _scene = scene;
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
    if (_videoWidget == nullptr) {
        if (_showInfo) {
            _videoWidget = new VideoWidget(_scene);
        } else {
            _videoWidget = new QVideoWidget();
        }
    }
    _scene->addWidget(_videoWidget);
}

VideoWidget *RoboCamera::getVideoWidget()
{
    return dynamic_cast<VideoWidget *>(_videoWidget);
}

VideoWidget::VideoWidget() : QVideoWidget::QVideoWidget()
{

}

VideoWidget::VideoWidget(QGraphicsScene *scene) : VideoWidget::VideoWidget()
{
    _scene = scene;
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    QVideoWidget::paintEvent(event);
    if (_currentDepth == 0) return;
    qreal height = _scene->height();

    qreal h = height/30.0;
    qreal middle_y = height/2.0;
    qreal p = _currentDepth - int(_currentDepth);

    QString s;
    QPainter painter(this);
    for (qreal i = middle_y - p*h - int((middle_y-p*h)/h)*h; i < height + h/2; i+=h) {
        painter.drawLine(1, i, 8, i);
        s.clear(); s.setNum(int(_currentDepth + i/h - middle_y/h) - 1);
        painter.drawText(20.0, i - 12, s);
    }

    QVector<QPointF> points;
    points << QPointF(1, middle_y - 6) << QPointF(15, middle_y) << QPointF(1, middle_y + 6);
    QPolygonF pointer(points);

    painter.drawPolygon(pointer);
    s.clear(); s.setNum(_currentDepth);
    painter.drawText(QPointF(50.0, middle_y), s);
}

void VideoWidget::setCurrentDepth(double value)
{
    _currentDepth = value;
}
