#include "basetool.h"
#include "graphicsscene.h"
#include <QCoreApplication>
#include <math.h>
#include "figure.h"

#define SCENE_WIDTH 640
#define SCENE_HEIGHT 480
double scaleCoef;

static bool inRect(QPointF p)
{
    return (p.x() >= 0 && p.x() <= SCENE_WIDTH && p.y() >= 0 && p.y() <= SCENE_HEIGHT);
}

static void intersection(QPointF &p1, QPointF &p2)
{
    QPointF points[4] = {QPointF(0, 0), QPointF(0, SCENE_HEIGHT), QPointF(SCENE_WIDTH, 0), QPointF(SCENE_WIDTH, SCENE_HEIGHT)};
    QPointF t(0, 0);
    for (int i = 0; i < 4; i++) {
         if (((p1.x() < points[i].x()) && (p2.x() > points[i].x())) ||
             ((p2.x() < points[i].x()) && (p1.x() > points[i].x()))){
             t.setX(points[i].x());
             t.setY((points[i].x() - p1.x())*(p2.y() - p1.y())/(p2.x() - p1.x())+p1.y());
             if (inRect(p1) && !inRect(p2)) { p2.setX(t.x()); p2.setY((t.y())); }
             if (inRect(p2) && !inRect(p1)) { p1.setX(t.x()); p1.setY((t.y())); }
         }

         if (((p1.y() < points[i].y()) && (p2.y() > points[i].y())) ||
             ((p2.y() < points[i].y()) && (p1.y() > points[i].y()))){
             t.setY(points[i].y());
             t.setX((points[i].y() - p1.y())*(p2.x() - p1.x())/(p2.y() - p1.y())+p1.x());
             if (inRect(p1) && !inRect(p2)) { p2.setX(t.x()); p2.setY((t.y())); }
             if (inRect(p2) && !inRect(p1)) { p1.setX(t.x()); p1.setY((t.y())); }
         }
    }
    qDebug() << p2.x() << " " << p2.y();
}

BaseTool::BaseTool(QWidget *parent)
{
    _iconsPath = (QCoreApplication::applicationDirPath() + "../remote-control/icons");
    _button = new QPushButton(parent);
    _parent = parent;
}

BaseTool::~BaseTool()
{
    delete _button;
}

void BaseTool::onToolButtonClick(bool checked)
{
    qDebug() << typeid(*this).name();
    currentTool->deleteToolProperties();
    currentTool = this;
    createToolProperties();
}

void BaseTool::setLevel(int level)
{
    _button->setGeometry(_button->x(), _button->y() + level*34, _button->width(), _button->height());
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
    _startPos = point;
}

void LineTool::drawOnMousePress(GraphicsScene *scene, QPointF point)
{
    _startPos = point;
}

void LineTool::drawOnMouseMove(GraphicsScene *scene, QPointF point)
{
    _endPos = point;
    intersection(_startPos, _endPos);
    scene->addLine(_startPos.x(), _startPos.y(), _endPos.x(), _endPos.y(), QPen(QColor(0, 0, 255, 127)));
}

void LineTool::drawOnMouseRelease(GraphicsScene *scene, QPointF point)
{
    _endPos = point;
    intersection(_startPos, _endPos);
    if (_endPos != _startPos){
        scene->addFigure(new LineFigure(_startPos, _endPos));
    }
    double t = sqrt(pow(_startPos.x() - _endPos.x(), 2) + pow(_startPos.y() - _endPos.y(), 2));
    qDebug() << t/scaleCoef;
}

OptionTool::OptionTool(QWidget *parent) : LineTool(parent)
{
    _spinBox = nullptr;
    _button->setText("Scale option");
    _button->setIconSize(QSize(30, 30));
    connect(_button, SIGNAL(clicked(bool)), this, SLOT(onToolButtonClick(bool)));
}

OptionTool::~OptionTool()
{
    if (_spinBox != nullptr) {
        delete _spinBox;
    }
}

void OptionTool::createToolProperties()
{
    _spinBox = new QDoubleSpinBox(_parent);
    _spinBox->setGeometry(_spinBox->x(), _spinBox->y() + tools.size()*34, _spinBox->width(), _spinBox->height());
    _spinBox->setMinimum(0.1);
    _spinBox->show();
}

void OptionTool::deleteToolProperties()
{
    delete _spinBox;
}

#include <string>

void OptionTool::drawOnMouseRelease(GraphicsScene *scene, QPointF point)
{
    _endPos = point;
    intersection(_startPos, _endPos);
    if (_endPos != _startPos){
        //scene->addFigure(new LineFigure(_startPos, _endPos));

        double dist = sqrt(pow(_startPos.x() - _endPos.x(), 2) + pow(_startPos.y() - _endPos.y(), 2));
        scaleCoef = dist / _spinBox->value();

        std::string str(std::to_string(_spinBox->value()) + "cm in " + std::to_string(dist) + " scene coordinates");
        //scene->addTextFigure(QString(str.c_str()));
    }
}

RectTool::RectTool(QWidget *parent) :
    BaseTool(parent)
{
    _button->setText("Rect");
    _button->setIconSize(QSize(30, 30));
    connect(_button, SIGNAL(clicked(bool)), this, SLOT(onToolButtonClick(bool)));
}

RectTool::~RectTool()
{

}

void RectTool::drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point)
{
    _p1 = point;
}

void RectTool::drawOnMouseMove(GraphicsScene *scene, QPointF point)
{
    QRectF t(_p1, point);
    scene->addRect(QRectF(_p1, point), QPen(QColor(0, 0, 255, 127)));
    qDebug() << scaleCoef;
    qDebug() << "width : " << abs(t.width()/scaleCoef) << " height : " << abs(t.height()/scaleCoef);
}

void RectTool::drawOnMousePress(GraphicsScene *scene, QPointF point)
{
    _p1 = point;
}

void RectTool::drawOnMouseRelease(GraphicsScene *scene, QPointF point)
{
    _p2 = point;
    if (_p1 == _p2) return;
    scene->addFigure(new RectFigure(_p1, _p2));
}
