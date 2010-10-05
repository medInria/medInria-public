#ifndef DCMTKPACSRESULTDATASET_H
#define DCMTKPACSRESULTDATASET_H


#include <QVector>
#include "dcmtkResultDataset.h"
#include <medPacs/medAbstractPacsResultDataset.h>


class dcmtkPacsResultDataset : public medAbstractPacsResultDataset
{
    Q_OBJECT

public:


    void convert(dcmtkResultDataset* dataset);


private:



};


#endif DCMTKPACSRESULTDATASET_H