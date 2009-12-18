/* medDatabasePreviewScene.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:54 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:43:55 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
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
    int group_count;
};

medDatabasePreviewScene::medDatabasePreviewScene(QObject *parent) : QGraphicsScene(parent), d(new medDatabasePreviewScenePrivate)
{
    d->group_count = 0;

    this->setBackgroundBrush(QColor(0x41, 0x41, 0x41));
}

medDatabasePreviewScene::~medDatabasePreviewScene(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreviewScene::addGroup(medDatabasePreviewItemGroup *group)
{
    qreal group_height  = medDatabasePreviewController::instance()->groupHeight();
    qreal group_spacing = medDatabasePreviewController::instance()->groupSpacing();

    this->addItem(group);

    medDatabasePreviewController::instance()->orientation() == Qt::Horizontal
        ? group->setPos(group_spacing, (3 - d->group_count) * (group_height + group_spacing) + group_spacing)
        : group->setPos((3 - d->group_count) * (group_height + group_spacing) + group_spacing, group_spacing);

    d->group_count++;
}

void medDatabasePreviewScene::reset(void)
{
    d->group_count = 0;
}
