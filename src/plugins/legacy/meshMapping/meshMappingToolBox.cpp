/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medAbstractLayeredView.h>
#include <medDataIndex.h>
#include <medPluginManager.h>
#include <medMetaDataKeys.h>
#include <medRunnableProcess.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>

#include <meshMapping.h>
#include <meshMappingToolBox.h>

class meshMappingToolBoxPrivate
{
public:
    dtkSmartPointer <meshMapping> process;

    dtkAbstractView *view;
    dtkSmartPointer<medAbstractData> data;
    dtkSmartPointer<medAbstractData> structure;

    QComboBox *layersForStructure;
    QComboBox *layersForData;

    // Need to re-implement the counting of layers because pb with meshes in the current counting
    int nbOfImageLayers, nbOfMeshLayers;
};

meshMappingToolBox::meshMappingToolBox(QWidget *parent)
    :medAbstractSelectableToolBox(parent),
    d(new meshMappingToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QLabel *dataLabel = new QLabel("Select the data to map ", this);
    dataLabel->setToolTip(tr("Select the dataset from which to obtain\n probe values (image or mesh)."));
    d->layersForData = new QComboBox;
    d->layersForData->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    d->layersForData->addItem("Select the layer", 0);

    QLabel *structureLabel = new QLabel("Select the structure", this);
    structureLabel->setToolTip(tr("Select the dataset whose geometry will be used\n \
                                  in determining positions to probe (typically a mesh)."));
    d->layersForStructure = new QComboBox;
    d->layersForStructure->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    d->layersForStructure->addItem("Select the layer", 0);
    
    QPushButton *runButton = new QPushButton(tr("Run"), this);
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(dataLabel);
    layout->addWidget(d->layersForData);
    layout->addWidget(structureLabel);
    layout->addWidget(d->layersForStructure);
    layout->addWidget(runButton);

    widget->setLayout(layout);
    this->addWidget(widget);

    d->nbOfMeshLayers = 0;
    d->nbOfImageLayers = 0;
}

meshMappingToolBox::~meshMappingToolBox()
{
    delete d;
    d = nullptr;
}

bool meshMappingToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<meshMappingToolBox>();
}

dtkPlugin* meshMappingToolBox::plugin()
{
    return medPluginManager::instance().plugin("Mesh Mapping");
}

medAbstractData* meshMappingToolBox::processOutput()
{
    if(!d->process)
    {
        return nullptr;
    }

    return d->process->output();
}

void meshMappingToolBox::run()
{ 
    if (!d->process)
    {
        d->process =  new meshMapping();
    }

    medAbstractLayeredView *medView = static_cast<medAbstractLayeredView *> (d->view);
    if (medView)
    {
        int structureLayer = d->layersForStructure->currentIndex() - 1;

        if ((structureLayer >= 0) && (medView->layersCount() >= 0))
        {
            this->setToolBoxOnWaitStatus();

            d->process->setInput(medView->layerData(structureLayer), 0);

            int dataLayer = d->layersForData->currentIndex() -1;
            d->process->setInput(medView->layerData(dataLayer), 1);

            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess (d->process);
            connect (runProcess, SIGNAL (failure  (int)),  this, SLOT (handleDisplayError(int)));
            this->addConnectionsAndStartJob(runProcess);
        }
    }
}

void meshMappingToolBox::updateView()
{
    resetComboBoxes();

    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    medAbstractLayeredView *medView = dynamic_cast<medAbstractLayeredView *> (view);
    if (medView)
    {
        d->view = medView;

        QList<medDataIndex> layerList = medView->dataList();
        for (int i=0; i<layerList.size(); i++)
        {
            addLayer(i);
        }

        QObject::connect(d->view, SIGNAL(closed()),
                         this, SLOT(resetComboBoxes()),
                         Qt::UniqueConnection);
    }
}

void meshMappingToolBox::resetComboBoxes()
{
    d->layersForStructure->clear();
    d->layersForData->clear();
    d->layersForStructure->addItem("Select a layer");
    d->layersForData->addItem("Select a layer");
    d->nbOfMeshLayers = 0;
    d->nbOfImageLayers = 0;
}

void meshMappingToolBox::addLayer(unsigned int layer)
{
    medAbstractLayeredView *medView = static_cast<medAbstractLayeredView *> (d->view);
    if (medView)
    {
        medAbstractData *data = medView->layerData(layer);

        QString name = medMetaDataKeys::SeriesDescription.getFirstValue(data,"<i>no name</i>");

        if(data && !data->identifier().contains("vtkDataMesh"))
        {
            d->nbOfImageLayers += 1;
            d->layersForStructure->addItem(name);
            d->layersForData->addItem(name);
        }
        else
        {
            d->nbOfMeshLayers += 1;
            d->layersForStructure->addItem(name);
            d->layersForData->addItem(name);
        }
    }
}
