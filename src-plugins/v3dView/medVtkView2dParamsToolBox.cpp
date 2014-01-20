/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medVtkView2dParamsToolBox.h>
#include <medToolBoxFactory.h>
#include <medVtkView.h>
#include <v3dView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <medSettingsManager.h>

//////////////
//
//  medVtkView2dParamsToolBox
//
//////////////

class medVtkView2dParamsToolBoxPrivate
{
public:
    medVtkView  * vtkView;
    QMap<medVtkView*, QWidget*> widgetList;
    QMultiMap<medVtkView*, medAbstractParameter*> viewParamMap;
};

medVtkView2dParamsToolBox::medVtkView2dParamsToolBox(QWidget *parent)
    : medToolBox(parent), d(new medVtkView2dParamsToolBoxPrivate)
{
    d->vtkView = 0;
}


bool medVtkView2dParamsToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medVtkView2dParamsToolBox>("medVtkView2dParamsToolBox","medVtkView2dParamsToolBox",
                                                                                "View management toolbox", QStringList());
}

QList<medAbstractParameter*> medVtkView2dParamsToolBox::getParameters(medVtkView* view)
{
    return d->viewParamMap.values(view);
}

void medVtkView2dParamsToolBox::update(dtkAbstractView * view)
{
    // Hide this toolbox when no views
    if(!view)
    {
        this->hide();
        d->vtkView = 0;
    }
    else
        this->show();

    if (d->vtkView)
        d->vtkView->disconnect(this);

    d->vtkView = qobject_cast<medVtkView*>(view);

    QMapIterator<medVtkView*, QWidget*> i(d->widgetList);

    while (i.hasNext())
    {
        i.next();
        i.value()->hide();
    }

    QMap<medVtkView*, QWidget*>::const_iterator iterator = d->widgetList.find(d->vtkView);
    if(iterator != d->widgetList.end())
    {
        iterator.value()->show();
        return;
    }

    // Mouse Interaction

    medActionParameter *windowingParam = new medActionParameter("Windowing", view);
    windowingParam->setIcon(QIcon (":/icons/wlww.png"));
    windowingParam->setCheckable(true);


    medActionParameter *zoomingParam = new medActionParameter("Zooming", view);
    zoomingParam->setIcon(QIcon (":/icons/magnify.png"));
    zoomingParam->setCheckable(true);


    medActionParameter *slicingParam = new medActionParameter("Slicing", view);
    slicingParam->setIcon(QIcon (":/icons/stack.png"));
    slicingParam->setCheckable(true);


    medActionParameter *measuringParam = new medActionParameter("Measuring", view);
    measuringParam->setIcon(QIcon (":/icons/length.png"));
    measuringParam->setCheckable(true);


    medActionParameterGroup *mouseInteractionGroup = new medActionParameterGroup (this);
    mouseInteractionGroup->addActionParameter ( windowingParam );
    mouseInteractionGroup->addActionParameter ( zoomingParam );
    mouseInteractionGroup->addActionParameter ( slicingParam );
    mouseInteractionGroup->addActionParameter ( measuringParam );
    mouseInteractionGroup->setExclusive (true);


    // Display

    medBooleanParameter *scalarBarParam = new medBooleanParameter("ScalarBar", view);
    medBooleanParameter *axisParam = new medBooleanParameter("Axis", view);
    medBooleanParameter *rulerParam = new medBooleanParameter("Ruler", view);
    rulerParam->setValue(true);
    medBooleanParameter *annotationsParam = new medBooleanParameter("Annotations", view);
    annotationsParam->setValue(true);


    // Layouts
    QWidget *toolboxWidget = new QWidget(this);

    QHBoxLayout *mouseInteractionLayout = new QHBoxLayout;
    mouseInteractionLayout->addWidget(windowingParam->getWidget());
    mouseInteractionLayout->addWidget(zoomingParam->getWidget());
    mouseInteractionLayout->addWidget(slicingParam->getWidget());
    mouseInteractionLayout->addWidget(measuringParam->getWidget());


    medActionParameterGroup *mouseGroup = new medActionParameterGroup (this);
    mouseGroup->addActionParameter ( windowingParam );
    mouseGroup->addActionParameter ( zoomingParam );
    mouseGroup->addActionParameter ( slicingParam );
    mouseGroup->addActionParameter ( measuringParam );
    mouseGroup->setExclusive (true);

    QHBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(scalarBarParam->getWidget());
    displayLayout->addWidget(axisParam->getWidget());
    displayLayout->addWidget(rulerParam->getWidget());
    displayLayout->addWidget(annotationsParam->getWidget());

    QVBoxLayout *toolBoxLayout = new QVBoxLayout(toolboxWidget);
    toolBoxLayout->addLayout(mouseInteractionLayout);
    toolBoxLayout->addLayout(displayLayout);

    this->addWidget(toolboxWidget);

    windowingParam->hideLabel();
    zoomingParam->hideLabel();
    slicingParam->hideLabel();
    measuringParam->hideLabel();
    scalarBarParam->hideLabel();
    axisParam->hideLabel();
    rulerParam->hideLabel();
    annotationsParam->hideLabel();

    d->widgetList.insert(d->vtkView, toolboxWidget);

    // Connection
    connect(windowingParam, SIGNAL(triggered(bool)),this, SLOT(setWindowing(bool)));
    connect(zoomingParam, SIGNAL(triggered(bool)),this, SLOT(setZooming(bool)));
    connect(slicingParam, SIGNAL(triggered(bool)),this, SLOT(setSlicing(bool)));
    connect(measuringParam, SIGNAL(triggered(bool)),this, SLOT(setMeasuring(bool)));
    connect(scalarBarParam, SIGNAL(valueChanged(bool)), this, SLOT(setScalarBarVisibility(bool)));
    connect(axisParam, SIGNAL(valueChanged(bool)), this, SLOT(setAxisVisibility(bool)));
    connect(rulerParam, SIGNAL(valueChanged(bool)), this, SLOT(setRulerVisibility(bool)));
    connect(annotationsParam, SIGNAL(valueChanged(bool)), this, SLOT(setAnnotationsVisibility(bool)));


    d->viewParamMap.insert(d->vtkView, windowingParam);
    d->viewParamMap.insert(d->vtkView, zoomingParam);
    d->viewParamMap.insert(d->vtkView, slicingParam);
    d->viewParamMap.insert(d->vtkView, measuringParam);
    d->viewParamMap.insert(d->vtkView, scalarBarParam);
    d->viewParamMap.insert(d->vtkView, axisParam);
    d->viewParamMap.insert(d->vtkView, rulerParam);
    d->viewParamMap.insert(d->vtkView, annotationsParam);


    //get default Mouse interaction from medSettings
    medSettingsManager * mnger = medSettingsManager::instance();
    QString mouseInteraction = mnger->value("interactions","mouse", "Windowing").toString();

    if(mouseInteraction == "Zooming")
        zoomingParam->setChecked(true);
    else if(mouseInteraction ==  "Windowing")
        windowingParam->setChecked(true);
    else if(mouseInteraction ==  "Slicing")
        slicingParam->setChecked(true);
    else if(mouseInteraction ==  "Measuring")
        measuringParam->setChecked(true);

}


