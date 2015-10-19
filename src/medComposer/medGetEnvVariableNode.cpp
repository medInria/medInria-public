// Copyright: INRIA

#include <dtkComposer>

#include "medGetEnvVariableNode.h"

Q_DECLARE_METATYPE(QProcessEnvironment);

class medGetEnvVariableNodePrivate
{
public:
    dtkComposerTransmitterReceiver< QString > nameRecv;
    dtkComposerTransmitterReceiver< QProcessEnvironment > envRecv;
    
    dtkComposerTransmitterEmitter< QString > valueEmt;
};

medGetEnvVariableNode::medGetEnvVariableNode(void) : d(new medGetEnvVariableNodePrivate())
{
    this->appendReceiver(&d->nameRecv);
    this->appendReceiver(&d->envRecv);
    
    this->appendEmitter(&d->valueEmt);
}

medGetEnvVariableNode::~medGetEnvVariableNode(void)
{
    delete d;
}

void medGetEnvVariableNode::run(void)
{
    if (d->nameRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input \"name\" is not set. Aborting.";
        return;
    }    
    if (d->envRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input \"env\" is not set. Aborting.";
        return;
    }
    d->valueEmt.setData(d->envRecv.data().value(d->nameRecv.data()));
}

