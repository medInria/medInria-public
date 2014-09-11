/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDbController.h>
#include <medCoreExport.h>
#include <medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class medAbstractData;
class medDatabaseNonPersistentItem;
class medDatabaseNonPersistentControllerPrivate;
class medImportJobWatcher;


class MEDCORE_EXPORT medDatabaseNonPersistentController: public medAbstractDbController
{
    Q_OBJECT

public:
    static medDatabaseNonPersistentController * instance();
    ~medDatabaseNonPersistentController();

    int patientId(bool increment=false);
    int   studyId(bool increment=false);
    int  seriesId(bool increment=false);
    int   imageId(bool increment=false);

    int nonPersistentDataStartingIndex() const;

    QList<medDatabaseNonPersistentItem *> items();

    void insert(medDataIndex index, medDatabaseNonPersistentItem *item);

    bool isConnected() const;

    QList<medDataIndex> availableItems() const;
    virtual QString metaData(const medDataIndex& index, const QString& key) const;

    virtual int dataSourceId() const;

    virtual QList<medDataIndex> patients() const;
    virtual QList<medDataIndex> studies(const medDataIndex& index ) const;
    virtual QList<medDataIndex> series(const medDataIndex& index ) const;
    virtual QList<medDataIndex> images(const medDataIndex& index ) const;

    virtual QPixmap thumbnail(const medDataIndex &index) const;

    virtual bool isPersistent() const;

    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value);

public slots:
    virtual medAbstractData* retrieve(const medDataIndex& index) const;

    void importData(medAbstractData *data, const QUuid & callerUuid);
    void importPath(const QString& file, const QUuid & callerUuid, bool indexWithoutCopying);

    void remove(const medDataIndex& index);

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient);

    medDataIndex moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy);

    bool contains( const medDataIndex& index) const;

    virtual void removeAll();

private:
    medDatabaseNonPersistentController();

    medDatabaseNonPersistentControllerPrivate *d;
    static medDatabaseNonPersistentController* s_instance;
};


