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

    const char* getStudyInstanceUID();
    void setStudyInstanceUID(const char* uid);

    const char* getSeriesInstanceUID();
    void setSeriesInstanceUID(const char* uid);

    const char* getSOPInstanceUID();
    void setSOPInstanceUID(const char* uid);

    QVector<medAbstractPacsKey*>* getKeyContainer();

};

#endif