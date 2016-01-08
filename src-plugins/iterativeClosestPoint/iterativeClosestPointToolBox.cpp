/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "iterativeClosestPointProcess.h"
#include "iterativeClosestPointToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractLayeredView.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>

#include <medAbstractImageData.h>
#include <medAbstractData.h>
#include <medAbstractWorkspace.h>

#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medProgressionStack.h>
#include <medMetaDataKeys.h>
#include <medDataManager.h>

#include <vtkMetaDataSet.h>
#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>
#include <medToolBox.h>
#include <medToolBoxBody.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medUtilities.h>

class iterativeClosestPointToolBoxPrivate
{
public:
    
    medAbstractLayeredView * currentView;
    medComboBox * layerSource, * layerTarget;
    QDoubleSpinBox * ScaleFactor,* MaxMeanDistance;
    QSpinBox * MaxNumIterations, * MaxNumLandmarks;
    QCheckBox * bStartByMatchingCentroids,*bRididBody,*bCheckMeanDistance;
};

iterativeClosestPointToolBox::iterativeClosestPointToolBox(QWidget *parent) : medRegistrationAbstractToolBox(parent), d(new iterativeClosestPointToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    medToolBoxTab * layoutToolBoxTab = new medToolBoxTab(this);
    layoutToolBoxTab->addTab(widget, tr("ICP"));

    QPushButton *runButton = new QPushButton(tr("Run"), this);
    
    this->setTitle("Mesh Registration");
      
    // Parameters'widgets
    d->layerSource = new medComboBox;
    d->layerSource->addItem("Select the layer", 0);
    QLabel * layerSource_Label = new QLabel("Layer number for source mesh");
    QHBoxLayout * layerSource_layout = new QHBoxLayout;
    layerSource_layout->addWidget(layerSource_Label);
    layerSource_layout->addWidget(d->layerSource);
    
    d->layerTarget = new medComboBox;
    d->layerTarget->addItem("Select the layer", 0);
    QLabel * layerTarget_Label = new QLabel("Layer number for target mesh");
    QHBoxLayout * layerTarget_layout = new QHBoxLayout;
    layerTarget_layout->addWidget(layerTarget_Label);
    layerTarget_layout->addWidget(d->layerTarget);
    
    d->bStartByMatchingCentroids = new QCheckBox(widget);
    d->bStartByMatchingCentroids->setText("StartByMatchingCentroids");
    d->bStartByMatchingCentroids->setChecked(true);
    
    d->bRididBody = new QCheckBox(widget);
    d->bRididBody->setText("RigidBody");
    d->bRididBody->setChecked(true);
    
    d->bCheckMeanDistance = new QCheckBox(widget);
    d->bCheckMeanDistance->setText("CheckMeanDistance");

    d->ScaleFactor = new QDoubleSpinBox(widget);
    d->ScaleFactor->setValue(1.0);
    QHBoxLayout * ScaleFactor_layout = new QHBoxLayout;
    QLabel * ScaleFactor_Label = new QLabel("Scale Factor");
    ScaleFactor_layout->addWidget(ScaleFactor_Label);
    ScaleFactor_layout->addWidget(d->ScaleFactor);
    
    d->MaxMeanDistance = new QDoubleSpinBox(widget);
    d->MaxMeanDistance->setDecimals(5);
    d->MaxMeanDistance->setValue(0.01);
    QHBoxLayout * MaxMeanDistance_layout = new QHBoxLayout;
    QLabel * MaxMeanDistance_Label = new QLabel("Max Mean Distance");
    MaxMeanDistance_layout->addWidget(MaxMeanDistance_Label);
    MaxMeanDistance_layout->addWidget(d->MaxMeanDistance);
    
    d->MaxNumIterations = new QSpinBox(widget);
    d->MaxNumIterations->setMaximum(1000000);
    d->MaxNumIterations->setValue(50);
    QHBoxLayout * MaxNumIterations_layout = new QHBoxLayout;
    QLabel * MaxNumIterations_Label = new QLabel("Max Num Iterations");
    MaxNumIterations_layout->addWidget(MaxNumIterations_Label);
    MaxNumIterations_layout->addWidget(d->MaxNumIterations);
    
    d->MaxNumLandmarks = new QSpinBox(widget);
    d->MaxNumLandmarks->setMaximum(1000000);
    d->MaxNumLandmarks->setValue(200);
    QHBoxLayout * MaxNumLandmarks_layout = new QHBoxLayout;
    QLabel * MaxNumLandmarks_Label = new QLabel("Max Num Landmarks");
    MaxNumLandmarks_layout->addWidget(MaxNumLandmarks_Label);
    MaxNumLandmarks_layout->addWidget(d->MaxNumLandmarks);

    QVBoxLayout * parameters_layout = new QVBoxLayout;
    widget->setLayout(parameters_layout);
    parameters_layout->addLayout(layerSource_layout);
    parameters_layout->addLayout(layerTarget_layout);
    parameters_layout->addWidget(d->bStartByMatchingCentroids);
    parameters_layout->addWidget(d->bRididBody);
    parameters_layout->addWidget(d->bCheckMeanDistance);
    parameters_layout->addLayout(ScaleFactor_layout);
    parameters_layout->addLayout(MaxMeanDistance_layout);
    parameters_layout->addLayout(MaxNumIterations_layout);
    parameters_layout->addLayout(MaxNumLandmarks_layout);
    parameters_layout->addWidget(runButton);

    this->setTabWidget(layoutToolBoxTab);
    
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

iterativeClosestPointToolBox::~iterativeClosestPointToolBox()
{
    delete d;
    
    d = NULL;
}

bool iterativeClosestPointToolBox::registered()
{
    return medToolBoxFactory::instance()->
    registerToolBox<iterativeClosestPointToolBox>();
}

void iterativeClosestPointToolBox::run()
{
    if (!d->currentView)
    {
        return;
    }

    dtkSmartPointer <dtkAbstractProcess> process = dtkAbstractProcessFactory::instance()->createSmartPointer("iterativeClosestPointProcess");

    medAbstractData *sourceData = d->currentView->layerData(d->layerSource->currentIndex()-1);
    medAbstractData *targetData = d->currentView->layerData(d->layerTarget->currentIndex()-1);
    
    if (!targetData || !sourceData)
    {
        return;
    }

    iterativeClosestPointProcess *process_Registration = dynamic_cast<iterativeClosestPointProcess *>(process.data());
    if (!process_Registration)
    {
        qWarning() << "registration process doesn't exist" ;
        return;
    }
    
    process_Registration->setInput(sourceData,  0);
    process_Registration->setInput(targetData, 1);
          
    process_Registration->setParameter((double)d->bStartByMatchingCentroids->isChecked(),0);
    process_Registration->setParameter((double)d->bRididBody->isChecked(),1);
    process_Registration->setParameter((double)d->bCheckMeanDistance->isChecked(),2);
    process_Registration->setParameter(d->MaxMeanDistance->value(),3);
    process_Registration->setParameter((double)d->MaxNumIterations->value(),4);
    process_Registration->setParameter((double)d->MaxNumLandmarks->value(),5);
    process_Registration->setParameter(d->ScaleFactor->value(),6);
    
    process_Registration->update();
    
    medAbstractData* data = process_Registration->output();
    medUtilities::setDerivedMetaData(data, sourceData, "ICP");
    
    d->currentView->insertLayer(d->layerSource->currentIndex() - 1, data);
    d->currentView->removeData(sourceData);

    medDataManager::instance()->importData(data, false);
}

void iterativeClosestPointToolBox::updateView()
{
    resetComboBoxes();

    medTabbedViewContainers * containers = this->getWorkspace()->stackedViewContainers();
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    medAbstractView *view=NULL;
    for(int i=0;i<containersInTabSelected.length();i++)
    {
        if (containersInTabSelected[i]->isSelected())
        {
            view = containersInTabSelected[i]->view();
            break;
        }
    }

    if (view)
    {
        d->currentView = qobject_cast<medAbstractLayeredView*>(view);
        QList<medDataIndex> layerList = d->currentView->dataList();

        for (int i=0; i<layerList.size(); i++)
        {
            addLayer(i);
        }
        QObject::connect(view, SIGNAL(closed()),
            this, SLOT(resetComboBoxes()),
            Qt::UniqueConnection);
    }
}

void iterativeClosestPointToolBox::setWorkspace(medAbstractWorkspace* workspace)
{
    medToolBox::setWorkspace(workspace);
    medTabbedViewContainers * containers = workspace->stackedViewContainers();

    QObject::connect(containers,SIGNAL(containersSelectedChanged()),this,SLOT(updateView()));
    updateView();
}

void iterativeClosestPointToolBox::resetComboBoxes()
{
    d->layerSource->clear();
    d->layerTarget->clear();
    d->layerSource->addItem("Select a layer");
    d->layerTarget->addItem("Select a layer");
}

void iterativeClosestPointToolBox::addLayer(unsigned int layer)
{
    medAbstractData *data = d->currentView->layerData(layer);

    QString name = medMetaDataKeys::SeriesDescription.getFirstValue(data,"<i>no name</i>");

    if(data && data->identifier().contains("vtkDataMesh"))
    {
        d->layerSource->addItem(name);
        d->layerTarget->addItem(name);
    }
}
