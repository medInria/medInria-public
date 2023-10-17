/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDiffusionScalarMapsProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include <medAbstractImageData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>

class medAbstractDiffusionScalarMapsProcessPresenterPrivate
{
public:
    medAbstractDiffusionScalarMapsProcess *process;
};

medAbstractDiffusionScalarMapsProcessPresenter::medAbstractDiffusionScalarMapsProcessPresenter(medAbstractDiffusionScalarMapsProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractDiffusionScalarMapsProcessPresenterPrivate)
{
    d->process = parent;

    connect(d->process, &medAbstractDiffusionScalarMapsProcess::finished,
            this, &medAbstractDiffusionScalarMapsProcessPresenter::_importOutput,
            Qt::QueuedConnection);
}

medAbstractDiffusionScalarMapsProcessPresenter::~medAbstractDiffusionScalarMapsProcessPresenter()
{

}

QWidget *medAbstractDiffusionScalarMapsProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());

    return tbWidget;
}

medViewContainerSplitter *medAbstractDiffusionScalarMapsProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    medViewContainer * outputContainer = inputContainer->splitVertically();

    auto viewLayoutInput = static_cast<QVBoxLayout*>(inputContainer->defaultWidget()->layout());
    auto labelInput = static_cast<QLabel*>(viewLayoutInput->itemAt(0)->widget());
    if (!labelInput->text().contains("Input model image"))
    {
        labelInput->setText("Input model image\n\n"+labelInput->text());
        labelInput->setAlignment(Qt::AlignCenter);
    }
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(false);

    auto viewLayoutOutput = static_cast<QVBoxLayout*>(outputContainer->defaultWidget()->layout());
    auto labelOutput = static_cast<QLabel*>(viewLayoutOutput->itemAt(0)->widget());
    labelOutput->setText("Output scalar map image");
    outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    outputContainer->setUserSplittable(false);
    outputContainer->setMultiLayered(false);
    outputContainer->setUserOpenable(false);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractDiffusionScalarMapsProcessPresenter::_setInputFromContainer);

    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            outputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medAbstractDiffusionScalarMapsProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractDiffusionModelImageData *>(data));
}

void medAbstractDiffusionScalarMapsProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance().importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}
