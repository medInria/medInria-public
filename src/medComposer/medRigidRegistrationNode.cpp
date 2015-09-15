// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medRigidRegistrationNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medRigidRegistrationNodePrivate
{
public:
  
    dtkComposerTransmitterReceiver< medAbstractImageData* >     movingImageRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     fixedImageRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
    dtkComposerTransmitterEmitter< QMatrix4x4 >                 transfoEmt;
};

medRigidRegistrationNode::medRigidRegistrationNode(void) : dtkComposerNodeObject<medAbstractRigidRegistration>(), d(new medRigidRegistrationNodePrivate())
{
    this->setFactory(medCore::registration::rigid::pluginFactory());
    
    this->appendReceiver(&d->fixedImageRecv);
    this->appendReceiver(&d->movingImageRecv);

    this->appendEmitter (&d->transfoEmt);
    this->appendEmitter (&d->imgEmt);

}

medRigidRegistrationNode::~medRigidRegistrationNode(void)
{
    delete d;
}

void medRigidRegistrationNode::run(void)
{
    qDebug()<< Q_FUNC_INFO ;
    if (d->fixedImageRecv.isEmpty() || d->movingImageRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        medAbstractRigidRegistration* process = this->object();
        if(!process)
        {
            qDebug()<<"no implementation found";
            return;
        }
        qDebug()<<"setting inputs";
        process->setMovingImage(d->movingImageRecv.data());
        process->setFixedImage(d->fixedImageRecv.data());
        process->run();
        d->imgEmt.setData(process->transformedImage());

        qDebug()<<"filtering done";
    }
}

