/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractArithmeticOperationProcessPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>

#include <medIntParameter.h>
#include <medIntParameterPresenter.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medDataManager.h>
#include <medAbstractImageView.h>
#include <medAbstractImageData.h>

class medAbstractArithmeticOperationProcessPresenterPrivate
{
public:
    medAbstractArithmeticOperationProcess *process;
    medIntParameterPresenter *progressionPresenter;
};

medAbstractArithmeticOperationProcessPresenter::medAbstractArithmeticOperationProcessPresenter(medAbstractArithmeticOperationProcess *parent)
    : medAbstractProcessPresenter(parent), d(new medAbstractArithmeticOperationProcessPresenterPrivate)
{
    d->process = parent;
    d->progressionPresenter = new medIntParameterPresenter(d->process->progression());

    connect(d->process, &medAbstractArithmeticOperationProcess::finished,
            this, &medAbstractArithmeticOperationProcessPresenter::_importOutput,
            Qt::QueuedConnection);
}

medAbstractArithmeticOperationProcessPresenter::~medAbstractArithmeticOperationProcessPresenter()
{

}

QWidget *medAbstractArithmeticOperationProcessPresenter::buildToolBoxWidget()
{
    QWidget *tbWidget = new QWidget;
    QVBoxLayout *tbLayout = new QVBoxLayout;
    tbWidget->setLayout(tbLayout);

    tbLayout->addWidget(this->buildRunButton());
    tbLayout->addWidget(this->buildCancelButton());
    tbLayout->addWidget(d->progressionPresenter->buildProgressBar());

    return tbWidget;
}

medViewContainerSplitter *medAbstractArithmeticOperationProcessPresenter::buildViewContainerSplitter()
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    medViewContainer *inputContainer1 = new medViewContainer;
    splitter->addViewContainer(inputContainer1);

    medViewContainer * outputContainer = inputContainer1->splitVertically();
    medViewContainer* inputContainer2 = inputContainer1->splitHorizontally();

    auto viewLayout1 = static_cast<QVBoxLayout*>(inputContainer1->defaultWidget()->layout());
    auto label1 = static_cast<QLabel*>(viewLayout1->itemAt(0)->widget());
    if (!label1->text().contains("INPUT 1"))
    {
        label1->setText("INPUT 1\n\n"+label1->text());
        label1->setAlignment(Qt::AlignCenter);
    }
    inputContainer1->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer1->setUserSplittable(false);
    inputContainer1->setMultiLayered(false);

    auto viewLayout2 = static_cast<QVBoxLayout*>(inputContainer2->defaultWidget()->layout());
    auto label2 = static_cast<QLabel*>(viewLayout2->itemAt(0)->widget());
    if (!label2->text().contains("INPUT 2"))
    {
        label2->setText("INPUT 2\n\n"+label2->text());
        label2->setAlignment(Qt::AlignCenter);
    }
    inputContainer2->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer2->setUserSplittable(false);
    inputContainer2->setMultiLayered(false);

    auto viewLayoutOutput = static_cast<QVBoxLayout*>(outputContainer->defaultWidget()->layout());
    auto labelOutput = static_cast<QLabel*>(viewLayoutOutput->itemAt(0)->widget());
    labelOutput->setText("OUTPUT");
    outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    outputContainer->setUserSplittable(false);
    outputContainer->setMultiLayered(false);
    outputContainer->setUserOpenable(false);

    connect(inputContainer1, &medViewContainer::dataAdded,
            this, &medAbstractArithmeticOperationProcessPresenter::_setInput1FromContainer);

    connect(inputContainer2, &medViewContainer::dataAdded,
            this, &medAbstractArithmeticOperationProcessPresenter::_setInput2FromContainer);


    connect(this, SIGNAL(_outputImported(medAbstractData*)),
            outputContainer, SLOT(addData(medAbstractData*)),
            Qt::QueuedConnection);

    return splitter;
}

void medAbstractArithmeticOperationProcessPresenter::_setInput1FromContainer(medAbstractData *data)
{
    d->process->setInput1(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractArithmeticOperationProcessPresenter::_setInput2FromContainer(medAbstractData *data)
{
    d->process->setInput2(qobject_cast<medAbstractImageData *>(data));
}

void medAbstractArithmeticOperationProcessPresenter::_importOutput(medAbstractJob::medJobExitStatus jobExitStatus)
{
    if(jobExitStatus == medAbstractJob::MED_JOB_EXIT_SUCCESS)
    {
        medDataManager::instance().importData(d->process->output());
        emit _outputImported(d->process->output());
    }
}

medAbstractArithmeticOperationProcess* medAbstractArithmeticOperationProcessPresenter::process() const
{
    return d->process;
}
