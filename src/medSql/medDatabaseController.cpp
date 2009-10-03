/* medDatabaseController.cpp ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 11:05:39 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sat Oct  3 19:07:21 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 104
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
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#include <medSql/medDatabaseController.h>

bool medDatabaseController::createConnection(void)
{
    this->mkpath(this->dataLocation() + "/");

    this->m_database = QSqlDatabase::addDatabase("QSQLITE");
    this->m_database.setDatabaseName(this->dataLocation() + "/" + "db");

    if (!m_database.open()) {
        qDebug() << DTK_COLOR_FG_RED << "Cannot open database: Unable to establish a database connection." << DTK_NOCOLOR;
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
    .append("/")
    .append(QCoreApplication::applicationName());
#else
    qDebug() <<  QDesktopServices::storageLocation(QDesktopServices::DataLocation);

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

void medDatabaseController::import(const QString& file)
{
    qDebug() << __func__ << file;

    // QDir dir(file); dir.setFilter(QDir::Files | QDir::Hidden);
    
    // dtkAbstractData *data = dtkAbstractDataFactory::instance()->create("dcmtkDataImage");

    // dtkAbstractDataImageDicom *dicom = dynamic_cast<dtkAbstractDataImageDicom *>(data);

    // if(!dicom) {
    //     dtkLog::critical() << "Not able to retreive a dicom data proxy";
    //     return;
    // }
        
    // dicom->read(dir.filePath(dir.entryList().first()).toAscii());

    // QSqlQuery query(*(medDatabaseController::instance()->database())); QVariant id;
    
    // // ///////////////////////////////////////////////////////////////// PATIENT

    // query.prepare("SELECT id FROM patient WHERE name = :name");
    // query.bindValue(":name", dicom->patient());
    // if(!query.exec())
    //     qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    // if(query.first()) {

    // id = query.value(0);
    // qDebug() << "Patient already exists in database" << id;

    // } else {

    // query.prepare("INSERT INTO patient (name) VALUES (:name)");
    // query.bindValue(":name", dicom->patient());
    // query.exec(); id = query.lastInsertId();
    // qDebug() << "Patient inserted" << id;

    // }

    // // ///////////////////////////////////////////////////////////////// STUDY

    // query.prepare("SELECT id FROM study WHERE patient = :id AND name = :name");
    // query.bindValue(":id", id);
    // query.bindValue(":name", dicom->study());
    // if(!query.exec())
    //     qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    // if(query.first()) {

    // id = query.value(0);
    // qDebug() << "Study already exists in database" << id << "for patient" << dicom->patient();

    // } else {

    // query.prepare("INSERT INTO study (patient, name) VALUES (:patient, :study)");
    // query.bindValue(":patient", id);
    // query.bindValue(":study", dicom->study());
    // query.exec(); id = query.lastInsertId();

    // }

    // // ///////////////////////////////////////////////////////////////// SERIES

    // query.prepare("SELECT id FROM series WHERE study = :id AND name = :name");
    // query.bindValue(":id", id);
    // query.bindValue(":name", dicom->series());
    // if(!query.exec())
    //     qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    // if(query.first()) {

    // id = query.value(0);
    // qDebug() << "Study already exists in database" << id << "for patient" << dicom->patient();

    // } else {

    // query.prepare("INSERT INTO series (study, size, name, path) VALUES (:study, :size, :name, :path)");
    // query.bindValue(":study", id);
    // query.bindValue(":size", dir.entryList().count());
    // query.bindValue(":name", dicom->series());
    // query.bindValue(":path", file);
    // query.exec(); id = query.lastInsertId();

    // }

    // // ///////////////////////////////////////////////////////////////// IMAGE

    // foreach(QString file, dir.entryList()) {
        
    // query.prepare("SELECT id FROM image WHERE series = :id AND path = :path");
    // query.bindValue(":id", id);
    // query.bindValue(":name", dir.filePath(file));
    // if(!query.exec())
    //     qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    // if(query.first()) {

    // qDebug() << "Image" << dir.filePath(file) << "already in database";

    // } else {

    // query.prepare("INSERT INTO image (series, size, name, path) VALUES (:series, :size, :name, :path)");
    // query.bindValue(":series", id);
    // query.bindValue(":size", 64);
    // query.bindValue(":name", file);
    // query.bindValue(":path", dir.filePath(file));
    // if(!query.exec())
    //     qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;
    
    // }
    
    // }

    // emit updated();
}

void medDatabaseController::createPatientTable(void)
{
    QSqlQuery query(*(this->database()));
    query.exec(
	"CREATE TABLE patient ("
	" id       INTEGER PRIMARY KEY,"
	" name        TEXT"
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
	" name         TEXT"
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
	" name        TEXT,"
        " path        TEXT"
	");"
    );
}

void medDatabaseController::createImageTable(void)
{
    QSqlQuery query(*(this->database()));
    query.exec(
	"CREATE TABLE image ("
	" id       INTEGER      PRIMARY KEY,"
        " series   INTEGER," // FOREIGN KEY
	" size     INTEGER,"
	" name        TEXT,"
        " path        TEXT,"
        " slice    INTEGER "
	");"
    );
}

medDatabaseController *medDatabaseController::m_instance = NULL;
