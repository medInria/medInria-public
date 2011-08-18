#include "itkProcessiLogDemonRegistration.h"
#include "itkProcessiLogDemonRegistrationToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
//#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>


#include <medCore/medAbstractView.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medJobManager.h>

#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medProgressionStack.h>

#include <rpiCommonTools.hxx>

class itkProcessiLogDemonRegistrationToolBoxPrivate
{
public:

    medProgressionStack * progression_stack;
    QDoubleSpinBox * minIterationsBox;
    QDoubleSpinBox * maximumUpdateStepLengthBox;
    QComboBox * gradientTypeBox;
    QDoubleSpinBox * upFieldSigmaBox;
    QComboBox * upFieldIncBox;
    QDoubleSpinBox * upFieldKappaBox;
    QDoubleSpinBox * velFieldSigmaBox;
    QComboBox * velFieldIncBox;
    QDoubleSpinBox * velFieldKappaBox;
    QCheckBox * useHistogramMatchingBox;
    QDoubleSpinBox * BCHExpansionBox;
    QDoubleSpinBox * spatialDerivativeSigmaBox;
    QDoubleSpinBox * stepSizeBox;
    QDoubleSpinBox * DTIStandardDeviationBox;
    QComboBox * noiseEstimatorTypeBox;
    QDoubleSpinBox * noiseEstimatorSigmaBox;
    QDoubleSpinBox * noiseEstimatorLMBox;
    QDoubleSpinBox * noiseEstimatorEpsilonBox;
    QLineEdit * iterationsBox;
    QLineEdit * stopBox;
};

