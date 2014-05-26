/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationWorkspace.h>

#include <medSegmentationSelectorToolBox.h>
#include <medSegmentationAbstractToolBox.h>

#include <medAbstractView.h>

#include <medProgressionStack.h>
#include <medTabbedViewContainers.h>
#include <medViewContainer.h>
#include <medWorkspaceFactory.h>
#include <medToolBoxFactory.h>
#include <medViewEventFilter.h>
#include <medViewContainerManager.h>
#include <medRunnableProcess.h>
#include <medDataManager.h>
#include <medJobManager.h>
#include <medMetaDataKeys.h>

#include <dtkLog/dtkLog.h>

#include <stdexcept>


class medSegmentationWorkspacePrivate
{
public:
    // Give values to items without a constructor.
    medSegmentationWorkspacePrivate() :
       segmentationToolBox(NULL)
    {}

    medSegmentationSelectorToolBox *segmentationToolBox;
};

static QString msegWorkspaceSegmentationDescription = "Segmentation";


medSegmentationWorkspace::medSegmentationWorkspace(QWidget * parent /* = NULL */ ) :
medAbstractWorkspace(parent), d(new medSegmentationWorkspacePrivate)
{
    d->segmentationToolBox = new medSegmentationSelectorToolBox(parent);

    connect(d->segmentationToolBox, SIGNAL(installEventFilterRequest(medViewEventFilter*)),
            this, SLOT(addViewEventFilter(medViewEventFilter*)));

    connect(d->segmentationToolBox,SIGNAL(success()),this,SLOT(onSuccess()));

    // Always have a parent.
    if (!parent)
        throw (std::runtime_error ("Must have a parent widget"));

    this->addToolBox(d->segmentationToolBox);
}

void medSegmentationWorkspace::setupViewContainerStack()
{
    if (!stackedViewContainers()->count())
    {
        const QString description = this->description();
        this->stackedViewContainers()->addContainerInTab(description);
    }
    this->stackedViewContainers()->unlockTabs();
}

medSegmentationWorkspace::~medSegmentationWorkspace(void)
{
    delete d;
    d = NULL;
}

//static
QString medSegmentationWorkspace::description( void ) const
{
    return msegWorkspaceSegmentationDescription;
}


medSegmentationSelectorToolBox * medSegmentationWorkspace::segmentationToobox()
{
    return d->segmentationToolBox;
}


QString medSegmentationWorkspace::s_identifier()
{
    return "Segmentation";
}

QString medSegmentationWorkspace::identifier( void ) const
{
    return s_identifier();
}

bool medSegmentationWorkspace::isUsable()
{
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("segmentation").size()!=0); 
}

void medSegmentationWorkspace::addViewEventFilter( medViewEventFilter * filter)
{
    foreach(QUuid uuid, this->stackedViewContainers()->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        if(!container)
            return;
        filter->installOnView(container->view());
    }
}

//TODO: not tested yet
void medSegmentationWorkspace::onSuccess()
{
    medAbstractData *output = d->segmentationToolBox->currentToolBox()->processOutput();

    medDataManager::instance()->importNonPersistent( output );
}
