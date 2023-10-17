/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractTractographyProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include <medAbstractImageData.h>
#include <medAbstractFibersData.h>
#include <medAbstractDiffusionModelImageData.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>

class medAbstractTractographyProcessPresenterPrivate
{
public:
    medAbstractTractographyProcess *process;
};

medAbstractTractographyProcessPresenter::medAbstractTractographyProcessPresenter(medAbstractTractographyProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractTractographyProcessPresenterPrivate)
{
    d->process = parent;

    connect(d->process, &medAbstractTractographyProcess::finished,
            this, &medAbstractTractographyProcessPresenter::_importOutput,
            Qt::QueuedConnection);
}

medAbstractTractographyProcessPresenter::~medAbstractTractographyProcessPresenter()
{

}

QWidget *medAbstractTractographyProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());

    return tbWidget;
}

medViewContainerSplitter *medAbstractTractographyProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer = new medViewContainer;
    splitter->addViewContainer(inputContainer);

    auto viewLayout = static_cast<QVBoxLayout*>(inputContainer->defaultWidget()->layout());
    auto label = static_cast<QLabel*>(viewLayout->itemAt(0)->widget());
    if (!label->text().contains("Input diffusion model"))
    {
        label->setText("Input diffusion model\n\n"+label->text());
        label->setAlignment(Qt::AlignCenter);
    }
    inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer->setUserSplittable(false);
    inputContainer->setMultiLayered(true);

    connect(inputContainer, &medViewContainer::dataAdded,
            this, &medAbstractTractographyProcessPresenter::_setInputFromContainer);

    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            inputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medAbstractTractographyProcessPresenter::_setInputFromContainer(medAbstractData *data)
{
    d->process->setInput(qobject_cast<medAbstractDiffusionModelImageData *>(data));
}

void medAbstractTractographyProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance().importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}
