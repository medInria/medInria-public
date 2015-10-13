// Copyright: INRIA

#include <dtkComposer>

#include "medAbstractSegmentationProcess.h"
#include "medSegmentationNode.h"
#include "dtkComposerNodeMetaData.h"

class medSegmentationNodePrivate
{
public:
        dtkComposerTransmitterReceiver< medAbstractImageData* >     imageRecv;
};

medSegmentationNode::medSegmentationNode(void) : dtkComposerNodeObject<medAbstractSegmentationProcess>(), d(new medSegmentationNodePrivate())
{
    this->setFactory(medCore::segmentation::pluginFactory());
    this->appendReceiver(&d->imageRecv);
}

medSegmentationNode::~medSegmentationNode(void)
{
    delete d;
}

void medSegmentationNode::run(void)
{
    if ( d->imageRecv.isEmpty() || !d->imageRecv.data())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {

        medAbstractSegmentationProcess* process = this->object();
        if(!this->object())
            return;
        process->setImage(d->imageRecv.data());
        process->run();
        qDebug()<<"segmentation done";
    }
}

