/* itkProcessRegistrationDiffeoMorphicDemonsToolBox.cpp ---
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

#include "itkProcessRegistrationDiffeoMorphicDemons.h"
#include "itkProcessRegistrationDiffeoMorphicDemonsToolBox.h"

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>


#include <medCore/medAbstractView.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medJobManager.h>

#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxRegistration.h>
#include <medGui/medProgressionStack.h>


class itkProcessRegistrationDiffeoMorphicDemonsToolBoxPrivate
{
public:

    medProgressionStack * progression_stack;
    QComboBox * updateRuleBox;
    QComboBox * gradientTypeBox;
    QDoubleSpinBox * maxStepLengthBox;
    QDoubleSpinBox * disFieldStdDevBox;
    QDoubleSpinBox * updateFieldStdDevBox;
    QCheckBox * useHistogramBox;
};

itkProcessRegistrationDiffeoMorphicDemonsToolBox::itkProcessRegistrationDiffeoMorphicDemonsToolBox(QWidget *parent) : medToolBoxRegistrationCustom(parent), d(new itkProcessRegistrationDiffeoMorphicDemonsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QPushButton *runButton = new QPushButton(tr("Run"), this);

    QFormLayout *layout = new QFormLayout(widget);

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

    this->setTitle("itkProcessRegistrationDiffeoMorphicDemonsToolBox");
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
    QHBoxLayout *progressStackLayout = new QHBoxLayout;
    progressStackLayout->addWidget(d->progression_stack);

    this->addWidget(widget);
    this->addWidget(runButton);
    this->addWidget(d->progression_stack);

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

itkProcessRegistrationDiffeoMorphicDemonsToolBox::~itkProcessRegistrationDiffeoMorphicDemonsToolBox(void)
{
    delete d;

    d = NULL;
}

bool itkProcessRegistrationDiffeoMorphicDemonsToolBox::registered(void)
{
    return medToolBoxFactory::instance()->
            registerCustomRegistrationToolBox("itkProcessRegistrationDiffeoMorphicDemonsToolBox",
                                 createItkProcessRegistrationDiffeoMorphicDemonsToolBox);
}

void itkProcessRegistrationDiffeoMorphicDemonsToolBox::run(void)
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
        process = dtkAbstractProcessFactory::instance()->create("itkProcessRegistrationDiffeoMorphicDemons");
        this->parent()->setProcess(process);
    }
    dtkAbstractData *fixedData = this->parent()->fixedData();
    dtkAbstractData *movingData = this->parent()->movingData();


    if (!fixedData || !movingData)
        return;




    // Many choices here

    itkProcessRegistrationDiffeoMorphicDemons *process_Registration = dynamic_cast<itkProcessRegistrationDiffeoMorphicDemons *>(process);
//    process_Registration->setNumberOfHistogramBins(d->binsBox->value());


    // process->setMyWonderfullParameter(fronTheGui);
    // process->setMyWonderfullParameter(fronTheGui);

    // or ...

    // itkProcessRegistrationDiffeoMorphicDemons *process = new itkProcessRegistrationDiffeoMorphicDemons;
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

medToolBoxRegistrationCustom *createItkProcessRegistrationDiffeoMorphicDemonsToolBox(QWidget *parent)
{
    return new itkProcessRegistrationDiffeoMorphicDemonsToolBox (parent);
}
