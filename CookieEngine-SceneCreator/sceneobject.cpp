#include "sceneobject.h"
#include "qgraphicssceneevent.h"
#include "globals.h"
#include <QDebug>
#include <math.h>

SceneObject* SceneObject::selectedObject = NULL;

SceneObject::SceneObject(QString objectType, int width, int height, QPen pen, QBrush brush)
{
    this->objectType = objectType;
    this->rect = QRect(0, 0, width, height);
    this->pen = pen;
    this->brush = brush;
    this->setFlag(GraphicsItemFlag::ItemIsMovable);
    this->setFlag(GraphicsItemFlag::ItemSendsGeometryChanges);
}

QRectF SceneObject::boundingRect() const {
    return QRectF(this->rect);
}

void SceneObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(this->pen);
    painter->setBrush(this->brush);
    painter->drawRect(this->rect);
}

QString SceneObject::getObjectType() const {
    return this->objectType;
}

QVariant SceneObject::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        QPointF pos = value.toPointF();
        int CELL_SIZE = Globals::CELL_SIZE;
        int x = Globals::snapToGrid ? floor(pos.x() / CELL_SIZE) * CELL_SIZE : pos.x();
        int y = Globals::snapToGrid ? floor(pos.y() / CELL_SIZE) * CELL_SIZE : pos.y();
        return QPointF(x, y);
    }
    return QGraphicsItem::itemChange(change, value);
}

void SceneObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    this->selectedObject = this;
}
