/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseController.h>

#include <QtCore>
#include <QtGui>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

#include <medMessageController.h>
#include <medStorage.h>
#include <medJobManager.h>
#include <medMetaDataKeys.h>

#include <medDataManager.h>

#include <medDatabaseImporter.h>
#include <medDatabaseExporter.h>
#include <medDatabaseReader.h>
#include <medDatabaseRemover.h>

#define EXEC_QUERY(q) execQuery(q, __FILE__ , __LINE__ )
namespace {
    inline bool execQuery( QSqlQuery & query, const char *file, int line ) {
        if ( ! query.exec() ) {
            qDebug() << file << "(" << line << ") :" << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
            return false;
        }
        return true;
    }
}
class medDatabaseControllerPrivate
{
public:
    void buildMetaDataLookup();
    bool isConnected;
    struct TableEntry {
        TableEntry( QString t, QString c, bool isPath_ = false ) : table(t), column(c), isPath(isPath_) {}
        QString table;
        QString column;
        bool isPath;
    };
    typedef QList<TableEntry> TableEntryList;
    typedef QHash< QString , TableEntryList > MetaDataMap;

    MetaDataMap metaDataLookup;
    // Reusable table names.
    static const QString T_image ;
    static const QString T_series ;
    static const QString T_study ;
    static const QString T_patient ;
};

const QString medDatabaseControllerPrivate::T_image = "image";
const QString medDatabaseControllerPrivate::T_series = "series";
const QString medDatabaseControllerPrivate::T_study = "study";
const QString medDatabaseControllerPrivate::T_patient = "patient";

void medDatabaseControllerPrivate::buildMetaDataLookup()
{
// The table defines the mapping between metadata in the medAbstractData and the database tables.
    metaDataLookup.insert(medMetaDataKeys::ThumbnailPath.key(),
        TableEntryList() << TableEntry(T_image, "thumbnail", true)
        << TableEntry(T_series, "thumbnail", true)
        << TableEntry(T_study, "thumbnail", true)
        << TableEntry(T_patient, "thumbnail", true) );

//Patient data
    metaDataLookup.insert(medMetaDataKeys::PatientName.key(),
        TableEntryList() << TableEntry(T_patient, "name") );
    metaDataLookup.insert(medMetaDataKeys::Gender.key(),
        TableEntryList() << TableEntry(T_patient, "gender") );
    metaDataLookup.insert(medMetaDataKeys::BirthDate.key(),
        TableEntryList() << TableEntry(T_patient, "birthdate") );
    metaDataLookup.insert(medMetaDataKeys::PatientID.key(),
        TableEntryList() << TableEntry(T_patient, "patientId") );

//Study Data
    metaDataLookup.insert(medMetaDataKeys::StudyDescription.key(),
        TableEntryList() << TableEntry(T_study, "name") );
    metaDataLookup.insert(medMetaDataKeys::StudyDicomID.key(),
        TableEntryList() << TableEntry(T_study, "uid") );
    metaDataLookup.insert(medMetaDataKeys::StudyID.key(),
        TableEntryList() << TableEntry(T_study, "studyId") );
//Series Data
    metaDataLookup.insert(medMetaDataKeys::Size.key(),
        TableEntryList() << TableEntry(T_series, "size") );
    metaDataLookup.insert(medMetaDataKeys::SeriesDescription.key(),
        TableEntryList() << TableEntry(T_series, "name") );
    metaDataLookup.insert(medMetaDataKeys::Size.key(),
        TableEntryList() << TableEntry(T_series, "size") );
    metaDataLookup.insert(medMetaDataKeys::SeriesDicomID.key(),
        TableEntryList() << TableEntry(T_series, "uid") );
    metaDataLookup.insert(medMetaDataKeys::SeriesID.key(),
        TableEntryList() << TableEntry(T_series, "seriesId") );
    metaDataLookup.insert(medMetaDataKeys::Orientation.key(),
        TableEntryList() << TableEntry(T_series, "orientation") );
    metaDataLookup.insert(medMetaDataKeys::SeriesNumber.key(),
        TableEntryList() << TableEntry(T_series, "seriesNumber") );
    metaDataLookup.insert(medMetaDataKeys::SequenceName.key(),
        TableEntryList() << TableEntry(T_series, "sequenceName") );
    metaDataLookup.insert(medMetaDataKeys::SliceThickness.key(),
        TableEntryList() << TableEntry(T_series, "sliceThickness") );
    metaDataLookup.insert(medMetaDataKeys::Rows.key(),
        TableEntryList() << TableEntry(T_series, "rows") );
    metaDataLookup.insert(medMetaDataKeys::Columns.key(),
        TableEntryList() << TableEntry(T_series, "columns") );
    metaDataLookup.insert(medMetaDataKeys::Age.key(),
        TableEntryList() << TableEntry(T_series, "age") );
    metaDataLookup.insert(medMetaDataKeys::Description.key(),
        TableEntryList() << TableEntry(T_series, "description") );
    metaDataLookup.insert(medMetaDataKeys::Modality.key(),
        TableEntryList() << TableEntry(T_series, "modality") );
    metaDataLookup.insert(medMetaDataKeys::Protocol.key(),
        TableEntryList() << TableEntry(T_series, "protocol") );
    metaDataLookup.insert(medMetaDataKeys::Comments.key(),
        TableEntryList() << TableEntry(T_series, "comments") );
    metaDataLookup.insert(medMetaDataKeys::Status.key(),
        TableEntryList() << TableEntry(T_series, "status") );
    metaDataLookup.insert(medMetaDataKeys::AcquisitionDate.key(),
        TableEntryList() << TableEntry(T_series, "acquisitiondate") );
    metaDataLookup.insert(medMetaDataKeys::ImportationDate.key(),
        TableEntryList() << TableEntry(T_series, "importationdate") );
    metaDataLookup.insert(medMetaDataKeys::Referee.key(),
        TableEntryList() << TableEntry(T_series, "referee") );
    metaDataLookup.insert(medMetaDataKeys::Performer.key(),
        TableEntryList() << TableEntry(T_series, "performer") );
    metaDataLookup.insert(medMetaDataKeys::Institution.key(),
        TableEntryList() << TableEntry(T_series, "institution") );
    metaDataLookup.insert(medMetaDataKeys::Report.key(),
        TableEntryList() << TableEntry(T_series, "report") );

//Image data


}

