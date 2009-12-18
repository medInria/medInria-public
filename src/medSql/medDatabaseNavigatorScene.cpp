/* medDatabaseNavigatorScene.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:40:46 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:40:46 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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
        ? group->setPos(d->position + QPointF(10, 0))
        : group->setPos(d->position + QPointF(0, 10));

    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
        ? d->position = group->boundingRect().topRight() + QPointF(group_spacing, 0)
        : d->position = group->boundingRect().bottomLeft() + QPointF(0, group_spacing);

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
