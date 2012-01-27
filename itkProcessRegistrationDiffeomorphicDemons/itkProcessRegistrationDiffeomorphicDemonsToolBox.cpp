/* itkProcessRegistrationDiffeomorphicDemonsToolBox.cpp ---
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

#include "itkProcessRegistrationDiffeomorphicDemons.h"
#include "itkProcessRegistrationDiffeomorphicDemonsToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <medAbstractDataImage.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>


#include <medAbstractView.h>
#include <medRunnableProcess.h>
#include <medJobManager.h>
#include <medPluginManager.h>

#include <medToolBoxFactory.h>
#include <medToolBoxRegistration.h>
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

itkProcessRegistrationDiffeomorphicDemonsToolBox::itkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parent) : medToolBoxRegistrationCustom(parent), d(new itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QPushButton *runButton = new QPushButton(tr("Run"), this);

    QFormLayout *layout = new QFormLayout(widget);

    d->iterationsBox = new QLineEdit(this);
    d->iterationsBox->setText("15x10x5");
    d->iterationsBox->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));

    d->maxStepLengthBox = new QDoubleSpinBox(this);
    d->maxStepLengthBox->setMinimum(0);
    d->maxStepLengthBox->setMaximum(1000);
    d->maxStepLengthBox->setSingleStep(0.01);
    d->maxStepLengthBox->setValue(2.0);

    d->updateFieldStdDevBox = new QDoubleSpinBox(this);
    d->updateFieldStdDevBox->setMinimum(0);
    d->updateFieldStdDevBox->setMaximum(1000);
    d->updateFieldStdDevBox->setSingleStep(0.01);
    d->updateFieldStdDevBox->setValue(0.0);

    d->disFieldStdDevBox = new QDoubleSpinBox(this);
    d->disFieldStdDevBox->setMinimum(0);
    d->disFieldStdDevBox->setMaximum(1000);
    d->disFieldStdDevBox->setSingleStep(0.01);
    d->disFieldStdDevBox->setValue(1.5);

    d->updateRuleBox = new QComboBox(this);
    QStringList updateRules;
    updateRules<< tr("Diffeomorphic") << tr ("Additive") << tr("Compositive");
    d->updateRuleBox->addItems(updateRules);

    d->gradientTypeBox = new QComboBox(this);
    QStringList gradientTypes;
    gradientTypes<< tr("Symmetrized") << tr ("Fixed Image") << tr("Warped Moving Image")
                 << tr("Mapped Moving Image");
    d->gradientTypeBox->addItems(gradientTypes);

    d->useHistogramBox =  new QCheckBox(this);
    d->useHistogramBox->setChecked(false);

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

    //enable about plugin. Construtor called after the plugin has been registered, go ahead call it.
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin(
                "itkProcessRegistrationDiffeomorphicDemonsPlugin");
    setAboutPluginButton(plugin);
    setAboutPluginVisibility(true);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

itkProcessRegistrationDiffeomorphicDemonsToolBox::~itkProcessRegistrationDiffeomorphicDemonsToolBox(void)
{
    delete d;

    d = NULL;
}

bool itkProcessRegistrationDiffeomorphicDemonsToolBox::registered(void)
{
    return medToolBoxFactory::instance()->
            registerCustomRegistrationToolBox(
                "itkProcessRegistrationDiffeomorphicDemonsToolBox",
                "Diffeomorphic Demons",
                "Diffeomorphic demons registration using the ITK implementation",
                createItkProcessRegistrationDiffeomorphicDemonsToolBox);
}

void itkProcessRegistrationDiffeomorphicDemonsToolBox::run(void)
{

    if(!this->parentToolBox())
        return;
    dtkAbstractProcess * process;

    if (this->parentToolBox()->process() && (this->parentToolBox()->process()->identifier() == "itkProcessRegistrationDiffeomorphicDemons"))
    {
        process = this->parentToolBox()->process();
    }
    else
    {
        process = dtkAbstractProcessFactory::instance()->create("itkProcessRegistrationDiffeomorphicDemons");
        this->parentToolBox()->setProcess(process);
    }

    dtkAbstractData *fixedData = this->parentToolBox()->fixedData();
    dtkAbstractData *movingData = this->parentToolBox()->movingData();


    if (!fixedData || !movingData)
        return;




    // Many choices here

    itkProcessRegistrationDiffeomorphicDemons *process_Registration =
            dynamic_cast<itkProcessRegistrationDiffeomorphicDemons *>(process);
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

    medJobManager::instance()->registerJobItem(runProcess);
    QThreadPool::globalInstance()->start(dynamic_cast<QRunnable*>(runProcess));
}

medToolBoxRegistrationCustom *createItkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parent)
{
    return new itkProcessRegistrationDiffeomorphicDemonsToolBox (parent);
}
