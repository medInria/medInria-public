// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medRegistrationNode.h"
#include "medAbstractImageData.h"
#include "medCore.h"

class medRegistrationNodePrivate
{
public:

    dtkComposerTransmitterReceiver< medAbstractImageData* >     fixedImageRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     movingImageRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
    dtkComposerTransmitterEmitter< QTransform >      transfoEmt;
};

medRegistrationNode::medRegistrationNode(void) : dtkComposerNodeObject<medAbstractRigidRegistration>(), d(new medRegistrationNodePrivate())
{
    this->setFactory(medCore::registration::rigid::pluginFactory());

    this->appendReceiver(&d->movingImageRecv);
    this->appendReceiver(&d->fixedImageRecv);

    this->appendEmitter (&d->imgEmt);
    this->appendEmitter (&d->transfoEmt);
}

medRegistrationNode::~medRegistrationNode(void)
{
    delete d;
}

void medRegistrationNode::run(void)
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
        //d->imgEmt.setData(process->transformedImage());
        
        qDebug()<<"filtering done";
    }
}