medDatabaseController * medDatabaseController::s_instance = NULL;

medDatabaseController* medDatabaseController::instance() {
    if ( ! s_instance) {
        s_instance = new medDatabaseController();
    }
    return s_instance;
}


const QSqlDatabase& medDatabaseController::database(void) const
{
    return m_database;
}

bool medDatabaseController::createConnection(void)
{
    medStorage::mkpath(medStorage::dataLocation() + "/");

    if (this->m_database.databaseName().isEmpty())
        this->m_database = QSqlDatabase::addDatabase("QSQLITE");
    this->m_database.setDatabaseName(medStorage::dataLocation() + "/" + "db");

    if (!m_database.open()) {
        qDebug() << DTK_COLOR_FG_RED << "Cannot open database: Unable to establish a database connection." << DTK_NO_COLOR;
        return false;
    }
    else
    {
        qDebug() << "Database opened at: " << m_database.databaseName();
        d->isConnected = true;
    }

    createPatientTable();
    createStudyTable();
    createSeriesTable();
    createImageTable();

    // optimize speed of sqlite db
    QSqlQuery query(m_database);
    if ( !query.exec( QLatin1String( "PRAGMA synchronous = 0" ) ) ) {
        qDebug() << "Could not set sqlite synchronous mode to asynchronous mode.";
    }
    if ( !query.exec( QLatin1String( "PRAGMA journal_mode=wal" ) ) ) {
        qDebug() << "Could not set sqlite write-ahead-log journal mode";
    }


    return true;
}

bool medDatabaseController::closeConnection(void)
{
    m_database.close();
    QSqlDatabase::removeDatabase("QSQLITE");
    d->isConnected = false;
    return true;
}

