#include "medDatabaseControllerImpl.h"

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#include <medCore/medMessageController.h>
#include <medCore/medStorage.h>
#include <medCore/medJobManager.h>
#include <medCore/medMetaDataHelper.h>

#include "medDatabaseImporter.h"
#include "medDatabaseExporter.h"
#include "medDatabaseReader.h"
#include "medDatabaseRemover.h"
#include "medDatabaseWriter.h"

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
class medDatabaseControllerImplPrivate 
{
public:
    void buildMetaDataLookup();
    bool isConnected;
    SigEmitter* emitter;
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

const QString medDatabaseControllerImplPrivate::T_image = "image";
const QString medDatabaseControllerImplPrivate::T_series = "series";
const QString medDatabaseControllerImplPrivate::T_study = "study";
const QString medDatabaseControllerImplPrivate::T_patient = "patient";

void medDatabaseControllerImplPrivate::buildMetaDataLookup()
{
// The table defines the mapping between metadata in the dtkAbstractData and the database tables.
    metaDataLookup.insert( medMetaDataHelper::KEY_ThumbnailPath(), 
        TableEntryList() << TableEntry(T_image, "thumbnail", true)
        << TableEntry(T_series, "thumbnail", true)
        << TableEntry(T_study, "thumbnail", true)
        << TableEntry(T_patient, "thumbnail", true) );

//Patient data
    metaDataLookup.insert( medMetaDataHelper::KEY_PatientName(), 
        TableEntryList() << TableEntry(T_patient, "name") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Gender(), 
        TableEntryList() << TableEntry(T_patient, "gender") );
    metaDataLookup.insert( medMetaDataHelper::KEY_BirthDate(), 
        TableEntryList() << TableEntry(T_patient, "birthdate") );
//Study Data
    metaDataLookup.insert( medMetaDataHelper::KEY_StudyDescription(), 
        TableEntryList() << TableEntry(T_study, "name") );
    metaDataLookup.insert( medMetaDataHelper::KEY_StudyID(), 
        TableEntryList() << TableEntry(T_study, "uid") );
//Series Data
    metaDataLookup.insert( medMetaDataHelper::KEY_Size(), 
        TableEntryList() << TableEntry(T_series, "size") );
    metaDataLookup.insert( medMetaDataHelper::KEY_SeriesDescription(), 
        TableEntryList() << TableEntry(T_series, "name") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Size(), 
        TableEntryList() << TableEntry(T_series, "size") );
    metaDataLookup.insert( medMetaDataHelper::KEY_SeriesID(), 
        TableEntryList() << TableEntry(T_series, "uid") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Orientation(), 
        TableEntryList() << TableEntry(T_series, "orientation") );
    metaDataLookup.insert( medMetaDataHelper::KEY_SeriesNumber(), 
        TableEntryList() << TableEntry(T_series, "seriesNumber") );
    metaDataLookup.insert( medMetaDataHelper::KEY_SequenceName(), 
        TableEntryList() << TableEntry(T_series, "sequenceName") );
    metaDataLookup.insert( medMetaDataHelper::KEY_SliceThickness(), 
        TableEntryList() << TableEntry(T_series, "sliceThickness") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Rows(), 
        TableEntryList() << TableEntry(T_series, "rows") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Columns(), 
        TableEntryList() << TableEntry(T_series, "columns") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Age(), 
        TableEntryList() << TableEntry(T_series, "age") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Description(), 
        TableEntryList() << TableEntry(T_series, "description") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Modality(), 
        TableEntryList() << TableEntry(T_series, "modality") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Protocol(), 
        TableEntryList() << TableEntry(T_series, "protocol") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Comments(), 
        TableEntryList() << TableEntry(T_series, "comments") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Status(), 
        TableEntryList() << TableEntry(T_series, "status") );
    metaDataLookup.insert( medMetaDataHelper::KEY_AcquisitionDate(), 
        TableEntryList() << TableEntry(T_series, "acquisitiondate") );
    metaDataLookup.insert( medMetaDataHelper::KEY_ImportationDate(), 
        TableEntryList() << TableEntry(T_series, "importationdate") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Referee(), 
        TableEntryList() << TableEntry(T_series, "referee") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Performer(), 
        TableEntryList() << TableEntry(T_series, "performer") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Institution(), 
        TableEntryList() << TableEntry(T_series, "institution") );
    metaDataLookup.insert( medMetaDataHelper::KEY_Report(), 
        TableEntryList() << TableEntry(T_series, "report") );

//Image data


}


QSqlDatabase *medDatabaseControllerImpl::database(void)
{
    return &m_database;
}

bool medDatabaseControllerImpl::createConnection(void)
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

