/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medOldDataManager.h>

#include <medCoreExport.h>
#include <medDataIndex.h>
#include <medAbstractData.h>

class medDataManagerPrivate;

class MEDCORE_EXPORT medDataManager : public medOldDataManager
{
    Q_OBJECT

public:
    static void initialize();
    static medDataManager * instance();

    QSharedPointer<medAbstractData> retrieveData(const medDataIndex& index);

signals:
    void retrievingFailed(const medDataIndex& index);

protected:
    medDataManagerPrivate * const d_ptr;

private:
    medDataManager();
    virtual ~medDataManager();

    static medDataManager * s_instance;

    Q_DECLARE_PRIVATE(medDataManager)
};


/*

    medDataIndex importData(medAbstractData* data);
    medDataIndex importFile(const QString& dataPath, bool indexWithoutCopying);
    medDataIndex importDataNonPersistent(medAbstractData* data);
    medDataIndex importFileNonPersistent(const QString& dataPath);
    bool transferDataToPersistentDatabase(medAbstractData* data);

    void exportData(medAbstractData* data);
    void exportDataToFile(medAbstractData* data, const QString& path, const QString& format = "");
    bool updateData(const medDataIndex& index, medAbstractData* data);
    bool updateMetadata(const medDataIndex& index, const medMetaDataKeys::Key& md, const QString& value);
    medDataIndex removeData(const medDataIndex& index);
signals:
    void dataImported(const medDataIndex& index);
    void dataUpdated(const medDataIndex& index);
    void metadataUpdated(const medDataIndex& index, const QString& key, const QString& value);
    void dataRemoved(const medDataIndex& index);
private slots:
    void exportDialog_updateSuffix(int index);
    void removeFromCache(QObject * obj);
private:
     medDataManager();
     virtual ~medDataManager();

 */