void medVtkView2dParamsToolBox::setWindowing(bool checked)
{
    if(checked && d->vtkView)
    {
        d->vtkView->setMouseInteraction("Windowing");
        d->vtkView->update();
    }
}

void medVtkView2dParamsToolBox::setZooming(bool checked)
{
    if(checked && d->vtkView)
    {
        d->vtkView->setMouseInteraction("Zooming");
        d->vtkView->update();
    }
}

void medVtkView2dParamsToolBox::setSlicing(bool checked)
{
    if(checked && d->vtkView)
    {
        d->vtkView->setMouseInteraction("Slicing");
        d->vtkView->update();
    }
}

void medVtkView2dParamsToolBox::setMeasuring(bool checked)
{
    if(checked && d->vtkView)
    {
        d->vtkView->setMouseInteraction("Measuring");
        d->vtkView->update();
    }
}

void medVtkView2dParamsToolBox::setAxisVisibility(bool visible)
{
    if (d->vtkView)
    {
        d->vtkView->showAxis(visible);
        d->vtkView->update();
    }
}

void medVtkView2dParamsToolBox::setScalarBarVisibility(bool visible)
{
    if (d->vtkView)
    {
        d->vtkView->showScalarBar(visible);
        d->vtkView->update();
    }
}

void medVtkView2dParamsToolBox::setRulerVisibility(bool visible)
{
    if (d->vtkView)
    {
        d->vtkView->showRuler(visible);
        d->vtkView->update();
    }
}

void medVtkView2dParamsToolBox::setAnnotationsVisibility(bool visible)
{
    if (d->vtkView)
    {
        d->vtkView->showAnnotations(visible);
        d->vtkView->update();
    }
}