itkProcessiLogDemonRegistrationToolBox::itkProcessiLogDemonRegistrationToolBox(QWidget *parent) : medToolBoxRegistrationCustom(parent), d(new itkProcessiLogDemonRegistrationToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QPushButton *runButton = new QPushButton(tr("Run"), this);

    QFormLayout *layout = new QFormLayout(widget);

    d->iterationsBox = new QLineEdit(this);
    d->iterationsBox->setText("15x10x5");
    d->iterationsBox->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));

    d->stopBox = new QLineEdit(this);
    d->stopBox->setText("-1x-1x-1");
    d->stopBox->setToolTip(tr("List of threshold values for each multi-scale pyramid level (from coarse to fine levels). Negative values: stops when MSE increases. Positive values: stops when MSE is lower than the provided value."));

    d->minIterationsBox = new QDoubleSpinBox(this);
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


    d->velFieldKappaBox = new QDoubleSpinBox(this);
    d->velFieldKappaBox->setMinimum(0);
    d->velFieldKappaBox->setMaximum(1000);
    d->velFieldKappaBox->setSingleStep(0.01);
    d->velFieldKappaBox->setValue(0.0);
    d->velFieldKappaBox->setToolTip(tr("kappa value used for the elastic-like smoothing of the stationary velocity field"));

    d->useHistogramMatchingBox =  new QCheckBox(this);
    d->useHistogramMatchingBox->setChecked(false);

    d->BCHExpansionBox = new QDoubleSpinBox(this);
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

    d->stepSizeBox = new QDoubleSpinBox(this);
    d->stepSizeBox->setMinimum(0);
    d->stepSizeBox->setMaximum(1000);
    d->stepSizeBox->setSingleStep(1);
    d->stepSizeBox->setValue(5);
    d->stepSizeBox->setToolTip(tr("Step size (number of iterations) between two applications of the Hessian-norm constraint on the update field "));

    d->DTIStandardDeviationBox = new QDoubleSpinBox(this);
    d->DTIStandardDeviationBox->setMinimum(0);
    d->DTIStandardDeviationBox->setMaximum(1000);
    d->DTIStandardDeviationBox->setSingleStep(0.01);
    d->DTIStandardDeviationBox->setValue(0.0);
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

    d->noiseEstimatorLMBox =  new QCheckBox(this);
    d->noiseEstimatorLMBox->setChecked(false);
    
    d->noiseEstimatorEpsilonBox = new QDoubleSpinBox(this);
    d->noiseEstimatorEpsilonBox->setMinimum(0);
    d->noiseEstimatorEpsilonBox->setMaximum(1000);
    d->noiseEstimatorEpsilonBox->setSingleStep(0.01);
    d->noiseEstimatorEpsilonBox->setValue(0.0);
    d->noiseEstimatorEpsilonBox->setToolTip(tr("Epsilon value (image weight) used in the Levenberg-Marquardt method"));


    

    this->setTitle("Incompressible Log Demons");
    layout->addRow(new QLabel(tr("Iterations"),this),d->iterationsBox);
    layout->addRow(new QLabel(tr("Threshold Values"),this),d->stopBox);
    layout->addRow(new QLabel(tr("Min. Iteration"),this),d->minIterationsBox);
    layout->addRow(new QLabel(tr("Max. Update Step Length"),this),d->maximumUpdateStepLengthBox);
    layout->addRow(new QLabel(tr("Gradient Type"),this), d->gradientTypeBox);
    layout->addRow(new QLabel(tr("Std. Deviation of Gaussian Smoothing for the Update Field"),this), d->upFieldSigmaBox);
    layout->addRow(new QLabel(tr("Incompressibility of the Update Field"),this),d->upFieldIncBox);
    layout->addRow(new QLabel(tr("Kappa Value of the Update Field"),this),d->upFieldKappaBox);
    layout->addRow(new QLabel(tr("Std. Deviation of Gaussian Smoothing for the Stationary Velocity Field"),this),d->velFieldSigmaBox);
    layout->addRow(new QLabel(tr("Incompressibility of the Velocity Field"),this),d->velFieldIncBox);
    layout->addRow(new QLabel(tr("Kappa Value of the Velocity Field"),this),d->velFieldKappaBox);
    layout->addRow(new QLabel(tr("Histogram Matching"),this), d->useHistogramMatchingBox);
    layout->addRow(new QLabel(tr("BCH Expansion"),this), d->BCHExpansionBox);
    layout->addRow(new QLabel(tr("Std. Deviation of Spatial Derivative Smoothing in Incompressibility Constraint"),this),d->spatialDerivativeSigmaBox);
    layout->addRow(new QLabel(tr("Step Size"),this),d->stepSizeBox);
    layout->addRow(new QLabel(tr("Std. deviation for the DTI smoothing"),this),d->DTIStandardDeviationBox);
    layout->addRow(new QLabel(tr("Gradient Type"),this),d->noiseEstimatorTypeBox);
    layout->addRow(new QLabel(tr("Noise Estimator"),this),d->noiseEstimatorSigmaBox);
    layout->addRow(new QLabel(tr("Use Levenberg-Marquardt Noise Estimator"),this), d->noiseEstimatorLMBox);
    layout->addRow(new QLabel(tr("Image Weight in Levenberg-Marquardt"),this), d->noiseEstimatorEpsilonBox);



    // progression stack
    d->progression_stack = new medProgressionStack(widget);
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);

    this->addWidget(widget);
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}
itkProcessiLogDemonRegistrationToolBox::~itkProcessiLogDemonRegistrationToolBox(void)
{
    delete d;
    d = NULL;
}
bool itkProcessiLogDemonRegistrationToolBox::registered(void)
{
    return medToolBoxFactory::instance()->
            registerCustomRegistrationToolBox("itkProcessiLogDemonRegistrationToolBox",
                                 createItkProcessiLogDemonRegistrationToolBox);
}
void itkProcessiLogDemonRegistrationToolBox::run(void)
{
    if(!this->parent())
        return;
    dtkAbstractProcess * process;

    if (this->parent()->process())
    {
        process = this->parent()->process();

    }
    else
    {
        process = dtkAbstractProcessFactory::instance()->create("itkProcessiLogDemonRegistration");
        this->parent()->setProcess(process);
    }
    dtkAbstractData *fixedData = this->parent()->fixedData();
    dtkAbstractData *movingData = this->parent()->movingData();


    if (!fixedData || !movingData)
        return;




    // Many choices here

    itkProcessiLogDemonRegistration *process_Registration = dynamic_cast<itkProcessiLogDemonRegistration *>(process);
    
    process_Registration->SetMinimumNumberOfIterations(d->minIterationsBox->value());
    process_Registration->SetMaximumUpdateStepLength(d->maximumUpdateStepLengthBox->value());
    process_Registration->SetUpdateFieldStandardDeviation(d->upFieldSigmaBox->value());
    process_Registration->SetUpdateFieldKappa(d->upFieldKappaBox->value());
    process_Registration->SetStationaryVelocityFieldStandardDeviation(d->velFieldSigmaBox->value());
    process_Registration->SetStationaryVelocityFieldKappa(d->velFieldKappaBox->value());
    process_Registration->SetUseHistogramMatching(d->useHistogramMatchingBox->isChecked());
    process_Registration->SetNumberOfTermsBCHExpansion(d->BCHExpansionBox->value());
    
    process_Registration->SetSpatialDerivativeStandardDeviation(d->spatialDerivativeSigmaBox->value());
    process_Registration->SetStepSize(d->stepSizeBox->value());
    process_Registration->SetDTIStandardDeviation(d->DTIStandardDeviationBox->value());
    process_Registration->SetNoiseEstimatorStandardDeviation(d->noiseEstimatorSigmaBox->value());
    process_Registration->SetNoiseEstimatorLM(d->noiseEstimatorLMBox->ischecked);
    process_Registration->SetNoiseEstimatorEpsilon(d->noiseEstimatorEpsilonBox->value());

    process_Registration->SetGradientType( d->gradientTypeBox->currentIndex() );
    process_Registration->SetUpdateFieldIncompressibility( d->upFieldIncBox->currentIndex() );
    process_Registration->SetStationaryVelocityFieldIncompressibility( d->velFieldIncBox->currentIndex() );
    process_Registration->SetNoiseEstimatorType( d->noiseEstimatorTypeBox->currentIndex() );




   
    try {
        process_Registration->SetNumberOfIterations(rpi::StringToVector<unsigned int>(d->iterationsBox->text().toStdString()));
    }
    catch ( std::exception & )
    {
        qDebug() << "wrong iteration format";
        return;
    }
    try {
        process_Registration->SetStopThreshold(rpi::StringToVector<unsigned int>(d->stopBox->text().toStdString()));
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

    process->setInput(fixedData,  0);
    process->setInput(movingData, 1);

    medRunnableProcess *runProcess = new medRunnableProcess;
    runProcess->setProcess (process);

    d->progression_stack->addJobItem(runProcess, "Progress:");

    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));

    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
    
}
medToolBoxRegistrationCustom *createItkProcessiLogDemonRegistrationToolBox(QWidget *parent)
{
    return new itkProcessiLogDemonRegistrationToolBox (parent);
}