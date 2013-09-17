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

#include <dtkCore/dtkSmartPointer.h>

class dtkAbstractView;
class medViewContainer;
class medViewPool;

class medViewContainerPrivate
{
public:
    QGridLayout *layout;

    dtkSmartPointer<dtkAbstractView> view;

    medViewContainer *current;

    // for view synchronization
    /*
      QList<dtkAbstractView *> views;
      dtkAbstractView         *refView;
      int                      synchronize;
    */
    medViewPool *pool;

    QHash<QString, QString> viewProperties;
    QString viewInfo;
    bool multiLayer;
    bool multiSelectionEnabled;
    bool selected;
};


