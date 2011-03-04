/* medViewerConfigurationVisualization.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:12:19 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 11:04:35 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerConfigurationVisualization.h"

#include <medGui/medViewerToolBoxView.h>
#include <medGui/medViewerToolBoxViewProperties.h>

class medViewerConfigurationVisualizationPrivate
{
public:
    medViewerToolBoxView                *viewToolBox;
    medViewerToolBoxViewProperties      *viewPropertiesToolBox;

};

medViewerConfigurationVisualization::medViewerConfigurationVisualization(QWidget *parent) : medViewerConfiguration(parent), d(new medViewerConfigurationVisualizationPrivate)
{
    // -- Layout toolbox --
    showLayoutToolBox();

    // d->layoutToolBox = new medViewerToolBoxLayout(parent);
    /*
    connect(d->layoutToolBox, SIGNAL(modeChanged(int)), this, SLOT(switchToContainer(int)));
    connect(d->layoutToolBox, SIGNAL(split(int, int)), this, SLOT(split(int, int)));
    connect(d->layoutToolBox, SIGNAL(presetClicked(int)), this, SLOT(switchToContainerPreset(int)));
    */
    
    // -- View toolbox --
    
    d->viewToolBox = new medViewerToolBoxView(parent);

    d->viewPropertiesToolBox = new medViewerToolBoxViewProperties(parent);
    /*
    connect(d->viewToolBox, SIGNAL(foregroundLookupTableChanged(QString)), this, SLOT(setupForegroundLookupTable(QString)));
    // connect(d->viewToolBox, SIGNAL(backgroundLookupTableChanged(QString)), this, SLOT(setupBackgroundLookupTable(QString)));
    connect(d->viewToolBox, SIGNAL(lutPresetChanged(QString)), this, SLOT(setupLUTPreset(QString)));
    connect(d->viewToolBox, SIGNAL(tdModeChanged(QString)), this, SLOT(setup3DMode(QString)));
    connect(d->viewToolBox, SIGNAL(tdVRModeChanged(QString)), this, SLOT(setup3DVRMode(QString)));
    connect(d->viewToolBox, SIGNAL(tdLodChanged(int)), this, SLOT(setup3DLOD(int)));
    connect(d->viewToolBox, SIGNAL(windowingChanged(bool)), this, SLOT(setupWindowing(bool)));
    connect(d->viewToolBox, SIGNAL(zoomingChanged(bool)), this, SLOT(setupZooming(bool)));
    connect(d->viewToolBox, SIGNAL(slicingChanged(bool)), this, SLOT(setupSlicing(bool)));
    connect(d->viewToolBox, SIGNAL(measuringChanged(bool)), this, SLOT(setupMeasuring(bool)));
    connect(d->viewToolBox, SIGNAL(croppingChanged(bool)), this, SLOT(setupCropping(bool)));
    connect(d->viewToolBox, SIGNAL(scalarBarVisibilityChanged(bool)), this, SLOT(setupScalarBarVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(axisVisibilityChanged(bool)), this, SLOT(setupAxisVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(rulerVisibilityChanged(bool)), this, SLOT(setupRulerVisibility(bool)));
    connect(d->viewToolBox, SIGNAL(annotationsVisibilityChanged(bool)), this, SLOT(setupAnnotationsVisibility(bool)));
     */
    
    this->addToolBox( d->viewToolBox );
    this->addToolBox(d->viewPropertiesToolBox);
    
    //this->setLayoutType( medViewerConfiguration::TopDbBottomTb );
    
}

medViewerConfigurationVisualization::~medViewerConfigurationVisualization(void)
{
    delete d;
    d = NULL;
}

QString medViewerConfigurationVisualization::description(void) const
{
    return "Visualization";
}

medViewerConfiguration *createMedViewerConfigurationVisualization(void)
{
    return new medViewerConfigurationVisualization;
}
