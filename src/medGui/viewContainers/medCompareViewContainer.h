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


