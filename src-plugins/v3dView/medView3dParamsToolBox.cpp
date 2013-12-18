/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <medView3dParamsToolBox.h>
#include <medToolBoxFactory.h>
#include <medVtkView.h>
#include <v3dView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

#include <vtkRenderWindow.h>

class medView3dParamsToolBoxPrivate
{
public:

    medVtkView  * vtkView;
    QMap<medVtkView*, QWidget*> widgetList;
    QMultiMap<medVtkView*, medAbstractParameter*> viewParamMap;

};


medView3dParamsToolBox::medView3dParamsToolBox(QWidget *parent)
    : medToolBox(parent), d(new medView3dParamsToolBoxPrivate)
{
    d->vtkView = 0;
}


bool medView3dParamsToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medView3dParamsToolBox>("medView3dParamsToolBox","medView3dParamsToolBox",
                                                                                "View management toolbox", QStringList());
}


QList<medAbstractParameter*> medView3dParamsToolBox::getParameters(medVtkView* view)
{
    return d->viewParamMap.values(view);
}

void medView3dParamsToolBox::update(dtkAbstractView * view)
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

    medListParameter *mode3dParam = new medListParameter("3dMode", view);
    QStringList modes;
    modes << "VR" << "MIP - Maximum" << "MIP - Minimum" << "MPR" << "Off";
    mode3dParam->setValues(modes);

    medListParameter *VRModeParam = new medListParameter("VR Mode", view);
    QStringList VRModes;
    VRModes << "GPU" <<  "Ray Cast / Texture" << "Ray Cast" << "Texture" <<  "Default";
    VRModeParam->setValues(VRModes);

    medIntParameter *LODParam = new medIntParameter("LOD", view);
    LODParam->setMinimum(0);
    LODParam->setMaximum(100);
    LODParam->setValue(100);

    medActionParameter *croppingParam = new medActionParameter("Cropping", view);
    croppingParam->setIcon(QIcon (":/icons/cropping.png"));
    croppingParam->setCheckable(true);

    medBooleanParameter *depthPeelingParam = new medBooleanParameter("Depth Peeling", view);

    // Layouts
    QWidget *toolboxWidget = new QWidget(this);

    QVBoxLayout *toolBoxLayout = new QVBoxLayout(toolboxWidget);
    toolBoxLayout->addWidget(mode3dParam->getWidget());
    toolBoxLayout->addWidget(VRModeParam->getWidget());
    toolBoxLayout->addWidget(LODParam->getWidget());
    toolBoxLayout->addWidget(croppingParam->getWidget()/*, 0, Qt::AlignLeft*/);
    toolBoxLayout->addWidget(depthPeelingParam->getWidget());

    this->addWidget(toolboxWidget);

    d->widgetList.insert(d->vtkView, toolboxWidget);


    // Connection
    connect(mode3dParam,SIGNAL(valueChanged(dtkAbstractView*,QString)),this,SLOT(onModeChanged(dtkAbstractView*,QString)));
    connect(VRModeParam,SIGNAL(valueChanged(dtkAbstractView*,QString)),this,SLOT(onVRModeChanged(dtkAbstractView*,QString)));
    connect(LODParam,SIGNAL(valueChanged(dtkAbstractView*,int)),this,SLOT(onLodChanged(dtkAbstractView*,int)));
    connect(croppingParam,SIGNAL(triggered(dtkAbstractView*,bool)),this,SLOT(onCroppingChanged(dtkAbstractView*,bool)));
    connect(depthPeelingParam, SIGNAL(valueChanged(dtkAbstractView*, bool)),this, SLOT(onDepthPeelingChanged(dtkAbstractView*,bool)));

    d->viewParamMap.insert(d->vtkView, mode3dParam);
    d->viewParamMap.insert(d->vtkView, VRModeParam);
    d->viewParamMap.insert(d->vtkView, LODParam);
    d->viewParamMap.insert(d->vtkView, croppingParam);
    d->viewParamMap.insert(d->vtkView, depthPeelingParam);

}




void medView3dParamsToolBox::onModeChanged(dtkAbstractView* view, QString mode)
{
    if (v3dView *v3dview = dynamic_cast<v3dView*>(view) )
    {
        v3dview->blockSignals (true);
        if ( qApp->arguments().contains ( "--stereo" ) )
              v3dview->view3d()->GetRenderWindow()->SetStereoRender ( 1 );
        v3dview->setProperty("3DMode", mode);
        v3dview->blockSignals (false);
        v3dview->update();
    }
}

void medView3dParamsToolBox::onVRModeChanged(dtkAbstractView* view, QString mode)
{
    if (view) {
        view->blockSignals (true);
        view->setProperty("Renderer", mode);
        view->blockSignals (false);
        view->update();
    }
}

void medView3dParamsToolBox::onLodChanged(dtkAbstractView* view, int value)
{
    if (view) {
        view->blockSignals (true);
        view->setMetaData("LOD", QString::number(value));
        view->blockSignals (false);
        view->update();
    }
}

void medView3dParamsToolBox::onCroppingChanged(dtkAbstractView* view, bool checked)
{
    if (view) {
        view->blockSignals (true);
        view->setProperty("Cropping", (checked ? "true" : "false"));
        view->blockSignals (false);
        view->update();
    }
}

void medView3dParamsToolBox::onDepthPeelingChanged(dtkAbstractView* view, bool checked)
{
    if (view) {
        view->blockSignals (true);
        view->setProperty("DepthPeeling", (checked ? "true" : "false"));
        view->blockSignals (false);
        view->update();
    }
}
