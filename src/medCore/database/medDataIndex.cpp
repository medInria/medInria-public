/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataIndex.h>


medDataIndex::medDataIndex(int dataSourceId, int patientId, int studyId, int seriesId, int imageId)
    : m_dataSourceId(dataSourceId),
    m_patientId(patientId),
    m_studyId(studyId),
    m_seriesId(seriesId),
    m_imageId(imageId)
{

}

medDataIndex::medDataIndex(const medDataIndex& index)
    : m_dataSourceId(index.m_dataSourceId),
    m_patientId(index.m_patientId),
    m_studyId(index.m_studyId),
    m_seriesId(index.m_seriesId),
    m_imageId(index.m_imageId)
{
}

medDataIndex::medDataIndex()
    : m_dataSourceId(NOT_VALID),
    m_patientId(NOT_VALID),
    m_studyId(NOT_VALID),
    m_seriesId(NOT_VALID),
    m_imageId(NOT_VALID)
{
}


medDataIndex::~medDataIndex(void)
{
}

bool medDataIndex::isValid(void) const
{
    return (m_dataSourceId != NOT_VALID && m_patientId != NOT_VALID);
}

bool medDataIndex::isValidForPatient(void) const
{
    return (m_dataSourceId != NOT_VALID && m_patientId != NOT_VALID);
}

bool medDataIndex::isValidForStudy(void) const
{
    return (isValidForPatient() && (m_studyId != NOT_VALID));
}

bool medDataIndex::isValidForSeries(void) const
{
    return (isValidForStudy() && (m_seriesId != NOT_VALID));
}

bool medDataIndex::isValidForImage(void) const
{
    return (isValidForSeries() && (m_imageId != NOT_VALID));
}


medDataIndex& medDataIndex::operator=(const medDataIndex& index)
{
    m_dataSourceId = index.m_dataSourceId;
    m_patientId = index.m_patientId;
    m_studyId = index.m_studyId;
    m_seriesId = index.m_seriesId;
    m_imageId = index.m_imageId;

	return *this;
}

QString medDataIndex::asString() const
{
    QString returnString = QString::number(m_dataSourceId) + " | " 
        + QString::number(m_patientId) + " | " 
        + QString::number(m_studyId) + " | " 
        + QString::number(m_seriesId) + " | " 
        + QString::number(m_imageId);
    return returnString;
}

/**
 * Returns true if two indexes match, where either input may be a patient, study, series or image.
 * Here, NOT_VALID on either side is treated as equality where operator== requires values that are equal.
 */
bool medDataIndex::isMatch( const medDataIndex& index1, const medDataIndex& index2)
{
    if ( index1.dataSourceId() != index2.dataSourceId() ) 
        return false;

    if ( index1.patientId() == NOT_VALID || index2.patientId() == NOT_VALID) 
        return true;

    if ( index1.patientId() != index2.patientId() ) 
        return false;

    if ( index1.studyId() == NOT_VALID || index2.studyId() == NOT_VALID) 
        return true;

    if ( index1.studyId() != index2.studyId() ) 
        return false;

    if ( index1.seriesId() == NOT_VALID || index2.seriesId() == NOT_VALID) 
        return true;

    if ( index1.seriesId() != index2.seriesId() ) 
        return false;

    if ( index1.imageId() == NOT_VALID || index2.imageId() == NOT_VALID) 
        return true;

    if ( index1.imageId() != index2.imageId() ) 
        return false;

    return true; // patient, study, series and image match and are not -1;
}

/** Create mime representation. Caller takes ownership of the pointer.*/
QMimeData * medDataIndex::createMimeData()
{
    QString indexString = QString("%1:%2:%3:%4:%5").arg(this->dataSourceId())
        .arg(this->patientId())
        .arg(this->studyId()).arg(this->seriesId()).arg(this->imageId());

    QMimeData *data = new QMimeData;
    data->setData("med/index", indexString.toLatin1());
    return data;
}

