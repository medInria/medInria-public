/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDiffusionModelEstimationMetaProcessPresenter.h>

#include <QComboBox>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>

#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>

#include <medCore.h>
#include <medWidgets.h>
#include <medIntParameterPresenter.h>
#include <medAbstractDiffusionModelEstimationProcessPresenter.h>
#include <medAbstractDWIMaskingProcessPresenter.h>
#include <medAbstractMaskImageProcessPresenter.h>

class medDiffusionModelEstimationMetaProcessPresenterPrivate
{
public:
    medDiffusionModelEstimationMetaProcess *process;

    medAbstractDiffusionModelEstimationProcessPresenter *modelEstimationPresenter;
    QGroupBox *modelEstimationWidget;
    medAbstractDWIMaskingProcessPresenter *dwiMaskCalculatorPresenter;
    QGroupBox *dwiMaskCalculatorWidget;
    medIntParameterPresenter *progressionPresenter;

    QComboBox *dwiMaskingComboBox;
    QComboBox *modelEstimationComboBox;
};

medDiffusionModelEstimationMetaProcessPresenter::medDiffusionModelEstimationMetaProcessPresenter(medDiffusionModelEstimationMetaProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medDiffusionModelEstimationMetaProcessPresenterPrivate)
{
    d->process = parent;
    d->progressionPresenter = new medIntParameterPresenter(d->process->progression());

    connect(d->process, &medDiffusionModelEstimationMetaProcess::finished,
            this, &medDiffusionModelEstimationMetaProcessPresenter::_importOutput,
            Qt::QueuedConnection);

    d->modelEstimationPresenter = nullptr;
    d->dwiMaskCalculatorPresenter = nullptr;
    d->dwiMaskCalculatorWidget = nullptr;
    d->modelEstimationWidget = nullptr;
}

medDiffusionModelEstimationMetaProcessPresenter::~medDiffusionModelEstimationMetaProcessPresenter()
{

}

medDiffusionModelEstimationMetaProcess *medDiffusionModelEstimationMetaProcessPresenter::process() const
{
    return d->process;
}

QWidget *medDiffusionModelEstimationMetaProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    QLabel *dwiMaskingLabel = new QLabel(tr("DWI masking"));
    QHBoxLayout *dwiMaskingComboLayout = new QHBoxLayout;
    dwiMaskingComboLayout->addWidget(dwiMaskingLabel);
    d->dwiMaskingComboBox = new QComboBox;
    d->dwiMaskingComboBox->addItem("None");
    for(QString maskingName : d->process->dwiMaskingPlugins())
    {
        medAbstractDWIMaskingProcess *tmpProcess = medCore::dwiMasking::pluginFactory().create(maskingName);
        d->dwiMaskingComboBox->addItem(tmpProcess->caption(),maskingName);
    }

    dwiMaskingComboLayout->addWidget(d->dwiMaskingComboBox);

    connect(d->dwiMaskingComboBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(chooseDWIMaskingPlugin(int)));

    tbLayout->addLayout(dwiMaskingComboLayout);

    d->dwiMaskCalculatorWidget = new QGroupBox;
    tbLayout->addWidget(d->dwiMaskCalculatorWidget);
    d->dwiMaskCalculatorWidget->hide();

    QLabel *modelEstimationLabel = new QLabel(tr("Estimation"));
    QHBoxLayout *modelEstimationComboLayout = new QHBoxLayout;
    modelEstimationComboLayout->addWidget(modelEstimationLabel);
    d->modelEstimationComboBox = new QComboBox;
    d->modelEstimationComboBox->addItem("None selected");
    for(QString estimationName : d->process->modelEstimationPlugins())
    {
        medAbstractDiffusionModelEstimationProcess *tmpProcess = medCore::diffusionModelEstimation::pluginFactory().create(estimationName);
        d->modelEstimationComboBox->addItem(tmpProcess->caption(),estimationName);
    }

    modelEstimationComboLayout->addWidget(d->modelEstimationComboBox);

    connect(d->modelEstimationComboBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(chooseEstimationPlugin(int)));

    tbLayout->addLayout(modelEstimationComboLayout);

    d->modelEstimationWidget = new QGroupBox;
    tbLayout->addWidget(d->modelEstimationWidget);
    d->modelEstimationWidget->hide();

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());
    tbLayout->addWidget(d->progressionPresenter->buildProgressBar());

    return tbWidget;
}

medViewContainerSplitter *medDiffusionModelEstimationMetaProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    inputContainer->setDefaultWidget(new QLabel("Input DWI"));
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(true);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medDiffusionModelEstimationMetaProcessPresenter::_setInputFromContainer);

    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            inputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medDiffusionModelEstimationMetaProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractImageData *>(data));
}

void medDiffusionModelEstimationMetaProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance().importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}

void medDiffusionModelEstimationMetaProcessPresenter::chooseDWIMaskingPlugin(int processIndex)
{
    QString processName = d->dwiMaskingComboBox->itemData(processIndex).toString();
    bool newProcessCreated = d->process->chooseDWIMaskingPlugin(processName);

    if (newProcessCreated)
    {
        d->dwiMaskCalculatorPresenter = medWidgets::dwiMasking::presenterFactory().create(d->process->dwiMaskCalculatorProcess());
        d->dwiMaskCalculatorPresenter->setUseRunControls(false);

        if (!d->dwiMaskCalculatorWidget->layout())
        {
            QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(d->dwiMaskCalculatorPresenter->buildToolBoxWidget());
            d->dwiMaskCalculatorWidget->setLayout(layout);
        }
        else
        {
            QLayout *layout = d->dwiMaskCalculatorWidget->layout();
            QWidget *oldWidget = layout->itemAt(layout->count() - 1)->widget();
            layout->replaceWidget(oldWidget,d->dwiMaskCalculatorPresenter->buildToolBoxWidget());
            oldWidget->deleteLater();
        }

        d->dwiMaskCalculatorWidget->setTitle(d->process->dwiMaskCalculatorProcess()->caption());
        d->dwiMaskCalculatorWidget->show();
    }
    else if (!d->process->dwiMaskCalculatorProcess())
        d->dwiMaskCalculatorWidget->hide();
}

void medDiffusionModelEstimationMetaProcessPresenter::chooseEstimationPlugin(int processIndex)
{
    QString processName = d->modelEstimationComboBox->itemData(processIndex).toString();
    bool newProcessCreated = d->process->chooseModelEstimationPlugin(processName);

    if (newProcessCreated)
    {
        d->modelEstimationPresenter = medWidgets::diffusionModelEstimation::presenterFactory().create(d->process->modelEstimationProcess());
        d->modelEstimationPresenter->setUseRunControls(false);

        if (!d->modelEstimationWidget->layout())
        {
            QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(d->modelEstimationPresenter->buildToolBoxWidget());
            d->modelEstimationWidget->setLayout(layout);
        }
        else
        {
            QLayout *layout = d->modelEstimationWidget->layout();
            QWidget *oldWidget = layout->itemAt(layout->count() - 1)->widget();
            layout->replaceWidget(oldWidget,d->modelEstimationPresenter->buildToolBoxWidget());
            oldWidget->deleteLater();
        }

        d->modelEstimationWidget->setTitle(d->process->modelEstimationProcess()->caption());
        d->modelEstimationWidget->show();
    }
    else if (!d->process->modelEstimationProcess())
        d->modelEstimationWidget->hide();
}
