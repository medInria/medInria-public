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
    mode3dParam->setDefaultValue("MPR");

    medListParameter *VRModeParam = new medListParameter("VR Mode", view);
    QStringList VRModes;
    VRModes << "GPU" <<  "Ray Cast / Texture" << "Ray Cast" << "Texture" <<  "Default";
    VRModeParam->setValues(VRModes);
#ifdef __APPLE__
    VRModeParam->setDefaultValue( "Ray Cast" );
#else
    VRModeParam->setDefaultValue( "Default" );
#endif

    medActionParameter *croppingParam = new medActionParameter("Cropping", view);
    croppingParam->setIcon(QIcon (":/icons/cropping.png"));
    croppingParam->setCheckable(true);

    medBooleanParameter *depthPeelingParam = new medBooleanParameter("Depth Peeling", view);

    // Layouts
    QWidget *toolboxWidget = new QWidget(this);

    QFormLayout *toolBoxLayout = new QFormLayout(toolboxWidget);
    toolBoxLayout->setLabelAlignment(Qt::AlignLeft);
    toolBoxLayout->addRow(mode3dParam->name(), mode3dParam->getWidget());
    toolBoxLayout->addRow(VRModeParam->name(), VRModeParam->getWidget());
    toolBoxLayout->addRow(croppingParam->name(), croppingParam->getWidget());
    toolBoxLayout->addRow(depthPeelingParam->name(), depthPeelingParam->getWidget());

    this->addWidget(toolboxWidget);

    d->widgetList.insert(d->vtkView, toolboxWidget);

    // Connection
    connect(mode3dParam,SIGNAL(valueChanged(QString)),this,SLOT(set3DMode(QString)));
    connect(VRModeParam,SIGNAL(valueChanged(QString)),this,SLOT(setRenderer(QString)));
    connect(croppingParam,SIGNAL(triggered(bool)),this,SLOT(setCropping(bool)));
    connect(depthPeelingParam, SIGNAL(valueChanged(bool)),this, SLOT(setDepthPeeling(bool)));

    d->viewParamMap.insert(d->vtkView, mode3dParam);
    d->viewParamMap.insert(d->vtkView, VRModeParam);
    d->viewParamMap.insert(d->vtkView, croppingParam);
    d->viewParamMap.insert(d->vtkView, depthPeelingParam);

}


void medView3dParamsToolBox::set3DMode( QString mode )
{
    if (d->vtkView)
    {
        d->vtkView->blockSignals (true);
        if ( qApp->arguments().contains ( "--stereo" ) )
              d->vtkView->view3d()->GetRenderWindow()->SetStereoRender ( 1 );
        d->vtkView->set3DMode( mode );
        d->vtkView->blockSignals (false);
        d->vtkView->update();
    }
}

void medView3dParamsToolBox::setRenderer( QString mode )
{
    if (d->vtkView)
    {
        d->vtkView->blockSignals (true);
        d->vtkView->setRenderer( mode );
        d->vtkView->blockSignals (false);
        d->vtkView->update();
    }
}

void medView3dParamsToolBox::setCropping( bool checked )
{
    if (d->vtkView)
    {
        d->vtkView->blockSignals (true);
        d->vtkView->setCropping( checked );
        d->vtkView->blockSignals (false);
        d->vtkView->update();
    }
}

void medView3dParamsToolBox::setDepthPeeling( bool checked )
{
    if (d->vtkView)
    {
        d->vtkView->blockSignals (true);
        d->vtkView->setDepthPeeling( checked );
        d->vtkView->blockSignals (false);
        d->vtkView->update();
    }
}
