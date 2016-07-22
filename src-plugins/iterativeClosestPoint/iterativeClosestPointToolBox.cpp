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

#include <medAbstractLayeredView.h>
#include <medDataManager.h>
#include <medJobManager.h>
#include <medMetaDataKeys.h>
#include <medPluginManager.h>
#include <medProgressionStack.h>
#include <medRunnableProcess.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medViewContainer.h>

class iterativeClosestPointToolBoxPrivate
{
public:
    
    medAbstractLayeredView * currentView;
    medComboBox * layerSource, * layerTarget;
    QDoubleSpinBox * ScaleFactor,* MaxMeanDistance;
    QSpinBox * MaxNumIterations, * MaxNumLandmarks;
    QCheckBox * bStartByMatchingCentroids,*bCheckMeanDistance;
    medComboBox* bTransformationComboBox;

    dtkSmartPointer<iterativeClosestPointProcess> process;
    medProgressionStack * progression_stack;
};

iterativeClosestPointToolBox::iterativeClosestPointToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new iterativeClosestPointToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QVBoxLayout * parameters_layout = new QVBoxLayout;

    // Parameters'widgets
    d->layerSource = new medComboBox;
    d->layerSource->addItem("Select the layer", 0);
    QLabel * layerSource_Label = new QLabel("Select the source mesh:");
    parameters_layout->addWidget(layerSource_Label);
    parameters_layout->addWidget(d->layerSource);
    
    d->layerTarget = new medComboBox;
    d->layerTarget->addItem("Select the layer", 0);
    QLabel * layerTarget_Label = new QLabel("Select the target mesh:");
    parameters_layout->addWidget(layerTarget_Label);
    parameters_layout->addWidget(d->layerTarget);
    
    d->bStartByMatchingCentroids = new QCheckBox(widget);
    d->bStartByMatchingCentroids->setText("StartByMatchingCentroids");
    d->bStartByMatchingCentroids->setChecked(true);
    
    // Choice between transformations
    QHBoxLayout * transformation_layout = new QHBoxLayout;

    QLabel * transformation_Label = new QLabel("Transformation");
    transformation_layout->addWidget(transformation_Label);

    d->bTransformationComboBox = new medComboBox(widget);
    d->bTransformationComboBox->addItem("Rigid body");
    d->bTransformationComboBox->addItem("Similarity");
    d->bTransformationComboBox->addItem("Affine");
    d->bTransformationComboBox->setCurrentIndex(0);
    transformation_layout->addWidget(d->bTransformationComboBox);

    // Mean Distance
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

    // Run button
    QPushButton *runButton = new QPushButton(tr("Run"), widget);
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

    // progression stack
    d->progression_stack = new medProgressionStack(widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);

    parameters_layout->addWidget(d->bStartByMatchingCentroids);
    parameters_layout->addLayout(transformation_layout);
    parameters_layout->addWidget(d->bCheckMeanDistance);
    parameters_layout->addLayout(ScaleFactor_layout);
    parameters_layout->addLayout(MaxMeanDistance_layout);
    parameters_layout->addLayout(MaxNumIterations_layout);
    parameters_layout->addLayout(MaxNumLandmarks_layout);
    parameters_layout->addWidget(runButton);
    parameters_layout->addLayout(progressStackLayout);

    widget->setLayout(parameters_layout);
    this->addWidget(widget);

    d->currentView = 0;
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
    if (d->currentView)
    {
        medAbstractData *sourceData = d->currentView->layerData(d->layerSource->currentIndex()-1);
        medAbstractData *targetData = d->currentView->layerData(d->layerTarget->currentIndex()-1);
    
        if (targetData && sourceData)
        {
            d->process = new iterativeClosestPointProcess();
            d->process->setInput(sourceData,  0);
            d->process->setInput(targetData, 1);

            d->process->setParameter((double)d->bStartByMatchingCentroids->isChecked(),0);
            d->process->setParameter((double)d->bTransformationComboBox->currentIndex(),1);
            d->process->setParameter((double)d->bCheckMeanDistance->isChecked(),2);
            d->process->setParameter(d->MaxMeanDistance->value(),3);
            d->process->setParameter((double)d->MaxNumIterations->value(),4);
            d->process->setParameter((double)d->MaxNumLandmarks->value(),5);
            d->process->setParameter(d->ScaleFactor->value(),6);

            // Progression stack
            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess (d->process);
            d->progression_stack->addJobItem(runProcess, "Progress:");
            d->progression_stack->disableCancel(runProcess);

            connect (runProcess, SIGNAL (success   (QObject*)),    this, SIGNAL (success ()));
            connect (runProcess, SIGNAL (success   (QObject*)),    this, SLOT   (displayOutput()));
            connect (runProcess, SIGNAL (failure   (QObject*)),    this, SIGNAL (failure ()));
            connect (runProcess, SIGNAL (failure   (int)),         this, SLOT   (handleDisplayError(int)));
            connect (runProcess, SIGNAL (cancelled (QObject*)),    this, SIGNAL (failure ()));
            connect (runProcess, SIGNAL (activate(QObject*,bool)), d->progression_stack, SLOT(setActive(QObject*,bool)));

            medJobManager::instance()->registerJobItem(runProcess);
            QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
        }
        else
        {
            medMessageController::instance()->showError(tr("Choose two meshes"),3000);
        }
    }
    else
    {
        medMessageController::instance()->showError(tr("Drop two meshes in the view"),3000);
    }
}

void iterativeClosestPointToolBox::displayOutput()
{
    medAbstractData *sourceData = d->currentView->layerData(d->layerSource->currentIndex()-1);
    d->currentView->insertLayer(d->layerSource->currentIndex() - 1, d->process->output());
    d->currentView->removeData(sourceData);
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
    else
    {
        d->currentView = NULL;
    }
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

medAbstractData* iterativeClosestPointToolBox::processOutput()
{
    if(!d->process)
    {
        return NULL;
    }
    return d->process->output();
}

dtkPlugin* iterativeClosestPointToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "iterativeClosestPointPlugin" );
    return plugin;
}
