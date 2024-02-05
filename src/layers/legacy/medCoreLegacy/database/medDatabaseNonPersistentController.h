#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore/QObject>
#include <QtCore/QList>

#include <medAbstractDbController.h>
#include <medCoreLegacyExport.h>
#include <medDataIndex.h>

#include <memory>

class medAbstractData;
class medDatabaseNonPersistentItem;
class medDatabaseNonPersistentControllerPrivate;
class medImportJobWatcher;

class MEDCORELEGACY_EXPORT medDatabaseNonPersistentController: public medAbstractDbController
{
    Q_OBJECT

public:
    static medDatabaseNonPersistentController &instance();
    ~medDatabaseNonPersistentController();

    int patientId(bool increment=false);
    int   studyId(bool increment=false);
    int  seriesId(bool increment=false);

    int nonPersistentDataStartingIndex() const;

    QList<medDatabaseNonPersistentItem *> items();

    void insert(medDataIndex index, medDatabaseNonPersistentItem *item);

    bool isConnected() const;

    QList<medDataIndex> availableItems() const;
    virtual QString metaData(const medDataIndex& index, const QString& key) const;

    virtual int dataSourceId() const;

    virtual bool createConnection(){return false;};
    virtual QList<medDataIndex> patients() const;
    virtual QList<medDataIndex> studies(const medDataIndex& index ) const;
    virtual QList<medDataIndex> series(const medDataIndex& index ) const;
    QStringList series(const QString &seriesName, const QString &studyId) const override;
    virtual void requestDatabaseForModel(QHash<int, QHash<QString, QVariant> > &patientData,
                                         QHash<int, QHash<QString, QVariant> > &studyData,
                                         QHash<int, QHash<QString, QVariant> > &seriesData) const {};

    virtual QPixmap thumbnail(const medDataIndex &index) const;

    virtual bool isPersistent() const;

    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value);

    bool loadData(const medDataIndex &index) override { return false; };
    bool isDataLoaded(const medDataIndex &index) override { return true; };

public slots:
    virtual medAbstractData *retrieve(const medDataIndex &index, bool readFullData = true) const;

    void importData(medAbstractData *data, const QUuid &callerUuid, bool allowDuplicateSeriesName = false);
    void importPath(const QString &file, const QUuid &callerUuid, bool indexWithoutCopying);
    bool importMetaDataFromPacs(const QHash<QString, QHash<QString, QVariant> > &pData,
                                const QHash<QString, QHash<QString, QVariant> > &sData) override { return false; };
    void remove(const medDataIndex &index);

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient);

    medDataIndex moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy);

    bool contains( const medDataIndex& index) const;

    virtual void removeAll();

private:
    medDatabaseNonPersistentController();

    medDatabaseNonPersistentControllerPrivate *d;
    static std::unique_ptr<medDatabaseNonPersistentController> s_instance;
};
