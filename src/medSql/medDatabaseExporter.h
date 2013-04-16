/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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
    ~medDatabaseExporter();
    
    /**
     * @brief Starts the job and writes to the file system. 
     *
     * @param void
     * @todo enrich the signals in the exporter (the writer progress signal is not forwarded to the explorer)
    */
    void run();

private:
    medDatabaseExporterPrivate *d;
};


