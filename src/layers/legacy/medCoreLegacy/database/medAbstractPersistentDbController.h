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
class medJobItemL;

/**
 * Concrete dbController implementation adhering to abstract base class
 */
class MEDCORELEGACY_EXPORT medAbstractPersistentDbController: public medAbstractDbController
{
    Q_OBJECT

public:
    virtual const QSqlDatabase& database() const = 0;

    bool createConnection() = 0 ;
    virtual bool  closeConnection() = 0;

    virtual medDataIndex indexForPatient(int id) = 0;
    virtual medDataIndex indexForStudy  (int id) = 0;
    virtual medDataIndex indexForSeries (int id) = 0;

    virtual medDataIndex indexForPatient (const QString &patientName) = 0;
    virtual medDataIndex indexForStudy   (const QString &patientName, const QString &studyName) = 0;
    virtual medDataIndex indexForSeries  (const QString &patientName, const QString &studyName,
                                  const QString &seriesName) = 0;

    bool moveDatabase(QString newLocation);

    bool isConnected() const = 0;

    QList<medDataIndex> patients() const = 0;
    QList<medDataIndex> studies(const medDataIndex& index ) const = 0;
    QList<medDataIndex> series(const medDataIndex& index) const = 0;

    QPixmap thumbnail(const medDataIndex& index) const;

    QString metaData(const medDataIndex& index,const QString& key) const = 0;
    bool setMetaData(const medDataIndex& index, const QString& key, const QString& value) = 0;

    virtual bool execQuery(QSqlQuery& query, const char* file = nullptr, int line = -1) const = 0;
    bool isPersistent() const;

public slots:

    medAbstractData *retrieve(const medDataIndex &index) const;

    void importPath(const QString& file, const QUuid& importUuid, bool indexWithoutCopying = false) override;
    void importData(medAbstractData *data, const QUuid & importUuid) override;

    void remove(const medDataIndex& index);

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient) = 0;
    medDataIndex moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy) = 0;

    int dataSourceId()  const;

    bool contains(const medDataIndex &index) const = 0;

    void removeAll();

protected slots:
    void showOpeningError(QObject *sender);

protected:
 
    QSqlDatabase m_database;

};
