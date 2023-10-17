/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "itkProcessRegistrationOptimus.h"
#include "itkProcessRegistrationOptimusToolBox.h"

#include <QtGui>

#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <medAbstractRegistrationProcess.h>

#include <medRunnableProcess.h>
#include <medJobManagerL.h>
#include <medAbstractImageData.h>

#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medProgressionStack.h>
#include <medPluginManager.h>
#include <rpiCommonTools.hxx>

class itkProcessRegistrationOptimusToolBoxPrivate
{
public:
    QSpinBox * iterationsBox;
    QSpinBox * binsBox;
    QSpinBox * samplesBox;
    QSpinBox * interpolationsBox;
    QDoubleSpinBox * rhoStartBox;
    QDoubleSpinBox * rhoEndBox;
    QDoubleSpinBox * scalingCoeffBox;
    QComboBox * interpolatorTypeBox;
    medAbstractRegistrationProcess * process;
};

itkProcessRegistrationOptimusToolBox::itkProcessRegistrationOptimusToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new itkProcessRegistrationOptimusToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QPushButton *runButton = new QPushButton(tr("Run"), this);
    runButton->setToolTip(tr("Start Registration"));

    QFormLayout *layout = new QFormLayout(widget);

    d->iterationsBox = new QSpinBox(widget);
    d->iterationsBox->setMinimum(0);
    d->iterationsBox->setMaximum(100000);
    d->iterationsBox->setSingleStep(1);
    d->iterationsBox->setValue(10000);
    d->iterationsBox->setToolTip(tr("Maximum number of iterations "
                                    "Must be greater than or equal to "
                                    "the number of interpolations"));

    d->binsBox = new QSpinBox(widget);
    d->binsBox->setMinimum(0);
    d->binsBox->setMaximum(100000);
    d->binsBox->setSingleStep(1);
    d->binsBox->setValue(100);
    d->binsBox->setToolTip(tr("Number of histogram bins"));

    d->samplesBox = new QSpinBox(widget);
    d->samplesBox->setMinimum(0);
    d->samplesBox->setMaximum(1000000);
    d->samplesBox->setSingleStep(1);
    d->samplesBox->setValue(10000);
    d->samplesBox->setToolTip(tr("Number of spatial samples"));

    //Not used now, we use the default
//    d->interpolatorTypeBox = new QComboBox(widget);
//    d->interpolatorTypeBox->addItem(tr("nearest neighbor"),"nearest neighbor");
//    d->interpolatorTypeBox->addItem(tr("linear"),"linear");
//    d->interpolatorTypeBox->addItem(tr("b-spline"),"b-spline");
//    d->interpolatorTypeBox->addItem(tr("sinc"),"sinc");

    d->interpolationsBox = new QSpinBox(widget);
    d->interpolationsBox->setMinimum(0);
    d->interpolationsBox->setMaximum(100000);
    d->interpolationsBox->setSingleStep(1);
    d->interpolationsBox->setValue(28);
    d->interpolationsBox->setToolTip(
                tr("Set the number of interpolation points"));

    d->rhoStartBox = new QDoubleSpinBox(widget);
    d->rhoStartBox->setMinimum(0.0);
    d->rhoStartBox->setSingleStep(0.1);
    d->rhoStartBox->setValue(0.6);
    d->rhoStartBox->setToolTip(
                tr("Starting value of rho (See reference on the algorithm "
                   "for more information)"));

    d->rhoEndBox = new QDoubleSpinBox(widget);
    d->rhoEndBox->setMinimum(0.0);
    d->rhoEndBox->setSingleStep(0.01);
    d->rhoEndBox->setDecimals(4);
    d->rhoEndBox->setValue(0.003);
    d->rhoEndBox->setToolTip(
                tr("Stopping value of rho (See reference on the algorithm "
                   "for more information)"));

    d->scalingCoeffBox = new QDoubleSpinBox(widget);
    d->scalingCoeffBox->setMinimum(0.0);
    d->scalingCoeffBox->setSingleStep(0.1);
    d->scalingCoeffBox->setValue(50.0);
    d->scalingCoeffBox->setToolTip(tr("Scaling coefficient"));

    this->setTitle(tr("Optimus"));
    layout->addRow(new QLabel(tr("Max. number of iterations"),widget),
                   d->iterationsBox);

    layout->addRow(new QLabel(tr("Number of Histogram bins"),widget),d->binsBox);
    layout->addRow(new QLabel(tr("Number of samples"),widget),d->samplesBox);
    //Not used now
