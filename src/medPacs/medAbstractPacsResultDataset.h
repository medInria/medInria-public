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
    virtual void setStudyInstanceUID(const char* uid);

    virtual const char* getSeriesInstanceUID(void);
    virtual void setSeriesInstanceUID(const char* uid);

    virtual const char* getSOPInstanceUID(void);
    virtual void setSOPInstanceUID(const char* uid);

    virtual QVector<medAbstractPacsKey*>* getKeyContainer(void);

};

#endif
