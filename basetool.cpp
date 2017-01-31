#include "basetool.h"
#include "graphicsscene.h"
#include <QCoreApplication>

BaseTool::BaseTool(QWidget *parent)
{
    _iconsPath = (QCoreApplication::applicationDirPath() + "../remote-control/icons");
    _button = new QPushButton(parent);
}

BaseTool::~BaseTool()
{
    delete _button;
}

void BaseTool::onToolButtonClick(bool checked)
{
    qDebug() << typeid(*this).name();
    currentTool = this;
}

LineTool::LineTool(QWidget *parent):
    BaseTool(parent)
{
    _button->setText("Line");
    _button->setIconSize(QSize(30, 30));
    connect(_button, SIGNAL(clicked(bool)), this, SLOT(onToolButtonClick(bool)));
}

LineTool::~LineTool()
{

}

void LineTool::drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point)
{
    _isDraw = true;
    _startPos = point;
}

void LineTool::drawOnMousePress(GraphicsScene *scene, QPointF point)
{
    _startPos = point;
}

void LineTool::drawOnMouseMove(GraphicsScene *scene, QPointF point)
{
    if (_isDraw) {
        scene->addLine(_startPos.x(), _startPos.y(), point.x(), point.y(), QPen(QColor(255, 0, 0, 127)));
    }
}

void LineTool::drawOnMouseRelease(GraphicsScene *scene, QPointF point)
{
    _isDraw = false;
}
