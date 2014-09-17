/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once


#include <QObject>
#include <QPixmap>
#include <QUuid>

#include <medCoreExport.h>
#include <medDataIndex.h>

class medDataManagerPrivate;
class medAbstractData;
class medAbstractDbController;

class MEDCORE_EXPORT medDataManager : public QObject
{
    Q_OBJECT

public:
    static void initialize();
    static medDataManager * instance();

    medAbstractData* retrieveData(const medDataIndex& index);

    QUuid importData(medAbstractData* data, bool persistent = false);
    QUuid importPath(const QString& dataPath, bool indexWithoutCopying, bool persistent = false);

    void exportData(medAbstractData* data);
    void exportDataToPath(medAbstractData* data, const QString& path, const QString& format = "");

    QUuid makePersistent(medAbstractData* data);

    bool setMetadata(const medDataIndex& index, const QString& key, const QString& value);
    void removeData(const medDataIndex& index);

    QPixmap thumbnail(const medDataIndex& index);

    // ------------------------- To be moved elsewhere -----------------------

    QList<medDataIndex> moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient);
    medDataIndex moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy);

    // ------------------------- Compatibility code, to be removed -----------

    medAbstractDbController* controllerForDataSource(int dataSourceId);

signals:
    void metadataModified(const medDataIndex& index, const QString& key = "", const QString& value = "");
    void dataImported(const medDataIndex& index, QUuid importId);
    void dataRemoved(const medDataIndex& index);

    // ------------------------- To be moved elsewhere -----------------------
    void patientModified(medDataIndex index);
    void studyModified(medDataIndex index);

private slots:
    void exportDialog_updateSuffix(int index);
    void garbageCollect();
    void removeFromNonPersistent(medDataIndex,QUuid);
    void setWriterPriorities();

protected:
    medDataManagerPrivate * const d_ptr;

private:
    medDataManager();
    virtual ~medDataManager();

    static medDataManager * s_instance;

    Q_DECLARE_PRIVATE(medDataManager)
};
