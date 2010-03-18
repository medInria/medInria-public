/* medDatabaseImporter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:41:28 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar 18 21:06:43 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEIMPORTER_H
#define MEDDATABASEIMPORTER_H

#include "medSqlExport.h"

#include <QtCore>

class medDatabaseImporterPrivate;

class MEDSQL_EXPORT medDatabaseImporter : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medDatabaseImporter(const QString& file);
    ~medDatabaseImporter(void);

    void run(void);

signals:
    void done(void);
    void progressed(int);

private:
    medDatabaseImporterPrivate *d;
};

#endif // MEDDATABASEIMPORTER_H
