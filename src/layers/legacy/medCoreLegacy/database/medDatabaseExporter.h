/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <medCoreLegacyExport.h>
#include <medJobItemL.h>

class medAbstractData;
class medDatabaseExporterPrivate;

class MEDCORELEGACY_EXPORT medDatabaseExporter : public medJobItemL
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
