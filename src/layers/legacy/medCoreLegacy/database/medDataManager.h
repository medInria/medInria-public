#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QPixmap>
#include <QUuid>

#include <medDatabasePersistentController.h>
#include <medCoreLegacyExport.h>
#include <medDatabaseExporter.h>
#include <medDataIndex.h>

#include <memory>

class medDataManagerPrivate;
class medAbstractData;
class medAbstractDbController;
class dtkAbstractDataWriter;

class MEDCORELEGACY_EXPORT medDataManager : public QObject
{
    Q_OBJECT

public:
    ~medDataManager();

    static medDataManager &instance();

    medAbstractData* retrieveData(const medDataIndex& index);
    void loadData(const medDataIndex &index);

    const QMetaObject* getDataType(const medDataIndex& index);

    QHash<QString, dtkAbstractDataWriter*> getPossibleWriters(medAbstractData* data);

    QUuid importData(medAbstractData* data, bool persistent = false, bool allowDuplicateSeriesName = false);
    QUuid importPath(const QString& dataPath, bool indexWithoutCopying, bool persistent = false);
    void fetchData(const QHash<QString, QHash<QString, QVariant> > &pData,
                   const QHash<QString, QHash<QString, QVariant> > &sData);

    void exportData(dtkSmartPointer<medAbstractData> data);
    void exportDataToPath(dtkSmartPointer<medAbstractData> data, const QString& path, const QString& format = "");
    void exportDataToPath(QList<medAbstractData *> dataList, const QString& path, const QString& format = "");

    QUuid makePersistent(medDataIndex index);

    QString getMetaData(const medDataIndex& index, const QString& key);
    bool setMetadata(const medDataIndex& index, const QString& key, const QString& value);

    void removeData(const medDataIndex& index);

    QPixmap thumbnail(const medDataIndex& index);

    QList<medDataIndex> getSeriesListFromStudy(const medDataIndex &indexStudy);

    // ------------------------- To be moved elsewhere -----------------------

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient);
    medDataIndex moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy);

    // ------------------------- Compatibility code, to be removed -----------

    medAbstractDbController* controllerForDataSource(int dataSourceId);
    medDatabasePersistentController *controller();

    void setDatabaseLocation();

signals:
    void metadataModified(const medDataIndex& index, const QString& key = "", const QString& value = "");
    void dataImported(const medDataIndex& index, QUuid importId);
    void dataRemoved(const medDataIndex& index);
    void exportFinished();

    // ------------------------- To be moved elsewhere -----------------------
    void patientModified(medDataIndex index);
    void studyModified(medDataIndex index);

    void updateProgress(int level);
    void moveRequested(const QString &uid, const QString &queryLevel);
    void moveState(int status, const QString &pathOrMessage);

private slots:
    void exportDialog_updateSuffix(int index);
    void garbageCollect();
    void removeFromNonPersistent(medDataIndex,QUuid);
    void setWriterPriorities();

protected:
    medDataManagerPrivate * const d_ptr;

private:
    medDataManager();
    medAbstractData* retrieveData(const medDataIndex& index, bool readFullData);

    static std::unique_ptr<medDataManager> s_instance;
    void launchExporter(medDatabaseExporter* exporter, const QString & filename);

    Q_DECLARE_PRIVATE(medDataManager)

    void saveAttachedMetadataToFile(const dtkSmartPointer<medAbstractData> &data, const QString &exportPath) const;
};
