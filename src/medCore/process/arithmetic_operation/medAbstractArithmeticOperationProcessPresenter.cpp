/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractArithmeticOperationProcessPresenter.h>

#include <medToolBox.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medAbstractArithmeticOperationProcess.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medDataManager.h>

#include <QStringList>
#include <QPushButton>



class medAbstractArithmeticOperationProcessPresenterPrivate
{
public:
    medViewContainer *inputContainer1;
    medViewContainer *inputContainer2;
    medViewContainer *outputContainer;
};

medAbstractArithmeticOperationProcessPresenter::medAbstractArithmeticOperationProcessPresenter(QObject *parent):
    medAsbtractProcessPresenter(parent), d(new medAbstractArithmeticOperationProcessPresenterPrivate)
{
    this->addTags(QStringList() << "arithmetic" << "operation");
}

medAbstractArithmeticOperationProcessPresenter::~medAbstractArithmeticOperationProcessPresenter()
{
    delete d;
}

QWidget* medAbstractArithmeticOperationProcessPresenter::toolbox() const
{
    medToolBox* tb = new medToolBox;
    tb->setTitle(this->name());
    QPushButton *runButton = new QPushButton(tr("Run"));
    tb->addWidget(runButton);

    connect(runButton, &QPushButton::clicked,
            this->process(), &medAbstractArithmeticOperationProcess::start);

    return tb;
}

medViewContainerSplitter* medAbstractArithmeticOperationProcessPresenter::viewContainerSplitter() const
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    d->inputContainer1 = new medViewContainer;
    splitter->addViewContainer(d->inputContainer1);

    d->outputContainer = d->inputContainer1->splitVertically();
    d->inputContainer2 = d->inputContainer1->splitHorizontally();

    d->inputContainer1->setDefaultWidget(new QLabel("INPUT 1"));
    d->inputContainer1->setClosingMode(medViewContainer::CLOSE_VIEW);
    d->inputContainer1->setUserSplittable(false);
    d->inputContainer1->setMultiLayered(false);

    d->inputContainer2->setDefaultWidget(new QLabel("INPUT 2"));
    d->inputContainer2->setClosingMode(medViewContainer::CLOSE_VIEW);
    d->inputContainer2->setUserSplittable(false);
    d->inputContainer2->setMultiLayered(false);

    d->outputContainer->setDefaultWidget(new QLabel("OUTPUT"));
    d->outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    d->outputContainer->setUserSplittable(false);
    d->outputContainer->setMultiLayered(false);
    d->outputContainer->setUserOpenable(false);

    connect(d->inputContainer1, &medViewContainer::viewContentChanged,
            this, &medAbstractArithmeticOperationProcessPresenter::_setInput1);

    connect(d->inputContainer2, &medViewContainer::viewContentChanged,
            this, &medAbstractArithmeticOperationProcessPresenter::_setInput2);
    connect(this->process(), &medAbstractArithmeticOperationProcess::success,
            this, &medAbstractArithmeticOperationProcessPresenter::_fillOutputContainer);

    return splitter;
}

void medAbstractArithmeticOperationProcessPresenter::_setInput1()
{
    medAbstractImageView* view = dynamic_cast<medAbstractImageView*> (d->inputContainer1->view());
    if(view)
    {
        medAbstractImageData *input1 = dynamic_cast<medAbstractImageData *>(view->layerData(0));
        if(input1)
        {
            this->process()->setInput1(input1);
        }
    }
}

void medAbstractArithmeticOperationProcessPresenter::_setInput2()
{
    medAbstractImageView* view = dynamic_cast<medAbstractImageView*> (d->inputContainer2->view());
    if(view)
    {
        medAbstractImageData *input2 = dynamic_cast<medAbstractImageData*>(view->layerData(0));
        if(input2)
        {
            this->process()->setInput2(input2);
        }
    }
}

void medAbstractArithmeticOperationProcessPresenter::_fillOutputContainer()
{
    medDataManager::instance()->importData(this->process()->output());
    d->outputContainer->removeView();
    d->outputContainer->addData(this->process()->output());
}
