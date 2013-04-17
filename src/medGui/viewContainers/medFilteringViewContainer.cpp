/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medFilteringViewContainer.h"
#include <medAbstractView.h>

class medFilteringViewContainerPrivate
{
public:
    medViewContainer* inputViewContainer;
    medViewContainer* outputViewContainer;
    dtkAbstractData * inputData;
};

medFilteringViewContainer::medFilteringViewContainer ( QWidget * parent ) :
        medCustomViewContainer ( parent ), d3 ( new medFilteringViewContainerPrivate )
{
    split(1, 2);
    setMultiLayer(false);
    d3->inputViewContainer = this->childContainers()[0];
    d3->outputViewContainer = this->childContainers()[1];

    //Set cues for the user:
    d3->inputViewContainer->setInfo ( tr ( "Input to be filtered" ) );
    d3->outputViewContainer->setInfo ( tr ( "Result of filtering" ) );

    d3->outputViewContainer->setAcceptDrops(false);

    connect(d3->inputViewContainer,SIGNAL(dropped(medDataIndex)),
            this,SIGNAL(droppedInput(medDataIndex)));
}

medFilteringViewContainer::~medFilteringViewContainer()
{
    delete d3;
    d3 = NULL;
}


void medFilteringViewContainer::updateOutput ( dtkAbstractData *data )
{
    if ( !data )
        return;
    
    d3->outputViewContainer->open(data);
}

void medFilteringViewContainer::dropEvent ( QDropEvent *event )
{
    //This drop should never read any image, we are not in a leaf,
    //if we end up here, it is only because the output container doesn't accept
    //drop events.
    //we need to re-anable the updates in the widget, and accept the event to
    // stop propagation.
    this->setAttribute ( Qt::WA_UpdatesDisabled, false );
    event->acceptProposedAction();
}