/** create dataIndices out of partial ids */
medDataIndex medDatabaseController::indexForPatient(int id)
{
    return medDataIndex::makePatientIndex(this->dataSourceId(), id);
}

/**
* Returns the index of a data given patient, study, series and image name
*/
medDataIndex medDatabaseController::indexForPatient (const QString &patientName)
{
    QSqlQuery query(m_database);
    QVariant patientId = -1;

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        patientId = query.value(0);
        return medDataIndex::makePatientIndex(this->dataSourceId(), patientId.toInt());
    }

    return medDataIndex();
}

medDataIndex medDatabaseController::indexForStudy(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        patientId = query.value(0);

    return medDataIndex::makeStudyIndex(this->dataSourceId(), patientId.toInt(), id);
}

medDataIndex medDatabaseController::indexForStudy(const QString &patientName, const QString &studyName)
{
    medDataIndex index = this->indexForPatient(patientName);
    if (!index.isValid())
        return index;

    QSqlQuery query(m_database);

    QVariant patientId = index.patientId();
    QVariant studyId   = -1;

    query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name");
    query.bindValue(":id",   patientId);
    query.bindValue(":name", studyName);

    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        studyId = query.value(0);
        index.setStudyId(studyId.toInt());
        return index;
    }

    return medDataIndex();
}

medDataIndex medDatabaseController::indexForSeries(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;
    QVariant   studyId = -1;

    query.prepare("SELECT study FROM series WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        studyId = query.value(0);

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        patientId = query.value(0);

    return medDataIndex::makeSeriesIndex(this->dataSourceId(), patientId.toInt(), studyId.toInt(), id);
}

medDataIndex medDatabaseController::indexForSeries(const QString &patientName, const QString &studyName,
                                                       const QString &seriesName)
{
    medDataIndex index = this->indexForStudy(patientName, studyName);
    if (!index.isValid())
        return index;

    QSqlQuery query(m_database);

    QVariant studyId   = index.studyId();

    query.prepare("SELECT id FROM series WHERE study = :id AND name = :name");
    query.bindValue(":id",   studyId);
    query.bindValue(":name", seriesName);

    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        QVariant seriesId = query.value(0);
        index.setSeriesId(seriesId.toInt());
        return index;
    }

    return medDataIndex();
}

medDataIndex medDatabaseController::indexForImage(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;
    QVariant   studyId = -1;
    QVariant  seriesId = -1;

    query.prepare("SELECT series FROM image WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        seriesId = query.value(0);

    query.prepare("SELECT study FROM series WHERE id = :id");
    query.bindValue(":id", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        studyId = query.value(0);

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        patientId = query.value(0);

    return medDataIndex(this->dataSourceId(),patientId.toInt(), studyId.toInt(), seriesId.toInt(), id);
}

medDataIndex medDatabaseController::indexForImage(const QString &patientName, const QString &studyName,
                                                  const QString &seriesName,  const QString &imageName)
{
    medDataIndex index = this->indexForSeries(patientName, studyName, seriesName);
    if (!index.isValid())
        return index;

    QSqlQuery query(m_database);

    QVariant seriesId = index.seriesId();

    query.prepare("SELECT id FROM image WHERE series = :id AND name = :name");
    query.bindValue(":id",   seriesId);
    query.bindValue(":name", imageName);

    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        QVariant imageId = query.value(0);
        index.setImageId(imageId .toInt());
        return index;
    }

    return medDataIndex();
}

/**
* Import data into the db read from file
* @param const QString & file The file containing the data
* @param bool indexWithoutCopying true if the file must only be indexed by its current path,
* false if the file will be imported (copied or converted to the internal storage format)
*/
void medDatabaseController::importPath(const QString& file, const QUuid &importUuid, bool indexWithoutCopying)
{
    QFileInfo info(file);
    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath(),importUuid, indexWithoutCopying);
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));
    importer->start();
}

/**
* Import data into the db read from memory
* @param medAbstractData * data dataObject
*/
void medDatabaseController::importData( medAbstractData *data, const QUuid & importUuid)
{    
    medDatabaseImporter *importer = new medDatabaseImporter(data, importUuid);
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));
    importer->start();
}

