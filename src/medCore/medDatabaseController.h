/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <medSqlExport.h>
#include <medAbstractDbController.h>

class medAbstractData;
class SigEmitter;
class medDatabaseControllerPrivate;
class medJobItem;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDSQL_EXPORT medDatabaseController: public medAbstractDbController
{
    Q_OBJECT

public:
    static medDatabaseController* instance();
    ~medDatabaseController();

    QSqlDatabase *database();

    bool createConnection();
    bool  closeConnection();

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
    bool isConnected() const;

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
    virtual QString metaData(const medDataIndex& index,const QString& key) const;

    /** Set metadata for specific item. Return true on success, false otherwise. */
    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value);

    /** Implement base class */
    virtual bool isPersistent() const;

signals:
    /**
     * Status message from controller to some user interface
     */
    void copyMessage(QString, int, QColor);

    /**
     * This signal is emitted when the import/index process
     * detects an attempt of partial importing. That is when the user
     * tried to import, in 2 separate steps, images belonging
     * to the same volume.
     */
    void partialImportAttempted ( const QString& message );

public slots:


    /**
    * Read the data from db
    * @params const medDataIndex & index Index pointing to data
    * @return medAbstractData * the data stored
    */
    dtkSmartPointer<medAbstractData> read(const medDataIndex& index) const;

    QSharedPointer<medAbstractData> retrieve(const medDataIndex &index) const;
    /**
    * Import data into the db read from file
    * @params const QString & file The file containing the data
    * @params bool indexWithoutCopying true if the file must only be indexed by its current path,
    * false if the file will be imported (copied or converted to the internal storage format)
    */
    void import(const QString& file, bool indexWithoutCopying);

    /**
     * Calls import(const QString& file,bool indexWithoutCopying) with indexWithoutCopying = false.
     *
    */
    void import(const QString& file, QString importUuid=QString());

    /**
    * Import data into the db read from memory
    * @params medAbstractData * data dataObject
    */
    void import(medAbstractData *data, QString importUuid=QString());

    /**
    * Export data to file
    */
    void exportDataToFile(medAbstractData *data, const QString &filename, const QString &writer);


    /** override base class */
    virtual void remove(const medDataIndex& index);

    /**
     * Moves study and its series from one patient to another and returns the list of new indexes
     * @params const medDataIndex & indexStudy The data index of the study to be moved
     * @params const medDataIndex & toPatient The data index to move the study to.
     */
    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient);

    /**
     * Moves serie from one study to another and returns the new index of the serie
     * @params const medDataIndex & indexSerie The data index of the serie to be moved
     * @params const medDataIndex & toStudy The data index to move the serie to.
     */
    medDataIndex moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy);

    virtual QImage thumbnail( const medDataIndex& index) const;

    /**Implement base class */
    virtual int dataSourceId() const;

     bool contains(const medDataIndex &index) const;

protected slots:
    void forwardMessage(QString);
    void showOpeningError(QObject *sender);

private:
    /* constructor */
    medDatabaseController();

    // helper to create tables
    void createPatientTable();
    void   createStudyTable();
    void  createSeriesTable();
    void   createImageTable();

    QSqlDatabase m_database;

    medDatabaseControllerPrivate * d;
    static medDatabaseController * s_instance;
};


