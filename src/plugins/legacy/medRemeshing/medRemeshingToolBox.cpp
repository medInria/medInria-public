/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <medAbstractLayeredView.h>
#include <medDecimateMeshProcess.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medRefineMeshProcess.h>
#include <medRemeshingToolBox.h>
#include <medRunnableProcess.h>
#include <medSmoothMeshProcess.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>
#include <medVtkViewBackend.h>

#include <vtkDataSetCollection.h>
#include <vtkImageView2D.h>
#include <vtkPointSet.h>

class medRemeshingToolBoxPrivate
{
public:
    dtkSmartPointer <medDecimateMeshProcess> decimateProcess;
    dtkSmartPointer <medRefineMeshProcess> refineProcess;
    dtkSmartPointer <medSmoothMeshProcess> smoothProcess;

    medRemeshingToolBox::ProcessName currentProcess;
    medRemeshingToolBox::ProcessName lastManualProcess;

    medAbstractLayeredView *view;
    dtkSmartPointer<medAbstractData> input, refinedMesh;
    QLabel *initialCellsLabel, *outputCellsLabel;
    QSpinBox *trianglesSpinBox;
    QPushButton *decimateButton, *refineButton, *runUserNumber, *resetButton;
    QRadioButton *topologyRadioButton;
    vtkSmartPointer<vtkDataSet> original_points;

    // Smooth tool
    QSpinBox *iterationsSpinBox;
    QDoubleSpinBox *relaxationSpinBox;
    QPushButton *smoothButton;
};

