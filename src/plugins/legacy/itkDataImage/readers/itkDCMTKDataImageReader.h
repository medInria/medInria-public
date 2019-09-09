/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractDataReader.h>
#include <itkDataImagePluginExport.h>
#include <medAbstractData.h>

class itkDCMTKDataImageReaderPrivate;

namespace itk {
    class SliceReadCommand;
}

class ITKDATAIMAGEPLUGIN_EXPORT itkDCMTKDataImageReader: public dtkAbstractDataReader {
    Q_OBJECT

public:
             itkDCMTKDataImageReader();
    virtual ~itkDCMTKDataImageReader();

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

    itkDCMTKDataImageReaderPrivate *d;

    static const char ID[];
};

dtkAbstractDataReader *createItkDCMTKDataImageReader();


