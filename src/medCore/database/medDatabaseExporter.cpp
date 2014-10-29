/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseExporter.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <medAbstractDataFactory.h>

class medDatabaseExporterPrivate
{
public:
    medAbstractData *data;
    QString          filename;
    QString          writer;
};

medDatabaseExporter::medDatabaseExporter(medAbstractData * data, const QString & filename, const QString & writer) : medAbstractJob(), d(new medDatabaseExporterPrivate)
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

/**
 * @brief Starts the job and writes to the file system.
 *
 * @param void
 * @todo enrich the signals in the exporter (the writer progress signal is not forwarded to the explorer)
*/
void medDatabaseExporter::internalRun(void)
{
    if (!d->data)
    {
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
        emit failure();
    } else {
        emit success();
    }
    delete dataWriter;
}
