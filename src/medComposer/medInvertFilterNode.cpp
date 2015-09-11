// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medInvertFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medInvertFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medInvertFilterNode::medInvertFilterNode(void) : dtkComposerNodeObject<medAbstractInvertFilter>(), d(new medInvertFilterNodePrivate())
{
    this->setFactory(medCore::filtering::invert::pluginFactory());

    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medInvertFilterNode::~medInvertFilterNode(void)
{
    delete d;
}

void medInvertFilterNode::run(void)
{
    if (d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractInvertFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