    return true;
}

bool medDatabaseControllerImpl::closeConnection(void)
{
    m_database.close();
    d->isConnected = false;
    return true;
}

medDataIndex medDatabaseControllerImpl::indexForPatient(int id)
{
    return medDataIndex(this->dataSourceId(), id);
}

medDataIndex medDatabaseControllerImpl::indexForPatient (const QString &patientName)
{
    QSqlQuery query(m_database);
    QVariant patientId = -1;

    query.prepare("SELECT id FROM patient WHERE name = :name");
    query.bindValue(":name", patientName);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first()) {
        patientId = query.value(0);
	return medDataIndex (this->dataSourceId(), patientId.toInt());
    }

    return medDataIndex();
}

medDataIndex medDatabaseControllerImpl::indexForStudy(int id)
{
    QSqlQuery query(m_database);

    QVariant patientId = -1;

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        patientId = query.value(0);

    return medDataIndex(this->dataSourceId(), patientId.toInt(), id);
}

medDataIndex medDatabaseControllerImpl::indexForStudy(const QString &patientName, const QString &studyName)
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

medDataIndex medDatabaseControllerImpl::indexForSeries(int id)
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

    return medDataIndex(this->dataSourceId(), patientId.toInt(), studyId.toInt(), id);
}

medDataIndex medDatabaseControllerImpl::indexForSeries(const QString &patientName, const QString &studyName,
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

medDataIndex medDatabaseControllerImpl::indexForImage(int id)
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

medDataIndex medDatabaseControllerImpl::indexForImage(const QString &patientName, const QString &studyName,
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

medDataIndex medDatabaseControllerImpl::import(const QString& file)
{
    Q_UNUSED(file);

    emit(updated(medDataIndex()));

    return medDataIndex();
}

medDataIndex medDatabaseControllerImpl::import( dtkAbstractData *data )
{
    medDatabaseWriter *writer = new medDatabaseWriter(data);

    connect(writer, SIGNAL(progressed(int)),    medMessageController::instance(), SLOT(setProgress(int)));
    connect(writer, SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(writer, SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(writer, SIGNAL(success(QObject *)), writer, SLOT(deleteLater()));
    connect(writer, SIGNAL(failure(QObject *)), writer, SLOT(deleteLater()));

    medMessageController::instance()->showProgress(writer, "Saving database item");

    return writer->run();
}

dtkSmartPointer<dtkAbstractData> medDatabaseControllerImpl::read(const medDataIndex& index) const
{
    QScopedPointer<medDatabaseReader> reader(new medDatabaseReader(index));

    connect(reader.data(), SIGNAL(progressed(int)), medMessageController::instance(), SLOT(setProgress(int)));
    connect(reader.data(), SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader.data(), SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader.data(), SIGNAL(success(QObject *)), reader.data(), SLOT(deleteLater()));
    connect(reader.data(), SIGNAL(failure(QObject *)), reader.data(), SLOT(deleteLater()));

    medMessageController::instance()->showProgress(reader.data(), "Opening database item");

    dtkSmartPointer<dtkAbstractData> data(reader->run());
    return data;
}

dtkSmartPointer<dtkAbstractData> medDatabaseControllerImpl::read(int patientId, int studyId, int seriesId)
{
    return read(medDataIndex(this->dataSourceId(),patientId, studyId, seriesId));
}

dtkSmartPointer<dtkAbstractData> medDatabaseControllerImpl::read(int patientId, int studyId, int seriesId, int imageId)
{
    return read(medDataIndex(this->dataSourceId(),patientId, studyId, seriesId, imageId));
}

void medDatabaseControllerImpl::createPatientTable(void)
{
    QSqlQuery query(*(this->database()));
    query.exec(
            "CREATE TABLE patient ("
            " id       INTEGER PRIMARY KEY,"
            " name        TEXT,"
            " thumbnail   TEXT,"
            " birthdate   TEXT,"
            " gender      TEXT"
            ");"
            );
}

void medDatabaseControllerImpl::createStudyTable(void)
{
    QSqlQuery query(*(this->database()));

    query.exec(
            "CREATE TABLE study ("
            " id        INTEGER      PRIMARY KEY,"
            " patient   INTEGER," // FOREIGN KEY
            " name         TEXT,"
            " uid          TEXT,"
            " thumbnail    TEXT"
            ");"
            );
}

void medDatabaseControllerImpl::createSeriesTable(void)
{
    QSqlQuery query(*(this->database()));
    query.exec(
            "CREATE TABLE series ("
            " id       INTEGER      PRIMARY KEY,"
            " study    INTEGER," // FOREIGN KEY
            " size     INTEGER,"
            " name            TEXT,"
            " path            TEXT,"
            " uid             TEXT,"
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

void medDatabaseControllerImpl::createImageTable(void)
{
    QSqlQuery query(*(this->database()));
    query.exec(
            "CREATE TABLE image ("
            " id         INTEGER      PRIMARY KEY,"
            " series     INTEGER," // FOREIGN KEY
            " size       INTEGER,"
            " name          TEXT,"
            " path          TEXT,"
            " instance_path TEXT,"
            " thumbnail     TEXT,"
            " slice      INTEGER"
            ");"
            );
}

bool medDatabaseControllerImpl::moveDatabase( QString newLocation)
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
        if (!medStorage::copyFiles(sourceList, destList, d->emitter))
            res = false;
    }

    if (res)
        emit copyMessage(tr("copying database: success"), Qt::AlignBottom, QColor((Qt::white)));
    else
        emit copyMessage(tr("copying database: failure"), Qt::AlignBottom, QColor((Qt::red)));


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
        if(medStorage::removeDir(oldLocation, d->emitter))
            emit copyMessage(tr("deleting old database: success"), Qt::AlignBottom, QColor((Qt::white)));
        else
            emit copyMessage(tr("deleting old database: failure"), Qt::AlignBottom, QColor((Qt::red)));

    }

    if (res)
        emit copyMessage(tr("relocating database successful"), Qt::AlignBottom, QColor((Qt::white)));
    else
        emit copyMessage(tr("relocating database failed"), Qt::AlignBottom, QColor((Qt::red)));
    return res;
}

bool medDatabaseControllerImpl::isConnected()
{
    return d->isConnected;
}

medDatabaseControllerImpl::medDatabaseControllerImpl() : d(new medDatabaseControllerImplPrivate)
{
    d->buildMetaDataLookup();
    d->isConnected = false;
    d->emitter = new SigEmitter();
    connect(d->emitter, SIGNAL(message(QString)), this, SLOT(forwardMessage(QString)));
}

medDatabaseControllerImpl::~medDatabaseControllerImpl()
{
    delete d->emitter;
    delete d;
}

void medDatabaseControllerImpl::forwardMessage( QString msg)
{
    copyMessage(msg, Qt::AlignBottom, QColor(Qt::white));
}

qint64 medDatabaseControllerImpl::getEstimatedSize( const medDataIndex& index ) const
{
    QScopedPointer<medDatabaseReader> reader(new medDatabaseReader(index));
    uint size = reader->getDataSize();
    return size + (size/100 * 3); // add 3% margin
}

void medDatabaseControllerImpl::remove( const medDataIndex& index )
{
    medDatabaseRemover *remover = new medDatabaseRemover(index);

    connect(remover, SIGNAL(progress(int)),    medMessageController::instance(), SLOT(setProgress(int)));
    connect(remover, SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(remover, SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(remover, SIGNAL(success(QObject *)), remover, SLOT(deleteLater()));
    connect(remover, SIGNAL(failure(QObject *)), remover, SLOT(deleteLater()));
    connect(remover, SIGNAL(removed(const medDataIndex &)), this, SIGNAL(updated(const medDataIndex &)));

    medMessageController::instance()->showProgress(remover, "Removing item");

    medJobManager::instance()->registerJobItem(remover);
    QThreadPool::globalInstance()->start(remover);
}

QString medDatabaseControllerImpl::metaData( const medDataIndex& index, const QString &key ) const
{
    typedef medDatabaseControllerImplPrivate::MetaDataMap MetaDataMap;
    typedef medDatabaseControllerImplPrivate::TableEntryList TableEntryList;

    const QSqlDatabase & db (*((const_cast<medDatabaseControllerImpl*>(this)->database())));
    QSqlQuery query(db);

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

bool medDatabaseControllerImpl::setMetaData( const medDataIndex& index, const QString& key, const QString& value )
{
    typedef medDatabaseControllerImplPrivate::MetaDataMap MetaDataMap;
    typedef medDatabaseControllerImplPrivate::TableEntryList TableEntryList;

    QSqlDatabase & db (*(this->database()));
    QSqlQuery query(db);

    // Attempt to translate the desired metadata into a table / column entry.
    MetaDataMap::const_iterator it(d->metaDataLookup.find(key));
    if (it == d->metaDataLookup.end() ) {
        return false;
    } 

    bool success (false);

    bool isPath = false;
    for ( TableEntryList::const_iterator entryIt(it.value().begin() ); entryIt != it.value().end(); ++entryIt ) {

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

int medDatabaseControllerImpl::dataSourceId() const
{
    return 1;
}

QList<int> medDatabaseControllerImpl::patients() const
{
    QList<int> ret;
    QSqlQuery query(*(const_cast<medDatabaseControllerImpl*>(this)->database()));
    query.prepare("SELECT id FROM patient");
    EXEC_QUERY(query);
    ret.reserve( query.size() );
    while( query.next() ){
        ret.push_back( query.value(0).toInt());
    }
    return ret;
}

QList<int> medDatabaseControllerImpl::studies( int patientId ) const
{
    QList<int> ret;
    QSqlQuery query(*(const_cast<medDatabaseControllerImpl*>(this)->database()));
    query.prepare("SELECT id FROM study WHERE patient = :patientId");
    query.bindValue(":patientId", patientId);
    EXEC_QUERY(query);
    ret.reserve( query.size() );
    while( query.next() ){
        ret.push_back( query.value(0).toInt());
    }
    return ret;
}

QList<int> medDatabaseControllerImpl::series( int patientId, int studyId ) const
{
    QList<int> ret;
    QSqlQuery query(*(const_cast<medDatabaseControllerImpl*>(this)->database()));
    query.prepare("SELECT id FROM series WHERE study = :studyId");
    query.bindValue(":studyId", studyId);
    EXEC_QUERY(query);
    ret.reserve( query.size() );
    while( query.next() ){
        ret.push_back( query.value(0).toInt());
    }
    return ret;
}

QList<int> medDatabaseControllerImpl::images( int patientId, int studyId, int seriesId ) const
{
    QList<int> ret;
    QSqlQuery query(*(const_cast<medDatabaseControllerImpl*>(this)->database()));
    query.prepare("SELECT id FROM image WHERE series = :seriesId");
    query.bindValue(":seriesId", seriesId);
    EXEC_QUERY(query);
    ret.reserve( query.size() );
    while( query.next() ){
        ret.push_back( query.value(0).toInt());
    }
    return ret;
}

QImage medDatabaseControllerImpl::thumbnail( const medDataIndex& index ) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return QImage();
}

bool medDatabaseControllerImpl::isPersistent( const medDataIndex& index ) const
{
    return true;
}

QString medDatabaseControllerImpl::stringForPath( const QString & name ) const
{
    QString ret = name.simplified();
    ret.replace(0x00EA, 'e');
    ret.replace (0x00E4, 'a');
    return ret;
}


