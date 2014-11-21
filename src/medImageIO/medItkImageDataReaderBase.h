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

#include <medImageIOExport.h>
#include <itkImageIOBase.h>

class MEDIMAGEIO_EXPORT medItkImageDataReaderBase:
        public dtkAbstractDataReader
{
    Q_OBJECT

public:

    medItkImageDataReaderBase();
    virtual ~medItkImageDataReaderBase();

    virtual QStringList handled() const = 0;

public slots:

    virtual bool canRead (const QString& path);
    virtual bool canRead (const QStringList& paths);

    virtual bool readInformation (const QString& path);
    virtual bool readInformation (const QStringList& paths);

    virtual bool read (const QString& path);
    virtual bool read (const QStringList& paths);

    virtual void setProgress (int value);

protected:

    itk::ImageIOBase::Pointer io;

private:
    template <unsigned DIM,typename T>
    bool read_image(const QString& path,const char* type);
};


