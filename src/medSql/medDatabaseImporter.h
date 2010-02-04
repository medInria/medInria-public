/* medDatabaseImporter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:41:28 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Jan 20 14:11:05 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 4
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
     medDatabaseImporter(void);
    ~medDatabaseImporter(void);

    void run(void);

signals:
    void progressed(int);

private:
    medDatabaseImporterPrivate *d;
};

#endif // MEDDATABASEIMPORTER_H
