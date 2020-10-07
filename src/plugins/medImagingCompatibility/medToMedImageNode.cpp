/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkComposer>

#include <medCore.h>

#include <medToMedImageNode.h>

#include <medDataReaderWriter.h>
#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractMeshData.h>

class medToMedImageNodePrivate
{
public:
    dtkComposerTransmitterReceiver< dtkImage* > dataRecv;
    dtkComposerTransmitterEmitter< medAbstractImageData* > imgEmt;
};

medToMedImageNode::medToMedImageNode(void) : d(new medToMedImageNodePrivate())
{
    this->setFactory(medCore::dataConverter::pluginFactory());

    this->appendReceiver(&d->dataRecv);
    this->appendEmitter(&d->imgEmt);
}

medToMedImageNode::~medToMedImageNode(void)
{
    delete d;
}

void medToMedImageNode::run(void)
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
            d->imgEmt.setData(converter->toMedImage(d->dataRecv.data()));
        }
    }
}
