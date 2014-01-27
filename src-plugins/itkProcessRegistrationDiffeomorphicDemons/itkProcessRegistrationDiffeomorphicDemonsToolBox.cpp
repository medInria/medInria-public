/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessRegistrationDiffeomorphicDemons.h"
#include "itkProcessRegistrationDiffeomorphicDemonsToolBox.h"

#include <QtGui>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medAbstractDataImage.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractView.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medPluginManager.h>

#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medProgressionStack.h>

#include <rpiCommonTools.hxx>

class itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate
{
public:

    medProgressionStack * progression_stack;
    QComboBox * updateRuleBox;
    QComboBox * gradientTypeBox;
    QDoubleSpinBox * maxStepLengthBox;
    QDoubleSpinBox * disFieldStdDevBox;
    QDoubleSpinBox * updateFieldStdDevBox;
    QCheckBox * useHistogramBox;
    QLineEdit * iterationsBox;
};

itkProcessRegistrationDiffeomorphicDemonsToolBox::itkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parent) : medRegistrationAbstractToolBox(parent), d(new itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QPushButton *runButton = new QPushButton(tr("Run"), this);
    runButton->setToolTip(tr("Start Registration"));

    QFormLayout *layout = new QFormLayout(widget);

    d->iterationsBox = new QLineEdit(this);
    d->iterationsBox->setText("15x10x5");
    d->iterationsBox->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));

    d->maxStepLengthBox = new QDoubleSpinBox(this);
    d->maxStepLengthBox->setMinimum(0);
    d->maxStepLengthBox->setMaximum(1000);
    d->maxStepLengthBox->setSingleStep(0.01);
    d->maxStepLengthBox->setValue(2.0);
    d->maxStepLengthBox->setToolTip(tr(
                "Maximum length of an update vector (voxel units)."
                " Setting it to 0 implies no restrictions will be made"
                " on the step length."));

    d->updateFieldStdDevBox = new QDoubleSpinBox(this);
    d->updateFieldStdDevBox->setMinimum(0);
    d->updateFieldStdDevBox->setMaximum(1000);
    d->updateFieldStdDevBox->setSingleStep(0.01);
    d->updateFieldStdDevBox->setValue(0.0);
    d->updateFieldStdDevBox->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing"
                "of the update field (voxel units). Setting it below 0.1"
                "means no smoothing will be performed (default 0.0)."));

    d->disFieldStdDevBox = new QDoubleSpinBox(this);
    d->disFieldStdDevBox->setMinimum(0);
    d->disFieldStdDevBox->setMaximum(1000);
    d->disFieldStdDevBox->setSingleStep(0.01);
    d->disFieldStdDevBox->setValue(1.5);
    d->disFieldStdDevBox->setToolTip(tr(
                "Standard deviation of the Gaussian smoothing of "
                "the displacement field (voxel units). Setting it below 0.1 "
                "means no smoothing will be performed (default 1.5)."));
    d->updateRuleBox = new QComboBox(this);
    QStringList updateRules;
    updateRules<< tr("Diffeomorphic") << tr ("Additive") << tr("Compositive");
    d->updateRuleBox->addItems(updateRules);
    //WARNING IF YOU CHANGE THE ORDER OF THE ITEMS, CHANGE TOOLTIPS also!!!
    d->updateRuleBox->setItemData(0,"s <- s o exp(u)",Qt::ToolTipRole);
    d->updateRuleBox->setItemData(1,"s <- s + u",Qt::ToolTipRole);
    d->updateRuleBox->setItemData(2,"s <- s o (Id+u)",Qt::ToolTipRole);

    d->gradientTypeBox = new QComboBox(this);
    d->gradientTypeBox->setToolTip(tr(
                "Type of gradient used for computing the demons force."));
    QStringList gradientTypes;
    gradientTypes<< tr("Symmetrized") << tr ("Fixed Image")
                 << tr("Warped Moving Image")
                 << tr("Mapped Moving Image");
    d->gradientTypeBox->addItems(gradientTypes);
    d->useHistogramBox =  new QCheckBox(this);
    d->useHistogramBox->setChecked(false);
    d->useHistogramBox->setToolTip(tr(
                "Use histogram matching before processing?"));

    this->setTitle("Diffeomorphic Demons");
    layout->addRow(new QLabel(tr("Iterations per level of res."),this),d->iterationsBox);
    layout->addRow(new QLabel(tr("Update Rule"),this),d->updateRuleBox);
    layout->addRow(new QLabel(tr("Gradient Type"),this),d->gradientTypeBox);
    layout->addRow(new QLabel(tr("Max. Update Step Length"),this),d->maxStepLengthBox);
    layout->addRow(new QLabel(tr("Update Field Std. Deviation"),this),
                   d->updateFieldStdDevBox);
    layout->addRow(new QLabel(tr("Displ. Field Std. Deviation"),this),
                   d->disFieldStdDevBox);
    layout->addRow(new QLabel(tr("Histogram Matching"),this),d->useHistogramBox);

    // progression stack
    d->progression_stack = new medProgressionStack(widget);
//    QHBoxLayout *progressStackLayout = new QHBoxLayout;
//    progressStackLayout->addWidget(d->progression_stack);

    this->addWidget(widget);
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);

    //enable about plugin. Constructor called after the plugin has been registered, go ahead call it.
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin(
                "itkProcessRegistrationDiffeomorphicDemonsPlugin");
    setAboutPluginButton(plugin);
    setAboutPluginVisibility(true);

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
            registerToolBox<itkProcessRegistrationDiffeomorphicDemonsToolBox>(
                "itkProcessRegistrationDiffeomorphicDemonsToolBox",
                tr("Diffeomorphic Demons"),
                tr("Diffeomorphic demons registration using the ITK implementation"),
                QStringList()<< "registration");
}

void itkProcessRegistrationDiffeomorphicDemonsToolBox::run()
{

    if(!this->parentToolBox())
        return;
    dtkSmartPointer<dtkAbstractProcess> process;

    if (this->parentToolBox()->process() && (this->parentToolBox()->process()->identifier() == "itkProcessRegistrationDiffeomorphicDemons"))
    {
        process = this->parentToolBox()->process();
    }
    else
    {
        process = dtkAbstractProcessFactory::instance()->create("itkProcessRegistrationDiffeomorphicDemons");
        this->parentToolBox()->setProcess(process);
    }

    dtkSmartPointer<medAbstractData> fixedData(this->parentToolBox()->fixedData());
    dtkSmartPointer<medAbstractData> movingData(this->parentToolBox()->movingData());

    if (!fixedData || !movingData)
        return;

    // Many choices here

    itkProcessRegistrationDiffeomorphicDemons *process_Registration =
            dynamic_cast<itkProcessRegistrationDiffeomorphicDemons *>(process.data());
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

    d->progression_stack->addJobItem(runProcess, tr("Progress:"));
    d->progression_stack->setActive(runProcess,true);
    d->progression_stack->disableCancel(runProcess);
    connect (runProcess, SIGNAL (success  (QObject*)),  this, SIGNAL (success ()));
    connect (runProcess, SIGNAL (failure  (QObject*)),  this, SIGNAL (failure ()));
    connect (runProcess, SIGNAL (cancelled (QObject*)), this, SIGNAL (failure ()));
    //First have the moving progress bar,
    //and then display the remaining % when known
    connect (runProcess, SIGNAL(activate(QObject*,bool)),
             d->progression_stack, SLOT(setActive(QObject*,bool)));

    medJobManager::instance()->registerJobItem(runProcess,process->identifier());
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}
