/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "LCCLogDemons.h"
#include "LCCLogDemonsToolBox.h"

#include <QtGui>

#include <medPluginManager.h>
#include <medAbstractDataFactory.h>
#include <medAbstractData.h>
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <medAbstractRegistrationProcess.h>

#include <medAbstractView.h>
#include <medRunnableProcess.h>
#include <medJobManagerL.h>

#include <medAbstractImageData.h>

#include <medToolBoxFactory.h>
#include <medRegistrationSelectorToolBox.h>
#include <medProgressionStack.h>
#include <medGroupBox.h>

#include <rpiCommonTools.hxx>

class LCCLogDemonsToolBoxPrivate
{
public:

    QWidget *logWidget, *LCCWidget, *commonWidget;
    QLineEdit * iterationsLine;
    QComboBox * updateRuleComboBox, * gradientTypeComboBox, * interpolatorTypeComboBox;
    QSpinBox * bchExpansionSpinBox;
    QDoubleSpinBox *sigmaISpinBox, *similaritySigmaSpinBox, *updateFieldSigmaSpinBox,
        *velocityFieldSigmaSpinBox, *maxStepLengthSpinBox;
    QCheckBox * boundaryCheckBox, *histoMatchingCheckBox;
    int updateRule;
    medAbstractRegistrationProcess *process;
};

