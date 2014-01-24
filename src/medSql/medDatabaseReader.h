/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <dtkCore/dtkSmartPointer.h>

class medAbstractData;
class medDatabaseReaderPrivate;
class medDataIndex;

class medDatabaseReader : public QObject
{
    Q_OBJECT

public:
    medDatabaseReader(const medDataIndex& index);
    ~medDatabaseReader();

    dtkSmartPointer<medAbstractData> run();

    QString getFilePath();

    qint64 getDataSize();

protected:
    dtkSmartPointer<medAbstractData> readFile(QString filename);
    dtkSmartPointer<medAbstractData> readFile(const QStringList filenames );

signals:
    void success(QObject *);
    void failure(QObject *);
    void progressed(int);

private:
    medDatabaseReaderPrivate *d;
};


