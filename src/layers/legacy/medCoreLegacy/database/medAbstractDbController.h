#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataIndex.h>
#include <medMetaDataKeys.h>
#include <medAbstractData.h>

#include <medCoreLegacyExport.h>

class medImportJobWatcher;
class medDataIndex;
class medJobItemL;

class MEDCORELEGACY_EXPORT medAbstractDbController : public QObject
{
    Q_OBJECT

public:
    virtual bool isConnected() const = 0;

    virtual int dataSourceId() const = 0;

    virtual bool createConnection() = 0;

    virtual QList<medDataIndex> patients() const = 0;
    virtual QList<medDataIndex> studies(const medDataIndex &index) const = 0;
    virtual QList<medDataIndex> series(const medDataIndex &index) const = 0;
    virtual QStringList series(const QString &seriesName, const QString &studyId = QString("")) const = 0;
    virtual void requestDatabaseForModel(QHash<int, QHash<QString, QVariant> > &patientData,
                                         QHash<int, QHash<QString, QVariant> > &studyData,
                                         QHash<int, QHash<QString, QVariant> > &seriesData) const = 0;

    virtual QPixmap thumbnail(const medDataIndex &index) const = 0;

    virtual QString metaData(const medDataIndex &index, const QString &key) const = 0;
    QString metaData(const medDataIndex &index, const medMetaDataKeys::Key &md) const { return metaData(index, md.key()); }
    virtual bool setMetaData(const medDataIndex &index, const QString &key, const QString &value) = 0;
    virtual bool isPersistent() const = 0;
    virtual bool loadData(const medDataIndex &index) = 0;
    virtual bool isDataLoaded(const medDataIndex &index) = 0;
    virtual QString attachedMetadataFileExists(const medDataIndex &index){ return QString();};

signals:
    void metadataModified(const medDataIndex &index, const QString &key = "", const QString &value = "");
    void dataImported(const medDataIndex &index, QUuid importId);
    void dataRemoved(const medDataIndex &index);
    void moveRequested(const QString &uid, const QString &queryLevel);

public slots:
    virtual medAbstractData* retrieve(const medDataIndex& index, bool readFullData = true) const = 0;

    virtual void importData(medAbstractData *data, const QUuid &importUuid, bool allowDuplicateSeriesName = false) = 0;
    virtual void importPath(const QString &file, const QUuid &importUuid, bool indexWithoutCopying) = 0;
    virtual bool importMetaDataFromPacs(const QHash<QString, QHash<QString, QVariant> > &pData,
                                        const QHash<QString, QHash<QString, QVariant> > &sData) = 0;
    virtual void remove(const medDataIndex &index) = 0;
    virtual void removeAll() = 0;

    virtual bool contains(const medDataIndex &index) const = 0;

    virtual QList<medDataIndex> moveStudy(const medDataIndex &indexStudy, const medDataIndex &toPatient) = 0;
    virtual medDataIndex moveSeries(const medDataIndex &indexSeries, const medDataIndex &toStudy) = 0;
};
