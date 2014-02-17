/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCustomViewContainer.h>

#include <medGuiExport.h>

class medFilteringViewContainerPrivate;
class medAbstractData;
class medDataIndex;
class dtkAbstractView;

/**
 * @brief filtering view container to visualize both input / output from image-to-image filtering processes
 */
class MEDGUI_EXPORT medFilteringViewContainer : public medCustomViewContainer
{
    Q_OBJECT

public:
    /**
     * @brief
     * @param parent
     */
    medFilteringViewContainer ( QWidget * parent = 0 );
    ~medFilteringViewContainer();

signals:

    /**
     * @brief signals emitted whenever an image has been dropped in the input view container
     */
    void droppedInput (const medDataIndex& index);

    void inputViewRemoved(dtkAbstractView *);
    
public slots:

    /**
     * @brief update output view according to the data in parameter
     * @param data
     */
    void updateOutput ( medAbstractData *data );

    void dropEvent ( QDropEvent *event );
    
private:
    medFilteringViewContainerPrivate *d3;
};


