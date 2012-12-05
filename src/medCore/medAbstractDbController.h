/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <medCoreExport.h>
#include <medDataIndex.h>
#include <QtCore>
#include <QtSql>

#include <medMetaDataKeys.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

class medImportJobWatcher;
class medDataIndex;

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
    virtual bool isConnected() const = 0;

    /**
    * return the size that the data behind the medDataIndex in byte
    * @param const medDataIndex& index the index for the data
    * @return estimated size of data
    */
    virtual qint64 getEstimatedSize(const medDataIndex& index) const = 0;

    /** Unique Id used in medDataIndex to recognize this DB.*/
    virtual int dataSourceId() const = 0;

    /** Enumerate all patients stored in this DB*/
    virtual QList<medDataIndex> patients() const = 0;

    /** Enumerate all studies for given patient*/
    virtual QList<medDataIndex> studies(const medDataIndex& index ) const = 0;

    /** Enumerate all series for given study*/
    virtual QList<medDataIndex> series(const medDataIndex& index) const = 0;

    /** Enumerate all images for given series*/
    virtual QList<medDataIndex> images(const medDataIndex& index) const = 0;

    /** Get metadata for specific item. Return uninitialized string if not present. */
    virtual QString metaData(const medDataIndex& index,const QString& key) const = 0;
    QString metaData(const medDataIndex& index,const medMetaDataKeys::Key& md) const { return metaData(index,md.key()); }

    /** Set metadata for specific item. Return true on success, false otherwise. */
    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value) = 0;

    /** Get thumbnail. */
    virtual QImage thumbnail( const medDataIndex& index) const = 0;

    /** return true if this is a persistent controller*/
    virtual bool isPersistent() const = 0;



signals:

    /**
     * signal each time the db gets modified, giving the dataindex that was involved
     */
    void updated(const medDataIndex &);
    /**
     * signal each time the db gets modified, giving the dataindex that was involved, and the unique identifier linked with the caller.
     */
    void updated(const medDataIndex &,const QString&);

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
    * @brief Imports a file into the db.
    * This method doesn't return any value since it should be run asynchronously. updated(const medDataIndex &) is called when the importation is complete.
    *
    * @param file the path to the file, or directory to import.
    * @param importUuid a string representation of a unique token (QUuid recommended),
    * If omited, an empty string will be used. It allows for actions specific to the caller of the importation. Only the caller should react to particular requests.
    */
    virtual void import(const QString& file, QString importUuid=QString()) = 0;

    /**
    * @brief Imports a data into the db.
    *
    * This method doesn't return any value since it should be run asynchronously. updated(const medDataIndex &) is called when the importation is complete.
    *
    * @param data a pointer to some data to import.
    * @param importUuid the caller's identifier.
    */
    virtual void import(dtkAbstractData *data, QString importUuid=QString()) = 0;

    /**
     * This method allows importing data from other databases
     * @param const medDataIndex & index The data index used in the referenced db (source)
     * @param const medAbstractDbController & controller  The referenced db (source)
     */
    virtual void import(const medDataIndex& index, const medAbstractDbController& controller);

    /**
    * Import data into the db read from file
    * @param const QString & file The file containing the data
    * @param bool indexWithoutCopying true if the file must only be indexed by its current path,
    * false if the file will be imported (copied or converted to the internal storage format)
    */
    virtual void import(const QString& file,bool indexWithoutCopying);

    /**
    * Export data to file
    * @param dtkAbstractData *data Pointer to some data to export
    * @param const QString & filename The location in which the data will be stored in the file system
    */
    virtual void exportDataToFile(dtkAbstractData *data, const QString &filename);

    /**
     * This method allows removing one data from the database
     * @params const medDataIndex & index The data index to be removed in the db
     */
    virtual void remove(const medDataIndex& index) = 0;

    virtual const QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient) =  0;

    virtual const medDataIndex moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy) =  0;

    /**
    * This method clears data already loaded in the database.
    */
    virtual void clear();

    /**
     * @brief Checks if a medDataIndex is in the db.
     *
     * The function traverses the hierarchy of indices to find the availibity of the medDataIndex. It stops at the first non defined value (-1) and returns true if all indices were found down to that level: i.e a patient with an empty study list is valid (1,-1,-1)
     * @param index a medDataIndex.
     */
    virtual bool contains(const medDataIndex& index) const = 0;

};


