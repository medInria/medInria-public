/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <medDataIndex.h>

#include <medMetaDataKeys.h>
#include <medAbstractData.h>

class medImportJobWatcher;
class medDataIndex;
class medAbstractJob;

class MEDCORE_EXPORT medAbstractDbController : public QObject
{
    Q_OBJECT

public:
    virtual bool isConnected() const = 0;

    virtual int dataSourceId() const = 0;

    virtual QList<medDataIndex> patients() const = 0;
    virtual QList<medDataIndex> studies(const medDataIndex& index ) const = 0;
    virtual QList<medDataIndex> series(const medDataIndex& index) const = 0;
    virtual QList<medDataIndex> images(const medDataIndex& index) const = 0;

    virtual QPixmap thumbnail(const medDataIndex& index) const = 0;

    virtual QString metaData(const medDataIndex& index,const QString& key) const = 0;
    QString metaData(const medDataIndex& index,const medMetaDataKeys::Key& md) const { return metaData(index,md.key()); }
    virtual bool setMetaData(const medDataIndex& index, const QString& key, const QString& value) = 0;
    virtual bool isPersistent() const = 0;

signals:
    void metadataModified(const medDataIndex& index, const QString& key = "", const QString& value = "");
    void dataImported(const medDataIndex& index, QUuid importId);
    void dataRemoved(const medDataIndex& index);

public slots:
    virtual medAbstractData* retrieve(const medDataIndex& index) const = 0;

    virtual void importData(medAbstractData* data, const QUuid& importUuid) = 0;
    virtual void importPath(const QString& file, const QUuid& importUuid, bool indexWithoutCopying) = 0;

    virtual void remove(const medDataIndex& index) = 0;
    virtual void removeAll() = 0;

    virtual bool contains(const medDataIndex& index) const = 0;

    virtual QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient) =  0;
    virtual medDataIndex moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy) =  0;
};
