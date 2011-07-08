#ifndef medDbController_h__
#define medDbController_h__


#include "medSqlExport.h"
#include <medCore/medAbstractDbController.h>

class dtkAbstractData;
class SigEmitter;
class medDatabaseControllerImplPrivate;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDSQL_EXPORT medDatabaseControllerImpl: public medAbstractDbController 
{
    Q_OBJECT

public:

    /* constructor */
    medDatabaseControllerImpl();
    ~medDatabaseControllerImpl();

    QSqlDatabase *database(void);

    bool createConnection(void);
    bool  closeConnection(void);

    /* create dataIndices out of partial ids */
    medDataIndex indexForPatient(int id);
    medDataIndex indexForStudy  (int id);
    medDataIndex indexForSeries (int id);
    medDataIndex indexForImage  (int id);

    /**
    * Returns the index of a data given patient, study, series and image name
    */
    medDataIndex indexForPatient (const QString &patientName);
    medDataIndex indexForStudy   (const QString &patientName, const QString &studyName);
    medDataIndex indexForSeries  (const QString &patientName, const QString &studyName,
                                  const QString &seriesName);
    medDataIndex indexForImage   (const QString &patientName, const QString &studyName,
                                  const QString &seriesName,  const QString &imageName);

    /* some overloads of read(medDataIndex) */
    dtkSmartPointer<dtkAbstractData> read(int patientId, int studyId, int seriesId);
    dtkSmartPointer<dtkAbstractData> read(int patientId, int studyId, int seriesId, int imageId);

    /** Remove / replace characters to transform into a pathname component. */
    QString stringForPath(const QString & name) const;

    /**
    * Change the storage location of the database by copy, verify, delete
    * @params QString newLocation path of new storage location, must be empty
    * @return bool true on success
    */
    bool moveDatabase(QString newLocation);

    /**
    * Status of connection
    * @return bool true on success
    */
    bool isConnected();

    /**
    * return the size that the data behind the medDataIndex in byte
    * @param const medDataIndex& index the index for the data
    * @return estimated size of data
    */
    qint64 getEstimatedSize(const medDataIndex& index) const;

    /** Enumerate all patients stored in this DB*/
    virtual QList<medDataIndex> patients() const;

    /** Enumerate all studies for given patient*/
    virtual QList<medDataIndex> studies(const medDataIndex& index ) const;

    /** Enumerate all series for given patient*/
    virtual QList<medDataIndex> series(const medDataIndex& index) const;

    /** Enumerate all images for given patient*/
    virtual QList<medDataIndex> images(const medDataIndex& index ) const;

    /** Get metadata for specific item. Return uninitialized string if not present. */
    virtual QString metaData(const medDataIndex& index, const QString& key) const;

    /** Set metadata for specific item. Return true on success, false otherwise. */
    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value);

    /** Implement base class */
    virtual bool isPersistent() const;
signals:
    /**
     * Status message from controller to some user interface
     */
    void copyMessage(QString, int, QColor);


public slots:
    

    /**
    * Read the data from db
    * @params const medDataIndex & index Index pointing to data
    * @return dtkAbstractData * the data stored
    */
    dtkSmartPointer<dtkAbstractData> read(const medDataIndex& index) const;

    /**
    * Import data into the db read from file
    * @Note _NOT_IMPLEMENTED_YET
    * @params const QString & file The file containing the data
    * @return medDataIndex the assigned index
    */
    medDataIndex import(const QString& file);

    /**
    * Import data into the db read from memory
    * @params dtkAbstractData * data dataObject
    * @return medDataIndex the assigned index
    */
    medDataIndex import(dtkAbstractData *data);

    /** override base class */
    virtual void remove(const medDataIndex& index);
    virtual QImage thumbnail( const medDataIndex& index) const;

    /**Implement base class */
    virtual int dataSourceId() const;

protected slots:
    void forwardMessage(QString);

private:
    // helper to create tables
    void createPatientTable(void);
    void   createStudyTable(void);
    void  createSeriesTable(void);
    void   createImageTable(void);

    QSqlDatabase m_database;

    medDatabaseControllerImplPrivate * d;
};

#endif
