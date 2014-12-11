/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <dtkCore/dtkSmartPointer.h>

class medAbstractData;
class medDatabaseController;
class medDatabaseReaderPrivate;
class medDataIndex;

class medDatabaseReader : public QObject
{
    Q_OBJECT

public:
    medDatabaseReader(const medDataIndex& index, const medDatabaseController* iDbController);
    ~medDatabaseReader();

    medAbstractData *run();

    QString getFilePath();

    qint64 getDataSize();

protected:
    medAbstractData* readFile(QString filename);
    medAbstractData* readFile(const QStringList filenames );

signals:
    void success(QObject *);
    void failure(QObject *);
    void progressed(int);

private:
    medDatabaseReaderPrivate *d;
};


