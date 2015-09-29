// Copyright: INRIA

#include <dtkComposer>

#include "medCloseFilterNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medCloseFilterNodePrivate
{
public:
    dtkComposerTransmitterReceiver< double >                    radiusRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     imgRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
};

medCloseFilterNode::medCloseFilterNode(void) : dtkComposerNodeObject<medAbstractCloseFilter>(), d(new medCloseFilterNodePrivate())
{
    this->setFactory(medCore::filtering::close::pluginFactory());

    this->appendReceiver(&d->radiusRecv);
    this->appendReceiver(&d->imgRecv);
    
    this->appendEmitter (&d->imgEmt);
}

medCloseFilterNode::~medCloseFilterNode(void)
{
    delete d;
}

void medCloseFilterNode::run(void)
{
    if (d->radiusRecv.isEmpty() || d->imgRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractCloseFilter* filter = this->object();
        if(!this->object())
            return;
        filter->setImage(d->imgRecv.data());
        filter->setRadius(d->radiusRecv.data());
        filter->run();
        d->imgEmt.setData(filter->filteredImage());
        qDebug()<<"filtering done";
    }
}

