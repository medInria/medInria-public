#ifndef MEDANNOTATIONDATA_H
#define MEDANNOTATIONDATA_H

#include "medAbstractViewExtensionExport.h"

#include <dtkCore/dtkAbstractData.h>

class medAbstractData;

/**
 * Base class for the data of view annotations.
 * 
 ***/
class MEDABSTRACTVIEWEXTENSION_EXPORT medAnnotationData : public dtkAbstractData
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

