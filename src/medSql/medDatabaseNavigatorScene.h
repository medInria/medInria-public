/* medDatabaseNavigatorScene.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:40:41 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:40:42 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENAVIGATORSCENE_H
#define MEDDATABASENAVIGATORSCENE_H

#include <QtGui>

class medDatabaseNavigatorItemGroup;
class medDatabaseNavigatorScenePrivate;

class medDatabaseNavigatorScene : public QGraphicsScene
{
public:
     medDatabaseNavigatorScene(QObject *parent = 0);
    ~medDatabaseNavigatorScene(void);

    void addGroup(medDatabaseNavigatorItemGroup *group);

    void reset(void);

private:
    medDatabaseNavigatorScenePrivate *d;
};

#endif // MEDDATABASENAVIGATORSCENE_H
