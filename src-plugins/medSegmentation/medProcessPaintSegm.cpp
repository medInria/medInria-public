/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medProcessPaintSegm.h"

#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkLog/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>

// /////////////////////////////////////////////////////////////////
// medProcessPaintSegmPrivate
// /////////////////////////////////////////////////////////////////

class medProcessPaintSegmPrivate
{
public:
    dtkSmartPointer<medAbstractData> imageInput;
    dtkSmartPointer<medAbstractData> maskInput;
    dtkSmartPointer<medAbstractData> segmentationOutput;
};


// /////////////////////////////////////////////////////////////////
// medProcessPaintSegm
// /////////////////////////////////////////////////////////////////

medProcessPaintSegm::medProcessPaintSegm() : dtkAbstractProcess(), d(new medProcessPaintSegmPrivate)
{

}

medProcessPaintSegm::~medProcessPaintSegm()
{
    delete d;
    d = NULL;
}

bool medProcessPaintSegm::registered()
{
    return dtkAbstractProcessFactory::instance()->registerProcessType(
        medProcessPaintSegm::s_identifier(),
        medProcessPaintSegm::create);
}

QString medProcessPaintSegm::description() const
{
    return tr("Paint");
}

QString medProcessPaintSegm::identifier() const
{
    return medProcessPaintSegm::s_identifier();
}

QString medProcessPaintSegm::s_identifier()
{
     return "medProcessPaintSegm";
}

void medProcessPaintSegm::setInput( medAbstractData *data, int channel )
{
    switch( channel ) {
    case ImageChannel : 
        d->imageInput = data;
        break;
    case MaskChannel : 
        d->maskInput = data;
        break;
    default :
        dtkWarn() << "Attempt to set channel out of range";
        break;
    }
}

void * medProcessPaintSegm::data()
{

    return NULL;
}

int medProcessPaintSegm::update()
{
    if (!d->imageInput || !d->maskInput) {
        return DTK_FAILURE;
    }

    this->setOutputMetadata(d->imageInput, d->maskInput);

    d->segmentationOutput = d->maskInput;

    return DTK_SUCCEED;
}

QString medProcessPaintSegm::MaskImageTypeIdentifier()
{
    return "itkDataImageUChar3";
}

medAbstractData * medProcessPaintSegm::output()
{
    return d->segmentationOutput;
}

void medProcessPaintSegm::setOutputMetadata(const medAbstractData * inputData, medAbstractData * outputData)
{
    Q_ASSERT(outputData && inputData);

    QStringList metaDataToCopy;
    metaDataToCopy 
        << medMetaDataKeys::PatientName.key()
        << medMetaDataKeys::StudyDescription.key();

    foreach( const QString & key, metaDataToCopy ) {
        outputData->setMetaData(key, inputData->metadatas(key));
    }

    QString seriesDesc;
    seriesDesc = tr("Segmented from ") + medMetaDataKeys::SeriesDescription.getFirstValue( inputData );

    medMetaDataKeys::SeriesDescription.set(outputData,seriesDesc);
}

// /////////////////////////////////////////////////////////////////
// Type instantiation
// /////////////////////////////////////////////////////////////////
dtkAbstractProcess * medProcessPaintSegm::create()
{
    return new medProcessPaintSegm;
}
