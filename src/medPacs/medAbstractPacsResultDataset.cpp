#include "medAbstractPacsResultDataset.h"

#include <dtkCore/dtkGlobal.h>

#include <QtCore>

medAbstractPacsResultDataset::medAbstractPacsResultDataset()
{

}

medAbstractPacsResultDataset::~medAbstractPacsResultDataset()
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
