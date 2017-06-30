/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkProcessRegistrationDiffeomorphicDemons.h>
#include <itkProcessRegistrationDiffeomorphicDemonsToolBox.h>

#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkSmartPointer.h>
#include <medJobManager.h>
#include <medPluginManager.h>
#include <medProgressionStack.h>
#include <medRegistrationSelectorToolBox.h>
#include <medRunnableProcess.h>
#include <medToolBoxFactory.h>
#include <rpiCommonTools.hxx>

class itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate
{
public:

    medProgressionStack * progressionStack;
    medComboBox * updateRuleBox;
    medComboBox * gradientTypeBox;
    QDoubleSpinBox * maxStepLengthBox;
    QDoubleSpinBox * disFieldStdDevBox;
    QDoubleSpinBox * updateFieldStdDevBox;
    QCheckBox * useHistogramBox;
    QLineEdit * iterationsBox;
    medAbstractRegistrationProcess *process;
};

itkProcessRegistrationDiffeomorphicDemonsToolBox::itkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* explanation = new QLabel("Drop 2 datasets with same size, origin and spacing.\n");
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    QFormLayout* formLayout = new QFormLayout();

    d->iterationsBox = new QLineEdit();
    d->iterationsBox->setText("15x10x5");
    d->iterationsBox->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));

    d->maxStepLengthBox = new QDoubleSpinBox();
    d->maxStepLengthBox->setMinimum(0);
    d->maxStepLengthBox->setMaximum(1000);
    d->maxStepLengthBox->setSingleStep(0.01);
    d->maxStepLengthBox->setValue(2.0);
    d->maxStepLengthBox->setToolTip(tr(
                "Maximum length of an update vector (voxel units)."
                " Setting it to 0 implies no restrictions will be made"
                " on the step length."));

    d->updateFieldStdDevBox = new QDoubleSpinBox();
    d->updateFieldStdDevBox->setMinimum(0);
    d->updateFieldStdDevBox->setMaximum(1000);
    d->updateFieldStdDevBox->setSingleStep(0.01);
    d->updateFieldStdDevBox->setValue(0.0);
    d->updateFieldStdDevBox->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing"
                "of the update field (voxel units). Setting it below 0.1"
                "means no smoothing will be performed (default 0.0)."));

    d->disFieldStdDevBox = new QDoubleSpinBox();
    d->disFieldStdDevBox->setMinimum(0);
    d->disFieldStdDevBox->setMaximum(1000);
    d->disFieldStdDevBox->setSingleStep(0.01);
    d->disFieldStdDevBox->setValue(1.5);
    d->disFieldStdDevBox->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing of "
                "the displacement field (voxel units). Setting it below 0.1 "
                "means no smoothing will be performed (default 1.5)."));
    d->updateRuleBox = new medComboBox();
    QStringList updateRules;
    updateRules<< tr("Diffeomorphic") << tr ("Additive") << tr("Compositive");
    d->updateRuleBox->addItems(updateRules);
    //WARNING IF YOU CHANGE THE ORDER OF THE ITEMS, CHANGE TOOLTIPS also!!!
    d->updateRuleBox->setItemData(0,"s <- s o exp(u)",Qt::ToolTipRole);
    d->updateRuleBox->setItemData(1,"s <- s + u",Qt::ToolTipRole);
    d->updateRuleBox->setItemData(2,"s <- s o (Id+u)",Qt::ToolTipRole);

    d->gradientTypeBox = new medComboBox();
    d->gradientTypeBox->setToolTip(tr(
                "Type of gradient used for computing the demons force."));
    QStringList gradientTypes;
    gradientTypes<< tr("Symmetrized") << tr ("Fixed Image")
                 << tr("Warped Moving Image")
                 << tr("Mapped Moving Image");
    d->gradientTypeBox->addItems(gradientTypes);
    d->useHistogramBox =  new QCheckBox();
    d->useHistogramBox->setChecked(false);
    d->useHistogramBox->setToolTip(tr(
                "Use histogram matching before processing?"));

    formLayout->addRow(new QLabel(tr("Iterations per level of res."),this),d->iterationsBox);
    formLayout->addRow(new QLabel(tr("Update Rule"),this),d->updateRuleBox);
    formLayout->addRow(new QLabel(tr("Gradient Type"),this),d->gradientTypeBox);
    formLayout->addRow(new QLabel(tr("Max. Update Step Length"),this),d->maxStepLengthBox);
    formLayout->addRow(new QLabel(tr("Update Field Std. Deviation"),this),
                   d->updateFieldStdDevBox);
    formLayout->addRow(new QLabel(tr("Displ. Field Std. Deviation"),this),
                   d->disFieldStdDevBox);
    formLayout->addRow(new QLabel(tr("Histogram Matching"),this),d->useHistogramBox);
    layout->addLayout(formLayout);

    // Run button
    QPushButton *runButton = new QPushButton(tr("Run"));
    runButton->setToolTip(tr("Start Registration"));
    layout->addWidget(runButton);

    // progression stack
    d->progressionStack = new medProgressionStack(widget);
    layout->addWidget(d->progressionStack);

    widget->setLayout(layout);
    this->addWidget(widget);
    this->addWidget(runButton);
    this->addWidget(d->progressionStack);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

