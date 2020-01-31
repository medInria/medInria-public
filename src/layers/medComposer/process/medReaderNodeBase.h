#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkComposer>

#include "medDataReaderWriter.h"
#include "medAbstractData.h"
#include "medAbstractImageData.h"
#include "medAbstractMeshData.h"

template <typename T> class medReaderNodeBasePrivate;

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

template <typename T> class medReaderNodeBase : public dtkComposerNodeLeaf
{
public:
     medReaderNodeBase(void);
    ~medReaderNodeBase(void);

public:
    void run(void);

private:
    medReaderNodeBasePrivate<T> *d;
};

// ///////////////////////////////////////////////////////////////////

template <typename T> class medReaderNodeBasePrivate
{
public:
    dtkComposerTransmitterReceiver< QString > pathRecv;

public:
    dtkComposerTransmitterEmitter< T* > outTypeEmt;

public:
    T* data;
};

template <typename T> inline medReaderNodeBase<T>::medReaderNodeBase(void) : d(new medReaderNodeBasePrivate<T>())
{
    this->appendReceiver(&d->pathRecv);
    this->appendEmitter(&d->outTypeEmt);

    d->data = 0;
}

template <typename T> inline medReaderNodeBase<T>::~medReaderNodeBase(void)
{
    delete d;
}

template <typename T> inline void medReaderNodeBase<T>::run(void)
{
    if (d->pathRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        medAbstractData* data=medDataReaderWriter::read(d->pathRecv.data());
        if(!data)
        {
            qWarning()<<"no data found";
            return;
        }

        T* specificData=dynamic_cast<T*>(data);
        if(specificData)
            d->outTypeEmt.setData(specificData);
        else
            qWarning()<<"the data does not match the expected type";
    }
}

typedef medReaderNodeBase<medAbstractData> medGenericReaderNode;
typedef medReaderNodeBase<medAbstractImageData> medImageReaderNode;
typedef medReaderNodeBase<medAbstractMeshData> medMeshReaderNode;


//
// medBaseReaderNode.h ends here