void medDatabaseController::showOpeningError(QObject *sender)
{
    medMessageController::instance()->showError("Opening item failed.", 3000);
}

void medDatabaseController::createPatientTable(void)
{
    QSqlQuery query(this->database());
    query.exec(
            "CREATE TABLE patient ("
            " id       INTEGER PRIMARY KEY,"
            " name        TEXT,"
            " thumbnail   TEXT,"
            " birthdate   TEXT,"
            " gender      TEXT,"
            " patientId   TEXT"
            ");"
            );
}

void medDatabaseController::createStudyTable(void)
{
    QSqlQuery query(this->database());

    query.exec(
            "CREATE TABLE study ("
            " id        INTEGER      PRIMARY KEY,"
            " patient   INTEGER," // FOREIGN KEY
            " name         TEXT,"
            " uid          TEXT,"
            " thumbnail    TEXT,"
            " studyId      TEXT"
            ");"
            );
}

void medDatabaseController::createSeriesTable(void)
{
    QSqlQuery query(this->database());
    query.exec(
            "CREATE TABLE series ("
            " id       INTEGER      PRIMARY KEY,"
            " study    INTEGER," // FOREIGN KEY
            " size     INTEGER,"
            " name            TEXT,"
            " path            TEXT,"
            " uid             TEXT,"
            " seriesId        TEXT,"
            " orientation     TEXT,"
            " seriesNumber    TEXT,"
            " sequenceName    TEXT,"
            " sliceThickness  TEXT,"
            " rows            TEXT,"
            " columns         TEXT,"
            " thumbnail       TEXT,"
            " age             TEXT,"
            " description     TEXT,"
            " modality        TEXT,"
            " protocol        TEXT,"
            " comments        TEXT,"
            " status          TEXT,"
            " acquisitiondate TEXT,"
            " importationdate TEXT,"
            " referee         TEXT,"
            " performer       TEXT,"
            " institution     TEXT,"
            " report          TEXT"
            ");"
            );
}

void medDatabaseController::createImageTable(void)
{
    // Note to the reader who came here looking for the 'size' column:
    // it was removed because it was always filled with a
    // placeholder (number 64), and it was never read.

    QSqlQuery query(this->database());
    query.exec(
            "CREATE TABLE image ("
            " id         INTEGER      PRIMARY KEY,"
            " series     INTEGER," // FOREIGN KEY
            " name          TEXT,"
            " path          TEXT,"
            " instance_path TEXT,"
            " thumbnail     TEXT,"
            " slice      INTEGER,"
            " isIndexed  BOOLEAN"
            ");"
            );
}

/**
* Change the storage location of the database by copy, verify, delete
* @param QString newLocation path of new storage location, must be empty
* @return bool true on success
*/
bool medDatabaseController::moveDatabase( QString newLocation)
{
    bool res = true;

    QString oldLocation = medStorage::dataLocation();

    // now copy all the images and thumbnails
    QStringList sourceList;
    medStorage::recurseAddDir(QDir(oldLocation), sourceList);

    // create destination filelist
    QStringList destList;
    if (!medStorage::createDestination(sourceList,destList,oldLocation, newLocation))
    {
        res = false;
    }
    else
    {
        // now copy
        if (!medStorage::copyFiles(sourceList, destList))
            res = false;
    }

    if (res)
        qDebug() << "copying database: success";
    else
        qDebug() << "copying database: failure";


    // only switch to the new location if copying succeeded
    if( res )
    {
        // close connection if necessary
        bool needsRestart = false;
        if (this->isConnected())
        {
            this->closeConnection();
            needsRestart = true;
        }

        // now update the datastorage path and make sure to reconnect
        medStorage::setDataLocation(newLocation);

        // restart if necessary
        if (needsRestart)
        {
            qDebug() << "Restarting connection...";
            this->createConnection();
        }

        // now delete the old archive
        if(medStorage::removeDir(oldLocation))
            qDebug() << "deleting old database: success";
        else
            qDebug() << "deleting old database: failure";

    }

    if (res)
        qDebug() << "relocating database successful";
    else
        qDebug() << "relocating database failed";
    return res;
}

