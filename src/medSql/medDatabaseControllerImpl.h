#ifndef medDbController_h__
#define medDbController_h__


#include "medSqlExport.h"
#include <medCore/medAbstractDbController.h>

class dtkAbstractData;
class SigEmitter;

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

protected slots:
    void forwardMessage(QString);

private:
    // helper to create tables
    void createPatientTable(void);
    void   createStudyTable(void);
    void  createSeriesTable(void);
    void   createImageTable(void);

    QSqlDatabase m_database;
    bool m_isConnected;
    SigEmitter* emitter;
};

#endif
