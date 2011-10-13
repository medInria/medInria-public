/* medDatabaseWriter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:25:31 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:32:21 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 12
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEWRITER_H
#define MEDDATABASEWRITER_H

#include <QtCore/QObject>

#include <medJobItem.h>

#include <medDataIndex.h>

class dtkAbstractData;
class medDatabaseWriterPrivate;

/**
 * @class medDatabaseWriter
 * @author Pierre Fillard
 * @brief Stores an instance of a dtkAbstractData to the SQL database.
 * This class stores an instance of a dtkAbstractData to the SQL database.
 * The metadata PatientName / StudyDescription are used to retrieve the
 * patient Id in the database. If none is given, John Doe / EmptyStudy will
 * be used. The SeriesDescription is mandatory and import will fail it not
 * found.
 */
class medDatabaseWriter : public medJobItem
{
    Q_OBJECT

public:
     medDatabaseWriter(dtkAbstractData *data, const QString& callerUuid = QString());
    ~medDatabaseWriter(void);

    void run(void);

public slots:
    void onCancel(QObject*);

signals:
    void addedIndex(const medDataIndex &, const QString &);
    void addedIndex(const medDataIndex &);

private:
    medDatabaseWriterPrivate *d;
};

#endif