medRemeshingToolBox::medRemeshingToolBox(QWidget *parent)
 : medAbstractSelectableToolBox(parent)
 , d(new medRemeshingToolBoxPrivate)
{
    QVBoxLayout *displayLayout = new QVBoxLayout();

    // Font used for warning, information, etc.
    QFont font;
    font.setItalic(true);

    // Decimate and Refine buttons
    QLabel* decimateRefineTitle = new QLabel("Decimate/Refine", this);
    decimateRefineTitle->setObjectName("innerTitle");
    displayLayout->addWidget(decimateRefineTitle);

    // Decimate
    QLabel *explainDecimationTxt = new QLabel("divide by 4", this);

    d->decimateButton = new QPushButton("Decimate", this);
    d->decimateButton->setToolTip("Reduce the number of cells by 4");
    connect (d->decimateButton, SIGNAL(clicked()), this, SLOT(launchDecimate()));

    QHBoxLayout *layoutDecimate = new QHBoxLayout();
    layoutDecimate->addWidget(explainDecimationTxt);
    layoutDecimate->addWidget(d->decimateButton);
    displayLayout->addLayout(layoutDecimate);

    d->topologyRadioButton = new QRadioButton(tr("Preserve topology during decimation"));
    d->topologyRadioButton->setToolTip("Turn on/off whether to preserve the mesh topology.\nIf on, splitting and hole elimination will not occur.\nCan limit the maximum reduction that may be achieved");
    d->topologyRadioButton->setChecked(true);
    d->topologyRadioButton->setObjectName("topologyRadioButton");
    displayLayout->addWidget(d->topologyRadioButton);
    connect (d->topologyRadioButton, SIGNAL(toggled(bool)), this, SLOT(allowDecimateIfTopologyButtonUnchecked(bool)));

    // Refine
    QLabel *explainRefineTxt = new QLabel("multiply by 4", this);
    d->refineButton = new QPushButton("Refine", this);
    d->refineButton->setToolTip("Increase the number of cells by 4");
    connect (d->refineButton, SIGNAL(clicked()), this, SLOT(launchRefine()));

    QHBoxLayout *layoutRefine = new QHBoxLayout();
    layoutRefine->addWidget(explainRefineTxt);
    layoutRefine->addWidget(d->refineButton);
    displayLayout->addLayout(layoutRefine);

    // Separator
    QFrame *line1 = new QFrame(this);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    displayLayout->addWidget(line1);

    // User can choose a number of cells
    QLabel *numberTitle = new QLabel("Choose the number of cells that you want", this);
    numberTitle->setObjectName("innerTitle");
    displayLayout->addWidget(numberTitle);

    QLabel *numberTxt = new QLabel("number of cells", this);
    d->trianglesSpinBox = new QSpinBox;
    d->trianglesSpinBox->setMaximum(30000000);
    d->trianglesSpinBox->setMinimum(0);
    d->trianglesSpinBox->setValue(0);
    d->runUserNumber = new QPushButton("Run", this);
    connect(d->runUserNumber, SIGNAL(clicked()), this, SLOT(imposeNbOfTriangles()));

    QHBoxLayout *numberLayout = new QHBoxLayout();
    numberLayout->addWidget(numberTxt);
    numberLayout->addWidget(d->trianglesSpinBox);
    numberLayout->addWidget(d->runUserNumber);
    displayLayout->addLayout(numberLayout);

    // Separator
    QFrame *line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    displayLayout->addWidget(line2);

    // Smooth tool
    QLabel *smoothTitle = new QLabel("Smooth", this);
    smoothTitle->setObjectName("innerTitle");
    displayLayout->addWidget(smoothTitle);

    QLabel *iterationsLabel = new QLabel("iterations", this);
    d->iterationsSpinBox = new QSpinBox;
    d->iterationsSpinBox->setRange(0, 100);
    d->iterationsSpinBox->setSingleStep(1);
    d->iterationsSpinBox->setValue(30);

    QLabel *relaxationLabel = new QLabel("relaxation factor", this);
    d->relaxationSpinBox = new QDoubleSpinBox;
    d->relaxationSpinBox->setRange(0.0, 1.0);
    d->relaxationSpinBox->setSingleStep(0.01);
    d->relaxationSpinBox->setDecimals(2);
    d->relaxationSpinBox->setValue(0.2);

    d->smoothButton = new QPushButton(tr("Run"), this);
    connect(d->smoothButton, SIGNAL(clicked()), this, SLOT(smoothMesh()));

    QHBoxLayout *smoothLayout = new QHBoxLayout();
    smoothLayout->addWidget(iterationsLabel);
    smoothLayout->addStretch(1);
    smoothLayout->addWidget(d->iterationsSpinBox);
    smoothLayout->addStretch(1);
    smoothLayout->addWidget(relaxationLabel);
    smoothLayout->addStretch(1);
    smoothLayout->addWidget(d->relaxationSpinBox);
    smoothLayout->addStretch(1);
    smoothLayout->addWidget(d->smoothButton);
    smoothLayout->addStretch(1);
    displayLayout->addLayout(smoothLayout);

    // Separator
    QFrame *line3 = new QFrame(this);
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);
    displayLayout->addWidget(line3);

    // Number of cells labels
    QVBoxLayout *numberOfCellsLayout = new QVBoxLayout();
    d->initialCellsLabel = new QLabel(this);
    d->initialCellsLabel->setFont(font);
    d->outputCellsLabel = new QLabel(this);
    d->outputCellsLabel->setFont(font);
    d->outputCellsLabel->hide();
    numberOfCellsLayout->addWidget(d->initialCellsLabel);
    numberOfCellsLayout->addWidget(d->outputCellsLabel);
    displayLayout->addLayout(numberOfCellsLayout);

    // Reset button
    d->resetButton = new QPushButton(tr("Reset"), this);
    d->resetButton->setEnabled(false);
    d->resetButton->setObjectName("resetButton");
    connect(d->resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    displayLayout->addWidget(d->resetButton);

    QWidget *widget = new QWidget(this);
    widget->setLayout(displayLayout);

    this->addWidget(widget);

    disableButtons(true);
    d->view = nullptr;
    d->input = nullptr;
    d->refinedMesh = nullptr;
    d->original_points = nullptr;

    this->hide();
}

medRemeshingToolBox::~medRemeshingToolBox()
{
    delete d;
    d = nullptr;
}

