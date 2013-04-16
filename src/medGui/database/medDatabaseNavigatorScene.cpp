/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
    Qt::Orientation orientation;
};

medDatabaseNavigatorScene::medDatabaseNavigatorScene(QObject *parent) : QGraphicsScene(parent), d(new medDatabaseNavigatorScenePrivate)
{
    d->orientation = medDatabaseNavigatorController::instance()->orientation();
}

medDatabaseNavigatorScene::~medDatabaseNavigatorScene(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorScene::addGroup(medDatabaseNavigatorItemGroup *group)
{
//    qreal group_width   = medDatabaseNavigatorController::instance()->groupWidth();
//    qreal group_height  = medDatabaseNavigatorController::instance()->groupHeight();
    qreal group_spacing = medDatabaseNavigatorController::instance()->groupSpacing();

    this->addItem(group);

    d->groups << group;

//    qDebug()<<"position of new group:" << d->position;
    d->orientation == Qt::Horizontal
        ? group->setPos(d->position + QPointF(10,  0))
        : group->setPos(d->position + QPointF( 0, 10));

    d->orientation == Qt::Horizontal
        ? d->position = group->pos() + group->boundingRect().topRight() + QPointF(group_spacing, 0)
        : d->position = group->pos() + group->boundingRect().bottomLeft() + QPointF(0, group_spacing);
//    qDebug()<<"group position: " << d->position;
    d->orientation == Qt::Horizontal
        ? this->setSceneRect(QRectF(0, 20, d->position.x(), 128))
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

void medDatabaseNavigatorScene::setOrientation (Qt::Orientation orientation)
{
    if (d->orientation == orientation)
        return;

    d->orientation = orientation;
    d->position = QPointF (0,0);
    this->setSceneRect(0, 0, 0, 0);

    QList<medDatabaseNavigatorItemGroup *> groups = d->groups;
    d->groups.clear();
    foreach (medDatabaseNavigatorItemGroup *group, groups) {
        this->removeItem (group);
	group->setOrientation (d->orientation);
        addGroup (group);
    }
}

Qt::Orientation medDatabaseNavigatorScene::orientation() const
{
    return d->orientation;
}
