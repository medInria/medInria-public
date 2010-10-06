#ifndef MEDABSTRACTPACSRESULTDATASET_H
#define MEDABSTRACTPACSRESULTDATASET_H

#include <QtCore/QObject>
#include <QVector>
#include "medPacsExport.h"

class medAbstractPacsKey;

class MEDPACS_EXPORT medAbstractPacsResultDataset : public QObject
{
    Q_OBJECT

public:
             medAbstractPacsResultDataset(void);
    virtual ~medAbstractPacsResultDataset(void);

    virtual const char* getStudyInstanceUID(void);

    virtual const char* getSeriesInstanceUID(void);

    virtual const char* getSOPInstanceUID(void);

    virtual QVector<medAbstractPacsKey*> getKeyContainer(void);

    virtual const char* findKeyValue(int group, int elem);

};

#endif
