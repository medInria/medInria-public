#ifndef MEDANNOTATIONDATA_H
#define MEDANNOTATIONDATA_H

#include "medCoreExport.h"

#include <medAttachedData.h>

class medAbstractData;

/**
 * Base class for the data of view annotations.
 * 
 ***/
class MEDCORE_EXPORT medAnnotationData : public medAttachedData
{
    Q_OBJECT;

public:
    medAnnotationData( );
    virtual ~medAnnotationData();


signals:

public slots:


protected:

private:

    class medAnnotationDataPrivate;
    medAnnotationDataPrivate * d;
};

#endif // MEDANNOTATIONDATA_H

