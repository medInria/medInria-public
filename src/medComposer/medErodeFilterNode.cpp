// Copyright: INRIA

#include <dtkComposer>

#include "medErodeFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medErodeFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    radiusRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medErodeFilterNode::medErodeFilterNode(void) : dtkComposerNodeObject<medAbstractErodeFilter>(), d(new medErodeFilterNodePrivate())
{
    this->setFactory(medCore::filtering::erode::pluginFactory());

    this->appendReceiver(&d->radiusRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medErodeFilterNode::~medErodeFilterNode(void)
{
    delete d;
}

void medErodeFilterNode::run(void)
{
    if (d->radiusRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractErodeFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setRadius(d->radiusRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