medAbstractData *medRemeshingToolBox::processOutput()
{
    medAbstractData *res = nullptr;

    switch (d->currentProcess)
    {
    case REFINE:
    {
        if(d->refineProcess)
        {
            res = d->refineProcess->output();
        }

        break;
    }
    case DECIMATE:
    {
        if(d->decimateProcess)
        {
            res = d->decimateProcess->output();
        }

        break;
    }
    case SMOOTH:
    {
        if(d->smoothProcess)
        {
            res = d->smoothProcess->output();
        }

        break;
    }
    case MANUAL:
    {
        if(d->lastManualProcess == REFINE)
        {
            medUtilities::setDerivedMetaData(d->refineProcess->output(), d->input, "manual");
            res = d->refineProcess->output();
        }
        else if(d->lastManualProcess == DECIMATE)
        {
            medUtilities::setDerivedMetaData(d->decimateProcess->output(), d->input, "manual");
            res = d->decimateProcess->output();
        }

        break;
    }
    }

    return res;
}

bool medRemeshingToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medRemeshingToolBox>();
}

dtkPlugin* medRemeshingToolBox::plugin()
{
    return medPluginManager::instance().plugin("Remeshing");
}

void medRemeshingToolBox::updateView()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if(!view)
    {
        clear();
        return;
    }

    medAbstractLayeredView *medView = dynamic_cast<medAbstractLayeredView *> (view);

    if ((d->view) && (d->view != medView) )
    {
        clear();
    }

    d->view = medView;

    medAbstractData *data = d->view->layerData(d->view->currentLayer());

    if (data && data->identifier().contains("vtkDataMesh"))
    {
        if(!d->input)
        {
            d->input       = d->view->layerData(d->view->currentLayer());
            d->refinedMesh = d->view->layerData(d->view->currentLayer());
            disableButtons(false);

            vtkMetaDataSet *_dataset = static_cast<vtkMetaDataSet*>(d->input->data());

            d->original_points = vtkSmartPointer<vtkDataSet>::Take(_dataset->GetDataSet()->NewInstance());
            d->original_points->DeepCopy(_dataset->GetDataSet());

            d->trianglesSpinBox->setValue(getNumberOfCells(d->input));
            d->initialCellsLabel->setText("Original number of cells: " + QString::number(getNumberOfCells(d->input)));
            QObject::connect(d->view, SIGNAL(currentLayerChanged()),
                             this, SLOT(clearAndUpdate()),
                             Qt::UniqueConnection);

            medUtilities::switchTo3D(d->view, medUtilities::MSR);
        }
    }
}

void medRemeshingToolBox::clearAndUpdate()
{
    clear();
    updateView();
}

void medRemeshingToolBox::clear()
{
    d->view = nullptr;
    d->input = nullptr;
    d->refinedMesh = nullptr;
    d->original_points = nullptr;
    d->resetButton->setEnabled(false);
    d->outputCellsLabel->hide();
    d->initialCellsLabel->setText("");
    d->trianglesSpinBox->setValue(0);

    // "Choose the number of cells", Decimate, Refine and Smooth are not allowed without data
    disableButtons(true);
}

void medRemeshingToolBox::reset()
{
    if (d->input && d->input->data())
    {
        displayChanges(d->input);
        allowDecimate();
    }
}

void medRemeshingToolBox::launchDecimate()
{
    d->currentProcess = DECIMATE;
    decimateMesh(0.75); // Divide the nb of triangles by 4 (reverse operation of refine)
}

void medRemeshingToolBox::decimateMesh(double factor, dtkSmartPointer<medAbstractData> data)
{
    if (d->refinedMesh)
    {
        this->setToolBoxOnWaitStatus();

        if (!d->decimateProcess)
        {
            d->decimateProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("medDecimateMeshProcess");
        }

        if (!data)
        {
            d->decimateProcess->setInput(d->refinedMesh, 0);
        }
        else
        {
            d->decimateProcess->setInput(data, 0);
        }
        d->decimateProcess->setParameter(factor, 0);
        d->decimateProcess->setParameter(d->topologyRadioButton->isChecked(), 1);

        connect(d->decimateProcess, SIGNAL(warning()), this, SLOT (cantDecimateMore()));
        connect(d->decimateProcess, SIGNAL(polyDataCastFailure()), this, SLOT (displayCastFailure()));

        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess (d->decimateProcess);
        connect (runProcess, SIGNAL (success  (QObject*)),  this, SLOT(addMeshToView()));
        this->addConnectionsAndStartJob(runProcess);
    }
}

