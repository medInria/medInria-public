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

#include <medSqlDbController.h>
#include <medCoreLegacyExport.h>

/**
 * Specialization of Concrete dbController implementation which allow to connect to local sqlite database
 */
class MEDCORELEGACY_EXPORT medLocalDbController : public medSqlDbController
{
    Q_OBJECT

public:
    static medLocalDbController *instance();

    bool createConnection();
    bool closeConnection() override;

    QList<medDataIndex> patients() const override;
    void requestDatabaseForModel(QHash<int, QHash<QString, QVariant>> &patientData,
                                 QHash<int, QHash<QString, QVariant>> &studyData,
                                 QHash<int, QHash<QString, QVariant>> &seriesData) const;

    void addTextColumnToSeriesTableIfNeeded(QSqlQuery query, QString columnName);

protected:
    medLocalDbController();

private:
    bool createPatientTable();
    bool createStudyTable();
    bool createSeriesTable();

    bool updateFromNoVersionToVersion1();
    static medLocalDbController *s_instance;
};
