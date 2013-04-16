/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <medCustomViewContainer.h>


#include "medGuiExport.h"

class medCompareViewContainerPrivate;

class MEDGUI_EXPORT medCompareViewContainer : public medCustomViewContainer
{
    Q_OBJECT
public:
    medCompareViewContainer(QWidget * parent = 0);
    ~medCompareViewContainer();

signals:
//  fixedViewAdded(dtkAbstractView*);
//  movingViewAdded(dtkAbstractView*);
    void droppedFixed  (const medDataIndex& index);
    void droppedMoving (const medDataIndex &index);

private:
    medCompareViewContainerPrivate *d3;
};


