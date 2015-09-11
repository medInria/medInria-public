// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medNormalizeFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medNormalizeFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medNormalizeFilterNode::medNormalizeFilterNode(void) : dtkComposerNodeObject<medAbstractNormalizeFilter>(), d(new medNormalizeFilterNodePrivate())
{
    this->setFactory(medCore::filtering::normalize::pluginFactory());

    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medNormalizeFilterNode::~medNormalizeFilterNode(void)
{
    delete d;
}

void medNormalizeFilterNode::run(void)
{
    if (d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractNormalizeFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

