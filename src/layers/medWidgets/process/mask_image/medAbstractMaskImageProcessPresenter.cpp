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

    auto viewLayoutMask = static_cast<QVBoxLayout*>(maskContainer->defaultWidget()->layout());
    auto labelMask = static_cast<QLabel*>(viewLayoutMask->itemAt(0)->widget());
    if (!labelMask->text().contains("MASK"))
    {
        labelMask->setText("MASK\n\n"+labelMask->text());
        labelMask->setAlignment(Qt::AlignCenter);
    }
    maskContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    maskContainer->setUserSplittable(false);
    maskContainer->setMultiLayered(false);

    auto viewLayoutOutput = static_cast<QVBoxLayout*>(outputContainer->defaultWidget()->layout());
    auto labelOutput = static_cast<QLabel*>(viewLayoutOutput->itemAt(0)->widget());
    labelOutput->setText("OUTPUT");
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
