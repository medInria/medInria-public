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

#include <QtCore/QObject>
#include <QtCore/QList>

class medAbstractData;

class medDatabaseNonPersistentItemPrivate;

class MEDCORE_EXPORT medDatabaseNonPersistentItem : public QObject
{
    Q_OBJECT

public:
     medDatabaseNonPersistentItem(void);
    ~medDatabaseNonPersistentItem(void);

    const QString& name(void) const;
    const QString& birthdate(void) const;
    const QString& patientId(void) const;
    const QString& studyName(void) const;
    const QString& seriesName(void) const;
    const QString& file(void) const;
    const QString& studyId(void) const;
    const QString& studyUid(void) const;
    const QString& seriesId(void) const;
    const QString& seriesUid(void) const;
    const QImage& thumb(void) const;
    const QString orientation(void) const;
    const QString seriesNumber(void) const;
    const QString sequenceName(void) const;
    const QString sliceThickness(void) const;
    const QString rows(void) const;
    const QString columns(void) const;

    const medDataIndex& index(void) const;

    medAbstractData *data(void);


    void setName(const QString& );
    void setBirthdate(const QString& );
    void setPatientId(const QString& );
    void setStudyName(const QString& );
    void setSeriesName(const QString& );
    void setFile(const QString& );
    void setStudyId(const QString& );
    void setStudyUid(const QString& );
    void setSeriesId(const QString& );
    void setSeriesUid(const QString& );
    void setThumb(const QImage& );

    void setIndex(const medDataIndex& );

    void setData(medAbstractData* );

    bool Match(medAbstractData *);

private:
    friend class medDatabaseNonPersistentControllerImpl;
    friend class medDatabaseNonPersistentImporter;

private:
    medDatabaseNonPersistentItemPrivate *d;
};

