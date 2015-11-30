/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDiffusionModelEstimationProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include <medAbstractImageData.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>

class medAbstractDiffusionModelEstimationProcessPresenterPrivate
{
public:
    medAbstractDiffusionModelEstimationProcess *process;
};

medAbstractDiffusionModelEstimationProcessPresenter::medAbstractDiffusionModelEstimationProcessPresenter(medAbstractDiffusionModelEstimationProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractDiffusionModelEstimationProcessPresenterPrivate)
{
    d->process = parent;

    connect(d->process, &medAbstractDiffusionModelEstimationProcess::finished,
            this, &medAbstractDiffusionModelEstimationProcessPresenter::_importOutput,
            Qt::QueuedConnection);
}

medAbstractDiffusionModelEstimationProcessPresenter::~medAbstractDiffusionModelEstimationProcessPresenter()
{
}

QWidget *medAbstractDiffusionModelEstimationProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());

    return tbWidget;
}

medViewContainerSplitter *medAbstractDiffusionModelEstimationProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    medViewContainer * maskContainer = inputContainer->splitVertically();

    inputContainer->setDefaultWidget(new QLabel("Input DWI"));
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(true);

    maskContainer->setDefaultWidget(new QLabel("Mask"));
    maskContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    maskContainer->setUserSplittable(false);
    maskContainer->setMultiLayered(false);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractDiffusionModelEstimationProcessPresenter::_setInputFromContainer);

    connect(maskContainer, &medViewContainer::dataAdded,
            this, &medAbstractDiffusionModelEstimationProcessPresenter::_setMaskFromContainer);

    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            inputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medAbstractDiffusionModelEstimationProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractDiffusionModelEstimationProcessPresenter::_setMaskFromContainer(medAbstractData *data)
{
    d->process->setMask(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractDiffusionModelEstimationProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance()->importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}
