/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataIndex.h>

medDataIndex::medDataIndex(int dataSourceId, int patientId, int studyId, int seriesId)
    : m_dataSourceId(dataSourceId),
    m_patientId(patientId),
    m_studyId(studyId),
    m_seriesId(seriesId)
{

}

medDataIndex::medDataIndex(QStringList const & uri)
    : m_dataSourceId(NOT_VALID),
    m_patientId(NOT_VALID),
    m_studyId(NOT_VALID),
    m_seriesId(NOT_VALID),
    m_uriAsList(uri)
{
}

medDataIndex::medDataIndex(const medDataIndex& index)
    : m_dataSourceId(index.m_dataSourceId),
    m_patientId(index.m_patientId),
    m_studyId(index.m_studyId),
    m_seriesId(index.m_seriesId),
    m_uriAsList(index.m_uriAsList)
{
}

medDataIndex::medDataIndex()
    : m_dataSourceId(NOT_VALID),
    m_patientId(NOT_VALID),
    m_studyId(NOT_VALID),
    m_seriesId(NOT_VALID)
{
}

medDataIndex::~medDataIndex(void)
{
}

bool medDataIndex::isV2() const
{
    return !m_uriAsList.isEmpty();
}

bool medDataIndex::isValid(void) const
{
    return (m_dataSourceId != NOT_VALID && m_patientId != NOT_VALID) ^ !m_uriAsList.isEmpty();
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

medDataIndex& medDataIndex::operator=(const medDataIndex& index)
{
    m_dataSourceId = index.m_dataSourceId;
    m_patientId = index.m_patientId;
    m_studyId = index.m_studyId;
    m_seriesId = index.m_seriesId;
    m_uriAsList = index.m_uriAsList;

	return *this;
}

QString medDataIndex::asString() const
{
    QString returnString;
    if (m_uriAsList.isEmpty())
    {
        returnString = QString::number(m_dataSourceId) + " | "
            + QString::number(m_patientId) + " | "
            + QString::number(m_studyId) + " | "
            + QString::number(m_seriesId);
    }
    else
    {
        returnString = m_uriAsList[0] + ":";
        for (int i = 1; i < m_uriAsList.size(); ++i)
        {
            returnString += m_uriAsList[i] + "\r\n";
        }
    }

    return returnString;
}

void medDataIndex::setUri(QString const & uri)
{
    int sourceDelimterIndex = uri.indexOf(QString(":"));

    m_uriAsList = uri.right(uri.size() - sourceDelimterIndex-1).split(QString("\r\n"));
    m_uriAsList.push_front(uri.left(sourceDelimterIndex));
}

/**
 * Returns true if two indexes match, where either input may be a patient, study or series.
 * Here, NOT_VALID on either side is treated as equality where operator== requires values that are equal.
 */
bool medDataIndex::isMatch( const medDataIndex& index1, const medDataIndex& index2)
{
    if (index1.m_uriAsList.isEmpty() && index2.m_uriAsList.isEmpty())
    {
        if (index1.dataSourceId() != index2.dataSourceId())
            return false;

        if (index1.patientId() == NOT_VALID || index2.patientId() == NOT_VALID)
            return true;

        if (index1.patientId() != index2.patientId())
            return false;

        if (index1.studyId() == NOT_VALID || index2.studyId() == NOT_VALID)
            return true;

        if (index1.studyId() != index2.studyId())
            return false;

        if (index1.seriesId() == NOT_VALID || index2.seriesId() == NOT_VALID)
            return true;

        if (index1.seriesId() != index2.seriesId())
            return false;

        return true; // patient, study, and series match and are not -1;
    }
    else
    {
        return index1.m_uriAsList == index2.m_uriAsList;
    }
}

/** Create mime representation. Caller takes ownership of the pointer.*/
QMimeData * medDataIndex::createMimeData()
{
    QString indexString = QString("%1:%2:%3:%4").arg(this->dataSourceId())
        .arg(this->patientId())
        .arg(this->studyId()).arg(this->seriesId());

    QMimeData *data = new QMimeData;
    if (m_uriAsList.isEmpty())
    {
        data->setData("med/index", indexString.toLatin1());
    }
    else
    {
        data->setData("med/index2", asString().toUtf8());
    }
    return data;
}

/** Read medDataIndex from mime data. Returns an invalid index in case of error.*/
medDataIndex medDataIndex::readMimeData( const QMimeData * mimeData )
{
    if (mimeData->hasFormat("med/index")) {
        QStringList ids = QString(mimeData->data("med/index")).split(":");
        if ( ids.size() >= 4 ) {
            int intIds[4];
            bool allOk = true;
            for ( int i(0); i<4; ++i ) {
                bool ok = false;
                intIds[i] = ids.at(i).toInt(&ok);
                if ( !ok ) {
                    allOk = false;
                    break;
                }
            }
            if (allOk) {
                return medDataIndex( intIds[0], intIds[1], intIds[2], intIds[3] );
            }
        }
    }
    else if (mimeData->hasFormat("med/index2"))
    {
        medDataIndex dataIndexRes;
        dataIndexRes.setUri(QString::fromUtf8(mimeData->data("med/index2")));
        return dataIndexRes;
    }
    return medDataIndex();
}

QList<medDataIndex> medDataIndex::readMimeDataMulti(const QMimeData * mimeData)
{
    QList<medDataIndex> dataIndexListRes;

    auto rawData = mimeData->data("med/index2");
    auto uris = rawData.split('\0');

    for (auto uri : uris)
    {
        medDataIndex dataIndexTmp;
        dataIndexTmp.setUri(QString::fromUtf8(uri));
        dataIndexListRes << dataIndexTmp;
    }

    return dataIndexListRes;
}

medDataIndex medDataIndex::makePatientIndex(int sourceId, int patientId )
{
    return medDataIndex(sourceId, patientId, NOT_VALID, NOT_VALID);
}

medDataIndex medDataIndex::makeStudyIndex( int sourceId, int patientId, int studyId )
{
    return medDataIndex(sourceId, patientId, studyId, NOT_VALID);
}

medDataIndex medDataIndex::makeSeriesIndex( int sourceId, int patientId, int studyId, int seriesId )
{
    return medDataIndex(sourceId, patientId, studyId, seriesId);
}

// /////////////////////////////////////////////////////////////////
// Convenience operators
// /////////////////////////////////////////////////////////////////

bool operator==(const medDataIndex& index1, const medDataIndex& index2)
{
    return ( (index1.dataSourceId() == index2.dataSourceId()) && 
        (index1.patientId() == index2.patientId()) && 
        (index1.studyId() == index2.studyId()) && 
        (index1.seriesId() == index2.seriesId()))
        ||
        (!index1.m_uriAsList.isEmpty() && (index1.m_uriAsList == index2.m_uriAsList));
}

bool operator!=(const medDataIndex& index1, const medDataIndex& index2)
{
    return (!operator==(index1, index2));
}

QDebug operator<<(QDebug debug, const medDataIndex& index)
{
    debug.nospace() << "(" << index.m_dataSourceId << ", " << index.m_patientId << ", " << index.m_studyId << ", " << index.m_seriesId << ", " << index.m_uriAsList << ")";

    return debug.space();
}

QDebug operator<<(QDebug debug, medDataIndex *index)
{
    if ( index )
    {
        debug << *index;
    }
    else
    {
        debug.nospace() << "(" << "[medDataIndex* null]" << ")";
    }

    return debug.space();
}

/**
 * The less than operator can be used with STL maps.
 *  Ordering is by patientId, then studyId, then seriesId.
 */
bool operator<(const medDataIndex& index1, const medDataIndex& index2)
{
    if (index1.m_uriAsList.isEmpty())
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
    }
    else
    {
        return index1.m_uriAsList < index2.m_uriAsList;
    }
    // if we reach here, either greater or equal, return false
    return false;
}

// /////////////////////////////////////////////////////////////////
// Hash functions
// /////////////////////////////////////////////////////////////////

uint qHash(const medDataIndex &key)
{
   uint hash = 0;
   if (key.uri().isEmpty())
   {
       hash = hash ^ qHash(key.dataSourceId());
       hash = hash ^ qHash(key.patientId());
       hash = hash ^ qHash(key.studyId());
       hash = hash ^ qHash(key.seriesId());
   }
   else
   {
       hash = hash ^ qHash(key.uri());
   }
   return hash;
}
