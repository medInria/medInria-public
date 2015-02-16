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
#include <medItkImageExport.h>

class medItkImageDataDcmtkReaderPrivate;

namespace itk {
    class SliceReadCommand;
}

class MEDITKIMAGEPLUGIN_EXPORT medItkImageDataDcmtkReader: public dtkAbstractDataReader {
    Q_OBJECT

public:
             medItkImageDataDcmtkReader();
    virtual ~medItkImageDataDcmtkReader();

    virtual QString identifier()  const;
    virtual QString description() const;

    virtual QStringList handled() const;
    static bool registered();

    friend class itk::SliceReadCommand;
    
public slots:
    bool canRead (const QString& path);
    bool canRead (const QStringList& paths);

    bool readInformation (const QString& path);
    bool readInformation (const QStringList& paths);
    
    bool read (const QString& path);
    bool read (const QStringList& paths);

    void setProgress (int value);
    
private:

    medItkImageDataDcmtkReaderPrivate *d;

    static const char ID[];
};

dtkAbstractDataReader *createmedItkImageDataDcmtkReader();


