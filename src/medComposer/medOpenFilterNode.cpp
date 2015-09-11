// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medOpenFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medOpenFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    radiusRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medOpenFilterNode::medOpenFilterNode(void) : dtkComposerNodeObject<medAbstractOpenFilter>(), d(new medOpenFilterNodePrivate())
{
    this->setFactory(medCore::filtering::open::pluginFactory());

    this->appendReceiver(&d->radiusRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medOpenFilterNode::~medOpenFilterNode(void)
{
    delete d;
}

void medOpenFilterNode::run(void)
{
    if (d->radiusRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractOpenFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setRadius(d->radiusRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