void medRemeshingToolBox::launchRefine()
{
    d->currentProcess = REFINE;
    refineMesh(1.0); // Multiply the nb of triangles by 4 (due to loop algorithm)
}

void medRemeshingToolBox::refineMesh(double factor, dtkSmartPointer<medAbstractData> data)
{
    if (d->refinedMesh)
    {
        this->setToolBoxOnWaitStatus();

        if (!d->refineProcess)
        {
            d->refineProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("medRefineMeshProcess");
        }

        if (!data)
        {
            d->refineProcess->setInput(d->refinedMesh);
        }
        else
        {
            d->refineProcess->setInput(data);
        }
        d->refineProcess->setParameter(factor, 0);

        connect(d->refineProcess, SIGNAL(polyDataCastFailure()), this, SLOT (displayCastFailure()));

        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess (d->refineProcess);
        connect (runProcess, SIGNAL (success  (QObject*)),  this, SLOT(addMeshToView()));
        this->addConnectionsAndStartJob(runProcess);
    }
}

void medRemeshingToolBox::imposeNbOfTriangles()
{
    this->setToolBoxOnWaitStatusForNonRunnableProcess();

    d->currentProcess = MANUAL;
    d->trianglesSpinBox->blockSignals(true);

    d->decimateProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("medDecimateMeshProcess");
    d->decimateProcess->setParameter(false, 2); //do not change MetaData

    d->refineProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("medRefineMeshProcess");
    d->refineProcess->setParameter(false, 2); //do not change MetaData

    double askedNumber = static_cast<double>(d->trianglesSpinBox->value());
    double actualNumber = getNumberOfCells(d->refinedMesh);
    double decimateValue = 0.0;

    if( askedNumber < actualNumber )
    {
        d->lastManualProcess = DECIMATE;
        decimateValue = 1.0 - askedNumber/actualNumber;
        decimateMesh(decimateValue);
    }
    else if( askedNumber > actualNumber )
    {
        // Refine only multiply the number of cells by x*4, so we need to
        // multiply by x*4 in order to have a higher number of cells than the asked number,
        // and decimate after.
        double refineValue = ceil(log(askedNumber/actualNumber)/log(4.0));

        d->refineProcess->setInput(d->refinedMesh);
        d->refineProcess->setParameter(refineValue, 0);
        connect(d->refineProcess, SIGNAL(polyDataCastFailure()), this, SLOT (displayCastFailure()));
        d->lastManualProcess = REFINE;

        if(d->refineProcess->update() == medAbstractProcessLegacy::SUCCESS)
        {
            allowDecimate();
            d->lastManualProcess = DECIMATE;

            decimateValue = 1.0 - askedNumber/getNumberOfCells(d->refineProcess->output());
            decimateMesh(decimateValue, d->refineProcess->output());
        }
    }

    d->decimateProcess->setParameter(true, 2); //change MetaData
    d->refineProcess->setParameter(true, 1); //change MetaData
    d->trianglesSpinBox->blockSignals(false);

    this->setToolBoxOnReadyToUse();
}

double medRemeshingToolBox::getNumberOfCells(dtkSmartPointer<medAbstractData> data)
{
    vtkMetaDataSet *metaDataSet = static_cast<vtkMetaDataSet*>(data->data());
    vtkDataSet *dataSet = dynamic_cast<vtkDataSet*>(metaDataSet->GetDataSet());
    return dataSet->GetNumberOfCells();
}

