/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medAbstractLayeredView.h>
#include <medCreateMeshFromMask.h>
#include <medCreateMeshFromMaskToolBox.h>
#include <medDoubleParameterL.h>
#include <medPluginManager.h>
#include <medRunnableProcess.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <statsROI.h>

class medCreateMeshFromMaskToolBoxPrivate
{
public:

    medCreateMeshFromMask *process;
    medAbstractLayeredView *view;
    medDoubleParameterL *thresholdSlider;
    QDoubleSpinBox *reductionSpinBox;
    QCheckBox *decimateCheckbox;
    QCheckBox *smoothCheckbox;
    QSpinBox *iterationsSpinBox;
    QDoubleSpinBox *relaxationSpinBox;
    QLabel *trianglesLabel;
};

medCreateMeshFromMaskToolBox::medCreateMeshFromMaskToolBox(QWidget *parent)
    : medAbstractSelectableToolBox(parent)
    , d(new medCreateMeshFromMaskToolBoxPrivate)
{
    // Threshold label/slider/spinbox
    d->thresholdSlider = new medDoubleParameterL("Threshold", this);
    d->thresholdSlider->setToolTip(tr("Only keep values over or equal to threshold"));
    d->thresholdSlider->setValue(1);
    d->thresholdSlider->setRange(0, 2000);
    d->thresholdSlider->getSlider()->setPageStep(200);
    d->thresholdSlider->getSlider()->setOrientation(Qt::Horizontal);

    QHBoxLayout *thresholdLayout = new QHBoxLayout;
    thresholdLayout->addWidget(d->thresholdSlider->getLabel());
    thresholdLayout->addWidget(d->thresholdSlider->getSlider());
    thresholdLayout->addWidget(d->thresholdSlider->getSpinBox());

    // Decimation
    d->decimateCheckbox = new QCheckBox("Decimate mesh");
    d->decimateCheckbox->setChecked(true);

    d->reductionSpinBox = new QDoubleSpinBox;
    d->reductionSpinBox->setRange(0.0, 1.0);
    d->reductionSpinBox->setSingleStep(0.01);
    d->reductionSpinBox->setDecimals(2);
    d->reductionSpinBox->setValue(0.8);
    connect(d->decimateCheckbox, SIGNAL(toggled(bool)), d->reductionSpinBox, SLOT(setEnabled(bool)));

    QLabel *reductionLabel = new QLabel("Reduction target ");
    QHBoxLayout *reductionLayout = new QHBoxLayout;
    reductionLayout->addWidget(reductionLabel);
    reductionLayout->addWidget(d->reductionSpinBox);
    reductionLayout->setAlignment(Qt::AlignRight);

    // Smoothing
    d->smoothCheckbox = new QCheckBox("Smooth mesh");
    d->smoothCheckbox->setChecked(true);

    d->iterationsSpinBox = new QSpinBox;
    d->iterationsSpinBox->setRange(0, 100);
    d->iterationsSpinBox->setSingleStep(1);
    d->iterationsSpinBox->setValue(30);
    connect(d->smoothCheckbox, SIGNAL(toggled(bool)), d->iterationsSpinBox, SLOT(setEnabled(bool)));

    QLabel *iterationsLabel = new QLabel("Iterations ");
    QHBoxLayout *iterationsLayout = new QHBoxLayout;
    iterationsLayout->addWidget(iterationsLabel);
    iterationsLayout->addWidget(d->iterationsSpinBox);
    iterationsLayout->setAlignment(Qt::AlignRight);

    d->relaxationSpinBox = new QDoubleSpinBox;
    d->relaxationSpinBox->setRange(0.0, 1.0);
    d->relaxationSpinBox->setSingleStep(0.01);
    d->relaxationSpinBox->setDecimals(2);
    d->relaxationSpinBox->setValue(0.2);
    connect(d->smoothCheckbox, SIGNAL(toggled(bool)), d->relaxationSpinBox, SLOT(setEnabled(bool)));

    QLabel *relaxationLabel = new QLabel("Relaxation factor ");
    QHBoxLayout *relaxationLayout = new QHBoxLayout;
    relaxationLayout->addWidget(relaxationLabel);
    relaxationLayout->addWidget(d->relaxationSpinBox);
    relaxationLayout->setAlignment(Qt::AlignRight);

    QPushButton *runButton = new QPushButton(tr("Run"));

    QFont font;
    font.setItalic(true);
    d->trianglesLabel = new QLabel;
    d->trianglesLabel->setFont(font);
    d->trianglesLabel->hide();

    QWidget *widget = new QWidget(this);

    QVBoxLayout *displayLayout = new QVBoxLayout();

    displayLayout->addLayout(thresholdLayout);
    displayLayout->addWidget(d->decimateCheckbox);
    displayLayout->addLayout(reductionLayout);
    displayLayout->addWidget(d->smoothCheckbox);
    displayLayout->addLayout(iterationsLayout);
    displayLayout->addLayout(relaxationLayout);
    displayLayout->addWidget(runButton);
    displayLayout->addWidget(d->trianglesLabel);
    widget->setLayout(displayLayout);

    this->addWidget(widget);

    d->view = nullptr;

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

medCreateMeshFromMaskToolBox::~medCreateMeshFromMaskToolBox()
{
    delete d;
    d = nullptr;
}

bool medCreateMeshFromMaskToolBox::registered()
{
    return medToolBoxFactory::instance()->
    registerToolBox<medCreateMeshFromMaskToolBox>();
}

dtkPlugin* medCreateMeshFromMaskToolBox::plugin()
{
    return medPluginManager::instance().plugin("Create Mesh From Mask");
}

medAbstractData* medCreateMeshFromMaskToolBox::processOutput()
{
    if(!d->process)
    {
        return nullptr;
    }
    return d->process->output();
}

void medCreateMeshFromMaskToolBox::updateView()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if(!view)
    {
        clear();
        return;
    }

    medAbstractLayeredView *medView = dynamic_cast<medAbstractLayeredView *> (view);

    if ((d->view) && (d->view != medView))
    {
        clear();
    }
    d->view = medView;

    medAbstractData *data = d->view->layerData(d->view->currentLayer());

    // Compute min and max of the data
    if (data && !data->identifier().contains("vtkDataMesh"))
    {
        statsROI statsProcess;
        statsProcess.setInput(data, 0); // Data
        statsProcess.setParameter(statsROI::MINMAX);

        if(statsProcess.update() == medAbstractProcessLegacy::SUCCESS)
        {
            double m_MinValueImage = statsProcess.output().at(0);
            double m_MaxValueImage = statsProcess.output().at(1);

            // Update threshold slider/spinbox
            d->thresholdSlider->setRange(m_MinValueImage, m_MaxValueImage);
            d->thresholdSlider->setValue(m_MinValueImage + 1);
            d->thresholdSlider->getSlider()->setPageStep((m_MaxValueImage-m_MinValueImage)/10);
        }
    }
}