LCCLogDemonsToolBox::LCCLogDemonsToolBox(QWidget *parent) : medAbstractSelectableToolBox(parent), d(new LCCLogDemonsToolBoxPrivate)
{
    QWidget *widget = new QWidget(this);

    QVBoxLayout * layout = new QVBoxLayout();

    QLabel *explanation = new QLabel("Drop 2 datasets with same size and spacing.\n");
    explanation->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    explanation->setWordWrap(true);
    explanation->setStyleSheet("font: italic");
    layout->addWidget(explanation);

    // Standard parameters
    d->iterationsLine = new QLineEdit();
    d->iterationsLine->setText("15x10x5");
    d->iterationsLine->setToolTip(tr("Each number of iteration per level must be separated by \"x\". From coarser to finest levels"));
    QLabel * iterationsLabel = new QLabel("Iterations per level  of res.");
    QHBoxLayout * iterationsLayout = new QHBoxLayout();
    iterationsLayout->addWidget(iterationsLabel);
    iterationsLayout->addWidget(d->iterationsLine);

    d->updateRuleComboBox =  new QComboBox();
    d->updateRuleComboBox->insertItem(0, "SSD Non Symmetric Log Domain");
    d->updateRuleComboBox->insertItem(1, "SSD Symmetric Log Domain");
    d->updateRuleComboBox->insertItem(2, "LCC");
    connect ( d->updateRuleComboBox, SIGNAL ( activated ( int ) ), this, SLOT ( chooseUpdateRule ( int ) ) );

    // Parameters for Log

    QLabel * maxStepLengthLabel = new QLabel("Maximum length of an update vector");
    d->maxStepLengthSpinBox = new QDoubleSpinBox();
    d->maxStepLengthSpinBox->setValue(2.0);
    QHBoxLayout * maxStepLengthLayout = new QHBoxLayout();
    maxStepLengthLayout->addWidget(maxStepLengthLabel);
    maxStepLengthLayout->addWidget(d->maxStepLengthSpinBox);

    QLabel * gradientTypeLabel = new QLabel("Gradient Type");
    d->gradientTypeComboBox = new QComboBox();
    d->gradientTypeComboBox->insertItem(0, "Symmetrized");
    d->gradientTypeComboBox->insertItem(1, "Fixed image");
    d->gradientTypeComboBox->insertItem(2, "Warped moving image");
    d->gradientTypeComboBox->insertItem(3, "Mapped moving image");
    QHBoxLayout * gradientTypeLayout = new QHBoxLayout();
    gradientTypeLayout->addWidget(gradientTypeLabel);
    gradientTypeLayout->addWidget(d->gradientTypeComboBox);

    QLabel * histoMatchingLabel = new QLabel("Use Histogram Matching");
    d->histoMatchingCheckBox = new QCheckBox();
    d->histoMatchingCheckBox->setChecked(true);
    QHBoxLayout * histoMatchingLayout  = new QHBoxLayout();
    histoMatchingLayout->addWidget(histoMatchingLabel);
    histoMatchingLayout->addWidget(d->histoMatchingCheckBox);

    // Parameters for LCC
    
    QLabel * boundaryLabel = new QLabel("Boundary");
    d->boundaryCheckBox = new QCheckBox();
    d->boundaryCheckBox->setChecked(true);
    QHBoxLayout * boundaryLayout  = new QHBoxLayout();
    boundaryLayout->addWidget(boundaryLabel);
    boundaryLayout->addWidget(d->boundaryCheckBox);

    QLabel * sigmaILabel = new QLabel("Sigma I");
    d->sigmaISpinBox = new QDoubleSpinBox();
    d->sigmaISpinBox->setValue(0.2);
    QHBoxLayout * sigmaILayout = new QHBoxLayout();
    sigmaILayout->addWidget(sigmaILabel);
    sigmaILayout->addWidget(d->sigmaISpinBox);

    QLabel * similarityLabel = new QLabel("Similarity Criteria Sigma");
    d->similaritySigmaSpinBox = new QDoubleSpinBox();
    d->similaritySigmaSpinBox->setValue(3.0);
    QHBoxLayout * similarityLayout = new QHBoxLayout();
    similarityLayout->addWidget(similarityLabel);
    similarityLayout->addWidget(d->similaritySigmaSpinBox);

    // Common parameters

    QLabel * updateFieldSigmaLabel = new QLabel("Update Field Sigma");
    d->updateFieldSigmaSpinBox = new QDoubleSpinBox();
    d->updateFieldSigmaSpinBox->setValue(0.0);
    QHBoxLayout * updateFieldSigmaLayout = new QHBoxLayout();
    updateFieldSigmaLayout->addWidget(updateFieldSigmaLabel);
    updateFieldSigmaLayout->addWidget(d->updateFieldSigmaSpinBox);

    QLabel * velocityFieldSigmaLabel = new QLabel("Velocity Field Sigma");
    d->velocityFieldSigmaSpinBox = new QDoubleSpinBox();
    d->velocityFieldSigmaSpinBox->setValue(1.5);
    QHBoxLayout * velocityFieldSigmaLayout = new QHBoxLayout();
    velocityFieldSigmaLayout->addWidget(velocityFieldSigmaLabel);
    velocityFieldSigmaLayout->addWidget(d->velocityFieldSigmaSpinBox);

    QLabel * bchExpansionLabel = new QLabel("BCH Expansion");
    d->bchExpansionSpinBox = new QSpinBox();
    d->bchExpansionSpinBox->setValue(2);
    QHBoxLayout * bchExpansionLayout = new QHBoxLayout();
    bchExpansionLayout->addWidget(bchExpansionLabel);
    bchExpansionLayout->addWidget(d->bchExpansionSpinBox);

    QLabel * interpolatorTypeLabel = new QLabel("Interpolator Type");
    d->interpolatorTypeComboBox = new QComboBox();
    d->interpolatorTypeComboBox->insertItem(0, "Nearest Neighbor");
    d->interpolatorTypeComboBox->insertItem(1, "Linear");
    d->interpolatorTypeComboBox->insertItem(2, "B-Spline");
    d->interpolatorTypeComboBox->insertItem(3, "Sinus Cardinal");
    QHBoxLayout * interpolatorTypeLayout = new QHBoxLayout();
    interpolatorTypeLayout->addWidget(interpolatorTypeLabel);
    interpolatorTypeLayout->addWidget(d->interpolatorTypeComboBox);
    d->interpolatorTypeComboBox->setCurrentIndex(1);

    QPushButton *runButton = new QPushButton(tr("Run"), this);

    // Layouts

    QVBoxLayout * logLayout = new QVBoxLayout();
    logLayout->addLayout(maxStepLengthLayout);
    logLayout->addLayout(gradientTypeLayout);
    logLayout->addLayout(histoMatchingLayout);
    d->logWidget = new QWidget(this);
    d->logWidget->setLayout(logLayout);

    QVBoxLayout * LCCLayout = new QVBoxLayout();
    LCCLayout->addLayout(boundaryLayout);
    LCCLayout->addLayout(sigmaILayout);
    LCCLayout->addLayout(similarityLayout);
    d->LCCWidget = new QWidget(this);
    d->LCCWidget->setLayout(LCCLayout);

    QVBoxLayout * commonLayout = new QVBoxLayout();
    commonLayout->addLayout(updateFieldSigmaLayout);
    commonLayout->addLayout(velocityFieldSigmaLayout);
    commonLayout->addLayout(bchExpansionLayout);
    commonLayout->addLayout(interpolatorTypeLayout);
    d->commonWidget = new QWidget(this);
    d->commonWidget->setLayout(commonLayout);

    // Main layout
    layout->addLayout(iterationsLayout);
    layout->addWidget(d->updateRuleComboBox);
    layout->addWidget(d->LCCWidget);
    layout->addWidget(d->logWidget);
    layout->addWidget(d->commonWidget);
    layout->addWidget(runButton);

    widget->setLayout(layout);
    this->addWidget(widget);
    
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
    d->updateRuleComboBox->setCurrentIndex(2);
    chooseUpdateRule(2);

    d->process = nullptr;
}

