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

class medAbstractView;
class medViewContainer;
class medViewPool;

class medViewContainerPrivate
{
public:
    QGridLayout *layout;
    medAbstractView *view;
    medViewContainer *current;

    QString viewInfo;
    bool multiLayer;
    bool multiSelectionEnabled;
    bool selected;
};


