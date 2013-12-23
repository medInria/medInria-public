#include <medViewParamsToolBox.h>
#include <medToolBoxFactory.h>
#include <medVtkView.h>
#include <medView3dParamsToolBox.h>
#include <medToolBoxHeader.h>


class medViewParamsToolBoxPrivate
{
public:

    medVtkView  * vtkView;
    QMap<medVtkView*, QWidget*> widgetList;
    QMultiMap<medVtkView*, medAbstractParameter*> viewParamMap;
    QWidget *mainOrientationWidget;
    QVBoxLayout *mainOrientationLayout;
    medView2dParamsToolBox *view2dToolBox;
    medView3dParamsToolBox *view3dToolBox;

};

medViewParamsToolBox::medViewParamsToolBox(QWidget *parent)
    : medToolBox(parent), d(new medViewParamsToolBoxPrivate)
{
    d->vtkView = 0;

    d->view2dToolBox = new medView2dParamsToolBox;
    d->view2dToolBox->header()->hide();
    d->view3dToolBox = new medView3dParamsToolBox;
    d->view3dToolBox->header()->hide();

    d->mainOrientationWidget = new QWidget(this);
    d->mainOrientationLayout = new QVBoxLayout;
    d->mainOrientationWidget->setLayout(d->mainOrientationLayout);

    this->addWidget(d->mainOrientationWidget);
    this->addWidget(d->view2dToolBox);
    this->addWidget(d->view3dToolBox);

}


bool medViewParamsToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medViewParamsToolBox>("medViewParamsToolBox","medViewParamsToolBox",
                                                                                "View management toolbox", QStringList());
}

QList<medAbstractParameter*> medViewParamsToolBox::getParameters(medVtkView* view)
{
    QList<medAbstractParameter*> viewParams;
    viewParams.append(d->viewParamMap.values(view));
    viewParams.append(d->view2dToolBox->getParameters(view));
    viewParams.append(d->view3dToolBox->getParameters(view));
    return viewParams;

}

void medViewParamsToolBox::update(dtkAbstractView * view)
{
    d->view2dToolBox->update(view);
    d->view3dToolBox->update(view);

    // Hide this toolbox when no views
    if(!view)
    {
        this->hide();
        d->vtkView = 0;
        return;
    }
    else
        this->show();

    if(view->property("Orientation") == "3D")
        setViewMode(display3d);
    else setViewMode(display2d);

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

    // Orientation

    medActionParameter *axialParam = new medActionParameter("Axial", view);
    axialParam->setIcon( QIcon (":/icons/AxialIcon.png"));
    axialParam->setCheckable(true);
    axialParam->setIconSize(45,45);
    axialParam->hideLabel();

    medActionParameter *coronalParam = new medActionParameter("Coronal", view);
    coronalParam->setIcon(QIcon (":/icons/CoronalIcon.png"));
    coronalParam->setCheckable(true);
    coronalParam->setIconSize(45,45);
    coronalParam->hideLabel();

    medActionParameter *sagitalParam = new medActionParameter("Sagital", view);
    sagitalParam->setIcon(QIcon (":/icons/SagittalIcon.png"));
    sagitalParam->setCheckable(true);
    sagitalParam->setIconSize(45,45);
    sagitalParam->hideLabel();

    medActionParameter *view3DParam = new medActionParameter("3D", view);
    view3DParam->setIcon(QIcon (":/icons/3DIcon.png"));
    view3DParam->setCheckable(true);
    view3DParam->setIconSize(45,45);
    view3DParam->hideLabel();

    medActionParameterGroup *orientationGroup = new medActionParameterGroup (this);
    orientationGroup->addActionParameter ( axialParam );
    orientationGroup->addActionParameter ( coronalParam );
    orientationGroup->addActionParameter ( sagitalParam );
    orientationGroup->addActionParameter ( view3DParam );
    orientationGroup->setExclusive (true);


    // Layouts
    QWidget *orientationWidget = new QWidget;
    QHBoxLayout *orientationLayout = new QHBoxLayout(orientationWidget);

    orientationLayout->addWidget(axialParam->getWidget());
    orientationLayout->addWidget(coronalParam->getWidget());
    orientationLayout->addWidget(sagitalParam->getWidget());
    orientationLayout->addWidget(view3DParam->getWidget());

    d->mainOrientationLayout->addWidget(orientationWidget);
    d->mainOrientationLayout->setContentsMargins(0,0,0,0);

    d->widgetList.insert(d->vtkView, orientationWidget);


    // Connection
    connect(axialParam,SIGNAL(triggered(dtkAbstractView*,bool)),this,SLOT(setAxial(dtkAbstractView*,bool)));
    connect(coronalParam,SIGNAL(triggered(dtkAbstractView*,bool)),this,SLOT(setCoronal(dtkAbstractView*,bool)));
    connect(sagitalParam,SIGNAL(triggered(dtkAbstractView*,bool)),this,SLOT(setSagittal(dtkAbstractView*,bool)));
    connect(view3DParam,SIGNAL(triggered(dtkAbstractView*,bool)),this,SLOT(setView3D(dtkAbstractView*,bool)));

    d->viewParamMap.insert(d->vtkView, axialParam);
    d->viewParamMap.insert(d->vtkView, coronalParam);
    d->viewParamMap.insert(d->vtkView, sagitalParam);
    d->viewParamMap.insert(d->vtkView, view3DParam);

    setViewMode(display2d);

}


