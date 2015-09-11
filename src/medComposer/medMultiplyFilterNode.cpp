// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medMultiplyFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medMultiplyFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    radiusRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medMultiplyFilterNode::medMultiplyFilterNode(void) : dtkComposerNodeObject<medAbstractMultiplyFilter>(), d(new medMultiplyFilterNodePrivate())
{
    this->setFactory(medCore::filtering::multiply::pluginFactory());

    this->appendReceiver(&d->radiusRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medMultiplyFilterNode::~medMultiplyFilterNode(void)
{
    delete d;
}

void medMultiplyFilterNode::run(void)
{
    if (d->radiusRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractMultiplyFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setValue(d->radiusRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