/**
* Status of connection
* @return bool true on success
*/
bool medDatabaseController::isConnected() const
{
    return d->isConnected;
}

medDatabaseController::medDatabaseController() : d(new medDatabaseControllerPrivate)
{
    d->buildMetaDataLookup();
    d->isConnected = false;
}

medDatabaseController::~medDatabaseController()
{
    delete d;
}

/** override base class */
void medDatabaseController::remove( const medDataIndex& index )
{
    medDatabaseRemover *remover = new medDatabaseRemover(index);
    connect(remover, SIGNAL(removed(const medDataIndex &)), this, SIGNAL(dataRemoved(medDataIndex)));
    remover->start();
}

/**
 * Moves study and its series from one patient to another and returns the list of new indexes
 * @param const medDataIndex & indexStudy The data index of the study to be moved
 * @param const medDataIndex & toPatient The data index to move the study to.
 */
QList<medDataIndex> medDatabaseController::moveStudy( const medDataIndex& indexStudy, const medDataIndex& toPatient)
{
    QSqlQuery query(this->database());

    bool result = false;
    QList<medDataIndex> newIndexList;
    medDataIndex newIndex;

    if(indexStudy.isValidForStudy() && toPatient.isValidForPatient())
    {
        query.prepare("UPDATE study SET patient=:patientId WHERE id=:studyId");
        query.bindValue(":patientId", toPatient.patientId());
        query.bindValue(":studyId", indexStudy.studyId());

        result = EXEC_QUERY(query);

        if(result)
        {
            // we need to update patient id in study index
            newIndex = indexStudy;
            newIndex.setPatientId(toPatient.patientId());

            newIndexList << newIndex;
            
            // and update patient id in series indexes
            QList<medDataIndex> seriesIndexList = series(indexStudy);
            foreach(medDataIndex newSerieIndex, seriesIndexList)
            {
                newSerieIndex.setPatientId(toPatient.patientId());
                newIndexList << newSerieIndex;
            }
        }
    }

    if(!newIndexList.isEmpty())
    {
        emit metadataModified(indexStudy, medMetaDataKeys::PatientID.key(), QString::number(toPatient.patientId()));
        emit metadataModified(newIndex);
    }
    return newIndexList;
}

/**
 * Moves serie from one study to another and returns the new index of the serie
 * @param const medDataIndex & indexSerie The data index of the serie to be moved
 * @param const medDataIndex & toStudy The data index to move the serie to.
 */
medDataIndex medDatabaseController::moveSerie( const medDataIndex& indexSerie, const medDataIndex& toStudy)
{
    QSqlQuery query(this->database());

    bool result = false;
    medDataIndex newIndex;

    if(indexSerie.isValidForSeries() && toStudy.isValidForStudy())
    {
        query.prepare("UPDATE series SET study=:studyId  WHERE id=:serieId");
        query.bindValue(":studyId", toStudy.studyId());
        query.bindValue(":serieId", indexSerie.seriesId());

        result = EXEC_QUERY(query);

        if(result)
        {
            newIndex = indexSerie;
            newIndex.setPatientId(toStudy.patientId());
            newIndex.setStudyId(toStudy.studyId());
        }
    }
    
    emit metadataModified(indexSerie, medMetaDataKeys::PatientID.key(), QString::number(toStudy.patientId()));
    emit metadataModified(indexSerie, medMetaDataKeys::StudyID.key(), QString::number(toStudy.studyId()));
    emit metadataModified(newIndex);

    return newIndex;
}

