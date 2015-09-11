// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medAddFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medAddFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    valueRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medAddFilterNode::medAddFilterNode(void) : dtkComposerNodeObject<medAbstractAddFilter>(), d(new medAddFilterNodePrivate())
{
    this->setFactory(medCore::filtering::add::pluginFactory());

    this->appendReceiver(&d->valueRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medAddFilterNode::~medAddFilterNode(void)
{
    delete d;
}

void medAddFilterNode::run(void)
{
    if (d->valueRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractAddFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setValue(d->valueRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

