/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <iterativeClosestPointProcess.h>
#include <iterativeClosestPointToolBox.h>

#include <medAbstractLayeredView.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>

class iterativeClosestPointToolBoxPrivate
{
public:
    
    medAbstractLayeredView *currentView;
    QComboBox *layerSource, *layerTarget;
    QDoubleSpinBox *ScaleFactor, *MaxMeanDistance;
    QSpinBox *MaxNumIterations, *MaxNumLandmarks;
    QCheckBox *bStartByMatchingCentroids, *bCheckMeanDistance;
    QComboBox *bTransformationComboBox;

    dtkSmartPointer<iterativeClosestPointProcess> process;
};

iterativeClosestPointToolBox::iterativeClosestPointToolBox(QWidget *parent)
    : medAbstractSelectableToolBox(parent), d(new iterativeClosestPointToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QVBoxLayout *parameters_layout = new QVBoxLayout;

    // Parameters widgets
    d->layerSource = new QComboBox;
    d->layerSource->addItem("Select the layer", 0);
    QLabel *layerSource_Label = new QLabel("Select the source mesh:");
    parameters_layout->addWidget(layerSource_Label);
    parameters_layout->addWidget(d->layerSource);
    
    d->layerTarget = new QComboBox;
    d->layerTarget->addItem("Select the layer", 0);
    QLabel *layerTarget_Label = new QLabel("Select the target mesh:");
    parameters_layout->addWidget(layerTarget_Label);
    parameters_layout->addWidget(d->layerTarget);
    
    d->bStartByMatchingCentroids = new QCheckBox(widget);
    d->bStartByMatchingCentroids->setText("Start by matching centroids");
    d->bStartByMatchingCentroids->setToolTip("Start the process by translating source centroid to target centroid");
    d->bStartByMatchingCentroids->setChecked(true);
    
    // Choice between transformations
    QHBoxLayout *transformation_layout = new QHBoxLayout;

    QLabel *transformation_Label = new QLabel("Transformation");
    transformation_layout->addWidget(transformation_Label);

    d->bTransformationComboBox = new QComboBox(widget);
    d->bTransformationComboBox->addItem("Rigid body");
    d->bTransformationComboBox->addItem("Similarity");
    d->bTransformationComboBox->addItem("Affine");
    d->bTransformationComboBox->setCurrentIndex(0);
    transformation_layout->addWidget(d->bTransformationComboBox);

    // Mean Distance
    d->bCheckMeanDistance = new QCheckBox(widget);
    d->bCheckMeanDistance->setText("Check mean distance");
    d->bCheckMeanDistance->setToolTip("Force the algorithm to check the mean distance between two iterations");

    d->ScaleFactor = new QDoubleSpinBox(widget);
    d->ScaleFactor->setValue(1.0);
    d->ScaleFactor->setToolTip("Set a linear transformation according to this factor");
    QHBoxLayout *ScaleFactor_layout = new QHBoxLayout;
    QLabel *ScaleFactor_Label = new QLabel("Scale Factor");
    ScaleFactor_layout->addWidget(ScaleFactor_Label);
    ScaleFactor_layout->addWidget(d->ScaleFactor);
    
    d->MaxMeanDistance = new QDoubleSpinBox(widget);
    d->MaxMeanDistance->setDecimals(5);
    d->MaxMeanDistance->setValue(0.01);
    d->MaxMeanDistance->setToolTip("Set the maximum mean distance between two iterations");
    QHBoxLayout *MaxMeanDistance_layout = new QHBoxLayout;
    QLabel *MaxMeanDistance_Label = new QLabel("Max Mean Distance");
    MaxMeanDistance_layout->addWidget(MaxMeanDistance_Label);
    MaxMeanDistance_layout->addWidget(d->MaxMeanDistance);
    
    d->MaxNumIterations = new QSpinBox(widget);
    d->MaxNumIterations->setMaximum(1000000);
    d->MaxNumIterations->setValue(50);
    d->MaxNumIterations->setToolTip("Set the maximum number of iterations");
    QHBoxLayout *MaxNumIterations_layout = new QHBoxLayout;
    QLabel *MaxNumIterations_Label = new QLabel("Max Num Iterations");
    MaxNumIterations_layout->addWidget(MaxNumIterations_Label);
    MaxNumIterations_layout->addWidget(d->MaxNumIterations);
    
    d->MaxNumLandmarks = new QSpinBox(widget);
    d->MaxNumLandmarks->setMaximum(1000000);
    d->MaxNumLandmarks->setValue(200);
    d->MaxNumLandmarks->setToolTip("Set the maximum number of landmarks");
    QHBoxLayout *MaxNumLandmarks_layout = new QHBoxLayout;
    QLabel *MaxNumLandmarks_Label = new QLabel("Max Num Landmarks");
    MaxNumLandmarks_layout->addWidget(MaxNumLandmarks_Label);
    MaxNumLandmarks_layout->addWidget(d->MaxNumLandmarks);

    // Run button
    QPushButton *runButton = new QPushButton(tr("Run"), widget);
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

    parameters_layout->addWidget(d->bStartByMatchingCentroids);
    parameters_layout->addLayout(transformation_layout);
    parameters_layout->addWidget(d->bCheckMeanDistance);
    parameters_layout->addLayout(ScaleFactor_layout);
    parameters_layout->addLayout(MaxMeanDistance_layout);
    parameters_layout->addLayout(MaxNumIterations_layout);
    parameters_layout->addLayout(MaxNumLandmarks_layout);
    parameters_layout->addWidget(runButton);

    widget->setLayout(parameters_layout);
    this->addWidget(widget);

    d->currentView = nullptr;
}

