// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medWindowingFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medWindowingFilterNodePrivate
{
public:


    dtkComposerTransmitterReceiver< double >     minimumIntensityValueRecv;
    dtkComposerTransmitterReceiver< double >     maximumIntensityValueRecv;
    dtkComposerTransmitterReceiver< double >     minimumOutputIntensityValueRecv;
    dtkComposerTransmitterReceiver< double >     maximumOutputIntensityValue;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medWindowingFilterNode::medWindowingFilterNode(void) : dtkComposerNodeObject<medAbstractWindowingFilter>(), d(new medWindowingFilterNodePrivate())
{
    this->setFactory(medCore::filtering::windowing::pluginFactory());

    this->appendReceiver(&d->minimumIntensityValueRecv);
    this->appendReceiver(&d->maximumIntensityValueRecv);
    this->appendReceiver(&d->minimumOutputIntensityValueRecv);
    this->appendReceiver(&d->maximumOutputIntensityValue);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medWindowingFilterNode::~medWindowingFilterNode(void)
{
    delete d;
}

void medWindowingFilterNode::run(void)
{
    if (d->imgRecv.isEmpty()    || d->minimumIntensityValueRecv.isEmpty()
                                || d->maximumIntensityValueRecv.isEmpty()
                                || d->minimumOutputIntensityValueRecv.isEmpty()
                                || d->maximumOutputIntensityValue.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractWindowingFilter* filter = this->object();
        if(!this->object())
            return;

        filter->setMinimumIntensityValue(d->minimumIntensityValueRecv.data());
        filter->setMaximumIntensityValue(d->maximumIntensityValueRecv.data());
        filter->setMaximumOutputIntensityValue(d->minimumOutputIntensityValueRecv.data());
        filter->setMinimumOutputIntensityValue(d->maximumOutputIntensityValue.data());

        filter->setImage(d->imgRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

