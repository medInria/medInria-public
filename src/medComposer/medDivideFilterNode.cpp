// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medDivideFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medDivideFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    radiusRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medDivideFilterNode::medDivideFilterNode(void) : dtkComposerNodeObject<medAbstractDivideFilter>(), d(new medDivideFilterNodePrivate())
{
    this->setFactory(medCore::filtering::divide::pluginFactory());

    this->appendReceiver(&d->radiusRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medDivideFilterNode::~medDivideFilterNode(void)
{
    delete d;
}

void medDivideFilterNode::run(void)
{
    if (d->radiusRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractDivideFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setValue(d->radiusRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

