#include "sceneobject.h"
#include <QDebug>
#include <math.h>

SceneObject::SceneObject(int width, int height, QPen pen, QBrush brush)
{
    this->rect = QRect(0, 0, width, height);
    this->pen = pen;
    this->brush = brush;
    this->setFlag(GraphicsItemFlag::ItemIsMovable);
}

QRectF SceneObject::boundingRect() const {
    return QRectF(this->rect);
}

void SceneObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << this->x() << " " << this->y();
    update();
    QGraphicsItem::mouseMoveEvent(event);
}

void SceneObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(this->pen);
    painter->setBrush(this->brush);
    painter->drawRect(this->rect);
}
