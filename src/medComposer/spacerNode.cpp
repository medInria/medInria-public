// Copyright: INRIA

#include <dtkComposer>

#include "spacerNode.h"

class SpacerNodePrivate
{
public:
    dtkComposerTransmitterReceiver< QString > stringRecv;
    dtkComposerTransmitterEmitter<  QString >  stringEmt;
};

SpacerNode::SpacerNode(void) : d(new SpacerNodePrivate())
{
    this->appendReceiver(&d->stringRecv);
    this->appendEmitter(&d->stringEmt);
}

SpacerNode::~SpacerNode(void)
{
    delete d;
}

void SpacerNode::run(void)
{
    if (d->stringRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input \"name\" is not set. Aborting.";
        return;
    }    

    d->stringEmt.setData(d->stringRecv.data()+" ");
}

