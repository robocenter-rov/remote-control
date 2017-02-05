#include "figure.h"
#include <QtCore>

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
    if (!_area.isEmpty()) {
        scene->addPolygon(_area, QPen(QColor(127, 0, 127, 127)));
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

RectFigure::RectFigure(QPointF p1, QPointF p2) :
    Figure()
{
    _p1 = p1;
    _p2 = p2;
}

void RectFigure::draw(GraphicsScene *scene)
{
    scene->addRect(QRectF(_p1, _p2), _pen);
    if (!_area.isEmpty()) {
        scene->addRect(_area);
    }
}

bool RectFigure::inArea(QPointF p)
{
    calcArea();
    if ((p.x() > min(_p1.x(), _p2.x()) - _offset) &&
        (p.x() < max(_p1.x(), _p2.x()) + _offset) &&
        (p.y() > min(_p1.y(), _p2.y()) - _offset) &&
        (p.y() < max(_p1.y(), _p2.y()) + _offset))
        return true;
    return false;
}

void RectFigure::resetPoints(QPointF deltaPoint)
{
    _p1.setX(_p1.x() + deltaPoint.x());
    _p1.setY(_p1.y() + deltaPoint.y());
    _p2.setX(_p2.x() + deltaPoint.x());
    _p2.setY(_p2.y() + deltaPoint.y());
    calcArea();
}

void RectFigure::calcArea()
{
    _area = QRectF(QPointF(min(_p1.x(), _p2.x()) - _offset, min(_p1.y(), _p2.y()) - _offset),
                   QPointF(max(_p1.x(), _p2.x()) + _offset, max(_p1.y(), _p2.y()) + _offset));
}
