/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataWriter.h>

#include <medTtkTensorImageExport.h>
#include <itkImageIOBase.h>

class medTtkTensorImagePlugin_EXPORT medTtkTensorImageDataWriterBase : public dtkAbstractDataWriter
{
    Q_OBJECT

public:
    medTtkTensorImageDataWriterBase();
    virtual ~medTtkTensorImageDataWriterBase();

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


