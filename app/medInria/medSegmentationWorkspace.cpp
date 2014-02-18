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

#include <medAbstractView.h>

#include <medProgressionStack.h>
#include <medTabbedViewContainers.h>
#include <medViewContainer.h>
#include <medWorkspaceFactory.h>
#include <medToolBoxFactory.h>

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
//    d->segmentationToolBox = new medSegmentationSelectorToolBox(this, parent );

//    connect(d->segmentationToolBox, SIGNAL(addToolBox(medToolBox *)), this, SLOT(addToolBox(medToolBox *)));
//    connect(d->segmentationToolBox, SIGNAL(removeToolBox(medToolBox *)), this, SLOT(removeToolBox(medToolBox *)));

//    // Always have a parent.
//    if ( !parent)
//        throw (std::runtime_error ("Must have a parent widget"));


//        //TODO make it fit with new API - RDE
////    // -- View toolboxes --
////    QList<QString> toolboxNames = medToolBoxFactory::instance()->toolBoxesFromCategory("view");
////    if(toolboxNames.contains("medViewPropertiesToolBox"))
////    {
////        // we want the medViewPropertiesToolBox to be the first "view" toolbox
////        toolboxNames.move(toolboxNames.indexOf("medViewPropertiesToolBox"),0);
////    }
////    foreach(QString toolbox, toolboxNames)
////    {
////       addToolBox( medToolBoxFactory::instance()->createToolBox(toolbox, parent) );
////    }

//    this->addWorkspaceToolBox(d->segmentationToolBox);
}

medSegmentationWorkspace::~medSegmentationWorkspace(void)
{
    delete d;
    d = NULL;
}

bool medSegmentationWorkspace::registerWithViewerWorkspaceFactory()
{
    //TODO not to be here - RDE
    return medWorkspaceFactory::instance()->registerWorkspace
            <medSegmentationWorkspace>(
                medSegmentationWorkspace::s_identifier(),
                tr("Segmentation"),
                tr("Segment Images"));
}


void medSegmentationWorkspace::setupViewContainerStack()
{
//    const QString description(this->description());
//    if (!stackedViewContainers()->count())
//    {
//        this->stackedViewContainers()->addContainerInTab(description);
//        //Default container:
////        this->connectToolboxesToCurrentContainer(createdTab);
//    }

//    this->stackedViewContainers()->unlockTabs();
}

//static
QString medSegmentationWorkspace::description( void ) const
{
    return msegWorkspaceSegmentationDescription;
}

void medSegmentationWorkspace::onViewAdded(medAbstractView* view)
{
    emit viewAdded(view);
}

void medSegmentationWorkspace::onViewRemoved(medAbstractView* view)
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

    this->addWorkspaceToolBox(d->segmentationToolBox);
}

medSegmentationSelectorToolBox * medSegmentationWorkspace::segmentationToobox()
{
    return d->segmentationToolBox;
}


void medSegmentationWorkspace::connectToolboxesToCurrentContainer(const QString &name)
{
    //TODO make it fit with new API - RDE

//    connect(stackedViewContainers()->container(name),SIGNAL(viewAdded(medAbstractView*)),
//        this,SLOT(onViewAdded(medAbstractView*)));
//    connect(stackedViewContainers()->container(name),SIGNAL(viewRemoved(medAbstractView*)),
//        this,SLOT(onViewRemoved(medAbstractView*)));
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
