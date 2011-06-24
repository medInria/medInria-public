
#ifndef medAttachedData_h__
#define medAttachedData_h__

#include <medCore/medAbstractData.h>
#include "medCoreExport.h"

class medAttachedDataPrivate;

/**
 * Class to store attachments to datasets
 */
class MEDCORE_EXPORT medAttachedData : public medAbstractData
{
    Q_OBJECT

public:
    medAttachedData( medAttachedData *parent = NULL );
    virtual ~medAttachedData(void);

private:
    medAttachedDataPrivate* d;
};

#endif // medAttachedData_h__

