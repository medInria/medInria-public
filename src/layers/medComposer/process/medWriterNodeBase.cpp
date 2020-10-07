/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
        if(!d->dataRecv.data())
        {
            qWarning()<<Q_FUNC_INFO<<"no data to write";
        }
        if(medDataReaderWriter::write(d->pathRecv.data(),d->dataRecv.data()))
            qDebug()<<"wrote data successfully at "<<d->pathRecv.data();
        else
            qDebug()<<"couldn't write: "<<d->pathRecv.data()<<d->dataRecv.data()->identifier();
    }
}

