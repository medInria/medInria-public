/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractArithmeticOperationProcess.h>

#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medDataManager.h>
#include <medToolBox.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>

class medAbstractArithmeticOperationProcessPrivate
{
public:
    medAbstractImageData *input1;
    medAbstractImageData *input2;
    medAbstractImageData *output;
    medViewContainer *inputContainer1;
    medViewContainer *inputContainer2;
    medViewContainer *outputContainer;
};

medAbstractArithmeticOperationProcess::medAbstractArithmeticOperationProcess(QObject *parent): medAbstractProcess(parent),
    d(new medAbstractArithmeticOperationProcessPrivate)
{
    d->input1 = NULL;
    d->input2 = NULL;
    d->output = NULL;
}

medAbstractArithmeticOperationProcess::~medAbstractArithmeticOperationProcess()
{
    delete d;
}

void medAbstractArithmeticOperationProcess::setInput1(medAbstractImageData *data)
{
    d->input1 = data;
}

medAbstractImageData* medAbstractArithmeticOperationProcess::input1() const
{
    return d->input1;
}


void medAbstractArithmeticOperationProcess::setInput2(medAbstractImageData *data)
{
    d->input2 = data;
}

medAbstractImageData* medAbstractArithmeticOperationProcess::input2() const
{
    return d->input2;
}

medAbstractImageData* medAbstractArithmeticOperationProcess::output() const
{
    return d->output;
}

void medAbstractArithmeticOperationProcess::setOutput(medAbstractImageData *data)
{
    d->output = data;
}


QWidget* medAbstractArithmeticOperationProcess::toolbox() const
{
    medToolBox* tb = new medToolBox;
    tb->setTitle(this->name());
    tb->addWidget(this->runButton());

    return tb;
}

medViewContainerSplitter* medAbstractArithmeticOperationProcess::viewContainerSplitter() const
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
            this, &medAbstractArithmeticOperationProcess::_setInput1);

    connect(d->inputContainer2, &medViewContainer::viewContentChanged,
            this, &medAbstractArithmeticOperationProcess::_setInput2);

    connect(this, &medAbstractArithmeticOperationProcess::success,
            this, &medAbstractArithmeticOperationProcess::_fillOutputContainer);

    return splitter;
}

void medAbstractArithmeticOperationProcess::_setInput1()
{
    medAbstractImageView* view = dynamic_cast<medAbstractImageView*> (d->inputContainer1->view());
    if(view)
    {
        medAbstractImageData *input1 = dynamic_cast<medAbstractImageData *>(view->layerData(0));
        if(input1)
        {
            this->setInput1(input1);
        }
    }
}

void medAbstractArithmeticOperationProcess::_setInput2()
{
    medAbstractImageView* view = dynamic_cast<medAbstractImageView*> (d->inputContainer2->view());
    if(view)
    {
        medAbstractImageData *input2 = dynamic_cast<medAbstractImageData*>(view->layerData(0));
        if(input2)
        {
            this->setInput2(input2);
        }
    }
}

void medAbstractArithmeticOperationProcess::_fillOutputContainer()
{
    medDataManager::instance()->importData(this->output());

    d->outputContainer->removeView();
    d->outputContainer->addData(this->output());
}