itkProcessRegistrationDiffeomorphicDemonsToolBox::~itkProcessRegistrationDiffeomorphicDemonsToolBox()
{
    delete d;

    d = NULL;
}

bool itkProcessRegistrationDiffeomorphicDemonsToolBox::registered()
{
    return medToolBoxFactory::instance()->
            registerToolBox<itkProcessRegistrationDiffeomorphicDemonsToolBox>();
}

dtkPlugin* itkProcessRegistrationDiffeomorphicDemonsToolBox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin ( "Diffeomorphic Demons" );
    return plugin;
}


void itkProcessRegistrationDiffeomorphicDemonsToolBox::run()
{
    medRegistrationSelectorToolBox* toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());

    if(!toolbox)
        return;

    
    d->process = dynamic_cast<medAbstractRegistrationProcess*> (dtkAbstractProcessFactory::instance()->create("itkProcessRegistrationDiffeomorphicDemons"));
    if(!d->process)
        return;

    toolbox->setProcess(d->process);

    //TODO smartPointing have to be managed only in abstract processes -rde
    dtkSmartPointer<medAbstractData> fixedData(toolbox->fixedData());
    dtkSmartPointer<medAbstractData> movingData(toolbox->movingData());

    if (!fixedData || !movingData)
        return;

    // Many choices here

    itkProcessRegistrationDiffeomorphicDemons *process_Registration =
            dynamic_cast<itkProcessRegistrationDiffeomorphicDemons *>(d->process);
    if (!process_Registration)
    {
        qWarning() << "registration process doesn't exist" ;
        return;
    }
    process_Registration->setDisplacementFieldStandardDeviation(
                d->disFieldStdDevBox->value());
    process_Registration->setGradientType(d->gradientTypeBox->currentIndex());
    process_Registration->setUpdateRule(d->updateRuleBox->currentIndex());
    process_Registration->setUpdateFieldStandardDeviation(d->updateFieldStdDevBox->value());
    process_Registration->setMaximumUpdateLength(d->maxStepLengthBox->value());
    process_Registration->setUseHistogramMatching(d->useHistogramBox->isChecked());
    try {
        process_Registration->setNumberOfIterations(
                    rpi::StringToVector<unsigned int>(
                        d->iterationsBox->text().toStdString()));
    }
    catch ( std::exception & )
    {
        qDebug() << "wrong iteration format";
        return;
    }

    process_Registration->setFixedInput(fixedData);
    process_Registration->setMovingInput(movingData);

    medRunnableProcess *runProcess = new medRunnableProcess;

    runProcess->setProcess (d->process);

    d->progressionStack->addJobItem(runProcess, tr("Progress:"));
    d->progressionStack->setActive(runProcess,true);
    d->progressionStack->disableCancel(runProcess);
    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (failure  (int)),       this, SLOT   (handleDisplayError(int)));
    //First have the moving progress bar,
    //and then display the remaining % when known
    connect (runProcess, SIGNAL(activate(QObject*,bool)),
             d->progressionStack, SLOT(setActive(QObject*,bool)));

    medJobManager::instance()->registerJobItem(runProcess,d->process->identifier());
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

medAbstractData* itkProcessRegistrationDiffeomorphicDemonsToolBox::processOutput()
{
    // If called from pipelines, and run() not called before.
    if(!d->process)
    {
        run();
    }

    if(d->process)
    {
        return d->process->output();
    }
    else
    {
        return NULL;
    }
}
