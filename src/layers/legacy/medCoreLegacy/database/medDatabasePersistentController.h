/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>

#include <medAbstractDbController.h>
#include <medCoreLegacyExport.h>

class medAbstractData;
class medDatabasePersistentControllerPrivate;
class medJobItemL;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDCORELEGACY_EXPORT medDatabasePersistentController : public medAbstractDbController
{
    Q_OBJECT

public:
    ~medDatabasePersistentController();

    bool createConnection() = 0;
    virtual QSqlDatabase getMainConnection() const = 0;
    virtual QSqlDatabase getThreadSpecificConnection() const = 0;
    virtual QMutex& getDatabaseMutex() const = 0;

    medDataIndex indexForPatient(int id);
    medDataIndex indexForStudy(int id);
    medDataIndex indexForSeries(int id);

    medDataIndex indexForPatient(const QString &patientName);
    medDataIndex indexForPatientID(const QString &patientId);
    medDataIndex indexForStudy(const QString &patientName,
                               const QString &studyName);
    medDataIndex indexForStudyUID(const QString &patientName,
                                  const QString &studyInstanceUID);
    medDataIndex indexForSeries(const QString &patientName,
                                const QString &studyName,
                                const QString &seriesName);
    medDataIndex indexForSeriesUID(const QString &patientName,
                                   const QString &studyInstanceUID,
                                   const QString &seriesInstanceUID);

    bool moveDatabase(QString newLocation);

    QString stringForPath(const QString &name) const;

    QList<medDataIndex> patients() const = 0;
    QList<medDataIndex> studies(const medDataIndex &index) const override;
    QList<medDataIndex> series(const medDataIndex &index) const override;
    QStringList series(const QString &seriesName, const QString &studyId = QString("")) const override;
    void requestDatabaseForModel(QHash<int, QHash<QString, QVariant> > &patientData,
                                 QHash<int, QHash<QString, QVariant> > &studyData,
                                 QHash<int, QHash<QString, QVariant> > &seriesData) const = 0;

    QPixmap thumbnail(const medDataIndex &index) const;

    QString metaData(const medDataIndex &index, const QString &key) const override;
    bool setMetaData(const medDataIndex &index, const QString &key, const QString &value) override;

    bool execQuery(QSqlQuery &query, const char *file = nullptr, int line = -1) const;

    bool isPersistent() const;

    bool loadData(const medDataIndex &index) override { return false; };
    bool isDataLoaded(const medDataIndex &index) override { return true; };

    QString attachedMetadataFileExists(const medDataIndex &index) override;
protected:
    void reset();

public slots:
    medAbstractData *retrieve(const medDataIndex &index, bool readFullData = true) const;

    void importPath(const QString &file, const QUuid &importUuid, bool indexWithoutCopying = false) override;
    void importData(medAbstractData *data, const QUuid &importUuid, bool allowDuplicateSeriesName = false) override;
    bool importMetaDataFromPacs(const QHash<QString, QHash<QString, QVariant> > &pData,
                                const QHash<QString, QHash<QString, QVariant> > &sData) override { return false; };

    void remove(const medDataIndex &index);

    int dataSourceId() const;

    QList<medDataIndex> moveStudy(const medDataIndex &indexStudy, const medDataIndex &toPatient) override;
    medDataIndex moveSeries(const medDataIndex &indexSeries, const medDataIndex &toStudy) override;

    bool contains(const medDataIndex &index) const override;

    void removeAll();

protected slots:
    void showOpeningError(QObject *sender);

protected:
    medDatabasePersistentController();

private:
    medDatabasePersistentControllerPrivate *d;
};
