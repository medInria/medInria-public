#include "dcmtkPacsResultDataset.h"

#include "dcmtkPacsKey.h"
#include <medPacs/medAbstractPacsKey.h>


void dcmtkPacsResultDataset::convert( dcmtkResultDataset* dataset )
{
    m_dataset = dataset;

    dcmtkContainer<dcmtkKey*>* container = dataset->getKeyContainer();
    for (int i = 0; i<container->size(); i++)
    {
        dcmtkPacsKey* ds = new dcmtkPacsKey;
        ds->convert(container->getAtPos(i));
        m_copyCont<<ds;
    }
}

const char* dcmtkPacsResultDataset::getStudyInstanceUID( void )
{
    return m_dataset->getStudyInstanceUID();
}


const char* dcmtkPacsResultDataset::getSeriesInstanceUID( void )
{
    return m_dataset->getSeriesInstanceUID();
}


const char* dcmtkPacsResultDataset::getSOPInstanceUID( void )
{
    return m_dataset->getSOPInstanceUID();
}


QVector<medAbstractPacsKey*> dcmtkPacsResultDataset::getKeyContainer( void )
{
    return m_copyCont;
}

const char* dcmtkPacsResultDataset::findKeyValue( int group, int elem )
{
    return m_dataset->findKeyValue(group, elem);
}
