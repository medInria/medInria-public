#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <QtCore/QObject>

#include <medCoreLegacyExport.h>

class medAbstractData;
class medDatabaseReaderPrivate;
class medDataIndex;

class MEDCORELEGACY_EXPORT medDatabaseReader : public QObject
{
    Q_OBJECT

public:
    enum ReadMode
    {
        READ_ALL,
        READ_INFORMATION
    };

    medDatabaseReader(const medDataIndex& index);
    ~medDatabaseReader();

    void setReadMode(ReadMode readMode);
    ReadMode getReadMode() const;

    medAbstractData *run();

protected:
    medAbstractData* readFile(const QStringList& filenames);

signals:
    void success(QObject *);
    void failure(QObject *);
    void progressed(int);

private:
    medDatabaseReaderPrivate *d;
};
