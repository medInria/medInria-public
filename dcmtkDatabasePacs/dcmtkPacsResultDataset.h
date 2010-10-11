#ifndef DCMTKPACSRESULTDATASET_H
#define DCMTKPACSRESULTDATASET_H

#include <QVector>

#include "dcmtkResultDataset.h"

#include <medPacs/medAbstractPacsResultDataset.h>

class medAbstractPacsKey;

class dcmtkPacsResultDataset : public medAbstractPacsResultDataset
{
    Q_OBJECT

public:
    void convert(dcmtkResultDataset* dataset);


    const char* getStudyInstanceUID(void);

    const char* getSeriesInstanceUID(void);

    const char* getSOPInstanceUID(void);

    QVector<medAbstractPacsKey*> getKeyContainer(void);

    const char* findKeyValue(int group, int elem);

protected:
    
    QVector<medAbstractPacsKey*> m_copyCont;
    dcmtkResultDataset*          m_dataset;
};

#endif DCMTKPACSRESULTDATASET_H
