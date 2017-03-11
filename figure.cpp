#include "figure.h"
#include <QtCore>
#include <string>
#include "basetool.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

Figure::Figure()
{
    _pen = QPen(QColor(0, 0, 127, 127));
}

LineFigure::LineFigure(QPointF p1, QPointF p2) :
    Figure()
{
    _p1 = (p1.x() < p2.x()) ? p1 : p2;
    _p2 = (p1.x() < p2.x()) ? p2 : p1;
    double deltay = _p2.y() - _p1.y();
    double deltax = _p2.x() - _p1.x();
    _angle = atan(deltay/deltax);
    calcArea();
}

void LineFigure::draw(GraphicsScene *scene)
{
    scene->addLine(_p1.x(), _p1.y(), _p2.x(), _p2.y(), _pen);
}

void LineFigure::drawArea(GraphicsScene *scene)
{
    if (!_area.isEmpty()) {
        scene->addPolygon(_area, QPen(QColor(127, 0, 127, 127)));
        scene->addText(getInfo());
    }
}

#include <math.h>
static QPointF rotate(QPointF p, double angle)
{
    return QPointF(p.x()*cos(angle) - p.y()*sin(angle), p.x()*sin(angle) + p.y()*cos(angle));
}

bool LineFigure::inArea(QPointF p)
{
    calcArea();
    QPointF t = rotate(QPointF(p.x() - _p1.x(), p.y() - _p1.y()), -_angle);
    if (((t.x() > 0) &&
         (t.x() < rotatedEndPoint2().x())) &&
        ((t.y() > - _offset) &&
         (t.y() < _offset)))
        return true;
    return false;
}

QPointF LineFigure::rotatedEndPoint2()
{
    return rotate(QPointF(_p2.x() - _p1.x(), _p2.y() - _p1.y()), -_angle);
}

void LineFigure::calcArea()
{
    QPointF np2 = rotatedEndPoint2();

    QPointF p1t = rotate(QPointF(0, -_offset), _angle); // p1t point1top
    QPointF p1b = rotate(QPointF(0, _offset), _angle);  // p1b point1bottom
    QPointF p2t = rotate(QPointF(np2.x(), -_offset), _angle);
    QPointF p2b = rotate(QPointF(np2.x(), _offset), _angle);

    QPointF r1t(_p1.x() + p1t.x(), _p1.y() + p1t.y()); // result1top
    QPointF r1b(_p1.x() + p1b.x(), _p1.y() + p1b.y()); // result1bottom
    QPointF r2t(_p1.x() + p2t.x(), _p1.y() + p2t.y()); // ...
    QPointF r2b(_p1.x() + p2b.x(), _p1.y() + p2b.y()); // ...

    QVector <QPointF> points;
    points << r1t << r2t << r2b << r1b;

    _area = QPolygonF(points);
}

void LineFigure::resetPoints(QPointF deltaPoint)
{
    _p1.setX(_p1.x() + deltaPoint.x());
    _p1.setY(_p1.y() + deltaPoint.y());
    _p2.setX(_p2.x() + deltaPoint.x());
    _p2.setY(_p2.y() + deltaPoint.y());
    calcArea();
}

int LineFigure::length()
{
    return sqrt(pow(_p2.x() - _p1.x(), 2) + pow(_p2.y() - _p1.y(), 2));
}

QString LineFigure::getInfo()
{
    QString s, len;
    len.setNum(length());
    s.append(len);
    s.append(" spt");
    if (scaleCoef) {
        s.append("\n");
        s.append(len.setNum(length()/scaleCoef));
        s.append(" cm");
    }
    return s;
}

RectFigure::RectFigure(QPointF p1, QPointF p2) :
    Figure()
{
    _points << QPointF(min(p1.x(), p2.x()), min(p1.y(), p2.y()))
            << QPointF(max(p1.x(), p2.x()), min(p1.y(), p2.y()))
            << QPointF(max(p1.x(), p2.x()), max(p1.y(), p2.y()))
            << QPointF(min(p1.x(), p2.x()), max(p1.y(), p2.y()));
    _pCenter = QPointF((p1.x() + p2.x())/2, (p1.y() + p2.y())/2);
    double deltay = _points[0].y() - _points[1].y();
    double deltax = _points[0].x() - _points[1].x();
    _angle = atan(deltay/deltax);
    calcArea();
}

void RectFigure::draw(GraphicsScene *scene)
{
    scene->addPolygon(QPolygonF(_points), _pen);
}

bool RectFigure::inArea(QPointF p)
{
    calcArea();
    QPointF t = rotate(QPointF(p.x() - _pCenter.x(), p.y() - _pCenter.y()), -_angle);
    QVector <QPointF> points = rotatedPoints();
    if ((t.x() > points[0].x() - _offset) &&
        (t.x() < points[1].x() + _offset) &&
        (t.y() > points[0].y() - _offset) &&
        (t.y() < points[3].y() + _offset))
        return true;
    return false;
}

void RectFigure::resetPoints(QPointF deltaPoint)
{
    for (auto it = _points.begin(); it != _points.end(); it++) {
        it->setX(it->x() + deltaPoint.x());
        it->setY(it->y() + deltaPoint.y());
    }
    _pCenter.setX(_pCenter.x() + deltaPoint.x());
    _pCenter.setY(_pCenter.y() + deltaPoint.y());
    calcArea();
}

QVector <QPointF> RectFigure::rotatedPoints()
{
    QVector <QPointF> points;
    for (auto it = _points.begin(); it != _points.end(); it++) {
        points.append(rotate(QPointF(it->x() - _pCenter.x(), it->y() - _pCenter.y()), -_angle));
    }
    return points;
}

void RectFigure::calcArea()
{
    QVector <QPointF> points = rotatedPoints();

    QVector <QPointF> r;
    int i = 0, j = 0;
    for (auto it = points.begin(); it != points.end(); it++) {
        int signX = (i++ % 3) ? 1 : -1;
        int signY = (j++ > 1) ? 1 : -1;
        QPointF t = rotate(QPointF(it->x() + signX * _offset, it->y() + signY * _offset), _angle);
        r.append(QPointF(_pCenter.x() + t.x(), _pCenter.y() + t.y()));
    }
    _area = QPolygonF(r);
}

void RectFigure::drawArea(GraphicsScene *scene)
{
    if (!_area.isEmpty()) {
        scene->addPolygon(_area, QPen(QColor(127, 0, 127, 127)));
        scene->addText(getInfo());
    }
}

QString RectFigure::getInfo()
{
    QString s("RectFigure:\t"), w, h;
    w.setNum(width()/scaleCoef); h.setNum(height()/scaleCoef);
    s.append(w + "x" + h);
    return s;
}

int RectFigure::width()
{
    return sqrt(pow(_points[0].x() - _points[1].x(), 2) + pow(_points[0].y() - _points[1].y(), 2));
}

int RectFigure::height()
{
    return sqrt(pow(_points[0].x() - _points[3].x(), 2) + pow(_points[0].y() - _points[3].y(), 2));
}
