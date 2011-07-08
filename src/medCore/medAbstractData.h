#ifndef medAbstractData_h__
#define medAbstractData_h__

#include <dtkCore/dtkAbstractData.h>

#include "medCoreExport.h"

class medAbstractDataPrivate;

class medAttachedData;

/**
 * Extending dtkAbstractData class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractData : public dtkAbstractData
{
    Q_OBJECT

public:
    medAbstractData( dtkAbstractData * parent = NULL );
    virtual ~medAbstractData(void);

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
    bool trueVolumetric();

    /**
     * @brief Get attached data (like histogram, annotations etc.)
     *
     * @return QList< medAttachedData * >
     */
    QList< medAttachedData * > attachedData() const;

    /**
     * @brief Clear the list of attached data
     *
     * @return void
     */
    void clearAttachedData();

    /**
     * @brief add attached data
     *
     * @return void
     */
    void addAttachedData( medAttachedData * data);

    /**
     * @brief remove attached data
     *
     * @return void
     */
    void removeAttachedData( medAttachedData * data);

private:
    medAbstractDataPrivate* d;

};

#endif // medAbstractData_h__
