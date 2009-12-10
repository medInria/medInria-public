#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItemGroup.h"
#include "medDatabaseNavigatorScene.h"

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorScenePrivate
{
public:
    QList<medDatabaseNavigatorItemGroup *> groups;

    QPointF position;
};

medDatabaseNavigatorScene::medDatabaseNavigatorScene(QObject *parent) : QGraphicsScene(parent), d(new medDatabaseNavigatorScenePrivate)
{
//    this->setBackgroundBrush(Qt::red);
}

medDatabaseNavigatorScene::~medDatabaseNavigatorScene(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorScene::addGroup(medDatabaseNavigatorItemGroup *group)
{
    qreal group_width   = medDatabaseNavigatorController::instance()->groupWidth();
    qreal group_height  = medDatabaseNavigatorController::instance()->groupHeight();
    qreal group_spacing = medDatabaseNavigatorController::instance()->groupSpacing();

    this->addItem(group);

    d->groups << group;

    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
        ? group->setPos(QPointF(d->position.x() + group_spacing, d->position.y()))
        : group->setPos(QPointF(d->position.x(), d->position.y() + group_spacing));

    switch(medDatabaseNavigatorController::instance()->orientation()) {
    case Qt::Horizontal: d->position = group->boundingRect().topRight(); break;
    case Qt::Vertical: d->position = group->boundingRect().bottomLeft(); break;
    default: break;
    }

    this->setSceneRect(0, 0, this->sceneRect().right(), this->sceneRect().bottom());
}

void medDatabaseNavigatorScene::reset(void)
{
    foreach(medDatabaseNavigatorItemGroup *group, d->groups) {
        this->removeItem(group);
        delete group;
    }

    d->groups.clear();

    d->position.setX(0);
    d->position.setY(0);

    this->setSceneRect(0, 0, 0, 0);
}
