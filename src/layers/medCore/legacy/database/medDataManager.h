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
#include <QList>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medDataIndex.h>

#include <memory>

class medDataManagerPrivate;
class medAbstractData;
class medAbstractDbController;
class dtkAbstractDataWriter;

class MEDCORE_EXPORT medDataManager : public QObject
{
    Q_OBJECT

public:
    ~medDataManager();
    static medDataManager &instance();

    medAbstractData* retrieveData(const medDataIndex &index);
    QList<medAbstractData*> retrieveDataList(const medDataIndex& index);
    void loadData(const medDataIndex &index);

    QHash<QString, dtkAbstractDataWriter*> getPossibleWriters(medAbstractData* data);

    QUuid importData(medAbstractData* data, bool persistent = false);
    QUuid importPath(const QString& dataPath, bool indexWithoutCopying, bool persistent = false);

    void exportData(dtkSmartPointer<medAbstractData> data);
    void exportDataToPath(dtkSmartPointer<medAbstractData> data, const QString& path, const QString& format = "");

    int getDataType(const medDataIndex &index);
    QList<medDataIndex> getSubData(const medDataIndex & index);

    QUuid makePersistent(medDataIndex index);

    QString getMetaData(const medDataIndex& index, const QString& key);
    bool    setMetadata(const medDataIndex& index, const QString& key, const QString& value);

    void removeData(const medDataIndex& index);

    QPixmap thumbnail(const medDataIndex& index);

    QList<medDataIndex> getSeriesListFromStudy(const medDataIndex &indexStudy);

    // ------------------------- To be moved elsewhere -----------------------

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient);
    medDataIndex moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy);


    void medDataHubRelay(medDataIndex index, QUuid uuid);

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

protected:
    medDataManagerPrivate * const d_ptr;

private:
    medDataManager();
    static std::unique_ptr<medDataManager> s_instance;

    Q_DECLARE_PRIVATE(medDataManager)
};
