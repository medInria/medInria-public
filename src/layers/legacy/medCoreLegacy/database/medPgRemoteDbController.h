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

#define EXEC_QUERY(q) medPgRemoteDbController::instance()->execQuery(q, __FILE__ , __LINE__ )

class medAbstractData;
class medLocalDbControllerPrivate;
class medJobItemL;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDCORELEGACY_EXPORT medPgRemoteDbController: public medAbstractPersistentDbController
{
    Q_OBJECT

public:
    static medPgRemoteDbController* instance();
    ~medPgRemoteDbController();

    const QSqlDatabase& database() const;

    bool createConnection();
    bool  closeConnection() override;

    medDataIndex indexForPatient(int id) override;
    medDataIndex indexForStudy  (int id) override;
    medDataIndex indexForSeries (int id)override;

    medDataIndex indexForPatient (const QString &patientName) override;
    medDataIndex indexForStudy   (const QString &patientName, const QString &studyName) override;
    medDataIndex indexForSeries  (const QString &patientName, const QString &studyName,
                                  const QString &seriesName) override;

    QString stringForPath(const QString & name) const;

    bool isConnected() const override;

    QList<medDataIndex> patients() const override;
    QList<medDataIndex> studies(const medDataIndex& index ) const override;
    QList<medDataIndex> series(const medDataIndex& index) const override;

    QString metaData(const medDataIndex& index,const QString& key) const override;
    bool setMetaData(const medDataIndex& index, const QString& key, const QString& value) override;

    bool execQuery(QSqlQuery& query, const char* file = nullptr, int line = -1) const override;

    void addTextColumnToSeriesTableIfNeeded(QSqlQuery query, QString columnName);

public slots:

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient) override;
    medDataIndex moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy) override;

    bool contains(const medDataIndex &index) const override;

private:
    medPgRemoteDbController();

    bool createPatientTable();
    bool   createStudyTable();
    bool  createSeriesTable();

    bool updateFromNoVersionToVersion1();

    medLocalDbControllerPrivate * d;
    static medPgRemoteDbController * s_instance;
};
