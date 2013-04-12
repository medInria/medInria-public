#pragma once

#include <QtCore/QObject>
#include <QVector>
#include "medPacsExport.h"

class medAbstractPacsKey;

class MEDPACS_EXPORT medAbstractPacsResultDataset : public QObject
{
    Q_OBJECT

public:
             medAbstractPacsResultDataset();
    virtual ~medAbstractPacsResultDataset();

    virtual const char* getStudyInstanceUID();

    virtual const char* getSeriesInstanceUID();

    virtual const char* getSOPInstanceUID();

    virtual QVector<medAbstractPacsKey*> getKeyContainer();

    virtual const char* findKeyValue(int group, int elem);

};


