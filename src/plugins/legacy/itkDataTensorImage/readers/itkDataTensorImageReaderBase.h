#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataReader.h>

#include <itkDataTensorImagePluginExport.h>
#include <itkImageIOBase.h>

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkDataTensorImageReaderBase : public medAbstractDataReader
{
    Q_OBJECT

public:
    itkDataTensorImageReaderBase();
    virtual ~itkDataTensorImageReaderBase();

    virtual QStringList handled() const;

    static QStringList s_handled();

public slots:
    virtual bool canRead (const QString &path);
    virtual bool canRead (const QStringList &paths);
    
    virtual bool readInformation (const QString &path);
    virtual bool readInformation (const QStringList &paths);
    
    virtual bool read (const QString &path);
    virtual bool read (const QStringList &paths);

protected:
    itk::ImageIOBase::Pointer io;
};
