/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>

class medDatabaseNavigatorItemGroup;
class medDatabaseNavigatorScenePrivate;

class medDatabaseNavigatorScene : public QGraphicsScene
{
public:
     medDatabaseNavigatorScene(QObject *parent = 0);
    ~medDatabaseNavigatorScene();

    void addGroup(medDatabaseNavigatorItemGroup *group);

    void reset();

    void setOrientation (Qt::Orientation orientation);
    Qt::Orientation orientation() const;

private:
    medDatabaseNavigatorScenePrivate *d;
};


