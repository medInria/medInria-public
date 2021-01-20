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

#include <medAbstractPersistentDbController.h>
#include <medCoreLegacyExport.h>

class medAbstractData;
class medSqlDbControllerPrivate;
class medJobItemL;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDCORELEGACY_EXPORT medSqlDbController : public medAbstractPersistentDbController
{
    Q_OBJECT

public:
    ~medSqlDbController();

    const QSqlDatabase &database() const;

    bool createConnection() = 0;
    virtual bool closeConnection() = 0;

    medDataIndex indexForPatient(int id) override;
    medDataIndex indexForStudy(int id) override;
    medDataIndex indexForSeries(int id) override;

    medDataIndex indexForPatient(const QString &patientName) override;
    medDataIndex indexForStudy(const QString &patientName, const QString &studyName) override;
    medDataIndex indexForSeries(const QString &patientName, const QString &studyName,
                                const QString &seriesName) override;

    QString stringForPath(const QString &name) const;

    bool isConnected() const override;

    QList<medDataIndex> patients() const = 0;
    QList<medDataIndex> studies(const medDataIndex &index) const override;
    QList<medDataIndex> series(const medDataIndex &index) const override;
    void requestDatabaseForModel(QHash<int, QHash<QString, QVariant>> &patientData,
                                 QHash<int, QHash<QString, QVariant>> &studyData,
                                 QHash<int, QHash<QString, QVariant>> &seriesData) const = 0;

    QString metaData(const medDataIndex &index, const QString &key) const override;
    bool setMetaData(const medDataIndex &index, const QString &key, const QString &value) override;

    bool execQuery(QSqlQuery &query, const char *file = nullptr, int line = -1) const override;

protected:
    void setConnected(bool flag);
    void reset();

public slots:

    QList<medDataIndex> moveStudy(const medDataIndex &indexStudy, const medDataIndex &toPatient) override;
    medDataIndex moveSeries(const medDataIndex &indexSeries, const medDataIndex &toStudy) override;

    bool contains(const medDataIndex &index) const override;

protected:
    medSqlDbController();

private:
    medSqlDbControllerPrivate *d;
};
