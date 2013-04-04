#pragma once

#include "medCoreExport.h"

#include <medAttachedData.h>

class medAbstractData;

/** 
 * Base class for annotations : a specialization of medAttachedData.
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