//    layout->addRow(new QLabel(tr("Interpolation Type"),widget),
//                   d->interpolatorTypeBox);
    layout->addRow(new QLabel(tr("Number of interpolations"),widget),
                   d->interpolationsBox);
    layout->addRow(new QLabel(tr("Rho start"),widget),d->rhoStartBox);
    layout->addRow(new QLabel(tr("Rho end"),widget),d->rhoEndBox);
    layout->addRow(new QLabel(tr("Scaling coefficient"),widget),
                   d->scalingCoeffBox);


    this->addWidget(widget);
    this->addWidget(runButton);

    d->process = nullptr;

    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
}

itkProcessRegistrationOptimusToolBox::~itkProcessRegistrationOptimusToolBox(void)
{
    delete d;

    d = nullptr;
}

dtkPlugin * itkProcessRegistrationOptimusToolBox::plugin()
{
    return medPluginManager::instance().plugin("itkProcessRegistrationOptimusPlugin");
}

medAbstractData * itkProcessRegistrationOptimusToolBox::processOutput()
{
    // If called from pipelines, and run() not called before.
    if (!d->process)
    {
        run();
    }

    if (d->process)
    {
        return d->process->output();
    }

    return nullptr;
}

bool itkProcessRegistrationOptimusToolBox::registered(void)
{
    return medToolBoxFactory::instance()->registerToolBox<itkProcessRegistrationOptimusToolBox>();
}

void itkProcessRegistrationOptimusToolBox::run(void)
{
    medRegistrationSelectorToolBox *toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());
    if (toolbox)
    {
        if (toolbox->process() && (toolbox->process()->identifier() == "itkProcessRegistrationOptimus"))
        {
            d->process = toolbox->process();

        }
        else
        {
            d->process = new itkProcessRegistrationOptimus;
            toolbox->setProcess(d->process);
        }
        medAbstractData *fixedData = toolbox->fixedData();
        medAbstractData *movingData = toolbox->movingData();

        if (!fixedData || !movingData)
            return;

        // Many choices here

        itkProcessRegistrationOptimus *process_Registration = dynamic_cast<itkProcessRegistrationOptimus *>(d->process);
        if (!process_Registration)
        {
            qWarning() << "registration process doesn't exist";
            return;
        }
        process_Registration->setNumberOfHistogramBins(d->binsBox->value());
        process_Registration->setNumberOfInterpolations(d->interpolationsBox->value());
        process_Registration->setNumberOfIterations(d->iterationsBox->value());
        process_Registration->setNumberOfSpatialSamples(d->samplesBox->value());
        process_Registration->setRhoEnd(d->rhoEndBox->value());
        process_Registration->setRhoStart(d->rhoStartBox->value());
        process_Registration->setScalingCoefficient(d->scalingCoeffBox->value());


        // process->setMyWonderfullParameter(fronTheGui);
        // process->setMyWonderfullParameter(fronTheGui);

        // or ...

        // itkProcessRegistrationOptimus *process = new itkProcessRegistrationOptimus;
        // process->setMyWonderfullParameter(fronTheGui);
        // process->setMyWonderfullParameter(fronTheGui);

        // ----

        d->process->setFixedInput(fixedData);
        d->process->setMovingInput(movingData);

        medRunnableProcess *runProcess = new medRunnableProcess;
        runProcess->setProcess(d->process);

        setToolBoxOnWaitStatus();

        medJobManagerL::instance().registerJobItem(runProcess, d->process->identifier());

        addConnectionsAndStartJob(runProcess);
        enableOnProcessSuccessImportOutput(runProcess, false);
    }
}

