/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <medCoreExport.h>

class QMimeData;

/** class medDataIndex
 *  \brief  A small class to uniquely identify data stored in the medInria datasources.
 *  The class contains 5 integer values :
 *  dataSourceId : Identifies the data source (see @medDataManager)
 *  patientId, studyId, seriesId,imageId identify the data within the source.
 *
 *  The integer value NOT_VALID is reserved to indicate unset or invalid data.
 *  */
class MEDCORE_EXPORT medDataIndex
{
public:
    enum {NOT_VALID = -1 };

     medDataIndex(int dataSourceId, int patientId, int studyId, int seriesId, int imageId);
     medDataIndex();
     medDataIndex(const medDataIndex& index);
    ~medDataIndex();

    bool isValid() const;
    bool isValidForPatient() const;
    bool isValidForStudy() const;
    bool isValidForSeries() const;
    bool isValidForImage() const;

    QString asString() const;

    void setDataSourceId (int id) { m_dataSourceId = id; }
    void setPatientId (int id) { m_patientId = id; }
    void setStudyId   (int id) { m_studyId = id; }
    void setSeriesId  (int id) { m_seriesId = id; }
    void setImageId   (int id) { m_imageId = id; }

    int dataSourceId() const { return m_dataSourceId; }
    int patientId() const { return m_patientId; }
    int   studyId() const { return m_studyId; }
    int  seriesId() const { return m_seriesId; }
    int   imageId() const { return m_imageId; }

    medDataIndex& operator=(const medDataIndex& index);

    static bool isMatch( const medDataIndex& index1, const medDataIndex& index2);

    friend MEDCORE_EXPORT bool operator==(const medDataIndex& index1, const medDataIndex& index2);
    friend MEDCORE_EXPORT bool operator!=(const medDataIndex& index1, const medDataIndex& index2);

    friend MEDCORE_EXPORT bool operator<(const medDataIndex& index1, const medDataIndex& index2);
    friend MEDCORE_EXPORT QDebug operator<<(QDebug debug, const medDataIndex& index);
    friend MEDCORE_EXPORT QDebug operator<<(QDebug debug,       medDataIndex *index);

    QMimeData * createMimeData();
    static medDataIndex readMimeData(const QMimeData * mimeData);

    static medDataIndex makePatientIndex(int sourceId, int patientId);
    static medDataIndex makeStudyIndex(int sourceId, int patientId, int studyId);
    static medDataIndex makeSeriesIndex(int sourceId, int patientId, int studyId, int seriesId);

private:
    int m_dataSourceId;
    int m_patientId;
    int m_studyId;
    int m_seriesId;
    int m_imageId;
};

// /////////////////////////////////////////////////////////////////
// Convenience operators
// /////////////////////////////////////////////////////////////////

MEDCORE_EXPORT bool operator==(const medDataIndex& index1, const medDataIndex& index2);
MEDCORE_EXPORT bool operator!=(const medDataIndex& index1, const medDataIndex& index2);
MEDCORE_EXPORT bool operator<(const medDataIndex& index1, const medDataIndex& index2);

MEDCORE_EXPORT QDebug operator<<(QDebug debug, const medDataIndex& index);
MEDCORE_EXPORT QDebug operator<<(QDebug debug,       medDataIndex *index);

// /////////////////////////////////////////////////////////////////
// Hash functions
// /////////////////////////////////////////////////////////////////

MEDCORE_EXPORT uint qHash(const medDataIndex &key);

// /////////////////////////////////////////////////////////////////
// Meta type registration
// /////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(medDataIndex)


