/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractPacsResultDataset.h>

#include <dtkCoreSupport/dtkGlobal.h>

#include <QtCore>

medAbstractPacsResultDataset::medAbstractPacsResultDataset( void )
{

}

medAbstractPacsResultDataset::~medAbstractPacsResultDataset( void )
{

}

const char* medAbstractPacsResultDataset::getStudyInstanceUID()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}


const char* medAbstractPacsResultDataset::getSeriesInstanceUID()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

const char* medAbstractPacsResultDataset::getSOPInstanceUID()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

QVector<medAbstractPacsKey*> medAbstractPacsResultDataset::getKeyContainer()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return QVector<medAbstractPacsKey*>();
}

const char* medAbstractPacsResultDataset::findKeyValue( int group, int elem )
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}
