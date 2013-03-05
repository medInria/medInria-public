/* medDatabaseExporter.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jan 19 13:42:53 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Feb  4 11:36:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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

medDatabaseExporter::~medDatabaseExporter()
{
    delete d;

    d = NULL;
}

void medDatabaseExporter::run()
{
    if (d->data.isNull()) {
        emit showError(this, "Cannot export data", 3000);
        return;
    }

    if (d->filename.isEmpty()) {
        emit showError(this, "File name is empty", 3000);
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
        emit showError(this, tr("Could not find suitable writer for file: ") + d->filename, 3000);
    }
}
