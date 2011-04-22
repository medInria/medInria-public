
#ifndef medAttachedData_h__
#define medAttachedData_h__

#include <dtkCore/dtkAbstractData.h>
#include "medCoreExport.h"

class medAttachedDataPrivate;

/**
 * Class to store attachments to datasets
 */
class MEDCORE_EXPORT medAttachedData : public dtkAbstractData
{
    Q_OBJECT

public:
    medAttachedData( medAttachedData *parent = 0);
    virtual ~medAttachedData(void);

private:
    medAttachedDataPrivate* d;

};

#endif // medAttachedData_h__

