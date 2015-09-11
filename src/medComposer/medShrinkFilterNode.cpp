// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medShrinkFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medShrinkFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    xShrinkRecv;
    dtkComposerTransmitterReceiver< double >                    yShrinkRecv;
    dtkComposerTransmitterReceiver< double >                    zShrinkRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medShrinkFilterNode::medShrinkFilterNode(void) : dtkComposerNodeObject<medAbstractShrinkFilter>(), d(new medShrinkFilterNodePrivate())
{
    this->setFactory(medCore::filtering::shrink::pluginFactory());

    this->appendReceiver(&d->xShrinkRecv);
    this->appendReceiver(&d->yShrinkRecv);
    this->appendReceiver(&d->zShrinkRecv);

    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medShrinkFilterNode::~medShrinkFilterNode(void)
{
    delete d;
}

void medShrinkFilterNode::run(void)
{
    if (d->xShrinkRecv.isEmpty()
            || d->yShrinkRecv.isEmpty()
            || d->zShrinkRecv.isEmpty()
            || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractShrinkFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setXShrink(d->xShrinkRecv.data());
        filter->setYShrink(d->yShrinkRecv.data());
        filter->setZShrink(d->zShrinkRecv.data());
        filter->setImage(d->imgRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

