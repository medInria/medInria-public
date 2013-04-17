/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabasePreviewController.h"
#include "medDatabasePreviewItemGroup.h"
#include "medDatabasePreviewScene.h"

#include <QtCore>
#include <QtGui>

class medDatabasePreviewScenePrivate
{
public:
    int groupCount;
};

medDatabasePreviewScene::medDatabasePreviewScene(QObject *parent) : QGraphicsScene(parent), d(new medDatabasePreviewScenePrivate)
{
    d->groupCount = 0;

    this->setBackgroundBrush(QColor(0x41, 0x41, 0x41));
}

medDatabasePreviewScene::~medDatabasePreviewScene(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreviewScene::addGroup(medDatabasePreviewItemGroup *group)
{
    qreal groupHeight  = medDatabasePreviewController::instance()->groupHeight();
    qreal groupSpacing = medDatabasePreviewController::instance()->groupSpacing();

    this->addItem(group);

    medDatabasePreviewController::instance()->orientation() == Qt::Horizontal
        ? group->setPos(groupSpacing, (1 - d->groupCount) * (groupHeight + groupSpacing) + groupSpacing)
        : group->setPos((1 - d->groupCount) * (groupHeight + groupSpacing) + groupSpacing, groupSpacing);

    d->groupCount++;
}

void medDatabasePreviewScene::reset(void)
{
    d->groupCount = 0;
}
