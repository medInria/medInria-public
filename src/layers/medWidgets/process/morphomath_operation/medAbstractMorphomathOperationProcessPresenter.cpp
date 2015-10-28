/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMorphomathOperationProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>

#include <medAbstractImageData.h>
#include <medIntParameter.h>
#include <medIntParameterPresenter.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>
#include <medAbstractImageData.h>

class medAbstractMorphomathOperationProcessPresenterPrivate
{
public:
    medAbstractMorphomathOperationProcess *process;
    medIntParameterPresenter *progressionPresenter;
    medIntParameterPresenter *kernelRadiusPresenter;
};

medAbstractMorphomathOperationProcessPresenter::medAbstractMorphomathOperationProcessPresenter(medAbstractMorphomathOperationProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractMorphomathOperationProcessPresenterPrivate)
{
    d->process = parent;
    d->kernelRadiusPresenter = new medIntParameterPresenter(d->process->kernelRadius());
    d->progressionPresenter = new medIntParameterPresenter(d->process->progression());

    connect(d->process, &medAbstractMorphomathOperationProcess::finished,
            this, &medAbstractMorphomathOperationProcessPresenter::_importOutput);
}

medAbstractMorphomathOperationProcessPresenter::~medAbstractMorphomathOperationProcessPresenter()
{
}

QWidget *medAbstractMorphomathOperationProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    tbLayout->addWidget(d->kernelRadiusPresenter->buildWidget());
    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(d->progressionPresenter->buildProgressBar());
    tbLayout->addWidget(this->buildCancelButton());

    return tbWidget;
}

medViewContainerSplitter *medAbstractMorphomathOperationProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    medViewContainer * outputContainer = inputContainer->splitVertically();

    inputContainer->setDefaultWidget(new QLabel("INPUT"));
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(false);

    outputContainer->setDefaultWidget(new QLabel("OUTPUT"));
    outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    outputContainer->setUserSplittable(false);
    outputContainer->setMultiLayered(false);
    outputContainer->setUserOpenable(false);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractMorphomathOperationProcessPresenter::_setInputFromContainer);


    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            outputContainer, SLOT(addData(medAbstractData*)));

    return splitter;
}


void medAbstractMorphomathOperationProcessPresenter::_setInputFromContainer(medAbstractData *data)
{

    medAbstractImageData *input = qobject_cast<medAbstractImageData *>(data);
    if(input)
    {
        d->process->setInput(input);
    }
}


void medAbstractMorphomathOperationProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCES)
    {
        medDataManager::instance()->importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}