void medCreateMeshFromMaskToolBox::addMeshToView()
{
    medAbstractData *data = this->processOutput();

    int current = d->view->currentLayer();
    d->view->addLayer(data);
    d->view->setCurrentLayer(current);
}

void medCreateMeshFromMaskToolBox::clear()
{
    d->trianglesLabel->hide();
    d->view = nullptr;
}

void medCreateMeshFromMaskToolBox::run()
{
    if(d->view)
    {
        if (d->view->layersCount() > 0)
        {
            this->setToolBoxOnWaitStatus();

            d->process = new medCreateMeshFromMask();
            d->process->setInput(d->view->layerData(d->view->currentLayer()));

            d->process->setParameter(static_cast<double>(d->thresholdSlider->value()),      0); // iso value
            d->process->setParameter(static_cast<double>(d->decimateCheckbox->isChecked()), 1); // decimation
            d->process->setParameter(d->reductionSpinBox->value(),                          2); // reduction
            d->process->setParameter(static_cast<double>(d->smoothCheckbox->isChecked()),   3); // smooth
            d->process->setParameter(static_cast<double>(d->iterationsSpinBox->value()),    4); // iterations
            d->process->setParameter(d->relaxationSpinBox->value(),                         5); // relaxation factor

            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess (d->process);
            connect (runProcess, SIGNAL (success  (QObject*)), this, SLOT (addMeshToView()));
            connect (runProcess, SIGNAL (success  (QObject*)), this, SLOT (displayNumberOfTriangles()));
            connect (runProcess, SIGNAL (failure  (int)),      this, SLOT (handleDisplayError(int)));
            this->addConnectionsAndStartJob(runProcess);
        }
    }
}

void medCreateMeshFromMaskToolBox::displayNumberOfTriangles()
{
    d->trianglesLabel->setText("Number of triangles: " + QString::number(d->process->getNumberOfTriangles()));
    d->trianglesLabel->show();
}
