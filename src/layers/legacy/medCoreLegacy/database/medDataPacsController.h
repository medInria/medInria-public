/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <medAbstractDbController.h>
#include <medCoreLegacyExport.h>

#include <memory>

class medDatabaseNonPersistentItem;
class medDataPacsControllerPrivate;

/**
 * Concrete Controller implementation adhering to abstract base class
 */
class MEDCORELEGACY_EXPORT medDataPacsController : public medAbstractDbController
{

public:
    enum ResultEventLoop
    {
        SUCCESS = 0,
        FAILURE
    };

    static medDataPacsController &instance();
    ~medDataPacsController();

    QList<medDatabaseNonPersistentItem *> items();

    int patientId(bool increment = false) const;
    int studyId(bool increment = false);
    int seriesId(bool increment = false);

    medDatabaseNonPersistentItem *getStudyItem(const QString &patientId,
                                               const QString &studyInstanceUID);

    medDatabaseNonPersistentItem *getSeriesItem(const QString &patientId,
                                                const QString &studyInstanceUID,
                                                const QString &seriesInstanceUID);

    medDataIndex index(medDatabaseNonPersistentItem *item);

    void insert(medDataIndex index, medDatabaseNonPersistentItem *item);

    bool isConnected() const override;

    int dataSourceId() const override;

    bool createConnection() override { return false; };

    QList<medDataIndex> patients() const override;
    QList<medDataIndex> studies(const medDataIndex &index) const override;
    QList<medDataIndex> series(const medDataIndex &index) const override;
    QStringList series(const QString &seriesName, const QString &studyId) const override;
    void requestDatabaseForModel(QHash<int, QHash<QString, QVariant> > &patientData,
                                 QHash<int, QHash<QString, QVariant> > &studyData,
                                 QHash<int, QHash<QString, QVariant> > &seriesData) const override;

    QPixmap thumbnail(const medDataIndex &index) const override { return QPixmap(); };

    QString metaData(const medDataIndex &index, const QString &key) const override;
    bool setMetaData(const medDataIndex &index, const QString &key, const QString &value) override;
    bool isPersistent() const override;

    bool loadData(const medDataIndex &index) override;
    void setNumberOfFilesInDir(int num);
    bool isDataLoaded(const medDataIndex &index) override;

public slots:
    medAbstractData *retrieve(const medDataIndex &index, bool readFullData = true) const override;

    void importData(medAbstractData *data, const QUuid &importUuid, bool allowDuplicateSeriesName = false) override{};
    void importPath(const QString &file, const QUuid &importUuid, bool indexWithoutCopying) override{};
    bool importMetaDataFromPacs(const QHash<QString, QHash<QString, QVariant> > &pData,
                                const QHash<QString, QHash<QString, QVariant> > &sData) override;
    void remove(const medDataIndex &index) override;
    void removeAll() override;

    bool contains(const medDataIndex &index) const override;

    QList<medDataIndex> moveStudy(const medDataIndex &indexStudy, const medDataIndex &toPatient) override;
    medDataIndex moveSeries(const medDataIndex &indexSeries, const medDataIndex &toStudy) override;

private:
    medDataPacsController();
    medDataIndex storePatientMetaDataFromPacs(QString &patientName,
                                              QString &patientId,
                                              QString &birthdate);
    medDataIndex storeStudyMetaDataFromPacs(medDataIndex &ptIndex,
                                            QString &studyInstanceUID,
                                            QString &studyDescription);
    medDataIndex storeSeriesMetaDataFromPacs(medDataIndex &ptIndex,
                                             medDataIndex &stIndex,
                                             QString &seriesInstanceUID,
                                             QString &seriesDescription,
                                             QString &modality);
    void onImportFinished(const QString &path, QEventLoop &loop, QUuid &uuid, QTimer &timer);
    medDataIndex getStudyIfEmpty(const medDataIndex seriesIndex);
    medDataIndex getPatientIfEmpty(const medDataIndex studyIndex);

    medDataPacsControllerPrivate *d;
    static std::unique_ptr<medDataPacsController> s_instance;
};
