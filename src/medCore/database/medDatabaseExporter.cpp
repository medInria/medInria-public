/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseExporter.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkSmartPointer.h>

class medDatabaseExporterPrivate
{
public:
    dtkSmartPointer<medAbstractData> data;
    QString          filename;
    QString          writer;
};

medDatabaseExporter::medDatabaseExporter(medAbstractData * data, const QString & filename, const QString & writer) : medJobItem(), d(new medDatabaseExporterPrivate)
{
    d->data     = data;
    d->filename = filename;
    d->writer   = writer;
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

    dtkAbstractDataWriter * dataWriter = medAbstractDataFactory::instance()->writer(d->writer);
    dataWriter->setData(d->data);


    if ( ! dataWriter->canWrite(d->filename) || ! dataWriter->write(d->filename)) {

        emit showError(QString(tr("Writing to file \"%1\" with exporter \"%2\" failed.")).arg(d->filename).arg(dataWriter->description()), 3000);
        emit failure(this);
    } else {
        emit success(this);
    }
    delete dataWriter;
}
