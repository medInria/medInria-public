/* medDatabaseController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Mar 31 11:05:10 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Sep 25 11:53:11 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASECONTROLLER_H
#define MEDDATABASECONTROLLER_H

#include <QtCore>
#include <QtSql>

class medDatabaseController : public QObject
{
    Q_OBJECT

public:
    static medDatabaseController *instance(void) {
        if(!m_instance)
            m_instance = new medDatabaseController;

        return m_instance;
    }

    QSqlDatabase *database(void) {
        return &m_database;
    }

    bool createConnection(void);
    bool  closeConnection(void);

    bool mkpath(const QString& dirPath);
    bool rmpath(const QString& dirPath);

    QString   dataLocation(void) const;
    QString configLocation(void) const;

signals:
    void updated(void);

public slots:
    void import(const QString& file);

private:
    void createPatientTable(void);
    void   createStudyTable(void);
    void  createSeriesTable(void);
    void   createImageTable(void);

private:
    QSqlDatabase m_database;

private:
    static medDatabaseController *m_instance;
};

#endif
