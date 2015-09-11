// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medMedianFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medMedianFilterNodePrivate
{
public:


    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medMedianFilterNode::medMedianFilterNode(void) : dtkComposerNodeObject<medAbstractMedianFilter>(), d(new medMedianFilterNodePrivate())
{
    this->setFactory(medCore::filtering::median::pluginFactory());


    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medMedianFilterNode::~medMedianFilterNode(void)
{
    delete d;
}

void medMedianFilterNode::run(void)
{
    if (d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractMedianFilter* filter = this->object();
        if(!this->object())
            return;

        filter->setImage(d->imgRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

