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

#include "medSqlExport.h"
#include <QtCore>

#include <medJobItem.h>

class dtkAbstractData;
class medDatabaseExporterPrivate;

class MEDSQL_EXPORT medDatabaseExporter : public medJobItem
{
    Q_OBJECT

public:
     medDatabaseExporter(dtkAbstractData *data, const QString &filename);
    ~medDatabaseExporter(void);
    
    /**
     * @brief Starts the job and writes to the file system. 
     *
     * @param void
     * @todo enrich the signals in the exporter (the writer progress signal is not forwarded to the explorer)
    */
    void run(void);

private:
    medDatabaseExporterPrivate *d;
};

#endif // MEDDATABASEEXPORTER_H
