#include "basetool.h"
#include "graphicsscene.h"
#include <QCoreApplication>
#include <math.h>
#include "figure.h"

#define SCENE_WIDTH 640
#define SCENE_HEIGHT 480

double scaleCoef;

QList <Figure *> figures;

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
}

int BaseTool::_nextId = 1;
BaseTool::BaseTool(QWidget *parent) :
    _button(new QPushButton(parent))
{
    _iconsPath = (QCoreApplication::applicationDirPath() + "../remote-control/icons");
    _isDraw = false;
    _parent = parent;
    _button->setIconSize(QSize(30, 30));
    _id = _nextId++;
    _button->setGeometry(_button->x(), _button->y() + _id*34, _button->width(), _button->height());
    connect(_button, SIGNAL(clicked(bool)), this, SLOT(selectButtonTool(bool)));
}

BaseTool::~BaseTool()
{

}

LineTool::LineTool(QWidget *parent):
    BaseTool(parent)
{
    _spinBox = nullptr;
    _button->setText("Line");
    _pen = QPen(QColor(0, 0, 255, 127), 2);
}

LineTool::~LineTool()
{

}

void LineTool::drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point)
{
    _startPos = point;
    _isDraw = true;
}

void LineTool::drawOnMousePress(GraphicsScene *scene, QPointF point)
{
    _startPos = point;
    _isDraw = true;
}

void LineTool::drawOnMouseMove(GraphicsScene *scene, QPointF point)
{
    if (!_isDraw) return;
    _endPos = point;
    intersection(_startPos, _endPos);
    scene->addLine(_startPos.x(), _startPos.y(), _endPos.x(), _endPos.y(), _pen);
    LineFigure line(_startPos, _endPos);
    scene->addText(line.getInfo());
}

void LineTool::drawOnMouseRelease(GraphicsScene *scene, QPointF point)
{
    _endPos = point;
    intersection(_startPos, _endPos);
    if (_endPos != _startPos){
        scene->addFigure(new LineFigure(_startPos, _endPos));
    }
    _isDraw = false;
    if (scaleCoef == 0) {
        if (_spinBox == nullptr) {
            _spinBox = new QDoubleSpinBox(_parent);
            _spinBox->setGeometry(_spinBox->x(), _spinBox->y() + 34*_nextId, _spinBox->width(), _spinBox->height());
            _spinBox->setMinimum(0.1);
            _spinBox->setMaximum(1000);
            _spinBox->show();
            connect(_spinBox, SIGNAL(valueChanged(double)), this, SLOT(calcScaleCoef()));
        }
    }
}

void LineTool::destroyProperties()
{
    if (_spinBox != nullptr) {
        _spinBox->~QDoubleSpinBox();
        _spinBox = nullptr;
        scaleCoef = 0.0;
    }
}

void LineTool::calcScaleCoef()
{
    double dist = sqrt(pow(_startPos.x() - _endPos.x(), 2) + pow(_startPos.y() - _endPos.y(), 2));
    scaleCoef = dist/_spinBox->value();
}

#include <string>

SelectTool::SelectTool(QWidget *parent) :
    BaseTool(parent)
{
    _isDraw = false;
    _button->setText("Select");
}

void SelectTool::drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point)
{
    _startPoint = point;
}

void SelectTool::drawOnMouseMove(GraphicsScene *scene, QPointF point)
{
    for (auto it = _selectedFigures.begin(); it != _selectedFigures.end(); it++) {
        QPointF deltaP(point.x() - _startPoint.x(), point.y() - _startPoint.y());
        if (_isResize) (*it)->resizePoint(_resizePointIdx, point);
        else if (_isDraw) (*it)->resetPoints(deltaP);
    }
    scene->updateScene();
    for (auto it = _selectedFigures.begin(); it != _selectedFigures.end(); it++) {
        (*it)->drawArea(scene);
    }
    _startPoint = point;
}

void SelectTool::drawOnMousePress(GraphicsScene *scene, QPointF point)
{
    _startPoint = point;
    for (auto it = _selectedFigures.rbegin(); it != _selectedFigures.rend(); it++) {
        _resizePointIdx = 0;
        if (_resizePointIdx = (*it)->inResizePointArea(point)) {
            _isResize = true;
            return;
        }
    }
    _selectedFigures.clear();
    scene->updateScene();
    for (auto it = figures.rbegin(); it != figures.rend(); it++) {
        if ((*it)->inArea(point)){
            _isDraw = true;
            (*it)->drawArea(scene);
            _selectedFigures.append(*it);
            return;
        }
    }
    _isDraw = false;
    _isResize = false;
}

void SelectTool::drawOnMouseRelease(GraphicsScene *scene, QPointF point)
{
    _isDraw = false;
    if (_isResize) {
        for (auto it = figures.rbegin(); it != figures.rend(); it++) {
            (*it)->sortPoints();
        }
        _isResize = false;
    }
}

void BaseTool::selectButtonTool(bool state)
{
    currentTool = this;
}

PoolLineTool::PoolLineTool(QWidget *parent):
    LineTool(parent)
{
    _spinBox = nullptr;
    _button->setText("PoolLine");
    _pen = QPen(QColor(255, 255, 153), 2);
    _relAngle = 0;
}

void PoolLineTool::drawOnMouseRelease(GraphicsScene *scene, QPointF point)
{
    _endPos = point;
    intersection(_startPos, _endPos);
    if (_endPos != _startPos){
        if (poolLine != nullptr) {
            delete poolLine;
        }
        poolLine = new LineFigure(_startPos, _endPos, _pen);
    }
    _isDraw = false;
    if (_spinBox == nullptr) {
        _spinBox = new QDoubleSpinBox(_parent);
        _spinBox->setGeometry(_spinBox->x(), _spinBox->y() + 34*_nextId, _spinBox->width(), _spinBox->height());
        _spinBox->setMinimum(0);
        _spinBox->setMaximum(360);
        _spinBox->show();
        connect(_spinBox, SIGNAL(valueChanged(double)), this, SLOT(calcAngleOffset(double)));
        calcAngleOffset(0);
    }
}

void PoolLineTool::calcAngleOffset(double value)
{
    _relAngle = value*3.1416/180;
    if (poolLine != nullptr)
        _absAngle = poolLine->getAngle();
    axis->rotateAxis(_absAngle-_relAngle);
}
