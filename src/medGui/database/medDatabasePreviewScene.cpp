/* medDatabasePreviewScene.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:54 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Mar 20 13:52:34 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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

medDatabasePreviewScene::~medDatabasePreviewScene()
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

void medDatabasePreviewScene::reset()
{
    d->groupCount = 0;
}
