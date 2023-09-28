#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThreadStorage>

#include <medCoreLegacyExport.h>
#include <medDatabasePersistentController.h>

#include <memory>

/**
 * Specialization of Concrete dbController implementation which allow to connect to local sqlite database
 */
class MEDCORELEGACY_EXPORT medLocalDbController : public medDatabasePersistentController
{
    Q_OBJECT

public:
    static medLocalDbController &instance();

    bool createConnection() override;
    bool isConnected() const override;
    QSqlDatabase getMainConnection() const override;
    QSqlDatabase getThreadSpecificConnection() const override;

    QMutex& getDatabaseMutex() const override;

    QList<medDataIndex> patients() const override;
    void requestDatabaseForModel(QHash<int, QHash<QString, QVariant> > &patientData,
                                 QHash<int, QHash<QString, QVariant> > &studyData,
                                 QHash<int, QHash<QString, QVariant> > &seriesData) const override;

    void addTextColumnToStudyTableIfNeeded(QSqlQuery query, QString columnName);
    void addTextColumnToSeriesTableIfNeeded(QSqlQuery query, QString columnName);

protected:
    medLocalDbController();

private:
    QThreadStorage<QSqlDatabase> databaseConnections;
    QString databasePath;
    QMutex databaseMutex;

    QSqlDatabase createConnection(QString name) const;

    bool createPatientTable();
    bool createStudyTable();
    bool createSeriesTable();

    bool updateFromNoVersionToVersion1();

    static std::unique_ptr<medLocalDbController> s_instance;

    static const char* mainConnectionName;
};
