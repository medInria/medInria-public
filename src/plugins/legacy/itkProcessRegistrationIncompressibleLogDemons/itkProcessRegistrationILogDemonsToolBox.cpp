/* itkProcessRegistrationILogDemonsToolBox.cpp ---
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

#include "itkProcessRegistrationILogDemons.h"
#include "itkProcessRegistrationILogDemonsToolBox.h"

#include <medRunnableProcess.h>
#include <medJobManagerL.h>
#include <medDataManager.h>
#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medAbstractRegistrationProcess.h>

#include <medPluginManager.h>
#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medProgressionStack.h>
#include <medDropSite.h>
#include <medGroupBox.h>

#include <rpiCommonTools.hxx>

#include <QtGui>


class itkProcessRegistrationILogDemonsToolBoxPrivate
{
public:
    medProgressionStack * progression_stack;
    QSpinBox * minIterationsBox;
    QDoubleSpinBox * maximumUpdateStepLengthBox;
    QComboBox * gradientTypeBox;
    QDoubleSpinBox * upFieldSigmaBox;
    QComboBox * upFieldIncBox;
    QDoubleSpinBox * upFieldKappaBox;
    QDoubleSpinBox * velFieldSigmaBox;
    QComboBox * velFieldIncBox;
    QDoubleSpinBox * velFieldKappaBox;
    QCheckBox * useHistogramMatchingBox;
    QSpinBox * BCHExpansionBox;
    QDoubleSpinBox * spatialDerivativeSigmaBox;
    QSpinBox * stepSizeBox;
    QDoubleSpinBox * DTIStandardDeviationBox;
    QComboBox * noiseEstimatorTypeBox;
    QDoubleSpinBox * noiseEstimatorSigmaBox;
    QCheckBox * noiseEstimatorLMBox;
    QDoubleSpinBox * noiseEstimatorEpsilonBox;
    QLineEdit * iterationsBox;
    QLineEdit * stopBox;
    QCheckBox * useMask;
    QLabel * maskLabel;
    medDropSite * dropSiteMask;
    dtkSmartPointer<medAbstractData> dataMask;

};

itkProcessRegistrationILogDemonsToolBox::itkProcessRegistrationILogDemonsToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new itkProcessRegistrationILogDemonsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QPushButton *runButton = new QPushButton(tr("Run"), this);

    QFormLayout *layout = new QFormLayout(widget);

    d->useMask = new QCheckBox();

    d->maskLabel = new QLabel(tr("Mask Image"), this);
    d->maskLabel->setVisible(false);
    d->dropSiteMask = new medDropSite();
    d->dropSiteMask->setVisible(false);
    //QObject::connect(this->parent()->movingView(), SIGNAL(dataAdded(dtkAbstractData*, int)), this, SLOT(onDataAdded(dtkAbstractData*, int)));

    d->iterationsBox = new QLineEdit(this);
    d->iterationsBox->setText("15x10x5");
    d->iterationsBox->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));

    d->stopBox = new QLineEdit(this);
    d->stopBox->setText("-1x-1x-1");
    d->stopBox->setToolTip(tr("List of threshold values for each multi-scale pyramid level (from coarse to fine levels). Negative values: stops when MSE increases. Positive values: stops when MSE is lower than the provided value."));

    d->minIterationsBox = new QSpinBox(this);
    d->minIterationsBox->setMinimum(-1);
    d->minIterationsBox->setMaximum(10);
    d->minIterationsBox->setSingleStep(1);
    d->minIterationsBox->setValue(-1);
    d->minIterationsBox->setToolTip(tr("Minimum number of iterations for every levels of resolution. Value -1 implies no minimum number of iterations."));

    d->maximumUpdateStepLengthBox = new QDoubleSpinBox(this);
    d->maximumUpdateStepLengthBox->setMinimum(0);
    d->maximumUpdateStepLengthBox->setMaximum(1000);
    d->maximumUpdateStepLengthBox->setSingleStep(0.01);
    d->maximumUpdateStepLengthBox->setValue(2.0);
    d->maximumUpdateStepLengthBox->setToolTip(tr("Maximum length of an update vector (voxel units). Setting it to 0 implies no restrictions will be made on the step length"));

    d->gradientTypeBox = new QComboBox(this);
    QStringList gradientTypes;
    gradientTypes<< tr("Symmetrized") << tr ("Fixed Image") << tr("Warped Moving Image")
        << tr("Mapped Moving Image");
    d->gradientTypeBox->addItems(gradientTypes);



    d->upFieldSigmaBox = new QDoubleSpinBox(this);
    d->upFieldSigmaBox->setMinimum(0);
    d->upFieldSigmaBox->setMaximum(1000);
    d->upFieldSigmaBox->setSingleStep(0.01);
    d->upFieldSigmaBox->setValue(0.5);
    d->upFieldSigmaBox->setToolTip(tr("Standard deviation of the Gaussian smoothing of the update field (world units). Setting it below 0.1 means no smoothing will be performed "));

    d->upFieldIncBox = new QComboBox(this);
    QStringList upFieldIncTypes;
    upFieldIncTypes<< tr("Disabled") << tr ("L2 Norm Projection") << tr("Hessian Norm Projection")
        << tr("Alternate Projection");
    d->upFieldIncBox->addItems(upFieldIncTypes);
    //d->upFieldIncBox->setCurrentIndex(1);

    d->upFieldKappaBox = new QDoubleSpinBox(this);
    d->upFieldKappaBox->setMinimum(0);
    d->upFieldKappaBox->setMaximum(1000);
    d->upFieldKappaBox->setSingleStep(0.01);
    d->upFieldKappaBox->setValue(0.0);
    d->upFieldKappaBox->setToolTip(tr("Kappa value used for the elastic-like smoothing of the update field."));

    d->velFieldSigmaBox = new QDoubleSpinBox(this);
    d->velFieldSigmaBox->setMinimum(0);
    d->velFieldSigmaBox->setMaximum(1000);
    d->velFieldSigmaBox->setSingleStep(0.01);
    d->velFieldSigmaBox->setValue(1.5);
    d->velFieldSigmaBox->setToolTip(tr("Standard deviation of the Gaussian smoothing of the stationary velocity field (world units). Setting it below 0.1 means no smoothing will be performed"));

    d->velFieldIncBox = new QComboBox(this);
    QStringList velFieldIncTypes;
    velFieldIncTypes<< tr("Disabled") << tr ("Enabled");
    d->velFieldIncBox->addItems(velFieldIncTypes);
    d->velFieldIncBox->setCurrentIndex(1);

    d->velFieldKappaBox = new QDoubleSpinBox(this);
    d->velFieldKappaBox->setMinimum(0);
    d->velFieldKappaBox->setMaximum(1000);
    d->velFieldKappaBox->setSingleStep(0.01);
    d->velFieldKappaBox->setValue(0.0);
    d->velFieldKappaBox->setToolTip(tr("kappa value used for the elastic-like smoothing of the stationary velocity field"));

    d->useHistogramMatchingBox =  new QCheckBox(this);
    d->useHistogramMatchingBox->setChecked(false);

    d->BCHExpansionBox = new QSpinBox(this);
    d->BCHExpansionBox->setMinimum(0);
    d->BCHExpansionBox->setMaximum(1000);
    d->BCHExpansionBox->setSingleStep(1);
    d->BCHExpansionBox->setValue(2);
    d->BCHExpansionBox->setToolTip(tr("Number of terms in the BCH expansion"));

    d->spatialDerivativeSigmaBox = new QDoubleSpinBox(this);
    d->spatialDerivativeSigmaBox->setMinimum(0);
    d->spatialDerivativeSigmaBox->setMaximum(1000);
    d->spatialDerivativeSigmaBox->setSingleStep(0.01);
    d->spatialDerivativeSigmaBox->setValue(0.5);
    d->spatialDerivativeSigmaBox->setToolTip(tr("Standard deviation of the spatial derivative smoothing used in the incompressibility constraint"));

    d->stepSizeBox = new QSpinBox(this);
    d->stepSizeBox->setMinimum(1);
    d->stepSizeBox->setMaximum(1000);
    d->stepSizeBox->setSingleStep(1);
    d->stepSizeBox->setValue(5);
    d->stepSizeBox->setToolTip(tr("If the mode of the selected update field "
                    "incompressibility is the alternate projection, "
                    "this parameters defines the step size (number of "
                    "iterations) between two applications of the "
                    "Hessian-norm constraint on the update field."));

    d->DTIStandardDeviationBox = new QDoubleSpinBox(this);
    d->DTIStandardDeviationBox->setMinimum(0);
    d->DTIStandardDeviationBox->setMaximum(1000);
    d->DTIStandardDeviationBox->setSingleStep(0.01);
    d->DTIStandardDeviationBox->setValue(1.0);
    d->DTIStandardDeviationBox->setToolTip(tr("Standard deviation for the DTI smoothing. The value 0 implies that no smoothing is performed "));

    d->noiseEstimatorTypeBox = new QComboBox(this);
    QStringList noiseEstimatorTypes;
    noiseEstimatorTypes<< tr("Local") << tr ("Regional") << tr("Global");
    d->noiseEstimatorTypeBox->addItems(noiseEstimatorTypes);

    d->noiseEstimatorSigmaBox = new QDoubleSpinBox(this);
    d->noiseEstimatorSigmaBox->setMinimum(0);
    d->noiseEstimatorSigmaBox->setMaximum(1000);
    d->noiseEstimatorSigmaBox->setSingleStep(0.01);
    d->noiseEstimatorSigmaBox->setValue(5.0);
    d->noiseEstimatorSigmaBox->setToolTip(tr("Standard deviation of the noise estimator"));

    d->noiseEstimatorLMBox = new QCheckBox(this);
    d->noiseEstimatorLMBox->setChecked(false);
    d->noiseEstimatorLMBox->setToolTip(tr("If activated, uses the "
                                          "Levenberg-Marquardt method to "
                                          "stabilize the regional image noise "
                                          "estimation"));

    d->noiseEstimatorEpsilonBox = new QDoubleSpinBox(this);
    d->noiseEstimatorEpsilonBox->setMinimum(0);
    d->noiseEstimatorEpsilonBox->setMaximum(1000);
    d->noiseEstimatorEpsilonBox->setSingleStep(0.01);
    d->noiseEstimatorEpsilonBox->setValue(0.0);
    d->noiseEstimatorEpsilonBox->setToolTip(tr("Epsilon value (image weight) used in the Levenberg-Marquardt method"));

    //only enable the epsilonBox if LM is activated:
    d->noiseEstimatorEpsilonBox->setEnabled(false);
    connect(d->noiseEstimatorLMBox,SIGNAL(toggled(bool)),
            d->noiseEstimatorEpsilonBox,SLOT(setEnabled(bool)));

    this->setTitle("Incompressible Log Demons");

    layout->addRow(new QLabel(tr("Use Mask"),this), d->useMask);
    layout->addRow(d->maskLabel, d->dropSiteMask);
    layout->addRow(new QLabel(tr("Iterations"),this),d->iterationsBox);
    layout->addRow(new QLabel(tr("Threshold Values"),this),d->stopBox);
    layout->addRow(new QLabel(tr("Min. Iterations"),this),d->minIterationsBox);
    layout->addRow(new QLabel(tr("Max. Step Length"),this),d->maximumUpdateStepLengthBox);
    layout->addRow(new QLabel(tr("Gradient Type"),this), d->gradientTypeBox);

    medGroupBox * upFieldBox = new medGroupBox(this);
    upFieldBox->setTitle(tr("Update Field"));
    layout->addRow(upFieldBox);
    QFormLayout *upFieldLayout = new QFormLayout(upFieldBox);

    upFieldLayout->addRow(new QLabel(tr("Sigma"),this), d->upFieldSigmaBox);
    upFieldLayout->addRow(new QLabel(tr("Incompressibility"),this),d->upFieldIncBox);
    upFieldLayout->addRow(new QLabel(tr("Kappa"),this),d->upFieldKappaBox);

    medGroupBox * veloFieldBox = new medGroupBox(this);
    veloFieldBox->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    veloFieldBox->setTitle(tr("Stationnary Velocity Field"));
    layout->addRow(veloFieldBox);
    QFormLayout *veloFieldLayout = new QFormLayout(veloFieldBox);

    veloFieldLayout->addRow(new QLabel(tr("Sigma"),this),d->velFieldSigmaBox);
    veloFieldLayout->addRow(new QLabel(tr("Incompressibility"),this),d->velFieldIncBox);
    veloFieldLayout->addRow(new QLabel(tr("Kappa"),this),d->velFieldKappaBox);

    layout->addRow(new QLabel(tr("Histogram Matching"),this), d->useHistogramMatchingBox);
    layout->addRow(new QLabel(tr("BCH Expansion"),this), d->BCHExpansionBox);
    layout->addRow(new QLabel(tr("Spatial Derivative Sigma"),this),d->spatialDerivativeSigmaBox);
    layout->addRow(new QLabel(tr("Step Size"),this),d->stepSizeBox);
    layout->addRow(new QLabel(tr("DTI smoothing Sigma"),this),d->DTIStandardDeviationBox);
    layout->addRow(new QLabel(tr("Gradient Type"),this),d->noiseEstimatorTypeBox);
    layout->addRow(new QLabel(tr("Noise Estimator"),this),d->noiseEstimatorSigmaBox);
    layout->addRow(new QLabel(tr("LM Noise Estimator"),this), d->noiseEstimatorLMBox);
    layout->addRow(new QLabel(tr("LM Image Weight"),this), d->noiseEstimatorEpsilonBox);

    //initialise dataMask to 0
    d->dataMask = NULL;


    // progression stack
    d->progression_stack = new medProgressionStack(widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);

    this->addWidget(widget);
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
    connect(d->dropSiteMask, SIGNAL(objectDropped(medDataIndex)), this, SLOT(onMaskDropped(medDataIndex)));
    connect(d->useMask, SIGNAL(stateChanged(int)), this, SLOT(onUseMaskStateChanged(int)));
}

itkProcessRegistrationILogDemonsToolBox::~itkProcessRegistrationILogDemonsToolBox(void)
{
    delete d;

    d = NULL;
}

dtkPlugin * itkProcessRegistrationILogDemonsToolBox::plugin(void)
{
	medPluginManager* pm = medPluginManager::instance();
	dtkPlugin* plugin = pm->plugin("ILog Demons");
	return plugin;
}

medAbstractData * itkProcessRegistrationILogDemonsToolBox::processOutput()
{
	// If called from pipelines, and run() not called before.
	if ( static_cast<medRegistrationSelectorToolBox*>(selectorToolBox())->process() == nullptr)
	{
		run();
	}

	if (static_cast<medRegistrationSelectorToolBox*>(selectorToolBox())->process() != nullptr)
	{
		return static_cast<medRegistrationSelectorToolBox*>(selectorToolBox())->process()->output();
	}
	else
	{
		return nullptr;
	}
}

bool itkProcessRegistrationILogDemonsToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerToolBox<itkProcessRegistrationILogDemonsToolBox>();
}

void itkProcessRegistrationILogDemonsToolBox::run(void)
{
    if(!this->parent())
        return;

	medRegistrationSelectorToolBox *toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());
    medAbstractRegistrationProcess *process;
    
    if (toolbox->process())
    {
        process = toolbox->process();
    }
    else
    {
        process = new itkProcessRegistrationILogDemons;
		toolbox->setProcess(process);
    }

    medAbstractData *fixedData  = toolbox->fixedData();
    medAbstractData *movingData = toolbox->movingData();

    qDebug()<<"AFTER";
    if (!fixedData || !movingData )
        return;

    // Many choices here
    itkProcessRegistrationILogDemons *process_Registration = dynamic_cast<itkProcessRegistrationILogDemons *>(process);

    process_Registration->setMinIterations(d->minIterationsBox->value());
    process_Registration->setMaximumUpdateStepLength(d->maximumUpdateStepLengthBox->value());
    process_Registration->setUpFieldSigma(d->upFieldSigmaBox->value());
    process_Registration->setUpFieldKappa(d->upFieldKappaBox->value());
    process_Registration->setVelFieldSigma(d->velFieldSigmaBox->value());
    process_Registration->setVelFieldKappa(d->velFieldKappaBox->value());
    process_Registration->setUseHistogramMatching(d->useHistogramMatchingBox->isChecked());
    process_Registration->setBCHExpansion(d->BCHExpansionBox->value());

    process_Registration->setSpatialDerivativeSigma(d->spatialDerivativeSigmaBox->value());
    process_Registration->setStepSize(d->stepSizeBox->value());
    process_Registration->setDTIStandardDeviation(d->DTIStandardDeviationBox->value());
    process_Registration->setNoiseEstimatorSigma(d->noiseEstimatorSigmaBox->value());
    process_Registration->setNoiseEstimatorLM(d->noiseEstimatorLMBox->isChecked());
    process_Registration->setNoiseEstimatorEpsilon(d->noiseEstimatorEpsilonBox->value());

    process_Registration->setGradientType( d->gradientTypeBox->currentIndex() );
    process_Registration->setUpFieldInc( d->upFieldIncBox->currentIndex() );
    process_Registration->setVelFieldInc( d->velFieldIncBox->currentIndex() );
    process_Registration->setNoiseEstimatorType( d->noiseEstimatorTypeBox->currentIndex() );
    if (d->dataMask)
    process_Registration->setMask(d->dataMask);
    try {
        process_Registration->setNumberOfIterations(rpi::StringToVector<unsigned int>(d->iterationsBox->text().toStdString()));
    }
    catch ( std::exception & )
    {
        qDebug() << "wrong iteration format";
        return;
    }
    try {
        process_Registration->setStop(rpi::StringToVector<float>(d->stopBox->text().toStdString()));
    }
    catch ( std::exception & )
    {
        qDebug() << "wrong iteration format";
        return;
    }
    // process->setMyWonderfullParameter(fronTheGui);
    // process->setMyWonderfullParameter(fronTheGui);

    // or ...

    // itkProcessRegistrationDiffeomorphicDemons *process = new itkProcessRegistrationDiffeomorphicDemons;
    // process->setMyWonderfullParameter(fronTheGui);
    // process->setMyWonderfullParameter(fronTheGui);

    // ----

    // process->setMyWonderfullParameter(fronTheGui);

    // or ...

    // itkProcessRegistrationILogDemons *process = new itkProcessRegistrationILogDemons;
    // process->setMyWonderfullParameter(fronTheGui);
    // process->setMyWonderfullParameter(fronTheGui);

    // ----

    if (!process)
        return;
    process->setFixedInput(fixedData);
    process->setMovingInput(movingData);
    qDebug() << "Desc : " << movingData->description();
    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (process);

    d->progression_stack->addJobItem(runProcess, "Progress:");
    d->progression_stack->setActive(runProcess,true);
    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));

    medJobManagerL::instance()->registerJobItem(runProcess,process->identifier());
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

medAbstractSelectableToolBox *createitkProcessRegistrationILogDemonsToolBox(QWidget* widget)
{
    return new itkProcessRegistrationILogDemonsToolBox(widget);
}

void itkProcessRegistrationILogDemonsToolBox::onMaskDropped(const medDataIndex& index)
{
    if (!index.isValid())
        return;

    d->dataMask = medDataManager::instance()->retrieveData(index);
}
void itkProcessRegistrationILogDemonsToolBox::onUseMaskStateChanged(int state)
{
    d->dropSiteMask->setVisible(!d->dropSiteMask->isVisible());
    d->maskLabel->setVisible(!d->maskLabel->isVisible());
}
