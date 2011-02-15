/* medDatabaseController.cpp ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 11:05:39 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:57:34 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 141
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

#include <medCore/medMessageController.h>

#include "medDatabaseController.h"
#include "medDatabaseImporter.h"
#include "medDatabaseExporter.h"
#include "medDatabaseReader.h"
#include <medCore/medStorage.h>

void medDatabaseController::recurseAddDir(QDir d, QStringList & list) 
{

    QStringList qsl = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

    foreach (QString file, qsl) {

        QFileInfo finfo(QString("%1/%2").arg(d.path()).arg(file));

        if (finfo.isSymLink())
            return;

        if (finfo.isDir()) {

            QString dirname = finfo.fileName();
            QDir sd(finfo.filePath());

            recurseAddDir(sd, list);

        } else
            list << QDir::toNativeSeparators(finfo.filePath());
    }
}

bool medDatabaseController::createDestination(QStringList sourceList, QStringList& destList, QString sourceDir, QString destDir)
{
    bool res = true;
    
    if (!QDir(destDir).entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files).isEmpty())
    {
        qDebug() << "Directory not empty: " << destDir;
        return false;
    }

    int trimCount = sourceDir.length();
    foreach (QString sourceFile, sourceList)
    {
        sourceFile.remove(0,trimCount);

        QString destination = destDir + sourceFile;

        // check if this is a directory
        QFileInfo completeFile (destination);
        QDir fileInfo(completeFile.path());

        if (!fileInfo.exists() && !medStorage::mkpath (fileInfo.path())) {
            qDebug() << "Cannot create directory: " << fileInfo.path();
            res = false;
        }

        destList.append(destination);
    }

    return res;
}

bool medDatabaseController::copyFiles(QStringList sourceList, QStringList destList)
{
 
    if (destList.count() != sourceList.count())
        return false;

    // just copy not using a dialog
    for (int i = 0; i < sourceList.count(); i++) 
    {
      
        // coping
        if (!QFile::copy(sourceList.at(i), destList.at(i))) 
        {
            qDebug() << "[Failure] copying file: " << sourceList.at(i) << " to " << destList.at(i);
            return false;
        }       
        else
        {   
            QString message;
            message = tr("copying files ") + QString::number(i) + " of " + QString::number(sourceList.count()); 
            emit copyMessage(message, Qt::AlignBottom, QColor((Qt::white)));
        }
      
    }

    return true;
}


bool medDatabaseController::removeDir(QString dirName)
{

    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                QString message;
                message = tr("removing files ") + info.baseName(); 
                emit copyMessage(message, Qt::AlignBottom, QColor((Qt::white)));
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    if (result)
        emit copyMessage(tr("deleting old database: success"), Qt::AlignBottom, QColor((Qt::white)));
    else
        emit copyMessage(tr("deleting old database: failure"), Qt::AlignBottom, QColor((Qt::red)));
    
    return result;
}

QPointer<medDatabaseController> medDatabaseController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseController;

    return s_instance;
}

void medDatabaseController::destroy(void)
{
    if (s_instance) {
        delete s_instance;
        s_instance = 0;
    }
}

QSqlDatabase *medDatabaseController::database(void)
{
    return &m_database;
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
        m_isConnected = true;
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
    m_isConnected = false;
    return true;
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
    medDatabaseReader *reader = new medDatabaseReader(index);

    connect(reader, SIGNAL(progressed(int)), medMessageController::instance(), SLOT(setProgress(int)));
    connect(reader, SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(success(QObject *)), reader, SLOT(deleteLater()));
    connect(reader, SIGNAL(failure(QObject *)), reader, SLOT(deleteLater()));

    medMessageController::instance()->showProgress(reader, "Opening database item");

    return reader->run();
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

bool medDatabaseController::moveDatabase( QString newLocation)
{
    bool res = true;
    QString oldLocation = medStorage::dataLocation();

    // now copy all the images and thumbnails
    QStringList sourceList;
    recurseAddDir(QDir(oldLocation), sourceList);
    
    // create destination filelist
    QStringList destList;
    if (!createDestination(sourceList,destList,oldLocation, newLocation))
    {
        res = false;
    }
    else
    {
        // now copy
        if (!copyFiles(sourceList, destList))
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
        removeDir(oldLocation);
     
    }

    if (res)
        emit copyMessage(tr("relocating database successful"), Qt::AlignBottom, QColor((Qt::white)));
    else
        emit copyMessage(tr("relocating database failed"), Qt::AlignBottom, QColor((Qt::red)));
    return res;
}

bool medDatabaseController::isConnected()
{
    return m_isConnected;
}

medDatabaseController::medDatabaseController()
{
    m_isConnected = false;
}

medDatabaseController::~medDatabaseController()
{

}


medDatabaseController *medDatabaseController::s_instance = NULL;
bool medDatabaseController::m_isConnected = false;