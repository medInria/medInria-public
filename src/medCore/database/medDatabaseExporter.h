/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QtCore>

#include <medAbstractJob.h>

class medAbstractData;
class medDatabaseExporterPrivate;

class MEDCORE_EXPORT medDatabaseExporter : public medAbstractJob
{
    Q_OBJECT

public:
     medDatabaseExporter(medAbstractData * data, const QString & filename, const QString & writer);
    ~medDatabaseExporter();

protected:
    void internalRun();

private:
    medDatabaseExporterPrivate *d;
};


