/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>

#include "itkDataTensorImageWriterPluginExport.h"
#include <itkImageIOBase.h>

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkDataTensorImageWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    itkDataTensorImageWriterBase();
    virtual ~itkDataTensorImageWriterBase();

    virtual QStringList handled() const;

    virtual QStringList supportedFileExtensions() const;

    static QStringList s_handled();

public slots:
    bool write    (const QString& path);
    bool canWrite (const QString& path);

private:
    template <class PixelType>
    bool write(const QString& path, PixelType dummyArgument);

protected:
    itk::ImageIOBase::Pointer io;
};


