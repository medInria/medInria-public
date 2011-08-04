#ifndef medAbstractData_h__
#define medAbstractData_h__

#include <dtkCore/dtkAbstractData.h>

#include "medCoreExport.h"

class medDataIndex;
class medAbstractDataPrivate;

/**
 * Extending medAbstractData class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractData : public dtkAbstractData
{
    Q_OBJECT

public:
    medAbstractData( dtkAbstractData *parent = 0);
    virtual ~medAbstractData(void);

    /**
    * Attach a meddataindex to the data to carry it arround
    * @params const medDataIndex & index
    */
    void setDataIndex(const medDataIndex& index);

    /**
    * Get the dataindex attached to the data or an invalid one
    * @return medDataIndex
    */
    medDataIndex dataIndex() const;

    /**
    * Set this to true if the volume has been generated with correct values (e.g. for z-dimension)
    * @params bool flag
    * @return void
    */
    void setTrueVolumetric(bool flag);

    /**
    * if set to true, the dataset is a valid 3d volume
    * else operation like MPR, VRT should not be possible
    * @return bool
    */
    bool trueVolumetric() const;

private:
    medAbstractDataPrivate* d;

};

#endif // medAbstractData_h__
