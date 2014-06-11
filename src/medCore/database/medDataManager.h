/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include <medCoreExport.h>

class medDataManagerPrivate;
class medAbstractData;
class medDataIndex;

class MEDCORE_EXPORT medDataManager : public QObject
{
    Q_OBJECT

public:
    static void initialize();
    static medDataManager * instance();

    medAbstractData* retrieveData(const medDataIndex& index);
    void exportData(medAbstractData* data);
    void exportDataToFile(medAbstractData* data, const QString& path, const QString& format = "");

    medDataIndex importInDatabase(medAbstractData* data);
    medDataIndex importInDatabase(const QString& dataPath, bool indexWithoutCopying);
    medDataIndex importInNonPersistentDatabase(medAbstractData* data);
    medDataIndex importInNonPersistentDatabase(const QString& dataPath);
    bool transferDataToPersistentDatabase(medAbstractData* data);

    bool updateData(const medDataIndex& index, medAbstractData* data);
    bool updateMetadata(const medDataIndex& index, const medMetaDataKeys::Key& md, const QString& value);
    void removeData(const medDataIndex& index);

signals:
    void retrievingFailed(const medDataIndex& index);
    void dataImported(const medDataIndex& index);
    void dataUpdated(const medDataIndex& index);
    void metadataUpdated(const medDataIndex& index, const QString& key, const QString& value);
    void dataRemoved(const medDataIndex& index);

private slots:
    void exportDialog_updateSuffix(int index);
    void removeFromCache(QObject * obj);

protected:
    medDataManagerPrivate * const d_ptr;

private:
    medDataManager();
    virtual ~medDataManager();

    static medDataManager * s_instance;

    Q_DECLARE_PRIVATE(medDataManager)
};