LCCLogDemonsToolBox::~LCCLogDemonsToolBox()
{
    delete d;
    d = nullptr;
}

dtkPlugin * LCCLogDemonsToolBox::plugin()
{
    return medPluginManager::instance().plugin("LCCLogDemonsPlugin");
}

medAbstractData * LCCLogDemonsToolBox::processOutput()
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

    return nullptr;
}

bool LCCLogDemonsToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<LCCLogDemonsToolBox>();
}

void LCCLogDemonsToolBox::chooseUpdateRule(int choice)
{
    d->LCCWidget->hide();
    d->logWidget->hide();
    d->commonWidget->hide();

    if(choice == 0)
    {
        d->logWidget->show();
        d->commonWidget->show();
        d->updateRule = 1;
    }
    else if (choice == 1)
    {
        d->logWidget->show();
        d->commonWidget->show();
        d->updateRule = 1;
    }
    else if (choice == 2)
    {
        d->LCCWidget->show();
        d->commonWidget->show();
        d->updateRule = 2;
    }
}

void LCCLogDemonsToolBox::run()
{
    medRegistrationSelectorToolBox *toolbox = dynamic_cast<medRegistrationSelectorToolBox*>(selectorToolBox());
    if(toolbox) // toolbox empty in Pipelines and not Registration workspace
    {
        d->process = dynamic_cast<medAbstractRegistrationProcess*>
                (dtkAbstractProcessFactory::instance()->create("LCCLogDemons"));
        toolbox->setProcess(d->process);

        medAbstractData *fixedData  = toolbox->fixedData();
        medAbstractData *movingData = toolbox->movingData();

        if (fixedData && movingData)
        {
            this->setToolBoxOnWaitStatus();

            LCCLogDemons *process_Registration = dynamic_cast<LCCLogDemons *>(d->process);

            process_Registration->setUpdateRule(d->updateRule);
            process_Registration->setVerbosity(false);
            process_Registration->setMaximumUpdateStepLength(d->maxStepLengthSpinBox->value());
            process_Registration->setGradientType(d->gradientTypeComboBox->currentIndex());
            process_Registration->setUseHistogramMatching(d->histoMatchingCheckBox->isChecked());
            process_Registration->setBoundaryCheck(d->boundaryCheckBox->isChecked());
            process_Registration->setSigmaI(d->sigmaISpinBox->value());
            process_Registration->setSimilarityCriteriaSigma(d->similaritySigmaSpinBox->value());
            process_Registration->setUpdateFieldSigma(d->updateFieldSigmaSpinBox->value());
            process_Registration->setVelocityFieldSigma(d->velocityFieldSigmaSpinBox->value());
            process_Registration->setNumberOfTermsBCHExpansion(
                        static_cast<unsigned int>(d->bchExpansionSpinBox->value()));
            process_Registration->useMask(false); // LCC-LogDemons crashs if set to true
            process_Registration->setInterpolatorType(d->interpolatorTypeComboBox->currentIndex());

            try
            {
                process_Registration->setNumberOfIterations(
                            rpi::StringToVector<unsigned int>(
                                d->iterationsLine->text().toStdString()));
            }
            catch (std::exception& err)
            {
                qDebug()<<"ExceptionObject caught in "<<metaObject()->className();
                qDebug()<<err.what();
                this->setToolBoxOnReadyToUse();
                return;
            }

            process_Registration->setFixedInput(fixedData);
            process_Registration->setMovingInput(movingData);

            medRunnableProcess *runProcess = new medRunnableProcess;
            runProcess->setProcess (d->process);
            this->addConnectionsAndStartJob(runProcess);
            enableOnProcessSuccessImportOutput(runProcess, false);
        }
    }
}
