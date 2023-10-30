/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMaskImageProcessPresenter.h>

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

class medAbstractMaskImageProcessPresenterPrivate
{
public:
    medAbstractMaskImageProcess *process;
    medIntParameterPresenter *progressionPresenter;
};

medAbstractMaskImageProcessPresenter::medAbstractMaskImageProcessPresenter(medAbstractMaskImageProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractMaskImageProcessPresenterPrivate)
{
    d->process = parent;
    d->progressionPresenter = new medIntParameterPresenter(d->process->progression());

    connect(d->process, &medAbstractMaskImageProcess::finished,
            this, &medAbstractMaskImageProcessPresenter::_importOutput,
            Qt::QueuedConnection);
}

medAbstractMaskImageProcessPresenter::~medAbstractMaskImageProcessPresenter()
{

}

QWidget *medAbstractMaskImageProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(d->progressionPresenter->buildProgressBar());
    tbLayout->addWidget(this->buildCancelButton());

    return tbWidget;
}

medViewContainerSplitter *medAbstractMaskImageProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    medViewContainer * outputContainer = inputContainer->splitVertically();
    medViewContainer * maskContainer = inputContainer->splitHorizontally();

    inputContainer->setDefaultWidget(new QLabel("INPUT"));
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(false);

    maskContainer->setDefaultWidget(new QLabel("MASK"));
    maskContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    maskContainer->setUserSplittable(false);
    maskContainer->setMultiLayered(false);

    outputContainer->setDefaultWidget(new QLabel("OUTPUT"));
    outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    outputContainer->setUserSplittable(false);
    outputContainer->setMultiLayered(false);
    outputContainer->setUserOpenable(false);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractMaskImageProcessPresenter::_setInputFromContainer);
    connect(maskContainer, &medViewContainer::dataAdded,
            this, &medAbstractMaskImageProcessPresenter::_setMaskFromContainer);


    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            outputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medAbstractMaskImageProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractMaskImageProcessPresenter::_setMaskFromContainer(medAbstractData *data)
{
    d->process->setMask(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractMaskImageProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance().importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}
