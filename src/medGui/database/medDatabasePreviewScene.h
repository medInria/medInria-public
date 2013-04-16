/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>

class medDatabasePreviewItemGroup;
class medDatabasePreviewScenePrivate;

class medDatabasePreviewScene : public QGraphicsScene
{
public:
     medDatabasePreviewScene(QObject *parent = 0);
    ~medDatabasePreviewScene();

    void addGroup(medDatabasePreviewItemGroup *group);

    void reset();

private:
    medDatabasePreviewScenePrivate *d;
};


