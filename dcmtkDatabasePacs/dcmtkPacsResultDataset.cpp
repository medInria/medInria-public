#include "dcmtkPacsResultDataset.h"

#include "dcmtkPacsKey.h";
#include <medPacs/medAbstractPacsKey.h>


void dcmtkPacsResultDataset::convert( dcmtkResultDataset* dataset )
{
    dcmtkContainer<dcmtkKey*>* container = dataset->getKeyContainer();
    for (int i = 0; i<container->size(); i++)
    {
        dcmtkPacsKey* ds = new dcmtkPacsKey;
        ds->convert(container->getAtPos(i));
        m_copyCont<<ds;
    }
}
