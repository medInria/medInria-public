/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>

#include <medIntParameter.h>
#include <medIntParameterPresenter.h>
#include <medDoubleParameter.h>
#include <medDoubleParameterPresenter.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>
#include <medAbstractImageData.h>

class medAbstractSingleFilterOperationProcessPresenterPrivate
{
public:
    medAbstractSingleFilterOperationProcess *process;
    medIntParameterPresenter *progressionPresenter;
};

medAbstractSingleFilterOperationProcessPresenter::medAbstractSingleFilterOperationProcessPresenter(medAbstractSingleFilterOperationProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractSingleFilterOperationProcessPresenterPrivate)
{
    d->process = parent;
    d->progressionPresenter = new medIntParameterPresenter(d->process->progression());

    connect(d->process, &medAbstractSingleFilterOperationProcess::finished,
            this, &medAbstractSingleFilterOperationProcessPresenter::_importOutput,
            Qt::QueuedConnection);
}

medAbstractSingleFilterOperationProcessPresenter::~medAbstractSingleFilterOperationProcessPresenter()
{

}

medIntParameterPresenter *medAbstractSingleFilterOperationProcessPresenter::progressionPresenter()
{
    return d->progressionPresenter;
}

QWidget *medAbstractSingleFilterOperationProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());
    tbLayout->addWidget(d->progressionPresenter->buildProgressBar());

    return tbWidget;
}

medViewContainerSplitter *medAbstractSingleFilterOperationProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    medViewContainer * outputContainer = inputContainer->splitVertically();

    auto viewLayoutInput = static_cast<QVBoxLayout*>(inputContainer->defaultWidget()->layout());
    auto labelInput = static_cast<QLabel*>(viewLayoutInput->itemAt(0)->widget());
    if (!labelInput->text().contains("INPUT"))
    {
        labelInput->setText("INPUT\n\n"+labelInput->text());
        labelInput->setAlignment(Qt::AlignCenter);
    }
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(false);

    auto viewLayoutOutput = static_cast<QVBoxLayout*>(outputContainer->defaultWidget()->layout());
    auto labelOutput = static_cast<QLabel*>(viewLayoutOutput->itemAt(0)->widget());
    labelOutput->setText("OUTPUT");
    outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    outputContainer->setUserSplittable(false);
    outputContainer->setMultiLayered(false);
    outputContainer->setUserOpenable(false);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractSingleFilterOperationProcessPresenter::_setInputFromContainer);

    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            outputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medAbstractSingleFilterOperationProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractSingleFilterOperationProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance().importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}