void medRemeshingToolBox::addMeshToView()
{
    medRunnableProcess *process = static_cast<medRunnableProcess*>(sender());

    QStringList options;
    options << "medDecimateMeshProcess" << "medRefineMeshProcess" << "medSmoothMeshProcess";

    switch (options.indexOf(process->getProcess()->description()))
    {
        case 0:
        {
            displayComputedMesh(d->decimateProcess->output());
            break;
        }
        case 1:
        {
            displayComputedMesh(d->refineProcess->output());
            allowDecimate();
            break;
        }
        case 2:
        {
            displayComputedMesh(d->smoothProcess->output());
            break;
        }
        default:
        {
            return;
        }
    }
}

void medRemeshingToolBox::displayComputedMesh(dtkSmartPointer<medAbstractData> output)
{
    if (output && output->data())
    {
        displayChanges(output);
        d->resetButton->setEnabled(true);
    }
}

void medRemeshingToolBox::displayChanges(dtkSmartPointer<medAbstractData> output)
{
    d->refinedMesh = output;

    vtkImageView3D *view3D = static_cast<medVtkViewBackend*>(d->view->backend())->view3D;
    vtkImageView2D *view2D = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    
    vtkMetaDataSet *_dataset = static_cast<vtkMetaDataSet*>(output->data());
    vtkPointSet *new_points = vtkPointSet::SafeDownCast(_dataset->GetDataSet());

    vtkPointSet *pointsInView3d = vtkPointSet::SafeDownCast(view3D->GetDataSetCollection()->GetItem(d->view->currentLayer()));
    vtkPointSet *pointsInView2d = vtkPointSet::SafeDownCast(view2D->GetDataSetCollection()->GetItem(d->view->currentLayer()));

    pointsInView3d->ShallowCopy(new_points);
    pointsInView2d->ShallowCopy(new_points);
    
    vtkMetaDataSet *inputDataSet = static_cast<vtkMetaDataSet*>(d->input->data());
    inputDataSet->SetDataSet(d->original_points);

    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    view->render();

    displayNewNumberOfCells(output);
}

void medRemeshingToolBox::displayNewNumberOfCells(dtkSmartPointer<medAbstractData> output)
{
    d->outputCellsLabel->setText("New number of cells: " + QString::number(static_cast<int>(getNumberOfCells(output))));
    d->outputCellsLabel->show();

    d->trianglesSpinBox->setValue(getNumberOfCells(output)); // Update in case of limit reached
}

void medRemeshingToolBox::cantDecimateMore()
{
    medMessageController::instance().showError(tr("Not possible to decimate more without changing the mesh topology"), 5000);
    d->decimateButton->setEnabled(false);
}

void medRemeshingToolBox::allowDecimate()
{
    d->decimateButton->setEnabled(true);
}

void medRemeshingToolBox::allowDecimateIfTopologyButtonUnchecked(bool checked)
{
    if (!checked && d->input)
    {
        allowDecimate();
    }
}

void medRemeshingToolBox::disableButtons(bool val)
{
    d->runUserNumber->setDisabled(val);  // "choose the number of cells"
    d->decimateButton->setDisabled(val); // Decimate
    d->refineButton->setDisabled(val);   // Refine
    d->smoothButton->setDisabled(val);   // Smooth
}

void medRemeshingToolBox::smoothMesh()
{
    d->currentProcess = SMOOTH;

    if (d->refinedMesh)
    {
        this->setToolBoxOnWaitStatus();

        if (!d->smoothProcess)
        {
            d->smoothProcess = dtkAbstractProcessFactory::instance()->createSmartPointer("medSmoothMeshProcess");
        }
        d->smoothProcess->setInput(d->refinedMesh);

        d->smoothProcess->setParameter((double)d->iterationsSpinBox->value(), 0);
        d->smoothProcess->setParameter(d->relaxationSpinBox->value(),         1);

        connect(d->smoothProcess, SIGNAL(polyDataCastFailure()), this, SLOT(displayCastFailure()));

        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess (d->smoothProcess);
        connect (runProcess, SIGNAL (success  (QObject*)),  this, SLOT(addMeshToView()));
        this->addConnectionsAndStartJob(runProcess);
    }
}

void medRemeshingToolBox::displayCastFailure()
{
    displayMessageError("This toolbox is designed for vtkPolyData-based meshes");
}