iterativeClosestPointToolBox::~iterativeClosestPointToolBox()
{
    delete d;
    d = nullptr;
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
            this->setToolBoxOnWaitStatus();

            d->process = new iterativeClosestPointProcess();
            d->process->setInput(sourceData,  0);
            d->process->setInput(targetData, 1);

            d->process->setParameter(d->bStartByMatchingCentroids->isChecked(),0);
            d->process->setParameter(d->bTransformationComboBox->currentIndex(),1);
            d->process->setParameter(d->bCheckMeanDistance->isChecked(),2);
            d->process->setParameter(d->MaxMeanDistance->value(),3);
            d->process->setParameter(d->MaxNumIterations->value(),4);
            d->process->setParameter(d->MaxNumLandmarks->value(),5);
            d->process->setParameter(d->ScaleFactor->value(),6);

            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess (d->process);
            connect (runProcess, SIGNAL(success(QObject*)), this, SLOT(displayOutput()));
            this->addConnectionsAndStartJob(runProcess);
        }
        else
        {
            medMessageController::instance()->showError(tr("Choose two meshes"), 3000);
        }
    }
    else
    {
        medMessageController::instance()->showError(tr("Drop two meshes in the view"), 3000);
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

    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if (view)
    {
        d->currentView = qobject_cast<medAbstractLayeredView*>(view);
        QList<medDataIndex> layerList = d->currentView->dataList();

        for (int i=0; i<layerList.size(); i++)
        {
            addLayer(i);
        }
        QObject::connect(view, SIGNAL(closed()), this, SLOT(resetComboBoxes()), Qt::UniqueConnection);
    }
    else
    {
        d->currentView = nullptr;
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

    if(data && (data->identifier().contains("vtkDataMesh") ||
                data->identifier().contains("EPMap")))
    {
        d->layerSource->addItem(name);
        d->layerTarget->addItem(name);
    }
}

medAbstractData* iterativeClosestPointToolBox::processOutput()
{
    if(!d->process)
    {
        return nullptr;
    }
    return d->process->output();
}

dtkPlugin* iterativeClosestPointToolBox::plugin()
{
    medPluginManager *pm = medPluginManager::instance();
    dtkPlugin *plugin = pm->plugin ( "Iterative Closest Point" );
    return plugin;
}
