// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medGaussianFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medGaussianFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    sigmaRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medGaussianFilterNode::medGaussianFilterNode(void) : dtkComposerNodeObject<medAbstractGaussianFilter>(), d(new medGaussianFilterNodePrivate())
{
    this->setFactory(medCore::filtering::gaussian::pluginFactory());

    this->appendReceiver(&d->sigmaRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medGaussianFilterNode::~medGaussianFilterNode(void)
{
    delete d;
}

void medGaussianFilterNode::run(void)
{
    if (d->sigmaRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractGaussianFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setData(d->imgRecv.data());
        filter->setSigma(d->sigmaRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

