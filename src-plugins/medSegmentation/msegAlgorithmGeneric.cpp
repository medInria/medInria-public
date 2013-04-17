/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "msegAlgorithmGeneric.h"

// Temporary fudge:

#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractData.h>

#include <dtkLog/dtkLog.h>

namespace mseg {

QString AlgorithmGeneric::ms_interfaceName = "mseg::AlgorithmGeneric";

class AlgorithmGenericPrivate 
{
    friend class AlgorithmGeneric;
    typedef QHash< QString, HandlerFunc * > HandlerContainerType;
    HandlerContainerType handlers;

    dtkSmartPointer< dtkAbstractData > inputData;
    dtkSmartPointer< dtkAbstractData > outputData;
};

AlgorithmGeneric::AlgorithmGeneric() :
 d (new AlgorithmGenericPrivate)
{

}

AlgorithmGeneric::~AlgorithmGeneric()
{
    qDeleteAll(d->handlers);
    delete d;
}

void AlgorithmGeneric::addHandler( const QString & typeName, HandlerFunc * func )
{
    typedef HandlerFunc * HandlerFuncPtr;
    // Should return NULL if not already present.
    // Get a reference so we can assign to it later.
    HandlerFuncPtr & oldFunc( d->handlers[ typeName ]);
     if ( oldFunc )
         delete oldFunc;
     oldFunc = func;
}

int AlgorithmGeneric::callHandler( dtkAbstractData * data )
{
    if ( !data  ) {
        dtkWarn() << "Attempt to run segmentation algorithm with no input set.";
        return DTK_FAILURE;
    }

    const QString dataId( data->identifier() );

    AlgorithmGenericPrivate::HandlerContainerType::const_iterator it( d->handlers.find( dataId ) );
    if ( it == d->handlers.end() ) {
        dtkWarn() << "Unknown data type encountered " << dataId;
        return DTK_FAILURE;
    }

    return (*it)->run(data);
}

bool AlgorithmGeneric::isHandled( const QString & dataId ) const
{
    AlgorithmGenericPrivate::HandlerContainerType::const_iterator it( d->handlers.find( dataId ) );
    return ( it != d->handlers.end() );
}

void AlgorithmGeneric::setInput( dtkAbstractData * data )
{
    this->d->inputData = data;
}

int AlgorithmGeneric::update()
{
    int ret = this->callHandler( this->input() );
    this->setOutputMetadata();

    return ret;
}

void AlgorithmGeneric::setOutput( dtkAbstractData * data )
{
    this->d->outputData = data;
}

void AlgorithmGeneric::reportItkProgress( const itk::Object * caller , float progress )
{
    emit progressed( static_cast<int>( progress * 100.) );
}

bool AlgorithmGeneric::isUndoAble()
{
    return false;
}

void AlgorithmGeneric::undo()
{

}

dtkAbstractData * AlgorithmGeneric::input()
{
    return d->inputData;
}

dtkAbstractData * AlgorithmGeneric::output()
{
    return d->outputData;
}

void AlgorithmGeneric::setOutputMetadata()
{
    if ( !d->inputData ) 
        return;

    QStringList metaDataToCopy;
    metaDataToCopy << medMetaDataKeys::PatientName.key()
        << medMetaDataKeys::StudyDescription.key();

    foreach( const QString & key, metaDataToCopy ) {
        d->outputData->setMetaData(key, d->inputData->metadatas(key));
    }

    QString seriesDesc;
    seriesDesc = tr("Segmented from ") + medMetaDataKeys::SeriesDescription.getFirstValue( d->inputData );

    d->outputData->setMetaData(medMetaDataKeys::SeriesDescription.key(),seriesDesc);
}


} // namespace mseg

