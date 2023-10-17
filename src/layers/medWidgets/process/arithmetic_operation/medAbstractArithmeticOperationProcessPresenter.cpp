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

    inputContainer1->setDefaultWidget(new QLabel("INPUT 1"));
    inputContainer1->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer1->setUserSplittable(false);
    inputContainer1->setMultiLayered(false);

    inputContainer2->setDefaultWidget(new QLabel("INPUT 2"));
    inputContainer2->setClosingMode(medViewContainer::CLOSE_VIEW);
    inputContainer2->setUserSplittable(false);
    inputContainer2->setMultiLayered(false);

    outputContainer->setDefaultWidget(new QLabel("OUTPUT"));
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
