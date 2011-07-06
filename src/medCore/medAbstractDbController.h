#ifndef medAbstractDbController_h__
#define medAbstractDbController_h__


#include "medCoreExport.h"
#include "medDataIndex.h"
#include <QtCore>
#include <QtSql>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

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
    
    /**
    * return the size that the data behind the medDataIndex in byte
    * @param const medDataIndex& index the index for the data
    * @return estimated size of data
    */
    virtual qint64 getEstimatedSize(const medDataIndex& index) const = 0;

    /** Unique Id used in medDataIndex to recognize this DB.*/
    virtual int dataSourceId() const = 0;

    /** Enumerate all patients stored in this DB*/
    virtual QList<int> patients() const = 0;

    /** Enumerate all studies for given patient*/
    virtual QList<int> studies(int patientId ) const = 0;

    /** Enumerate all series for given study*/
    virtual QList<int> series(int patientId, int studyId ) const = 0;

    /** Enumerate all images for given series*/
    virtual QList<int> images(int patientId, int studyId, int seriesId ) const = 0;

    /** Get metadata for specific item. Return uninitialized string if not present. */
    virtual QString metaData(const medDataIndex& index, const QString& key) const = 0;

    /** Set metadata for specific item. Return true on success, false otherwise. */
    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value) = 0;

    /** Get thumbnail. */
    virtual QImage thumbnail( const medDataIndex& index) const = 0;

    /** return true if data is stored in persistent storage*/
    virtual bool isPersistent(const medDataIndex& index) const = 0;

signals:
    
    /**
     * signal each time the db gets modified, giving the dataindex that was involved
     */
    void updated(medDataIndex);

public slots:

    /**
    * Returns pointer to dtkAbstractData read from db (physical or virtual) or NULL
    * @Note There is no read(QString file) method, as you can achieve this using import(QString) and read(index) 
    * using the index returned from the import
    * @params const medDataIndex & index Index for data
    * @return dtkAbstractData * the data
    */
    virtual dtkSmartPointer<dtkAbstractData> read(const medDataIndex& index) const = 0;

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
    virtual medDataIndex import(dtkAbstractData *data) = 0;
  
    /**
     * This method allows importing data from other databases
     * @params const medDataIndex & index The data index used in the referenced db (source)
     * @params const medAbstractDbController & controller  The referenced db (source)
     * @return medDataIndex New assigned dataIndex in target (this) db
     */
    virtual medDataIndex import(const medDataIndex& index, const medAbstractDbController& controller);
    
    /**
     * This method allows removing one data from the database
     * @params const medDataIndex & index The data index to be removed in the db
     */
    virtual void remove(const medDataIndex& index);
    
    /**
    * This method clears data already loaded in the database.
    */
    virtual void clear (void);

};

#endif // medAbstractDbController_h__
