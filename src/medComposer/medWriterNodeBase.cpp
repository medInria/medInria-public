// Copyright: INRIA

#pragma once

#include <dtkComposer>

#include "medWriterNodeBase.h"

#include "medDataReaderWriter.h"
#include "medAbstractData.h"
#include "medAbstractImageData.h"
#include "medAbstractMeshData.h"




class medWriterNodeBasePrivate
{
public:
    dtkComposerTransmitterReceiver< QString > pathRecv;
    dtkComposerTransmitterReceiver< medAbstractData* >      dataRecv;
};

medWriterNodeBase::medWriterNodeBase(void) : d(new medWriterNodeBasePrivate())
{
    this->appendReceiver(&d->pathRecv);
    this->appendReceiver(&d->dataRecv);
}

medWriterNodeBase::~medWriterNodeBase(void)
{
    delete d;
}

void medWriterNodeBase::run(void)
{
    if (d->pathRecv.isEmpty() || d->dataRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        medDataReaderWriter::write(d->pathRecv.data(),d->dataRecv.data());
    }
}

