#ifndef medAbstractDbController_h__
#define medAbstractDbController_h__


#include "medCoreExport.h"
#include "medDataIndex.h"
#include <QtCore>
#include <QtSql>

class dtkAbstractData;

/**
 * Abstract sql-db controller class for medDatabaseController implementations
 */
class MEDCORE_EXPORT medAbstractDbController : public QObject
{
    Q_OBJECT

public:

    /**
    * get the database
    * @return QSqlDatabase *pointer to qSqlDatabase
    */
    //virtual QSqlDatabase *database(void) = 0;

    /**
    * Open database and create connection
    * @return bool true on success
    */
    //virtual bool createConnection(void) = 0;
    
    /**
    * Close database and destroy connection
    * @return bool true on success
    */
    //virtual bool  closeConnection(void) = 0;

    //virtual medDataIndex indexForPatient(int id) = 0;
    //virtual medDataIndex indexForStudy  (int id) = 0;
    //virtual medDataIndex indexForSeries (int id) = 0;
    //virtual medDataIndex indexForImage  (int id) = 0;

    /**
    * Returns pointer to dtkAbstractData read from db or NULL
    * @params const medDataIndex & index Index for data
    * @return dtkAbstractData * the data
    */
    virtual dtkAbstractData *read(const medDataIndex& index) = 0;
    //virtual dtkAbstractData *read(int patientId, int studyId, int seriesId) = 0;
    //virtual dtkAbstractData *read(int patientId, int studyId, int seriesId, int imageId) = 0;

    /**
    * Move the database location somewhere else
    * @params QString newLocation Path to new location, must be empty
    * @return bool true on success
    */
    //virtual bool moveDatabase(QString newLocation) = 0;

    /**
    * Get the status of the Db connection
    * @return bool true if connected
    */
    //virtual bool isConnected() = 0;

signals:
    void updated(void);
    void copyMessage(QString, int, QColor);

public slots:
    /**
    * Import a file into the db
    * @params const QString & file Filename
    */
    //virtual void import(const QString& file) = 0;


};

#endif // medAbstractDbController_h__
