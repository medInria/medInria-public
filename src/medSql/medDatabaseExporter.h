/* medDatabaseExporter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:41:39 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jan 19 13:42:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEEXPORTER_H
#define MEDDATABASEEXPORTER_H

#include <QtCore>

class medDatabaseExporterPrivate;

class medDatabaseExporter : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medDatabaseExporter(void);
    ~medDatabaseExporter(void);

    void run(void);

signals:
    void progressed(int);

private:
    medDatabaseExporterPrivate *d;
};

#endif // MEDDATABASEEXPORTER_H
