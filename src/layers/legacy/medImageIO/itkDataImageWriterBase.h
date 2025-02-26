#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataWriter.h>

#include <medImageIOExport.h>

#include <itkImageIOBase.h>

class MEDIMAGEIO_EXPORT itkDataImageWriterBase: public medAbstractDataWriter
{
    Q_OBJECT

public:

    itkDataImageWriterBase();
    virtual ~itkDataImageWriterBase();

    virtual QStringList handled() const = 0;

    /** Override base class */
    virtual QStringList supportedFileExtensions() const;

public slots:

    bool write    (const QString& path);
    bool canWrite (const QString& path);

protected:

    itk::ImageIOBase::Pointer io;
    QHash<QString, std::string> medKeyToItkKey;

    void encapsulateSharedMetaData(itk::MetaDataDictionary& dict);
    std::string convertMedKeyToItkKey(QString medKey);

private:

    template <unsigned DIM,typename T>
    bool write_image(const QString& path,const char* type);

};
