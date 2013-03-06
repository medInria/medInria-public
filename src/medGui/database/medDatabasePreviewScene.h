/* medDatabasePreviewScene.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:48 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:43:48 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <QtGui>

class medDatabasePreviewItemGroup;
class medDatabasePreviewScenePrivate;

class medDatabasePreviewScene : public QGraphicsScene
{
public:
     medDatabasePreviewScene(QObject *parent = 0);
    ~medDatabasePreviewScene(void);

    void addGroup(medDatabasePreviewItemGroup *group);

    void reset(void);

private:
    medDatabasePreviewScenePrivate *d;
};


