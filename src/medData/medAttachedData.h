
#ifndef medAttachedData_h__
#define medAttachedData_h__

#include <medData/medAbstractData.h>
#include "medCoreExport.h"

class medAttachedDataPrivate;

/**
 * Class to store attachments to datasets
 */
class MEDDATA_EXPORT medAttachedData : public dtkAbstractData
{
    Q_OBJECT

public:
    medAttachedData( medAttachedData *parent = 0);
    virtual ~medAttachedData(void);

private:
    medAttachedDataPrivate* d;

};

#endif // medAttachedData_h__

