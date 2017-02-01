#include "figure.h"

Figure::Figure()
{

}

LineFigure::LineFigure(QPointF p1, QPointF p2)
{
    _p1 = p1;
    _p2 = p2;
    _pen = QPen(QColor(0, 0, 127, 127));
}

void LineFigure::draw(GraphicsScene *scene)
{
    scene->addLine(_p1.x(), _p1.y(), _p2.x(), _p2.y(), _pen);
}
