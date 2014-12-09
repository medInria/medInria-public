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

#include <medImageIOExport.h>

#include <itkImageIOBase.h>

class MEDIMAGEIO_EXPORT medItkImageDataWriterBase: public dtkAbstractDataWriter {
    Q_OBJECT

public:

    medItkImageDataWriterBase();
    virtual ~medItkImageDataWriterBase();

    virtual QStringList handled() const = 0;

    /** Override base class */
    virtual QStringList supportedFileExtensions() const;

public slots:

    bool write    (const QString& path);
    bool canWrite (const QString& path);

protected:

    itk::ImageIOBase::Pointer io;

private:

    template <unsigned DIM,typename T>
    bool write_image(const QString& path,const char* type);
};


