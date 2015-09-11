/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMorphomathOperationProcess.h>

#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medDataManager.h>
#include <medToolBox.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>

#include <medDoubleParameter.h>

class medAbstractMorphomathOperationProcessPrivate
{
public:
    medAbstractImageData *input;
    medAbstractImageData *output;
    medViewContainer *inputContainer;
    medViewContainer *outputContainer;

    medDoubleParameter *kernelRadius;
};

medAbstractMorphomathOperationProcess::medAbstractMorphomathOperationProcess(QObject *parent): medAbstractProcess(parent),
    d(new medAbstractMorphomathOperationProcessPrivate)
{
    d->input = NULL;
    d->output = NULL;

    d->kernelRadius = new medDoubleParameter("Kernel radius", this);
    d->kernelRadius->setSingleStep(1);
    d->kernelRadius->setRange(1, 10);
    d->kernelRadius->setValue(1);
}

medAbstractMorphomathOperationProcess::~medAbstractMorphomathOperationProcess()
{
    delete d;
}

void medAbstractMorphomathOperationProcess::setInput(medAbstractImageData *data)
{
    d->input = data;
}

medAbstractImageData* medAbstractMorphomathOperationProcess::input() const
{
    return d->input;
}

medAbstractImageData* medAbstractMorphomathOperationProcess::output() const
{
    return d->output;
}

void medAbstractMorphomathOperationProcess::setOutput(medAbstractImageData *data)
{
    d->output = data;
}

medDoubleParameter* medAbstractMorphomathOperationProcess::kernelRadius() const
{
    return d->kernelRadius;
}

void medAbstractMorphomathOperationProcess::setKernelRadius(const double &radius)
{
    d->kernelRadius->setValue(radius);
}

QWidget* medAbstractMorphomathOperationProcess::toolbox() const
{
    medToolBox* tb = new medToolBox;
    tb->setTitle(this->details().name);
    tb->addWidget(d->kernelRadius->getWidget());
    tb->addWidget(this->runButton());

    return tb;
}

medViewContainerSplitter* medAbstractMorphomathOperationProcess::viewContainerSplitter() const
{
    medViewContainerSplitter* splitter = new medViewContainerSplitter;
    d->inputContainer = new medViewContainer;
    splitter->addViewContainer(d->inputContainer);

    d->outputContainer = d->inputContainer->splitVertically();

    d->inputContainer->setDefaultWidget(new QLabel("INPUT"));
    d->inputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    d->inputContainer->setUserSplittable(false);
    d->inputContainer->setMultiLayered(false);

    d->outputContainer->setDefaultWidget(new QLabel("OUTPUT"));
    d->outputContainer->setClosingMode(medViewContainer::CLOSE_VIEW);
    d->outputContainer->setUserSplittable(false);
    d->outputContainer->setMultiLayered(false);
    d->outputContainer->setUserOpenable(false);

    connect(d->inputContainer, &medViewContainer::viewContentChanged,
            this, &medAbstractMorphomathOperationProcess::_setInput);

    connect(this, &medAbstractMorphomathOperationProcess::success,
            this, &medAbstractMorphomathOperationProcess::_fillOutputContainer);

    return splitter;
}

void medAbstractMorphomathOperationProcess::_setInput()
{
    medAbstractImageView* view = dynamic_cast<medAbstractImageView*> (d->inputContainer->view());
    if(view)
    {
        medAbstractImageData *input = dynamic_cast<medAbstractImageData *>(view->layerData(0));
        if(input)
        {
            this->setInput(input);
        }
    }
}


void medAbstractMorphomathOperationProcess::_fillOutputContainer()
{
    medDataManager::instance()->importData(this->output());

    d->outputContainer->removeView();
    d->outputContainer->addData(this->output());
}
