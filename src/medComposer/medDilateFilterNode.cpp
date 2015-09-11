// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medDilateFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medDilateFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    radiusRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medDilateFilterNode::medDilateFilterNode(void) : dtkComposerNodeObject<medAbstractDilateFilter>(), d(new medDilateFilterNodePrivate())
{
    this->setFactory(medCore::filtering::dilate::pluginFactory());

    this->appendReceiver(&d->radiusRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medDilateFilterNode::~medDilateFilterNode(void)
{
    delete d;
}

void medDilateFilterNode::run(void)
{
    if (d->radiusRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractDilateFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setRadius(d->radiusRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

