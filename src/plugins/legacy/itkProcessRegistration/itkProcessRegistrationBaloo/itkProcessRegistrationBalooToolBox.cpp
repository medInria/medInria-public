/* itkProcessRegistrationBalooToolBox.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:39:49 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:42:22 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 71
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "itkProcessRegistrationBaloo.h"
#include "itkProcessRegistrationBalooToolBox.h"

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <medAbstractRegistrationProcess.h>

#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medAbstractImageData.h>
#include <medRunnableProcess.h>
#include <medProgressionStack.h>
#include <medJobManagerL.h>
#include <medPluginManager.h>
#include <medGroupBox.h>

#include <QtGui>

class itkProcessRegistrationBalooToolBoxPrivate
{
public:
    QComboBox * transformTypeCombo;
    QComboBox * simMeasureComboBox;
    QSpinBox * iterationBox;
    QSpinBox * finestLevel;
    QSpinBox * coarsestLevel;
    QSpinBox * blockKeepRatio;
    QSpinBox * blockSizeX;
    QSpinBox * blockSizeY;
    QSpinBox * blockSizeZ;
    QSpinBox * blockSpacingX;
    QSpinBox * blockSpacingY;
    QSpinBox * blockSpacingZ;
    QSpinBox * NBSizeX;
    QSpinBox * NBSizeY;
    QSpinBox * NBSizeZ;
    QSpinBox * stepSizeX;
    QSpinBox * stepSizeY;
    QSpinBox * stepSizeZ;
    QDoubleSpinBox * minimalVariance;
    QDoubleSpinBox * lts;
    QCheckBox * doubleIterations;
    QCheckBox * blocksOverStep;
    QSpinBox * processorsBox;
    medProgressionStack * progression_stack;
};

itkProcessRegistrationBalooToolBox::itkProcessRegistrationBalooToolBox(QWidget *parent) : medRegistrationAbstractToolBox(parent), d(new itkProcessRegistrationBalooToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QFormLayout *formLayout = new QFormLayout(widget);

    QPushButton *runButton = new QPushButton(tr("Run"), widget);
    runButton->setToolTip(tr("Start Registration"));



    d->transformTypeCombo = new QComboBox (widget);

    d->transformTypeCombo->addItem(tr("rigid"),"rigid");
    d->transformTypeCombo->addItem(tr("similitude"),"similitude");
    d->transformTypeCombo->addItem(tr("affine"),"affine");

    d->simMeasureComboBox = new QComboBox(this);
    QStringList simMeasures;
    simMeasures << "SCC" << "CC" << "SSD";
    d->simMeasureComboBox->addItems(simMeasures);
    //WARNING IF YOU CHANGE THE ORDER OF THE ITEMS, CHANGE TOOLTIPS also!!!
    d->simMeasureComboBox->setItemData(0,tr("Squared Correlation Coefficient"),
                                       Qt::ToolTipRole);
    d->simMeasureComboBox->setItemData(1,tr("Correlation Coefficient"),
                                  Qt::ToolTipRole);
    d->simMeasureComboBox->setItemData(2,tr("Sum of Squared "
                                  "Differences, for scalar and tensor "
                                  "images."),
                                  Qt::ToolTipRole);
//    d->simMeasureComboBox->setItemData(3,tr("Tensor Squared Correlation "
//                                  "Coefficient, for tensor images only, "
//                                  "experimental measure: should not be used"),
//                                  Qt::ToolTipRole);
//    d->simMeasureComboBox->setItemData(4,tr("Tensor Squared Correlation "
//                                  "Coefficient OVL, for tensor images only, "
//                                  "experimental measure: should not be used"),
//                                  Qt::ToolTipRole);
//    d->simMeasureComboBox->setItemData(5,tr("Mixed Squared Correlation Coefficient, "
//                                  "experimental measure: should not be "
//                                  "used"),Qt::ToolTipRole);
//    d->simMeasureComboBox->setItemData(6,tr("MAXimum  Squared Correlation "
//                                  "Coefficient, experimental measure: "
//                                  "should not be used"),Qt::ToolTipRole);


    d->iterationBox = new QSpinBox(widget);
    d->iterationBox->setMinimum(1);
    d->iterationBox->setSingleStep(1);
    d->iterationBox->setValue(10);
    d->iterationBox->setToolTip(
                tr("Number of iterations at each pyramid level (default 10)."));

    d->finestLevel = new QSpinBox(widget);
    d->finestLevel->setMinimum(0);
    d->finestLevel->setSingleStep(1);
    d->finestLevel->setValue(1);

    d->coarsestLevel = new QSpinBox(widget);
    d->coarsestLevel->setMinimum(0);
    d->coarsestLevel->setSingleStep(1);
    d->coarsestLevel->setValue(3);

    d->minimalVariance = new QDoubleSpinBox(widget);
    d->minimalVariance->setMinimum(0.0);
    d->minimalVariance->setSingleStep(0.01);
    d->minimalVariance->setValue(10.0);
    d->minimalVariance->setToolTip(
                tr("Minimal variance for block pruning (default 10.0)."));
    d->doubleIterations = new QCheckBox (widget);
    d->doubleIterations->setChecked(false);
    d->doubleIterations->setToolTip(
                tr("If activated, double the number of iterations for half the "
                   "pyramid levels (the coarsest)."));

    //Advanced parameters in groupBox:
    medGroupBox * advancedBox = new medGroupBox(this);
    advancedBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    advancedBox->setTitle(tr("Advanced Parameters"));
//    advancedBox->setToolTip(tr("Click to see advanced parameters."));
    //connect signals before checking the box if we want to set the tooltip
    connect(advancedBox,SIGNAL(toggled(bool)),
            this,SLOT(onAdvancedBoxToggled(bool)));
    advancedBox->setCollapsible(true);
    advancedBox->setCheckable(true);
    advancedBox->setChecked(false);
    advancedBox->setStyleSheet("medGroupBox {margin: 10px;}");
    //Added a proxy widget here because the size
    //of the collapsed Box was too big...
    QWidget * advancedWidgets = new QWidget(advancedBox);
    QFormLayout *advancedformLayout = new QFormLayout(advancedWidgets);
    QVBoxLayout * vlayout = new QVBoxLayout(advancedBox);
    vlayout->addWidget(advancedWidgets);

    d->lts = new QDoubleSpinBox(advancedWidgets);
    d->lts->setMinimum(0.0);
    d->lts->setSingleStep(0.01);
    d->lts->setValue(0.8);
    d->lts->setToolTip(
                tr("Least Trimmed Square cut value for the "
                   "Weighted LTS estimator."));

    d->blocksOverStep = new QCheckBox (advancedWidgets);
    d->blocksOverStep->setChecked(false);
    d->blocksOverStep->setToolTip(
                tr("If activated, allows the blocks to overstep "
                   "the image border."));

    d->blockKeepRatio = new QSpinBox(advancedWidgets);
    d->blockKeepRatio->setMinimum(0);
    d->blockKeepRatio->setSingleStep(1);
    d->blockKeepRatio->setValue(80);
    d->blockKeepRatio->setSuffix(" %");
    d->blockKeepRatio->setToolTip(
                tr("Percentage of Blocks to keep. Blocks are sorted by "
                   "variance. Blocks with the smaller variance "
                   "(homogeneous blocks) are ignored(default 80)."));

    d->blockSizeX = new QSpinBox(advancedWidgets);
    d->blockSizeX->setMinimum(1);
    d->blockSizeX->setSingleStep(1);
    d->blockSizeX->setValue(2);
    d->blockSizeX->setSuffix(" vx");
    d->blockSizeX->setToolTip(
                tr("Half size (in voxels) of blocks along the X-axis"
                   " (block-matching). 2 indicates that blocks of size 5 "
                   "pixels  along X will be used.(default 2)."));

    d->blockSizeY = new QSpinBox(advancedWidgets);
    d->blockSizeY->setMinimum(1);
    d->blockSizeY->setSingleStep(1);
    d->blockSizeY->setValue(2);
    d->blockSizeY->setSuffix(" vx");
    d->blockSizeY->setToolTip(
                tr("Half size (in voxels) of blocks along the Y-axis"
                   " (block-matching). 2 indicates that blocks of size 5 "
                   "pixels  along Y will be used.(default 2)."));
    d->blockSizeZ = new QSpinBox(advancedWidgets);
    d->blockSizeZ->setMinimum(1);
    d->blockSizeZ->setSingleStep(1);
    d->blockSizeZ->setValue(2);
    d->blockSizeZ->setSuffix(" vx");
    d->blockSizeZ->setToolTip(
                tr("Half size (in voxels) of blocks along the Z-axis"
                   " (block-matching). 2 indicates that blocks of size 5 "
                   "pixels  along Z will be used.(default 2)."));

    d->blockSpacingX = new QSpinBox(advancedWidgets);
    d->blockSpacingX->setMinimum(1);
    d->blockSpacingX->setSingleStep(1);
    d->blockSpacingX->setValue(2);
    d->blockSpacingX->setSuffix(" vx");
    d->blockSpacingX->setToolTip(
                tr("Space (in voxels) between blocks along the X-axis "
                   "(block-matching) (default 2)."));

    d->blockSpacingY = new QSpinBox(advancedWidgets);
    d->blockSpacingY->setMinimum(1);
    d->blockSpacingY->setSingleStep(1);
    d->blockSpacingY->setValue(2);
    d->blockSpacingY->setSuffix(" vx");
    d->blockSpacingY->setToolTip(
                tr("Space (in voxels) between blocks along the Y-axis "
                   "(block-matching) (default 2)."));
    d->blockSpacingZ = new QSpinBox(advancedWidgets);
    d->blockSpacingZ->setMinimum(1);
    d->blockSpacingZ->setSingleStep(1);
    d->blockSpacingZ->setValue(2);
    d->blockSpacingZ->setSuffix(" vx");
    d->blockSpacingZ->setToolTip(
                tr("Space (in voxels) between blocks along the Z-axis "
                   "(block-matching) (default 2)."));

    d->NBSizeX = new QSpinBox(advancedWidgets);
    d->NBSizeX->setMinimum(1);
    d->NBSizeX->setSingleStep(1);
    d->NBSizeX->setValue(1);
    d->NBSizeX->setSuffix(" vx");
    d->NBSizeX->setToolTip(
                tr("Neighborhood size (in voxels) along the X-axis. This size "
                   "defines how far will go the exploration around "
                   "the initial block. (default 1)."));

    d->NBSizeY = new QSpinBox(advancedWidgets);
    d->NBSizeY->setMinimum(1);
    d->NBSizeY->setSingleStep(1);
    d->NBSizeY->setValue(1);
    d->NBSizeY->setSuffix(" vx");
    d->NBSizeY->setToolTip(
                tr("Neighborhood size (in voxels) along the Y-axis. This size "
                   "defines how far will go the exploration around "
                   "the initial block. (default 1)."));
    d->NBSizeZ = new QSpinBox(advancedWidgets);
    d->NBSizeZ->setMinimum(1);
    d->NBSizeZ->setSingleStep(1);
    d->NBSizeZ->setValue(1);
    d->NBSizeZ->setSuffix(" vx");
    d->NBSizeZ->setToolTip(
                tr("Neighborhood size (in voxels) along the Z-axis. This size "
                   "defines how far will go the exploration around "
                   "the initial block. (default 1)."));

    d->stepSizeX = new QSpinBox(advancedWidgets);
    d->stepSizeX->setMinimum(1);
    d->stepSizeX->setSingleStep(1);
    d->stepSizeX->setValue(1);
    d->stepSizeX->setSuffix(" vx");
    d->stepSizeX->setToolTip(
                tr("Step size (in voxels) along the X-axis (block-matching) "
                   "(default 1)."));

    d->stepSizeY = new QSpinBox(advancedWidgets);
    d->stepSizeY->setMinimum(1);
    d->stepSizeY->setSingleStep(1);
    d->stepSizeY->setValue(1);
    d->stepSizeY->setSuffix(" vx");
    d->stepSizeY->setToolTip(
                tr("Step size (in voxels) along the X-axis (block-matching) "
                   "(default 1)."));
    d->stepSizeZ = new QSpinBox(advancedWidgets);
    d->stepSizeZ->setMinimum(1);
    d->stepSizeZ->setSingleStep(1);
    d->stepSizeZ->setValue(1);
    d->stepSizeZ->setSuffix(" vx");
    d->stepSizeZ->setToolTip(
                tr("Step size (in voxels) along the X-axis (block-matching) "
                   "(default 1)."));

    d->processorsBox = new QSpinBox(widget);
    d->processorsBox->setMinimum(1);
    d->processorsBox->setValue(1);
    d->processorsBox->setToolTip(
                tr("Number of logical processors to be used."));


    formLayout->addRow(tr("Transformation type"),d->transformTypeCombo);
    formLayout->addRow(tr("Similarity Measure"),d->simMeasureComboBox);
    formLayout->addRow(tr("Iterations"),d->iterationBox);
    formLayout->addRow(tr("Coarsest Pyramid Level"),d->coarsestLevel);
    formLayout->addRow(tr("Finest Pyramid Level"),d->finestLevel);
    formLayout->addRow(tr("Double iterations at coarsest "
                          "levels"),d->doubleIterations);
    formLayout->addRow(tr("Minimal Block Variance"),d->minimalVariance);

    //advanced parameters
    advancedformLayout->addRow(tr("LTS cut value"),d->lts);
    advancedformLayout->addRow(tr("Blocks Overstep"),d->blocksOverStep);
    advancedformLayout->addRow(tr("Kept Blocks Ratio"),d->blockKeepRatio);
    advancedformLayout->addRow(tr("Half Block Size (X axis)"),d->blockSizeX);
    advancedformLayout->addRow(tr("Half Block Size (Y axis)"),d->blockSizeY);
    advancedformLayout->addRow(tr("Half Block Size (Z axis)"),d->blockSizeZ);
    advancedformLayout->addRow(tr("Block Spacing (X axis)"),d->blockSpacingX);
    advancedformLayout->addRow(tr("Block Spacing (Y axis)"),d->blockSpacingY);
    advancedformLayout->addRow(tr("Block Spacing (Z axis)"),d->blockSpacingZ);
    advancedformLayout->addRow(tr("Neighborhood Size (X axis)"),d->NBSizeX);
    advancedformLayout->addRow(tr("Neighborhood Size (Y axis)"),d->NBSizeY);
    advancedformLayout->addRow(tr("Neighborhood Size (Z axis)"),d->NBSizeZ);
    advancedformLayout->addRow(tr("Step Size (X axis)"),d->stepSizeX);
    advancedformLayout->addRow(tr("Step Size (Y axis)"),d->stepSizeY);
    advancedformLayout->addRow(tr("Step Size (Z axis)"),d->stepSizeZ);
    advancedformLayout->addRow(tr("Processors"),d->processorsBox);

    this->setTitle(tr("Baloo"));

    // progression stack
    d->progression_stack = new medProgressionStack(widget);

    this->addWidget(widget);
    this->addWidget(advancedBox);
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);


    //enable about plugin. Constructor called after the plugin has been registered, go ahead call it.
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin(
                "itkProcessRegistrationBalooPlugin");
    setAboutPluginButton(plugin);
    setAboutPluginVisibility(true);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));

}

itkProcessRegistrationBalooToolBox::~itkProcessRegistrationBalooToolBox(void)
{
    delete d;

    d = NULL;
}

bool itkProcessRegistrationBalooToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerToolBox<itkProcessRegistrationBalooToolBox>();
}

void itkProcessRegistrationBalooToolBox::run(void)
{
    if(!this->parentToolBox())
        return;

    medAbstractRegistrationProcess *process;
    
    if (this->parentToolBox()->process() && (this->parentToolBox()->process()->identifier() == "itkProcessRegistrationBaloo"))
    {
        process = this->parentToolBox()->process();

    }
    else
    {
        process = new itkProcessRegistrationBaloo;
        this->parentToolBox()->setProcess(process);
    }
    medAbstractData *fixedData = this->parentToolBox()->fixedData();
    medAbstractData *movingData = this->parentToolBox()->movingData();

    if (!fixedData || !movingData )
        return;

    // Many choices here

    itkProcessRegistrationBaloo *process_reg = dynamic_cast<itkProcessRegistrationBaloo *>(process);
    if (!process_reg)
    {
        qWarning() << "registration process doesn't exist" ;
        return;
    }
    process_reg->setTransformationType(d->transformTypeCombo->itemData(
            d->transformTypeCombo->currentIndex()).toString());
    process_reg->setSimilarityMeasure(d->simMeasureComboBox->currentText());
    process_reg->setCoarsestLevel(d->coarsestLevel->value());
    process_reg->setFinestLevel(d->finestLevel->value());
    process_reg->setMinimalVarianceBlockPruning(d->minimalVariance->value());
    process_reg->setIterations(d->iterationBox->value());
    process_reg->setDoubleIterations(d->doubleIterations->isChecked());
    //advanced values:
    process_reg->setLts(d->lts->value());
    process_reg->setOverstep(d->blocksOverStep->isChecked());
    process_reg->setBlockKeepRatio(d->blockKeepRatio->value()/100.);
    process_reg->setBlockSize(d->blockSizeX->value(),
                              d->blockSizeY->value(),
                              d->blockSizeZ->value());
    process_reg->setBlockSpacing(d->blockSpacingX->value(),
                                 d->blockSpacingY->value(),
                                 d->blockSpacingZ->value());
    process_reg->setNeighbourhoodSize(d->NBSizeX->value(),
                                      d->NBSizeY->value(),
                                      d->NBSizeZ->value());
    process_reg->setStepSize(d->stepSizeX->value(),
                             d->stepSizeY->value(),
                             d->stepSizeZ->value());
    process_reg->setProcessors(d->processorsBox->value());
    // process->setMyWonderfullParameter(fronTheGui);

    // or ...

    // itkProcessRegistrationBaloo *process = new itkProcessRegistrationBaloo;
    // process->setMyWonderfullParameter(fronTheGui);
    // process->setMyWonderfullParameter(fronTheGui);

    // ----

    if (!process)
        return;

    process->setFixedInput(fixedData);
    process->setMovingInput(movingData);

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (process);

    d->progression_stack->addJobItem(runProcess, "Progress:");
    d->progression_stack->setActive(runProcess,true);
    d->progression_stack->disableCancel(runProcess);
    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));
    //First have the moving progress bar,
    //and then display the remaining % when known
    connect (runProcess, SIGNAL(activate(QObject*,bool)),
             d->progression_stack, SLOT(setActive(QObject*,bool)));

    medJobManagerL::instance()->registerJobItem(runProcess,process->identifier());
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

void itkProcessRegistrationBalooToolBox::onAdvancedBoxToggled(bool toggle)
{
    QWidget * senderWidget = qobject_cast<QWidget*> (sender());

    if (senderWidget)
    {
        if ( toggle)
            senderWidget->setToolTip("");
        else
            senderWidget->setToolTip(tr("Click to see advanced parameters."));
    }
}

