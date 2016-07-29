/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkComposer>

#include <medCore.h>

#include <medToDtkImageNode.h>

#include <medDataReaderWriter.h>
#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractMeshData.h>

class medToDtkImageNodePrivate
{
public:
    dtkComposerTransmitterReceiver< medAbstractImageData* > dataRecv;
    dtkComposerTransmitterEmitter< dtkImage* > imgEmt;
};

medToDtkImageNode::medToDtkImageNode(void) : d(new medToDtkImageNodePrivate())
{
    this->setFactory(medCore::dataConverter::pluginFactory());

    this->appendReceiver(&d->dataRecv);
    this->appendEmitter(&d->imgEmt);
}

medToDtkImageNode::~medToDtkImageNode(void)
{
    delete d;
}

void medToDtkImageNode::run(void)
{
    if ( d->dataRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        if(!d->dataRecv.data())
        {
            qWarning()<<Q_FUNC_INFO<<"no data to convert";
        }

        medAbstractDataConverter* converter= this->object();
        if(converter)
        {
            d->imgEmt.setData(converter->toDtkImage(d->dataRecv.data()));
        }
    }
}
