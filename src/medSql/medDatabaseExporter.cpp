/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseExporter.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

class medDatabaseExporterPrivate
{
public:
    dtkSmartPointer<dtkAbstractData> data;
    QString          filename;
};

medDatabaseExporter::medDatabaseExporter(dtkAbstractData *data, const QString &filename) : medJobItem(), d(new medDatabaseExporterPrivate)
{
    d->data     = data;
    d->filename = filename;
}

medDatabaseExporter::~medDatabaseExporter(void)
{
    delete d;

    d = NULL;
}

void medDatabaseExporter::run(void)
{
    if (d->data.isNull()) {
        emit showError("Cannot export data", 3000);
        return;
    }

    if (d->filename.isEmpty()) {
        emit showError("File name is empty", 3000);
        return;
    }

    QList<QString> writers = dtkAbstractDataFactory::instance()->writers();
    bool written = false;
    for (int i=0; i<writers.size(); i++)
    {
        dtkSmartPointer<dtkAbstractDataWriter> dataWriter = dtkAbstractDataFactory::instance()->writerSmartPointer(writers[i]);

        if (! dataWriter->handled().contains(d->data->identifier()))
            continue;

        dataWriter->setData (d->data);

        if (dataWriter->canWrite( d->filename )) {
            if (dataWriter->write( d->filename )) {
                written = true;
                break;
            }
        }
    }

    if (!written) {
        emit showError(tr("Could not find suitable writer for file: ") + d->filename, 3000);
    }
}
