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

    // The data may belong to a parent dataset.
    medAbstractData * parentData() const;
    void setParentData( medAbstractData * mdata );

signals :
    /** Emitted when the data is changed */
    void dataModified();

public slots:
protected:

private:

    class medAnnotationDataPrivate;
    medAnnotationDataPrivate * d;
};

#endif // MEDANNOTATIONDATA_H

