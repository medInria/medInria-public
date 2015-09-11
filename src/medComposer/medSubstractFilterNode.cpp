// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medSubstractFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medSubstractFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    radiusRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medSubstractFilterNode::medSubstractFilterNode(void) : dtkComposerNodeObject<medAbstractSubstractFilter>(), d(new medSubstractFilterNodePrivate())
{
    this->setFactory(medCore::filtering::substract::pluginFactory());

    this->appendReceiver(&d->radiusRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medSubstractFilterNode::~medSubstractFilterNode(void)
{
    delete d;
}

void medSubstractFilterNode::run(void)
{
    if (d->radiusRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractSubstractFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setValue(d->radiusRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

