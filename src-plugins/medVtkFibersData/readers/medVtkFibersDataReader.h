/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataReader.h>
#include <medVtkFibersDataPluginExport.h>

class medVtkFibersDataReaderPrivate;

class MEDVTKFIBERSDATAPLUGIN_EXPORT medVtkFibersDataReader: public dtkAbstractDataReader {
    Q_OBJECT

public:
    medVtkFibersDataReader();
    virtual ~medVtkFibersDataReader();

    virtual QStringList handled() const;

public slots:
    virtual bool canRead (const QString& path);
    virtual bool canRead (const QStringList& paths);

    virtual bool readInformation (const QString& path);
    virtual bool readInformation (const QStringList& paths);

    virtual bool read (const QString& path);
    virtual bool read (const QStringList& paths);

    virtual void setProgress (int value);

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();	

private:

    medVtkFibersDataReaderPrivate* d;

    static const char ID[];
};

dtkAbstractDataReader *create_medVtkFibersDataReader();


