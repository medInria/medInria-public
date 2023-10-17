/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDWIMaskingProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageData.h>

class medAbstractDWIMaskingProcessPresenterPrivate
{
public:
    medAbstractDWIMaskingProcess *process;
    bool useRunControls;
};

medAbstractDWIMaskingProcessPresenter::medAbstractDWIMaskingProcessPresenter(medAbstractDWIMaskingProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractDWIMaskingProcessPresenterPrivate)
{
    d->process = parent;

    connect(d->process, &medAbstractDWIMaskingProcess::finished,
            this, &medAbstractDWIMaskingProcessPresenter::_importOutput);
}

medAbstractDWIMaskingProcessPresenter::~medAbstractDWIMaskingProcessPresenter()
{

}

void medAbstractDWIMaskingProcessPresenter::setUseRunControls(bool useRun)
{
    d->useRunControls = useRun;
}

bool medAbstractDWIMaskingProcessPresenter::useRunControls()
{
    return d->useRunControls;
}

QWidget *medAbstractDWIMaskingProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    if (d->useRunControls)
    {
        tbLayout->addWidget(this->buildRunButton());
        tbLayout->addWidget(this->buildCancelButton());
    }

    return tbWidget;
}

medViewContainerSplitter *medAbstractDWIMaskingProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    medViewContainer * outputContainer = inputContainer->splitVertically();

    auto viewLayoutInput = static_cast<QVBoxLayout*>(inputContainer->defaultWidget()->layout());
    auto labelInput = static_cast<QLabel*>(viewLayoutInput->itemAt(0)->widget());
    if (!labelInput->text().contains("Input DWI"))
    {
        labelInput->setText("Input DWI\n\n"+labelInput->text());
        labelInput->setAlignment(Qt::AlignCenter);
    }
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(false);

    auto viewLayoutOutput = static_cast<QVBoxLayout*>(outputContainer->defaultWidget()->layout());
    auto labelOutput = static_cast<QLabel*>(viewLayoutOutput->itemAt(0)->widget());
    labelOutput->setText("Output mask");
    outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    outputContainer->setUserSplittable(false);
    outputContainer->setMultiLayered(false);
    outputContainer->setUserOpenable(false);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractDWIMaskingProcessPresenter::_setInputFromContainer);

    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            outputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}


void medAbstractDWIMaskingProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractImageData *>(data));
}


void medAbstractDWIMaskingProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance().importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}
