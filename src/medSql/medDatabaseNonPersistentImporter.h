/* medDatabaseNonPersitentImporter.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:25:31 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:54:34 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include <QtCore/QObject>

#include <medDataIndex.h>
#include <medJobItem.h>


class dtkAbstractData;

class medDatabaseNonPersistentImporterPrivate;

/**
 * @class medDatabaseNonPersistentImporter
 * @author Pierre Fillard
 * @brief Import an instance of a dtkAbstractData into the non-persistent database
 * This class imports an instance of a dtkAbstractData into the non-persistent
 * database. The metaData PatientName, StudyDescription and SeriesDescription
 * must be set, otherwise import will fail.
 */
class medDatabaseNonPersistentImporter : public medJobItem
{
    Q_OBJECT

public:
     medDatabaseNonPersistentImporter(dtkAbstractData *data,
                                      const QString& callerUuid);
    ~medDatabaseNonPersistentImporter();

    void run();

public slots:
    void onCancel(QObject*);

signals:
    void nonPersistentImported(const medDataIndex &, QString);


private:
    medDatabaseNonPersistentImporterPrivate *d;
};



