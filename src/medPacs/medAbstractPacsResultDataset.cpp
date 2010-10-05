#include "medAbstractPacsResultDataset.h"

#include <dtkCore/dtkGlobal.h>

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

void medAbstractPacsResultDataset::setStudyInstanceUID( const char* uid )
{
    DTK_UNUSED(uid);

    DTK_DEFAULT_IMPLEMENTATION;
}

const char* medAbstractPacsResultDataset::getSeriesInstanceUID()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

void medAbstractPacsResultDataset::setSeriesInstanceUID( const char* uid )
{
    DTK_UNUSED(uid);

    DTK_DEFAULT_IMPLEMENTATION;
}

const char* medAbstractPacsResultDataset::getSOPInstanceUID()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

void medAbstractPacsResultDataset::setSOPInstanceUID( const char* uid )
{
    DTK_UNUSED(uid);

    DTK_DEFAULT_IMPLEMENTATION;
}

QVector<medAbstractPacsKey*>* medAbstractPacsResultDataset::getKeyContainer()
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}