void medViewParamsToolBox::setAxial(dtkAbstractView* view, bool checked)
{
    if (checked && view){
        view->setProperty ( "Orientation", "Axial" );
        view->update();
        setViewMode(display2d);
    }
}


void medViewParamsToolBox::setSagittal(dtkAbstractView* view, bool checked)
{
    if (checked && view){
        view->setProperty ( "Orientation", "Sagittal" );
        view->update();
        setViewMode(display2d);
    }
}

void medViewParamsToolBox::setCoronal(dtkAbstractView* view, bool checked)
{
    if (checked && view){
        view->setProperty ( "Orientation", "Coronal" );
        view->update();
        setViewMode(display2d);
    }
}

void medViewParamsToolBox::setView3D(dtkAbstractView* view, bool checked)
{
    if (checked && view){
        view->setProperty ( "3DMode", view->property ( "3DMode" ) );
        view->setProperty ( "Orientation", "3D" );
        view->update();
        setViewMode(display3d);
    }
}

void medViewParamsToolBox::setViewMode(viewMode mode)
{
    if(mode==display2d)
    {
        d->view2dToolBox->show();
        d->view3dToolBox->hide();
    }
    else if(mode==display3d)
    {
        d->view2dToolBox->hide();
        d->view3dToolBox->show();
    }
}





//////////////
//
//  medView2dParamsToolBox
//
//////////////

class medView2dParamsToolBoxPrivate
{
public:
    medVtkView  * vtkView;
    QMap<medVtkView*, QWidget*> widgetList;
    QMultiMap<medVtkView*, medAbstractParameter*> viewParamMap;
};

medView2dParamsToolBox::medView2dParamsToolBox(QWidget *parent)
    : medToolBox(parent), d(new medView2dParamsToolBoxPrivate)
{
    d->vtkView = 0;
}


bool medView2dParamsToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medView2dParamsToolBox>("medView2dParamsToolBox","medView2dParamsToolBox",
                                                                                "View management toolbox", QStringList());
}

QList<medAbstractParameter*> medView2dParamsToolBox::getParameters(medVtkView* view)
{
    return d->viewParamMap.values(view);
}