/** Get metadata for specific item. Return uninitialized string if not present. */
QString medDatabaseController::metaData(const medDataIndex& index,const QString& key) const
{
    typedef medDatabaseControllerPrivate::MetaDataMap MetaDataMap;
    typedef medDatabaseControllerPrivate::TableEntryList TableEntryList;

    QSqlQuery query(this->database());

    // Attempt to translate the desired metadata into a table / column entry.
    MetaDataMap::const_iterator it(d->metaDataLookup.find(key));
    if (it == d->metaDataLookup.end() ) {
        return QString();
    }

    QString ret;
    bool isPath = false;
    for ( TableEntryList::const_iterator entryIt(it.value().begin() );
        entryIt != it.value().end(); ++entryIt ) {

        const QString tableName = entryIt->table;
        const QString columnName = entryIt->column;
        isPath = entryIt->isPath;

        int id = -1;
        if ( tableName == d->T_image && index.isValidForImage() ) {
            id = index.imageId();
        } else if ( tableName == d->T_series && index.isValidForSeries() ) {
            id = index.seriesId();
        } else if ( tableName == d->T_study && index.isValidForStudy() ) {
            id = index.studyId();
        } else if ( tableName == d->T_patient && index.isValidForPatient() ) {
            id = index.patientId();
        }
        if ( id != -1 ) {
            query.prepare("SELECT " + columnName + " FROM " + tableName + " WHERE id = :id");
            query.bindValue(":id", id);
            EXEC_QUERY(query);
            if ( query.next() ) {
                ret = query.value(0).toString();
                break;
            }
        }
    }

    if ( ret.isNull() ){
        dtkDebug() << "medDatabaseControllerImpl : Failed to get metadata " << key << " for index " << index.asString();
    }

    if ( !ret.isEmpty() && isPath )
        ret = medStorage::dataLocation() + ret;

    return ret;
}

/** Set metadata for specific item. Return true on success, false otherwise. */
bool medDatabaseController::setMetaData( const medDataIndex& index, const QString& key, const QString& value )
{
    typedef medDatabaseControllerPrivate::MetaDataMap MetaDataMap;
    typedef medDatabaseControllerPrivate::TableEntryList TableEntryList;

    QSqlQuery query(this->database());

    // Attempt to translate the desired metadata into a table / column entry.
    MetaDataMap::const_iterator it(d->metaDataLookup.find(key));
    if (it == d->metaDataLookup.end() ) {
        return false;
    }

    bool success (false);

    for ( TableEntryList::const_iterator entryIt(it.value().begin() ); entryIt != it.value().end(); ++entryIt ) {

        const QString tableName = entryIt->table;
        const QString columnName = entryIt->column;

        int id = -1;
        if ( tableName == d->T_image && index.isValidForImage() ) {
            id = index.imageId();
        } else if ( tableName == d->T_series && index.isValidForSeries() ) {
            id = index.seriesId();
        } else if ( tableName == d->T_study && index.isValidForStudy() ) {
            id = index.studyId();
        } else if ( tableName == d->T_patient && index.isValidForPatient() ) {
            id = index.patientId();
        }
        if ( id != -1 ) {
            query.prepare(QString("UPDATE %1 SET %2 = :value WHERE id = :id")
                .arg(tableName).arg(columnName) );
            query.bindValue(":value", value);
            query.bindValue(":id", id);
            success = EXEC_QUERY(query);
            if ( success )
                break;
        }
    }
    // emit ?
    return success;
}

/** Implement base class */
int medDatabaseController::dataSourceId() const
{
    return 1;
}

/** Enumerate all patients stored in this DB*/
QList<medDataIndex> medDatabaseController::patients() const
{
    QList<medDataIndex> ret;
    QSqlQuery query(this->database());
    query.prepare("SELECT id FROM patient");
    EXEC_QUERY(query);
#if QT_VERSION > 0x0406FF
    ret.reserve( query.size() );
#endif
    while( query.next() ){
        ret.push_back( medDataIndex::makePatientIndex(this->dataSourceId(), query.value(0).toInt()));
    }
    return ret;
}

/** Enumerate all studies for given patient*/
QList<medDataIndex> medDatabaseController::studies( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForPatient() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    QSqlQuery query(this->database());
    query.prepare("SELECT id FROM study WHERE patient = :patientId");
    query.bindValue(":patientId", index.patientId());
    EXEC_QUERY(query);
#if QT_VERSION > 0x0406FF
    ret.reserve( query.size() );
#endif
    while( query.next() ){
        ret.push_back( medDataIndex::makeStudyIndex(this->dataSourceId(), index.patientId(), query.value(0).toInt()));
    }
    return ret;
}

