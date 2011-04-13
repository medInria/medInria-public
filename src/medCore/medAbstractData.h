#ifndef medAbstractData_h__
#define medAbstractData_h__

#include <dtkCore/dtkAbstractData.h>
#include "medCoreExport.h"

class medAbstractDataPrivate;

/**
 * Extending dtkAbstractData class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractData : public dtkAbstractData
{
    Q_OBJECT

public:
    medAbstractData( medAbstractData *parent = 0);
    medAbstractData(const medAbstractData& data);
    virtual ~medAbstractData(void);

private:
    medAbstractDataPrivate* d;

};

#endif // medAbstractData_h__
