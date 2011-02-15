/* medDatabaseController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 11:05:10 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 17:39:21 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASECONTROLLER_H
#define MEDDATABASECONTROLLER_H

#include "medSqlExport.h"

#include <medCore/medDataIndex.h>

#include <QtCore>
#include <QtSql>

class dtkAbstractData;

class MEDSQL_EXPORT medDatabaseController : public QObject
{
    Q_OBJECT

public:
    static QPointer<medDatabaseController> instance(void);
    
    void destroy(void);

    QSqlDatabase *database(void);

    bool createConnection(void);
    bool  closeConnection(void);

    medDataIndex indexForPatient(int id);
    medDataIndex indexForStudy  (int id);
    medDataIndex indexForSeries (int id);
    medDataIndex indexForImage  (int id);

    QStringList patientNames(void);

    dtkAbstractData *read(const QString& file);
    dtkAbstractData *read(const medDataIndex& index);
    dtkAbstractData *read(int patientId, int studyId, int seriesId);
    dtkAbstractData *read(int patientId, int studyId, int seriesId, int imageId);

    /**
    * moveDatabase - Change the database directory location
    * @params: QString newLocation - needs to be a valid directory name
    * @return   bool
    */
    bool moveDatabase(QString newLocation);

    bool isConnected();

signals:
    void updated(void);
    void copyMessage(QString, int, QColor);

public slots:
    void import(const QString& file);

protected:
    medDatabaseController();
    ~medDatabaseController();

    // some helpers
    bool copyFiles(QStringList sourceList, QStringList destList);
    bool createDestination(QStringList sourceList, QStringList& destList, QString sourceDir, QString destDir);
    void recurseAddDir(QDir d, QStringList & list);
    bool removeDir(QString dirName);





private:
    void createPatientTable(void);
    void   createStudyTable(void);
    void  createSeriesTable(void);
    void   createImageTable(void);

private:
    QSqlDatabase m_database;
    static bool         m_isConnected;

private:
    static medDatabaseController *s_instance;
};

#endif
