#ifndef medAbstractDbController_h__
#define medAbstractDbController_h__


#include "medCoreExport.h"
#include "medDataIndex.h"
#include <QtCore>
#include <QtSql>

class dtkAbstractData;

/**
 * Abstract dbController class. Implementation needs to adhere to the common interface
 */
class MEDCORE_EXPORT medAbstractDbController : public QObject
{
    Q_OBJECT

public:


    /**
    * Get the status of the Db connection
    * @return bool true if connected
    */
    virtual bool isConnected() = 0;

signals:
    
    /**
     * signal each time the db gets modified, giving the dataindex that was involved
     */
    void updated(medDataIndex);

public slots:

    /**
    * Returns pointer to dtkAbstractData read from db or NULL
    * @params const medDataIndex & index Index for data
    * @return dtkAbstractData * the data
    */
    virtual dtkAbstractData *read(const medDataIndex& index) const = 0;
 
    /**
    * Import a file into the db
    * @params const QString & file Filename
    * @return medDataIndex that was assigned
    */
    virtual medDataIndex import(const QString& file) = 0;

    /**
    * Import a data into the db
    * @params const dtkAbstractData& data reference to data
    * @return medDataIndex that was assigned
    */
    virtual medDataIndex import(const dtkAbstractData& data) = 0;

    /**
     * This method allows importing data from other databases
     */
    virtual medDataIndex import(const medDataIndex& index, const medAbstractDbController& controller);

};

#endif // medAbstractDbController_h__