/** Enumerate all series for given patient*/
QList<medDataIndex> medDatabaseController::series( const medDataIndex& index) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForStudy() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    QSqlQuery query(this->database());
    query.prepare("SELECT id FROM series WHERE study = :studyId");
    query.bindValue(":studyId", index.studyId());
    EXEC_QUERY(query);
#if QT_VERSION > 0x0406FF
    ret.reserve( query.size() );
#endif
    while( query.next() ){
        ret.push_back( medDataIndex::makeSeriesIndex(this->dataSourceId(), index.patientId(), index.studyId(), query.value(0).toInt()));
    }
    return ret;
}

/** Enumerate all images for given patient*/
QList<medDataIndex> medDatabaseController::images( const medDataIndex& index) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForSeries() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    QSqlQuery query(this->database());
    query.prepare("SELECT id FROM image WHERE series = :seriesId");
    query.bindValue(":seriesId", index.seriesId());
    EXEC_QUERY(query);
#if QT_VERSION > 0x0406FF
    ret.reserve( query.size() );
#endif
    while( query.next() ){
        ret.push_back( medDataIndex(this->dataSourceId(), index.patientId(), index.studyId(), index.seriesId(), query.value(0).toInt()));
    }
    return ret;
}

QPixmap medDatabaseController::thumbnail(const medDataIndex &index) const
{
    QString thumbpath = this->metaData(index, medMetaDataKeys::ThumbnailPath.key());

    QFileInfo fileInfo(thumbpath);
    if ( fileInfo.exists() ) {
        return QPixmap(thumbpath);
    }
    return QPixmap();
}

/** Implement base class */
bool medDatabaseController::isPersistent(  ) const
{
    return true;
}

/** Remove / replace characters to transform into a pathname component. */
QString medDatabaseController::stringForPath( const QString & name ) const
{
    QString ret = name.simplified();
    ret.replace(0x00EA, 'e');
    ret.replace (0x00E4, 'a');
    return ret;
}

bool medDatabaseController::contains(const medDataIndex &index) const
{
    if (index.isValid() && index.dataSourceId() == dataSourceId())
    {
        //index is valid and comes from this dataSource
        QVariant patientId = index.patientId();
        QVariant studyId = index.studyId();
        QVariant seriesId = index.seriesId();
        QVariant imageId = index.imageId();

        QSqlQuery query(this->database());
        QString fromRequest = "SELECT * FROM patient";
        QString whereRequest = " WHERE patient.id = :id";

        if (studyId != -1)
        {
            fromRequest += " INNER JOIN study ON (patient.id = study.patient)";
            whereRequest += " AND study.id = :stID";
            if (seriesId != -1)
            {
                fromRequest += " INNER JOIN series ON (study.id = series.study)";
                whereRequest +=  " AND series.id = :seID";
                if (imageId != -1)
                {
                    fromRequest += " INNER JOIN image ON (series.id = image.series)";
                    whereRequest +=  " AND image.id = :imID";
                }
            }
        }
        QString request = fromRequest + whereRequest;
        
        query.prepare(request);
        query.bindValue(":id", patientId);
        if (studyId != -1)
            query.bindValue(":stID", studyId);
        if (seriesId != -1)
            query.bindValue(":seID", seriesId);
        if (imageId != -1)
            query.bindValue(":imID", imageId);

        if(!query.exec())
            qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        if(query.first())
            return true;

    }
    return false;
}



medAbstractData* medDatabaseController::retrieve(const medDataIndex &index) const
{
    QScopedPointer<medDatabaseReader> reader(new medDatabaseReader(index));
    medMessageProgress *message = medMessageController::instance()->showProgress("Opening database item");

    connect(reader.data(), SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(reader.data(), SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(reader.data(), SIGNAL(failure(QObject *)), message, SLOT(failure()));

    connect(reader.data(), SIGNAL(failure(QObject *)), this, SLOT(showOpeningError(QObject *)));

    medAbstractData* data;
    data = reader->run();
    return data;
}


void medDatabaseController::removeAll()
{
    qWarning()<< "Attempt to remove all item from PERSISTENT dataBase";
}
