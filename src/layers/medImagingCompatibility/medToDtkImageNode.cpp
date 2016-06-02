// Copyright: INRIA

#include <dtkComposer>

#include "medCore.h"

#include "medToDtkImageNode.h"

#include "medDataReaderWriter.h"
#include "medAbstractData.h"
#include "medAbstractImageData.h"
#include "medAbstractMeshData.h"




class medToDtkImageNodePrivate
{
public:
    dtkComposerTransmitterReceiver< medAbstractImageData* >                        dataRecv;
    dtkComposerTransmitterEmitter< dtkImage* >               imgEmt;
};

medToDtkImageNode::medToDtkImageNode(void) : d(new medToDtkImageNodePrivate())
{
    this->setFactory(medCore::converter::pluginFactory());

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

        medAbstractConverter* converter= this->object();
        if(converter)
        {
            d->imgEmt.setData(converter->toDtkImage(d->dataRecv.data()));
        }
    }
}

