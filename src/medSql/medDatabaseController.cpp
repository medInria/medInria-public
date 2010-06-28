/* medDatabaseController.cpp ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 11:05:39 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 17:43:20 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 130
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseImporter.h>
#include <medSql/medDatabaseExporter.h>

medDatabaseController *medDatabaseController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseController;

    return s_instance;
}

QSqlDatabase *medDatabaseController::database(void)
{
    return &m_database;
}

bool medDatabaseController::createConnection(void)
{
    this->mkpath(this->dataLocation() + "/");

    this->m_database = QSqlDatabase::addDatabase("QSQLITE");
    this->m_database.setDatabaseName(this->dataLocation() + "/" + "db");

    if (!m_database.open()) {
        qDebug() << DTK_COLOR_FG_RED << "Cannot open database: Unable to establish a database connection." << DTK_NO_COLOR;
        return false;
    }

    createPatientTable();
    createStudyTable();
    createSeriesTable();
    createImageTable();

    return true;
}

bool medDatabaseController::closeConnection(void)
{
    m_database.close();

    return true;
}

bool medDatabaseController::mkpath(const QString& dirPath)
{
    QDir dir; return(dir.mkpath(dirPath));
}

bool medDatabaseController::rmpath(const QString& dirPath)
{
    QDir dir; return(dir.rmpath(dirPath));
}

QString medDatabaseController::dataLocation(void) const
{
#ifdef Q_WS_MAC
    return QString(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
            .remove(QCoreApplication::applicationName())
            .append(QCoreApplication::applicationName());
#else
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
}

QString medDatabaseController::configLocation(void) const
{
#ifdef Q_WS_MAC
    return(QDir::homePath() + "/Library/Preferences/" + "com" + "." + QCoreApplication::organizationName() + "." + QCoreApplication::applicationName() + "." + "plist");
#else
    return(dataLocation());
#endif
}

medDataIndex medDatabaseController::indexForPatient(int id)
{
    return medDataIndex(id);
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

    return medDataIndex(patientId.toInt(), id);
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

    return medDataIndex(patientId.toInt(), studyId.toInt(), id);
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

    return medDataIndex(patientId.toInt(), studyId.toInt(), seriesId.toInt(), id);
}

void medDatabaseController::import(const QString& file)
{
    Q_UNUSED(file);

    emit(updated());
}

dtkAbstractData *medDatabaseController::read(const medDataIndex& index)
{
    QVariant patientId = index.patientId();
    QVariant   studyId = index.studyId();
    QVariant  seriesId = index.seriesId();
    QVariant   imageId = index.imageId();

    QSqlQuery query(m_database);

    QString patientName;
    QString studyName;
    QString seriesName;
    
    query.prepare("SELECT name FROM patient WHERE id = :id");
    query.bindValue(":id", patientId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if (query.first())
        patientName = query.value(0).toString();
    
    query.prepare("SELECT name FROM study WHERE id = :id");
    query.bindValue(":id", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if (query.first())
        studyName = query.value(0).toString();
    
    query.prepare("SELECT name FROM series WHERE id = :id");
    query.bindValue(":id", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
    if (query.first())
        seriesName = query.value(0).toString();
    
    QStringList filenames;
    QString     filename;
    
    query.prepare("SELECT name, id, path, instance_path FROM image WHERE series = :series");
    query.bindValue(":series", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    while(query.next()) {
        filenames << query.value(2).toString();
        filename = query.value(3).toString();
    }

    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;

    dtkAbstractData *data = NULL;

    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();

    for (int i = 0; i < readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);
        
        if (dataReader->canRead(filename)) {
            dataReader->read(filename);
            data = dataReader->data();
            delete dataReader;
            break;
        }
    }
    
    
    if (data) {
        data->addMetaData("PatientName", patientName);
        data->addMetaData("StudyDescription",   studyName);
        data->addMetaData("SeriesDescription",  seriesName);
    }
	
    return data;
}

dtkAbstractData *medDatabaseController::read(int patientId, int studyId, int seriesId)
{
    return read(medDataIndex(patientId, studyId, seriesId));
}

dtkAbstractData *medDatabaseController::read(int patientId, int studyId, int seriesId, int imageId)
{
    return read(medDataIndex(patientId, studyId, seriesId, imageId));
}

void medDatabaseController::createPatientTable(void)
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

void medDatabaseController::createStudyTable(void)
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

void medDatabaseController::createSeriesTable(void)
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

void medDatabaseController::createImageTable(void)
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

medDatabaseController *medDatabaseController::s_instance = NULL;
