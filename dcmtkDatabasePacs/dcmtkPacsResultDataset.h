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


private:
    QVector<medAbstractPacsKey*>     m_copyCont;


};


#endif DCMTKPACSRESULTDATASET_H