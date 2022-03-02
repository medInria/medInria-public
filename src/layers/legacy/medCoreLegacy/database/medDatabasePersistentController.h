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

    const QSqlDatabase &database() const;

    bool createConnection() = 0;
    virtual bool closeConnection() = 0;

    medDataIndex indexForPatient(int id);
    medDataIndex indexForStudy(int id);
    medDataIndex indexForSeries(int id);

    medDataIndex indexForPatient(const QString &patientName);
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

    bool isConnected() const override;

    QList<medDataIndex> patients() const = 0;
    QList<medDataIndex> studies(const medDataIndex &index) const override;
    QList<medDataIndex> series(const medDataIndex &index) const override;
    QHash<QString, QString> series(const QString &studyInstanceUID) const override;
    void requestDatabaseForModel(QHash<int, QHash<QString, QVariant> > &patientData,
                                 QHash<int, QHash<QString, QVariant> > &studyData,
                                 QHash<int, QHash<QString, QVariant> > &seriesData) const = 0;

    QPixmap thumbnail(const medDataIndex &index) const;

    QString metaData(const medDataIndex &index, const QString &key) const override;
    bool setMetaData(const medDataIndex &index, const QString &key, const QString &value) override;

    bool execQuery(QSqlQuery &query, const char *file = nullptr, int line = -1) const;

    bool isPersistent() const;

    bool loadData(const medDataIndex &index) override { return false; };

protected:
    void setConnected(bool flag);
    void reset();

public slots:
    medAbstractData *retrieve(const medDataIndex &index) const;

    void importPath(const QString &file, const QUuid &importUuid, bool indexWithoutCopying = false) override;
    void importData(medAbstractData *data, const QUuid &importUuid) override;
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
    QSqlDatabase m_database;

private:
    medDatabasePersistentControllerPrivate *d;
};
