/* medDatabaseImporter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:41:28 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 15:33:33 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
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
    void progressed(int);
    void success (void);
    void failure (void);
    void showError(QObject*,const QString&,unsigned int timeout);

private:
    medDatabaseImporterPrivate *d;
};

#endif // MEDDATABASEIMPORTER_H
