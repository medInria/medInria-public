/* medDatabaseReader.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:25:31 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:32:21 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 12
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDDATABASEREADER_H
#define MEDDATABASEREADER_H

#include <QtCore/QObject>
#include <dtkCore/dtkSmartPointer.h>

class dtkAbstractData;
class medDatabaseReaderPrivate;
class medDataIndex;

class medDatabaseReader : public QObject
{
    Q_OBJECT

public:
    medDatabaseReader(const medDataIndex& index);
    ~medDatabaseReader();

    dtkSmartPointer<dtkAbstractData> run();

    QString getFilePath();

    qint64 getDataSize();

protected:
    dtkSmartPointer<dtkAbstractData> readFile(QString filename);
    dtkSmartPointer<dtkAbstractData> readFile(const QStringList filenames );

signals:
    void success(QObject *);
    void failure(QObject *);
    void progressed(int);

private:
    medDatabaseReaderPrivate *d;
};

#endif
