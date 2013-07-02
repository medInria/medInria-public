/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSegmentationWorkspace.h"

#include "medSegmentationSelectorToolBox.h"

#include <medCore/medAbstractView.h>

#include <medProgressionStack.h>
#include <medTabbedViewContainers.h>
#include <medViewContainer.h>
#include <medWorkspaceFactory.h>
#include <medVisualizationLayoutToolBox.h>
#include <medViewPropertiesToolBox.h>
#include <medToolBoxFactory.h>

#include <dtkLog/dtkLog.h>

#include <stdexcept>

// /////////////////////////////////////////////////////////////////
// WorkspacePrivate
// /////////////////////////////////////////////////////////////////
class medSegmentationWorkspacePrivate
{
public:
    // Give values to items without a constructor.
    medSegmentationWorkspacePrivate() :
      layoutToolBox(NULL), viewPropertiesToolBox(NULL), segmentationToolBox(NULL)
    {}

    medVisualizationLayoutToolBox *layoutToolBox;
    medViewPropertiesToolBox *viewPropertiesToolBox;

    medSegmentationSelectorToolBox *segmentationToolBox;
};

// /////////////////////////////////////////////////////////////////
// medSegmentationWorkspace
// /////////////////////////////////////////////////////////////////

static QString msegWorkspaceSegmentationDescription = "Segmentation";


medSegmentationWorkspace::medSegmentationWorkspace(QWidget * parent /* = NULL */ ) :
medWorkspace(parent), d(new medSegmentationWorkspacePrivate)
{
    d->segmentationToolBox = new medSegmentationSelectorToolBox(this, parent );

    connect(d->segmentationToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
    connect(d->segmentationToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));

    // Always have a parent.
    if ( !parent)
        throw (std::runtime_error ("Must have a parent widget"));

    // -- Layout toolbox --
    d->layoutToolBox = new medVisualizationLayoutToolBox(parent);

    connect (d->layoutToolBox, SIGNAL(modeChanged(const QString&)),
        this,             SIGNAL(layoutModeChanged(const QString&)));
    connect (d->layoutToolBox, SIGNAL(presetClicked(int)),
        this,             SIGNAL(layoutPresetClicked(int)));
    connect (d->layoutToolBox, SIGNAL(split(int,int)),
        this,             SIGNAL(layoutSplit(int,int)));

    connect(this,SIGNAL(setLayoutTab(const QString &)), d->layoutToolBox, SLOT(setTab(const QString &)));

    // -- View toolbox --

    d->viewPropertiesToolBox = new medViewPropertiesToolBox(parent);


    connect ( this, SIGNAL(layoutModeChanged(const QString &)),
        stackedViewContainers(), SLOT(changeCurrentContainerType(const QString &)));
    connect ( stackedViewContainers(), SIGNAL(currentChanged(const QString &)),
        this, SLOT(connectToolboxesToCurrentContainer(const QString &)));

    this->addToolBox( d->layoutToolBox );
    this->addToolBox( d->viewPropertiesToolBox );
    this->addToolBox( d->segmentationToolBox );
}

medSegmentationWorkspace::~medSegmentationWorkspace(void)
{
    delete d;
    d = NULL;
}

bool medSegmentationWorkspace::registerWithViewerWorkspaceFactory()
{
    return medWorkspaceFactory::instance()->registerWorkspace
            <medSegmentationWorkspace>(
                medSegmentationWorkspace::s_identifier(),
                tr("Segmentation"),
                tr("Segment Images"));
}


void medSegmentationWorkspace::setupViewContainerStack()
{
    const QString description(this->description());
    if (!stackedViewContainers()->count())
    {
        QString createdTab = addDefaultTypeContainer(description);
        //Default container:
        this->connectToolboxesToCurrentContainer(createdTab);
    }

    this->stackedViewContainers()->unlockTabs();
}

//static
QString medSegmentationWorkspace::description( void ) const
{
    return msegWorkspaceSegmentationDescription;
}

void medSegmentationWorkspace::onViewAdded( dtkAbstractView* view )
{
    emit viewAdded(view);
}

void medSegmentationWorkspace::onViewRemoved( dtkAbstractView* view )
{
    emit viewRemoved(view);
}


medProgressionStack * medSegmentationWorkspace::progressionStack()
{
    return d->segmentationToolBox->progressionStack();
}

void medSegmentationWorkspace::buildWorkspace(  )
{
    QWidget * parent = qobject_cast<QWidget *>(this->parent());
    if ( !d->segmentationToolBox)
        d->segmentationToolBox = new medSegmentationSelectorToolBox( this, parent );

    this->addToolBox( d->segmentationToolBox );
}

medSegmentationSelectorToolBox * medSegmentationWorkspace::segmentationToobox()
{
    return d->segmentationToolBox;
}


void medSegmentationWorkspace::connectToolboxesToCurrentContainer(const QString &name)
{
    connect(stackedViewContainers()->container(name),SIGNAL(viewAdded(dtkAbstractView*)),
        this,SLOT(onViewAdded(dtkAbstractView*)));
    connect(stackedViewContainers()->container(name),SIGNAL(viewRemoved(dtkAbstractView*)),
        this,SLOT(onViewRemoved(dtkAbstractView*)));
}

QString medSegmentationWorkspace::s_identifier()
{
    return "msegWorkspace";
}

QString medSegmentationWorkspace::identifier( void ) const
{
    return s_identifier();
}

bool medSegmentationWorkspace::isUsable(){
    medToolBoxFactory * tbFactory = medToolBoxFactory::instance();
    return (tbFactory->toolBoxesFromCategory("segmentation").size()!=0); 
}