void medView2dParamsToolBox::update(dtkAbstractView * view)
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
    windowingParam->hideLabel();

    medActionParameter *zoomingParam = new medActionParameter("Zooming", view);
    zoomingParam->setIcon(QIcon (":/icons/magnify.png"));
    zoomingParam->setCheckable(true);
    zoomingParam->hideLabel();

    medActionParameter *slicingParam = new medActionParameter("Slicing", view);
    slicingParam->setIcon(QIcon (":/icons/stack.png"));
    slicingParam->setCheckable(true);
    slicingParam->hideLabel();

    medActionParameter *measuringParam = new medActionParameter("Measuring", view);
    measuringParam->setIcon(QIcon (":/icons/length.png"));
    measuringParam->setCheckable(true);
    measuringParam->hideLabel();


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


    d->widgetList.insert(d->vtkView, toolboxWidget);

    // Connection
    connect(windowingParam, SIGNAL(triggered(dtkAbstractView*, bool)),this, SLOT(setWindowing(dtkAbstractView*,bool)));
    connect(zoomingParam, SIGNAL(triggered(dtkAbstractView*,bool)),this, SLOT(setZooming(dtkAbstractView*,bool)));
    connect(slicingParam, SIGNAL(triggered(dtkAbstractView*,bool)),this, SLOT(setSlicing(dtkAbstractView*,bool)));
    connect(measuringParam, SIGNAL(triggered(dtkAbstractView*,bool)),this, SLOT(setMeasuring(dtkAbstractView*,bool)));
    connect(scalarBarParam, SIGNAL(valueChanged(dtkAbstractView*,bool)), this, SLOT(setScalarBarVisibility(dtkAbstractView*,bool)));
    connect(axisParam, SIGNAL(valueChanged(dtkAbstractView*,bool)), this, SLOT(setAxisVisibility(dtkAbstractView*,bool)));
    connect(rulerParam, SIGNAL(valueChanged(dtkAbstractView*,bool)), this, SLOT(setRulerVisibility(dtkAbstractView*,bool)));
    connect(annotationsParam, SIGNAL(valueChanged(dtkAbstractView*,bool)), this, SLOT(setAnnotationsVisibility(dtkAbstractView*,bool)));


    d->viewParamMap.insert(d->vtkView, windowingParam);
    d->viewParamMap.insert(d->vtkView, zoomingParam);
    d->viewParamMap.insert(d->vtkView, slicingParam);
    d->viewParamMap.insert(d->vtkView, measuringParam);
    d->viewParamMap.insert(d->vtkView, scalarBarParam);
    d->viewParamMap.insert(d->vtkView, axisParam);
    d->viewParamMap.insert(d->vtkView, rulerParam);
    d->viewParamMap.insert(d->vtkView, annotationsParam);

}


void medView2dParamsToolBox::setWindowing(dtkAbstractView* view, bool checked)
{
    if (checked && view) {
        view->setProperty("MouseInteraction", "Windowing");
        view->update();
    }
}

void medView2dParamsToolBox::setZooming(dtkAbstractView* view, bool checked)
{
    if (checked && view) {
        view->setProperty("MouseInteraction", "Zooming");
        view->update();
    }
}

void medView2dParamsToolBox::setSlicing(dtkAbstractView* view, bool checked)
{
    if (checked && view) {
        view->setProperty("MouseInteraction", "Slicing");
        view->update();
    }
}

void medView2dParamsToolBox::setMeasuring(dtkAbstractView* view, bool checked)
{
    if (checked && view) {
        view->setProperty("MouseInteraction", "Measuring");
        view->update();
    }
}


void medView2dParamsToolBox::setAxisVisibility(dtkAbstractView* view, bool visible)
{
    if (view) {
        view->setProperty("ShowAxis", (visible ? "true" : "false"));
        view->update();
    }
}

void medView2dParamsToolBox::setScalarBarVisibility(dtkAbstractView* view, bool visible)
{
    if (view) {
        view->setProperty("ShowScalarBar", (visible ? "true" : "false"));
        view->update();
    }
}

void medView2dParamsToolBox::setRulerVisibility(dtkAbstractView* view, bool visible)
{
    if (view) {
        view->setProperty("ShowRuler", (visible ? "true" : "false"));
        view->update();
    }
}

void medView2dParamsToolBox::setAnnotationsVisibility(dtkAbstractView* view, bool visible)
{
    if (view) {
        view->setProperty("ShowAnnotations", (visible ? "true" : "false"));
        view->update();
    }
}


