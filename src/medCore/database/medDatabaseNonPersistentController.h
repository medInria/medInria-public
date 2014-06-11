/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDbController.h>
#include <medCoreExport.h>
#include <medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class medAbstractData;
class medDatabaseNonPersistentItem;
class medDatabaseNonPersistentControllerPrivate;
class medImportJobWatcher;


class MEDCORE_EXPORT medDatabaseNonPersistentController: public medAbstractDbController
{
    Q_OBJECT

public:
    static medDatabaseNonPersistentController * instance();
    ~medDatabaseNonPersistentController();

    /**
    * Get the table indices
    * @params bool increment if true auto-increment the counter
    * @return int return the id (before incrementation)
    */
    int patientId(bool increment=false);
    int   studyId(bool increment=false);
    int  seriesId(bool increment=false);
    int   imageId(bool increment=false);

    /**
    * Return offset needed to separate data indices between databases
    * @params void
    * @return int
    */
    int nonPersistentDataStartingIndex() const;

    /* proprietary method, should be avoided */
    QList<medDatabaseNonPersistentItem *> items();

    /* proprietary method, should be avoided */
    void insert(medDataIndex index, medDatabaseNonPersistentItem *item);

    /**
    * Status of connection, should always be true
    * @return bool
    */
    bool isConnected() const;

    /**
    * return the size that the data behind the medDataIndex in byte
    * @param const medDataIndex& index the index for the data
    * @return estimated size of data
    */
    qint64 getEstimatedSize(const medDataIndex& index) const;

    /** List all the items in the DB */
    QList<medDataIndex> availableItems() const;
    virtual QString metaData(const medDataIndex& index, const QString& key) const;
    virtual QImage thumbnail( const medDataIndex& index) const;

    /**Implement base class */
    virtual int dataSourceId() const;

    /** Enumerate all patients stored in this DB*/
    virtual QList<medDataIndex> patients() const;

    /** Enumerate all studies for given patient*/
    virtual QList<medDataIndex> studies(const medDataIndex& index ) const;

    /** Enumerate all series for given patient*/
    virtual QList<medDataIndex> series(const medDataIndex& index ) const;

    /** Enumerate all images for given patient*/
    virtual QList<medDataIndex> images(const medDataIndex& index ) const;

    /** Return true if data source is persistent. This implementation returns false.*/
    virtual bool isPersistent() const;

    /** Set metadata for specific item. Return true on success, false otherwise. */
    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value);

public slots:

    /**
    * Stores data temporarily referenced by temp index
    * @param data data to be stored
    * @param callerUuid
    */
    void import(medAbstractData *data, QString callerUuid);

    /**
     * Stores data temporarily referenced by temp index
     * @param file data stored at file path.
     * @param callerUuid caller's identifier.
     */
    void import(const QString& file, QString callerUuid);

    /**
     * Remove data referenced by index from temporary database
     * @params const medDataIndex & index : data index
     */
    void remove(const medDataIndex& index);

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

    /** true if the given data index matches one in our db*/
    bool contains( const medDataIndex& index) const;

    virtual medAbstractData* retrieve(const medDataIndex& index) const;

    virtual void removeAll();

    /**
    * Import data into the db read from file
    * @param const QString & file The file containing the data
    * @param bool indexWithoutCopying true if the file must only be indexed by its current path,
    * false if the file will be imported (copied or converted to the internal storage format)
    */
    virtual void import(const QString& file,bool indexWithoutCopying);





private:
    medDatabaseNonPersistentController();

    medDatabaseNonPersistentControllerPrivate *d;
    static medDatabaseNonPersistentController* s_instance;
};


