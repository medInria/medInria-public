/* medDatabaseNavigatorScene.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:40:46 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Oct 26 10:30:09 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 41
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
        ? group->setPos(d->position + QPointF(10,  0))
        : group->setPos(d->position + QPointF( 0, 10));

    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
        ? d->position = group->pos() + group->boundingRect().topRight() + QPointF(group_spacing, 0)
        : d->position = group->pos() + group->boundingRect().bottomLeft() + QPointF(0, group_spacing);

    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
        ? this->setSceneRect(QRectF(0, 0, d->position.x(), 128))
        : this->setSceneRect(QRectF(20, 0, 128, d->position.y()));
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