/** Read medDataIndex from mime data. Returns an invalid index in case of error.*/
medDataIndex medDataIndex::readMimeData( const QMimeData * mimeData )
{
    if (mimeData->hasFormat("med/index")) {
        QStringList ids = QString(mimeData->data("med/index")).split(":");
        if ( ids.size() >= 5 ) {
            int intIds[5];
            bool allOk = true;
            for ( int i(0); i<5; ++i ) {
                bool ok = false;
                intIds[i] = ids.at(i).toInt(&ok);
                if ( !ok ) {
                    allOk = false;
                    break;
                }
            }
            if (allOk) {
                return medDataIndex( intIds[0], intIds[1], intIds[2], intIds[3], intIds[4] );
            }
        }
    }
    return medDataIndex();
}

medDataIndex medDataIndex::makePatientIndex(int sourceId, int patientId )
{
    return medDataIndex(sourceId, patientId, NOT_VALID, NOT_VALID, NOT_VALID); 
}

medDataIndex medDataIndex::makeStudyIndex( int sourceId, int patientId, int studyId )
{
    return medDataIndex(sourceId, patientId, studyId, NOT_VALID, NOT_VALID); 
}

medDataIndex medDataIndex::makeSeriesIndex( int sourceId, int patientId, int studyId, int seriesId )
{
    return medDataIndex(sourceId, patientId, studyId, seriesId, NOT_VALID); 
}





// /////////////////////////////////////////////////////////////////
// Convenience operators
// /////////////////////////////////////////////////////////////////

bool operator==(const medDataIndex& index1, const medDataIndex& index2)
{
    return ( (index1.dataSourceId() == index2.dataSourceId()) && 
        (index1.patientId() == index2.patientId()) && 
        (index1.studyId() == index2.studyId()) && 
        (index1.seriesId() == index2.seriesId()) && 
        (index1.imageId() == index2.imageId()));
}

bool operator!=(const medDataIndex& index1, const medDataIndex& index2)
{
    return (!operator==(index1, index2));
}

QDebug operator<<(QDebug debug, const medDataIndex& index)
{
    debug.nospace() << "(" << index.m_dataSourceId << ", " << index.m_patientId << ", " << index.m_studyId << ", " << index.m_seriesId << ", " << index.m_imageId << ")";

    return debug.space();
}

QDebug operator<<(QDebug debug, medDataIndex *index)
{
    if ( index ){
        debug.nospace() << "(" << index->m_dataSourceId << ", " << index->m_patientId << ", " << index->m_studyId << ", " << index->m_seriesId << ", " << index->m_imageId << ")";
    } else {
        debug.nospace() << "(" << "[medDataIndex* NULL]" << ")";
    }

    return debug.space();
}

/**
 * The less than operator can be used with STL maps.
 *  Ordering is by patientId, then studyId, then seriesId, then imageId.
 */
bool operator<(const medDataIndex& index1, const medDataIndex& index2)
{
    if (index1.dataSourceId() < index2.dataSourceId()) 
        return true;
    else if (index2.dataSourceId() < index1.dataSourceId()) 
        return false;
    else if (index1.patientId() < index2.patientId()) 
        return true;
    else if (index2.patientId() < index1.patientId()) 
        return false;
    else if (index1.studyId() < index2.studyId())
        return true;
    else if (index2.studyId() < index1.studyId())
        return false;
    else if (index1.seriesId() < index2.seriesId()) 
        return true;
    else if (index2.seriesId() < index1.seriesId()) 
        return false;
    else if (index1.imageId() < index2.imageId())
        return true;
    // if we reach here, either greater or equal, return false
        return false;
}

// /////////////////////////////////////////////////////////////////
// Hash functions
// /////////////////////////////////////////////////////////////////

uint qHash(const medDataIndex &key)
{
   uint hash = 0;
   hash = hash ^ qHash(key.dataSourceId());
   hash = hash ^ qHash(key.patientId());
   hash = hash ^ qHash(key.studyId());
   hash = hash ^ qHash(key.seriesId());
   hash = hash ^ qHash(key.imageId());
   return hash;
}
